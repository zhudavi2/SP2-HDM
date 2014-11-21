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

#ifndef _GOLEM_EVENT_ECONOMIC_HEALTH_UPDATE_H_
#define _GOLEM_EVENT_ECONOMIC_HEALTH_UPDATE_H_


namespace SP2
{
   class GEconomicEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GEconomicHealthUpdate: public SP2::Event::GRequestAckGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class SP2::GEconomicEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         INT32  m_iCountryId;
         REAL64 m_fIncomeTax;
         REAL64 m_fInterestLevel;
         REAL64 m_fUnemploymentRate;
         REAL64 m_fPovertyRate;
         REAL64 m_fGdp;
         UINT64 m_iPopulation;
         REAL64 m_fInflation;
         REAL64 m_fInflationVar;

         bool   m_bGetData;

      protected:
      private:
         GEconomicHealthUpdate();
         ~GEconomicHealthUpdate();
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_ECONOMIC_HEALTH_UPDATE_H_
