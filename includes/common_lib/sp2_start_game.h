/**************************************************************
*
* golem_eventstartgame.h
*
* Description
* ===========
*  Game events that have something to do with starting the game
*  (requesting to start the game and starting it)
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENTSTARTGAME_H_
#define _GOLEM_EVENTSTARTGAME_H_

namespace SP2
{
   class GGeneralEventHandler;
   class GClient;
   class GServer;

   namespace Event
   {
      /*!
       * Event Sent from the Admin player to the server to request the 
       * beginning of the game
       **/
      class GStartGameRequest: public SDK::Event::GGameEvent
      {  
         friend SP2::GClient;
         friend SP2::GServer;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

      protected:
      private:
         static SDK::GGameEventSPtr New();
         GStartGameRequest();
         ~GStartGameRequest();
      };     


      /*!
       * Event sent from the server to indicate the actual game start to a client
       **/
      class GStartGame: public SDK::Event::GGameEvent
      {
         friend class GGeneralEventHandler;
         friend SP2::GClient;
         friend SP2::GServer;
      public:

         // Class SDK::Event::GGameEvent implementation
         bool Serialize(GIBuffer&) const;
         bool Unserialize(GOBuffer&);

         INT64  StartDate;          //!< Date at game time 0.

      protected:
      private:
         static SDK::GGameEventSPtr New();
      };      

   } //end namespace Event

} //end namespace SP2

#endif //_GOLEM_EVENTSTARTGAME_H_
