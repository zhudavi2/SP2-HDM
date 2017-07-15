/**************************************************************
*
* sp2_unit_production_event_handler.h
*
* Description
* ===========
*  Handle events related to the production of units, such as unit queue refresh, unit deployement etc.
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SP2_UNIT_PRODUCTION_EVENT_HANDLER_H
#define _GOLEM_SP2_UNIT_PRODUCTION_EVENT_HANDLER_H

namespace SP2
{
   /*!
   * Handles Unit production event on client side
   **/
   class GUnitProductionEventHandler
   {
   public:
      //! Received the unit prodction queue
      bool HandleUnitProductionQueueUpdate(SDK::GGameEventSPtr in_pEvent);

      //! Received the list of country that can build for you
      bool HandleUpdateForeignBuildingCountryList(SDK::GGameEventSPtr in_pEvent);

      //! Received the list of units that can be buy
      bool HandleUpdateForSaleUnitList(SDK::GGameEventSPtr in_pEvent);
   };

};//end namespace sp2

#endif //_GOLEM_SP2_UNIT_PRODUCTION_EVENT_HANDLER_H