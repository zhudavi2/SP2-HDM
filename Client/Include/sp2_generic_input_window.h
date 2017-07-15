/**************************************************************
*
* sp2_generic_input_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_GENERIC_INPUT_WINDOW_H_
#define _CSP2_GENERIC_INPUT_WINDOW_H_

namespace SP2
{
   const UINT32 c_OnGenericInputWindowCancel      = 98471661;
   const UINT32 c_OnGenericInputWindowOk          = 98421662;

   class GGenericInputWindow : public GFrame
   {
   public:
      static const GString TypeName;
	   GGenericInputWindow();
	   ~GGenericInputWindow();

	   bool OnCreate();

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnKeyUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      bool OnLoseFocus(const GUI::GBaseObject *in_pFocusTo, GUI::GBaseObject* in_pCaller);
      bool OnGotFocus (const GBaseObject* in_pFocusFrom, GBaseObject* Caller = NULL);

      void Show(GString in_sMessage, GUI::GBaseObject* in_pCaller);
   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

   private:
      GUI::GButton*  m_pObjOkBtn;
      GUI::GButton*  m_pObjCancelBtn;
      GUI::GLabel*   m_pObjTextLbl;
      GUI::GEditBox* m_pObjInput;

      GUI::GBaseObject* m_pCaller;
   };
};

#endif //_CSP2_GENERIC_INPUT_WINDOW_H_
