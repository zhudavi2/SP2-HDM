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

#ifndef _GOLEM_EVENT_CELL_NEW_MISSION_H_
#define _GOLEM_EVENT_CELL_NEW_MISSION_H_

namespace SP2
{
   class GMilitaryEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GEventCellNewMission: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class GMilitaryEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         ECovertActionsMissionType::Enum       m_eType;
         ECovertActionsMissionComplexity::Enum m_eComplexity;
         ECovertActionsTargetSector::Enum      m_eSector;

         SP2::EResources::Enum                 m_eResourceType;
         SP2::EUnitCategory::Enum              m_eUnitCategory;

         INT32                                 m_iFramedCountry;

         UINT32                                m_iCellID;

      protected:
      private:
         GEventCellNewMission();
         ~GEventCellNewMission();
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_CELL_NEW_MISSION_H_
