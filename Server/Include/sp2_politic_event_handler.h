/**************************************************************
*
* sp2_Politic_event_handler.h
*
* Description
* ===========
*  Describes the interface of class GPoliticEventHandler
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_Politic_EVENT_HANDLER_H_
#define _GOLEM_Politic_EVENT_HANDLER_H_

namespace SP2
{

/*!
 * Handles Politic events on the Server
 **/
class GPoliticEventHandler 
{
public:
   GPoliticEventHandler();
   ~GPoliticEventHandler();

   bool HandlePoliticWindowUpdate(SDK::GGameEventSPtr in_pEvent);
   bool HandleUpdateConstitutionalForm(SDK::GGameEventSPtr in_Event);
   bool HandleUpdateInternalLaws(SDK::GGameEventSPtr in_Event);
   bool HandleTreatyAdd(SDK::GGameEventSPtr in_Event);
protected:
private:

};

};//end namespace sp2

#endif //_GOLEM_Politic_EVENT_HANDLER_H_
