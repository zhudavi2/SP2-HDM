/**************************************************************
*
* sp2_increase_production_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_increase_production_window.h"
#include "../include/sp2_resources_window.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"
#include <limits.h>

const GString GIncreaseProductionWindow::TypeName(L"Increase_Production_Window");

GUI::GBaseObject* GIncreaseProductionWindow::New()
{
   return (GUI::GBaseObject*) new GIncreaseProductionWindow();
}

GIncreaseProductionWindow::GIncreaseProductionWindow() : SP2::GBaseWindow()
{
   m_pObjInnerFrm       = NULL;

   m_pObjIncrease1Btn  = NULL;
   m_pObjIncrease2Btn  = NULL;
   m_pObjIncrease5Btn   = NULL;
   m_pObjIncrease10Btn  = NULL;
   m_pObjIncrease25Btn  = NULL;

   m_pObjIncrease1Txt  = NULL;
   m_pObjIncrease2Txt  = NULL;
   m_pObjIncrease5Txt   = NULL;
   m_pObjIncrease10Txt  = NULL;
   m_pObjIncrease25Txt  = NULL;
}

GIncreaseProductionWindow::~GIncreaseProductionWindow()
{
}

GUI::EEventStatus::Enum GIncreaseProductionWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{  
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(in_pCaller == m_pObjIncrease1Btn)
         Dirty(true);

      if(in_pCaller == m_pObjIncrease2Btn)
         Dirty(true);

      if(in_pCaller == m_pObjIncrease5Btn)
         Dirty(true);

      if(in_pCaller == m_pObjIncrease10Btn)
         Dirty(true);

      if(in_pCaller == m_pObjIncrease25Btn)
         Dirty(true);

      if(in_pCaller == m_pObjApplyBtn)
      {
         GEventData l_Data;
         l_Data.Data = (void*)0;

         if(m_pObjIncrease1Btn->ToggleState())
            l_Data.Data = (void*)1;

         if(m_pObjIncrease2Btn->ToggleState())
            l_Data.Data = (void*)2;

         if(m_pObjIncrease5Btn->ToggleState())
            l_Data.Data = (void*)5;

         if(m_pObjIncrease10Btn->ToggleState())
            l_Data.Data = (void*)10;

         if(m_pObjIncrease25Btn->ToggleState())
            l_Data.Data = (void*)25;

         gassert(l_Data.Data != (void*)0, "Increase production of 0% maybe there is a problem somewhere");

         g_ClientDDL.ResourcesWindow()->FireCustomEvent(c_OnResourceIncreaseProduction, l_Data, true, this);
      }

      if(in_pCaller == m_pObjExitBtn)
      {
         Hide();
      }
   }

   return GUI::EEventStatus::Handled;
}

bool GIncreaseProductionWindow::OnCreate()
{
   __super::OnCreate();

   m_pObjInnerFrm       = (GUI::GFrame*)Child(L"frmInner")->Child(L"frmInner");

   m_pObjIncrease1Btn   = (GUI::GToggle*)m_pObjInnerFrm->Child(L"togIncrease1");
   m_pObjIncrease2Btn   = (GUI::GToggle*)m_pObjInnerFrm->Child(L"togIncrease2");
   m_pObjIncrease5Btn   = (GUI::GToggle*)m_pObjInnerFrm->Child(L"togIncrease5");
   m_pObjIncrease10Btn  = (GUI::GToggle*)m_pObjInnerFrm->Child(L"togIncrease10");
   m_pObjIncrease25Btn  = (GUI::GToggle*)m_pObjInnerFrm->Child(L"togIncrease25");

   m_pObjIncrease1Txt   = (GUI::GLabel*)m_pObjInnerFrm->Child(L"txtIncrease1");
   m_pObjIncrease2Txt   = (GUI::GLabel*)m_pObjInnerFrm->Child(L"txtIncrease2");
   m_pObjIncrease5Txt   = (GUI::GLabel*)m_pObjInnerFrm->Child(L"txtIncrease5");
   m_pObjIncrease10Txt  = (GUI::GLabel*)m_pObjInnerFrm->Child(L"txtIncrease10");
   m_pObjIncrease25Txt  = (GUI::GLabel*)m_pObjInnerFrm->Child(L"txtIncrease25");

   m_pObjIncrease1Btn->ToggleState(false);
   m_pObjIncrease2Btn->ToggleState(false); 
   m_pObjIncrease5Btn->ToggleState(false);
   m_pObjIncrease10Btn->ToggleState(false);
   m_pObjIncrease25Btn->ToggleState(false);

   m_pObjLockTog->Hide();
   m_pObjApplyBtn->Left(m_pObjLockTog->Left());

   return true;
}

void GIncreaseProductionWindow::OnShow()
{
   __super::OnShow();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();

   m_pObjIncrease1Btn->ToggleState(false);
   m_pObjIncrease2Btn->ToggleState(false); 
   m_pObjIncrease5Btn->ToggleState(false);
   m_pObjIncrease10Btn->ToggleState(false);
   m_pObjIncrease25Btn->ToggleState(false);
}

void GIncreaseProductionWindow::Update(REAL64 in_fCurrentProduction)
{
   REAL64 l_fProduction = g_ClientDCL.IncreaseProductionApproxCost(0.01f, in_fCurrentProduction);
   m_pObjIncrease1Txt->Text(GString::FormatNumber(l_fProduction, L" ", L"", L"", L" $", 3, 0));

   l_fProduction = g_ClientDCL.IncreaseProductionApproxCost(0.02f, in_fCurrentProduction);
   m_pObjIncrease2Txt->Text(GString::FormatNumber(l_fProduction, L" ", L"", L"", L" $", 3, 0));

   l_fProduction = g_ClientDCL.IncreaseProductionApproxCost(0.05f, in_fCurrentProduction);
   m_pObjIncrease5Txt->Text(GString::FormatNumber(l_fProduction, L" ", L"", L"", L" $", 3, 0));

   l_fProduction = g_ClientDCL.IncreaseProductionApproxCost(0.10f, in_fCurrentProduction);
   m_pObjIncrease10Txt->Text(GString::FormatNumber(l_fProduction, L" ", L"", L"", L" $", 3, 0));

   l_fProduction = g_ClientDCL.IncreaseProductionApproxCost(0.25f, in_fCurrentProduction);
   m_pObjIncrease25Txt->Text(GString::FormatNumber(l_fProduction, L" ", L"", L"", L" $", 3, 0));
}