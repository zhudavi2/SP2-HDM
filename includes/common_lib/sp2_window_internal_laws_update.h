/**************************************************************
*
* sp2_window_economic_health_update.h
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

#ifndef _GOLEM_EVENT_INTERNAL_LAWS_UPDATE_H_
#define _GOLEM_EVENT_INTERNAL_LAWS_UPDATE_H_

namespace SP2
{
   class GPoliticEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GInternalLawsUpdate: public SP2::Event::GRequestAckGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class SP2::GPoliticEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         vector<INT32>   m_viType;
         vector<INT32>   m_viStatus;
         vector<INT32>   m_viId;
         vector<REAL32>  m_vfPerc;
         vector<UINT8>   m_viGvtType;
         bool            m_vbLaws[EInternalLaws::ItemCount];

         REAL32 m_fEmigrationLevel;
         REAL32 m_fImmigrationLevel;

         INT16  m_iCountryId;
         INT16  m_iGvtType;
         bool   m_bGetData;
         bool   m_bEmigrationClosed; 
         bool   m_bImmigrationClosed;

      protected:
      private:
         GInternalLawsUpdate();
         ~GInternalLawsUpdate();
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_INTERNAL_LAWS_UPDATE_H_
