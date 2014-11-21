/**************************************************************
*
* sp2_event_group_split.h
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

#ifndef _GOLEM_EVENT_GROUP_SPLIT_H_
#define _GOLEM_EVENT_GROUP_SPLIT_H_

namespace SP2
{
   class GMilitaryEventHandler;
   namespace Event
   {
      class GEventGroupSplit: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class GMilitaryEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         INT32 m_iGroupToSplit;
         //       UnitID, Qty
         hash_map< INT32, INT32 > m_UnitsToSplit;
      protected:
      private:
         GEventGroupSplit();
         ~GEventGroupSplit();

      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_GROUP_SPLIT_H_
