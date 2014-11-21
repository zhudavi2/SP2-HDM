/**************************************************************
*
* golem_event_manager.h
*
* Description
* ===========
*  Describes the interface of class GEventManager
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_MANAGER_H_
#define _GOLEM_EVENT_MANAGER_H_

#include "golem_event.h"
#include "golem_event_handler.h"
#include "golem_registered_event.h"

namespace Hector
{


/******************************************************
*
*  GEventManager
*  
*      Define a GEventManager class that has a event queue
*      Events must be registered and raised.
*      The GEventManager runs in a separate thread that Process
*      the event queue with an algorithm that Process events
*      in order or priority
****************************************************/
class GEventManager
{
public:
   GEventManager();
   ~GEventManager();

   bool RegisterEvent(EEventType event,EVENT_HANDLING_FUNC_PTR funcPtr,void* thisPtr);
   bool RaiseEvent(GEvent* event);
   bool RaiseEvent(EEventType eventType,GDynamicData* data,EEventPriority priority);

   bool RaiseEvent(EEventType eventType,GDynamicData* data);
   bool RaiseEvent(EEventType eventType);

private:
   void ReportError(char* string);
   void CallHandlerFunction(void* thisPtr,GDynamicData* data,EVENT_HANDLING_FUNC_PTR func); 

   vector<GRegisteredEvent> m_registeredEvtList; //List of registered events
};

}


#endif //_GOLEM_EVENT_MANAGER_H_
