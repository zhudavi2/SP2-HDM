/**************************************************************
*
* sp2_frame_updatable_value.h
*
* Description
* ===========
*  Describes the interface of class GUpdatableValue
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _SP2_FRAME_UPDATABLE_VALUE_H_
#define _SP2_FRAME_UPDATABLE_VALUE_H_

#include "sp2_editbox_numeric.h"

const UINT32 c_OnUpdatableValueChange = 2143486735;

namespace SP2
{

   /*!
   * Defines the progress meter.
   **/
   class GUpdatableValue : public GUI::GFrame
   {
   public:
      static const GString TypeName;

      GUpdatableValue();
      ~GUpdatableValue();

      bool OnCreate();
      bool OnDestroy();
      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller = 0);

      SP2::GEditBoxNumeric* TextValueObj();

      void   Value(REAL64 in_fValue, bool in_bUpdateLinked = true, bool in_bCallEvent = true);
      REAL64 Value();

      void   Initialize(vector<UINT32> in_viNbStep, vector<REAL64> in_vfStep, GString in_sPrefix, GString in_sSuffix, UINT8 in_iPrecision, UINT8 in_iGrouping, REAL64 in_fMinimum, REAL64 in_fMaximum);
      void   Initialize(GString in_sPrefix, GString in_sSuffix, UINT8 in_iPrecision, UINT8 in_iGrouping, REAL64 in_fMinimum, REAL64 in_fMaximum);
      void   SetStep(vector<UINT32> in_viNbStep, vector<REAL64> in_vfStep);
      void   SetStep();
      void   AddLinkedCursor(GUpdatableValue* in_pObj);

      void   UpdateTimerProc(bool in_bIncrement);
      void   Enabled(bool in_bEnabled);
      bool   Enabled();
      void   Minimum(REAL64 in_fValue);
      REAL64 Minimum();
      void   Maximum(REAL64 in_fValue);
      REAL64 Maximum();

      UINT32 m_iCount;
   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

      GUI::GButton* m_pUpBtn;
      GUI::GButton* m_pDownBtn;
      SP2::GEditBoxNumeric*  m_pTextLbl;

   private:
      REAL64  m_fValue;
      REAL64  m_fMinimum;
      REAL64  m_fMaximum;
      GString m_sSuffix;
      GString m_sPrefix;
      UINT8   m_iPrecision;
      UINT8   m_iGrouping;
      vector<GUpdatableValue*> m_vpLinked;
      vector<UINT32> m_viNbStep;
      vector<REAL64> m_vfStep;
   };
};

#endif //_SP2_FRAME_UPDATABLE_VALUE_H_