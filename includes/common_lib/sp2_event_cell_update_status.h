/**************************************************************
*
* sp2_event_cell_update_status.h
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

#ifndef _GOLEM_EVENT_CELL_UPDATE_STATUS_H_
#define _GOLEM_EVENT_CELL_UPDATE_STATUS_H_

namespace SP2
{
   class GMilitaryEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GEventCellUpdateStatus: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class GMilitaryEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         INT32  m_iToCountryID;
         UINT32 m_iCellID;
         bool m_bToDormant;
         bool m_bToActive;
         bool m_bCancelMission;
         bool m_bEraseCell;
         bool m_bTrain;
         bool m_bStartMission;

      protected:
      private:
         GEventCellUpdateStatus();
         ~GEventCellUpdateStatus();
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_CELL_UPDATE_STATUS_H_
