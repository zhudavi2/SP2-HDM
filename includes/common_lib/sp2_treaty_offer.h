/**************************************************************
*
* sp2_treaty_offer.h
*
* Description
* ===========
*  Everytime a treaty is created, this event is sent to the players
*  which are members of this new treaty.
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_TREATY_OFFER_H_
#define _GOLEM_EVENT_TREATY_OFFER_H_

namespace SP2
{
   namespace Event
   {
      /*!
      * Event Sent from player to player when asking for a peace treaty.
		* It is sent back once a player has given its answer
      **/
      class GTreatyOffer: public SDK::Event::GGameEvent
      {                  
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

			UINT32 TreatyID;			
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_TREATY_OFFER_H_