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

#define GDZLOG(logLevel, msg)\
    GDZDebug::Log((logLevel), (msg), __FUNCTION__, __LINE__)

#ifdef gassert
#undef gassert
#endif // gassert

#define gassert(expr, msg)\
    GDZDebug::Assert((expr) ? true : false, (#expr), (msg), __FUNCTION__, __FILE__, __LINE__)

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
            Entry   = 1 << 5,
            Exit    = 1 << 6,
            Trace   = Entry | Exit,
        };
    }

    class GDZDebug
    {
    public:
        static inline void Log(EDZLogLevel::Enum in_iLogLevel,
                               const GString& in_sMsg,
                               const GString& in_sFunc,
                               INT32 in_iLine);

        static inline void Assert(bool in_bExpr,
                                  const GString& in_sExpr,
                                  const GString& in_sMsg,
                                  const GString& in_sFunc,
                                  const GString& in_sFile,
                                  INT32 in_iLine);

        //! Helper to add thousands separators to a large integer
        static inline GString FormatInt(INT64 in_iInt);

    private:
        typedef pair<EDZLogLevel::Enum, GString> GLogLevelNamePair;
        typedef pair<GString, UINT32> GLogLevelEnabled;

        //! Default log level when m_bLogEnabled is true. Log levels enabled in this bitfield don't need to also be enabled in m_mLogLevelsEnabled.
        static const UINT32 c_iDefaultLogLevel;

        //! Helper array and actual map for log level names.
        static const GLogLevelNamePair c_LogLevelNames[];
        static const map<EDZLogLevel::Enum, GString> c_mLogLevelNames;

        //! Must be enabled for any DZLOGs to appear, including Always-level logs
        static bool m_bLogEnable;
        
        //! Helper array and actual map for enabled log levels. Maps module name to level enablement bitfield.
        static const GLogLevelEnabled c_LogLevelsEnabled[];
        static map<GString, UINT32> m_mLogLevelsEnabled;

        //! Must be enabled for gassert
        static bool m_bAssertEnable;
    };
}

void GDZDebug::Log(EDZLogLevel::Enum in_iLogLevel,
                   const GString& in_sMsg,
                   const GString& in_sFunc,
                   INT32 in_iLine)
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

            g_Joshua.Log(L"(DZLOG) [" + c_mLogLevelNames.at(in_iLogLevel) + L"] " +
                         l_sModuleName + L" - " +
                         in_sFunc + L" - " +
                         GString(in_iLine) + L": " +
                         l_sTraceString +
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

#endif //_GOLEM_SP2_DZDEBUG_H_
