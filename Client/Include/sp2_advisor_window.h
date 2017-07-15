/**************************************************************
*
* sp2_advisor_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_AdvisorWindow_H_
#define _CSP2_AdvisorWindow_H_

namespace SP2
{
   struct GAdvisorPlan
   {
      INT32 m_ObjectiveID;
      vector< pair<UINT32, UINT32> > m_Actions;
      vector< pair<UINT32, bool> > m_Results;
      SP2::ETypeOfPlan::Enum m_Type;
   };

   class GAdvisorWindow : public SP2::GBaseWindow
   {
   public:
	   GAdvisorWindow();
	   ~GAdvisorWindow();

	   bool OnCreate();
      void OnShow();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnCustomEvent(UINT32 in_EventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller = NULL);

      void Plan(const GAdvisorPlan & in_Plan);

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GUI::GFrame* m_pObjInnerFrm;
      GUI::GFrame* m_pObjProblemFrm;
      GUI::GFrame* m_pObjPlanFrm;
      GUI::GFrame* m_pObjResultFrm;
      GUI::GFrame* m_pObjGradientFrm;
      GUI::GFrame* m_pObjGradientBlackFrm;

      GUI::GButton* m_pObjExecuteBtn;

      GUI::GLabel*  m_pObjProblemLbl;
      GUI::GLabel*  m_pObjResultLbl;

      GUI::GVScrollBar* m_pObjResultScr;

   private:
      void AddPlanPart(GString in_sText, INT32 in_iTargetCountry = -1);
      void Problem(GString in_sText);
      void Result(GString in_sText);
      void DisplayPlan(INT32 in_iID);

      vector<GAdvisorPlan> m_Plans;
      vector<GUI::GFrame*> m_Frames;
      INT32 m_iCurrentPlan;
      INT32 m_iNbPart;
   };
};

#endif //_CSP2_AdvisorWindow_H_
