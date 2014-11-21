/**************************************************************
*
* sp2_unit_production_event_handler.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

// Includes
#include "golem_pch.hpp"
#include "..\include\sp2_unit_production_event_handler.h"

bool GUnitProductionEventHandler::HandleBuildUnitOrder(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GBuildUnitOrder* l_pBuildUnitOrder = (SP2::Event::GBuildUnitOrder*)in_pEvent.get();

   // build our units
   g_ServerDCL.BuildUnits( l_pBuildUnitOrder->m_iBuildingCountryID,
                           l_pBuildUnitOrder->m_iDestinationCountryID,
                           l_pBuildUnitOrder->m_iDesignID,
                           l_pBuildUnitOrder->m_iQuantity,
                           l_pBuildUnitOrder->m_iProductionSpeed,
                           l_pBuildUnitOrder->m_ePriority);
   return true;
}

bool GUnitProductionEventHandler::HandleChangeOrder(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GChangeUnitOrder* l_pChangeUnitOrder = (SP2::Event::GChangeUnitOrder*)in_pEvent.get();

   // Do we need to cancel this order?
   if(l_pChangeUnitOrder->m_bCancelOrder)
   {
      // Get Country ID from client ID
      return g_ServerDCL.CancelUnitProduction(l_pChangeUnitOrder->m_iCountryId,l_pChangeUnitOrder->m_iCountryId,l_pChangeUnitOrder->m_iOrderId);
   }
   else
   {
      // Change order priority
      return g_ServerDCL.ChangeUnitProductionPriority(l_pChangeUnitOrder->m_iCountryId,l_pChangeUnitOrder->m_iOrderId,l_pChangeUnitOrder->m_eNewPriority);
   }
}

bool GUnitProductionEventHandler::HandleRequestProductionQueue(SDK::GGameEventSPtr in_pEvent)
{
   // Send Prodcution Queue to Client
   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayer(in_pEvent->m_iSource);
   if(!l_pPlayer)
      return false;
   
   g_ServerDCL.SendProductionQueueToCountry(l_pPlayer->ModID());
   return true;
}

bool GUnitProductionEventHandler::HandleDeployUnits(SDK::GGameEventSPtr in_pEvent)
{
   // Get Event
   SP2::Event::GDeployUnits* l_pDeployUnitsEvent= (SP2::Event::GDeployUnits*)in_pEvent.get();
   gassert(l_pDeployUnitsEvent,"Invalid null pointer in the event");

   // Get player
   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayer(in_pEvent->m_iSource);
   if(!l_pPlayer)
      return false;

   // Deploye players units.
   g_ServerDCL.DeployUnits(l_pPlayer->ModID(),l_pDeployUnitsEvent->m_UnitListToDeploy,l_pDeployUnitsEvent->m_DeployementCoordinate );
   
   return true;
}


bool GUnitProductionEventHandler::HandleSplitUnits(SDK::GGameEventSPtr in_pEvent)
{
   // Get Event
   SP2::Event::GSplitUnits* l_pSplitUnitsEvent= (SP2::Event::GSplitUnits*)in_pEvent.get();
   gassert(l_pSplitUnitsEvent,"Invalid null pointer in the event");

   
   // Get player to get country ID
   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayer(in_pEvent->m_iSource);
   if(!l_pPlayer)
      return false;

   // Get special production group
   SP2::GUnitGroupEx* l_pGroup = g_ServerDCL.UnitMover().ProductionQueueUnitGroups()[l_pPlayer->ModID()-1];
   gassert(l_pGroup,"There should be a unit production queue group where the units are to be placed when built");

   // Find units in the production group
   vector<SDK::Combat::GUnit*>::const_iterator l_UnitIterator = l_pGroup->Units().begin();
   while( (*l_UnitIterator)->Id() != l_pSplitUnitsEvent->m_iUnitID && l_UnitIterator !=  l_pGroup->Units().end())
   {
      l_UnitIterator++;
   }

   gassert(l_UnitIterator !=  l_pGroup->Units().end(),"Didn't find the unit to split, invalid unit Id");
   
   SP2::GUnit* l_pUnitToSplit = (SP2::GUnit* )( *l_UnitIterator);   

   g_ServerDCL.SplitUnit(l_pUnitToSplit, l_pSplitUnitsEvent->m_iQty, l_pGroup);


   return true;
}


bool GUnitProductionEventHandler::HandleCancelDeployement(SDK::GGameEventSPtr in_pEvent)
{
   // Get Event
   SP2::Event::GCancelDeployement* l_pCancelDeployementEvent= (SP2::Event::GCancelDeployement*)in_pEvent.get();
   gassert(l_pCancelDeployementEvent != NULL,"Invalid null pointer in the event");

   // Cancel the deployement all those groups.
   list<UINT32>::iterator l_GroupIdIterator = l_pCancelDeployementEvent->m_GroupIdListToCancel.begin();
   while(l_GroupIdIterator != l_pCancelDeployementEvent->m_GroupIdListToCancel.end())
   {
      g_ServerDCL.UnitMover().CancelDeployement((SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_GroupIdIterator));
      l_GroupIdIterator++;
   }

   // I guest we are all done.
   return true;
}

bool GUnitProductionEventHandler::HandleRequestBuildingCountryList(SDK::GGameEventSPtr in_pEvent)
{   
   // Get player to get country ID
   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayer(in_pEvent->m_iSource);
   if(!l_pPlayer)
      return false;

   // send it
   return g_ServerDCL.SendBuildingCountriesList(l_pPlayer->ModID());
}


bool GUnitProductionEventHandler::HandleGRegisterAsBuildingCountry(SDK::GGameEventSPtr in_pEvent)
{
	// Get Event
   SP2::Event::GRegisterAsBuildingCountry* l_pRegisteringEvent = (SP2::Event::GRegisterAsBuildingCountry*)in_pEvent.get();
   gassert(l_pRegisteringEvent != NULL,"Invalid null pointer in the event");

   // Find country ID
   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayer(in_pEvent->m_iSource);
   if(!l_pPlayer)
      return false;
   UINT32 l_iCountryId =  l_pPlayer->ModID();

	g_ServerDAL.CountryData(l_iCountryId)->AcceptForeignOrder(l_pRegisteringEvent->m_bAcceptForeignCountryOrder);
   
	return true;
	
}


bool GUnitProductionEventHandler::HandleTrainUnitRequest(SDK::GGameEventSPtr in_pEvent)
{
   // Get Event
   SP2::Event::GTrainUnitRequest* l_pTrainingRequest = (SP2::Event::GTrainUnitRequest*)in_pEvent.get();
   gassert(l_pTrainingRequest != NULL,"Invalid null pointer in the event");

   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayer(in_pEvent->m_iSource);
   if(!l_pPlayer)
      return false;
   UINT32 l_iCountryId =  l_pPlayer->ModID();

   if( l_pTrainingRequest->m_UnitIdListToTrain.size() )
   {
      bool l_bAllYourUnits = true;
      for(list<UINT32>::iterator l_It = l_pTrainingRequest->m_UnitIdListToTrain.begin(); 
          l_It != l_pTrainingRequest->m_UnitIdListToTrain.end(); l_It++)
      {
         SP2::GUnit* l_pUnit = (SP2::GUnit*)g_Joshua.UnitManager().Unit( *l_It );
         if(!l_pUnit)
            return false;

         if( l_iCountryId != ((SP2::GUnitGroup*)l_pUnit->Group())->OwnerId() )
         {
            l_bAllYourUnits = false;
            break;
         }
      }

      if(!l_bAllYourUnits)
         return false;

      // Start training those units
      g_ServerDCL.UnitMover().TrainUnits(l_pTrainingRequest->m_UnitIdListToTrain);
   }

   return true;
}

bool GUnitProductionEventHandler::HandleDisbandUnitRequest(SDK::GGameEventSPtr in_pEvent)
{
   // Get Event
   SP2::Event::GDisbandUnitRequest* l_pDisbandRequest = (SP2::Event::GDisbandUnitRequest*)in_pEvent.get();
   gassert(l_pDisbandRequest != NULL,"Invalid null pointer in the event");

   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayer(in_pEvent->m_iSource);
   if(!l_pPlayer)
      return false;

   list<UINT32>::iterator l_It = l_pDisbandRequest->m_UnitIdListToDisband.begin();
   while( l_It != l_pDisbandRequest->m_UnitIdListToDisband.end() )
   {
      //Disband those units
      SDK::Combat::GUnit* l_pUnit = g_Joshua.UnitManager().Unit(*l_It);
      if(l_pUnit)
         g_ServerDCL.DisbandUnits( l_pPlayer->ModID(), l_pUnit );
      
      l_It++;
   }
   
   return true;
}

bool GUnitProductionEventHandler::HandleRequestForSaleUnitList(SDK::GGameEventSPtr in_pEvent)
{
   SendForSellUnitList(in_pEvent->m_iSource);
   return true;
}

bool GUnitProductionEventHandler::HandleUnitTransfert(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GTransfertUnit* l_pUpdateEvent = (SP2::Event::GTransfertUnit*) (in_pEvent.get() );

   // Are we selling or buying a unit
   if(l_pUpdateEvent->m_bForSale)
   {
      g_ServerDCL.SellUnit(l_pUpdateEvent->m_iUnitId);
   }
   else
   {
      SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayer(in_pEvent->m_iSource);
      if(!l_pPlayer)
         return false;

      g_ServerDCL.BuyUnit((UINT16)l_pPlayer->ModID(),l_pUpdateEvent->m_iUnitId,l_pUpdateEvent->m_iQty);
   }

   // Syncrhonize military data
   g_SP2Server->SyncronizeMilitary(SDK::Event::ESpecialTargets::BroadcastHumanPlayers,g_Joshua.Clock() );

   // Send unit list to all client
   SendForSellUnitList(SDK::Event::ESpecialTargets::BroadcastActiveHumanPlayers);

   return true;
}

void  GUnitProductionEventHandler::SendForSellUnitList(INT32  in_iTargetId)
{
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GUpdateForSaleUnitList);
   SP2::Event::GUpdateForSaleUnitList* l_pUpdateEvent = (SP2::Event::GUpdateForSaleUnitList*) (l_pEvent.get() );

   // Set Target and Source for this event
   l_pUpdateEvent->m_iTarget = in_iTargetId;
   l_pUpdateEvent->m_iSource = SDK::Event::ESpecialTargets::Server;


   hash_set<UINT32>::const_iterator l_unitListIterator = g_ServerDCL.ForSaleUnitList().begin();
   
   while(l_unitListIterator != g_ServerDCL.ForSaleUnitList().end())
   {
      l_pUpdateEvent->m_ForSaleUnitIdList.push_back(*l_unitListIterator);
      l_unitListIterator++;
   }

   // Raise It
   g_Joshua.RaiseEvent(l_pEvent);	
}

void GUnitProductionEventHandler::HandleGroupSplit(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GEventGroupSplit* l_pUpdateEvent = (SP2::Event::GEventGroupSplit*) (in_pEvent.get() );

   // dont botter splitting a group with no units to split
   if(l_pUpdateEvent->m_UnitsToSplit.size() == 0)
      return;

   SP2::GUnitGroup* l_pOldGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup( l_pUpdateEvent->m_iGroupToSplit );
   gassert(l_pOldGroup, "Getting a group that do not exist");
   if(!l_pOldGroup)
      return;

   SP2::GUnitGroup* l_pNewGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().CreateUnitGroup();
   EMilitaryStatus::Enum l_eOldGroupStatus = l_pOldGroup->Status();
   GVector2D<REAL32>     l_OldGroupPosition = l_pOldGroup->Position();
   
   l_pNewGroup->Id( g_Joshua.UnitManager().NewUnitGroupID() );

   // Add the new group to the unit manager
   g_Joshua.UnitManager().AddUnitGroup(l_pNewGroup);
   l_pNewGroup->OwnerId(  l_pOldGroup->OwnerId() );

   for(hash_map<INT32, INT32>::iterator l_It = l_pUpdateEvent->m_UnitsToSplit.begin(); l_It != l_pUpdateEvent->m_UnitsToSplit.end(); l_It++)
   {
      SP2::GUnit* l_pUnit = (SP2::GUnit*)l_pOldGroup->Unit( l_It->first );
      
      g_ServerDCL.SplitUnit( l_pUnit, l_It->second, l_pNewGroup);
   }

   if( !l_pNewGroup->Units().size() )
      g_Joshua.UnitManager().DeleteUnitGroup( l_pNewGroup );
   else
   {
      l_pNewGroup->ChangeStatus( l_eOldGroupStatus );
      g_Joshua.UnitManager().SetGroupPosition( l_pNewGroup, l_OldGroupPosition );
   }
   
   // Add the new group to the unit mover
   g_ServerDCL.UnitMover().AddGroup(l_pNewGroup);


}