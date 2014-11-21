/**************************************************************
*
* sp2_covert_actions_new_cell.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_GCovertActionsNewCellWindow_H_
#define _CSP2_GCovertActionsNewCellWindow_H_

namespace SP2
{

   class GCovertActionsNewCellWindow : public GBaseWindow
   {
   public:
	   GCovertActionsNewCellWindow();
	   ~GCovertActionsNewCellWindow();

	   bool OnCreate();
      void OnShow();
      void OnHide();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnCustomEvent(UINT32 in_iType, const GEventData & in_EventData, GBaseObject* in_pCaller = 0);

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

   private:
      void CalculateCost();
      
      GUI::GEditBox*  m_pObjNameEdt;
      GUI::GComboBox* m_pObjTraining;
      GUI::GLabel*    m_pObjSetupCostLbl;
      GUI::GLabel*    m_pObjUpkeepLbl;
      GUI::GLabel*    m_pObjUpdateTimeLbl;
      SP2::GComboListBoxLess* m_pObjCountryCbo;

      GUI::GButton*   m_pObjConfirmBtn;
      GUI::GButton*   m_pObjCancelBtn;
   };
};

#endif //_CSP2_GCovertActionsNewCellWindow_H_
