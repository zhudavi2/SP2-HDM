/**************************************************************
*
* sp2_main_menu.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_save_confirmation_window.h"

const GString GSaveConfirmationWindow::TypeName(L"Save_Confirmation_Window");

GUI::GBaseObject* GSaveConfirmationWindow::New()
{
	return (GUI::GBaseObject*) new GSaveConfirmationWindow();
}

GSaveConfirmationWindow::GSaveConfirmationWindow() : GUI::GFrame()
{
   m_pCaller = NULL;
   m_pObjApplyBtn       = NULL;
   m_pObjDontApplyBtn   = NULL;
   m_pObjRememberTog    = NULL;
}

GSaveConfirmationWindow::~GSaveConfirmationWindow()
{
}

GUI::EEventStatus::Enum GSaveConfirmationWindow::OnMouseClick(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller)
{
	if(in_pCaller && m_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(m_pObjRememberTog->ToggleState())
         {
            g_ClientDAL.GenericDataHolder().m_bSaveConfirmationDoNotShow = true;
         }
         else
         {
            g_ClientDAL.GenericDataHolder().m_bSaveConfirmationDoNotShow = false;
         }

         if(in_pCaller == m_pObjApplyBtn)
         {
            if(g_ClientDAL.GenericDataHolder().m_bSaveConfirmationDoNotShow)
               g_ClientDAL.GenericDataHolder().m_bSaveConfirmationAlwaysApply = true;
            
            m_pCaller->Save();
            m_pCaller->Hide();
            Hide();
         }
         
         if(in_pCaller == m_pObjDontApplyBtn)
         {
            if(g_ClientDAL.GenericDataHolder().m_bSaveConfirmationDoNotShow)
               g_ClientDAL.GenericDataHolder().m_bSaveConfirmationAlwaysApply = false;

            m_pCaller->Hide();
            Hide();
         
         }
      }
   }

	return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GSaveConfirmationWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      GBaseObject* l_pClicked = g_Joshua.GUIManager()->MouseDownTarget();
      if(l_pClicked->Name() == L"Save_Confirmation_Window" || l_pClicked->Name() == L"lblText1" || 
         l_pClicked->Name() == L"lblText2" || l_pClicked->Name() == L"picExclamation" || l_pClicked->Name() == L"lblText3")
      {
         Drag();
      }
   }

   return EEventStatus::Handled;
}

void GSaveConfirmationWindow::OnDrag(const GEventData &, const GVector2D<INT32> & in_MouseOffset, const GVector2D<INT32> &)
{
   INT16 l_iLeftPos = (INT16)Left() + (INT16)in_MouseOffset.x;
   INT16 l_iTopPos  = (INT16)Top() + (INT16)in_MouseOffset.y;
   
   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   if(l_iLeftPos < 0)
      l_iLeftPos = 0;
   
   if(l_iTopPos < 0)
      l_iTopPos = 0;
   
   if(l_iLeftPos + Width() > mode.m_Resolution.x)
      l_iLeftPos = (INT16)mode.m_Resolution.x - (INT16)Width();

   if(l_iTopPos + Height() > mode.m_Resolution.y)
      l_iTopPos = (INT16)mode.m_Resolution.y - (INT16)Height();

   Position(l_iLeftPos, l_iTopPos);
}

bool GSaveConfirmationWindow::OnCreate()
{
	__super::OnCreate();
   BringToFront();

   m_pObjApplyBtn       = (GUI::GButton*)Child(L"btnApply");
   m_pObjDontApplyBtn   = (GUI::GButton*)Child(L"btnDont");
   m_pObjRememberTog    = (GUI::GToggle*)Child(L"togRemember");

	return true;
}

void GSaveConfirmationWindow::OnShow()
{
   __super::OnShow();
   BringToFront();   
}

void GSaveConfirmationWindow::Caller(SP2::GBaseWindow* in_pCaller)
{
   m_pCaller = in_pCaller;
}

void GSaveConfirmationWindow::OnHide()
{
   __super::OnHide();
   m_pCaller = NULL;
}