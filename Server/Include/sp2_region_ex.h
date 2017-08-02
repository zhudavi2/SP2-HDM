/**************************************************************
*
* sp2_region_ex.h
*
* Description
* ===========
*  Extension of GRegion class for region-specific utility methods
*
* Copyright  (C) 2017, D.Z. "BlasterMillennia"
***************************************************************/

#ifndef _SP2_REGION_EX_H_
#define _SP2_REGION_EX_H_

namespace SP2
{
    class GRegionEx : public GRegion
    {
    public:
        /*!
        * Debug utility to verify population. Non-const because GetReligions and GetLanguages are non-const
        * @return true if population is consistent, false otherwise. Always true if assert and DZLog disabled
        **/
        bool VerifyPopulation();
    };
}

#endif //_SP2_REGION_EX_H_
