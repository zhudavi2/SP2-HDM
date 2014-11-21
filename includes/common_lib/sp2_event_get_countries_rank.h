/**************************************************************
*
* sp2_event_get_countries_ranks.h
*
* Description
* ===========
*  Get all country ranks
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_GET_COUNTRIES_RANKS_H_
#define _GOLEM_EVENT_GET_COUNTRIES_RANKS_H_

namespace SP2
{
   class GGeneralEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GGetCountriesRanks: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class GPoliticEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
   
         vector<INT16> m_viCountryID;
         vector<INT16> m_viPoliticRank;
         vector<INT16> m_viMilitaryRank;
         vector<INT16> m_viEconomicRank;

      protected:
      private:
         GGetCountriesRanks();
         ~GGetCountriesRanks();
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_GET_COUNTRIES_RANKS_H_
