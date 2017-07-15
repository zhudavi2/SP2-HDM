/**************************************************************
*
* sp2_start_single_player_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_start_single_player_window.h"
#include "../include/sp2_generic_message_window.h"
#include "../../includes/common_lib//sp2_game_scenario.h"
#include "../include/sp2_country_profile.h"
#include "../include/sp2_party_selection_window.h"

const GString GStartSinglePlayerWindow::TypeName(L"Start_Single_Player");

GUI::GBaseObject* GStartSinglePlayerWindow::New()
{
	return (GUI::GBaseObject*) new GStartSinglePlayerWindow();
}


GStartSinglePlayerWindow::GStartSinglePlayerWindow() : GBaseWindow()
{
   m_pObjInnerFrm            = NULL;
   m_pObjCountryCbo          = NULL;
   m_pObjProfileBtn          = NULL;
   m_pObjPoliticalPartyBtn   = NULL;
   m_pObjStartGameBtn        = NULL;
   m_pObjCancelBtn           = NULL;
   m_pObjEconomicHumanTog    = NULL;
   m_pObjEconomicCpuTog      = NULL;
   m_pObjPoliticHumanTog     = NULL;
   m_pObjPoliticCpuTog       = NULL;
   m_pObjMilitaryHumanTog    = NULL;
   m_pObjMilitaryCpuTog      = NULL;
   m_pObjConquerWorldTog     = NULL;
   m_pObjWorldPeaceTog       = NULL;
   m_pObjBalanceResTog       = NULL;
   m_pObjDevelopNationTog    = NULL;
   m_pObjRaiseGDPTog         = NULL;
   m_pObjConquerWorldCbo     = NULL;
   m_pObjRaiseGDPCbo         = NULL;
   m_pObjWorldPeaceCbo       = NULL;
   m_pObjBalanceResCbo       = NULL;
   m_pObjDevelopNationCbo    = NULL;

   m_iSelectedPartyID = -1;

   m_bMustShowProfile = false;
   m_bMustShowParties = false;
}

GStartSinglePlayerWindow::~GStartSinglePlayerWindow()
{
}

bool GStartSinglePlayerWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm            = (GUI::GFrame*)Child(L"frmInner");

   m_pObjCountryCbo          = (SP2::GComboListBoxLess*)m_pObjInnerFrm->Child(L"frmCountrySelection")->Child(L"cboCountry");
   m_pObjProfileBtn          = (GUI::GButton*)m_pObjInnerFrm->Child(L"frmCountrySelection")->Child(L"btnProfile");
   m_pObjPoliticalPartyBtn   = (GUI::GButton*)m_pObjInnerFrm->Child(L"frmSphereControl")->Child(L"btnParty");
   m_pObjStartGameBtn        = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnStart");
   m_pObjCancelBtn           = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnCancel");

   m_pObjEconomicHumanTog    = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmSphereControl")->Child(L"frmControlEconomy")->Child(L"togHuman");
   m_pObjEconomicCpuTog      = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmSphereControl")->Child(L"frmControlEconomy")->Child(L"togComputer");
   m_pObjPoliticHumanTog     = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmSphereControl")->Child(L"frmControlPolitics")->Child(L"togHuman");
   m_pObjPoliticCpuTog       = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmSphereControl")->Child(L"frmControlPolitics")->Child(L"togComputer");
   m_pObjMilitaryHumanTog    = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmSphereControl")->Child(L"frmControlMilitary")->Child(L"togHuman");
   m_pObjMilitaryCpuTog      = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmSphereControl")->Child(L"frmControlMilitary")->Child(L"togComputer");

   m_pObjWorldPeaceTog       = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmGoals")->Child(L"frmTog1")->Child(L"togGoal");
   m_pObjBalanceResTog       = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmGoals")->Child(L"frmTog2")->Child(L"togGoal");
   m_pObjConquerWorldTog     = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmGoals")->Child(L"frmTog3")->Child(L"togGoal");
   m_pObjDevelopNationTog    = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmGoals")->Child(L"frmTog4")->Child(L"togGoal");
   m_pObjRaiseGDPTog         = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmGoals")->Child(L"frmTog5")->Child(L"togGoal");

   m_pObjWorldPeaceCbo       = (GUI::GComboBox*)m_pObjInnerFrm->Child(L"frmGoals")->Child(L"cboGoal1");
   m_pObjBalanceResCbo       = (GUI::GComboBox*)m_pObjInnerFrm->Child(L"frmGoals")->Child(L"cboGoal2");
   m_pObjConquerWorldCbo     = (GUI::GComboBox*)m_pObjInnerFrm->Child(L"frmGoals")->Child(L"cboGoal3");
   m_pObjDevelopNationCbo    = (GUI::GComboBox*)m_pObjInnerFrm->Child(L"frmGoals")->Child(L"cboGoal4");
   m_pObjRaiseGDPCbo         = (GUI::GComboBox*)m_pObjInnerFrm->Child(L"frmGoals")->Child(L"cboGoal5");

   m_pObjPartyName           = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmSphereControl")->Child(L"txtParty");

   //Hide unused buttons
   m_pObjLockTog->Hide();
   m_pObjApplyBtn->Hide();

   m_pObjWorldPeaceCbo    ->Enabled(false);
   m_pObjBalanceResCbo    ->Enabled(false);
   m_pObjConquerWorldCbo  ->Enabled(false); 
   m_pObjDevelopNationCbo ->Enabled(false);
   m_pObjRaiseGDPCbo      ->Enabled(false);
   m_pObjPoliticalPartyBtn->Enabled(false);

   FeedComboGoals(m_pObjWorldPeaceCbo);
   FeedComboGoals(m_pObjBalanceResCbo);
   FeedComboGoals(m_pObjConquerWorldCbo);
   FeedComboGoals(m_pObjDevelopNationCbo);
   FeedComboGoals(m_pObjRaiseGDPCbo);

   if(!g_ClientDDL.GenericMessageWindow())
      g_ClientDDL.GenericMessageWindowSpawn();

   if(!g_ClientDDL.CountryProfileWindow())
      g_ClientDDL.CountryProfileWindowSpawn();

   if(!g_ClientDDL.PartySelectionWindow())
      g_ClientDDL.PartySelectionWindowSpawn();

   RegisteredModalWindow.push_back(g_ClientDDL.GenericMessageWindow());
   RegisteredModalWindow.push_back(g_ClientDDL.CountryProfileWindow());
   RegisteredModalWindow.push_back(g_ClientDDL.PartySelectionWindow());

	return true;
}

bool GStartSinglePlayerWindow::OnDestroy()
{
   if(g_ClientDDL.CountryProfileWindow())
   {
      g_ClientDDL.CountryProfileWindowKill();
   }

   if(g_ClientDDL.PartySelectionWindow())
   {
      g_ClientDDL.PartySelectionWindowKill();   
   }

   RegisteredModalWindow.clear();

   return __super::OnDestroy();
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GStartSinglePlayerWindow::OnShow()
{
   __super::OnShow();

   m_pObjCountryCbo       ->Selected_Content(g_ClientDAL.GetString(EStrId::SelectACountry));
   m_pObjStartGameBtn     ->Enabled(false);
   m_pObjPoliticalPartyBtn->Enabled(false);
   m_pObjProfileBtn       ->Enabled(true);
}

GUI::EEventStatus::Enum GStartSinglePlayerWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjStartGameBtn)
         {
            // Create event that gives player info
            {
               // Get Selected Country
               GString l_sSelectedCountry = m_pObjCountryCbo->Selected_Content();
               if(l_sSelectedCountry != g_ClientDAL.GetString(EStrId::SelectACountry))
               {
                  const GCountry& l_country = g_ClientDAL.Country(l_sSelectedCountry);
                  //Send the player information to the server
                  g_ClientDCL.SystemSendPlayerInformationToServer(l_country.Id(),
                                                                  g_ClientDAL.GetString(102136),
                                                                  SDK::PLAYER_STATUS_READY,
                                                                  m_iSelectedPartyID);

                  //Send the game objectives to the player
                  SDK::GGameEventSPtr l_SetObjEvt = CREATE_GAME_EVENT(SP2::Event::GSetGameObjectives);
                  l_SetObjEvt->m_iSource = g_Joshua.Client()->Id();
                  l_SetObjEvt->m_iTarget = SDK::Event::ESpecialTargets::Server;
                  SP2::Event::GSetGameObjectives* l_pSetGameObj = (SP2::Event::GSetGameObjectives*)l_SetObjEvt.get();
                  l_pSetGameObj->m_vObjectives = g_ClientDCL.m_vGameObjectives;
                  g_Joshua.RaiseEvent(l_SetObjEvt);

                  
                  if(m_pObjEconomicCpuTog->ToggleState())
                     g_ClientDAL.m_eAdvisorStatusEconomic = EAdvisorStatus::Confirmation;
                  else
                     g_ClientDAL.m_eAdvisorStatusEconomic = EAdvisorStatus::Manual;

                  if(m_pObjPoliticCpuTog->ToggleState())
                     g_ClientDAL.m_eAdvisorStatusPolitic = EAdvisorStatus::Confirmation;
                  else
                     g_ClientDAL.m_eAdvisorStatusPolitic = EAdvisorStatus::Manual;
      
                  if(m_pObjMilitaryCpuTog->ToggleState())
                     g_ClientDAL.m_eAdvisorStatusMilitary = EAdvisorStatus::Confirmation;
                  else
                     g_ClientDAL.m_eAdvisorStatusMilitary = EAdvisorStatus::Manual;

                  m_pObjWorldPeaceCbo    ->Enabled(false);
                  m_pObjBalanceResCbo    ->Enabled(false);
                  m_pObjConquerWorldCbo  ->Enabled(false);
                  m_pObjDevelopNationCbo ->Enabled(false);
                  m_pObjRaiseGDPCbo      ->Enabled(false);                     
                  m_pObjPoliticalPartyBtn->Enabled(false);
                  m_pObjProfileBtn       ->Enabled(false);

                  m_pObjStartGameBtn->Enabled(false);
               }
               else
               {
                  if(!g_ClientDDL.GenericMessageWindow())
                     g_ClientDDL.GenericMessageWindowSpawn();

                  g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(EStrId::MustChooseCountryBeforeStartGame), this);
                  
                  return GUI::EEventStatus::Handled;
               }
            }
         }
         else if(in_pCaller == m_pObjExitBtn)
         {
            g_ClientDCL.CancelConnect();
         }
         else if(in_pCaller == m_pObjConquerWorldTog)
         {
            if(m_pObjConquerWorldTog->ToggleState())
            {
               m_pObjConquerWorldCbo->Enabled(true);
               AddGameObjective(EObjectiveType::ConquerTheWorld,0);
            }
            else
            {
               m_pObjConquerWorldCbo->Enabled(false);
               RemoveGameObjective(EObjectiveType::ConquerTheWorld);
            }
         }
         else if(in_pCaller == m_pObjRaiseGDPTog)
         {
            if(m_pObjRaiseGDPTog->ToggleState())
            {
               m_pObjRaiseGDPCbo->Enabled(true);
               AddGameObjective(EObjectiveType::RaiseGDPPerCapita,0,1.5f);
            }
            else
            {
               m_pObjRaiseGDPCbo->Enabled(false);
               RemoveGameObjective(EObjectiveType::RaiseGDPPerCapita);
            }
         }
         else if(in_pCaller == m_pObjWorldPeaceTog)
         {
            if(m_pObjWorldPeaceTog->ToggleState())
            {
               m_pObjWorldPeaceCbo->Enabled(true);
               AddGameObjective(EObjectiveType::AchieveWorldPeace,0);
            }
            else
            {
               m_pObjWorldPeaceCbo->Enabled(false);
               RemoveGameObjective(EObjectiveType::AchieveWorldPeace);
            }
         }
         else if(in_pCaller == m_pObjBalanceResTog)
         {
            if(m_pObjBalanceResTog->ToggleState())
            {
               m_pObjBalanceResCbo->Enabled(true);
               AddGameObjective(EObjectiveType::MeetResourcesNeeds,0);
            }
            else
            {
               m_pObjBalanceResCbo->Enabled(false);
               RemoveGameObjective(EObjectiveType::MeetResourcesNeeds);
            }
         }
         else if(in_pCaller == m_pObjDevelopNationTog)
         {
            if(m_pObjDevelopNationTog->ToggleState())
            {
               m_pObjDevelopNationCbo->Enabled(true);
               AddGameObjective(EObjectiveType::DevelopNation,0);
            }
            else
            {
               m_pObjDevelopNationCbo->Enabled(false);
               RemoveGameObjective(EObjectiveType::DevelopNation);
            }
         }
         else if(in_pCaller == m_pObjProfileBtn)
         {
            if(m_viCountriesID.size())
            {
               g_ClientDDL.CountryProfileWindow()->Show(this, m_viCountriesID, m_viEconomicRanks, m_viPoliticRanks, m_viMilitaryRanks);
            }
            else
            {
               m_bMustShowProfile = true;
               m_pObjProfileBtn->Enabled(false);
            }
         }
         else if(in_pCaller == m_pObjPoliticalPartyBtn)
         {
            if(!g_ClientDDL.PartySelectionWindow())
               g_ClientDDL.PartySelectionWindowSpawn();

            if( m_vParties.size() )
            {
               g_ClientDDL.PartySelectionWindow()->Show( m_vParties, m_iSelectedPartyID, this );
            }
            else
            {
               m_bMustShowParties = true;
               m_pObjPoliticalPartyBtn->Enabled(false);
            }
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

void GStartSinglePlayerWindow::AddGameObjective(EObjectiveType::Enum in_eObjectiveType,
                                                UINT32 in_fTimeLimitYears,
                                                REAL32 in_fValueToReach)
{
   GGameObjective l_Obj;
   l_Obj.m_eObjectiveType = in_eObjectiveType;
   l_Obj.m_fValueToReach  = in_fValueToReach;
   g_ClientDCL.m_vGameObjectives.push_back(l_Obj);
   UpdateObjective(in_eObjectiveType);
}

void GStartSinglePlayerWindow::RemoveGameObjective(EObjectiveType::Enum in_eObjectiveType)
{
   for(UINT32 i = 0 ; i < g_ClientDCL.m_vGameObjectives.size() ; i++)
   {
      if(g_ClientDCL.m_vGameObjectives[i].m_eObjectiveType == in_eObjectiveType)
      {
         g_ClientDCL.m_vGameObjectives.erase(g_ClientDCL.m_vGameObjectives.begin()+i);
         return;
      }
   }
   gassert(0,"GStartSinglePlayerWindow::RemoveGameObjective incoherence, asked to remove a non present Objectivetype");
}

/*!
 * Update the game objective in the list of the game objectives with the data from the interface
 **/
void GStartSinglePlayerWindow::UpdateObjective(EObjectiveType::Enum in_eType)
{
   //Get the time limit
   UINT32 l_iTimeLimit = 0;
   {
      GString l_sSelectedTime;
      switch(in_eType)
      {
         case EObjectiveType::RaiseGDPPerCapita:
         {
            l_sSelectedTime = m_pObjRaiseGDPCbo->Selected_Content();
            break;
         }
         case EObjectiveType::ConquerTheWorld:
         {
            l_sSelectedTime = m_pObjConquerWorldCbo->Selected_Content();
            break;
         }
         case EObjectiveType::AchieveWorldPeace:
         {
            l_sSelectedTime = m_pObjWorldPeaceCbo->Selected_Content();
            break;
         }
         case EObjectiveType::MeetResourcesNeeds:
         {
            l_sSelectedTime = m_pObjBalanceResCbo->Selected_Content();
            break;
         }
         case EObjectiveType::DevelopNation:
         {
            l_sSelectedTime = m_pObjDevelopNationCbo->Selected_Content();
            break;
         }
      }
      if(l_sSelectedTime == g_ClientDAL.GetString(EStrId::NoLimit))
      {
         l_iTimeLimit = 0;
      }
      else if(l_sSelectedTime == g_ClientDAL.GetString(EStrId::Goal1Year))
      {
         l_iTimeLimit = 1;
      }
      else if(l_sSelectedTime == g_ClientDAL.GetString(EStrId::Goal5Years))
      {
         l_iTimeLimit = 5;
      }
      else if(l_sSelectedTime == g_ClientDAL.GetString(EStrId::Goal10Years))
      {
         l_iTimeLimit = 10;
      }
   }
   //Update the objective with the new time limit
   for(UINT32 i = 0 ; i < g_ClientDCL.m_vGameObjectives.size() ; i++)
   {
      if(g_ClientDCL.m_vGameObjectives[i].m_eObjectiveType == in_eType)
      {
         if(l_iTimeLimit)
            g_ClientDCL.m_vGameObjectives[i].m_fTimeOutTime = g_Joshua.GameTime() + (l_iTimeLimit * 365.25f);
         else
            g_ClientDCL.m_vGameObjectives[i].m_fTimeOutTime = 0.f;
         return;
      }
   }
   gassert(0,"GStartSinglePlayerWindow::UpdateObjective incoherence, asked to update a non present Objectivetype");
   return;
}


GUI::EEventStatus::Enum GStartSinglePlayerWindow::OnCustomEvent(UINT32 in_iEventID, const GUI::GEventData& in_EventData, GUI::GBaseObject * in_pCaller)
{
   switch (in_iEventID)
   {
   case c_iOnPartySelection:
      {
         INT32 l_iPartyID = (INT32)in_EventData.Data;

         for(UINT32 i=0; i<m_vParties.size(); i++)
         {
            if(m_vParties[i].Id() == l_iPartyID)
            {
               m_iSelectedPartyID = m_vParties[i].Id();
               m_pObjPartyName->Text( g_ClientDAL.GetString( m_vParties[i].NameId() ) );
               break;
            }
         }
      }
      break;
   case c_iOnCountryProfileWindowSelection:
      {
         INT32 l_iCountryID = (INT32)in_EventData.Data;

         if(l_iCountryID > 0)
         {
            m_pObjCountryCbo->Selected_Content( g_ClientDAL.Country(l_iCountryID).Name() );

            SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GGetCountryParties);
            SP2::Event::GGetCountryParties* l_Evt = (SP2::Event::GGetCountryParties*)l_Event.get();
            l_Evt->m_iSource = g_Joshua.Client()->Id();
            l_Evt->m_iCountryId = g_ClientDAL.Country( m_pObjCountryCbo->Selected_Content() ).Id();
            l_Evt->m_iTarget = SDK::Event::ESpecialTargets::Server;
            g_Joshua.RaiseEvent(l_Event);   

            m_pObjStartGameBtn->Enabled(true);
            m_pObjPoliticalPartyBtn->Enabled(true);
         }
      }
      break;
      //If a combo box was selected
   case ECustomEvents::OnComboBoxBeforeOpen:
      {
         if(in_EventData.Data == m_pObjCountryCbo)
            g_ClientDDL.ComboboxSetCountriesAndFlag(g_ClientDAL.Countries(), (GUI::GComboBox*)in_EventData.Data, true);

         return GUI::EEventStatus::Handled;
      }
   case ECustomEvents::OnComboBoxSelectionEvent:
      {
         //Determine which combo box was selected and update the objective for that box
         GUI::GComboBox * l_pCombo = (GUI::GComboBox *)in_EventData.Data;
         gassert(l_pCombo, "GStartSinglePlayerWindow::OnCustomEvent Combo box pointer shouldnt be null on custom event");
         if(l_pCombo == m_pObjConquerWorldCbo)
         {
            UpdateObjective(EObjectiveType::ConquerTheWorld);
         }
         else if(l_pCombo == m_pObjRaiseGDPCbo)
         {
            UpdateObjective(EObjectiveType::RaiseGDPPerCapita);
         }
         else if(l_pCombo == m_pObjWorldPeaceCbo)
         {
            UpdateObjective(EObjectiveType::AchieveWorldPeace);
         }
         else if(l_pCombo == m_pObjBalanceResCbo)
         {
            UpdateObjective(EObjectiveType::MeetResourcesNeeds);
         }
         else if(l_pCombo == m_pObjDevelopNationCbo)
         {
            UpdateObjective(EObjectiveType::DevelopNation);
         }
         else if(l_pCombo == m_pObjCountryCbo)
         {
            if( m_pObjCountryCbo->Selected_Content() != g_ClientDAL.GetString(EStrId::SelectACountry))
            {
               SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GGetCountryParties);
               SP2::Event::GGetCountryParties* l_Evt = (SP2::Event::GGetCountryParties*)l_Event.get();
               l_Evt->m_iSource = g_Joshua.Client()->Id();
               l_Evt->m_iCountryId = g_ClientDAL.Country( m_pObjCountryCbo->Selected_Content() ).Id();
               l_Evt->m_iTarget = SDK::Event::ESpecialTargets::Server;
               g_Joshua.RaiseEvent(l_Event);   
               
               m_pObjStartGameBtn->Enabled(true);
               m_pObjPoliticalPartyBtn->Enabled(true);
            }
         }
         break;
      }
   default:
         break;
   }
   return EEventStatus::Handled;
}

void GStartSinglePlayerWindow::SetCountryRanks(const vector<INT16> & in_viMilitaryRanks, const vector<INT16> & in_viPoliticRanks, const vector<INT16> & in_viEconomicRanks, const vector<INT16> & in_viCountriesID)
{
   m_viEconomicRanks = in_viEconomicRanks;
   m_viMilitaryRanks = in_viMilitaryRanks;
   m_viPoliticRanks  = in_viPoliticRanks;
   m_viCountriesID   = in_viCountriesID;

   if(m_bMustShowProfile)
   {
      g_ClientDDL.CountryProfileWindow()->Show(this, m_viCountriesID, m_viEconomicRanks, m_viPoliticRanks, m_viMilitaryRanks);
      m_pObjProfileBtn->Enabled(true);
      m_bMustShowProfile = false;
   }
}

void GStartSinglePlayerWindow::SetPoliticalParties(const vector<GPoliticalParty> & in_vParties)
{
   m_vParties = in_vParties;

   for(UINT32 i=0; i<m_vParties.size(); i++)
   {
      if( m_vParties[i].InPower() )
      {
         m_iSelectedPartyID = m_vParties[i].Id();
         m_pObjPartyName->Text( g_ClientDAL.GetString( m_vParties[i].NameId() ) );
         break;
      }
   }

   if(m_bMustShowParties)
      g_ClientDDL.PartySelectionWindow()->Show( m_vParties, m_iSelectedPartyID, this );
}

void GStartSinglePlayerWindow::FeedComboGoals(GUI::GComboBox* in_pCbo)
{
   in_pCbo->Add_Item( g_ClientDAL.GetString(EStrId::NoLimit) );
   in_pCbo->Add_Item( g_ClientDAL.GetString(EStrId::Goal1Year) );
   in_pCbo->Add_Item( g_ClientDAL.GetString(EStrId::Goal5Years) );
   in_pCbo->Add_Item( g_ClientDAL.GetString(EStrId::Goal10Years) );
   in_pCbo->Selected_Content( g_ClientDAL.GetString(EStrId::NoLimit) );
}