/**************************************************************
*
* sp2_human_development_utilities.cpp
*
* Description
* ===========
* See .h file.
*
* Copyright  (C) 2014 David Zhu
***************************************************************/

#include "golem_pch.hpp"

void GHumanDevelopmentUtilities::SetHumanDevelopmentForEventCountryInfo(REAL32 in_fHumanDev, REAL32 in_fLifeExpectancy, REAL32 in_fMeanYearsSchooling, REAL32 in_fExpectedYearsSchooling, REAL32 in_fHumanDevAverage, SP2::Event::GEventCountryInfo& in_eventCountryInfo)
{
    GHumanDevelopmentPacked l_HumanDevPacked = {0};
    l_HumanDevPacked.m_components.m_iHumanDev = static_cast<UINT32>(in_fHumanDev * 1000);
    l_HumanDevPacked.m_components.m_iLifeExpectancy = static_cast<UINT32>(in_fLifeExpectancy * 10);
    l_HumanDevPacked.m_components.m_iMeanYearsSchooling = static_cast<UINT32>(in_fMeanYearsSchooling * 10);
    //g_Joshua.Log(L"MYS " + GString(l_HumanDevPacked.m_iMeanYearsSchooling) + L" packed " + GString(*reinterpret_cast<UINT32*>(&l_HumanDevPacked.m_fAll)));

    GHumanDevelopmentAveragePacked l_HumanDevAveragePacked = {0};
    l_HumanDevAveragePacked.m_components.m_iHumanDevAverage = static_cast<UINT32>(in_fHumanDevAverage * 1000);
    l_HumanDevAveragePacked.m_components.m_iExpectedYearsSchooling = static_cast<UINT32>(in_fExpectedYearsSchooling * 10);

    in_eventCountryInfo.m_fHumanDev = l_HumanDevPacked.m_fAll;
    in_eventCountryInfo.m_fHumanDevAverage = l_HumanDevAveragePacked.m_fAll;
}

void GHumanDevelopmentUtilities::GetHumanDevelopmentFromEventCountryInfo(const SP2::Event::GEventCountryInfo& in_eventCountryInfo, REAL32& in_fHumanDev, REAL32& in_fLifeExpectancy, REAL32& in_fMeanYearsSchooling, REAL32& in_fExpectedYearsSchooling, REAL32& in_fHumanDevAverage)
{
    GHumanDevelopmentPacked l_HumanDevPacked = {0};
    l_HumanDevPacked.m_fAll = in_eventCountryInfo.m_fHumanDev;

    in_fHumanDev = static_cast<REAL32>(l_HumanDevPacked.m_components.m_iHumanDev) / 1000.f;
    in_fLifeExpectancy = static_cast<REAL32>(l_HumanDevPacked.m_components.m_iLifeExpectancy) / 10.f;
    in_fMeanYearsSchooling = static_cast<REAL32>(l_HumanDevPacked.m_components.m_iMeanYearsSchooling) / 10.f;
    //g_Joshua.Log(L"MYS " + GString(l_HumanDevPacked.m_iMeanYearsSchooling) + L" packed " + GString(*reinterpret_cast<UINT32*>(&l_HumanDevPacked.m_fAll)));

    GHumanDevelopmentAveragePacked l_HumanDevAveragePacked = {0};
    l_HumanDevAveragePacked.m_fAll = in_eventCountryInfo.m_fHumanDevAverage;

    in_fExpectedYearsSchooling = static_cast<REAL32>(l_HumanDevAveragePacked.m_components.m_iExpectedYearsSchooling) / 10.f;
    in_fHumanDevAverage = static_cast<REAL32>(l_HumanDevAveragePacked.m_components.m_iHumanDevAverage) / 1000.f;
}

REAL32 GHumanDevelopmentUtilities::FindIncomeIndex(REAL64 in_fGDPPerCapita)
{
    if(in_fGDPPerCapita <= 100)
    {
        return 0;
    }
    else if(in_fGDPPerCapita <= 60000)
    {
        return static_cast<REAL32>(log(in_fGDPPerCapita/100) / log(600.0));
    }
    else
    {
        return 1;
    }
}
