/**************************************************************
*
* sp2_event_load_game.h
*
* Description
* ===========
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/


#ifndef _GOLEM_EVENT_LOAD_GAME_H_
#define _GOLEM_EVENT_LOAD_GAME_H_

namespace SP2
{   
   namespace Event
   {
      class GLoadGame: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         GString  m_sName;
      };

      class GLoadComplete : public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         GString                    m_sName;
         SDK::EGameDataError::Enum  m_eResult;
      };

   }

}

#endif //_GOLEM_EVENT_LOAD_GAME_H_
