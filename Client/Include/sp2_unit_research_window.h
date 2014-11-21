/**************************************************************
*
* sp2_unit_research_window.h
*
* Description
* ===========
*  Describes the interface of class GUnitResearchWindow
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _SP2_UNIT_Research_WINDOW_H_
#define _SP2_UNIT_Research_WINDOW_H_

namespace SP2
{

   /*!
   * Error notification window
   **/
   class GUnitResearchWindow : public GBaseWindow, SP2::GRequestObject
   {
      friend class GTutorials;
   public:
      static const GString TypeName;

      //! Creation function
      static GUI::GBaseObject* New();
      GUnitResearchWindow();
      ~GUnitResearchWindow();
      bool OnCreate();
      bool OnDestroy();

      //! Display functions
      void OnShow();
      void OnHide();    

      //! Events Handler
      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller);

      //! Controls
      GUI::GButton* m_pObjExitBtn;
      GUI::GToggle* m_pObjGroundTog;
      GUI::GToggle* m_pObjAirTog;
      GUI::GToggle* m_pObjNavalTog;
      GUI::GToggle* m_pObjStrategicTog;
      GUI::GTabbed* m_pObjResearchTab;      
      
      void Update(REAL32 in_fNewBudget);

   protected:
   private:
      //! research info data
      GResearchInformationHolder*            m_pOriginalInfo;
      GResearchInformationHolder             m_ModifiedInfo;

      //! private objects
      class GFinancingMeter*                 m_vMainFinancing[4];
      class GFinancingMeter*                 m_vSpecFinancing[12];
      class GTechnologyMeterBlue*            m_vSpecTechnology[12];
      class GProgressMeterYellowValueSmall*  m_vSpecProgress[12];

      GUI::GLabel*   m_vFinancingLbl[12];
      GUI::GLabel*   m_vTechnologyLbl[12];
      GUI::GLabel*   m_vProgressLbl[12];

      GFrame*                               m_pModelFrm;

      //! static members
      static EUnitCategory::Enum m_Filter;
      static GUnitResearchWindow* m_pThis;
      static VOID CALLBACK UpdateTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);

      bool  m_bDragging;
      INT32 m_iUpdateTimerId;
          
      //! update screen values functions
      void Update(SP2::EUnitCategory::Enum in_Filter);
      void BudgetAir(REAL64 in_fNewValue);
      void BudgetGround(REAL64 in_fNewValue);
      void BudgetNaval(REAL64 in_fNewValue);
      void BudgetNuclear(REAL64 in_fNewValue);
      void UpdateMaxValues();
      void UpdateBudget();
      void SetRemainingDays(GUI::GLabel* in_pLabel, INT32 in_iNbDays);

      //! Save functions
      void Save();  
      void SaveBudget();
   };

}

#endif //_SP2_UNIT_Research_WINDOW_H_
