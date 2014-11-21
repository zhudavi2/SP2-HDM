/**************************************************************
*
* sp2_covert_actions_new_Mission.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_GCovertActionsNewMissionWindow_H_
#define _CSP2_GCovertActionsNewMissionWindow_H_

namespace SP2
{

   class GCovertActionsNewMissionWindow : public GBaseWindow
   {
   public:
	   GCovertActionsNewMissionWindow();
	   ~GCovertActionsNewMissionWindow();

	   bool OnCreate();
      void OnShow();

      void UpdateComboBox();

      void CellCleanUp();
      void AddCell(UINT32 in_iCellID);
      void Show();

      void UpdateCountryToFrameCbo();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_Data, SDK::GUI::GBaseObject* in_pCaller = 0);
      
      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

   private:
      GUI::GComboBox* m_pObjTypeCbo;
      GUI::GComboBox* m_pObjSectorCbo;
      GUI::GComboBox* m_pObjComplexityCbo;
      GUI::GComboBox* m_pObjResourceCbo;
      GUI::GLabel*    m_pObjTargetLbl;
      GUI::GComboBox* m_pObjFramedCbo;
      GUI::GLabel*    m_pObjResourceLbl;
      GUI::GLabel*    m_pObjSectorLbl;

      GUI::GButton* m_pObjConfirm;
      GUI::GButton* m_pObjCancel;

      GUI::GListBox::GRow *m_pRowCoupDetat;

      vector<UINT32> m_viCellsID;
   };
};

#endif //_CSP2_GCovertActionsNewMissionWindow_H_
