/**************************************************************
*
* sp2_generic_input_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_generic_input_window.h"

const GString GGenericInputWindow::TypeName(L"Generic_Input_Window");

GUI::GBaseObject* GGenericInputWindow::New()
{
	return (GUI::GBaseObject*) new GGenericInputWindow();
}


GGenericInputWindow::GGenericInputWindow() : GFrame()
{
}

GGenericInputWindow::~GGenericInputWindow()
{
}

bool GGenericInputWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjOkBtn = (GUI::GButton*)Child(L"btnOk");
   m_pObjCancelBtn = (GUI::GButton*)Child(L"btnCancel");
   m_pObjInput = (GUI::GEditBox*)Child(L"edtAnswer");
   m_pObjTextLbl = (GUI::GLabel*)Child(L"lblMessage");

	return true;
}

GUI::EEventStatus::Enum GGenericInputWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_pCaller == m_pObjOkBtn)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         Hide();
         GUI::GEventData l_Data;

         GString l_sText = m_pObjInput->Text();
         l_Data.Data = (void*)&l_sText;
    
         m_pCaller->FireCustomEvent(c_OnGenericInputWindowOk, l_Data, true, this);

         return EEventStatus::Handled;
      }
   }
   else if(in_pCaller == m_pObjCancelBtn)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         Hide();
         GUI::GEventData l_Data;

         FireCustomEvent(c_OnGenericInputWindowCancel, l_Data, true, m_pCaller);

         return EEventStatus::Handled;
      }
   }
   
   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GGenericInputWindow::OnKeyUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   switch(in_EventData.Key())
   {
      case VK_ESCAPE:
         break;
      case VK_RETURN:
         break;
   }

   return GUI::EEventStatus::Handled;
}

void GGenericInputWindow::Show(GString in_sMessage, GUI::GBaseObject* in_pCaller)
{
   m_pCaller = in_pCaller;
   m_pObjTextLbl->Text(in_sMessage);

   __super::Show();

   m_pObjInput->Focus();
}

bool GGenericInputWindow::OnLoseFocus(const GUI::GBaseObject *in_pFocusTo, GUI::GBaseObject* in_pCaller)
{
   if((in_pFocusTo != this && in_pFocusTo != m_pObjOkBtn ) && Visible())
   {
      g_Joshua.GUIManager()->Focus(this);
      BringToFront();
   }

   return true;
}

bool GGenericInputWindow::OnGotFocus(const GBaseObject* in_pFocusFrom, GBaseObject* in_pCaller)
{
   BringToFront();
   return true;
}
