/**************************************************************
*
* sp2_economic_event_handler.h
*
* Description
* ===========
*  Describes the interface of class GEconomicEventHandler
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_ECONOMIC_EVENT_HANDLER_H_
#define _GOLEM_ECONOMIC_EVENT_HANDLER_H_


namespace SP2
{

   CWSTR GE_ECO_ADJUST_INCOME_TAX = L"ECO_ADJUST_INCOME_TAX";


   /*!
   * Handles economic events on Client side
   **/
   class GEconomicEventHandler 
   {
   public:
      GEconomicEventHandler();
      ~GEconomicEventHandler();

      bool HandleEconomicWindowUpdate(SDK::GGameEventSPtr in_pEvent);
      bool HandleEconomicHealthWindowUpdate(SDK::GGameEventSPtr in_pEvent);
      bool HandleResourcesWindowUpdate(SDK::GGameEventSPtr in_pEvent);
      bool HandleRegionsValuesUpdate(SDK::GGameEventSPtr in_pEvent);
      bool HandleTechnologiesValuesUpdate(SDK::GGameEventSPtr in_pEvent);
      bool HandleRegionsTradeProposal(SDK::GGameEventSPtr in_pEvent);

   protected:
   private:

   };

};//end namespace sp2

#endif //_GOLEM_ECONOMIC_EVENT_HANDLER_H_
