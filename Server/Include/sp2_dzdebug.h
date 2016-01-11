/**************************************************************
*
* sp2_dzdebug.h
*
* Description
* ===========
*  Debug logging.
*
* Copyright  (C) 2015, D. Z. (BlasterMillennia)
***************************************************************/
#ifndef _GOLEM_SP2_DZDEBUG_H_
#define _GOLEM_SP2_DZDEBUG_H_

#include "golem_pch.hpp"

namespace SP2
{
    class GDZDebug
    {
    public:
        static inline void Log(const GString& in_sMsg, const GString& in_sFunc = L"", const INT32 in_iLine = 0)
        {
            if(m_bLoggingEnabled)
            {
                g_Joshua.Log(L"(DZDEBUG) " + in_sFunc + L": " + GString(in_iLine) + L" - " + in_sMsg);
            }
        }

    private:
        //! Set to true to enable logging.
        static const bool m_bLoggingEnabled = false;
    };
}

#endif //_GOLEM_SP2_DZDEBUG_H_
