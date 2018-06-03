/**************************************************************
*
* sp2_hdm_event_country_info.h
*
* Description
* ===========
*  HDM event version of country info.
*
* Copyright  (C) 2018, D. Z. (BlasterMillennia)
***************************************************************/
#ifndef _GOLEM_SP2_HDM_EVENT_COUNTRY_INFO_H_
#define _GOLEM_SP2_HDM_EVENT_COUNTRY_INFO_H_

namespace SP2
{
    namespace Event
    {
        class GHDMEventCountryInfo : public SP2::Event::GEventCountryInfo
        {
            friend class GClient;
            friend class GServer;

            static SDK::GGameEventSPtr New();

        public:
            REAL32 m_fLifeExpectancy;
            REAL32 m_fMeanYearsSchooling;
            REAL32 m_fExpectedYearsSchooling;

            virtual bool Serialize(GIBuffer& io_Buffer) const;
            virtual bool Unserialize(GOBuffer& io_Buffer);
        };
    }
}

#endif //_GOLEM_SP2_HDM_EVENT_COUNTRY_INFO_H_
