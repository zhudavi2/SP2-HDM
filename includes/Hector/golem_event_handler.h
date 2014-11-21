/**************************************************************
*
* golem_event_handler.h
*
* Description
* ===========
*  Describes the interface of class GEventHandler
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_HANDLER_H_
#define _GOLEM_EVENT_HANDLER_H_

#include "golem_event.h"

namespace Hector
{

class GEventHandler
{
};

/********************
* Typedef the Function pointer -- to use as FUNC_PTR
*      FUNC_PTR is a pointer to a function 
*      member of a GEventHandler that receives a 
*      GData as an argument and returns nothing (void)
*******************/
typedef void (GEventHandler::*EVENT_HANDLING_FUNC_PTR)(GDynamicData*);

}

#endif //_GOLEM_EVENT_HANDLER_H_
