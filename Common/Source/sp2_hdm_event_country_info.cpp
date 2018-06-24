/**************************************************************
*
* sp2_hdm_event_country_info.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2018, D. Z. (BlasterMillennia)
***************************************************************/

#include "golem_pch.hpp"

SDK::GGameEventSPtr SP2::Event::GHdmEventCountryInfo::New()
{
    GDZLOG(EDZLogLevel::Entry, L"");
    SDK::GGameEventSPtr l_Event = SDK::GGameEventSPtr(new GHdmEventCountryInfo());
    GDZLOG(EDZLogLevel::Exit, L"Returning " + GDZDebug::FormatPtr(l_Event.get()));
    return l_Event;
}

bool SP2::Event::GHdmEventCountryInfo::Serialize(GIBuffer& io_Buffer) const
{
    GDZLOG(EDZLogLevel::Entry, L"io_Buffer.Size() = " + GString(io_Buffer.Size()));

    bool l_bReturn = false;
    if(__super::Serialize(io_Buffer))
    {
        GDZLOG(EDZLogLevel::Info1, L"io_Buffer.Size() = " + GString(io_Buffer.Size()) + L", m_fLifeExpectancy = " + GString::FormatNumber(m_fLifeExpectancy, 1) + L", m_fMeanYearsSchooling = " + GString::FormatNumber(m_fMeanYearsSchooling, 1) + L", m_fExpectedYearsSchooling = " + GString::FormatNumber(m_fExpectedYearsSchooling, 1));
        io_Buffer << m_fLifeExpectancy
            << m_fMeanYearsSchooling
            << m_fExpectedYearsSchooling;
        GDZLOG(EDZLogLevel::Info1, L"io_Buffer.Size() = " + GString(io_Buffer.Size()));
        l_bReturn = true;
    }

    GDZLOG(EDZLogLevel::Exit, L"Returning " + GString(l_bReturn));
    return l_bReturn;
}

bool SP2::Event::GHdmEventCountryInfo::Unserialize(GOBuffer& io_Buffer)
{
    GDZLOG(EDZLogLevel::Entry, L"io_Buffer.Remaining() = " + GString(io_Buffer.Remaining()));

    bool l_bReturn = false;
    if(__super::Unserialize(io_Buffer) && io_Buffer.Remaining() >= sizeof(REAL32) * 3)
    {
        GDZLOG(EDZLogLevel::Info1, L"io_Buffer.Remaining() = " + GString(io_Buffer.Remaining()));
        io_Buffer >> m_fLifeExpectancy
            >> m_fMeanYearsSchooling
            >> m_fExpectedYearsSchooling;
        GDZLOG(EDZLogLevel::Info1, L"io_Buffer.Remaining() = " + GString(io_Buffer.Remaining()) + L", m_fLifeExpectancy = " + GString::FormatNumber(m_fLifeExpectancy, 1) + L", m_fMeanYearsSchooling = " + GString::FormatNumber(m_fMeanYearsSchooling, 1) + L", m_fExpectedYearsSchooling = " + GString::FormatNumber(m_fExpectedYearsSchooling, 1));
        l_bReturn = true;
    }

    GDZLOG(EDZLogLevel::Exit, L"Returning " + GString(l_bReturn));
    return l_bReturn;
}
