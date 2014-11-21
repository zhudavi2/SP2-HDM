/**************************************************************
*
* sp2_event_military.h
*
* Description
* ===========
*  Describes the interface of class 
*  SP2::Event::GRequestMilitaryStatusChange, which asks the server to
*  change the status of a unit group.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_SP2_EVENT_MILITARY_H_
#define _GOLEM_SP2_EVENT_MILITARY_H_

namespace SP2
{
   namespace Event
   {

      /*!
       * Event sent from the client to the server which asks
       * for a unit status change.
       **/
      class GRequestMilitaryStatusChange : public SDK::Event::GGameEvent
      {
         friend SP2::GClient;
         friend SP2::GServer;
      public:
         //! New status for the specified group IDs
         EMilitaryStatus::Enum   m_eNewStatus;

         //! ID of the group that we want to change the status
         vector<UINT32>          m_vGroupIDs;

      protected:
         GRequestMilitaryStatusChange();

         //! GRequestMilitaryStatusChange construction function
         static SDK::GGameEventSPtr New();

         // Serialization functions
         bool Serialize(GIBuffer&) const;
         bool Unserialize(GOBuffer&);
      };

      class GRefuseMilitaryAction : public SDK::Event::GGameEvent
      {
         friend SP2::GClient;
         friend SP2::GServer;
      public:
         //! ID of the groups which has been refused movement & the reason
         vector<pair<UINT32, ERefusedMoveReason::Enum> > m_vRefusedMoves;

      protected:
         //! GRefuseMilitaryAction construction function
         static SDK::GGameEventSPtr New();

         // Serialization functions
         bool Serialize(GIBuffer&) const;
         bool Unserialize(GOBuffer&);
      };
   }
}

#endif // #ifndef _GOLEM_SP2_EVENT_MILITARY_H_
