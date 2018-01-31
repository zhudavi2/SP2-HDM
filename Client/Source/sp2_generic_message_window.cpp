/**************************************************************
*
* sp2_generic_message_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_generic_message_window.h"

const GString GGenericMessageWindow::TypeName(L"Generic_Message_Window");

GUI::GBaseObject* GGenericMessageWindow::New()
{
	return (GUI::GBaseObject*) new GGenericMessageWindow();
}


GGenericMessageWindow::GGenericMessageWindow() : GBaseWindow()
{
}

GGenericMessageWindow::~GGenericMessageWindow()
{
}

bool GGenericMessageWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjOkBtn = (GUI::GButton*)Child(L"btnOk");
   m_pObjTextLbl = (GUI::GLabel*)Child(L"lblMessage");

	return true;
}

GUI::EEventStatus::Enum GGenericMessageWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_pCaller == m_pObjOkBtn)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         Hide();
         GUI::GEventData l_Data;
         if(m_pCaller)
            m_pCaller->OnCustomEvent(c_OnGenericMessageWindowAnswerEvent, l_Data);
         //Notify any potential observer
         NotifyObserver(c_OnGenericMessageWindowOk,this);
         return EEventStatus::Handled;
      }
   }
   
   return GUI::EEventStatus::Handled;
}

void GGenericMessageWindow::Show(GString in_sMessage, GUI::GBaseObject* in_pCaller)
{
   m_pCaller = in_pCaller;
   m_pObjTextLbl->Text(Hector::Util::ReplaceEndLinesByRealEndLines(in_sMessage));

   if(m_pObjTextLbl->Height() != (INT32)m_pObjTextLbl->Text2D()->Height())
   {
      INT32 l_iDiff = m_pObjTextLbl->Text2D()->Height() - m_pObjTextLbl->Height();
      m_pObjTextLbl->Height(m_pObjTextLbl->Height() + l_iDiff + 4);
      m_pObjOkBtn->Top(m_pObjTextLbl->Height() + m_pObjTextLbl->Top() + 12);
      Height(m_pObjOkBtn->Top() + m_pObjOkBtn->Height() + 10);
   }

   __super::Show();
}

void GGenericMessageWindow::Hide()
{
   GDZLOG(EDZLogLevel::Entry, L"");
   
   __super::Hide();

   GDZLOG(EDZLogLevel::Exit, L"");
}


bool GGenericMessageWindow::OnLoseFocus(const GUI::GBaseObject *in_pFocusTo, GUI::GBaseObject* in_pCaller)
{
   if((in_pFocusTo != this && in_pFocusTo != m_pObjOkBtn ) && Visible())
   {
      g_Joshua.GUIManager()->Focus(this);
      BringToFront();
   }

   return true;
}

bool GGenericMessageWindow::OnGotFocus(const GBaseObject* in_pFocusFrom, GBaseObject* in_pCaller)
{
   BringToFront();
   return true;
}
