/**************************************************************
*
* sp2_frame_scenario.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_FrameScenario_H_
#define _CSP2_FrameScenario_H_

namespace SP2
{

   class GFrameScenario : public GFrame
   {
   public:
	   GFrameScenario();
	   ~GFrameScenario();

	   bool OnCreate();

      EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller = 0);
      void UpdateSelectedScenario();
      INT32 SelectedCountryID();

      GSmartPtr<SP2::GGameScenario> Scenario();
      void Scenario( GSmartPtr<GGameScenario> in_pScenario );

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      friend class GGameScenarioWindow;
      friend class GObjectivesWindow;

      static GBaseObject* New();

      GUI::GTabbed*     m_pObjScenarioTab;

      GUI::GLabel*      m_pObjScenarioTitle;
      
      GUI::GLabel*      m_pObjDescriptionLbl;
      GUI::GLabel*      m_pObjTipsLbl;
      GUI::GLabel*      m_pObjScenarioCountry;
      GUI::GListBox*    m_pObjObjectiveList;

      GUI::GFrame*      m_pObjTipsFrm;
      GUI::GFrame*      m_pObjDescriptionFrm;

      GUI::GVScrollBar* m_pObjTipsScr;
      GUI::GVScrollBar* m_pObjDescriptionScr;

      //! internal
      GSmartPtr<GGameScenario>   m_pSelectedScenario;

   private:
   };
};

#endif //_CSP2_FrameScenario_H_
