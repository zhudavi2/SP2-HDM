/************************************************
* File:  sp2_editbox_numeric.cpp
* Owner: Francois Durand
* Description:
*	  Describes the numeric edit box
*
*************************************************/
#include "golem_pch.hpp"
#include "../include/sp2_editbox_numeric.h"

using namespace SP2;

const GString GEditBoxNumeric::TypeName(L"edtNumeric");

/*!
 * Edit box constructor
 **/
GUI::GBaseObject* GEditBoxNumeric::New()
{
   return new GEditBoxNumeric();
}

GEditBoxNumeric::GEditBoxNumeric() : GUI::GEditBox()
{
   m_iGrouping  = 3;
   m_iPrecision = 0;
   m_sSuffix = L"";
   m_sPrefix = L"";
   m_fValue  = 0.f;
}

GEditBoxNumeric::~GEditBoxNumeric()
{
}

bool GEditBoxNumeric::OnCreate()
{
   __super::OnCreate();

   //This is used to override ESCAPE hot key that brings main menu
   m_iHotKeyID = g_Joshua.GUIManager()->Hotkeys.Register(GKeyboardInfo(VK_ESCAPE, false, false, false), this);

   return true;
}

bool GEditBoxNumeric::OnDestroy()
{
   __super::OnDestroy();

   g_Joshua.GUIManager()->Hotkeys.Unregister(m_iHotKeyID);

   return true;
}

GUI::EEventStatus::Enum GEditBoxNumeric::OnChar(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   switch(in_EventData.Key())
   {
   //From 0 to 9
   case 48:
   case 49:
   case 50:
   case 51:
   case 52:
   case 53:
   case 54:
   case 55:
   case 56:
   case 57:
      break;
   
   //.
   case 46:
      if(m_iPrecision)
         break;
      else
         return EEventStatus::Handled;
   
   case VK_BACK:
      break;

   case VK_RETURN:
      {
         m_fValue = __super::Text().ToREAL64();
         g_Joshua.GUIManager()->Focus(this->Owner());
         GEventData l_Data;
         l_Data.Data = (void*)this;
         FireCustomEvent(c_OnNumericEditBoxValueChange, l_Data);
      }
      return EEventStatus::Handled;
   
   default:
      return EEventStatus::Handled;
   }

   __super::OnChar(in_EventData, in_pCaller);

   return EEventStatus::Handled;
}

GUI::EEventStatus::Enum GEditBoxNumeric::OnHotkey(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_EventData.Key() == VK_ESCAPE)
   {
      g_Joshua.GUIManager()->Focus(this->Owner());
      return EEventStatus::Handled;
   }

   return EEventStatus::NotHandled;
}

GUI::EEventStatus::Enum GEditBoxNumeric::OnKeyDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   __super::OnKeyDown(in_EventData, in_pCaller);

   return EEventStatus::Handled;
}

GUI::EEventStatus::Enum GEditBoxNumeric::OnKeyUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   __super::OnKeyUp(in_EventData, in_pCaller);

   return EEventStatus::Handled;
}

bool GEditBoxNumeric::OnGotFocus(const GUI::GBaseObject * in_pFocusFrom, GUI::GBaseObject * in_pCaller)
{
   if(Enabled())
      Text(GString::FormatNumber(m_fValue, L"", L".", L"", L"", 0, m_iPrecision));

   __super::OnGotFocus(in_pFocusFrom, in_pCaller);

   return true;
}

bool GEditBoxNumeric::OnLoseFocus(const GUI::GBaseObject * in_pFocusTo, GUI::GBaseObject * in_pCaller)
{
   __super::OnLoseFocus(in_pFocusTo, in_pCaller);

   if(Enabled())
   {
      m_fValue = __super::Text().ToREAL64();
      GEventData l_Data;
      l_Data.Data = (void*)this;
      FireCustomEvent(c_OnNumericEditBoxValueChange, l_Data);

      Text(GString::FormatNumber(m_fValue, L" ", L".", m_sPrefix, m_sSuffix, m_iGrouping, m_iPrecision));
   }

   return true;
}

void GEditBoxNumeric::Initialize(GString in_sPrefix, GString in_sSuffix, UINT8 in_iPrecision, UINT8 in_iGrouping)
{
   m_sPrefix = in_sPrefix;
   m_sSuffix = in_sSuffix;
   m_iPrecision = in_iPrecision;
   m_iGrouping  = in_iGrouping;
}

void GEditBoxNumeric::Value(REAL64 in_fValue)
{
   m_fValue = in_fValue;

   GString l_sText = GString::FormatNumber(m_fValue, L" ", L".", m_sPrefix, m_sSuffix, m_iGrouping, m_iPrecision);
   Text(l_sText);
}

REAL64 GEditBoxNumeric::Value()
{
   return m_fValue;
}

bool GEditBoxNumeric::Text(const GString & in_sText)
{
   __super::Text(in_sText);

   return true;
}

GString GEditBoxNumeric::Text()
{
   return __super::Text();
}