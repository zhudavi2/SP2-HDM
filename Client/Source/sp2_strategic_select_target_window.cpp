/**************************************************************
*
* sp2_strategic_select_target_window.cpp
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

#include "../include/sp2_strategic_select_target_window.h"

#include "../include/sp2_background_earth.h"

const GString GStrategicSelectTargetWindow::TypeName(L"Strategic_Select_Target_Window");

GUI::GBaseObject* SP2::GStrategicSelectTargetWindow::New()
{
	return (GUI::GBaseObject*) new GStrategicSelectTargetWindow();
}


GStrategicSelectTargetWindow::GStrategicSelectTargetWindow() : GFrame()
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Created : Strategic Select Target Window");
}

GStrategicSelectTargetWindow::~GStrategicSelectTargetWindow()
{
   g_ClientDDL.m_pLogger->Log(Hector::MSGTYPE_MSG,L"Destroyed : Strategic Select Target Window");
}

bool GStrategicSelectTargetWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjCancelBtn = (GUI::GButton*)Child(L"frmMessage")->Child(L"btnCancel");
   Position(20,60);

	return true;
}

bool GStrategicSelectTargetWindow::OnDestroy()
{
   __super::OnDestroy();

   return true;
}

GUI::EEventStatus::Enum GStrategicSelectTargetWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjCancelBtn)
         {
            g_ClientDDL.StrategicSelectTargetWindow()->Hide();
            g_ClientDDL.BackgroundEarth()->m_ReturnClickedPosition = NULL;
            if(g_ClientDDL.BackgroundEarth()->m_pReturnWindow)
            {
               g_ClientDDL.BackgroundEarth()->m_pReturnWindow->Show();
               g_ClientDDL.BackgroundEarth()->m_pReturnWindow = NULL;
            }
            return GUI::EEventStatus::Handled;
         }
      }
   }

   return GUI::EEventStatus::Handled;
}
