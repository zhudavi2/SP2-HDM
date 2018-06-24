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

    GLogLevelEnabled(L"CountryInformationWindow", 0),
    GLogLevelEnabled(L"DataAccessLayerClient",    0),

    GLogLevelEnabled(L"DataAccessLayerServer", 0),

    GLogLevelEnabled(L"DataControlLayer",      0), // Client and Server

    GLogLevelEnabled(L"DataDisplayLayer",      0),
    GLogLevelEnabled(L"Disconnected",          0),
    GLogLevelEnabled(L"GameLobbyWindow",       0),

    GLogLevelEnabled(L"GeneralEventHandler",   0), // Client and Server

    GLogLevelEnabled(L"GenericMessageWindow",  0),
	
    GLogLevelEnabled(L"HdmEventCountryInfo",   0),
	GLogLevelEnabled(L"HdmSetPlayerInfo",      0),
	
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

bool GDZDebug::LogEnabled()
{
    return m_bLogEnable;
}

void GDZDebug::Log(EDZLogLevel::Enum in_iLogLevel, const GString& in_sMsg, const GString& in_sFunc, INT32 in_iLine)
{
    if(m_bLogEnable)
    {
        static const wregex l_rModuleNameRegex(L"::G(\\w+)::\\w+$");
        wsmatch l_vMatches;
        regex_search(in_sFunc, l_vMatches, l_rModuleNameRegex);
        const GString l_sModuleName(l_vMatches[1]);

        const auto l_LogLevel = m_mLogLevelsEnabled.find(l_sModuleName);
        const UINT32 l_iLogLevelsEnabled = c_iDefaultLogLevel | ((l_LogLevel == m_mLogLevelsEnabled.cend()) ? 0 : l_LogLevel->second);

        if((l_iLogLevelsEnabled & in_iLogLevel) != 0)
        {
            GString l_sTraceString(L"");
            switch(in_iLogLevel)
            {
            case EDZLogLevel::Entry:
                l_sTraceString = L">>> ";
                break;

            case EDZLogLevel::Exit:
                l_sTraceString = L"<<< ";
                break;

            default:
                break;
            }

            g_Joshua.Log(L"(DZLOG) [" + c_mLogLevelNames.at(in_iLogLevel) + L"] " + l_sModuleName + L" - " + in_sFunc + L" - " + GString(in_iLine) + L": " + l_sTraceString + in_sMsg);
        }
    }
}

bool GDZDebug::AssertEnabled()
{
    return m_bAssertEnable;
}

void GDZDebug::Assert(const bool in_bExpr, const GString& in_sExpr, const GString& in_sMsg, const GString& in_sFunc, const GString& in_sFile, const INT32 in_iLine)
{
    if(m_bAssertEnable && !in_bExpr)
    {
        //g_Joshua.Log() doesn't log out newlines within a GString for some reason
        //Need to log out each line separately
        vector<GString> l_vAssertMsgs;
        l_vAssertMsgs.push_back(L"----------------------------------------------------------------\n");
        l_vAssertMsgs.push_back(L"ASSERT FAILED  : " + in_sExpr + L"\n");
        l_vAssertMsgs.push_back(L"ASSERT MESSAGE : " + in_sMsg  + L"\n");
        l_vAssertMsgs.push_back(L"ASSERT FUNCTION: " + in_sFunc + L"\n");
        l_vAssertMsgs.push_back(L"ASSERT LOCATION: " + in_sFile + L":" + GString(in_iLine) + L"\n");
        l_vAssertMsgs.push_back(L"----------------------------------------------------------------");

        for(auto l_It = l_vAssertMsgs.cbegin(); l_It < l_vAssertMsgs.cend(); ++l_It)
            g_Joshua.Log(*l_It);

        abort();
    }
}

GString GDZDebug::FormatInt(const INT64 in_iInt)
{
    return GString::FormatNumber(static_cast<REAL64>(in_iInt), L",", L"", L"", L"", 3, 0, false);
}

GString GDZDebug::FormatPtr(const void* const in_pPtr)
{
    stringstream l_StringStream;
    l_StringStream << in_pPtr;
    return GString(L"0x") + l_StringStream.str();
}
