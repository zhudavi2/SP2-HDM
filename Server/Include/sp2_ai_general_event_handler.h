/**************************************************************
*
* golem_aigeneraleventhandler.h
*
* Description
* ===========
*  Describes the interface of class GAIGeneralEventHandler
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_AIGENERALEVENTHANDLER_H_
#define _GOLEM_AIGENERALEVENTHANDLER_H_

namespace SP2
{

/*!
 * General event handler for the AI
 **/
class GAIGeneralEventHandler
{
public:
   GAIGeneralEventHandler();
   ~GAIGeneralEventHandler();

   bool HandleNukeLaunchNotify(SDK::GGameEventSPtr in_Event);
	bool HandleWarDeclaration(SDK::GGameEventSPtr in_Event);
	bool HandleTreatyOffer(SDK::GGameEventSPtr in_Event);
	bool HandleTreatyJoin(SDK::GGameEventSPtr in_Event);
	bool HandleTreatyLeave(SDK::GGameEventSPtr in_Event);
	bool HandleTreatyPeaceSigned(SDK::GGameEventSPtr in_Event);	
   
};

}
#endif //_GOLEM_AIGENERALEVENTHANDLER_H_
