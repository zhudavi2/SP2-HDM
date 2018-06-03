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
        static bool LogEnabled();

        static void Log(EDZLogLevel::Enum in_iLogLevel, const GString& in_sMsg, const GString& in_sFunc, INT32 in_iLine);

        static bool AssertEnabled();

        static void Assert(bool in_bExpr, const GString& in_sExpr, const GString& in_sMsg, const GString& in_sFunc, const GString& in_sFile, INT32 in_iLine);

        //! Helper to add thousands separators to a large integer
        static GString FormatInt(INT64 in_iInt);

        //! Helper for pointer values
        static GString FormatPtr(const void* in_pPtr);

        template<typename T>
        static GString FormatHex(T in_iInt);

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

    template<typename T> GString GDZDebug::FormatHex(const T in_iInt)
    {
        wstringstream l_StringStream;
        l_StringStream << L"0x" << hex << setfill(L'0') << setw(sizeof(T) * 2) << uppercase << in_iInt;
        return l_StringStream.str();
    }
}

#endif //_GOLEM_SP2_DZDEBUG_H_
