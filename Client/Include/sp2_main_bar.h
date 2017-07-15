/**************************************************************
*
* sp2_main_bar.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_Main_Bar_H_
#define _CSP2_Main_Bar_H_

namespace SP2
{
   // Custom event to "untoggle" a spawned window
   const UINT32 c_iCloseWindowEvent = ECustomEvents::UserEvent;

   class GContextMenuItem;

   class GMainBar : public GUI::GFrame
   {
   friend class GEconomicWindow;
   friend class GMilitaryWindow;
   friend class GPoliticWindow;
   friend class GActionListWindow;
   friend class GMailWindow;
   friend class GTutorials;
   friend class GDataDisplayLayer;

   public:
      static const GString TypeName;

	   GMainBar();
	   ~GMainBar();

      void  Date(const GDateTime& in_Date);
      const GDateTime& Date(); 
      bool  Update();
      void  UpdateSpeed(REAL64 in_fNewSpeed);
      void  HandleContextMenu(const GContextMenuItem* in_pItem);
      void  CloseAdvisor();

      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller);
      
      CGFX_Track_Itf* & AnimationSlidingTrack();

      void Iterate();

      void Status(EDiplomaticStatus::Enum in_eStatus);

      //! toggle window states functions
      void TogglePoliticWindow();
      void ToggleEconomicWindow();
      void ToggleMilitaryWindow();
      void ToggleMailWindow();

      void SetAdminView();

   protected:      
      static GUI::GBaseObject* New();

      //! Events related functions
	   bool OnCreate();
      void OnShow();
      bool OnDestroy();
      GUI::EEventStatus::Enum OnMouseDown(const GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseUp(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseOver(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseOut(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller);      
      
      void IterateNews();      

      GFrame*       m_pObjTimeSelection;
      GButton*      m_pObjMoreInfo;
      GButton*      m_pObjRelations;
      GButton*      m_pObjAdvisorBtn;
      GToggle*      m_pObjAdvisor;
      GToggle*      m_pObjHistory;
      GToggle*      m_pObjMail;
      GToggle*      m_pObjEventLog;
      GButton*      m_pObjStatus;
      GButton*      m_pObjTime;
      GButton*      m_pObjFlag;
      GToggle*      m_pObjPolitic;
      GToggle*      m_pObjMilitary;
      GToggle*      m_pObjEconomic;
      GPictureBox*  m_pObjFlagTrim;
      GLabel*       m_pObjTimeLabel;
      GLabel*       m_pObjDateLabel;
      GFrame*       m_pObjNewsFrm;      
      GUI::GButton* m_pObjLocateBtn;

      GUI::GToggle* m_pObjTime1Tog;
      GUI::GToggle* m_pObjTime2Tog;
      GUI::GToggle* m_pObjTime3Tog;
      GUI::GToggle* m_pObjTime4Tog;
      GUI::GToggle* m_pObjTime5Tog;

      SP2::GComboListBoxLess* m_pObjCountryCbo;

   private:
      void SwapTimeDisplay();

      CGFX_Track_Itf* m_pSlidingTrack;

      vector<GUI::GFrame*>        m_vpNewsFrm;
      vector<CGFX_Action_Itf*>    m_vpNewsAct;
      vector< GVector2D<REAL32> > m_vNewsPosition;
      GVector2D<REAL32>           m_NewsPosition;

      REAL64 m_fSpeed;

      REAL64    m_fLastMailFlashClock;
      bool      m_bMailFlashStateChanged;
      bool      m_bMailHighlighted;
      GDateTime m_ActualDate;
      bool      m_bTimeDisplayed;

      static UINT32 m_iNewsId;
   };
};

#endif //_CSP2_Main_Bar_H_
