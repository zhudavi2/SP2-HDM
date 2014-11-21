/**************************************************************
*
* sp2_event_trade_propose.h
*
* Description
* ===========
*  Contains a trade proposal
*
* Owner
* =====
*  David Shooner
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _GOLEM_EVENT_TRADE_PROPOSE_H_
#define _GOLEM_EVENT_TRADE_PROPOSE_H_

namespace SP2
{
   // our class event handler
   class GEconomicEventHandler;

   namespace Event
   {
      /*!
      * Event Sent to retrieve regions values for a country
      **/
      class GEventTradeProposal: public SDK::Event::GGameEvent
      {
         //! friends calsses
         friend class GClient;
         friend class GServer;         
         friend class GEconomicEventHandler;

         //! static members
         static SDK::GGameEventSPtr New();
      public:
         //! Network serialization functions
         bool Serialize(GIBuffer&) const;
         bool Unserialize(GOBuffer&);

         //! our trade data
         GTradeData m_trdData;
      private:
         GEventTradeProposal();
         ~GEventTradeProposal();
      };

   }; // namespace Event

}; // namespace SP2

#endif