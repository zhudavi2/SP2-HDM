/**************************************************************
*
* sp2_treaties_detailed_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_TREATIES_DETAILED_WINDOW_H_
#define _CSP2_TREATIES_DETAILED_WINDOW_H_

#include "sp2_treaties_window.h"
#include "sp2_frame_cursor_meter.h"

namespace SP2
{

   class GTreatyDetailsWindow : public SP2::GBaseWindow, DesignPattern::GObserver
   {
   public:
      static const GString TypeName;

      GTreatyDetailsWindow();
	   ~GTreatyDetailsWindow();

      bool OnCreate();
      bool OnDestroy();
      void OnShow();
      void OnHide();

		void QuickTreatyWindow(ETreatyType::Enum in_eTreatyType,
										const GString& in_sTypeName,
										UINT32 in_iSideA,
										UINT32 in_iSideB);

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      void OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );

      void  Update(INT32 in_iTreatyID);
      INT32 DisplayedTreatyID();
      GTreaty* DisplayedTreaty(){return m_pCurrentTreaty;}
      void  ApplyCountryConditions(const set<UINT32> & in_Countries);
      void RemoveTreaty(INT32 in_iTreatyID);

      GDataTreatiesWindow* m_pData;
      bool m_bWaitForClose;

   protected:
      friend class GDataDisplayLayer;
      friend class GTreatyWindow;
      static GUI::GBaseObject* New();

   private:
		GUI::EEventStatus::Enum InitializeTreaty(ETreatyType::Enum in_eTreatyType);
      void FillSides(GTreaty* in_pTreaty);
      void FillPressure(GTreaty* in_pTreaty);
      void FillConditions(const UINT32* in_Conditions);
      void FillDescription(ETreatyType::Enum in_eType);
      bool IsTreatyTwoSided(ETreatyType::Enum in_eType);
      void FetchNewAvailableCountryList();
      ETreatyType::Enum FindTreatyType(GString in_sTreatyName);
      INT32 FindCountryIdFromCode(GString in_sCode);
      void UpdateListBoxes();
      void SortPressure();
      void EnableConditions(bool in_bEnabled);
		void OnAddRemoveCountrySelection();

      EConditionStatusRank::Enum     ConditionGetEconomicStrengthSimilitude(const GString & in_sText);
      EConditionStatusRank::Enum     ConditionGetMilitaryStrengthSimilitude(const GString & in_sText);
      EConditionStatusPolitical::Enum ConditionGetPoliticalSimilitude(const GString & in_sText);
      EConditionStatusGeo::Enum      ConditionGetGeographicProximity(const GString & in_sText);
      EConditionStatusRelation::Enum ConditionGetRelationsCloseness(const GString & in_sText);
      EConditionStatusResearch::Enum ConditionGetResearchLevelSimilitude(const GString & in_sText);

      vector<INT32>  m_viSideA;
      vector<INT32>  m_viSideB;
      set<UINT32>    m_siEligibleCountries;

      GString m_sTreatyName;
      
      UINT32  m_pConditions[ETreatyConditions::ItemCount];	

      GUI::GEditBox*    m_pObjNameEdt;
      GUI::GEditBox*    m_pObjNameOverEdt;
      GUI::GComboBox*   m_pObjTypeCbo;
      GUI::GFrame*      m_pObjDescriptionFrm;
      GUI::GLabel*      m_pObjDescriptionLbl;
      GUI::GVScrollBar* m_pObjDescriptionScr;
      GUI::GLabel*      m_pObjSideALbl;
      GUI::GLabel*      m_pObjSideBLbl;
      GUI::GListBox*    m_pObjSideALst;
      GUI::GListBox*    m_pObjSideBLst;
      GUI::GFrame*      m_pObjModifyFrm;
      GUI::GToggle*     m_pObjTreatyOpenTog;
      GUI::GPictureBox* m_pObjStatusPic;
      GUI::GLabel*      m_pObjStatusLbl;

      GUI::GComboBox* m_pObjCondRelations;
      GUI::GComboBox* m_pObjCondGeographicProximity;
      GUI::GComboBox* m_pObjCondPoliticalStrength;
      GUI::GComboBox* m_pObjCondEconomicStrength;
      GUI::GComboBox* m_pObjCondMilitaryStrength;
      GUI::GComboBox* m_pObjCondResearchLevel;

      GUI::GLabel* m_pObjCondRelationsLbl;
      GUI::GLabel* m_pObjCondGeographicProximityLbl;
      GUI::GLabel* m_pObjCondPoliticalStrengthLbl;
      GUI::GLabel* m_pObjCondEconomicStrengthLbl;
      GUI::GLabel* m_pObjCondMilitaryStrengthLbl;
      GUI::GLabel* m_pObjCondResearchLevelLbl;

      GUI::GButton*   m_pObjModifyBtn;
      GUI::GButton*   m_pObjRefuseBtn;

      GUI::GButton*   m_pObjConfirmBtn;
      GUI::GButton*   m_pObjCancelBtn;
      GUI::GButton*   m_pObjRemoveNotEligibleBtn;

      GUI::GButton*   m_pObjSponsorBtn;
      GUI::GListBox*  m_pObjPressureLst;
      GUI::GLabel*    m_pObjPressureTxt;
      SP2::GGradientBar* m_pObjPressureBar;

      GUI::GListBox*  m_pObjEligibleLst;

      GTreaty* m_pCurrentTreaty;

      INT32 m_iTreatyID;

      bool m_bTwoSidesTreaty;
      bool m_bSideADirty;
      bool m_bSideBDirty;
      bool m_bCountryNotEligible;
      bool m_bWaitForEligibleAnswer;
      bool m_bTreatyIsActive;

      bool m_bTreatySameName;
      bool m_bSingleOnSide;
      bool m_bSkipEligibleWarning;
      bool m_bIsEditingName;
   };
};

#endif //_CSP2_TREATIES_DETAILED_WINDOW_H_
