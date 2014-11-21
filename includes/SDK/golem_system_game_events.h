/**************************************************************
*
* golem_system_game_events.h
*
* Description
* ===========
* Description: This file contains de definition and the implementation (.cpp)
*	of system game events that are needed to communicate between the different modules
*
* Owner
* =====
*  Mathieu Tremblay, Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef __GE_SYSTEM_H_
#define __GE_SYSTEM_H_

#include "golem_sdk_api.h"

namespace SDK
{
   class GJoshua;
   namespace Event
   {
      /*!
      * System game event
      *  Used to communicate some system commands to 
      *	different Actors.
      **/
      class GOLEM_SDK_API GSystemGameEvent: public GGameEvent
      {
      public:
         enum ESystemEventType
         {
            UNKNOWN		     = 0,
            TERMINATE_SERVER,
			   KICK_PLAYER,
            BAN_PLAYER,
            BANNED_PLAYER_LIST_COMMAND,
            DISCONNECT,
         };

         virtual bool Serialize(GIBuffer&)  const;
         virtual bool Unserialize(GOBuffer&);
         //! Gets/Sets the system game event type
         void			     SystemEventType(ESystemEventType);
         ESystemEventType SystemEventType() {return m_iSystemEventType;}

		   UINT32				m_iSystemEventData;
      private:
         friend class GJoshua;

         ESystemEventType	m_iSystemEventType;
		 

         static GGameEventSPtr New();

         //Constructor is private
         GSystemGameEvent();
         ~GSystemGameEvent();

      }; 
   }
}
#endif //__H_
