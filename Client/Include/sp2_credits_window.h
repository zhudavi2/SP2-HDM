/**************************************************************
*
* sp2_credits_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_CreditsWindow_H_
#define _CSP2_CreditsWindow_H_

namespace SP2
{

   class GCreditsWindow : public GBaseWindow
   {
   public:
	   GCreditsWindow();
	   ~GCreditsWindow();

	   bool OnCreate();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32, const GUI::GEventData &, GUI::GBaseObject *);
	  GUI::EEventStatus::Enum OnMouseWheel(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

   private:
      GUI::GVScrollBar* m_pObjScrl;
      GUI::GFrame*      m_pObjCreditsFrm;
   };
};

#endif //_CSP2_CreditsWindow_H_
