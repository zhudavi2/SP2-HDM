/**************************************************************
*
* sp2_region_ex.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2017, D.Z. "BlasterMillennia"
**************************************************************/
#include "golem_pch.hpp"

bool GRegionEx::VerifyPopulation()
{
    GDZLOG(EDZLogLevel::Entry, L"");

    bool l_bConsistent = true;

    while(GDZDebug::AssertEnabled() || GDZDebug::LogEnabled())
    {
        const INT64 l_iPop = Population();
        const bool l_bAgePopConsistent = l_iPop == Population15() + Population1565() + Population65();
        gassert(l_bAgePopConsistent, L"Age population inconsistent");
        if(!l_bAgePopConsistent)
        {
            l_bConsistent = false;
            break;
        }

        {
            GReligionList l_Religions;
            GetReligions(l_Religions);
            INT64 l_iTotalPopByReligion = 0;
            for(auto it = l_Religions.cbegin(); it != l_Religions.cend(); ++it)
                l_iTotalPopByReligion += it->second;

            const bool l_bReligionPopConsistent = l_iPop == l_iTotalPopByReligion;
            gassert(l_bReligionPopConsistent, L"Religious population inconsistent");
            if(!l_bReligionPopConsistent)
            {
                l_bConsistent = false;
                break;
            }
        }

        {
            GLanguageList l_Languages;
            GetLanguages(l_Languages);
            INT64 l_iTotalPopByLanguage = 0;
            for(auto it = l_Languages.cbegin(); it != l_Languages.cend(); ++it)
                l_iTotalPopByLanguage += it->second;

            const bool l_bLanguagePopConsistent = l_iPop == l_iTotalPopByLanguage;
            gassert(l_bLanguagePopConsistent, L"Language population inconsistent");
            if(!l_bLanguagePopConsistent)
                l_bConsistent = false;
        }

        break;
    }

    GDZLOG(EDZLogLevel::Exit, L"Returning " + GString(l_bConsistent));
    return l_bConsistent;
}

GString GRegionEx::NameForLog() const
{
    const UINT32 l_iId = Id();
    return g_ServerDAL.GetString(g_ServerDAL.StringIdRegion(l_iId)) + L" (" + GString(l_iId) + L")";
}
