/**************************************************************
*
* sp2_event_treaty_refusal_details.h
*
* Description
* ===========
*  Contains a treaty refusal details (which countries refused, and why)
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_TREATY_REFUSAL_DETAILS_H_
#define _GOLEM_EVENT_TREATY_REFUSAL_DETAILS_H_

namespace SP2
{
   namespace Event
   {   
      class GEventTreatyRefusalDetails: public SDK::Event::GGameEvent
      {
         //! friends calsses
         friend class GClient;
         friend class GServer;         

         //! static members
         static SDK::GGameEventSPtr New();
      public:
         //! Network serialization functions
         bool Serialize(GIBuffer&) const;
         bool Unserialize(GOBuffer&);

			GTreatyRefusalDetails	m_TreatyRefusalDetails;         
      };

   }; // namespace Event

}; // namespace SP2

#endif