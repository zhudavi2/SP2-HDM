/**************************************************************
*
* sp2_window_treaty_details_treaty_add.h
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

#ifndef _GOLEM_EVENT_TREATY_ADD_UPDATE_H_
#define _GOLEM_EVENT_TREATY_ADD_UPDATE_H_

namespace SP2
{
   class GPoliticEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GTreatyDetailsTreatyAdd: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class SP2::GPoliticEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         GTreaty m_Treaty;

      protected:
      private:
         GTreatyDetailsTreatyAdd();
         ~GTreatyDetailsTreatyAdd();
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_TREATY_ADD_UPDATE_H_
