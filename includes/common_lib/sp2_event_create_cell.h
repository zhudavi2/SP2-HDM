/**************************************************************
*
* sp2_event_create_cell.h
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

#ifndef _GOLEM_EVENT_CREATE_CELL_H_
#define _GOLEM_EVENT_CREATE_CELL_H_

namespace SP2
{
   class GMilitaryEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GEventCellCreation: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class GMilitaryEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         GString              m_sName;
         INT16                m_iAssignedCountryID;
         ECovertActionCellTraining::Enum m_eTraining;

      protected:
      private:
         GEventCellCreation();
         ~GEventCellCreation();



      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_CREATE_CELL_H_
