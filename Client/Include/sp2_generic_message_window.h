/**************************************************************
*
* sp2_economic_health_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_GENERIC_MESSAGE_WINDOW_H_
#define _CSP2_GENERIC_MESSAGE_WINDOW_H_

namespace SP2
{
   const UINT32 c_OnGenericMessageWindowAnswerEvent = 98471651;
   const UINT32 c_OnGenericMessageWindowOk          = 98421652;

   class GGenericMessageWindow : public GBaseWindow, public DesignPattern::GSubject
   {
   public:
      static const GString TypeName;
	   GGenericMessageWindow();
	   ~GGenericMessageWindow();

	   bool OnCreate();

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      bool OnLoseFocus(const GUI::GBaseObject *in_pFocusTo, GUI::GBaseObject* in_pCaller);
      bool OnGotFocus (const GBaseObject* in_pFocusFrom, GBaseObject* Caller = NULL);

      void Show(GString in_sMessage, GUI::GBaseObject* in_pCaller);
      void Hide();
   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

   private:
      GUI::GButton* m_pObjOkBtn;
      GUI::GLabel*  m_pObjTextLbl;

      GUI::GBaseObject* m_pCaller;
   };
};

#endif //_CSP2_GENERIC_MESSAGE_WINDOW_H_
