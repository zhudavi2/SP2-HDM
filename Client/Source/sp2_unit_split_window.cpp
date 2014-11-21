/**************************************************************
*
* sp2_unit_split_window.cpp
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
#include "../include/sp2_unit_split_window.h"

const GString GUnitSplitWindow::TypeName(L"Unit_Split_Window");

GUI::GBaseObject* GUnitSplitWindow::New()
{
   return new GUnitSplitWindow();
}

GUnitSplitWindow::GUnitSplitWindow() : GUI::GFrame()
{
}

GUnitSplitWindow::~GUnitSplitWindow()
{
}

bool GUnitSplitWindow::OnCreate()
{
   __super::OnCreate();

   m_pObjDownBtn  = (GUI::GButton*)Child(L"frmSplit")->Child(L"frmDown")->Child(L"btnDown");
   m_pObjUpBtn    = (GUI::GButton*)Child(L"frmSplit")->Child(L"frmDown1")->Child(L"btnUp");
   m_pObjExitBtn  = (GUI::GButton*)Child(L"frmTitle")->Child(L"btnClose");
   m_pObjGroupAEdt = (GUI::GEditBox*)Child(L"frmSplit")->Child(L"edtGroupA");
   m_pObjGroupBEdt = (GUI::GEditBox*)Child(L"frmSplit")->Child(L"edtGroupB");
   m_pObjSplitMeter = (SP2::GSplitMeter*)Child(L"frmSplit")->Child(L"frmSplit");
   m_pObjSplitMeter->Owner(this);

   m_pCallingWindow    = NULL;
   m_iQuantityToSplit   = 0;
   m_iQuantityInGroupA  = 0;
   m_iQuantityInGroupB  = 0;
   
   return true;
}


void GUnitSplitWindow::Initialize(GBaseObject *in_pParent,UINT32 in_iQuantityToSplit,float in_fInitialeRatio)
{
   gassert(in_pParent != NULL,"Invalid null pointer");
   gassert(in_iQuantityToSplit > 0, "How do you split zero units ?");
   gassert(in_fInitialeRatio >= 0 && in_fInitialeRatio<=1.0f,"Ratio should be between 0 and 1");
   if( in_pParent == NULL        || 
       in_fInitialeRatio < 0     ||
       in_fInitialeRatio > 1.0f  ||
       in_iQuantityToSplit == 0)
   {
      return;
   }

   // Collect information and calculate group
   m_pCallingWindow     = in_pParent;
   m_iQuantityToSplit   = in_iQuantityToSplit;

   // Update split bar
   m_pObjSplitMeter->Value(in_fInitialeRatio,0,true);
}


GUI::EEventStatus::Enum GUnitSplitWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {

            // Send notification to the parent
            gassert(m_pCallingWindow != NULL,"Split window wasn't initialize");
            m_pCallingWindow->OnCustomEvent(c_iOnSplitWindowClose ,in_EventData,in_pCaller);

            // Kill ourself!
            Hide();

         }
         else if(in_pCaller == m_pObjUpBtn)
         {
            REAL64 l_newValue = (m_iQuantityInGroupB + 1) / (m_iQuantityToSplit * 1.0f);
            m_pObjSplitMeter->Value(l_newValue,0,true);
         }
         else if(in_pCaller == m_pObjDownBtn)
         {
            REAL64 l_newValue = (m_iQuantityInGroupB - 1) / (m_iQuantityToSplit * 1.0f);
            m_pObjSplitMeter->Value(l_newValue,0,true);
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GUnitSplitWindow::OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   switch(in_iEventID)
   {
   // On Meter Change event
      case c_OnMeterChangeEvent:
      {
         UpdateQuantityFromRatio(m_pObjSplitMeter->Value(0));
      }
      break;
   }

   return GUI::EEventStatus::Handled;
}


void GUnitSplitWindow::UpdateQuantityFromRatio(REAL64 in_fRatio)
{
   //The in_fRatio is used like a transfert ratio, from group A to group B.
   m_iQuantityInGroupB  = (UINT32) (in_fRatio * m_iQuantityToSplit);
   m_iQuantityInGroupA  = m_iQuantityToSplit - m_iQuantityInGroupB;
   m_pObjGroupAEdt->Text(GString(m_iQuantityInGroupA));
   m_pObjGroupBEdt->Text(GString(m_iQuantityInGroupB));

   m_pObjDownBtn->Enabled(m_iQuantityInGroupA != m_iQuantityToSplit);
   m_pObjUpBtn->Enabled(m_iQuantityInGroupA != 0);
}

UINT32 GUnitSplitWindow::QuantityInGroupA(void)
{
   return m_iQuantityInGroupA;
}

UINT32 GUnitSplitWindow::QuantityInGroupB(void)
{
   return m_iQuantityInGroupB;
}
