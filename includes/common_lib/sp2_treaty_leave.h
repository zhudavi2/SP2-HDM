/**************************************************************
*
* sp2_treaty_leave.h
*
* Description
* ===========
*  Game Event sent to all members of a treaty when a member is leaving
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_TREATY_LEAVE_H_
#define _GOLEM_EVENT_TREATY_LEAVE_H_

namespace SP2
{
   namespace Event
   {
      /*!
      * Event Sent from player to player when asking for a peace treaty.
		* It is sent back once a player has given its answer
      **/
      class GTreatyLeave: public SDK::Event::GGameEvent
      {                  
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

			UINT32 TreatyID;
			UINT32 OldMemberID;
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_TREATY_LEAVE_H_