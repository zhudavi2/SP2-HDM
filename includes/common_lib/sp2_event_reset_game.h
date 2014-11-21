/**************************************************************
*
* sp2_event_reset_game.h
*
* Description
* ===========
*  Event sent by client to indicate that it wants the server 
*  to start a Reset game.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_EVENT_RESET_GAME_H_
#define _SP2_EVENT_RESET_GAME_H_

namespace SP2
{   
   namespace Event
   {
      class GResetGame: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
      };

   }

}

#endif // #ifndef _SP2_EVENT_RESET_GAME_H_
