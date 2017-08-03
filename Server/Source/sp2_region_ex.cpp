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
        const INT64 l_iPop15 = Population15();
        const INT64 l_iPop1565 = Population1565();
        const INT64 l_iPop65 = Population65();
        const bool l_bAgePopConsistent = l_iPop == l_iPop15 + l_iPop1565 + l_iPop65;
        gassert(l_bAgePopConsistent, L"Population by age inconsistent");
        if(!l_bAgePopConsistent)
        {
            GDZLOG(EDZLogLevel::Error, L"Population by age inconsistent: " + GDZDebug::FormatInt(l_iPop) + L" total, " + GDZDebug::FormatInt(l_iPop15) + L" under 15, " + GDZDebug::FormatInt(l_iPop1565) + L"15-65 , " + GDZDebug::FormatInt(l_iPop65) + L" over 65");
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
            gassert(l_bReligionPopConsistent, L"Population by religion inconsistent");
            if(!l_bReligionPopConsistent)
            {
                GDZLOG(EDZLogLevel::Error, L"Population by religion inconsistent: " + GDZDebug::FormatInt(l_iPop) + L" by age, " + GDZDebug::FormatInt(l_iTotalPopByReligion) + L" by religion");
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
            gassert(l_bLanguagePopConsistent, L"Population by language inconsistent");
            if(!l_bLanguagePopConsistent)
            {
                GDZLOG(EDZLogLevel::Error, L"Population by language inconsistent: " + GDZDebug::FormatInt(l_iPop) + L" by age, " + GDZDebug::FormatInt(l_iTotalPopByLanguage) + L" by language");
                l_bConsistent = false;
            }
        }

        break;
    }

    GDZLOG(EDZLogLevel::Exit, L"Returning " + GString(l_bConsistent));
    return l_bConsistent;
}

GString GRegionEx::Name() const
{
    return g_ServerDAL.GetString(g_ServerDAL.StringIdRegion(Id()));
}

GString GRegionEx::NameForLog() const
{
    return Name() + L" (" + GString(Id()) + L")";
}
