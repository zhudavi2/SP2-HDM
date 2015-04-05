/**************************************************************
*
* sp2_human_development_utilities.h
*
* Description
* ===========
* Utilities to help handle human-development-related items that are common to
* both server and client.
*
* Copyright  (C) 2015 David Zhu
***************************************************************/

#ifndef _GOLEM_HUMANDEVELOPMENTUTILITIES_H_
#define _GOLEM_HUMANDEVELOPMENTUTILITIES_H_

namespace SP2
{
    class GHumanDevelopmentUtilities
    {
    public:
        static REAL32 FindIncomeIndex(REAL64 in_fGDPPerCapita);

        static void SetHumanDevelopmentForEventCountryInfo(REAL32 in_fHumanDev, REAL32 in_fLifeExpectancy, REAL32 in_fMeanYearsSchooling, REAL32 in_fExpectedYearsSchooling, REAL32 in_fHumanDevAverage, SP2::Event::GEventCountryInfo& in_eventCountryInfo);

        static void GetHumanDevelopmentFromEventCountryInfo(const SP2::Event::GEventCountryInfo& in_eventCountryInfo, REAL32& in_fHumanDev, REAL32& in_fLifeExpectancy, REAL32& in_fMeanYearsSchooling, REAL32& in_fExpectedYearsSchooling, REAL32& in_fHumanDevAverage);

    private:
        union GHumanDevelopmentPacked
        {
            struct GHumanDevelopmentPackedInternal
            {
                UINT32   m_iHumanDev           : 10;
                UINT32   m_iLifeExpectancy     : 10;
                UINT32   m_iMeanYearsSchooling : 8;
                UINT32   m_iReserved           : 4;
            } m_components;

            REAL32   m_fAll;
        };

        union GHumanDevelopmentAveragePacked
        {
            struct GHumanDevelopmentAveragePackedInternal
            {
                UINT32   m_iHumanDevAverage        : 10;
                UINT32   m_iExpectedYearsSchooling : 8;
                UINT32   m_iReserved               : 14;
            } m_components;

            REAL32   m_fAll;
        };
    };
};//end namespace SP2
#endif //_GOLEM_HUMANDEVELOPMENTUTILITIES_H_
