/**************************************************************
*
* sp2_unit_split_window.h
*
* Description
* ===========
*  Describes the interface of class GUnitSplitWindow
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _SP2_UNIT_SPLIT_WINDOW_H_
#define _SP2_UNIT_SPLIT_WINDOW_H_

#include "sp2_frame_cursor_meter.h"

const UINT32 c_iOnSplitWindowClose = 3541239874;

namespace SP2
{

   /*!
   * Unit Split window
   **/
   class GUnitSplitWindow : public GUI::GFrame
   {
   public:
      static const GString TypeName;
      //! Creation function
      static GUI::GBaseObject* New();
      GUnitSplitWindow();
      ~GUnitSplitWindow();

      bool OnCreate();
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller = 0);

      void Initialize(GBaseObject *in_pParent,UINT32 in_iQuantityToSplit,float in_fInitialeRatio = 0.5f);

      UINT32   QuantityInGroupA(void);
      UINT32   QuantityInGroupB(void);

   protected:
      friend class GDataDisplayLayer;

      GUI::GButton*  m_pObjDownBtn;
      GUI::GButton*  m_pObjUpBtn;
      GUI::GButton*  m_pObjExitBtn;
      GUI::GEditBox* m_pObjGroupAEdt;
      GUI::GEditBox* m_pObjGroupBEdt;
      SP2::GSplitMeter* m_pObjSplitMeter;

   private:

      UINT32         m_iQuantityToSplit;
      UINT32         m_iQuantityInGroupA;
      UINT32         m_iQuantityInGroupB;
      GBaseObject *  m_pCallingWindow;

      //! Update quantity label from a ratio.
      void UpdateQuantityFromRatio(REAL64 in_pRatio);
   };

};//end namespace SP2
#endif //_SP2_UNIT_List_WINDOW_H_
