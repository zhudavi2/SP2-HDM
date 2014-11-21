/************************************************
*
*  golem_event.h
*
*  Event Manager
*
*  Owner : Mathieu Tremblay
*
************************************************/

#ifndef _GOLEM_EVENT_
#define _GOLEM_EVENT_


#include "golem_types.h"
#include "golem_thread.h"
#include "golem_datastruct.h"
#include "golem_mutex.h"

namespace Hector
{

/*************
* Time that the event Process function will sleep
* before checking again if there is some items in the
* event list (if the list is empty)
**************/
#define LIST_PROCESS_SLEEP_TIME   0.05


/***********
EEventType
- Define a list of event_types
***********/
enum EEventType
{
   e_EVT_UNDEFINED = 0,
   e_EVT_MOUSE_MOVE,
   e_EVT_MOUSE_DOWN,
   e_EVT_MOUSE_DBLCLICK,
   e_EVT_MOUSE_UP,
   e_EVT_MOUSE_WHEEL,
   e_EVT_KBD_KEYDOWN,
   e_EVT_KBD_KEYUP,
   e_EVT_KBD_CHAR,
   e_EVT_QUIT_PROGRAM,
   e_EVT_ON_SUSPEND,
   e_EVT_ON_RESUME,

   /************************************
   * The used defined events are defined here
   *  So to define a use defined event, a user
   *  would do a register event on
   *  e_EVT_USER_EVENT + X where X could be 0 to a whole lot
   *
   * If the user plan to use integers, the numbers used must be
   * typecasted to EEventType
   **************************************/
   e_EVT_USER_EVENT = 10000
};


/*****************
*  0 = less important event
*  99999999999 = more important event
*******************/
enum EEventPriority
{
   e_EP_USELESS        = 0    ,
   e_EP_NORMAL         = 100  ,
   e_EP_IMPORTANT      = 250  ,
   e_EP_CRITICAL       = 500  ,
   e_EP_URGENT         = 1000
};

/*****************************************************
* GEvent
*
*  Simple class to hold an Event (used to raise events)
*****************************************************/
class GEvent
{
   EEventType          m_type;
   GDynamicData*       m_data;
   EEventPriority      m_priority;
   UINT32              m_size ;     //data size
   UINT32              m_time; // time stamp of the event

public:
   /*!
   * Sets the time
   * @param The time of the event to set
   * @return NULL
   * @see CEvent_Manager
   **/
   void Time( UINT32 pTime );

   UINT32 Time();

   //To set the data we copy the data (given the size)
   //To the m_data variable
   void Data(GDynamicData* data);

   void Type(EEventType type);

   void Priority(EEventPriority priority);

   GDynamicData* Data(UINT32& size);

   EEventType  Type();

   EEventPriority Priority();

   GEvent();

   ~GEvent();

};


}

#endif

//EOF

