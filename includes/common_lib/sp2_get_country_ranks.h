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

#ifndef _GOLEM_EVENT_GET_COUNTRY_RANKS_H_
#define _GOLEM_EVENT_GET_COUNTRY_RANKS_H_

namespace SP2
{
   class GGeneralEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GGetCountryRanks: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class GPoliticEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
         INT16*  PoliticRank();
         void    PoliticRank(INT16 in_iPoliticRank);
         INT16*  EconomicRank();
         void    EconomicRank(INT16 in_iEconomicRank);
         INT16*  CountryId();
         void    CountryId(INT16 in_iCountryId);

      protected:
      private:
         GGetCountryRanks();
         ~GGetCountryRanks();

         INT16 m_iCountryId;
         INT16 m_iPoliticRank;
         INT16 m_iEconomicRank;

      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_GET_COUNTRY_RANKS_H_
