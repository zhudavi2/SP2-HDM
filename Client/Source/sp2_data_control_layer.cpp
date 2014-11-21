/**************************************************************
*
* sp2_data_control_layer.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  François Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_fsm_main.h"
#include "../../includes/common_lib/sp2_game_scenario.h"
#include "../include/sp2_unit_attack_confirm_window.h"
#include "../include/sp2_sound.h"
#include "../../includes/sfx/GingerSoundDX/GingerSDX_player.h"
#include "../include/sp2_adaptive_music_player.h"
#include "../include/sp2_region_control_change_window.h"
#include "../include/sp2_economic_window.h"
#include "../include/sp2_economic_health_window.h"
#include "../include/sp2_budget_window.h"
#include "../include/sp2_resources_window.h"

#include "../include/sp2_politic_window.h"
#include "../include/sp2_constitutional_form_window.h"
#include "../include/sp2_internal_laws_window.h"

#include "../include/sp2_military_window.h"

#include "../include/sp2_background_earth.h"
#include "../include/sp2_battle_info_window.h"
#include "../include/sp2_battle_overview_window.h"
#include "../include/sp2_treaties_detailed_window.h"
#include "../include/sp2_objectives_window.h"
#include "../include/sp2_system_bar_window.h"
#include "../include/sp2_trade_detailed_window.h"
#include "../include/sp2_battle_summary_window.h"
#include "../include/sp2_mail_window.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_main_menu.h"
#include "../include/sp2_mini_map.h"
#include "../include/sp2_generic_message_window.h"
#include "../include/sp2_generic_question_window.h"
#include "../include/sp2_host_multiplayer_game.h"
#include "../include/sp2_join_multiplayer_game_window.h"
#include "../include/sp2_progress_meter.h"
#include "../include/sp2_start_single_player_window.h"
#include "../include/sp2_game_lobby_window.h"
#include "../include/sp2_covert_actions_mission_outcome.h"
#include "../include/sp2_covert_actions_mission_outcome_attacked.h"
#include "../include/sp2_covert_actions_window.h"
#include "../include/sp2_game_scenario_window.h"


using namespace SDK::Event;

GDataControlLayer::GDataControlLayer(GDALInterface* in_pDAL) : GDCLInterface(in_pDAL)
, m_ClientAdvisor()
{
   m_sSelectedCountryFlagPath = L"";
   m_iSelectedCountryID       = 0;
   m_bStartTimeSet            = false;
   m_pSubjectThatWillAnswerOccupyTerritoryQuestion = NULL;
   CurrentThematicMap = ECharacteristicToHighlight::Geographic;
   m_iTerritoryToOccupy = 0;
   m_fFiveSecondsClock = 0;

   m_sServerName     = L"SuperPower 2 Server";
   m_sPwd            = L"";
   m_iPort           = c_iServerDefaultPort;
   m_iMaxPlayers     = 8;
   m_iTimeLimit      = 0;
   m_bPrivate        = false;
   m_bDedicated      = false;
   m_iSpecificUnitToMove          = EUnitMovementPermissions::c_iAnyUnitCanMove;
   m_iSpecificDestinationCountry  = EUnitMovementPermissions::c_iUnitCanMoveAnywhere;

   in_pDAL->DataManager()->RegisterNode(this);

   //Auto save information
   m_AutoSaveInformation.m_bAutoSave         = false;
   m_AutoSaveInformation.m_bIncrementalSave  = false;
   m_AutoSaveInformation.m_bNowAutoSaving    = false;
   m_AutoSaveInformation.m_iSaveFrequency    = 15;
   m_AutoSaveInformation.m_sSaveFileName     = "Autosave";
   m_AutoSaveInformation.m_iSaveCount        = 0;

   m_iEscapeKeyId = 0;
   
   Memory::Clear(m_iRefusedMovCount, SP2::ERefusedMoveReason::Count);
}

GDataControlLayer::~GDataControlLayer()
{
}



bool GDataControlLayer::SelectedCountrySet(const SP2::GCountry& in_iCountry,bool in_bForce)
{
   //Dont select again the country if it is already selected.
   if(m_iSelectedCountryID == in_iCountry.Id() && !in_bForce)
   {
      return true;
   }
   m_iSelectedCountryID = (INT32)in_iCountry.Id();

   switch (CurrentThematicMap)
   {
   case ECharacteristicToHighlight::CountryEmpire:
	case ECharacteristicToHighlight::PoliticalControl:
   case ECharacteristicToHighlight::CountryRelations:
      g_ClientDAL.RegionsCharacteristic(g_ClientDCL.CurrentThematicMap, m_iSelectedCountryID);
   }      

   // update the units
   UpdateCurrentCountryUnits();

   g_ClientDDL.MainBar()->Status( g_ClientDAL.DiplomaticStatus( g_ClientDAL.ControlledCountryID(), in_iCountry.Id() ) );

   //Get the Flag path
   if(m_sSelectedCountryFlagPath == L"")
   {
      if(!g_SP2Client->DAL().XMLValue(SP2::FLAG_FOLDER, m_sSelectedCountryFlagPath) )
      {
         m_sSelectedCountryFlagPath = SP2::FLAG_DEFAULT_FOLDER;
      }   
   }

   //Display the flag in the Flag picture box
   g_ClientDDL.SelectedCountryWindowDisplayCountry(m_sSelectedCountryFlagPath + (const GString(in_iCountry.Flag())),in_iCountry.Name());

   g_ClientDAL.FetchCountryRanks(m_iSelectedCountryID);
   g_ClientDAL.FetchCountryInfo(m_iSelectedCountryID);
   g_ClientDAL.m_ResearchInfo.Update();

   if( g_ClientDCL.SelectedCountryID() == g_ClientDAL.ControlledCountryID() )
   {
      WindowsEnabled(true);

      if(g_ClientDDL.BudgetWindow() && g_ClientDDL.BudgetWindow()->Minimized())
         g_ClientDDL.BudgetWindow()->Maximize(); 

      if(g_ClientDDL.PoliticWindow())
         g_ClientDDL.PoliticWindow()->Update();

      if(g_ClientDDL.EconomicWindow())
         g_ClientDDL.EconomicWindow()->Update();

      if(g_ClientDDL.MilitaryWindow())
         g_ClientDDL.MilitaryWindow()->m_pObjConventionalForcesMoreBtn->Enabled(true);

      if(g_ClientDDL.EconomicHealthWindow())
         g_ClientDDL.EconomicHealthWindow()->Update(true);

      if(g_ClientDDL.ResourcesWindow())
         g_ClientDDL.ResourcesWindow()->Update(true);

      if(g_ClientDDL.ConstitutionalFormWindow())
         g_ClientDDL.ConstitutionalFormWindow()->Update(true);

      if(g_ClientDDL.InternalLawsWindow())
         g_ClientDDL.InternalLawsWindow()->Update(true);
   }
   else
   {
      g_ClientDAL.FetchResourcesInfo(g_ClientDCL.SelectedCountryID());
      g_ClientDAL.FetchConstitutionalFormInfo(g_ClientDCL.SelectedCountryID());
      g_ClientDAL.FetchInternalLawsInfo(g_ClientDCL.SelectedCountryID());
      g_ClientDAL.FetchEconomicHealthWindowInfo(g_ClientDCL.SelectedCountryID());
      g_ClientDAL.FetchPoliticWindowInfo(m_iSelectedCountryID);
      g_ClientDAL.FetchEconomicWindowInfo(m_iSelectedCountryID);

      if(g_ClientDDL.BudgetWindow() && !g_ClientDDL.BudgetWindow()->Minimized())
         g_ClientDDL.BudgetWindow()->Minimize();

      if(g_ClientDDL.MilitaryWindow())
         g_ClientDDL.MilitaryWindow()->m_pObjConventionalForcesMoreBtn->Enabled(false);

      WindowsEnabled(false);
   }

   if(g_ClientDDL.MilitaryWindow())
      g_ClientDDL.MilitaryWindow()->Update();

   //Highlight the selected country by setting its regions in a beige coloration using the thread pool
   HighlightCountry((void*)m_iSelectedCountryID);

   //Notify any observers that the selected country did change
   NotifyObserver(c_NotificationOnSelectedCountryChange);

   return true;
}

void GDataControlLayer::UpdateCurrentCountryUnits()
{
   // displays the units on the map
   g_ClientDDL.BackgroundEarth()->HideAllCountryUnits();
   const UINT8* l_pDiplStatus = g_ClientDAL.CountryDiplomaticStatuses(g_ClientDAL.ControlledCountryID() );
   for(INT32 i = 1;i <= g_ClientDAL.NbCountry();i ++)
   {
      if( (l_pDiplStatus[i] != (UINT8) EDiplomaticStatus::Neutral) || (i == m_iSelectedCountryID) )
      {
         g_ClientDDL.BackgroundEarth()->DisplayCountryUnits(i);	
      }
   }
}

void GDataControlLayer::WindowsEnabled(bool in_bEnabled)
{
   if( g_ClientDDL.EconomicWindow() )
      g_ClientDDL.EconomicWindow()->Enabled( in_bEnabled );

   if( g_ClientDDL.EconomicHealthWindow() )
      g_ClientDDL.EconomicHealthWindow()->Enabled( in_bEnabled );

   if( g_ClientDDL.BudgetWindow() )
      g_ClientDDL.BudgetWindow()->Enabled( in_bEnabled );

   if( g_ClientDDL.ResourcesWindow() )
      g_ClientDDL.ResourcesWindow()->Enabled( in_bEnabled );

   if( g_ClientDDL.PoliticWindow() )
      g_ClientDDL.PoliticWindow()->Enabled( in_bEnabled );

   if( g_ClientDDL.ConstitutionalFormWindow() )
      g_ClientDDL.ConstitutionalFormWindow()->Enabled( in_bEnabled );

   if( g_ClientDDL.InternalLawsWindow() )
      g_ClientDDL.InternalLawsWindow()->Enabled( in_bEnabled );
}

bool GDataControlLayer::SelectedCountrySetLookAt(const SP2::GCountry& in_iCountry,bool in_bForce)
{
   //Dont select again the country if it is already selected.
   if( (m_iSelectedCountryID != in_iCountry.Id() ) || in_bForce)
   {
      SelectedCountrySet(in_iCountry, in_bForce);
   }

   //Play the camera animation
   g_ClientDDL.BackgroundEarth()->CreateCameraAnimation();
   g_ClientDDL.BackgroundEarth()->PlayCameraAnimation();      

   return true;
}

INT32 GDataControlLayer::SelectedCountryID()
{
   return m_iSelectedCountryID;
}

bool GDataControlLayer::RequestCountryListToServer()
{
   GGameEventSPtr l_RequestEvent = CREATE_GAME_EVENT(Event::GRequestCountryList);
   l_RequestEvent->m_iSource = g_Joshua.Client()->Id();
   l_RequestEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;

   return g_Joshua.RaiseEvent(l_RequestEvent) == JOSHUA_OK;
}

void GDataControlLayer::ReceiveCountryListFromServer(SP2::Event::GReceiveCountryList* in_pData)
{
   g_ClientDAL.NbCountry( (UINT16) in_pData->m_vCountries.size() );
   g_ClientDAL.Countries() = in_pData->m_vCountries;

   // the countries table is filled, now load the flags textures in memory
   g_ClientDDL.SmallFlagsTexturesCreate();

   // fill the global country listbox
   g_ClientDDL.CountriesListBoxFillUp( g_ClientDAL.Countries() );   

   //Set the Countries into the mainbar window
   if(g_ClientDDL.MainBar())
      ((SP2::GComboListBoxLess*)g_ClientDDL.MainBar()->Child(L"cboCountry"))->SetListBox(g_ClientDDL.CountriesListBox()); 

   //Set the Countries into the single player window
   if(g_ClientDDL.StartSinglePlayerWindow())
      ((SP2::GComboListBoxLess*)g_ClientDDL.StartSinglePlayerWindow()->Child(L"frmInner")->Child(L"frmCountrySelection")->Child(L"cboCountry"))->SetListBox(g_ClientDDL.CountriesListBox()); 

   if(g_ClientDDL.GameScenarioWindow())
      g_ClientDDL.GameScenarioWindow()->OnEndReceivedCountryData();

   // Set the countries into the country list of the lobby
   if(g_ClientDDL.GameLobby())
      g_ClientDDL.GameLobby()->OnEndLoadData();

   if(g_SP2Client->LoadingGame() )
   {
      if(m_CurrentGameData.m_pScenario.get() )
      {
         gassert(m_CurrentGameData.m_iCountryID != 0, "Loading a scenario, but country is 0");

         // Set this scenario as the current one
         SelectScenario(m_CurrentGameData.m_pScenario, 
                        m_CurrentGameData.m_iCountryID,
                        m_CurrentGameData.m_sPlayerName);
      }
      else
      {
         // Send the player information to the server
         g_ClientDCL.SystemSendPlayerInformationToServer(m_CurrentGameData.m_iCountryID,
                                                         m_CurrentGameData.m_sPlayerName,
                                                         g_SP2Client->CurrentGameType() == EGameTypes::SinglePlayer ? SDK::PLAYER_STATUS_READY : SDK::PLAYER_STATUS_IDLE);
      }
   }
}

Map::GMultifaceView* GDataControlLayer::CountryView()
{
   return g_ClientDAL.CountryView();
}

/*!
* Called to start a multiplayer game, switch the game to the start 
* multiplayer screen
**/
bool GDataControlLayer::StartMultiplayerGame()
{
   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::StartMultiplayerBtn);
   return true;
}

/*!
* Called to start a singleplayer game, switch the game to the start 
* singleplayer screen
**/
bool GDataControlLayer::StartSingleplayerGame()
{
   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::StartSinglePlayerBtn);
   return true;
}

bool GDataControlLayer::StartCampaingSinglePlayerGame()
{
   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::ScenarioBtn);
   return true;
}

/*!
* Called to join a multiplayer game, switches the game to the join multiplayer game
* screen
* @return true if successfull, false otherwise
**/
bool GDataControlLayer::JoinMultiplayerGame()
{
   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::JoinMultiplayerBtn);
   return true;
}

/*!
* Connect to the selected 
* @return true if successfull, false otherwise
**/
void GDataControlLayer::Connect(const GString& in_sIP, UINT32 in_iPort, const GString& in_sPassword)
{
	m_sIP = in_sIP;
	m_iPort = in_iPort;
	m_sPwd = in_sPassword;
	m_PublicAndLocalIPs.clear();

	g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::JoinConnectBtn);
}

void GDataControlLayer::Connect(const GString& in_sIP, UINT32 in_iPort, const GString& in_sPassword, const std::vector<std::string>& in_vsIPS)
{
   m_sIP = in_sIP;
   m_iPort = in_iPort;
   m_sPwd = in_sPassword;
   m_PublicAndLocalIPs = in_vsIPS;   

   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::JoinConnectBtn);
}

/*!
* Displays the console, checks if the console is not already displayed or created,
* if not it creates it and then displays it
**/
bool GDataControlLayer::ConsoleDisplayToggle()
{
   //If the console is visible, Hide it
   if(g_ClientDDL.ConsoleWindow() && g_ClientDDL.ConsoleWindow()->Visible())
   {
      g_ClientDDL.ConsoleWindowHide();
      return true;
   }

   //If console window does not exist, create one
   if(!g_ClientDDL.ConsoleWindow())
   {
      g_ClientDDL.ConsoleWindowSpawn();
   }   
   //Display it
   g_ClientDDL.ConsoleWindowShow();
   return true;
}


/*!
* Highlight all the regions of a country with a beige color
**/
bool GDataControlLayer::HighlightCountry(void * in_iCountryID)
{
   //! \todo TBM : This can cause a race condition : use an asynchronous notfication
   if(!g_ClientDDL.BackgroundEarth() )
      return false;

   GColorRGBReal l_Beige(1.0f,1.0f,1.0f,1.0f);
   GColorRGBReal l_Grey(0.3f, 0.3f, 0.3f, 1);

   switch(CurrentThematicMap)
   {
   case ECharacteristicToHighlight::Geographic:
      {
         g_ClientDDL.BackgroundEarth()->ClearAllRegionColors();
         
         GColorRGBReal l_NotPoliticalColor(0.75f, 1.f, 0.75f, 1.0f);
         GColorRGBReal l_NotMilitaryColor(1.f, 0.75f, 0.75f, 1.0f);
         GColorRGBReal l_AnnexedByMeColor(1.f, 1.f, 0.75f, 1.0f);
         GColorRGBReal l_AnnexedByYouColor(0.75f, 1.f, 1.f, 1.0f);

         const set<UINT32>& l_vMilitaryControl = g_ClientDAL.CountryMilitaryControl( (UINT32) in_iCountryID);
         const vector<GRegionControl>& l_vRegions = g_ClientDAL.RegionControlArray();
         for(set<UINT32>::const_iterator l_Itr = l_vMilitaryControl.begin(); l_Itr != l_vMilitaryControl.end(); l_Itr++)
         {
            UINT32 l_iRegion = *l_Itr;
            if(l_vRegions[l_iRegion].m_iPolitical == (UINT32) in_iCountryID)
            {
               g_ClientDDL.BackgroundEarth()->SetRegionColor(l_iRegion, l_Beige, l_vRegions[l_iRegion].m_iMilitary == (UINT32) in_iCountryID);
            }
            else if(g_ClientDAL.RegionIsAnnexed(l_iRegion) )
               g_ClientDDL.BackgroundEarth()->SetRegionColor(l_iRegion, l_AnnexedByMeColor);
            else
               g_ClientDDL.BackgroundEarth()->SetRegionColor(l_iRegion, l_NotPoliticalColor);
         }

         const set<UINT32>& l_vPoliticalControl = g_ClientDAL.CountryPoliticalControl( (UINT32) in_iCountryID);
         for(set<UINT32>::const_iterator l_Itr = l_vPoliticalControl.begin(); l_Itr != l_vPoliticalControl.end(); l_Itr++)
         {
            UINT32 l_iRegion = *l_Itr;
            if(l_vRegions[l_iRegion].m_iMilitary != (UINT32) in_iCountryID)
            {
               if(g_ClientDAL.RegionIsAnnexed(l_iRegion) )
                  g_ClientDDL.BackgroundEarth()->SetRegionColor(l_iRegion, l_AnnexedByYouColor);
               else
                  g_ClientDDL.BackgroundEarth()->SetRegionColor(l_iRegion, l_NotMilitaryColor);
            }
         }
      }
      break;
   default:
      for (UINT32 i = 1; i < ThematicMapColorData.size(); i++)
      {
         if (ThematicMapColorData[i].A == 0)
            g_ClientDDL.BackgroundEarth()->ClearRegionColor(i);
         else         
            g_ClientDDL.BackgroundEarth()->SetRegionColor(i, ThematicMapColorData[i]);   

         if(g_ClientDAL.RegionControlArray()[i].m_iPolitical == (UINT32) in_iCountryID)
         {
            switch (CurrentThematicMap)
            {
            case ECharacteristicToHighlight::CountryRelations:
            case ECharacteristicToHighlight::CountryEmpire:
               break;
            default:
               if (ThematicMapColorData[i].A == 0)
                  g_ClientDDL.BackgroundEarth()->SetRegionColor(i, l_Beige, true);   
               else         
                  g_ClientDDL.BackgroundEarth()->SetRegionColor(i, ThematicMapColorData[i] + l_Grey, true);   
            }
         }
      }
   }
   return true;
}

/*!
* Cancel connection
**/
void GDataControlLayer::CancelConnect()
{
   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::CancelConnect);
}

/*!
*Cancel join multiplayer game
*/
void GDataControlLayer::CancelJoinMPGame()
{
   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::JoinMPCancel);
}

/*!
*Cancel host multiplayer game
*/
void GDataControlLayer::CancelHostMPGame()
{
   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::HostMPCancel);
}

//! Select the home country
/*!
* Selects the player controlled country
**/
bool GDataControlLayer::SelectHomeCountry()
{
   if (g_SP2Client->CurrentFSMState() != FSM::EStates::Playing) 
      return false;

   const GCountry& l_Country = g_ClientDAL.Country(g_ClientDAL.ControlledCountryID() );
   g_ClientDCL.SelectedCountrySetLookAt(l_Country);
   return true;
}

bool GDataControlLayer::HandleGetCountryRanks(INT16 in_iEconomicRank, INT16 in_iPoliticRank)
{
   g_ClientDAL.GenericDataHolder().m_iRankedCountryId = (INT16)SelectedCountryID();
   g_ClientDAL.GenericDataHolder().m_iEconomicRank = in_iEconomicRank;
   g_ClientDAL.GenericDataHolder().m_iPoliticRank  = in_iPoliticRank;

   REAL32 l_fMyValue;
   vector<REAL32> l_MilitaryRank;

   for(INT32 i=1; i<=g_ClientDAL.NbCountry(); i++)
   {
      REAL32 l_fResult = 0;

      l_fResult += g_ClientDAL.TotalUnitsValue(i,SP2::EUnitCategory::Infantry);
      l_fResult += g_ClientDAL.TotalUnitsValue(i,SP2::EUnitCategory::Ground);
      l_fResult += g_ClientDAL.TotalUnitsValue(i,SP2::EUnitCategory::Air);
      l_fResult += g_ClientDAL.TotalUnitsValue(i,SP2::EUnitCategory::Naval);

      if(i == g_ClientDCL.SelectedCountryID())
         l_fMyValue = l_fResult;

      l_MilitaryRank.push_back(l_fResult);
   }

   sort(l_MilitaryRank.begin(),l_MilitaryRank.end());
   vector<REAL32>::iterator l_It = find(l_MilitaryRank.begin(),l_MilitaryRank.end(),l_fMyValue);
   g_ClientDAL.GenericDataHolder().m_iMilitaryRank = (INT16)(l_It - l_MilitaryRank.begin());

   g_ClientDDL.MainBarUpdateRank();

   return true;
}

/*!
 * Will limit the unit movement to one unit and/or one country
 */
void GDataControlLayer::LimitMovementTo(UINT32 in_iUnitID, UINT32 in_iCountryID)
{
   m_iSpecificUnitToMove         = in_iUnitID;
   m_iSpecificDestinationCountry = in_iCountryID;
}

bool GDataControlLayer::RequestUnitMove(const vector<UINT32>& in_vUnitID,
                                        const GVector2D<REAL32>& in_Dest,
                                        EMilitaryAction::Flags in_Action)
{
   if(in_vUnitID.empty() )
      return false;

   // Specific check to only move a requested unit
   if(m_iSpecificUnitToMove != EUnitMovementPermissions::c_iAnyUnitCanMove)
   {
      if( (in_vUnitID.size() > 1) || 
          (in_vUnitID[0] != m_iSpecificUnitToMove) )
      {
         return false;
      }
   }

   UINT32 l_iDestRegionID = EarthLocateRegion(in_Dest.x, in_Dest.y);
   UINT32 l_iTargetCountry = g_ClientDAL.RegionControl(l_iDestRegionID).m_iMilitary;

   // Specific check to only move to a requested country
   if( (m_iSpecificDestinationCountry != EUnitMovementPermissions::c_iUnitCanMoveAnywhere) && 
       (l_iTargetCountry != m_iSpecificDestinationCountry) )
   {
      return false;  
   }

   SDK::GGameEventSPtr l_MoveEvent = CREATE_GAME_EVENT(SP2::Event::GRequestTroopsMove);
   l_MoveEvent->m_iSource = g_Joshua.Client()->Id();
   l_MoveEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
   Event::GRequestTroopsMove* l_pTroopMove = (Event::GRequestTroopsMove*) l_MoveEvent.get();
   l_pTroopMove->Destination = in_Dest;
   l_pTroopMove->Action = in_Action;

   // Only move naval units on water
   for(UINT32 i = 0;i < in_vUnitID.size();i ++)
   {
      // Check if unit exists
      SP2::GUnitGroup* l_pGroupToMove = (SP2::GUnitGroup*) g_Joshua.UnitManager().UnitGroup(in_vUnitID[i] );
      if(l_pGroupToMove)
      {
         // Check if we have the right to move the requested unit
         if(l_pGroupToMove->OwnerId() == (UINT32) g_ClientDAL.ControlledCountryID() )
         {
            // Immediately remove unit we know the server will refuse to move
            // i.e. ground units going on water & naval units going on land.
            if( ( (l_iDestRegionID != 0) && (!l_pGroupToMove->IsNaval() ) ) ||
                ( (l_iDestRegionID == 0) && (l_pGroupToMove->IsNaval() ) ) )
            {
               l_pTroopMove->UnitIDs.push_back(in_vUnitID[i] );
            }
            else
            {
               // Movement not possible, inform user
               g_ClientDCL.RefuseMovement(1, 
                  l_pGroupToMove->IsNaval() ? 
                  SP2::ERefusedMoveReason::NoWaterSupport : 
                  SP2::ERefusedMoveReason::NoLandSupport);
            }
         }
      }
   }

   // No need to send an empty move request
   if(!l_pTroopMove->UnitIDs.empty() )
   {
      // Play some sound
      if(l_pTroopMove->Action == EMilitaryAction::Default)
      {
         g_SP2Client->PlaySound(ESoundEventID::OrderMove1);
      }
      else
      {
         g_SP2Client->PlaySound(ESoundEventID::Orders1);
      }

      if(  (l_iTargetCountry != 0) && !(in_Action & EMilitaryAction::Attack) && 
           (g_ClientDAL.DiplomaticStatus(l_iTargetCountry, g_ClientDAL.ControlledCountryID() ) == EDiplomaticStatus::Neutral) )
      {
         // Ask the player if he really wants to attack the target country
         l_pTroopMove->Action |= EMilitaryAction::Attack;

         g_ClientDDL.UnitAttackConfirmWindowSpawn(l_iTargetCountry);
         GUnitAttackConfirmWindow* l_pWindow = g_ClientDDL.UnitAttackConfirmWindow(l_iTargetCountry);
         gassert(l_pWindow, "Attack confirmation window does not exist");
         l_pWindow->AddMoveEvent(l_MoveEvent);
         l_pWindow->Show();
      }
      else
      {
         g_Joshua.RaiseEvent(l_MoveEvent);
      }
   }

   return true;
}

void GDataControlLayer::RefuseMovement(UINT32 in_iUnitCount, 
                                       SP2::ERefusedMoveReason::Enum in_eReason)
{
   gassert(in_eReason < SP2::ERefusedMoveReason::Count, "Invalid movement denial reason");

   // Notify by mail when first denial for unit happens
   if(m_iRefusedMovCount[in_eReason] == 0)
   {
      g_ClientDDL.MailWindow()->AddMail(g_ClientDAL.GetString(EStrId::c_MoveDenial[in_eReason] ), 
                                        ShowMovementRefused, 
                                        (void*) in_eReason);
   }

   // Update movement denail count
   m_iRefusedMovCount[in_eReason] += in_iUnitCount;

   // Play sound that unit was unable to move
   g_SP2Client->PlaySound(ESoundEventID::InvalidAction);
}

bool GDataControlLayer::ChangeUnitStatus(EMilitaryStatus::Enum in_eNewStatus)
{
   for (set<UINT32>::const_iterator itr = g_ClientDAL.SelectedUnitsID().begin(); itr != g_ClientDAL.SelectedUnitsID().end(); itr++)
   {
      UINT32 l_iGroupID = *itr;

      //Check if we have the right to move the requested unit
      if(g_Joshua.UnitManager().UnitGroup(l_iGroupID)->OwnerId() != (UINT32) g_ClientDAL.ControlledCountryID())
      {
         //cannot move a unit that belong to another country
         return false;
      }

      SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GRequestMilitaryStatusChange);
      l_Event->m_iSource = g_Joshua.Client()->Id();
      l_Event->m_iTarget = SDK::Event::ESpecialTargets::Server;
      Event::GRequestMilitaryStatusChange* l_pStatusChange = 
         (Event::GRequestMilitaryStatusChange*) l_Event.get();
      l_pStatusChange->m_eNewStatus = in_eNewStatus;
      l_pStatusChange->m_vGroupIDs.push_back(l_iGroupID);

      g_Joshua.RaiseEvent(l_Event);

      g_SP2Client->PlaySound(ESoundEventID::Orders1);
   }

   return true;
}

void SetRandomTheme(UINT32 in_iSeed)
{
   // Inner continent, make it random between plain and forest
   if(in_iSeed&1)
      g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Plains");
   else
      g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Forest");
}

void SetThemeFromArena(GArenaInfo* in_pArenaInfo)
{

   // If we are on water, just look for ice
   if(in_pArenaInfo->m_iRegionID == 0)
   {
      if(in_pArenaInfo->m_Coordinate.y > 60 || in_pArenaInfo->m_Coordinate.y < -60)
         g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Artic");
      else
         g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Ocean");
   }
   else
   {
      if(in_pArenaInfo->m_Coordinate.y > 60 || in_pArenaInfo->m_Coordinate.y < -60)
         g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Snow");
      else
      {
         bool l_bRegionTouchWater = g_ClientDAL.FindWaterAccess(g_ClientDAL.CountryView()->Multiface(in_pArenaInfo->m_iRegionID));
         EContinent::Enum l_eContinent = g_ClientDAL.RegionContinent(in_pArenaInfo->m_iRegionID) ;
         EGeoGroups::Enum l_eGeoGroup  = g_ClientDAL.RegionGeoGroup(in_pArenaInfo->m_iRegionID);
         switch(l_eContinent)
         {
         case EContinent::Africa :
            {
               if(in_pArenaInfo->m_Coordinate.y > -16)
                  g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Desert");
               else
                  if(l_bRegionTouchWater)
                     g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Tropical");
                  else
                     g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Jungle");
            }
            break;
         case EContinent::Asia :
            {
               // Middle-East
               if(g_ClientDAL.RegionGeoGroup(in_pArenaInfo->m_iRegionID) == EGeoGroups::MiddleEast)
               {
                  if(l_bRegionTouchWater)
                     g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Tropical");
                  else
                     g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Desert");
               }

               else
               {
                  // Desert MGN.
                  if(in_pArenaInfo->m_Coordinate.y < 50 && in_pArenaInfo->m_Coordinate.y > 42 && in_pArenaInfo->m_Coordinate.x > 90 && in_pArenaInfo->m_Coordinate.x < 120)
                     g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Desert");
                  else
                     // Vietnam
                     if(in_pArenaInfo->m_Coordinate.y < 23 && in_pArenaInfo->m_Coordinate.x > 90) 
                        g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Jungle");
                     else
                        SetRandomTheme((UINT32)in_pArenaInfo->m_Coordinate.y);
               }
            }break;
         case EContinent::America :
            {
               if( (in_pArenaInfo->m_iRegionID == 220) ||
						 (in_pArenaInfo->m_iRegionID >= 517 && in_pArenaInfo->m_iRegionID <= 531))
					{
						g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Tropical");
					}
					else if(l_eGeoGroup == EGeoGroups::SouthernAmerica)
               {
                  if(l_bRegionTouchWater)
                     g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Tropical");
                  else
                     g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Jungle");
               }
               else
               {
                  if(in_pArenaInfo->m_Coordinate.y < 34 && in_pArenaInfo->m_Coordinate.y > 30 && in_pArenaInfo->m_Coordinate.x > -115 && in_pArenaInfo->m_Coordinate.x < 100)
                     g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Desert");
                  else
                  {
                     SetRandomTheme((UINT32)in_pArenaInfo->m_Coordinate.y);
                  }
               }
            }
            break;
         case EContinent::Europe:
            {
               SetRandomTheme((UINT32)in_pArenaInfo->m_Coordinate.y);
            }
            break;
         case EContinent::Oceania:
            {
               if(l_bRegionTouchWater)
                  g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Tropical");
               else
                  if(in_pArenaInfo->m_Coordinate.y < -14 && in_pArenaInfo->m_Coordinate.y > -40 && in_pArenaInfo->m_Coordinate.x > 120 && in_pArenaInfo->m_Coordinate.x < 142)
                  {
                     // Desert Australia
                     g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Desert");
                  }
                  else
                  {
                     if(in_pArenaInfo->m_Coordinate.x > 160 && in_pArenaInfo->m_Coordinate.x < -140 )
                        g_ClientDDL.BattleOverviewWindow()->SetSceneFromTheme("Jungle");
                     else
                        SetRandomTheme((UINT32)in_pArenaInfo->m_Coordinate.y);
                  }
            }
            break;
         default :
            gassert(false,"Theme for continent not implemented");
         }
      }
   }
}

/*!
* Display combat overall information
* @param in_pArena: Pointer to the arena info to display
**/
bool GDataControlLayer::DisplayCombatInfo(const Combat::GArenaInfo* in_pArena)
{
   if(!in_pArena)
      return false;


   if(!g_ClientDDL.BattleOverviewWindowSpawn())
      return false;


   SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)in_pArena;
   // Set Theme
   SetThemeFromArena(l_pArenaInfo);
   //Advise the DAL that this combat is the displayed one
   g_ClientDAL.DisplayedOverallCombat(l_pArenaInfo->m_iId);

   g_ClientDDL.BattleOverviewWindow()->SetArena(l_pArenaInfo);

   g_ClientDDL.BattleOverviewWindow()->Show();
   return true;
}

/*!
* Display small combat information
* @param in_pArena: Pointer to the arena info to display
**/
bool GDataControlLayer::DisplayCombatInfoSmall(const Combat::GArenaInfo* in_pArena)
{
   if(!in_pArena)
      return false;

   if(!g_ClientDDL.BattleInfoWindowSpawn())
      return false;

   //Advise the DAL that this combat is the displayed one
   g_ClientDAL.DisplayedInfoCombat(in_pArena->m_iId);

   g_ClientDDL.BattleInfoWindow()->SetArena((SP2::GArenaInfo*)in_pArena);

   g_ClientDDL.BattleInfoWindow()->UpdateContent();

   return true;
}

/*!
* Hides the overall combat info if it is displayed
**/
bool GDataControlLayer::KillCombatOverview()
{
   if(g_ClientDDL.BattleOverviewWindow())
   {
      g_ClientDDL.BattleOverviewWindowKill();
   }

   return true;
}

/*!
* Hides the overall combat info if it is displayed
**/
bool GDataControlLayer::KillCombatInfo()
{
   if(g_ClientDDL.BattleInfoWindow() )
   {
      g_ClientDAL.DisplayedInfoCombat(0);
      g_ClientDDL.BattleInfoWindowKill();
   }

   return true;
}

/*!
* Updates the overall combat information window
**/
REAL64 g_fLAST_OVERALL_COMBAT_INFO_UPDATE = 0;
void GDataControlLayer::UpdateCombatInfo()
{
   gassert(g_SP2Client->CurrentFSMState() == FSM::EStates::Playing, "UpdateCombatInfo should only be called while playing");

   //Don't Update if there is no window to display
   if(!g_ClientDDL.BattleOverviewWindow() && !g_ClientDDL.BattleInfoWindow() )
   {
      return;
   }

   //Update the arena informatoin
   if(g_ClientDDL.BattleOverviewWindow())
      g_ClientDDL.BattleOverviewWindow()->Iterate();

   // Update the battle info
   if(g_ClientDDL.BattleInfoWindow())
      g_ClientDDL.BattleInfoWindow()->UpdateContent();

   //Set the new update time since we updated it
   g_fLAST_OVERALL_COMBAT_INFO_UPDATE = g_Joshua.Clock();
}

bool GDataControlLayer::UpdateWorldPosition(SEarthState & in_State)
{
   g_ClientDDL.BackgroundEarth()->UpdateEarthPosition(in_State);
   g_ClientDDL.MiniMap()->UpdateCrosshair(in_State);

   return true;
}

bool GDataControlLayer::UpdateWorldPosition()
{
   g_ClientDDL.BackgroundEarth()->UpdateEarthPosition(m_EarthState);

   return true;
}

//! Switch to the Playing State
bool GDataControlLayer::StartGame()
{
   g_ClientDAL.GameIsOver(false);
   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::StartGame);
   m_AutoSaveInformation.m_fLastSaveTime = g_Joshua.TimeCounter().GetTime();
   return true;
}

bool GDataControlLayer::SystemRequestAvailablesCountriesToServer()
{
   GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GSendAvailableCountries);
   l_Event->m_iSource = g_SP2Client->Id();
   l_Event->m_iTarget = SDK::Event::ESpecialTargets::Server;
   g_Joshua.RaiseEvent(l_Event);
   return true;
}


bool GDataControlLayer::SystemSendPlayerInformationToServer(UINT32               in_iControlledCountryID,
                                                            const GString&       in_sPlayerName,
                                                            SDK::GEPlayerStatus  in_ePlayerStatus,
                                                            INT32                in_iPartyID)
{
   g_ClientDAL.ControlledCountryID((INT16)in_iControlledCountryID);

   SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GSetPlayerInfo);
   l_Event->m_iSource  = g_Joshua.Client()->Id();
   l_Event->m_iTarget= SDK::Event::ESpecialTargets::Server;

   // Get Event structure
   SP2::Event::GSetPlayerInfo* l_pPlayerInfo = (SP2::Event::GSetPlayerInfo*)l_Event.get();

   // Get Client ID
   l_pPlayerInfo->m_PlayerInfo.ClientID      = g_Joshua.Client()->Id();
   l_pPlayerInfo->m_PlayerInfo.PlayerName    = in_sPlayerName;
   l_pPlayerInfo->m_PlayerInfo.PlayerStatus  = in_ePlayerStatus;
   l_pPlayerInfo->m_PlayerInfo.CountryID     = in_iControlledCountryID;
   l_pPlayerInfo->m_PlayerInfo.PartyID       = in_iPartyID;
   g_Joshua.RaiseEvent(l_Event);
   return true;
}


/*!
 * Called when a packet is received that gives information about a combat that is occuring
 **/
bool GDataControlLayer::HandleCombatInformationUDPPacket(const GCombatInformationPacket& in_Packet)
{
   //If the battle overview window exists and the packet is concerning the actually selected arena
   if( g_ClientDDL.BattleOverviewWindow() && 
      (g_ClientDDL.BattleOverviewWindow()->ArenaId() == (UINT32)in_Packet.m_iArenaID)
     )
   {
      //Handle the packet
      return g_ClientDDL.BattleOverviewWindow()->HandleCombatInformationUDPPacket(in_Packet);
   }
   else
   {
      return true;
   }
}

bool GDataControlLayer::CleanGame()
{
   // Clean all pending questions
   GGenericQuestionWindow::CleanQuestionWindow();

   g_ClientDDL.KillWindowsLvl();
   g_ClientDDL.KillWindowsMilitaryLv2(true);
   g_ClientDDL.KillWindowsPoliticLv2(true);
   g_ClientDDL.KillWindowsEconomicLv2(true); 

   // Clean all message
   if(g_ClientDDL.GenericMessageWindow())
   {
      g_ClientDDL.GenericMessageWindow()->Hide();
      g_ClientDDL.GenericMessageWindowKill();
   }

   // Clean client advisor
   m_ClientAdvisor.CleanUp();

   m_bServerIsReset = true;

   if(g_ClientDDL.BackgroundEarth() )
      g_ClientDDL.BackgroundEarth()->RemoveNotificationSigns();

   m_sSelectedCountryFlagPath = L"";
   m_iSelectedCountryID       = 0;
   m_bStartTimeSet            = false;
   m_iSpecificUnitToMove         = EUnitMovementPermissions::c_iAnyUnitCanMove;
   m_iSpecificDestinationCountry = EUnitMovementPermissions::c_iUnitCanMoveAnywhere;
   CurrentThematicMap = ECharacteristicToHighlight::Geographic;
   KillCombatOverview();
   m_RegionIds.clear();   

   Memory::Clear(m_iRefusedMovCount, SP2::ERefusedMoveReason::Count);

   //Clear the player country Data
   g_ClientDAL.m_PlayerCountryData.Clear();

   if(g_ClientDDL.GameScenarioWindow() )
   {
      g_ClientDDL.GameScenarioWindow()->ClearSelectedScenario();
   }
   if(g_ClientDDL.ObjectivesWindow() )
   {
      g_ClientDDL.ObjectivesWindow()->ClearSelectedScenario();
   }
   m_vGameObjectives.clear();

   g_ClientDDL.Cleanup();
   g_ClientDAL.Cleanup();

   g_Joshua.UnitManager().Shutdown();

   // Clear game lobby
   if(g_ClientDDL.GameLobby())
   {
      g_ClientDDL.GameLobby()->CleanMessageQueue();		
   }

   //Cancel the thread that receives country data from the server
   if(g_ClientDAL.m_iRecvCountryDataThreadHandle)
   {
      TerminateThread(g_ClientDAL.m_iRecvCountryDataThreadHandle,0);
   }

   return true;
}

/*!
 * Access the client advisor
 **/
GClientAdvisor& GDataControlLayer::ClientAdvisor()
{
   return m_ClientAdvisor;
}

/*!
 * confirms the territory to occupy, confirm war declarations and raise event
 **/
void GDataControlLayer::OccupyTerritory(UINT32 in_iCountryID)
{
   m_OccupyTerritoryConfirmedWarDeclarations.clear();
   m_OccupyTerritoryPendingWarDeclarations.clear();
   m_iTerritoryToOccupy = in_iCountryID;

   //If we want to occupy ourselves, it's ok
   if(in_iCountryID == (UINT32)g_ClientDAL.ControlledCountryID())
   {
   }
   else
   {
      //If there is already a pending occupy territory question, do not do anything
      if(m_OccupyTerritoryPendingWarDeclarations.size())
      {
         return;
      }

      //Check if we already are enemies with the country we want to occupy
      EDiplomaticStatus::Enum l_eDiplStatus = g_ClientDAL.DiplomaticStatus(in_iCountryID,g_ClientDAL.ControlledCountryID());
      if(l_eDiplStatus != EDiplomaticStatus::Hostile)
      {
         //Must ask a confirmation if wish to declare war to that country
         m_OccupyTerritoryPendingWarDeclarations.insert(in_iCountryID);
      }

		/*
		const vector<GRegionControl>& l_vControls = g_ClientDAL.RegionControlArray();
      //Check if we have to declare war to some other countries if we want to occupy its
      //territory.  Must do so if the military controlled region by somebody else
      const set<UINT32>& l_CountryRegions = g_ClientDAL.CountryPoliticalControl(in_iCountryID);
      for(set<UINT32>::const_iterator l_RegionIt = l_CountryRegions.begin();
          l_RegionIt != l_CountryRegions.end();
          l_RegionIt++)
      {
			UINT32 l_iRegionID = *l_RegionIt;
         if( 
            (l_vControls[l_iRegionID].m_iMilitary != l_vControls[l_iRegionID].m_iPolitical) && 
            (l_vControls[l_iRegionID].m_iMilitary != g_ClientDAL.ControlledCountryID())
           )
         {
            m_OccupyTerritoryPendingWarDeclarations.insert(l_vControls[l_iRegionID].m_iMilitary);
         }
      }
		*/
   }

   //If there are confirmations we must do
   if(m_OccupyTerritoryPendingWarDeclarations.size())
   {
         const GCountry& l_Country = g_ClientDAL.Country(*m_OccupyTerritoryPendingWarDeclarations.begin());
         GGenericQuestion l_Question;
         l_Question.m_sMessage         = g_ClientDAL.GetString(100972);
			l_Question.m_sMessage			= l_Question.m_sMessage.ReplaceNextPattern(l_Country.Name(),L"[string]");
         l_Question.m_pOberserToNotify = this;

         //Pop the question
         m_pSubjectThatWillAnswerOccupyTerritoryQuestion = g_ClientDDL.GenericQuestion(l_Question);
   }
   else
   {
      //Raise the game event asking to control the territory
      SDK::GGameEventSPtr l_Evt = CREATE_GAME_EVENT(SP2::Event::GOneClickInvade);
      SP2::Event::GOneClickInvade* l_pEvt = (SP2::Event::GOneClickInvade*)l_Evt.get();
      l_Evt->m_iSource = g_SP2Client->Id();
      l_Evt->m_iTarget = SDK::Event::ESpecialTargets::Server;
      l_pEvt->m_iOccupiedCountry = m_iTerritoryToOccupy;
      l_pEvt->m_iOccupyingCountry = g_ClientDAL.ControlledCountryID();
      l_pEvt->m_OtherCountriesToDeclareWar = m_OccupyTerritoryConfirmedWarDeclarations;
      g_Joshua.RaiseEvent(l_Evt);
      m_OccupyTerritoryPendingWarDeclarations.clear();
      m_OccupyTerritoryConfirmedWarDeclarations.clear();
   }

 

}


/*!
 * Subject has changed
 **/
void GDataControlLayer::OnSubjectChange(DesignPattern::GSubject& in_Subject,
                                        const DesignPattern::GSubjectNotification& in_Notification,
                                        void* in_pAdditional)
{

   //If a generic question window about occupy Territory 
   if(&in_Subject == m_pSubjectThatWillAnswerOccupyTerritoryQuestion)
   {
      gassert(m_OccupyTerritoryPendingWarDeclarations.size() > 0,"We have an answer, there should remain at least one unanswered question");
      if(in_Notification == c_OnGenericQuestionWindowAnswerOk)
      {         
         m_OccupyTerritoryConfirmedWarDeclarations.insert(*m_OccupyTerritoryPendingWarDeclarations.begin());
         m_OccupyTerritoryPendingWarDeclarations.erase(m_OccupyTerritoryPendingWarDeclarations.begin());

         //If there still are unanswered questions, ask them
         if(m_OccupyTerritoryPendingWarDeclarations.size())
         {
            const GCountry& l_Country = g_ClientDAL.Country(*m_OccupyTerritoryConfirmedWarDeclarations.begin());

            GGenericQuestion l_Question;
            l_Question.m_sMessage         = g_ClientDAL.GetString(100972);
				l_Question.m_sMessage			= l_Question.m_sMessage.ReplaceNextPattern(l_Country.Name(),L"[string]");
            l_Question.m_pOberserToNotify = this;

            m_pSubjectThatWillAnswerOccupyTerritoryQuestion = g_ClientDDL.GenericQuestion(l_Question);
         }
         else
         {
            //Send the game event
            SDK::GGameEventSPtr l_Evt = CREATE_GAME_EVENT(SP2::Event::GOneClickInvade);
            SP2::Event::GOneClickInvade* l_pEvt = (SP2::Event::GOneClickInvade*)l_Evt.get();
            l_Evt->m_iSource = g_SP2Client->Id();
            l_Evt->m_iTarget = SDK::Event::ESpecialTargets::Server;
            l_pEvt->m_iOccupiedCountry = m_iTerritoryToOccupy;
            l_pEvt->m_iOccupyingCountry = g_ClientDAL.ControlledCountryID();
            l_pEvt->m_OtherCountriesToDeclareWar = m_OccupyTerritoryConfirmedWarDeclarations;
            g_Joshua.RaiseEvent(l_Evt);
            m_OccupyTerritoryPendingWarDeclarations.clear();
            m_OccupyTerritoryConfirmedWarDeclarations.clear();
         }
      }
      else if(in_Notification == c_OnGenericQuestionWindowAnswerCancel)
      {
         //Cancel all this, dont ask any more question and dont raise an event saying to occupy territory
         m_OccupyTerritoryConfirmedWarDeclarations.clear();
         m_OccupyTerritoryPendingWarDeclarations.clear();
         return;
      }
   }
} 

/*!
 * Starts the given tutorial
 **/
void GDataControlLayer::StartTutorial(ETutorials::Enum in_eTutorial)
{
   GTutorials::Instance().StartTutorial(in_eTutorial);
   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::TutorialBtn);
}


bool GDataControlLayer::RequestGameSpeedChange(REAL64 in_fNewSpeed)
{
   // request accepted ?
   bool l_bRequestAccepted = false;
   //If we are the admin player, we can request a game speed change
   if(g_Joshua.AdminPlayerID() == g_SP2Client->Id())
   {
      SDK::GGameEventSPtr l_Evt = CREATE_GAME_EVENT(SP2::Event::GChangeGameSpeed);
      l_Evt->m_iSource = g_Joshua.Client()->Id();
      l_Evt->m_iTarget = SDK::Event::ESpecialTargets::Server;
      SP2::Event::GChangeGameSpeed* l_pEvt = (SP2::Event::GChangeGameSpeed*)l_Evt.get();
      l_pEvt->NewSpeed(in_fNewSpeed);
      g_Joshua.RaiseEvent(l_Evt);
      l_bRequestAccepted = true;
   }
   return l_bRequestAccepted;
}

void GDataControlLayer::RequestCountriesRanks()
{
   SDK::GGameEventSPtr l_Evt = CREATE_GAME_EVENT(SP2::Event::GGetCountriesRanks);
   l_Evt->m_iSource = g_Joshua.Client()->Id();
   l_Evt->m_iTarget = SDK::Event::ESpecialTargets::Server;
   g_Joshua.RaiseEvent(l_Evt);   
}

/*!
 * Requests a unit production (sends an order to the server)
 **/
void GDataControlLayer::RequestUnitProduction(UINT32 in_iDesignID,
                                              UINT32 in_iQuantity,
                                              UINT32 in_iProductionSpeed,
                                              UINT32 in_iBuildingCountryID,
                                              EUnitProductionPriority::Enum in_ePriority)
{
   // Send query to server.
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GBuildUnitOrder);
   SP2::Event::GBuildUnitOrder* l_pBuildOrderEvent = (SP2::Event::GBuildUnitOrder*) (l_pEvent.get() );

   // Set Target and Source for this event
   l_pBuildOrderEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pBuildOrderEvent->m_iSource = g_SP2Client->Id();

   // Fill out order
   l_pBuildOrderEvent->m_ePriority              = in_ePriority;
   l_pBuildOrderEvent->m_iDesignID              = (UINT16)in_iDesignID;
   l_pBuildOrderEvent->m_iBuildingCountryID     = (UINT16)in_iBuildingCountryID;
   l_pBuildOrderEvent->m_iDestinationCountryID  = g_ClientDAL.ControlledCountryID();
   l_pBuildOrderEvent->m_iQuantity              = in_iQuantity;
   l_pBuildOrderEvent->m_iProductionSpeed       = (UINT8)in_iProductionSpeed;
   m_ClientAdvisor.RaiseEvent(l_pEvent);	
}

void GDataControlLayer::ShowTreaty(void* in_iTreatyID)
{
   INT32 l_iTreaty = (INT32)in_iTreatyID;

   if(!g_ClientDDL.TreatyDetailsWindow())
      g_ClientDDL.TreatyDetailsWindowSpawn();

   g_ClientDDL.TreatyDetailsWindow()->Show();
   g_ClientDDL.TreatyDetailsWindow()->Update(l_iTreaty);
}

/*!
* Display a treaty refusal in the trade window throuh its mail id
**/
void GDataControlLayer::ShowTreatyRefusal(void* in_pMailID)
{
   // retrieve our mail id
   INT32 l_iMailID = (INT32)in_pMailID;
	if(!g_ClientDDL.RegionControlChangeWindow())
		g_ClientDDL.RegionControlChangeWindowSpawn();
	
	GString l_sTextToPrint = g_ClientDAL.m_TreatyRefusalMail[l_iMailID].TreatyName() + L"\n\n";
		
	l_sTextToPrint += g_ClientDAL.GetString(102319);
	l_sTextToPrint += L": \n";

	if(g_ClientDAL.m_TreatyRefusalMail[l_iMailID].CountriesThatAccepted().size() == 0)
		l_sTextToPrint += L"  " + g_ClientDAL.GetString(101525) +"\n";
	else
	{
		set<GString> l_CountryToAdd;
		for(hash_set<UINT32>::const_iterator it = g_ClientDAL.m_TreatyRefusalMail[l_iMailID].CountriesThatAccepted().begin();
			 it != g_ClientDAL.m_TreatyRefusalMail[l_iMailID].CountriesThatAccepted().end(); it++)
		{
			l_CountryToAdd.insert(g_ClientDAL.Country(*it).Name());			
		}
		for(set<GString>::iterator it = l_CountryToAdd.begin();
			 it != l_CountryToAdd.end(); it++)
		{
			l_sTextToPrint += L"  " + *it + L"\n";
		}
	}

	l_sTextToPrint += L"\n\n";
	l_sTextToPrint += g_ClientDAL.GetString(102320);
	l_sTextToPrint += L": \n";

	set<GString> l_CountryToAdd;
	for(vector<pair<UINT32,ETreatyRefusal::Enum> >::const_iterator it = g_ClientDAL.m_TreatyRefusalMail[l_iMailID].CountriesThatRefused().begin();
		 it != g_ClientDAL.m_TreatyRefusalMail[l_iMailID].CountriesThatRefused().end(); it++)
	{		
		l_CountryToAdd.insert(g_ClientDAL.Country(it->first).Name() + L"  (" + g_ClientDAL.TreatyRefusalEnumToString(it->second) + L")"  + L"\n");					
	}
	for(set<GString>::iterator it = l_CountryToAdd.begin();
		 it != l_CountryToAdd.end(); it++)
	{
		l_sTextToPrint += L"  " + *it;
	}
	g_ClientDDL.RegionControlChangeWindow()->Show(l_sTextToPrint);
}

/*!
* Display a trade in the trade window throuh its mail id
**/
void GDataControlLayer::ShowTrade(void* in_pMailID)
{
   // retrieve our mail id
   INT32 l_iMailID = (INT32)in_pMailID;
   // make sure trade window is available
   if(!g_ClientDDL.TradeDetailedWindow())
      g_ClientDDL.TradeDetailedWindowSpawn();
   // show our trade trough mail id
   if(g_ClientDAL.GotTradeMailWithId(l_iMailID))
      g_ClientDDL.TradeDetailedWindow()->ShowTradeProposal(g_ClientDAL.m_TradesMail[l_iMailID]);
}

/*!
* display a combat over info in the battle summary window
**/
void GDataControlLayer::ShowCombatOver(void* in_pMailID)
{
   // retrieve our mail id
   INT32 l_iMailID = (INT32)in_pMailID;
   // make sure battle summary window is available
   if(!g_ClientDDL.BattleSummaryWindow())
      g_ClientDDL.BattleSummaryWindowSpawn((SDK::GUI::GBaseObject*)g_ClientDDL.BackgroundEarth());
   // show our battle summary trough mail id
   if(g_ClientDAL.GotCombatOverMailWithId(l_iMailID))
   {
      g_ClientDDL.BattleSummaryWindow()->SetArenaInformation(g_ClientDAL.m_CombatsOverMail[l_iMailID]);       
      g_ClientDDL.BattleSummaryWindow()->PlayResultMusic(g_ClientDAL.m_CombatsOverMail[l_iMailID]);
   }
}

void GDataControlLayer::ShowMissionReady(void* in_CellID)
{
   UINT32 l_iCellID = (UINT32)in_CellID;

   if(!g_ClientDDL.CovertActionsWindow())
      g_ClientDDL.CovertActionsWindowSpawn();

   g_ClientDDL.CovertActionsWindow()->Show();
   g_ClientDDL.CovertActionsWindow()->SelectCellRow(l_iCellID);
}

void GDataControlLayer::ShowMissionResult(void* in_pResultStruct)
{
   INT32 l_iMailID = (INT32)in_pResultStruct;

   map<INT32, SDataMissionOver>::iterator l_It = g_ClientDAL.m_DataMissionOver.find( l_iMailID );
   if(l_It != g_ClientDAL.m_DataMissionOver.end())
   {
      if(l_It->second.m_iCellSourceCountry == (UINT32)g_ClientDAL.ControlledCountryID())
      {
         if(!g_ClientDDL.CovertActionsMissionOutcomeWindow())
            g_ClientDDL.CovertActionsMissionOutcomeWindowSpawn();
         
         g_ClientDDL.CovertActionsMissionOutcomeWindow()->Show();
         g_ClientDDL.CovertActionsMissionOutcomeWindow()->Update(l_It->second.m_iCellID, l_It->second.m_bSuccessful, 
                                                                 l_It->second.m_bCaptured );
      }
      else
      {
         if(!g_ClientDDL.CovertActionsMissionOutcomeAttackedWindow())
            g_ClientDDL.CovertActionsMissionOutcomeAttackedWindowSpawn();
  
         g_ClientDDL.CovertActionsMissionOutcomeAttackedWindow()->Show();
         g_ClientDDL.CovertActionsMissionOutcomeAttackedWindow()->Update(l_It->second);
      }

      g_ClientDAL.m_DataMissionOver.erase(l_It);
   }
}

void GDataControlLayer::ShowMovementRefused(void* in_pRefusalID)
{
   // The void param corresponds to the type of movement refusal
   UINT32 l_iRefusalReason = (UINT32) in_pRefusalID;

   gassert(g_ClientDCL.m_iRefusedMovCount[l_iRefusalReason] > 0, "There is a refusal mail, but no refused move");

   GString l_sText = g_ClientDAL.GetString(EStrId::MoveDenialPrefix) + 
                     g_ClientDAL.GetString(EStrId::c_MoveDenial[l_iRefusalReason] );

   l_sText = l_sText.ReplaceNextPattern(GString(g_ClientDCL.m_iRefusedMovCount[l_iRefusalReason] ), "[number]");

   // Pop a generic message window displaying the message
   g_ClientDDL.GenericMessageWindowSpawn();
   g_ClientDDL.GenericMessageWindow()->Show(l_sText, NULL);

   // Reset refused movement count
   g_ClientDCL.m_iRefusedMovCount[l_iRefusalReason] = 0;
}

/*!
 * Request the relation history table
 * (auto updates will be send until we stop requesting updates)
 **/
void GDataControlLayer::StartRequestRelationHistory(UINT32 in_iCountry1,UINT32 in_iCountry2)
{
   SDK::GGameEventSPtr                l_Evt  = CREATE_GAME_EVENT(SP2::Event::GRelationHistoryFetch);
   SP2::Event::GRelationHistoryFetch* l_pEvt = (SP2::Event::GRelationHistoryFetch*)l_Evt.get();
   l_pEvt->m_iCountryA                       = in_iCountry1;
   l_pEvt->m_iCountryB                       = in_iCountry2;
   l_pEvt->m_iSource                         = g_SP2Client->Id();
   l_pEvt->m_iTarget                         = SDK::Event::ESpecialTargets::Server;
   l_pEvt->m_bStartFetch                     = true;
   g_Joshua.RaiseEvent(l_Evt);
}

/*!
 * Request to stop receiving further relation history updates
 **/
void GDataControlLayer::EndRequestRelationHistory()
{
   //If we are connected to the server
   if(g_Joshua.Connected())
   {
      SDK::GGameEventSPtr                l_Evt  = CREATE_GAME_EVENT(SP2::Event::GRelationHistoryFetch);
      SP2::Event::GRelationHistoryFetch* l_pEvt = (SP2::Event::GRelationHistoryFetch*)l_Evt.get();
      l_pEvt->m_iSource                         = g_SP2Client->Id();
      l_pEvt->m_iTarget                         = SDK::Event::ESpecialTargets::Server;
      l_pEvt->m_bStartFetch                     = false;
      g_Joshua.RaiseEvent(l_Evt);
   }
}

void GDataControlLayer::ConnectToGame(EGameTypes::Enum in_eGameType)
{
   // Kill server if creating or joining a multiplayer game
   bool l_bForceConnect = false;
   if( (in_eGameType == EGameTypes::HostMultiPlayer) ||
       (in_eGameType == EGameTypes::JoinMultiPlayer) )
   {
      l_bForceConnect = true;
   }

   g_SP2Client->ChangeGameType(in_eGameType);

   // Connect to server if needed
   if(l_bForceConnect || (!g_Joshua.Connected() && !g_Joshua.Connecting() ) )
   {
      g_Joshua.Disconnect();
      g_Joshua.KillServer();

      Sleep(0);

      // Set connection parameters for single player games
      if( (in_eGameType == EGameTypes::SinglePlayer) || 
          (in_eGameType == EGameTypes::Scenario) ||
          (in_eGameType == EGameTypes::Tutorial) ||
          g_SP2Client->LoadingGame() )
      {
         m_sIP             = LOCAL_IP_ADDRESS;
         m_iPort           = c_iServerDefaultPort;
         m_iMaxPlayers     = 1;
         m_bPrivate        = true;
         m_bDedicated      = false;
         g_SP2Client->DAL().GameOptions().NuclearAllowed(true);
         if(in_eGameType == EGameTypes::SinglePlayer)
         {
            m_sServerName = g_ClientDAL.GetString(102127);
         }
         else
         {
            m_sServerName = g_ClientDAL.GetString(102128);
         }

         // Generate a unique password that will ensure the single player game 
         // will only be connected by the current process
         srand(timeGetTime() );
         m_sPwd = GString(rand() + rand() );
         m_iTimeLimit = 0;
      }
      else if(in_eGameType == EGameTypes::HostMultiPlayer)
      {
         m_sIP             = LOCAL_IP_ADDRESS;
         m_sServerName     = g_ClientDDL.HostMPGameWindow()->m_sServerName;
         m_sPwd            = g_ClientDDL.HostMPGameWindow()->m_sPassword;
         m_iPort           = g_ClientDDL.HostMPGameWindow()->m_iPort;
         m_iMaxPlayers     = g_ClientDDL.HostMPGameWindow()->m_iMaxPlayers;
         m_bPrivate        = g_ClientDDL.HostMPGameWindow()->m_bPrivate;
         m_bDedicated      = g_ClientDDL.HostMPGameWindow()->m_bDedicated;
         g_SP2Client->DAL().GameOptions().NuclearAllowed(g_ClientDDL.HostMPGameWindow()->m_bNuclearWarfareEnabled);
      }

      g_Joshua.Password(m_sPwd);

      // Spawn server if needed
      if( (in_eGameType == EGameTypes::SinglePlayer) || 
          (in_eGameType == EGameTypes::HostMultiPlayer) || 
          (in_eGameType == EGameTypes::Scenario) ||
          (in_eGameType == EGameTypes::Tutorial) )
      {
         //Setup our mod specific command line parameter
         GString l_sSP2Params = g_SP2Client->DAL().GameOptions().NuclearAllowed() ? L"" : L"+nonuke";
         //Spawn joshua as server
         g_Joshua.SpawnServer(m_sServerName, m_sPwd, m_iMaxPlayers, m_iTimeLimit,
                              m_iPort, m_bPrivate, m_bDedicated, l_sSP2Params);
         // A dedicated server ends the game client
         if(m_bDedicated)
         {
            g_SP2Client->QuitGame();
            return;
         }
      }

      // Connect to server
      g_Joshua.Port((INT32)m_iPort);
	  
      if(!g_Joshua.Connect(m_sIP, GString(m_iPort), m_sPwd, true, m_PublicAndLocalIPs) )
      {
		  g_Joshua.Port(-1);
		  g_ClientDCL.CancelConnect();
		  g_ClientDDL.PopupMessage(L"Couldnt connect to server");			
      }
   }
}

void GDataControlLayer::InitGame()
{
   m_iEscapeKeyId = g_Joshua.GUIManager()->Hotkeys.Register(GKeyboardInfo(VK_ESCAPE, false, false, false));
}

void GDataControlLayer::LoadGameData()
{
   //Variables
   GFile    l_TempFile;
   GString  l_sTemp;

   //Initialize the DAL
   if(!g_ClientDAL.Initialize(SP2::CLIENT_CONFIG))
   {
      g_Joshua.Log(L"Couldnt Initialize the DAL",MSGTYPE_FATAL_ERROR);

      g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::InitError);
      return;
   };

   //Init the GUI Manager
   if(!g_ClientDAL.DialogDefinitionFile(l_TempFile))
   {
      g_Joshua.Log(L"Couldnt get the Dialog Definition File",MSGTYPE_FATAL_ERROR);
      g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::InitError);
      return;
   }

   if(!g_Joshua.GUIManager()->ReadDialogResource(l_TempFile))
   {
      g_Joshua.Log(SP2::ERROR_CANT_PROCESS_FILE + l_TempFile.Name(),MSGTYPE_CRITICAL_ERROR);
      g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::InitError);
      return;
   }

   //Initialize the DDL
   if(!g_ClientDDL.Initialize())
   {
      g_Joshua.Log(L"Couldnt Initialize the DDL",MSGTYPE_FATAL_ERROR);
      g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::InitError);
   }

   //Display the splash screen
   g_ClientDDL.SplashScreenSpawn();
   g_ClientDDL.SplashScreen()->Show();
   g_ClientDDL.ProgressMeterSpawn();
   g_ClientDDL.ProgressMeter()->PercentageUpdate(0);

   g_SP2Client->RegisterElements();

   RegisterCreateCountryDataMethod(GCountryDataClient::New);

   if(!g_ClientDAL.LoadUnitTypes() )
   {
      g_Joshua.Log(L"Couldnt load correctly the unit types",MSGTYPE_FATAL_ERROR);

      g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::InitError);
      return;
   }

   if(!g_ClientDAL.LoadUnitDesign() )
   {
      g_Joshua.Log(L"Couldnt load correctly the unit designs",MSGTYPE_FATAL_ERROR);

      g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::InitError);
      return;
   }

   //Register the Unit, Unit Group and Unit Design constructors to the unit manager
   {
      g_Joshua.UnitManager().RegisterUnitCreationFunc(SP2::GUnit::New);
      g_Joshua.UnitManager().RegisterUnitGroupCreationFunc(SP2::GUnitGroup::New);
      g_Joshua.UnitManager().RegisterUnitDesignCreationFunc(SP2::GUnitDesign::New);
      g_Joshua.UnitManager().RegisterArenaInfoCreationFunc(SP2::GArenaInfo::New);
   }	
	
   //Loading the soundbox
   {
      //Check if we must load the sound engine       
      bool l_bLoadMusic = true;
      //if(g_ClientDAL.XMLValue(SP2::PLAY_MUSIC,l_sTemp))
      //{
      //   l_bLoadMusic = (l_sTemp.ToINT32()? true : false);
      //}

      // Init sound Box
      if(l_bLoadMusic)
      {
         // Initialize Sound Box
         g_SP2Client->m_pSoundBox = new GSP2Sound();
         g_SP2Client->m_pSoundBox->CreateSoundTable();
         g_SP2Client->m_pSoundBox->LoadSounds();

         
         // Create Theme Manager
         AdaptiveMusic::GThemeManager *l_pThemeManager = new AdaptiveMusic::GThemeManager;
         l_pThemeManager->LoadThemes();

         // Initialize Music Player
         g_SP2Client->m_pAdaptiveMusicPlayer = new AdaptiveMusic::GThemePlayer( ( (CSDX_Player *)g_Joshua.SoundPlayer() )->Get_SDX_Info().m_DXSound,l_pThemeManager);

      }
   }

   //If we must load the map engine, load it
   bool l_bLoadMapEngine = true;
   if(g_ClientDAL.XMLValue(SP2::LOAD_MAP_ENGINE_NODE,l_sTemp))
   {
      l_bLoadMapEngine = (l_sTemp.ToINT32()? true : false);
   }
   g_ClientDDL.ProgressMeter()->PercentageUpdate(5);

   if(!g_ClientDAL.MapEngineInit(l_bLoadMapEngine) )
   {
      g_Joshua.Log(SP2::ERROR_INIT_MAP_ENGINE,MSGTYPE_FATAL_ERROR);
      g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::InitError);
      return;
   }

   g_ClientDDL.ProgressMeter()->PercentageUpdate(20);

   g_ClientDDL.ProgressMeter()->PercentageUpdate(30);

   g_ClientDDL.ProgressMeter()->Split(50);

   // Create base windows
   g_ClientDDL.BackgroundEarthSpawn();
   g_ClientDDL.BackgroundEarth()->Hide();

   g_ClientDDL.GenericMessageWindowSpawn();

   g_ClientDDL.MainMenuSpawn();
   g_ClientDDL.MainMenu()->Hide();

   g_ClientDDL.MiniMapSpawn();

   g_ClientDDL.TextOverlaySpawn();
   g_ClientDDL.MainBarSpawn();
   g_ClientDDL.SystemBarWindowSpawn();

   g_ClientDDL.ConsoleWindowSpawn();
   g_ClientDDL.ConsoleWindowHide();

   g_ClientDDL.GameLobbySpawn();

   g_ClientDDL.ProgressMeter()->EndSplit();
   g_ClientDDL.ProgressMeter()->Update();

   // Load Battle Units.
   GBattleScene::LoadAnimations();

   g_ClientDDL.ProgressMeter()->PercentageUpdate(100);

   g_ClientDDL.ResetMousePointer();

   g_Joshua.GUIManager()->Hotkeys.Register(GKeyboardInfo(VK_F10, false, false, false));
   g_Joshua.GUIManager()->Hotkeys.Register(GKeyboardInfo(VK_F9, false, false, false));
   g_Joshua.GUIManager()->Hotkeys.Register(GKeyboardInfo(VK_F2, false, false, false));
   g_Joshua.GUIManager()->Hotkeys.Register(GKeyboardInfo(VK_F3, false, false, false));
   g_Joshua.GUIManager()->Hotkeys.Register(GKeyboardInfo(VK_F4, false, false, false));
   g_Joshua.GUIManager()->Hotkeys.Register(GKeyboardInfo(VK_F5, false, false, false));
   g_Joshua.GUIManager()->Hotkeys.Unregister(m_iEscapeKeyId);

   // Init some memory
   InitCacheMemory();

   // Initialize the client Advisor
   m_ClientAdvisor.Initialize();

   g_ClientDDL.ProgressMeterKill();

   // Start music player
   g_SP2Client->m_pAdaptiveMusicPlayer->Start();

   g_SP2Client->m_pAdaptiveMusicPlayer->ThemeManager()->RequestTheme(AdaptiveMusic::ESP2MusicalTheme::Main);
   g_SP2Client->m_pAdaptiveMusicPlayer->ThemeManager()->SetIntensity(AdaptiveMusic::ESP2MusicalIntensity::Normal);

	LoadGameOptions();
}

void GDataControlLayer::LoadGameOptions()
{
	g_ClientDAL.m_DataOptionsWindow.m_bGeometricDetailDirty = true;
	g_ClientDAL.m_DataOptionsWindow.m_bGraphicDetailsDirty = true;
	g_ClientDAL.m_DataOptionsWindow.m_bGraphicModeDirty = true;
	g_ClientDAL.m_DataOptionsWindow.m_bPlayerNameDirty = true;
	g_ClientDAL.m_DataOptionsWindow.m_bSoundDirty = true;	

	GString l_sTemp;
	//Player name
	g_ClientDAL.XMLValue(SP2::PLAYER_NAME,l_sTemp);
	g_ClientDAL.m_DataOptionsWindow.m_sPlayerName = l_sTemp;		

	//Clouds
	g_ClientDAL.XMLValue(SP2::CLOUDS,l_sTemp);
	if(l_sTemp == L"true")
		g_ClientDAL.m_DataOptionsWindow.m_bClouds = true;
	else
		g_ClientDAL.m_DataOptionsWindow.m_bClouds = false;
	//Sun
	g_ClientDAL.XMLValue(SP2::SUN,l_sTemp);
	if(l_sTemp == L"true")
		g_ClientDAL.m_DataOptionsWindow.m_bSun = true;
	else
		g_ClientDAL.m_DataOptionsWindow.m_bSun = false;
	//Moon
	g_ClientDAL.XMLValue(SP2::MOON,l_sTemp);
	if(l_sTemp == L"true")
		g_ClientDAL.m_DataOptionsWindow.m_bMoon = true;
	else
		g_ClientDAL.m_DataOptionsWindow.m_bMoon = false;
	//Day Light Cycle
	g_ClientDAL.XMLValue(SP2::DAY_LIGHT_CYCLE,l_sTemp);
	if(l_sTemp == L"true")
		g_ClientDAL.m_DataOptionsWindow.m_bDayCycle = true;
	else
		g_ClientDAL.m_DataOptionsWindow.m_bDayCycle = false;
	//Geometric detail
	g_ClientDAL.XMLValue(SP2::GEOMETRIC_DETAIL,l_sTemp);
	if(l_sTemp == L"0")
		g_ClientDAL.m_DataOptionsWindow.m_eGeometricDetail = SP2::EQualityLevel::Low;
	else if(l_sTemp == L"2")
		g_ClientDAL.m_DataOptionsWindow.m_eGeometricDetail = SP2::EQualityLevel::High;
	else
		g_ClientDAL.m_DataOptionsWindow.m_eGeometricDetail = SP2::EQualityLevel::Medium;

	//Zoom level
	g_ClientDAL.XMLValue(SP2::ZOOM_LEVEL,l_sTemp);
	if(l_sTemp == L"0")
		g_ClientDAL.m_DataOptionsWindow.m_eZoomLevel = SP2::EZoomLevel::None;
	else if(l_sTemp == L"1")
		g_ClientDAL.m_DataOptionsWindow.m_eZoomLevel = SP2::EZoomLevel::Elevation;
	else if(l_sTemp == L"2")
		g_ClientDAL.m_DataOptionsWindow.m_eZoomLevel = SP2::EZoomLevel::ElevationWater;
	else
		g_ClientDAL.m_DataOptionsWindow.m_eZoomLevel = SP2::EZoomLevel::All;

	//Texture Detail
	g_ClientDAL.XMLValue(SP2::TEXTURE_DETAIL,l_sTemp);
	if(l_sTemp == L"0")
		g_ClientDAL.m_DataOptionsWindow.m_eTextureQuality = SP2::EQualityLevel::Low;
	else if(l_sTemp == L"1")
		g_ClientDAL.m_DataOptionsWindow.m_eTextureQuality = SP2::EQualityLevel::Medium;
   else
		g_ClientDAL.m_DataOptionsWindow.m_eTextureQuality = SP2::EQualityLevel::High;

	//AI Aggressiveness
	REAL32 l_fAggressiveness = 0.f;
	g_ClientDAL.XMLValue(SP2::AI_AGGRESSIVENESS,l_sTemp);
	l_fAggressiveness = l_sTemp.ToREAL32();
	if(l_fAggressiveness > 1.f)
		l_fAggressiveness = 1.f;
	else if(l_fAggressiveness < 0.f)
		l_fAggressiveness = 0.f;
	g_ClientDAL.m_DataOptionsWindow.m_fAIAggressiveness = l_fAggressiveness;

	//Music Volume
	REAL32 l_fMusicVolume = 0.f;
	g_ClientDAL.XMLValue(SP2::AUDIO_VOLUME,l_sTemp);
	l_fMusicVolume = l_sTemp.ToREAL32();
	if(l_fMusicVolume > 1.f)
		l_fMusicVolume = 1.f;
	else if(l_fMusicVolume < 0.f)
		l_fMusicVolume = 0.f;
	g_ClientDAL.m_DataOptionsWindow.m_fMusicVolume = l_fMusicVolume;

	//Sound Volume
	REAL32 l_fSoundVolume = 0.f;
	g_ClientDAL.XMLValue(SP2::SOUND_VOLUME,l_sTemp);
	l_fSoundVolume = l_sTemp.ToREAL32();
	if(l_fSoundVolume > 1.f)
		l_fSoundVolume = 1.f;
	else if(l_fSoundVolume < 0.f)
		l_fSoundVolume = 0.f;
	g_ClientDAL.m_DataOptionsWindow.m_fSoundVolume = l_fSoundVolume;

   // Autosave option
   if(g_ClientDAL.XMLValue(SP2::AUTOSAVE_ENABLED,l_sTemp))
      m_AutoSaveInformation.m_bAutoSave = (l_sTemp == L"true");
   if(g_ClientDAL.XMLValue(SP2::AUTOSAVE_INCREMENTAL,l_sTemp))
      m_AutoSaveInformation.m_bIncrementalSave = (l_sTemp == L"true");
   if(g_ClientDAL.XMLValue(SP2::AUTOSAVE_FREQUENCY,l_sTemp))
      m_AutoSaveInformation.m_iSaveFrequency = l_sTemp.ToINT32();
   if(g_ClientDAL.XMLValue(SP2::AUTOSAVE_FILENAME,l_sTemp))
      m_AutoSaveInformation.m_sSaveFileName = l_sTemp;
   if(g_ClientDAL.XMLValue(SP2::AUTOSAVE_COUNTER,l_sTemp))
      m_AutoSaveInformation.m_iSaveCount = l_sTemp.ToINT32();

	g_ClientDAL.ApplyOptions();

}

void GDataControlLayer::IterateGame()
{
   g_ClientDAL.UpdateCountryUnitValues();

   REAL64 l_fTime = g_Joshua.GameTime();
   g_ClientDAL.PathManager().Iterate(l_fTime);

   g_ClientDAL.IterateHistoryValues();

   //Have the advisor evaluate the situation
   REAL64 l_fActualClock = g_Joshua.Clock();
   if((l_fActualClock - m_fFiveSecondsClock) >= 5.f)
   {
      g_ClientDCL.ClientAdvisor().EvaluateSituation();

      /*
      EClientAdvisorMsg::Enum l_eMsgType;
      GString                 l_sMessage;
      while(g_ClientDCL.ClientAdvisor().NextMessage(l_eMsgType,l_sMessage))
      {
         //g_ClientDDL.GenericMessageWindowSpawn();
         //g_ClientDDL.GenericMessageWindow()->Show(l_sMessage,NULL);
         g_Joshua.Log(l_sMessage);
      }
      */
      m_fFiveSecondsClock = g_Joshua.Clock();
   }

   //Update the overall combat info window (the check to see if displayed)
   //is done inside the function call.
   UpdateCombatInfo();

   IterateMusicAdaptiveness();

   // Autosave in game type SP, Scenario or Hosting.
   if ( (g_SP2Client->CurrentGameType() == EGameTypes::SinglePlayer)     ||
        (g_SP2Client->CurrentGameType() == EGameTypes::HostMultiPlayer)  ||
        (g_SP2Client->CurrentGameType() == EGameTypes::Scenario)
      )
   {
      AutoSave();
   }
}

void GDataControlLayer::IterateMusicAdaptiveness(void)
{
   static REAL32 l_fEvaluationThemeTime = 0;
   static REAL32 l_fEvaluationIntensityTime = 0;
   static REAL32 l_fLastLosingEval = 0;
   static UINT32 l_iLastTotalMarker = 0;
   static UINT32 l_iLastChangeInMarker = 0;

   REAL32 l_timeDeltaTheme = g_Joshua.TimeCounter().GetTime() - l_fEvaluationThemeTime ;
   REAL32 l_timeDeltaIntensity = g_Joshua.TimeCounter().GetTime() - l_fEvaluationIntensityTime ;

   if(g_SP2Client->m_pAdaptiveMusicPlayer && l_timeDeltaTheme > 120)
   {
      AdaptiveMusic::GThemeManager * l_pThemeManager = g_SP2Client->m_pAdaptiveMusicPlayer->ThemeManager();
      // Evaluate !!
      // Loosing (between 0 and 1)
      REAL32 l_fLoosingEval =0.0f;
      l_fLoosingEval += 0.25f *  ((g_ClientDAL.m_PlayerCountryData.EconomicRank() -1) / (REAL32)g_ClientDAL.NbCountry());
      l_fLoosingEval += 0.25f *  ((g_ClientDAL.m_PlayerCountryData.MilitaryRank() -1) /(REAL32) g_ClientDAL.NbCountry()) ;
      l_fLoosingEval += 0.25f *  ((g_ClientDAL.m_PlayerCountryData.PoliticRank() -1) / (REAL32)g_ClientDAL.NbCountry()) ;
      l_fLoosingEval += 0.10f * (1 - g_ClientDAL.m_PlayerCountryData.HumanDevelopment());
      l_fLoosingEval += 0.05f * (1 - g_ClientDAL.m_PlayerCountryData.TelecomLevel());
      l_fLoosingEval += 0.10f * (1 - g_ClientDAL.m_PlayerCountryData.Infrastructure());

      // Evaluate change
      REAL32 l_change = l_fLoosingEval - l_fLastLosingEval;

      // First Evaluation ?
      if(l_iLastTotalMarker == 0)
         l_change = 0;

      if(l_change > 0.02f)
      {
         l_pThemeManager->RequestTheme(AdaptiveMusic::ESP2MusicalTheme::Loosing);
      }
      else
      if(l_change < -0.02f)
      {
         l_pThemeManager->RequestTheme(AdaptiveMusic::ESP2MusicalTheme::Winning);
      }
      else
      {
         l_pThemeManager->RequestTheme(AdaptiveMusic::ESP2MusicalTheme::Main);
      }

      l_fLastLosingEval = l_fLoosingEval;
    
      // Retain eval time
      l_fEvaluationThemeTime = g_Joshua.TimeCounter().GetTime();
   }
  
   if(g_SP2Client->m_pAdaptiveMusicPlayer && l_timeDeltaIntensity > 60)
   {
      AdaptiveMusic::GThemeManager * l_pThemeManager = g_SP2Client->m_pAdaptiveMusicPlayer->ThemeManager();
   
      UINT32 l_iChangeInMarker = g_ClientDAL.m_iTotalMarkerReceivedSoFar - l_iLastTotalMarker;
      if(l_iLastChangeInMarker)
      {
         REAL32 l_fRatioBetweenMarkerChanges = l_iChangeInMarker / (REAL32)l_iLastChangeInMarker;
         if(l_fRatioBetweenMarkerChanges < 0.75f)
            l_pThemeManager->SetIntensity(AdaptiveMusic::ESP2MusicalIntensity::Low);
         else 
         if(l_fRatioBetweenMarkerChanges > 1.25f)
         {
            l_pThemeManager->SetIntensity(AdaptiveMusic::ESP2MusicalIntensity::High);
         }
         else
         {
            l_pThemeManager->SetIntensity(AdaptiveMusic::ESP2MusicalIntensity::Normal);
         }
      }
      else
      {
         l_pThemeManager->SetIntensity(AdaptiveMusic::ESP2MusicalIntensity::Normal);
      }

      l_iLastTotalMarker      = g_ClientDAL.m_iTotalMarkerReceivedSoFar;
      l_iLastChangeInMarker   = l_iChangeInMarker;

      l_fEvaluationIntensityTime = g_Joshua.TimeCounter().GetTime();

   }
}

bool GDataControlLayer::ServerIsReseted() const
{
   return m_bServerIsReset;
}

void GDataControlLayer::BeginGame()
{
   m_bServerIsReset = false;

   // Begin fetch of historical values
   g_ClientDAL.BeginHistoryValuesIterations();

   g_ClientDAL.DataMilitaryWindow.PreviouslyShown = false;
   g_ClientDAL.DataMilitaryWindow.Left = -1;
   g_ClientDAL.DataMilitaryWindow.Top = -1;
   g_ClientDAL.DataEconomicWindow.PreviouslyShown = false;
   g_ClientDAL.DataEconomicWindow.Left = -1;
   g_ClientDAL.DataEconomicWindow.Top = -1;
   g_ClientDAL.DataEconomicHealthWindow.PreviouslyShown = false;
   g_ClientDAL.DataEconomicHealthWindow.Left = -1;
   g_ClientDAL.DataEconomicHealthWindow.Top = -1;
   g_ClientDAL.DataBudgetWindow.PreviouslyShown = false;
   g_ClientDAL.DataBudgetWindow.Left = -1;
   g_ClientDAL.DataBudgetWindow.Top = -1;
   g_ClientDAL.DataResourcesWindow.PreviouslyShown = false;
   g_ClientDAL.DataResourcesWindow.Left = -1;
   g_ClientDAL.DataResourcesWindow.Top = -1;
   g_ClientDAL.DataPoliticWindow.PreviouslyShown = false;
   g_ClientDAL.DataPoliticWindow.Left = -1;
   g_ClientDAL.DataPoliticWindow.Top = -1;
   g_ClientDAL.DataConstitutionalForm.PreviouslyShown = false;
   g_ClientDAL.DataConstitutionalForm.Left = -1;
   g_ClientDAL.DataConstitutionalForm.Top = -1;
   g_ClientDAL.DataInternalLawsWindow[0].PreviouslyShown = false;
   g_ClientDAL.DataInternalLawsWindow[0].Left = -1;
   g_ClientDAL.DataInternalLawsWindow[0].Top = -1;
   g_ClientDAL.DataInternalLawsWindow[1].PreviouslyShown = false;
   g_ClientDAL.DataInternalLawsWindow[1].Left = -1;
   g_ClientDAL.DataInternalLawsWindow[1].Top = -1;
   g_ClientDAL.DataInternalLawsWindow[2].PreviouslyShown = false;
   g_ClientDAL.DataInternalLawsWindow[2].Left = -1;
   g_ClientDAL.DataInternalLawsWindow[2].Top = -1;


   g_ClientDDL.SpawnWindowsLvl();

   g_ClientDDL.MainBar()->UpdateSpeed( g_Joshua.GameSpeed() );

   //g_ClientDAL.FetchEconomicWindowInfo(g_ClientDAL.ControlledCountryID());
   g_ClientDAL.FetchPoliticWindowInfo(g_ClientDAL.ControlledCountryID());

   g_ClientDDL.MainBar()->Show();
   g_ClientDDL.SystemBarWindow()->Show();
   g_ClientDDL.MiniMap()->Show();

   g_ClientDDL.AnimationPlaySliding(GFX::PLAYBACK_FORWARD, g_ClientDDL.MainBar()->AnimationSlidingTrack() );
   g_ClientDDL.AnimationPlaySliding(GFX::PLAYBACK_FORWARD, g_ClientDDL.MiniMap()->AnimationSlidingTrack() );

   // Select Country
   g_ClientDCL.SelectHomeCountry();

   g_ClientDAL.SendAdvisorSettings( g_ClientDAL.m_eAdvisorStatusEconomic, g_ClientDAL.m_eAdvisorStatusPolitic, g_ClientDAL.m_eAdvisorStatusMilitary );


   // Request list of used unit to avoid having them in the deployement list
   RequestUsedUnit();
}

void GDataControlLayer::ReceiveData()
{
   // Create Custom Scenario
   GSmartPtr<GGameScenario> l_pScenario(new GGameScenario() );
   l_pScenario->Title(g_ClientDAL.GetString(102360));
   l_pScenario->Description(GString("User defined goal(s)"));                 
   g_ClientDAL.Scenario(l_pScenario);


   if(!g_SP2Client->LoadingGame() )
   {
      // Set objectives if needed
      if(g_SP2Client->CurrentGameType() == EGameTypes::HostMultiPlayer || g_SP2Client->CurrentGameType() == EGameTypes::SinglePlayer)
      {    
         for(UINT32 i=0;i<m_vGameObjectives.size();i++)
         {
            GGameObjective* l_pObjective = new GGameObjective;
            *l_pObjective = m_vGameObjectives[i];
            g_ClientDAL.Scenario()->AddObjective(l_pObjective);
         }

         //Send the game event that sets the objectives
         SDK::GGameEventSPtr l_SetObjEvt = CREATE_GAME_EVENT(SP2::Event::GSetGameObjectives);
         l_SetObjEvt->m_iSource = g_Joshua.Client()->Id();
         l_SetObjEvt->m_iTarget = SDK::Event::ESpecialTargets::Server;
         SP2::Event::GSetGameObjectives* l_pSetGameObj = (SP2::Event::GSetGameObjectives*)l_SetObjEvt.get();
         l_pSetGameObj->m_vObjectives = m_vGameObjectives;
         g_Joshua.RaiseEvent(l_SetObjEvt);
      }
   }

   // Fetch static country info
   RequestStaticCountryDataToServer();
}

void GDataControlLayer::StartHostServerGame()
{
   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::HostStartBtn);
}

void GDataControlLayer::InitialDataReceived()
{
   for(UINT32 i = 1;i <= g_ClientDAL.NbCountry();i ++)
   {
      g_ClientDAL.DirtyCountryUnits(i);
   }

   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::DataReceived);
}

void GDataControlLayer::SetupTutorial()
{
   g_ClientDCL.SystemSendPlayerInformationToServer(c_iMilitaryTutorialCountryToControl,L"Player",SDK::PLAYER_STATUS_READY);
}

void GDataControlLayer::TrainUnits(list<UINT32>& in_ListOfUnitIdToTrain, GUI::GBaseObject* in_pGuiCallback, Hector::DesignPattern::GObserver *in_pWhoNeedToBeNotify)
{		
   gassert(in_pWhoNeedToBeNotify,"Invalid pointer for notification");

   // Copy Data
   m_UnitListToTrain = in_ListOfUnitIdToTrain;

   REAL32 l_fMaxDay  = 0 ;
   REAL32 l_fTotalCost = 0 ; 
   list<UINT32>::iterator l_UnitIterator = in_ListOfUnitIdToTrain.begin();
   while(l_UnitIterator != in_ListOfUnitIdToTrain.end())
   {
      GUnit* l_pUnit = (GUnit*)g_Joshua.UnitManager().Unit(*l_UnitIterator);
      SP2::GUnitDesign* l_pUnitDesign =(SP2::GUnitDesign*) l_pUnit->Design();
      if((l_pUnit->Training() == ETrainingLevel::Elite) ||
         (l_pUnitDesign->Type()->Id() == EUnitType::BallisticMissile))
      {
         m_UnitListToTrain.remove(*l_UnitIterator);
      }
      else
      {
         ETrainingLevel::Enum l_eNextTrainingLevel = (ETrainingLevel::Enum)(l_pUnit->Training() + 1);
         l_fTotalCost += g_ClientDAL.TrainingUnitCost(l_eNextTrainingLevel) * l_pUnit->Qty();
         if(l_fMaxDay < g_ClientDAL.TrainingUnitTime(l_eNextTrainingLevel))
            l_fMaxDay = g_ClientDAL.TrainingUnitTime(l_eNextTrainingLevel);
      }
      
      l_UnitIterator++;
   }

   if(l_fTotalCost > 0)
   {
      // As for a confirmation
      GGenericQuestion  l_Question;

		GString l_sMessage = g_ClientDAL.GetString(102139);
		l_sMessage = l_sMessage.ReplaceNextPattern(GString((UINT32)(l_fMaxDay)), "[number]");
		l_sMessage += L"\n" + GString::FormatNumber(l_fTotalCost, L" ", L"", L"", L"$", 3, 0);
      l_Question.m_sMessage         = l_sMessage;
      l_Question.m_pParentWindow    = in_pGuiCallback;
      l_Question.m_pOberserToNotify = in_pWhoNeedToBeNotify;

      g_ClientDDL.GenericQuestion(l_Question);
   }
}

void GDataControlLayer::DisbandUnits(list<UINT32>& in_ListOfUnitIdToDisband)
{
   // Train units.
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GDisbandUnitRequest);
   SP2::Event::GDisbandUnitRequest* l_pDisbandUnitRequest = (SP2::Event::GDisbandUnitRequest*) (l_pEvent.get() );

   // Set Target and Source for this event
   l_pDisbandUnitRequest->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pDisbandUnitRequest->m_iSource = g_SP2Client->Id();
   l_pDisbandUnitRequest->m_UnitIdListToDisband = in_ListOfUnitIdToDisband;

   // Fire event
   g_Joshua.RaiseEvent(l_pEvent);	
}

void GDataControlLayer::RequestTraining(void)
{
   // Train units.
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GTrainUnitRequest);
   SP2::Event::GTrainUnitRequest* l_pTrainUnitRequest = (SP2::Event::GTrainUnitRequest*) (l_pEvent.get() );

   // Set Target and Source for this event
   l_pTrainUnitRequest->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pTrainUnitRequest->m_iSource = g_SP2Client->Id();
   l_pTrainUnitRequest->m_UnitIdListToTrain = m_UnitListToTrain;

   // Fire event
   g_Joshua.RaiseEvent(l_pEvent);	

   // Add all units to the local training list
   list<UINT32>::iterator l_UnitIterator  = m_UnitListToTrain.begin();
   while(l_UnitIterator != m_UnitListToTrain.end())
   {
      SP2::GUnit* l_pUnit = (SP2::GUnit*) g_Joshua.UnitManager().Unit(*l_UnitIterator);
      ETrainingLevel::Enum l_eNextTrainingLevel = (ETrainingLevel::Enum)(l_pUnit->Training() + 1);
      if(l_eNextTrainingLevel <= ETrainingLevel::Elite)
      {
         GUnitInTraining l_UnitInTraining;
         l_UnitInTraining.m_fTrainingStartTime        = (REAL32) g_Joshua.GameTime();
         l_UnitInTraining.m_fTrainingCompletionTime   = (REAL32) g_Joshua.GameTime() + g_ClientDAL.TrainingUnitTime(l_eNextTrainingLevel);
         l_UnitInTraining.m_ePreviousTrainingLevel    = l_pUnit->Training();
         g_ClientDAL.m_UnitInTraining.insert(make_pair<UINT32,GUnitInTraining>(*l_UnitIterator,l_UnitInTraining));
      }
      l_UnitIterator++;
   }
}

void GDataControlLayer::BombardRegionWithSelectedUnits(UINT32 in_iRegionID)
{
   SDK::GGameEventSPtr l_Evt = CREATE_GAME_EVENT(SP2::Event::GPerformOffshoreBombardment);
   SP2::Event::GPerformOffshoreBombardment* l_pEvt = (SP2::Event::GPerformOffshoreBombardment*)l_Evt.get();
   
   l_pEvt->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pEvt->m_iSource = g_SP2Client->Id();
    
   l_pEvt->m_iTargetRegion = in_iRegionID;

   //Among all the actually selected unit groups, take the ones that can bombard
   for(set<UINT32>::const_iterator l_It = g_ClientDAL.SelectedUnitsID().begin();
                                    l_It != g_ClientDAL.SelectedUnitsID().end();
                                    l_It++)
   {
      SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_It);
      if(l_pGroup->CanPerformOffShoreBombardment())
         l_pEvt->m_vBombardingUnitGroups.insert(*l_It);
   }


   //If there are no units selected actually, process all the unit groups of the country and
   //use all the units that can bombard
   if(!l_pEvt->m_vBombardingUnitGroups.size())
   {
      const set<UINT32>& l_CountryUnitGroups = g_Joshua.UnitManager().CountryUnitGroups(g_ClientDAL.ControlledCountryID());

      for(set<UINT32>::const_iterator l_It = l_CountryUnitGroups.begin();
                                      l_It != l_CountryUnitGroups.end();
                                      l_It++)
      {
         SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_It);
         //If the group can perform offshore bombardments
         if(l_pGroup->CanPerformOffShoreBombardment())
         {
            l_pEvt->m_vBombardingUnitGroups.insert(*l_It);
         }
      }
   }


   if(l_pEvt->m_vBombardingUnitGroups.size())
   {
      g_Joshua.RaiseEvent(l_Evt);   
   }
}

void GDataControlLayer::AutoSave(void) 
{

   if(m_AutoSaveInformation.m_bAutoSave && m_AutoSaveInformation.m_bNowAutoSaving == false && g_ClientDAL.GameIsOver() == false)
   {
      // Is it time to save?
      REAL32 l_fTimeDelta = g_Joshua.TimeCounter().GetTime() - m_AutoSaveInformation.m_fLastSaveTime;
      if(l_fTimeDelta > m_AutoSaveInformation.m_iSaveFrequency * 60.0f)
      {
         GString l_sSaveFileName = m_AutoSaveInformation.m_sSaveFileName;
         if(m_AutoSaveInformation.m_bIncrementalSave)
         {
            wchar_t l_pSaveFileNameNumber[256];
            swprintf(l_pSaveFileNameNumber,L"%04i",m_AutoSaveInformation.m_iSaveCount);
            l_sSaveFileName +=l_pSaveFileNameNumber;
            m_AutoSaveInformation.m_iSaveCount++;
            m_AutoSaveInformation.m_iSaveCount%=10000;

            // Keep autosave counter
            GString l_sNode = SP2::AUTOSAVE_COUNTER;
            GString l_sValue = GString(g_ClientDCL.AutoSaveInformation().m_iSaveCount);
            g_ClientDAL.SaveNodeToXML(l_sNode,l_sValue,GString(CLIENT_CONFIG));
         }
         
         m_AutoSaveInformation.m_bNowAutoSaving = true;
         SaveGame(l_sSaveFileName);
      }

   }
}

GAutoSaveInformation& GDataControlLayer::AutoSaveInformation(void)
{
   return m_AutoSaveInformation;
}

void GDataControlLayer::SaveGame(const GString& in_sSaveName)
{
   gassert(g_SP2Client->CurrentFSMState() == FSM::EStates::Playing, "Trying to save while non playing");
   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::SaveGame);

   // Make sure save directory exists
   CreateSaveDirectory();

   // Save the client data
   m_CurrentGameData.m_SaveDate = GDateTime();
   m_CurrentGameData.m_GameDate = g_ClientDAL.ActualDate();
   if(g_SP2Client->CurrentGameType() == EGameTypes::Scenario)
   {
      m_CurrentGameData.m_pScenario = GSmartPtr<GGameScenario>(new GGameScenario);
      *(m_CurrentGameData.m_pScenario) = *(g_ClientDAL.Scenario() );
   }
   else
   {
      m_CurrentGameData.m_pScenario = GSmartPtr<GGameScenario>(NULL);
   }

   if( (g_SP2Client->CurrentGameType() == EGameTypes::HostMultiPlayer) || 
       (g_SP2Client->CurrentGameType() == EGameTypes::JoinMultiPlayer) )
   {
      m_CurrentGameData.m_iGameIsMp = 1;
   }
   else
   {
      m_CurrentGameData.m_iGameIsMp = 0;
   }

   m_CurrentGameData.m_iCountryID = g_ClientDAL.ControlledCountryID();
   const GCountry& l_Country = g_ClientDAL.Country(g_ClientDAL.ControlledCountryID() );
   m_CurrentGameData.m_sCountryName = l_Country.Name();
   m_CurrentGameData.m_sCountryFlagPath = GString(FLAG_SMALL_DEFAULT_FOLDER) + l_Country.Flag();
   m_CurrentGameData.m_sPlayerName = g_SP2Client->PlayerName();

   EGameDataError::Enum l_eResult = g_ClientDAL.DataManager()->SaveGame(g_Joshua.CurrentMod().m_sPath + c_sSaveGameLocation + in_sSaveName + c_sSaveExtClient, c_iSP2GameType, c_iSP2DataVersion);

   if(l_eResult == EGameDataError::NoError)
   {
      // Send save request to server
      SDK::GGameEventSPtr l_SaveEvent = CREATE_GAME_EVENT(SP2::Event::GSaveGame);
      l_SaveEvent->m_iSource = g_SP2Client->Id();
      l_SaveEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;

      Event::GSaveGame* l_pSave = (Event::GSaveGame*) l_SaveEvent.get();
      l_pSave->m_sName = in_sSaveName;

      g_Joshua.RaiseEvent(l_SaveEvent);
   }
   else
   {
      // Error while saving client data, immediately inform user of error
      FinalizeGameSave(l_eResult);
   }

   m_eSaveLoadResult = (EGameDataError::Enum) -1;
}

void GDataControlLayer::FinalizeGameSave(EGameDataError::Enum in_eSaveResult)
{
   m_eSaveLoadResult = in_eSaveResult;
   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::EndSaveGame);
   BuildSavedGameList();
}

void GDataControlLayer::DisplaySaveResult()
{
   if(m_eSaveLoadResult != (EGameDataError::Enum) -1)
   {
      if(!g_ClientDDL.GenericMessageWindow() )
         g_ClientDDL.GenericMessageWindowSpawn();

      // Beware of error when player ahs no admin access, the ID is different
      if(m_eSaveLoadResult != c_iGameSaveLoadErrorNotAdmin)
      {
         if(m_AutoSaveInformation.m_bNowAutoSaving == false)
            g_ClientDDL.GenericMessageWindow()->Show(g_ClientDAL.GetString(EStrId::c_SaveResult[m_eSaveLoadResult] ), NULL);
      }
      else
      {
         g_ClientDDL.GenericMessageWindow()->Show(g_ClientDAL.GetString(EStrId::PlayerNotAdminSave), NULL);
      }
   }

   // Flag for autosave.
   m_AutoSaveInformation.m_bNowAutoSaving = false;
   m_AutoSaveInformation.m_fLastSaveTime = g_Joshua.TimeCounter().GetTime();
}

bool GDataControlLayer::OnSave(GIBuffer& io_Buffer)
{
   io_Buffer << m_CurrentGameData.m_SaveDate
             << m_CurrentGameData.m_GameDate
             << m_CurrentGameData.m_iGameIsMp
             << m_CurrentGameData.m_iCountryID
             << m_CurrentGameData.m_sCountryName
             << m_CurrentGameData.m_sCountryFlagPath
             << m_CurrentGameData.m_sPlayerName;

   // Only save scenario if present
   if(m_CurrentGameData.m_pScenario.get() )
   {
      io_Buffer << (UINT8) 1;
      m_CurrentGameData.m_pScenario->Serialize(io_Buffer);
   }
   else
   {
      io_Buffer << (UINT8) 0;
   }

   return true;
}

bool GDataControlLayer::OnLoad(GOBuffer& io_Buffer)
{
   m_CurrentGameData.m_bInfoComplete = true;

   io_Buffer >> m_CurrentGameData.m_SaveDate
             >> m_CurrentGameData.m_GameDate
             >> m_CurrentGameData.m_iGameIsMp
             >> m_CurrentGameData.m_iCountryID
             >> m_CurrentGameData.m_sCountryName
             >> m_CurrentGameData.m_sCountryFlagPath
             >> m_CurrentGameData.m_sPlayerName;

   // Only load scenario if present
   UINT8 l_iLoadScenario;
   io_Buffer >> l_iLoadScenario;
   if(l_iLoadScenario)
   {
      m_CurrentGameData.m_pScenario = GSmartPtr<GGameScenario>(new GGameScenario() );
      m_CurrentGameData.m_pScenario->Unserialize(io_Buffer);
   }
   else
   {
      m_CurrentGameData.m_pScenario = GSmartPtr<GGameScenario>(NULL);
   }

   return true;
}




/*!
 * Bombardment notification, says when a bombardment started or stopped in a given region
 **/
void GDataControlLayer::OnBombardmentNotification(UINT32 in_iRegionID,bool in_bStarting,const vector<GVector2D<REAL32> >& in_vBombardedPoints)
{
   gassert(in_iRegionID > 0,"Bombardments should never be seen on water... and it should never happen, check the server");
   if(in_iRegionID <= 0)
      return;

   //Check if there is already a bombardment notification for that region
   hash_map<UINT32,GDisplayedBombardedRegion>::iterator l_It = g_ClientDAL.m_BombardedRegions.find(in_iRegionID);
   if(in_bStarting)
   {
      //Not already present, insert a new item
      if(l_It == g_ClientDAL.m_BombardedRegions.end())
      {
         GDisplayedBombardedRegion l_BombardDisplay;
         l_BombardDisplay.m_iRefCount = 1;
         l_BombardDisplay.m_iRegionId = in_iRegionID;
         l_BombardDisplay.m_vBombardmentPoints = in_vBombardedPoints;
         l_BombardDisplay.m_vDisplayedClock.resize(in_vBombardedPoints.size(),-1.f);
         g_ClientDAL.m_BombardedRegions[in_iRegionID] = l_BombardDisplay;
      }
      else
      {
         l_It->second.m_iRefCount++;
      }
   }
   else
   {
      //Not already present, ignore, else, decrease refcount, if refcount == 0, remove item
      if(l_It != g_ClientDAL.m_BombardedRegions.end())
      {
         l_It->second.m_iRefCount--;
         if(!l_It->second.m_iRefCount)
            g_ClientDAL.m_BombardedRegions.erase(l_It);
      }
   }
}

void GDataControlLayer::RequestStaticCountryDataToServer()
{
   GGameEventSPtr l_RequestEvent = CREATE_GAME_EVENT(Event::GRequestCountryStaticData);
   l_RequestEvent->m_iSource = g_Joshua.Client()->Id();
   l_RequestEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;

   g_Joshua.RaiseEvent(l_RequestEvent);
}

void GDataControlLayer::ReceiveStaticCountryDataFromServer(SP2::Event::GReceiveCountryStaticData* in_pData)
{
   g_ClientDAL.ReceiveCountryStaticData(in_pData);
   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::CountryDataReceived);
}

void GDataControlLayer::ReceiveStaticCountryDataError()
{
   g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::CountryDataFailed);
}

void GDataControlLayer::LoadGame(const GString& in_sLoadName)
{
   // Load the client data
   EGameDataError::Enum l_eResult = g_ClientDAL.DataManager()->LoadGame(g_Joshua.CurrentMod().m_sPath + c_sSaveGameLocation + in_sLoadName + c_sSaveExtClient, c_iSP2GameType, c_iSP2DataVersion);

   if(l_eResult == EGameDataError::CantOpenFile)
   {
      // If we are here, it may be because the client save is not present, 
      // but the server save is. If so, load the game on the server, but let
      // the player choose its country like if the game was multiplayer.
      l_eResult = EGameDataError::NoError;
      m_CurrentGameData.m_iGameIsMp = true;
      m_CurrentGameData.m_pScenario = GSmartPtr<GGameScenario>(NULL);
      m_CurrentGameData.m_iCountryID = 0;
   }

   m_sCurrentGame = in_sLoadName;

   if(l_eResult == EGameDataError::NoError)
   {
      // Only inform state machine & client that we load when the server 
      // is actually asked to load, so that if an error occurs before, the current
      // game is not lost.
      g_SP2Client->LoadGame(true);

      // Change game type depending on client game content
      if(m_CurrentGameData.m_iGameIsMp)
      {
         g_SP2Client->ChangeGameType(EGameTypes::HostMultiPlayer);
      }
      else
      {
         if(m_CurrentGameData.m_pScenario.get() )
         {
            g_SP2Client->ChangeGameType(EGameTypes::Scenario);
         }
         else
         {
            g_SP2Client->ChangeGameType(EGameTypes::SinglePlayer);
         }
      }
      g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::LoadGame);
   }
   else
   {
      // Error while loading client data, immediately inform user of error
      FinalizeGameLoad(l_eResult);
   }

   m_eSaveLoadResult = (EGameDataError::Enum) -1;
}

void GDataControlLayer::FinalizeGameLoad(EGameDataError::Enum in_eLoadResult)
{
   m_eSaveLoadResult = in_eLoadResult;
   if(m_eSaveLoadResult == SDK::EGameDataError::NoError)
   {
      g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::EndLoadGame);
   }
   else
   {
      g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::ErrorNewOrLoadGame);
   }
}

void GDataControlLayer::FinalizeGameNew(EGameDataError::Enum in_eNewResult)
{
   m_eSaveLoadResult = in_eNewResult;
   if(m_eSaveLoadResult == SDK::EGameDataError::NoError)
   {
      g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::EndNewGame);
   }
   else
   {
      g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::ErrorNewOrLoadGame);
   }
}

void GDataControlLayer::DisplayLoadResult()
{
   if( (m_eSaveLoadResult != (EGameDataError::Enum) -1) && 
       (m_eSaveLoadResult != EGameDataError::NoError) )
   {
      if(!g_ClientDDL.GenericMessageWindow() )
         g_ClientDDL.GenericMessageWindowSpawn();

      // Beware of error when player has no admin access, the ID is different
      if(m_eSaveLoadResult != c_iGameSaveLoadErrorNotAdmin)
      {
         g_ClientDDL.GenericMessageWindow()->Show(g_ClientDAL.GetString(EStrId::c_LoadResult[m_eSaveLoadResult] ), NULL);
      }
      else
      {
         g_ClientDDL.GenericMessageWindow()->Show(g_ClientDAL.GetString(EStrId::PlayerNotAdminLoad), NULL);
      }
   }
}

void GDataControlLayer::DisplayNewResult()
{
   if( (m_eSaveLoadResult != (EGameDataError::Enum) -1) && 
       (m_eSaveLoadResult != EGameDataError::NoError) )
   {
      if(!g_ClientDDL.GenericMessageWindow() )
         g_ClientDDL.GenericMessageWindowSpawn();

      g_ClientDDL.GenericMessageWindow()->Show(g_ClientDAL.GetString(EStrId::CannotCreateGame), NULL);
   }
}

void GDataControlLayer::BuildSavedGameList()
{
   m_vSaveList.clear();

	// Checks if the save directory exists
	WIN32_FIND_DATA l_DirItemData;
   HANDLE l_DirSearchHandle = FindFirstFile( GString(g_Joshua.CurrentMod().m_sPath + c_sSaveGameLocation + L"*").c_str(), &l_DirItemData);
   if(l_DirSearchHandle != INVALID_HANDLE_VALUE)
   {
      // Find all files that have client or server extensions
      hash_set<GString> m_ClientSaves;
      hash_map<GString, GDateTime> m_ServerSaves;
      do
      {
         // Only process files (no directories)
         if( !(l_DirItemData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
         {
            GString l_sFileName(l_DirItemData.cFileName);
            if(l_sFileName.rfind(c_sSaveExtClient) == (l_sFileName.size() - c_sSaveExtClient.size() ) )
            {
               m_ClientSaves.insert(l_sFileName.substr(0, l_sFileName.size() - c_sSaveExtClient.size() ) );
            }
            else if(l_sFileName.rfind(c_sSaveExtServer) == (l_sFileName.size() - c_sSaveExtServer.size() ) )
            {
               SYSTEMTIME l_FileCreationUTC, l_FileCreationTimeLocal;
               FileTimeToSystemTime(&l_DirItemData.ftCreationTime, &l_FileCreationUTC);
               SystemTimeToTzSpecificLocalTime(NULL, &l_FileCreationUTC, &l_FileCreationTimeLocal);
               m_ServerSaves.insert(
                  make_pair(l_sFileName.substr(0, l_sFileName.size() - c_sSaveExtServer.size() ),
                            GDateTime(l_FileCreationTimeLocal.wYear, 
                                      l_FileCreationTimeLocal.wMonth, 
                                      l_FileCreationTimeLocal.wDay, 
                                      l_FileCreationTimeLocal.wHour, 
                                      l_FileCreationTimeLocal.wMinute, 
                                      l_FileCreationTimeLocal.wSecond, 
                                      l_FileCreationTimeLocal.wMilliseconds  / 10) ) );
            }
         }

      } while(FindNextFile(l_DirSearchHandle, &l_DirItemData) );

      FindClose(l_DirSearchHandle);

      // Now, associate each server file with its client file
      for(hash_map<GString, GDateTime>::const_iterator it = m_ServerSaves.begin();
          it != m_ServerSaves.end();++ it)
      {
         if(m_ClientSaves.count(it->first) > 0)
         {
            // A client save exists, load it to know the client info
            EGameDataError::Enum l_eResult = 
               g_ClientDAL.DataManager()->LoadGame(g_Joshua.CurrentMod().m_sPath + c_sSaveGameLocation+ it->first + c_sSaveExtClient, c_iSP2GameType, c_iSP2DataVersion);

            if(l_eResult == EGameDataError::NoError)
            {
               m_vSaveList.push_back(make_pair(it->first, m_CurrentGameData) );
            }
         }
         else
         {
            // No client info available, build a "default" one
            GSaveGameData l_Data;
            l_Data.m_bInfoComplete = false;
            l_Data.m_iGameIsMp = 1;
            l_Data.m_iCountryID = 0;
            l_Data.m_pScenario = GSmartPtr<GGameScenario>(NULL);
            l_Data.m_SaveDate = it->second;

            m_vSaveList.push_back(make_pair(it->first, l_Data) );
         }
      }
   }
}

void GDataControlLayer::ResetServer()
{
   if(g_Joshua.Connected() )
   {
      // Send game reset request to server
      SDK::GGameEventSPtr l_ResetEvent = CREATE_GAME_EVENT(SP2::Event::GResetGame);
      l_ResetEvent->m_iSource = g_SP2Client->Id();
      l_ResetEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;

      g_Joshua.RaiseEvent(l_ResetEvent);
   }
}

void GDataControlLayer::LoadSavedGameOnServer()
{
   // Send load request to server
   SDK::GGameEventSPtr l_LoadEvent = CREATE_GAME_EVENT(SP2::Event::GLoadGame);
   l_LoadEvent->m_iSource = g_SP2Client->Id();
   l_LoadEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;

   Event::GLoadGame* l_pLoad = (Event::GLoadGame*) l_LoadEvent.get();
   l_pLoad->m_sName = m_sCurrentGame;

   g_Joshua.RaiseEvent(l_LoadEvent);
}

void GDataControlLayer::CreateNewGame()
{
   // Send new game creation request to server
   SDK::GGameEventSPtr l_NewEvent = CREATE_GAME_EVENT(SP2::Event::GNewGame);
   l_NewEvent->m_iSource = g_SP2Client->Id();
   l_NewEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;

   Event::GNewGame* l_pNew = (Event::GNewGame*) l_NewEvent.get();
   l_pNew->m_sDBFileName = c_sDatabaseLocation;
   l_pNew->m_GameOptions = g_ClientDAL.GameOptions();
   g_Joshua.RaiseEvent(l_NewEvent);
   m_eSaveLoadResult = (SDK::EGameDataError::Enum) -1;
}

void GDataControlLayer::SelectScenario(GSmartPtr<GGameScenario> in_pScenario, 
                                       UINT32 in_iCountrySelection,
                                       const GString& in_sPlayerName)
{
   // Send the player information to the server
   g_ClientDCL.SystemSendPlayerInformationToServer(in_iCountrySelection,
                                                   in_sPlayerName,
                                                   SDK::PLAYER_STATUS_READY);

   // Send the game event that sets the objectives
   SDK::GGameEventSPtr l_SetObjEvt = CREATE_GAME_EVENT(SP2::Event::GSetGameObjectives);
   l_SetObjEvt->m_iSource = g_Joshua.Client()->Id();
   l_SetObjEvt->m_iTarget = SDK::Event::ESpecialTargets::Server;
   SP2::Event::GSetGameObjectives* l_pSetGameObj = (SP2::Event::GSetGameObjectives*)l_SetObjEvt.get();

   list<GGameObjective*>::const_iterator  l_GameObjectiveIterator = in_pScenario->Objectives().begin();
   while(l_GameObjectiveIterator != in_pScenario->Objectives().end())
   {
      GGameObjective* l_pGameObjective = *l_GameObjectiveIterator;
      if(l_pGameObjective->m_fTimeOutTime > 0)
         l_pGameObjective->m_fTimeOutTime = g_Joshua.GameTime() + l_pGameObjective->m_fTimeOutTime * 365.25f;
      l_pSetGameObj->m_vObjectives.push_back(*(*l_GameObjectiveIterator));
      l_GameObjectiveIterator++;
   } 

   // Send them
   g_Joshua.RaiseEvent(l_SetObjEvt);

   // Set Scenario on the DAL
   g_ClientDAL.Scenario(in_pScenario);
}

void GDataControlLayer::RequestUsedUnit(void)
{

   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GRequestForSaleUnitList);
   SP2::Event::GRequestForSaleUnitList* l_pRequest = (SP2::Event::GRequestForSaleUnitList*) (l_pEvent.get() );

   // Set Target and Source for this event
   l_pRequest->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pRequest->m_iSource = g_SP2Client->Id();

   // Fire event
   g_Joshua.RaiseEvent(l_pEvent);	
}