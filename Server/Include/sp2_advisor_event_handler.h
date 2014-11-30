/**************************************************************
*
* sp2_advisor_event_handler.h
*
* Description
* ===========
*  Handles advisor events
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_ADVISOREVENTHANDLER_H_
#define _GOLEM_ADVISOREVENTHANDLER_H_

namespace SP2
{

class SP2::Event::GSendPlan;

/*!
 * Handles the advisor events sent by the client
 **/
class GAdvisorEventHandler 
{
public:
   GAdvisorEventHandler( );
   ~GAdvisorEventHandler();

   void HandleAdvisorAnswer(SDK::GGameEventSPtr in_Event);

protected:
private:
	void HandleEHEPlan(SP2::Event::GSendPlan* in_pPlan);
	void HandleTooManyMilitaryUnits(SP2::Event::GSendPlan* in_pPlan);

};

};//end namespace sp2

#endif //_GOLEM_ADVISOREVENTHANDLER_H_