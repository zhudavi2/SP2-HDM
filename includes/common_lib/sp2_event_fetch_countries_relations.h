/**************************************************************
*
* sp2_event_fetch_countries_relations.h
*
* Description
* ===========
*  [A quick overview of the content in both files (.h and .cpp)]
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _GOLEM_EVENT_FETCH_COUNTRIES_RELATIONS_H_
#define _GOLEM_EVENT_FETCH_COUNTRIES_RELATIONS_H_

namespace SP2
{
   namespace Event
   {
      /*!
      * Game event to fetch the history relation betwen 2 given countries
      **/
      class GEventFetchCountriesRelations :  public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
      public:
         GEventFetchCountriesRelations();
         ~GEventFetchCountriesRelations();
			
			UINT32 m_iCountryId; //<! Country asking for the relations
         vector<REAL32> m_vRelationsValues;
      protected:
      private:
         static SDK::GGameEventSPtr New();
         bool Serialize(GIBuffer&) const;
         bool Unserialize(GOBuffer&);


      };

   };
};
#endif //_GOLEM_EVENT_FETCH_COUNTRIES_RELATIONS_H_
