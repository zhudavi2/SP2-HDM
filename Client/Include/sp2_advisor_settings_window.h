/**************************************************************
*
* sp2_advisor_settings_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_AdvisorSettingsWindow_H_
#define _CSP2_AdvisorSettingsWindow_H_

namespace SP2
{

   class GAdvisorSettingsWindow : public SP2::GBaseWindow, DesignPattern::GObserver, SP2::GRequestObject
   {
   public:
	   GAdvisorSettingsWindow();
	   ~GAdvisorSettingsWindow();

	   bool OnCreate();
      void OnShow();
      void OnHide();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      void OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );

      bool Update();
      void Save();

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GUI::GLabel*  m_pObjTitleLbl;
      GUI::GToggle* m_pObjEconManualTog;
      GUI::GToggle* m_pObjEconAutomaticTog;
      GUI::GToggle* m_pObjEconConfirmationTog;
      GUI::GToggle* m_pObjPoliManualTog;
      GUI::GToggle* m_pObjPoliAutomaticTog;
      GUI::GToggle* m_pObjPoliConfirmationTog;
      GUI::GToggle* m_pObjMiliManualTog;
      GUI::GToggle* m_pObjMiliAutomaticTog;
      GUI::GToggle* m_pObjMiliConfirmationTog;
      //GUI::GButton* m_pObjExitBtn;

   private:
      REAL64 m_fWhenSaved;
      EAdvisorStatus::Enum m_eEconomic;
      EAdvisorStatus::Enum m_ePolitic;
      EAdvisorStatus::Enum m_eMilitary;
   };
};

#endif //_CSP2_AdvisorSettingsWindow_H_
