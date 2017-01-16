/**************************************************************
*
* sp2_dzdebug.h
*
* Description
* ===========
*  Debug logging and assertion.
*
* Copyright  (C) 2016, D. Z. (BlasterMillennia)
***************************************************************/
#ifndef _GOLEM_SP2_DZDEBUG_H_
#define _GOLEM_SP2_DZDEBUG_H_

#define GDZLOG(msg, logLevel, logCategories)\
    GDZDebug::Log((msg), (logLevel), (logCategories), __FUNCTION__, __LINE__)

#ifdef GOLEM_DEBUG
#undef gassert
#define gassert(expr, msg)\
    GDZDebug::Assert((expr) ? true : false, (#expr), (msg), __FUNCTION__, __FILE__, __LINE__)
#endif // GOLEM_DEBUG

namespace SP2
{
    namespace EDZLogLevel
    {
        enum Enum
        {
            Always  = 1 << 0,
            Error   = 1 << 1,
            Warning = 1 << 2,
            Info1   = 1 << 3,
            Info2   = 1 << 4,
        };
    }

    namespace EDZLogCat
    {
        enum Enum
        {
            General      = 1 << 0,
            ObtainUnits  = 1 << 1,
            Covert       = 1 << 2,
            HDI          = 1 << 3,
            Resources    = 1 << 4,
            UnitMovement = 1 << 5,
            War          = 1 << 6,
            Nuclear      = 1 << 7,
            Regions      = 1 << 8,
            Upkeep       = 1 << 9,
            Player       = 1 << 10,
            Population   = 1 << 11,
            ClientStates = 1 << 12,
            Treaties     = 1 << 13,
        };
    }

    class GDZDebug
    {
    public:
        static inline void Log(const GString& in_sMsg,
                               EDZLogLevel::Enum in_iLogLevel,
                               UINT32 in_iLogCategories,
                               const GString& in_sFunc,
                               INT32 in_iLine);

        static inline void Assert(bool in_bExpr,
                                  const GString& in_sExpr,
                                  const GString& in_sMsg,
                                  const GString& in_sFunc,
                                  const GString& in_sFile,
                                  INT32 in_iLine);

    private:
        //! Default log level when m_bLogEnabled is true. Log levels enabled in this bitfield don't need to also be enabled in m_mLogLevelsEnabled for default logs to appear.
        static const UINT32 c_iDefaultLogLevel;

        //! Helper array and actual map for log level names.
        static const pair<EDZLogLevel::Enum, GString> c_LogLevelNames[];
        static const map<EDZLogLevel::Enum, GString> c_mLogLevelNames;

        //! Must be enabled for any DZLOGs to appear, including Always-level logs
        static bool m_bLogEnable;
        
        //! Helper array and actual map for enabled log levels. Maps module name to level enablement bitfield.
        static const pair<GString, UINT32> c_LogLevelsEnabled[];
        static map<GString, UINT32> m_mLogLevelsEnabled;

        //! Bitfield to enable log categories.
        static UINT32 m_iEnabledLogCategories;
    };
}

void GDZDebug::Log(const GString& in_sMsg,
                   EDZLogLevel::Enum in_iLogLevel,
                   UINT32 in_iLogCategories,
                   const GString& in_sFunc,
                   INT32 in_iLine)
{
    static const wregex l_rModuleNameRegex(L"::G(\\w+)::\\w+$");
    wsmatch l_vMatches;
    regex_search(in_sFunc, l_vMatches, l_rModuleNameRegex);
    const GString l_sModuleName(l_vMatches[1]);

    if(m_bLogEnable)
    {
        const auto l_LogLevel = m_mLogLevelsEnabled.find(l_sModuleName);
        const UINT32 l_iLogLevelsEnabled = c_iDefaultLogLevel | ((l_LogLevel == m_mLogLevelsEnabled.cend()) ? 0 : l_LogLevel->second);

        if((l_iLogLevelsEnabled & in_iLogLevel) != 0 || (in_iLogCategories & m_iEnabledLogCategories) != 0)
        {
            g_Joshua.Log(L"(DZLOG) [" + c_mLogLevelNames.at(in_iLogLevel) + L"] " +
                         l_sModuleName + L" - " +
                         in_sFunc + L" - " +
                         GString(in_iLine) + L": " +
                         in_sMsg);
        }
    }
}

void GDZDebug::Assert(const bool in_bExpr,
                      const GString& in_sExpr,
                      const GString& in_sMsg,
                      const GString& in_sFunc,
                      const GString& in_sFile,
                      const INT32 in_iLine)
{
    if(!in_bExpr)
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

        GString l_sLogicErrorMsg;
        for(auto l_It = l_vAssertMsgs.cbegin(); l_It < l_vAssertMsgs.cend(); ++l_It)
        {
            g_Joshua.Log(*l_It);
            l_sLogicErrorMsg += *l_It;
        }

        abort();
    }
}

#endif //_GOLEM_SP2_DZDEBUG_H_
