/**************************************************************
*
* sp2_frame_updatable_value.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_frame_updatable_value.h"
#include "../include/sp2_data_display_layer.h"

const GString GUpdatableValue::TypeName(L"frmUpdatableValue");

GUI::GBaseObject* GUpdatableValue::New()
{
   return new GUpdatableValue();
}

GUpdatableValue::GUpdatableValue()
{
   m_iCount   = 0;
   m_pUpBtn   = NULL;
   m_pDownBtn = NULL;
   m_pTextLbl = NULL;
}

GUpdatableValue::~GUpdatableValue()
{
}

bool GUpdatableValue::OnCreate()
{
   __super::OnCreate();

   m_pUpBtn    = (GUI::GButton*)Child(L"btnUp");
   m_pDownBtn  = (GUI::GButton*)Child(L"btnDown");
   m_pTextLbl  = (SP2::GEditBoxNumeric*)Child(L"edtText");
   m_iGrouping = 3;
   m_iPrecision= 0;

   return true;
}

bool GUpdatableValue::OnDestroy()
{
   __super::OnDestroy();

   return true;
}

SP2::GEditBoxNumeric* GUpdatableValue::TextValueObj()
{
   return m_pTextLbl;
}

GUI::EEventStatus::Enum GUpdatableValue::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(!Enabled())
      return GUI::EEventStatus::Callback;

   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(!m_viNbStep.size())
         SetStep();

      SP2::GUpdateValueData l_Data;

      m_iCount = 0;

      if(in_pCaller == m_pUpBtn)
      {
         l_Data.m_bIncrement = true;
         Value(Value() + m_vfStep[0]);
      }
      else if(in_pCaller == m_pDownBtn)
      {
         l_Data.m_bIncrement = false;
         Value(Value() - m_vfStep[0]);
      }

      l_Data.m_pObject = this;
      l_Data.m_fWhen   = g_Joshua.TimeCounter().GetPrecisionTime() + 0.5f;
      
      g_ClientDDL.AddToIterateUpdateCursor(l_Data);
   }

   return GUI::EEventStatus::Callback;
}

GUI::EEventStatus::Enum GUpdatableValue::OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(!Enabled())
      return GUI::EEventStatus::Callback;

   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      g_ClientDDL.RemoveFromIterateUpdateCursor(this);  

   return GUI::EEventStatus::Callback;
}

GUI::EEventStatus::Enum GUpdatableValue::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   return GUI::EEventStatus::Callback;
}

GUI::EEventStatus::Enum GUpdatableValue::OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   switch(in_iEventID)
   {
   case c_OnNumericEditBoxValueChange:
      Value(TextValueObj()->Value());
      break;
   }
   
   return GUI::EEventStatus::Callback;
}

void GUpdatableValue::Initialize(vector<UINT32> in_viNbStep, vector<REAL64> in_vfStep, GString in_sPrefix, GString in_sSuffix, UINT8 in_iPrecision, UINT8 in_iGrouping, REAL64 in_fMinimum, REAL64 in_fMaximum)
{ 
   Initialize(in_sPrefix, in_sSuffix, in_iPrecision, in_iGrouping, in_fMinimum, in_fMaximum);
   SetStep(in_viNbStep, in_vfStep);
}

void GUpdatableValue::Initialize(GString in_sPrefix, GString in_sSuffix, UINT8 in_iPrecision, UINT8 in_iGrouping, REAL64 in_fMinimum, REAL64 in_fMaximum)
{
   m_sPrefix    = in_sPrefix;
   m_sSuffix    = in_sSuffix;
   m_iPrecision = in_iPrecision;
   m_iGrouping  = in_iGrouping;
   m_fMinimum   = in_fMinimum;
   m_fMaximum   = in_fMaximum;

   m_pTextLbl->Initialize(m_sPrefix, m_sSuffix, m_iPrecision, m_iGrouping);
}

void GUpdatableValue::SetStep(vector<UINT32> in_viNbStep, vector<REAL64> in_vfStep)
{
   if(!in_viNbStep.size())
   {
      m_viNbStep.clear();
      m_viNbStep.push_back(0);
      m_viNbStep.push_back(10);
      m_viNbStep.push_back(20);
      m_viNbStep.push_back(35);
   }
   else
   {
      m_viNbStep   = in_viNbStep;
   }

   if(!in_vfStep.size())
   {
      REAL64 l_fIncrement = 1.f;
      l_fIncrement = l_fIncrement / 100000;

      m_vfStep.clear();
      
      while((m_fValue / l_fIncrement) > 1000)
      {
         l_fIncrement *= 10;
      }
      l_fIncrement = ceil( m_fValue / (l_fIncrement * 10) ) * (l_fIncrement * 10);

   }
   else
   {
      m_vfStep     = in_vfStep;
   }
}

void GUpdatableValue::SetStep()
{
   m_viNbStep.clear();
   m_vfStep.clear();

   m_viNbStep.push_back(0);
   m_viNbStep.push_back(10);
   m_viNbStep.push_back(20);
   m_viNbStep.push_back(35);

   REAL64 l_fIncrement = 1.f;
   l_fIncrement = l_fIncrement / 100000;

   while((m_fValue / l_fIncrement) > 1000)
   {
      l_fIncrement *= 10;
   }

   m_vfStep.push_back(l_fIncrement);
   m_vfStep.push_back(l_fIncrement * 10);
   m_vfStep.push_back(l_fIncrement * 100);
   m_vfStep.push_back(l_fIncrement * 1000);
}

void GUpdatableValue::AddLinkedCursor(GUpdatableValue* in_pObj)
{
   m_vpLinked.push_back(in_pObj);
   in_pObj->m_vpLinked.push_back(this);
}

void GUpdatableValue::Value(REAL64 in_fValue, bool in_bUpdateLinked, bool in_bCallEvent)
{
   if(in_fValue == m_fValue && TextValueObj()->Text() != L"")
      return;

   if(in_bUpdateLinked)
   {
      REAL64 l_fToDistribute = in_fValue - m_fValue;
      if(l_fToDistribute == 0)
         return;
      
      if(m_fMinimum > in_fValue)
      {
         l_fToDistribute = m_fMinimum - m_fValue;
      }
      
      if(m_fMaximum < in_fValue)
      {
         l_fToDistribute = m_fMaximum - m_fValue;
      }
      
      INT32  l_iVecSize = (INT32)m_vpLinked.size();
      INT32  l_iVecSizeLocked = 0;
      
      for(INT32 i=0; i<l_iVecSize; i++)
      {
         if(!Enabled())
            l_iVecSizeLocked++;
      }

      for(INT32 i=0; i<l_iVecSize; i++)
      {
         if(!m_vpLinked[i]->Enabled())
            continue;

         if(l_iVecSize == 1)
         {
            m_vpLinked[i]->Value(m_vpLinked[i]->Value() - l_fToDistribute, false);
         }
         else
         {
            if(m_fValue == 0 && l_fToDistribute)
            {
               m_vpLinked[i]->Value( l_fToDistribute / (l_iVecSize - l_iVecSizeLocked) );
            }
            else
            {
               REAL64 l_fRatio = m_vpLinked[i]->Value() / m_fValue;
               m_vpLinked[i]->Value( m_vpLinked[i]->Value() + (l_fToDistribute * l_fRatio), false );
            }
         }
      }
   }

   m_fValue = in_fValue;
   
   if(m_fMinimum > m_fValue)
   {
      m_fValue = m_fMinimum;
   }
   if(m_fMaximum < m_fValue)
   {
      m_fValue = m_fMaximum;
   }

   m_pTextLbl->Value(m_fValue);
   
   if(in_bCallEvent)
   {
      SDK::GUI::GEventData l_Data;
      l_Data.Data = (void*)this;
      Owner()->FireCustomEvent(c_OnUpdatableValueChange, l_Data, true, this );
   }
}

REAL64 GUpdatableValue::Value()
{
   return m_fValue;
}

void GUpdatableValue::UpdateTimerProc(bool in_bIncrement)
{
   //find the next increment value   
   REAL64 l_fIncrement = m_vfStep[0];
   
   for(INT32 i=(INT32)m_viNbStep.size() - 1; i>=0; i--)
   {
      if(m_viNbStep[i] <= m_iCount)
      {
         l_fIncrement = m_vfStep[i];
         break;
      }
   }

   if(!in_bIncrement)
   {
      l_fIncrement = -l_fIncrement;
   }

   Value(Value() + l_fIncrement);

   m_iCount++;
}

void GUpdatableValue::Enabled(bool in_bEnabled)
{
   if(!m_pDownBtn || !m_pUpBtn)
      return;

   m_bEnabled = in_bEnabled;

   if(m_bEnabled)
   {
      m_pDownBtn->Enabled(true);
      m_pUpBtn->Enabled(true);
      m_pTextLbl->Enabled(true);
   }
   else
   {
      m_pDownBtn->Enabled(false);
      m_pUpBtn->Enabled(false);
      m_pTextLbl->Enabled(false);
   }
}

bool GUpdatableValue::Enabled()
{
   return m_bEnabled;
}

REAL64 GUpdatableValue::Minimum()
{
   return m_fMinimum;
}

void GUpdatableValue::Minimum(REAL64 in_fValue)
{
   m_fMinimum = in_fValue;

   if(Value() < m_fMinimum)
   {
      Value(m_fMinimum, true, false);
   }
}

REAL64 GUpdatableValue::Maximum()
{
   return m_fMaximum;
}

void GUpdatableValue::Maximum(REAL64 in_fValue)
{
   m_fMaximum = in_fValue;

   if(Value() > m_fMaximum)
   {
      Value(m_fMaximum, true, false);
   }
}
