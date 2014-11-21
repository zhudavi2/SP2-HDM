/**************************************************************
*
* sp2_save_confirmation_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_SAVE_CONFIRMATION_WINDOW_H_
#define _CSP2_SAVE_CONFIRMATION_WINDOW_H_

namespace SP2
{

   class GSaveConfirmationWindow : public GUI::GFrame
   {
   public:
	   GSaveConfirmationWindow();
	   ~GSaveConfirmationWindow();

      void Caller(SP2::GBaseWindow* in_pCaller);
      
      bool OnCreate();
      void OnShow();
      void OnHide();

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      void OnDrag(const GEventData &, const GVector2D<INT32> & in_MouseOffset, const GVector2D<INT32> &);
      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

      GUI::GButton* m_pObjApplyBtn;
      GUI::GButton* m_pObjDontApplyBtn;
      GUI::GToggle* m_pObjRememberTog;

   private:
      SP2::GBaseWindow* m_pCaller;
   };
};

#endif //_CSP2_SAVE_CONFIRMATION_WINDOW_H_
