/**************************************************************
*
* sp2_connecting_window.h
*
* Description
* ===========
*  Class that contains the connecting animated window
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_CONNECTING_WINDOW_H_
#define _SP2_CONNECTING_WINDOW_H_

#include "sp2_fsm_main.h"

namespace SP2
{
   //! Window used when progress is shown
   class GConnectingWindow : public GFrame
   {
   public:
      static const GString TypeName;

      //! Creation function
      static GUI::GBaseObject* New();

      bool OnCreate();
      void OnShow();

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      void SetDisplayText(const GString& in_sText);
      void SetCancelEvent(FSM::EEvents::Enum in_eEvent);

      //! Simply move window content to indicate that client is connecting
      void Animate();

   private:
      GUI::GLabel*         m_pConnectLabel;
      GUI::GBaseObject*    m_pCancelBtn;

      GString              m_sInitialText;
      REAL64               m_fShowTime;
      FSM::EEvents::Enum   m_eCancelEvent;
   };
}

#endif // #ifndef _SP2_CONNECTING_WINDOW_H_
