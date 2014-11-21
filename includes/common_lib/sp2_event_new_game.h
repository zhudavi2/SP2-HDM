/**************************************************************
*
* sp2_event_new_game.h
*
* Description
* ===========
*  Event sent by client to indicate that it wants the server 
*  to start a new game.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_EVENT_NEW_GAME_H_
#define _SP2_EVENT_NEW_GAME_H_

namespace SP2
{   
   namespace Event
   {
      class GNewGame: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:
         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         GString                    m_sDBFileName;
         SP2::GGameOptions          m_GameOptions;
      };

      class GNewComplete : public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:
         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         GString                    m_sDBFileName;
         SDK::EGameDataError::Enum  m_eResult;
      };

   }

}

#endif // #ifndef _SP2_EVENT_NEW_GAME_H_
