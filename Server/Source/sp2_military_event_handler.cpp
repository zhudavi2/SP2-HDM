/**************************************************************
*
* sp2_economic_event_handler.cpp
*
* Description
* ===========
*  Used to handle every economic events that happen
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#include "golem_pch.hpp"

using namespace SP2::Event;
using namespace SDK::Event;

GMilitaryEventHandler::GMilitaryEventHandler()
{
}

GMilitaryEventHandler::~GMilitaryEventHandler()
{
}

bool GMilitaryEventHandler::HandleStrategicWarfareLaunch(SDK::GGameEventSPtr in_Event)
{
   GStrategicWarfareLaunch* l_pWarfareLaunch = (GStrategicWarfareLaunch*)in_Event.get();

   g_ServerDCL.LaunchNuclearAttacks(l_pWarfareLaunch->m_vNukeLaunchRequests);

   return true;
}

/*!
 * Handle when a combat is over (do something)
 * @param in_Status: The status of the battle (aborted, over, etc.)
 * @param in_pArena: Pointer to the Arena which contains the finished fight
 * @return void
 **/
void GMilitaryEventHandler::HandleCombatOver(SDK::Combat::EBattleStatus::Enum  in_Status,
                                             SDK::Combat::GArena*        in_pArena)
{

   UINT32 l_iRegionID = g_ServerDCL.EarthLocateRegion(in_pArena->Info()->m_Coordinate.x, in_pArena->Info()->m_Coordinate.y);

   const set<UINT32>& l_Units = g_ServerDCL.UnitMover().UnitGroupsInsideRegion(l_iRegionID);
   set<UINT32>::const_iterator l_It;
   REAL64 l_fMaxValue = 0.f;
   UINT32 l_iMaxOwner = 0;
   for(l_It = l_Units.begin();l_It != l_Units.end();l_It ++)
   {
      SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*) g_Joshua.UnitManager().UnitGroup(*l_It);
      if(l_pGroup->Value() > l_fMaxValue)
      {
         l_fMaxValue = l_fMaxValue;
         l_iMaxOwner = l_pGroup->OwnerId();
      }
   }
   if(l_iMaxOwner)
   {
      g_ServerDCL.UpdateMilitaryControl(l_iRegionID, l_iMaxOwner);
   }


   //Notify everybody that the combat is over
   {
      SP2::GArena* l_pArena = (SP2::GArena*)in_pArena;
      SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GCombatOver);
      SP2::Event::GCombatOver* l_pCombatOverEvent = (SP2::Event::GCombatOver*)l_Event.get();
      l_pCombatOverEvent->m_ArenaInfo = *(SP2::GArenaInfo*)l_pArena->Info();
      l_pCombatOverEvent->m_iSource   = SDK::Event::ESpecialTargets::Server;
      l_pCombatOverEvent->m_iTarget   = SDK::Event::ESpecialTargets::BroadcastActiveHumanPlayers;
      g_Joshua.RaiseEvent(l_Event);
   }


   return;
}

//! Handles the event when a a unit is moved
bool GMilitaryEventHandler::HandleMoveUnit(SDK::GGameEventSPtr in_Event)
{
   //Set the path on several unit groups
   GRequestTroopsMove* l_pEvent = (GRequestTroopsMove*)in_Event.get();

   vector<SDK::Combat::GUnitGroup*> l_vGroups;

   if(!g_Joshua.ActivePlayer(l_pEvent->m_iSource) )
      return false;

   //Check if the player has the right to move all units
   UINT32 l_iCurPlayerID = g_Joshua.ActivePlayer(l_pEvent->m_iSource)->ModID();
   for(UINT32 i = 0;i < l_pEvent->UnitIDs.size();i ++)
   {
      SDK::Combat::GUnitGroup* l_pGroup = g_Joshua.UnitManager().UnitGroup(l_pEvent->UnitIDs[i] );
      if(l_pGroup)
      {
         if(l_iCurPlayerID != (INT32)l_pGroup->OwnerId() )
         {
            return false;
         }
         l_vGroups.push_back(l_pGroup);
      }
   }

   //Move the units
   if(!l_vGroups.size())
      return false;
   g_ServerDCL.UnitMover().MoveUnits(l_vGroups, l_pEvent->Destination, l_pEvent->Action);

   return true;
}

bool GMilitaryEventHandler::HandleStatusChange(SDK::GGameEventSPtr in_Event)
{
   // Change the status of the specified units
   GRequestMilitaryStatusChange* l_pEvent = (GRequestMilitaryStatusChange*)in_Event.get();

   if(!g_Joshua.ActivePlayer(l_pEvent->m_iSource) )
      return false;

   //Check if the player has the right to move all units
   UINT32 l_iCurPlayerID = g_Joshua.ActivePlayer(l_pEvent->m_iSource)->ModID();
   for(UINT32 i = 0;i < l_pEvent->m_vGroupIDs.size();i ++)
   {
      SDK::Combat::GUnitGroup* l_pGroup = g_Joshua.UnitManager().UnitGroup(l_pEvent->m_vGroupIDs[i] );
      if(l_pGroup)
      {
         if(l_iCurPlayerID == (INT32)l_pGroup->OwnerId() )
         {
            g_ServerDCL.UnitMover().ChangeUnitState(l_pGroup, l_pEvent->m_eNewStatus);
         }
      }
   }

   return true;
}

/*!
 * Handle add unit design when the client sent that game event
 **/
void GMilitaryEventHandler::HandleAddUnitDesign(SDK::GGameEventSPtr in_Event)
{
   GAddUnitDesign* l_pEvent = (GAddUnitDesign*)in_Event.get();

   g_ServerDCL.CreateUnitDesign(g_Joshua.UnitManager().NewUnitDesignID(),
                                l_pEvent->m_iDesignerId,
                                g_ServerDAL.UnitTypes()[l_pEvent->m_iUnitType - 1],
                                l_pEvent->m_sName,
										  -1,
                                l_pEvent->m_iSpeed,
                                l_pEvent->m_iSensors,
                                l_pEvent->m_iGunRange,
                                l_pEvent->m_iGunPrecision,
                                l_pEvent->m_iGunDamage,
                                l_pEvent->m_iMissilePayload,
                                l_pEvent->m_iMissileRange,
                                l_pEvent->m_iMissilePrecision,
                                l_pEvent->m_iMissileDamage,
                                l_pEvent->m_iStealth,
                                l_pEvent->m_iCountermeasures,
                                l_pEvent->m_iArmor,
                                l_pEvent->m_Variations.PieceIndex,
                                l_pEvent->m_Variations.ColorIndex
                                );
}

void GMilitaryEventHandler::HandleMilitaryMainUpdateRequest(SDK::GGameEventSPtr in_Event)
{
   GMilitaryMainUpdate* l_pEvent = (GMilitaryMainUpdate*)in_Event.get();

   if(l_pEvent->StartNuclearResearch)
   {
		g_ServerDCL.StartNuclearResearch(l_pEvent->CountryId);
   }

   if(l_pEvent->StartAmdsResearch)
   {
      g_ServerDCL.StartAMDSResearch(l_pEvent->CountryId);		
   }
}

void GMilitaryEventHandler::HandleBattleOverviewUnitActionUpdate(SDK::GGameEventSPtr in_Event)
{
   GBattleOverviewUnitActionUpdate* l_pEvent = (GBattleOverviewUnitActionUpdate*)in_Event.get();
   

   //Get the arena
   GArena* l_pArena = (SP2::GArena*)g_CombatManager.Arena(l_pEvent->m_iCombatId);
   

   if(!l_pArena)
   {
      g_Joshua.Log("Trying to modify a unit action in an invalid combat");
      return;
   }
   //Update the unit action
   for(UINT32 i=0;i<EUnitType::AircraftCarrier;i++)
   {
      l_pArena->UpdateUnitAction(l_pEvent->m_iCountryId,i+1,l_pEvent->m_bAttack[i]);
   }

   GArenaInfo* l_pArenaInfo = (GArenaInfo*)g_Joshua.UnitManager().ArenaInfo(l_pEvent->m_iCombatId);
   l_pArenaInfo->Actor(l_pEvent->m_iCountryId)->m_bRetreating = l_pEvent->m_bRetreat;

   
}

void GMilitaryEventHandler::HandleUnitResearchUpdateRequest(SDK::GGameEventSPtr in_Event)
{
   GUnitResearchUpdate* l_pEvent = (GUnitResearchUpdate*)in_Event.get();

   if(l_pEvent->m_iCountryId < 0)
      return;

   GCountryData* l_pData = g_ServerDAL.CountryData(l_pEvent->m_iCountryId);
   SP2::GResearchInformation* l_pInfo = (SP2::GResearchInformation*)l_pData->ResearchInfo();
   //l_Info.Update(g_Joshua.GameTime());

   if(!l_pEvent->SetData)
   {
      //Reverse source/target to send it back to client
      l_pEvent->m_iTarget = l_pEvent->m_iSource;
      l_pEvent->m_iSource = SDK::Event::ESpecialTargets::Server;

      l_pEvent->m_fBudgetAir     = l_pInfo->m_fBudgetAir;
      l_pEvent->m_fBudgetGround  = l_pInfo->m_fBudgetGround;
      l_pEvent->m_fBudgetNaval   = l_pInfo->m_fBudgetNaval;
      l_pEvent->m_fBudgetNuclear = l_pInfo->m_fBudgetNuclear;
      memcpy(l_pEvent->m_fBudget,   l_pInfo->m_fBudget,sizeof(REAL64)*EUnitCategory::ItemCount*EUnitDesignCharacteristics::ItemCount);
      memcpy(l_pEvent->m_fMaxValues,l_pInfo->m_fMaxValues,sizeof(REAL32)*EUnitCategory::ItemCount*EUnitDesignCharacteristics::ItemCount);     

      g_Joshua.RaiseEvent(in_Event);
   }
   else
   {
      l_pInfo->m_fBudgetAir     = l_pEvent->m_fBudgetAir;
      l_pInfo->m_fBudgetGround  = l_pEvent->m_fBudgetGround;
      l_pInfo->m_fBudgetNaval   = l_pEvent->m_fBudgetNaval;
      l_pInfo->m_fBudgetNuclear = l_pEvent->m_fBudgetNuclear;
      memcpy(l_pInfo->m_fBudget,   l_pEvent->m_fBudget,sizeof(REAL64)*EUnitCategory::ItemCount*EUnitDesignCharacteristics::ItemCount);

      // This event requires an acknowledgement !
      g_ServerDCL.AcknowledgeRequest(l_pEvent->m_iRequestID, l_pEvent->m_iSource);
   }
}

void GMilitaryEventHandler::HandleOneClickInvade(SDK::GGameEventSPtr in_Event)
{
   GOneClickInvade* l_pEvent = (GOneClickInvade*)in_Event.get();
	g_ServerDCL.OccupyCountry(l_pEvent->m_iOccupyingCountry,
									  l_pEvent->m_iOccupiedCountry,
									  l_pEvent->m_OtherCountriesToDeclareWar);   
}

void GMilitaryEventHandler::HandleCellCreate(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GEventCellCreation* l_pEvent = (SP2::Event::GEventCellCreation*)in_Event.get();

   //Can't be const because GPlayer::ModID() isn't const
   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayer(l_pEvent->m_iSource);
   if(!l_pPlayer )
      return;

   INT32 l_iCurPlayerID = l_pPlayer->ModID();

   //Handle name change
   static const GString c_sChangeNamePrefix("NAME ");
   if(l_pEvent->m_sName.find(c_sChangeNamePrefix) == 0)
   {
       gassert(l_iCurPlayerID >= 1,"Invalid player ID, name change won't work");

       const GString& l_sNewCountryName = l_pEvent->m_sName.substr(c_sChangeNamePrefix.length());

       //See if the new country's name is taken already. If yes, then cancel the name change.
       for(vector<GCountry>::const_iterator l_CountryIt = g_SP2Server->Countries().cbegin();
           l_CountryIt != g_SP2Server->Countries().cend();
           ++l_CountryIt)
       {
           if(l_CountryIt->Name() == l_sNewCountryName)
            return;
       }

       //Change the country's name
       //g_SP2Server->Countries() is 0-based
       GCountry& l_Country = g_SP2Server->Countries().at(l_iCurPlayerID - 1);

       const GString  l_sOldCountryName = l_Country.Name();

       g_ServerDAL.CountryData(l_iCurPlayerID)->Name(l_sNewCountryName);
       l_Country.Name(l_sNewCountryName);

       g_Joshua.Log(L"Player country ID " + GString(l_iCurPlayerID) + ", " +
           l_pPlayer->Name() + L", has changed its country's name from " +
           l_sOldCountryName + L" to " + l_sNewCountryName);

       const SDK::GPlayers& l_HumanPlayers = g_Joshua.HumanPlayers();
       for(SDK::GPlayers::const_iterator l_PlayerIt = l_HumanPlayers.cbegin();
           l_PlayerIt != l_HumanPlayers.cend(); 
           ++l_PlayerIt)
	   {
        if(l_PlayerIt->second->PlayerStatus() == SDK::PLAYER_STATUS_ACTIVE)
        {
            SDK::GGameEventSPtr l_RequestEvent = CREATE_GAME_EVENT(Event::GRequestCountryList);
            l_RequestEvent->m_iSource = l_PlayerIt->second->Id();
            l_RequestEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
            g_Joshua.RaiseEvent(l_RequestEvent);
        }
       }

       return;
   }
   
   GCountryData* l_pData = g_ServerDAL.CountryData(l_iCurPlayerID);

   INT32 l_iStartingCellNumber = -1;

   //Create 10 cells at a time if the given name was '-' followed immediately by a whole number.
   static const GString c_sMultipleCellsPrefix = L"-";
   if(l_pEvent->m_sName.find(c_sMultipleCellsPrefix) == 0)
   {
       const GString& l_sStartingCellName = l_pEvent->m_sName.substr(c_sMultipleCellsPrefix.length());
       l_iStartingCellNumber = l_sStartingCellName.ToINT32();
       gassert(l_iStartingCellNumber >= 1,"Invalid starting cell number for multiple cell creation");
   }

   for(INT32 i=l_iStartingCellNumber; i<l_iStartingCellNumber+10; i++)
   {
       SP2::GCovertActionCell l_Cell;

       l_Cell.OwnerID(l_iCurPlayerID);

       const GString& l_sCellName = (l_iStartingCellNumber == -1) ? l_pEvent->m_sName : GString(i);
       l_Cell.Name( l_sCellName );

       l_Cell.AssignedCountry( l_pEvent->m_iAssignedCountryID );
       l_Cell.ExperienceLevel( (REAL32)l_pEvent->m_eTraining );
       l_Cell.Initialize();

       l_pData->AddCovertActionCell(l_Cell);
       
       //If it's not the special case of creating multiple cells
       if(l_iStartingCellNumber == -1)
           break;
   }
}

void GMilitaryEventHandler::HandleUpdateWarState(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GEventUpdateWarState* l_pEvent = (SP2::Event::GEventUpdateWarState*)in_Event.get();

   if(!g_Joshua.ActivePlayer(l_pEvent->m_iSource) )
      return;

   UINT32 l_iCurPlayerID = g_Joshua.ActivePlayer(l_pEvent->m_iSource)->ModID();   

	if(l_pEvent->m_bJoinWar)
	{
		if(l_pEvent->m_bAttacker)
			g_ServerDCL.JoinAWar(l_iCurPlayerID,l_pEvent->m_iWarID,1,!l_pEvent->m_bWantPeace);
		else
			g_ServerDCL.JoinAWar(l_iCurPlayerID,l_pEvent->m_iWarID,2,!l_pEvent->m_bWantPeace);
	}
	else
	{
		g_ServerDCL.ChangeOpinionOnWar(l_iCurPlayerID,l_pEvent->m_iWarID,l_pEvent->m_bWantPeace);
	}

	return;
}

void GMilitaryEventHandler::HandleCellUpdate(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GEventCellUpdateStatus* l_pEvent = (SP2::Event::GEventCellUpdateStatus*)in_Event.get();
   if(!g_Joshua.ActivePlayer(l_pEvent->m_iSource))
      return;

   UINT32 l_iCurPlayerID = g_Joshua.ActivePlayer(l_pEvent->m_iSource)->ModID();   
   GCountryData* l_pData = g_ServerDAL.CountryData(l_iCurPlayerID);

   vector<GCovertActionCell> & l_Cells = l_pData->CovertActionCells();

   UINT32 i=0;
   bool l_bFound = false;
   for(i=0; i<l_Cells.size(); i++)
   {
      if(l_Cells[i].ID() == l_pEvent->m_iCellID)
	  {
		  l_bFound = true;
		  break;
	  }
   }

   if(!l_bFound)
   {
	   //asked for an event on an invalid cell, ignore the event
	   g_Joshua.Log("Tried to do an event on an invalid cell... ignoring event");
	   return;
   }


   if(l_pEvent->m_bToActive)
   {
      l_Cells[i].ChangeState(ECovertActionsCellState::GettingReady);     
      l_pData->m_bCovertActionCellsDirty = true;
   }
   else if(l_pEvent->m_bToDormant)
   {
      l_Cells[i].ChangeState(ECovertActionsCellState::Dormant);
      l_pData->m_bCovertActionCellsDirty = true;
   }
   else if(l_pEvent->m_iToCountryID)
   {
      l_Cells[i].NextAssignedCountry( l_pEvent->m_iToCountryID); 
      l_Cells[i].ChangeState(ECovertActionsCellState::InTransit);
      l_pData->m_bCovertActionCellsDirty = true;
   }
   else if(l_pEvent->m_bCancelMission)
   {
      l_Cells[i].CancelAction();
      l_pData->m_bCovertActionCellsDirty = true;
   }
   else if(l_pEvent->m_bEraseCell)
   {
      l_pData->RemoveCovertActionCell( l_Cells[i] );
		l_pData->NationalSecurity( g_ServerDCL.FindNationalSecurity(l_iCurPlayerID) );
		g_ServerDCL.IterateBudgetExpenseSecurity(l_iCurPlayerID);
   }
   else if(l_pEvent->m_bTrain)
   {
      if(l_Cells[i].ActualState() == ECovertActionsCellState::Active)
      {
         l_Cells[i].ChangeState(ECovertActionsCellState::Training);
      }
      else if(l_Cells[i].ActualState() == ECovertActionsCellState::Dormant)
      {
         l_Cells[i].ChangeState(ECovertActionsCellState::GettingReady);
         l_Cells[i].SubsequentStateAdd(ECovertActionsCellState::Training);
      }

      l_pData->m_bCovertActionCellsDirty = true;
   }
   else if(l_pEvent->m_bStartMission)
   {
      l_Cells[i].ChangeState(ECovertActionsCellState::Active);
      if(g_ServerDCL.ExecuteMission( l_Cells[i] ))
			l_pData->RemoveCovertActionCell(l_Cells[i]);
      l_pData->m_bCovertActionCellsDirty = true;
   }
}

void GMilitaryEventHandler::HandleNewMission(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GEventCellNewMission* l_pEvent = (SP2::Event::GEventCellNewMission*)in_Event.get();

   if(!g_Joshua.ActivePlayer(l_pEvent->m_iSource) )
      return;
   
   UINT32 l_iCurPlayerID = g_Joshua.ActivePlayer(l_pEvent->m_iSource)->ModID();   
   GCountryData* l_pData = g_ServerDAL.CountryData(l_iCurPlayerID);

   vector<GCovertActionCell> & l_Cells = l_pData->CovertActionCells();
  
   UINT32 i=0;
   bool l_bCellFound = false;
   for(i=0; i<l_Cells.size(); i++)
   {
      if(l_Cells[i].ID() == l_pEvent->m_iCellID)
      {
         l_bCellFound = true;
         break;
      }
   }

   gassert(l_bCellFound,"Cell should have been found");
   if(!l_bCellFound)
      return;

   l_Cells[i].TargetSector( l_pEvent->m_eSector );
   l_Cells[i].MissionComplexity( l_pEvent->m_eComplexity );
   l_Cells[i].CountryToFrame( l_pEvent->m_iFramedCountry );
   
   l_Cells[i].MissionType( l_pEvent->m_eType );
   
   if( l_pEvent->m_eSector == SP2::ECovertActionsTargetSector::Civilian )
      l_Cells[i].ResourceType(l_pEvent->m_eResourceType);
   else if( l_pEvent->m_eSector == SP2::ECovertActionsTargetSector::Military )
      l_Cells[i].UnitCategory(l_pEvent->m_eUnitCategory);

   if(l_Cells[i].ActualState() == ECovertActionsCellState::Dormant)
   {
      l_Cells[i].ChangeState(ECovertActionsCellState::GettingReady);
      l_Cells[i].SubsequentStateAdd(ECovertActionsCellState::PreparingMission);
   }
   else if(l_Cells[i].ActualState() == ECovertActionsCellState::Active)
   {
      l_Cells[i].ChangeState(ECovertActionsCellState::PreparingMission);
   }

   l_pData->m_bCovertActionCellsDirty = true;
}

/*!
 * Notification received when the subject change, (actually the unit manager)
 **/
void GMilitaryEventHandler::OnSubjectChange(DesignPattern::GSubject&  in_Subject,const DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg  )
{
   if(&in_Subject == &g_Joshua.UnitManager())
   {
      if(in_Notification == SDK::Combat::c_notificationBeforeUnitKilled)
      {                
         UINT32 l_iUnitID = (UINT32)in_pAdditionalMsg;
         SP2::GUnit* l_pUnit = (SP2::GUnit*)g_Joshua.UnitManager().Unit(l_iUnitID);
         gassert(l_pUnit,"GMilitaryEventHandler::OnSubjectChange Notification of a killed unit that does not exist");
         SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)l_pUnit->Design();
         if(l_pDesign->Type()->Id() == c_iNuclearSubmarineUnitTypeID)
         {
            UINT32 l_iUnitOwner = l_pUnit->Group()->OwnerId();
            g_ServerDAL.CountryData(l_iUnitOwner)->RemoveMissilesForNuclearSubmarine(l_iUnitID);   
         }        
      }
   }
}

/*!
 * Offshore bombardment handler
 **/
void GMilitaryEventHandler::HandlePerformOffshoreBombardment(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GPerformOffshoreBombardment* l_pEvt = (SP2::Event::GPerformOffshoreBombardment*)in_Event.get();

   //Fetch the pointers to the unit group
   vector<SDK::Combat::GUnitGroup*> l_vGroups;
   for(set<UINT32>::const_iterator l_It = l_pEvt->m_vBombardingUnitGroups.begin();
      l_It != l_pEvt->m_vBombardingUnitGroups.end();
      l_It++)
   {
      SDK::Combat::GUnitGroup* l_pGroup = g_Joshua.UnitManager().UnitGroup(*l_It);
      gassert(l_pGroup,"Invalid NULL pointer in a group to bombard");
      if(l_pGroup)
      {
         l_vGroups.push_back(l_pGroup);
      }
   }   
   g_ServerDCL.NavalUnitsBombardRegion(l_vGroups,l_pEvt->m_iTargetRegion);
}
