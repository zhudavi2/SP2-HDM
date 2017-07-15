/**************************************************************
*
* sp2_region_control_change_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_REGION_CONTROL_WINDOW_H_
#define _CSP2_REGION_CONTROL_WINDOW_H_

namespace SP2
{
   class GRegionControlChangeWindow : public GFrame
   {
   public:
      static const GString TypeName;
	   GRegionControlChangeWindow();
	   ~GRegionControlChangeWindow();

	   bool OnCreate();

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller);

      void Show(GString in_sMessage);

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

   private:
      GUI::GButton*     m_pObjOkBtn;
      GUI::GFrame*      m_pObjTextFrm;
      GUI::GLabel*      m_pObjTextLbl;
      GUI::GVScrollBar* m_pObjTextScr;
   };
};

#endif //_CSP2_REGION_CONTROL_WINDOW_H_
