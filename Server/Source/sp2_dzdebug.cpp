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
    GLogLevelEnabled(L"CombatPlanner",         0),
    GLogLevelEnabled(L"CountryData",           0),
    GLogLevelEnabled(L"DataAccessLayerServer", 0),
    GLogLevelEnabled(L"DataControlLayer",      0),
    GLogLevelEnabled(L"Server",                0),
    GLogLevelEnabled(L"UnitGroupEx",           0),
    GLogLevelEnabled(L"UnitMover",             0),
    GLogLevelEnabled(L"WorldBehavior",         0),
};

map<GString, UINT32> GDZDebug::m_mLogLevelsEnabled(c_LogLevelsEnabled, c_LogLevelsEnabled + sizeof(c_LogLevelsEnabled) / sizeof(GLogLevelEnabled));

bool GDZDebug::m_bAssertEnable = m_bLogEnable || false;
