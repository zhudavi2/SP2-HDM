/**************************************************************
*
* sp2_event_country_static_data.h
*
* Description
* ===========
*  Description & implementaiton of classes 
*  SP2::Event::GRequestCountryStaticData && 
*  SP2::Event::GReceiveCountryStaticData
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_EVENT_COUNTRY_STATIC_DATA_H_
#define _SP2_EVENT_COUNTRY_STATIC_DATA_H_

namespace SP2
{
   class GClient;
   class GServer;

   namespace Event
   {
      class GRequestCountryStaticData : public SDK::Event::GGameEvent
      {
         friend GClient;//so they have access to New
         friend GServer;//so they have access to New

      protected:
         static SDK::GGameEventSPtr New();
      };

      class GErrorCountryStaticData : public SDK::Event::GGameEvent
      {
         friend GClient;//so they have access to New
         friend GServer;//so they have access to New

      protected:
         static SDK::GGameEventSPtr New();
      };

      class GReceiveCountryStaticData : public SDK::Event::GGameEvent
      {
         friend GClient;//so they have access to New
         friend GServer;//so they have access to New

      public:
         vector<INT32>     m_vStringTableIdsReligions;
		   vector<INT32>     m_vStringTableIdsResources;
         vector<INT32>     m_vStringTableIdsLanguages;
         vector<INT32>     m_vStringTableIdsPoliticalParties;
         vector<INT32>     m_vStringTableIdsGvtType;
         vector<INT32>     m_vStringTableIdsStatus;
		   vector<INT32>     m_vStringTableIdsObjectiveResults;

      protected:
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         static SDK::GGameEventSPtr New();
      };
   }
}

#endif // #ifndef _SP2_EVENT_COUNTRY_STATIC_DATA_H_