/**************************************************************
*
* sp2_military_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_MilitaryWindow_H_
#define _CSP2_MilitaryWindow_H_

namespace SP2
{
   class GMilitaryWindow : public GBaseWindow, DesignPattern::GObserver
   {
      friend class GTutorials;
   public:
	   GMilitaryWindow();
	   ~GMilitaryWindow();

      void OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
	   bool OnCreate();
      void OnShow();
      void OnHide();

      void UpdateMilitaryUnits();
      bool Update();
      void Clear();

      static const GString TypeName;

      GUI::GButton* m_pObjConventionalForcesMoreBtn;

      GDataMilitaryWindow* m_pData;


   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

      GUI::GFrame*  m_pObjConventionalFrame;
      GUI::GFrame*  m_pObjStrategicFrame;
      GUI::GButton* m_pObjStrategicWarfareBtn;
      GUI::GButton* m_pObjDesignBtn;
      GUI::GButton* m_pObjBuildBtn;
      GUI::GButton* m_pObjBuyBtn;
      GUI::GButton* m_pObjResearchBtn;
      GUI::GButton* m_pObjStartNuclearResearchBtn;
      GUI::GLabel*  m_pObjAmdsLbl;
      GUI::GButton* m_pObjAmdsResearchBtn;
      GUI::GButton* m_pObjNuclearBuildBtn;
      GUI::GButton* m_pObjCovertActionsBtn;
      GUI::GButton* m_pObjWarListBtn;
      GUI::GButton* m_pObjDeployBtn;
      class GProgressMeterYellowValue* m_pObjNuclearProgressMeter;
		class GProgressMeterYellowValue* m_pObjAMDSProgressMeter;

   private:
      // disable the nuclear features for this window
      void DisableNuclear();
   };
};

#endif //_CSP2_MilitaryWindow_H_
