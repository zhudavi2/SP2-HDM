/**************************************************************
*
* sp2_dzdebug.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2016, D. Z. (BlasterMillennia)
***************************************************************/

#include "golem_pch.hpp"

const UINT32 GDZDebug::c_iDefaultLogLevel = EDZLogLevel::Always | EDZLogLevel::Error | EDZLogLevel::Warning;

const GDZDebug::GLogLevelNamePair GDZDebug::c_LogLevelNames[] =
{
    GLogLevelNamePair(EDZLogLevel::Always,  L"ALWAYS"),
    GLogLevelNamePair(EDZLogLevel::Error,   L"ERROR"),
    GLogLevelNamePair(EDZLogLevel::Warning, L"WARNING"),
    GLogLevelNamePair(EDZLogLevel::Info1,   L"INFO1"),
    GLogLevelNamePair(EDZLogLevel::Info2,   L"INFO2"),
    GLogLevelNamePair(EDZLogLevel::Entry,   L"TRACE"),
    GLogLevelNamePair(EDZLogLevel::Exit,    L"TRACE"),
};

const map<EDZLogLevel::Enum, GString> GDZDebug::c_mLogLevelNames(GDZDebug::c_LogLevelNames, GDZDebug::c_LogLevelNames + sizeof(c_LogLevelNames) / sizeof(GLogLevelNamePair));

bool GDZDebug::m_bLogEnable = false;

const GDZDebug::GLogLevelEnabled GDZDebug::c_LogLevelsEnabled[] =
{
    GLogLevelEnabled(L"AI",                    0),

    GLogLevelEnabled(L"Client",                0),

    GLogLevelEnabled(L"CombatPlanner",         0),

    GLogLevelEnabled(L"ConnectingWindow",      0),

    GLogLevelEnabled(L"CountryData",           0),

    GLogLevelEnabled(L"DataAccessLayerClient", 0),

    GLogLevelEnabled(L"DataAccessLayerServer", 0),

    GLogLevelEnabled(L"DataControlLayer",      0), // Client and Server

    GLogLevelEnabled(L"DataDisplayLayer",      0),
    GLogLevelEnabled(L"Disconnected",          0),
    GLogLevelEnabled(L"GameLobbyWindow",       0),

    GLogLevelEnabled(L"GeneralEventHandler",   0),

    GLogLevelEnabled(L"GenericMessageWindow",  0),
    GLogLevelEnabled(L"HDMEventCountryInfo",   0),
    GLogLevelEnabled(L"JoinMPGameWindow",      0),
    GLogLevelEnabled(L"LoginRGAPIWindow",      0),

    GLogLevelEnabled(L"PoliticEventHandler",   0),
    GLogLevelEnabled(L"RegionEx",              0),
    GLogLevelEnabled(L"Server",                0),
    GLogLevelEnabled(L"UnitGroupEx",           0),
    GLogLevelEnabled(L"UnitMover",             0),
    GLogLevelEnabled(L"WorldBehavior",         0),
};

map<GString, UINT32> GDZDebug::m_mLogLevelsEnabled(c_LogLevelsEnabled, c_LogLevelsEnabled + sizeof(c_LogLevelsEnabled) / sizeof(GLogLevelEnabled));

bool GDZDebug::m_bAssertEnable = m_bLogEnable || false;

GString GDZDebug::FormatPtr(const void* const in_pPtr)
{
    stringstream l_StringStream;
    l_StringStream << in_pPtr;
    return GString(L"0x") + l_StringStream.str();
}
