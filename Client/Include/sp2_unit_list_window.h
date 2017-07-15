/**************************************************************
*
* sp2_unit_list_window.h
*
* Description
* ===========
*  Describes the interface of class GUnitListWindow
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _SP2_UNIT_List_WINDOW_H_
#define _SP2_UNIT_List_WINDOW_H_

#include "sp2_frame_cursor_meter.h"

namespace SP2
{
   /*!
   * Error notification window
   **/
   class GUnitListWindow : public GBaseWindow
   {
      friend class GTutorials;
   public:
      static const GString TypeName;
      //! Creation function
      static GUI::GBaseObject* New();
      GUnitListWindow();
      ~GUnitListWindow();

      bool OnCreate();
      void OnShow();
      void OnHide();
      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseWheel(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32, const GUI::GEventData &, GUI::GBaseObject *);
      GUI::GTreeView*    m_pObjUnitTree;
      GUI::GFrame*       m_pObjAccuracyOfInformationFrm;
      GUI::GFrame*       m_pObjButtonsFrm;
      GUI::GButton*      m_pObjTrainBtn;
      GUI::GButton*      m_pObjSellBtn;
      GUI::GButton*      m_pObjMergeBtn;
      GUI::GButton*      m_pObjSplitBtn;
      GUI::GVScrollBar*  m_pObjTreeScr;
      GUI::GFrame*       m_pObjTreeFrm;
      SP2::GGradientBar* m_pObjAccuracy;
      GUI::GLabel*       m_pObjAccuracyTxt;

      void GenerateUnitTree();
      GTreeBranch* PrepareTreeItem(GTreeBranch* node, GString in_sUnitName, INT32 in_iUnitQty, SP2::EMilitaryStatus::Enum in_Status = SP2::EMilitaryStatus::Ready,INT32 in_iUnitId = -1);
      bool Update();

      void UpdateFromSelectedUnits(bool in_bState);

   protected:
      friend class GDataDisplayLayer;
      friend class GMilitaryWindow;

   private:

      void     SellUnit(UINT32 in_iUnitId);


      bool m_bUpdateFromSelectedUnits;
      GString           m_UnitTrainingIDLogo[SP2::ETrainingLevel::ItemCount];
      INT32 m_iSelectedUnitId;
      GUI::GFrame* m_pSelectedItem ;
		Random::GQuick		m_Rand;
   };

};//end namespace SP2
#endif //_SP2_UNIT_List_WINDOW_H_
