/**************************************************************
*
* sp2_tooltip_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_TOOLTIP_WINDOW_H_
#define _CSP2_TOOLTIP_WINDOW_H_

namespace SP2
{

   class GTooltipWindow : public GUI::GFrame
   {
   public:
	   GTooltipWindow();
	   ~GTooltipWindow();

	   bool OnCreate();
      void OnShow();
      void OnMove();
      void OnResize();

      GUI::GLabel* Label();

      static const GString TypeName;
   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

      GUI::GLabel* m_pObjText;

   private:
   };
};

#endif //_CSP2_TOOLTIP_WINDOW_H_
