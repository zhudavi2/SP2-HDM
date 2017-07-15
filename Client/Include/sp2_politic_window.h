/**************************************************************
*
* sp2_politic_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_1st_PoliticWindow_H_
#define _CSP2_1st_PoliticWindow_H_

namespace SP2
{
   const INT32 c_MilitaryDictatorshipSTID = 9153;
   class GIdeologyMeter;
   class GCursorMeter;

   class GPoliticWindow : public GBaseWindow, DesignPattern::GObserver
   {
   public:
	   GPoliticWindow();
	   ~GPoliticWindow();

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

	   bool OnCreate();
      void OnShow();
      void OnHide();

      bool Enabled();
      void Enabled(bool in_bEnabled);

      bool Update();

      void OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );


      static const GString TypeName;

      GDataPoliticWindow* m_pData;

   protected:
      friend class GDataDisplayLayer;
      friend class GConstitutionalFormWindow;
      static GUI::GBaseObject* New();

      GIdeologyMeter* m_pObjIdeology;
      GCursorMeter* m_pObjApproval;
      GCursorMeter* m_pObjPressure;
      GCursorMeter* m_pObjStability;
      GCursorMeter* m_pObjCorruption;

      GUI::GLabel* m_pObjApprovalTxt;
      GUI::GLabel* m_pObjPressureTxt;
      GUI::GLabel* m_pObjStabilityTxt;
      GUI::GLabel* m_pObjCorruptionTxt;
      GUI::GLabel* m_pObjConstitutionalFormTxt;

      GUI::GButton* m_pObjConstitutionalFormMoreBtn;
      GUI::GButton* m_pObjInternalLawsBtn;
      GUI::GButton* m_pObjTreatiesBtn;


   private:
      
   };
};

#endif //_CSP2_1st_PoliticWindow_H_
