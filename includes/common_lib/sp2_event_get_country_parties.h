/**************************************************************
*
* golem_eventAdjustIncomeTax.h
*
* Description
* ===========
*  Game events that have something to do with starting the game
*  (requesting to start the game and starting it)
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_GET_COUNTRY_PARTIES_H_
#define _GOLEM_EVENT_GET_COUNTRY_PARTIES_H_

namespace SP2
{
   class GGeneralEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GGetCountryParties: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class GPoliticEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         INT16 m_iCountryId;
         vector< GPoliticalParty > m_vPartyList;


      protected:
      private:
         GGetCountryParties();
         ~GGetCountryParties();
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_GET_COUNTRY_PARTIES_H_
