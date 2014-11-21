/**************************************************************
*
* sp2_unit_info_small_window.h
*
* Description
* ===========
*  Description & implementation of class 
*  SP2::GUnitInfoSmallWindow.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_UNIT_INFO_SMALL_WINDOW_H_
#define _SP2_UNIT_INFO_SMALL_WINDOW_H_

namespace SP2
{
   class GUnitInfoSmallWindow : public SDK::GUI::GFrame
   {
   public:
      static const GString TypeName;

      bool OnCreate();

      void OnShow();
      void OnHide();

      void DescribeGroup(SP2::GUnitGroup* in_pGroup);

   private:
      GUnitInfoSmallWindow();

      UINT32   m_iContainerWidth;

      GFrame*  m_pInfantryValue;
      GFrame*  m_pGroundValue;
      GFrame*  m_pNavalValue;
      GFrame*  m_pAirValue;

      GLabel*  m_pInfantryTxt;
      GLabel*  m_pGroundTxt;
      GLabel*  m_pNavalTxt;
      GLabel*  m_pAirTxt;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();
   };
};
#endif // #ifndef _SP2_UNIT_INFO_SMALL_WINDOW_H_
