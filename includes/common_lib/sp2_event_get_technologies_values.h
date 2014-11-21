/**************************************************************
*
* sp2_event_get_technologies_values.h
*
* Description
* ===========
*  Retrieve the money values for the technologies of a requested 
*  country
*
* Owner
* =====
*  David Shooner
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _GOLEM_EVENT_GET_TECHNOLOGIES_VALUES_H_
#define _GOLEM_EVENT_GET_TECHNOLOGIES_VALUES_H_

namespace SP2
{
   // our trade technologie class predef
   class GTradeTechnologie;
   // our class event handler
   class GEconomicEventHandler;

   // technologies values typedef
   typedef map<GTradeTechnologie, REAL64> GTechnologiesValues;

   namespace Event
   {
      /*!
      * Event Sent to retrieve regions values for a country
      **/
      class GEventGetTechnologiesValues: public SDK::Event::GGameEvent
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
         // 2d array with the technologies and its money value
			UINT8 m_Technologies[EUnitCategory::ItemCount][EUnitDesignCharacteristics::ItemCount];
         //GTechnologiesValues m_TechnologiesValues;
      private:
         GEventGetTechnologiesValues();
         ~GEventGetTechnologiesValues();
      };

   }; // namespace Event

}; // namespace SP2

#endif