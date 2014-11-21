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

/*!
 * Handles economic events on the Server
 **/
class GEconomicEventHandler 
{
public:
   GEconomicEventHandler();
   ~GEconomicEventHandler();

   //! Handles the event when a player modifies income taxes
   bool HandleUpdateEconomicHealth(SDK::GGameEventSPtr in_Event);
   bool HandleUpdateResources(SDK::GGameEventSPtr in_Event);
   bool HandleUpdateBudget(SDK::GGameEventSPtr in_Event);
   bool HandleEconomicWindowUpdate(SDK::GGameEventSPtr in_pEvent);
   bool HandleIncreaseProduction(SDK::GGameEventSPtr in_pEvent);
	bool HandleTradeOffer(SDK::GGameEventSPtr in_pEvent);
	bool HandleGetRegionValues(SDK::GGameEventSPtr in_pEvent);
   bool HandleGetTechnologiesValues(SDK::GGameEventSPtr in_pEvent);

protected:
private:

};

};//end namespace sp2

#endif //_GOLEM_ECONOMIC_EVENT_HANDLER_H_
