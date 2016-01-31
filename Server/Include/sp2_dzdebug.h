/**************************************************************
*
* sp2_dzdebug.h
*
* Description
* ===========
*  Debug logging.
*
* Copyright  (C) 2016, D. Z. (BlasterMillennia)
***************************************************************/
#ifndef _GOLEM_SP2_DZDEBUG_H_
#define _GOLEM_SP2_DZDEBUG_H_

namespace SP2
{
    namespace EDZDebugLogCategory
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
        };
    }

    class GDZDebug
    {
    public:
        static inline void Log(const GString& in_sMsg,
                               const UINT32 in_iLogCategories = EDZDebugLogCategory::General,
                               const GString& in_sFunc = L"",
                               const INT32 in_iLine = 0);

    private:
        //! Bitfield to enable logging.
        static UINT32 m_iEnabledLogCategories;
    };
}

void GDZDebug::Log(const GString& in_sMsg,
                   const UINT32 in_iLogCategories,
                   const GString& in_sFunc,
                   const INT32 in_iLine)
{
    if((in_iLogCategories & GDZDebug::m_iEnabledLogCategories) != 0)
    {
        g_Joshua.Log(L"(DZDEBUG) " +
                     (in_sFunc.empty() ? L"" : (in_sFunc + L": ")) +
                     ((in_iLine == 0) ? L"" : (GString(in_iLine) + L" ")) +
                     L"- " + in_sMsg);
    }
}

#endif //_GOLEM_SP2_DZDEBUG_H_
