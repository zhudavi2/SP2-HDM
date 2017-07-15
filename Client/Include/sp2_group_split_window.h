/**************************************************************
*
* sp2_group_split_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_GroupSplitWindow_H_
#define _CSP2_GroupSplitWindow_H_

namespace SP2
{

   class GGroupSplitWindow : public GBaseWindow
   {
   public:
	   GGroupSplitWindow();
	   ~GGroupSplitWindow();

      //! update must be called to display
      void Update(INT32 in_iGroupID, bool in_bSplitEqually = false);

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

	   bool OnCreate();
      void OnShow();
      void OnHide();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnCustomEvent(UINT32 in_EventID, const GEventData & in_Data, GBaseObject* in_pCaller = NULL);

      GUI::GListBox* m_pObjOriginalLst;
      GUI::GListBox* m_pObjNewLst;

      GUI::GButton*  m_pObjToLeftBtn;
      GUI::GButton*  m_pObjToRightBtn;
      GUI::GButton*  m_pObjSplitEquallyBtn;

   private:
      void SwapRow( GUI::GListBox* in_pFrom, GUI::GListBox* in_pTo, INT32 in_iQtyToSwap );
      void SetUpRowWithUnit(GUI::GListBox::GRow* in_pRow, SDK::Combat::GUnit* in_pUnit, UINT32 in_QtyToDisplay);

      //! set up the window if we can plit
      bool CanSplit();

      //-------RowID,      UnitID, Qty-------------//
      hash_map<INT32, pair<INT32, INT32> > m_Original;
      hash_map<INT32, pair<INT32, INT32> > m_New;

      INT32 m_iOriginalLastSelectedRowID;
      INT32 m_iNewLastSelectedRowID;
      
      INT32 m_iGroupID;
      bool  m_bFromLeft;
   };
};

#endif //_CSP2_GroupSplitWindow_H_
