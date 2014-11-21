/**************************************************************
*
* sp2_event_get_regions_values.h
*
* Description
* ===========
*  Retrieve the money values for the regions of a requested 
*  country
*
* Owner
* =====
*  David Shooner
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _GOLEM_EVENT_GET_REGIONS_VALUES_H_
#define _GOLEM_EVENT_GET_REGIONS_VALUES_H_

namespace SP2
{
   // regions values typedef
   typedef hash_map<UINT32, REAL64> GRegionsValues;

   // our class event handler
   class GEconomicEventHandler;

   namespace Event
   {
      /*!
      * Event Sent to retrieve regions values for a country
      **/
      class GEventGetRegionsValues: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class GEconomicEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         // the country id from wich we request the regions
         UINT16 m_iCountryIdRequested;
         // map with the region id and the region value
         GRegionsValues m_RegionsValues;
      private:
         GEventGetRegionsValues();
         ~GEventGetRegionsValues();
      };

   }; // namespace Event

}; // namespace SP2

#endif