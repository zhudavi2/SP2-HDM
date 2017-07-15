/**************************************************************
*
* sp2_action_list_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_action_list_window.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GActionListWindow::TypeName(L"Action_List");

GUI::GBaseObject* GActionListWindow::New()
{
	return (GUI::GBaseObject*) new GActionListWindow();
}


GActionListWindow::GActionListWindow() : GBaseWindow()
{
}

GActionListWindow::~GActionListWindow()
{
}

bool GActionListWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrame = (GUI::GFrame*)Child(L"frmInner");
   m_pObjTabActions = (GUI::GTabbed*)m_pObjInnerFrame->Child(L"tabActions");
   m_pObjLockTog->Hide();

   m_pObjTabActions->m_vTabPage[0]->Set_Background_Color(GColorRGBInt(0,0,0,255));
   m_pObjTabActions->m_vTabPage[0]->Set_Background_Picture(L"256",true);
   m_pObjTabActions->m_vTabPage[0]->Set_Border(1,1,0,1);
   m_pObjTabActions->m_vTabPage[0]->Height(m_pObjTabActions->m_vTabPage[0]->Height() + 1);

   m_pObjTabActions->m_vTabPage[1]->Set_Background_Color(GColorRGBInt(0,0,0,255));
   m_pObjTabActions->m_vTabPage[1]->Set_Background_Picture(L"256",true);
   m_pObjTabActions->m_vTabPage[1]->Set_Border(1,1,0,1);
   m_pObjTabActions->m_vTabPage[1]->Height(m_pObjTabActions->m_vTabPage[1]->Height() + 1);


   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   Position(((INT16)mode.m_Resolution.x / 2) - (Width() / 2),((INT16)mode.m_Resolution.y / 2) - (Height() / 2));

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GActionListWindow::OnShow()
{
   __super::OnShow();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

GUI::EEventStatus::Enum GActionListWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {
            g_ClientDDL.ActionListWindow()->Hide();
            return GUI::EEventStatus::Handled;
         }
      }
   }

   return GUI::EEventStatus::Handled;
}