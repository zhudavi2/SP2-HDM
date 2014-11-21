/**************************************************************
*
* .h
*
* Description
* ===========
*  Sends the information for a news to the client
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_REQUEST_TREATY_CONDITIONS_COUNTRIES_H_
#define _GOLEM_EVENT_REQUEST_TREATY_CONDITIONS_COUNTRIES_H_

namespace SP2
{   
   namespace Event
   {
      /*!
      * Event Sent from the server to the client, to tell him a news has been posted
      **/
      class GRequestTreatyConditionsCountries: public SDK::Event::GGameEvent
      {                  
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         UINT32      m_iConditions[ETreatyConditions::ItemCount];
         set<UINT32> m_CountryList;
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_REQUEST_TREATY_CONDITIONS_COUNTRIES_H_
