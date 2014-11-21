/**************************************************************
*
* golem_registered_event.h
*
* Description
* ===========
*  Describes the interface of class GRegisteredEvent
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_REGISTERED_EVENT_H_
#define _GOLEM_REGISTERED_EVENT_H_

#include "golem_event.h"

namespace Hector
{

   /*******************************
   * GRegisteredEvent
   *      Represents a class to hold registered events
   *      and the function pointer corresponding to that
   *      event.
   *  When the event is raised, the function (identified with
   *  the function pointer) is called.
   *
   *  The GEventManager holds a list of registered events
   *  so when an event is raised, it processes the list of registered
   *  events to call the correct function
   ******************************/
   class GRegisteredEvent
   {
   public:
      GRegisteredEvent();
      ~GRegisteredEvent();

      //Set
      void FuncPtr(EVENT_HANDLING_FUNC_PTR funcPtr);
      void EventType(EEventType type);
      void ThisPtr(void* ptr);

      //Get
      EVENT_HANDLING_FUNC_PTR    FuncPtr();
      EEventType  EventType();
      void*       ThisPtr();

   private:
      EVENT_HANDLING_FUNC_PTR    m_func;
      EEventType  m_type;
      void*       m_thisPtr;
   };

}

#endif //_GOLEM_REGISTERED_EVENT_H_
