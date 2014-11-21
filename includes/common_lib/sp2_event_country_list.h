/**************************************************************
*
* sp2_event_country_list.h
*
* Description
* ===========
*  Description & implementaiton of classes 
*  SP2::Event::GRequestCountryList && 
*  SP2::Event::GReceiveCountryList
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_EVENT_COUNTRY_LIST_H_
#define _SP2_EVENT_COUNTRY_LIST_H_

namespace SP2
{
   class GClient;
   class GServer;

   namespace Event
   {
      class GRequestCountryList : public SDK::Event::GGameEvent
      {
         friend GClient;//so they have access to New
         friend GServer;//so they have access to New

      protected:
         static SDK::GGameEventSPtr New();
      };

      class GReceiveCountryList : public SDK::Event::GGameEvent
      {
         friend GClient;//so they have access to New
         friend GServer;//so they have access to New

      public:
         SP2::GCountries      m_vCountries;

      protected:
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         static SDK::GGameEventSPtr New();
      };
   }
}

#endif // #ifndef _SP2_EVENT_COUNTRY_LIST_H_