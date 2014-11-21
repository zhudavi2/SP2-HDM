/************************************************
* File:  sp2_editbox_numeric.h
* Owner: Francois Durand
* Description:
*	  Describes the numeric edit box
*
*************************************************/

#ifndef _GEditBoxNumeric_H_
#define _GEditBoxNumeric_H_

#include "../../includes/SDK/golem_generic_editbox.h"

const UINT32 c_OnNumericEditBoxValueChange = 2143486739;

/*!
* Represents a basic Edit box (accepts input)
**/ 
namespace SP2
{
   class GEditBoxNumeric : public GUI::GEditBox
   {
   public:

      bool OnCreate();
      bool OnDestroy();
      GUI::EEventStatus::Enum OnChar(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnKeyDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnKeyUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnHotkey(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller = 0);

      bool OnGotFocus(const SDK::GUI::GBaseObject * in_pFocusFrom, SDK::GUI::GBaseObject * Caller = 0);
      bool OnLoseFocus(const SDK::GUI::GBaseObject * in_pFocusTo, SDK::GUI::GBaseObject * Caller = 0);

      void Initialize(GString in_sPrefix, GString in_sSuffix, UINT8 in_iPrecision, UINT8 in_iGrouping);
      void Value(REAL64 in_fValue);
      REAL64 Value();
      bool Text(const GString &);
      GString Text();

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

      GEditBoxNumeric();
      ~GEditBoxNumeric();

   private:
      GString m_sSuffix;
      GString m_sPrefix;
      REAL64  m_fValue;
      UINT32  m_iHotKeyID;
      UINT8   m_iPrecision;
      UINT8   m_iGrouping;
   };
}

#endif //_GEditBoxNumeric_H_
