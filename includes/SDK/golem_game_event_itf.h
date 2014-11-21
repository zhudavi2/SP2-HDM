#ifndef _GOLEM_GAME_EVENT__H
#define _GOLEM_GAME_EVENT__H

#include "golem_sdk_api.h"


namespace SDK
{
   class GJoshua;
   class GGenericNetworkClient;

   namespace Event
   {
      namespace ESpecialTargets 
      { 
         enum Enum 
         {
            InvalidTarget                    = 0xFFFFFFFF,
            SpecialTarget_Flag               = 0xF0000000,
            Server_Flag                      = 0x1000,
            Broadcast_Flag                   = 0x4000,
            Server                           = SpecialTarget_Flag | Server_Flag,
            BroadcastActivePlayers           = SpecialTarget_Flag | Broadcast_Flag | 0x01,
            BroadcastPlayers                 = SpecialTarget_Flag | Broadcast_Flag | 0x02,
            BroadcastIdleHumanPlayers        = SpecialTarget_Flag | Broadcast_Flag | 0x03,
            BroadcastActiveHumanPlayers      = SpecialTarget_Flag | Broadcast_Flag | 0x04,
            BroadcastHumanPlayers            = SpecialTarget_Flag | Broadcast_Flag | 0x05,
            BroadcastAIPlayers               = SpecialTarget_Flag | Broadcast_Flag | 0x06,
         };
      }

     /*!
      * Defines a generic game event, an interface to communicate between the different
      * actors in the game (server, players).
      *  To create a game event, a User must create a new class and inherit from this one.
      *	It then define the pure virtual functions so the effect of the game event customized.
      **/
      class GOLEM_SDK_API GGameEvent: public Hector::GSerializable
      {
         friend class GGenericNetworkClient;
         friend class GJoshua;

         static GSmartPtr<GGameEvent> New();

      public:
         INT64  m_iWhen;
         //!Who sent the game event
         INT32  m_iSource; 
         //!To who is the event destinated
         INT32  m_iTarget;
         INT32  m_iSpecificTarget;

         INT32 Type() const {return m_iType;}

         //!Joshua time value when the game event entered the queue
         REAL64 m_fTimeAtReception;

         //! if true, this event will be compress over the network
         bool  m_bCompressEvent;

         //! Serializes the game event so it can be sent via the network.
         virtual bool Serialize(GIBuffer & io_Obj) const;
         //! Deserializes the game event from raw data received by the network.
         virtual bool Unserialize(GOBuffer & io_Obj);
         virtual bool IsDone() {return true;};
         virtual ~GGameEvent(){} //destructor is virtual
      protected:
         GGameEvent();		


      private:
         //! Game event type, used only by Joshua to create / uniquely identify 
         //! different game events
         INT32 m_iType;
      };
   }

   //Smart pointer to a GGameEvent
   typedef GSmartPtr<Event::GGameEvent> GGameEventSPtr; 
   //!Pointer to a function that creates a Game_Event
   typedef GGameEventSPtr (*CREATE_GAME_EVENT_FUNC)();
}

typedef bool (GCallbackHandlerItf::*CALLBACK_HANDLER_bool_spGE)(SDK::GGameEventSPtr);
#endif
