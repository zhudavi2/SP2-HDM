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

const pair<EDZLogLevel::Enum, GString> GDZDebug::c_LogLevelNames[] =
{
    pair<EDZLogLevel::Enum, GString>(EDZLogLevel::Always,  L"ALWAYS"),
    pair<EDZLogLevel::Enum, GString>(EDZLogLevel::Error,   L"ERROR"),
    pair<EDZLogLevel::Enum, GString>(EDZLogLevel::Warning, L"WARNING"),
    pair<EDZLogLevel::Enum, GString>(EDZLogLevel::Info1,   L"INFO1"),
    pair<EDZLogLevel::Enum, GString>(EDZLogLevel::Info2,   L"INFO2"),
};

const map<EDZLogLevel::Enum, GString> GDZDebug::c_mLogLevelNames(GDZDebug::c_LogLevelNames, GDZDebug::c_LogLevelNames + sizeof(c_LogLevelNames) / sizeof(pair<EDZLogLevel::Enum, GString>));

bool GDZDebug::m_bLogEnable = false;

const pair<GString, UINT32> GDZDebug::c_LogLevelsEnabled[] =
{
    pair<GString, UINT32>(L"AI",            0),
    pair<GString, UINT32>(L"CombatPlanner", 0),
    pair<GString, UINT32>(L"UnitGroupEx",   0),
    pair<GString, UINT32>(L"UnitMover",     0),
};

map<GString, UINT32> GDZDebug::m_mLogLevelsEnabled(c_LogLevelsEnabled, c_LogLevelsEnabled + sizeof(c_LogLevelsEnabled) / sizeof(pair<GString, UINT32>));

UINT32 GDZDebug::m_iEnabledLogCategories = 0;
