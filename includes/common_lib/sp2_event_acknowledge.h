/**************************************************************
*
* sp2_event_acknowledge.h
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


#ifndef _GOLEM_EVENT_ACKNOWLEDGE_H_
#define _GOLEM_EVENT_ACKNOWLEDGE_H_

namespace SP2
{   
   namespace Event
   {
      class GAcknowledge: public SDK::Event::GGameEvent
      {                  
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         UINT32 m_iEventID;
         bool   m_bAcknowledge;

      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_ACKNOWLEDGE_H_
