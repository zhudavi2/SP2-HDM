/**************************************************************
*
* sp2_event_country_info.h
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


#ifndef _GOLEM_EVENT_COUNTRY_INFO_H_
#define _GOLEM_EVENT_COUNTRY_INFO_H_

namespace SP2
{   
   namespace Event
   {
      class GEventCountryInfo: public SDK::Event::GGameEvent
      {                  
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();

      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         INT32  m_iCountryID;
         
         REAL32 m_fArableLand;
         REAL32 m_fForestLand;
         REAL32 m_fParksLand;
         REAL32 m_fUnusableLand;

         INT64  m_iPop15;
         INT64  m_iPop1565;
         INT64  m_iPop65;

         REAL32 m_fLandArea;
         REAL32 m_fWaterArea;
         REAL32 m_fTotalArea;

         REAL32 m_fHumanDev;
         REAL32 m_fHumanDevAverage;

         INT32  m_iClimateStid;

         REAL32 m_fBirthRate;
         REAL32 m_fDeathRate;
         REAL32 m_fInfrastructure;
         REAL32 m_fTelecommunications;

      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_COUNTRY_INFO_H_
