/**************************************************************
*
* sp2_objectives_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_objectives_window.h"
#include "../include/sp2_frame_scenario.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GObjectivesWindow::TypeName(L"Objectives_Window");

GUI::GBaseObject* GObjectivesWindow::New()
{
	return (GUI::GBaseObject*) new GObjectivesWindow();
}

GObjectivesWindow::GObjectivesWindow() : GBaseWindow()
{
}

GObjectivesWindow::~GObjectivesWindow()
{
}

bool GObjectivesWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm = (GUI::GFrame*)Child(L"frmInner");
   m_pScenarioFrm = (SP2::GFrameScenario*)m_pObjInnerFrm->Child(L"frmInner");

   m_pObjLockTog->Hide();
   m_pObjApplyBtn->Hide();

   m_pScenarioFrm->m_pObjScenarioCountry->Enabled(false);

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GObjectivesWindow::OnShow()
{
   __super::OnShow();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

GUI::EEventStatus::Enum GObjectivesWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {
            Hide();
            return GUI::EEventStatus::Handled;
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

void GObjectivesWindow::UpdateSelectedScenario()
{
   m_pScenarioFrm->UpdateSelectedScenario();
}

void GObjectivesWindow::ClearSelectedScenario()
{
   m_pScenarioFrm->Scenario(GSmartPtr<GGameScenario>(NULL) );
}
