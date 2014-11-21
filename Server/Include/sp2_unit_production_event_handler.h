/**************************************************************
*
* sp2_unit_production_event_handler.h
*
* Description
* ===========
*  Used to receive event from the client that target the unit productor.
*  This handler, then send commands to the unit productor.
*
* Owner
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/

// Includes guards
#ifndef _SP2_UNIT_PRODUCTION_EVENT_HANDLER_
#define _SP2_UNIT_PRODUCTION_EVENT_HANDLER_

namespace SP2
{

/*!
 * Handles Unit production events on the Server side
 **/
class GUnitProductionEventHandler 
{
public:

   //! Request a new unit to be build
   bool HandleBuildUnitOrder(SDK::GGameEventSPtr in_pEvent);

   //! Change the priority or cancel an order
   bool HandleChangeOrder(SDK::GGameEventSPtr in_pEvent);

   //! Request the production queue
   bool HandleRequestProductionQueue(SDK::GGameEventSPtr in_pEvent);

   //! Deploy new units
   bool HandleDeployUnits(SDK::GGameEventSPtr in_pEvent);

   //! Split a units into smaller quantity
   bool HandleSplitUnits(SDK::GGameEventSPtr in_pEvent);

   //! Handle a cancelation of a deployement
   bool HandleCancelDeployement(SDK::GGameEventSPtr in_pEvent);

   //! Handle request to be updated for the list of bulding country
   bool HandleRequestBuildingCountryList(SDK::GGameEventSPtr in_pEvent);

   //! Handle country registering as being capable of building for other country
   bool HandleGRegisterAsBuildingCountry(SDK::GGameEventSPtr in_pEvent);

   //! Handle Training request
   bool HandleTrainUnitRequest(SDK::GGameEventSPtr in_pEvent);

   //! Handle Disband request
   bool HandleDisbandUnitRequest(SDK::GGameEventSPtr in_pEvent);

   //! Handle Request ForSaleUnit List
   bool HandleRequestForSaleUnitList(SDK::GGameEventSPtr in_pEvent);

   //! Handle units transfert between buy and sell on the unit market
   bool HandleUnitTransfert(SDK::GGameEventSPtr in_pEvent);

   //! Send the unit list to the target
   void SendForSellUnitList(INT32  in_iTargetId);

   void HandleGroupSplit(SDK::GGameEventSPtr in_pEvent);

};

};//end namespace sp2

#endif //_SP2_UNIT_PRODUCTION_EVENT_HANDLER_
