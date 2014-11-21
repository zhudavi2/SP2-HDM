/**************************************************************
*
* sp2_unit_production_event_handler.cpp
*
* Description
* ===========
*  See .h
*  
* Owner
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/

// Includes
#include "golem_pch.hpp"
#include "..\include\sp2_unit_production_window.h"
#include "..\include\sp2_used_unit_window.h"


bool GUnitProductionEventHandler::HandleUnitProductionQueueUpdate(SDK::GGameEventSPtr in_pEvent)
{
   // Collect Event data
   SP2::Event::GUpdateUnitOrderStatus* l_pOrderStatus = (SP2::Event::GUpdateUnitOrderStatus*)in_pEvent.get();

   // Update produciton window, if it exist.
   if(g_ClientDDL.UnitProductionWindow() && g_ClientDDL.UnitProductionWindow()->Visible())
   {
      g_ClientDDL.UnitProductionWindow()->Capacity(l_pOrderStatus->m_fProductionCapacity);
      g_ClientDDL.UnitProductionWindow()->UpdateProductionStatus(l_pOrderStatus->m_ProductionStatusList,l_pOrderStatus->m_fProductionCapacity);
   }
   // We are done here.
   return true;
}

bool GUnitProductionEventHandler::HandleUpdateForeignBuildingCountryList(SDK::GGameEventSPtr in_pEvent)
{
   // Collect Event data
   SP2::Event::GUpdateForeignBuildingCountryList* l_pCountryListEvent = (SP2::Event::GUpdateForeignBuildingCountryList*)in_pEvent.get();

   // Update produciton window, if it exist.
   if(g_ClientDDL.UnitProductionWindow() && g_ClientDDL.UnitProductionWindow()->Visible())
   {
      // ensure that our country is in the list
      if(l_pCountryListEvent->m_iForeignBuildingCountryList.find(g_ClientDAL.ControlledCountryID()) == 
         l_pCountryListEvent->m_iForeignBuildingCountryList.end())
         l_pCountryListEvent->m_iForeignBuildingCountryList.insert(g_ClientDAL.ControlledCountryID());
      // Update the unit production window 
      g_ClientDDL.UnitProductionWindow()->UpdateBuildingCountry(l_pCountryListEvent->m_iForeignBuildingCountryList);
   }

   // We are done here.
   return true;
}

bool GUnitProductionEventHandler::HandleUpdateForSaleUnitList(SDK::GGameEventSPtr in_pEvent)
{
   // Collect Event data
   SP2::Event::GUpdateForSaleUnitList* l_pUnitListEvent = (SP2::Event::GUpdateForSaleUnitList*)in_pEvent.get();

   // Update Client side list
   g_ClientDAL.UnitsForSale().clear();

   // Iterate all units that are for sale, and find the one that we are selling, so that we don't display them in the deployement window
   list<UINT32>::iterator l_UnitIterator = l_pUnitListEvent->m_ForSaleUnitIdList.begin();
   while(l_UnitIterator != l_pUnitListEvent->m_ForSaleUnitIdList.end())
   {
      GUnit* l_pUnit  =(GUnit*) g_Joshua.UnitManager().Unit(*l_UnitIterator);

      if(l_pUnit)
      {
         if((UINT16)l_pUnit->Group()->OwnerId() == (UINT16)g_ClientDAL.ControlledCountryID())
            g_ClientDAL.UnitsForSale().push_back(*l_UnitIterator);
      }
      else
      {
         g_Joshua.Log("Trying to access an invalid unit", "HandleUpdateForSaleUnitList");
      }

      l_UnitIterator++;      
   };
   
   // Send new list to the Unit market window
   if(g_ClientDDL.UsedUnitWindow() && g_ClientDDL.UsedUnitWindow()->Visible() == true)
   {
      g_ClientDDL.UsedUnitWindow()->UpdateUnitsForSale(l_pUnitListEvent->m_ForSaleUnitIdList);
   }

   return true;
}