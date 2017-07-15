/**************************************************************
*
* sp2_covert_actions_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_CovertActionsWindow_H_
#define _CSP2_CovertActionsWindow_H_

namespace SP2
{

   class GCovertActionsWindow : public GBaseWindow, DesignPattern::GObserver
   {
   public:
	   GCovertActionsWindow();
	   ~GCovertActionsWindow();

	   bool OnCreate();
      bool OnDestroy();
      void OnShow();
      void OnHide();
      bool Update();

      void Iterate();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnCustomEvent(UINT32 in_iType, const SDK::GUI::GEventData & in_EventData, GBaseObject* in_pCaller = 0);
      void OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );

      void SelectCellRow(UINT32 in_ID);

      void ClearBeforeUpdate(bool in_bClearBeforeUpdate);
      
      static const GString TypeName;
      REAL64 m_fWhen;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

   private:
      void UpdateNationalSecurity();

      void RebuildShownCells(UINT32 in_iID);
      void DeselectListRows(const vector<INT32> &in_Rows);
      void SelectListRows(const vector<INT32> &in_Rows);
      
      GUI::GFrame*   m_pObjInnerFrm;
      GUI::GListBox* m_pObjCellLst;
      GUI::GButton*  m_pObjNewCellBtn;
      GUI::GButton*  m_pObjNewMissionBtn;
      GUI::GButton*  m_pObjTrainCellBtn;
      GUI::GButton*  m_pObjDisbandCellBtn;
      GUI::GButton*  m_pObjCancelMissionBtn;

      GUI::GLabel*   m_pObjMissionTypeLbl;
      GUI::GLabel*   m_pObjSectorLbl;
      GUI::GLabel*   m_pObjTypeLbl;
      GUI::GLabel*   m_pObjComplexityLbl;
      GUI::GLabel*   m_pObjFramedCountryLbl;
      GUI::GLabel*   m_pObjTotalLbl;
      GUI::GLabel*   m_pObjProtectionLbl;

      hash_map<UINT32, UINT32> m_ShownCell;
      hash_map<UINT32, bool>   m_JustDidMission;

      vector<INT32>  m_viLastSelectedRows;
      bool           m_bClearBeforeUpdate;
      bool           m_bIgnoreNextEvent;
   };
};

#endif //_CSP2_CovertActionsWindow_H_
