/**************************************************************
*
* sp2_event_random_event.h
*
* Description
* ===========
*  Event sent to clients so they'll show the random event on the globe
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_RANDOM_EVENT_H_
#define _GOLEM_EVENT_RANDOM_EVENT_H_


namespace SP2
{   	  
   namespace Event
   {
      /*!
      * Event sent to clients so they'll show the random event on the globe
      **/
      class GRandomEvent: public SDK::Event::GGameEvent
      {                  
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

			ERandomEvents::Enum	m_RandomEvent;	//!<Random Event that happened
			UINT32					m_iCountryID;	//!<Country ID where the event happened
			UINT32					m_iRegionID;	//!<Region ID where the event happened
			REAL32					m_fLongitude;	//!<Longitude where the event happened
			REAL32					m_fLatitude;	//!<Latitude where the event happened
			
			//Effects of random events
			INT64						m_iPopulationChange;
			REAL64					m_pResourceProductionChange[EResources::ItemCount]; 
			REAL32					m_fInfrastructureChange;
			REAL64					m_fMoneyChange;
			REAL32					m_fApprovalChange;
			REAL32					m_fStabilityChange;

         			
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_RANDOM_EVENT_H_