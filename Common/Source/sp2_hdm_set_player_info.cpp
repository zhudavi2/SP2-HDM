/**************************************************************
*
* sp2_hdm_set_player_info.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2018, D. Z. (BlasterMillennia)
***************************************************************/

#include "golem_pch.hpp"

SDK::GGameEventSPtr SP2::Event::GHdmSetPlayerInfo::New()
{
    GDZLOG(EDZLogLevel::Entry, L"");
    SDK::GGameEventSPtr l_Event = SDK::GGameEventSPtr(new GHdmSetPlayerInfo());
    GDZLOG(EDZLogLevel::Exit, L"Returning " + GDZDebug::FormatPtr(l_Event.get()));
    return l_Event;
}

bool SP2::Event::GHdmSetPlayerInfo::Serialize(GIBuffer& io_Buffer) const
{
    GDZLOG(EDZLogLevel::Entry, L"io_Buffer.Size() = " + GString(io_Buffer.Size()));

    GDZLOG(EDZLogLevel::Info1, "m_PlayerInfo.ClientID = " + GString(m_PlayerInfo.ClientID) + L", m_PlayerInfo.CountryID = " + GString(m_PlayerInfo.CountryID) + L", m_PlayerInfo.PartyID = " + GString(m_PlayerInfo.PartyID) + L", m_PlayerInfo.PlayerName = " + GString(m_PlayerInfo.PlayerName) + L", m_PlayerInfo.PlayerStatus = " + GString(m_PlayerInfo.PlayerStatus));

    bool l_bReturn = false;
    if(__super::Serialize(io_Buffer))
    {
        GDZLOG(EDZLogLevel::Info1, L"io_Buffer.Size() = " + GString(io_Buffer.Size()) + L", m_iPassword = " + GDZDebug::FormatHex(m_iPassword));

        io_Buffer << m_iPassword;

        GDZLOG(EDZLogLevel::Info1, L"io_Buffer.Size() = " + GString(io_Buffer.Size()));

        l_bReturn = true;
    }

    GDZLOG(EDZLogLevel::Exit, L"Returning " + GString(l_bReturn));
    return l_bReturn;
}

bool SP2::Event::GHdmSetPlayerInfo::Unserialize(GOBuffer& io_Buffer)
{
    GDZLOG(EDZLogLevel::Entry, L"io_Buffer.Remaining() = " + GString(io_Buffer.Remaining()));

    bool l_bReturn = false;
    if(__super::Unserialize(io_Buffer) && io_Buffer.Remaining() >= sizeof(UINT32))
    {
        GDZLOG(EDZLogLevel::Info1, L"m_PlayerInfo.ClientID = " + GString(m_PlayerInfo.ClientID) + L", m_PlayerInfo.CountryID = " + GString(m_PlayerInfo.CountryID) + L", m_PlayerInfo.PartyID = " + GString(m_PlayerInfo.PartyID) + L", m_PlayerInfo.PlayerName = " + GString(m_PlayerInfo.PlayerName) + L", m_PlayerInfo.PlayerStatus = " + GString(m_PlayerInfo.PlayerStatus) + L"io_Buffer.Remaining() = " + GString(io_Buffer.Remaining()));

        io_Buffer >> m_iPassword;

        GDZLOG(EDZLogLevel::Info1, L"io_Buffer.Remaining() = " + GString(io_Buffer.Remaining()) + L", m_iPassword = " + GDZDebug::FormatHex(m_iPassword));

        l_bReturn = true;
    }

    GDZLOG(EDZLogLevel::Exit, L"Returning " + GString(l_bReturn));
    return l_bReturn;
}

SP2::Event::GHdmSetPlayerInfo::GHdmSetPlayerInfo() :
m_iPassword(0)
{
}
