/**************************************************************
*
* sp2_treaty_join.h
*
* Description
* ===========
*  Everytime a new member joins a treaty, 
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_TREATY_JOIN_H_
#define _GOLEM_EVENT_TREATY_JOIN_H_

namespace SP2
{
   namespace Event
   {
      class GTreatyJoin: public SDK::Event::GGameEvent
      {                  
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

			UINT32 TreatyID;
			UINT32 NewMemberID;
         UINT32 Side;
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_TREATY_JOIN_H_