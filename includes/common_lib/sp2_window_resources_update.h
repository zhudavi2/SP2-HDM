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

#ifndef _GOLEM_EVENT_RESOURCES_UPDATE_H_
#define _GOLEM_EVENT_RESOURCES_UPDATE_H_

namespace SP2
{
   class GEconomicEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GResourcesUpdate: public SP2::Event::GRequestAckGameEvent
      {
      private:
         GResourcesUpdate();
         ~GResourcesUpdate();         
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class SP2::GEconomicEventHandler;

      public:
         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         INT32  m_iCountryID;

         INT32  m_iNbResources;
         INT32  m_iID[EResources::ItemCount];
         REAL64 m_fDemand[EResources::ItemCount];
         REAL64 m_fProduction[EResources::ItemCount];
         REAL64 m_fImport[EResources::ItemCount];
         REAL64 m_fExport[EResources::ItemCount];
         REAL64 m_fImportDesired[EResources::ItemCount];
         REAL64 m_fExportDesired[EResources::ItemCount];
         REAL64 m_fMarketAvailability[EResources::ItemCount];
			REAL64 m_fMarketShare[EResources::ItemCount];
         REAL32 m_fGdp[EResources::ItemCount];
         REAL32 m_fTaxes[EResources::ItemCount];
         REAL32 m_fGeneralTaxMod;
         bool   m_bMeetConsumption[EResources::ItemCount];
         bool   m_bGvtCtrl[EResources::ItemCount];
         bool   m_bLegal[EResources::ItemCount];
         
         bool   m_bGetData;


      protected:
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_RESOURCES_UPDATE_H_
