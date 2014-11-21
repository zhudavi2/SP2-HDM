/**************************************************************
*
* sp2_system_bar_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_SystemBarWindow_H_
#define _CSP2_SystemBarWindow_H_

namespace SP2
{

   class GSystemBarWindow : public GFrame
   {
   public:
	   GSystemBarWindow();
	   ~GSystemBarWindow();

	   bool OnCreate();
	   bool OnDestroy();

      void OnShow();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnMouseMove(const GEventData & in_EventData, GBaseObject* in_pCaller);
      
      CGFX_Track_Itf* & AnimationSlidingTrack();
      
      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

   private:
      CGFX_Track_Itf*   m_pSlidingTrack;

      GUI::GButton* m_pObjF1Btn;
      GUI::GButton* m_pObjF2Btn;
      GUI::GButton* m_pObjF3Btn;
      GUI::GButton* m_pObjF4Btn;
      GUI::GButton* m_pObjF5Btn;
      GUI::GButton* m_pObjF10Btn;

      GUI::GLabel*  m_pObjF1Lbl;
      GUI::GLabel*  m_pObjF2Lbl;
      GUI::GLabel*  m_pObjF3Lbl;
      GUI::GLabel*  m_pObjF4Lbl;
      GUI::GLabel*  m_pObjF5Lbl;
      GUI::GLabel*  m_pObjF10Lbl;

      GUI::GToggle* m_pObjMainTog;

      bool m_bTooltipShown;

   };
};

#endif //_CSP2_SystemBarWindow_H_
