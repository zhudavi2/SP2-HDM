/**************************************************************
*
* sp2_event_update_war_state.h
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

#ifndef _GOLEM_EVENT_UPDATE_WAR_STATE_H_
#define _GOLEM_EVENT_UPDATE_WAR_STATE_H_

namespace SP2
{
   class GMilitaryEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GEventUpdateWarState: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class GMilitaryEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

			INT32 m_iWarID;
			bool  m_bJoinWar;
			bool  m_bAttacker;
			bool  m_bWantPeace;

		protected:
      private:
         GEventUpdateWarState();
         ~GEventUpdateWarState();
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_UPDATE_WAR_STATE_H_
