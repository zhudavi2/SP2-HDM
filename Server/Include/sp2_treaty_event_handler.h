/**************************************************************
*
* sp2_treaty_event_handler.h
*
* Description
* ===========
*  Contains methods for all treaties events, server-side.
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_TREATY_EVENT_HANDLER_H_
#define _GOLEM_TREATY_EVENT_HANDLER_H_

namespace SP2
{

/*!
 * Handles Politic events on the Server
 **/
class GTreatyEventHandler 
{
public:
   GTreatyEventHandler();
   ~GTreatyEventHandler();   

   bool HandleTreatyJoin(SDK::GGameEventSPtr in_pEvent);
   bool HandleTreatyLeave(SDK::GGameEventSPtr in_pEvent);
   bool HandleConditionsCountries(SDK::GGameEventSPtr in_pEvent);
   
protected:
private:

};

};//end namespace sp2

#endif //_GOLEM_TREATY_EVENT_HANDLER_H_