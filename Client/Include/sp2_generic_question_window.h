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
#ifndef _CSP2_GENERIC_QUESTION_WINDOW_H_
#define _CSP2_GENERIC_QUESTION_WINDOW_H_

namespace SP2
{
   const UINT32 c_OnGenericQuestionWindowAnswerOk      = 98421652;
   const UINT32 c_OnGenericQuestionWindowAnswerCancel  = 98421653;

   class GGenericQuestionWindow : public GBaseWindow, public DesignPattern::GSubject
   {
   public:
      static const GString TypeName;
	   GGenericQuestionWindow();
	   ~GGenericQuestionWindow();

	   bool OnCreate();

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      bool OnLoseFocus(const GUI::GBaseObject *in_pFocusTo, GUI::GBaseObject* in_pCaller);
      bool OnGotFocus (const GBaseObject* in_pFocusFrom, GBaseObject* Caller = NULL);
      void Show(GGenericQuestion  &in_Question);

      void* OkValue() const;
      void* CancelValue() const;

      static void CleanQuestionWindow(void);

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

   private:
      GUI::GButton* m_pObjOkBtn;
      GUI::GButton* m_pObjCancelBtn;
      GUI::GLabel*  m_pObjTextLbl;
      bool           m_bEventSent;
      void KillMe(void);

      GGenericQuestion  m_Question;

      static list<GGenericQuestionWindow*> m_OpenQuestionWindow;
   };
};

#endif //_CSP2_GENERIC_QUESTION_WINDOW_H_
