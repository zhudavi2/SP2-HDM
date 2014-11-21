/**************************************************************
*
* sp2_event_conquer_country.h
*
* Description
* ===========
*  Game event that is sent to all clients when a country is 
*  completely conquered.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_EVENT_CONQUER_COUNTRY_H_
#define _GOLEM_EVENT_CONQUER_COUNTRY_H_

namespace SP2
{
   namespace Event
   {
      class GConquerCountry : public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         UINT32         m_iConqeredID;
         UINT32         m_iConqueringID;

      protected:
      private:
         GConquerCountry();
      };

   }

}

#endif // #ifndef _GOLEM_EVENT_CONQUER_COUNTRY_H_
