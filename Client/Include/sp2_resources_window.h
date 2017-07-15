/**************************************************************
*
* sp2_resources_window.h
*
* Description
* ===========
*  [A quick overview of the content in both files (.h and .cpp)]
*
* Owner
* =====
*  François Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _CSP2_Resources_H_
#define _CSP2_Resources_H_

#include "../include/sp2_frame_updatable_value.h"

namespace SP2
{ 
   class GResourcesWindow : public SP2::GBaseWindow, DesignPattern::GObserver, SP2::GRequestObject
   {
   public:
	   GResourcesWindow();
	   ~GResourcesWindow();

	   bool OnCreate();
      bool OnDestroy();

      bool Enabled();
      void Enabled(bool in_bEnabled);

	   //! called when the button is released into our menu
	   GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller);
      GUI::EEventStatus::Enum OnMouseWheel(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller);
      void OnShow();
      void OnHide();
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventID, const GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      void OnSubjectChange(DesignPattern::GSubject&  in_Subject,const DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );

      void Update(bool in_bForce = false);
      void Save();

      GDataResourcesWindow* m_pData;

      static const GString TypeName;
   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

      SP2::GUpdatableValue* m_pObjGlobalTaxMod;
      SP2::GUpdatableValue* m_pObjSectorTax;
      SP2::GUpdatableValue* m_pObjDesired;
      GUI::GLabel*          m_pObjGlobalTaxModLbl;
      GUI::GLabel*          m_pObjPartGdp;
      GUI::GLabel*          m_pObjMarketShare;
      GUI::GLabel*          m_pObjMarketAvail;
      GUI::GLabel*          m_pObjActual;
      GUI::GComboBox*       m_pObjManagement;
      GUI::GComboBox*       m_pObjStatus;
      GUI::GComboBox*       m_pObjTrade;
      GUI::GButton*         m_pObjIncreaseProduction;
      GUI::GToggle*         m_pObjMeetConsumption;
      GUI::GVScrollBar*     m_pObjScroll;
      GUI::GFrame*          m_pObjInnerFrame;
      GUI::GFrame*          m_pObjResourcesFrm;
      GUI::GFrame*          m_pObjInnerResourcesFrm;

      GUI::GLabel*          m_pObjManagementLbl;
      GUI::GLabel*          m_pObjStatusLbl;
      GUI::GLabel*          m_pObjSectorTaxLbl;
      GUI::GLabel*          m_pObjPartGdpLbl;
      GUI::GLabel*          m_pObjMarketShareLbl;
      GUI::GLabel*          m_pObjTradeLbl;
      GUI::GLabel*          m_pObjDesiredLbl;
      GUI::GLabel*          m_pObjActualLbl;
      GUI::GLabel*          m_pObjMeetComsumptionLbl;
      GUI::GLabel*          m_pObjMarketAvailLbl;

      GUI::GTreeView*       m_pObjTree[6];

   private:
      GUI::GTreeBranch *PrepareTreeResourceItem(GUI::GTreeBranch * node, GString in_sName);
      
      void UpdateResourcesFrame();
      void DisplayResourceInfo();
      void FormatCurrency(GUI::GLabel* in_pLabel, REAL64 in_fValue, bool in_bColored = false, bool in_bSelected = false, bool in_bSum = false);
      void UpdateTreePosition();

      GUI::GTreeBranch* m_pLastSelectedNode;
      GUI::GTreeBranch* m_pSelectedNode;

      INT32 m_iSelectedId;
   };

};
#endif //_CSP2_Resources_H_
