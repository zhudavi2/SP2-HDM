/**************************************************************
*
* sp2_event_progress.h
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


#ifndef _GOLEM_EVENT_PROGRESS_H_
#define _GOLEM_EVENT_PROGRESS_H_

namespace SP2
{   
   namespace Event
   {
      class GProgress: public SDK::Event::GGameEvent
      {                  
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         UINT32 m_iProgressID;
         REAL32 m_fProgress;

      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_PROGRESS_H_
