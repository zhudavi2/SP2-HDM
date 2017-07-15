/**************************************************************
*
* sp2_connecting_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_connecting_window.h"

// Constants
const GString GConnectingWindow::TypeName(L"Connecting_Window");

// Constructors and Destructors
GUI::GBaseObject* GConnectingWindow::New()
{
   return new GConnectingWindow();
}

bool GConnectingWindow::OnCreate()
{
   __super::OnCreate();

   m_pConnectLabel = (GUI::GLabel*) Child("txtMessage");
   m_pCancelBtn    = Child("btnCancel");
   m_eCancelEvent  = FSM::EEvents::NoEvent;

   return true;
}

void GConnectingWindow::OnShow()
{
   // Select appropriate connection message
   m_fShowTime = g_Joshua.Clock();

   m_pConnectLabel->Text(m_sInitialText);
}

GUI::EEventStatus::Enum GConnectingWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(in_pCaller == m_pCancelBtn)
      {
         g_SP2Client->m_pStateMachine->SendEvent(m_eCancelEvent);
         return GUI::EEventStatus::Handled;
      }
   }

   return __super::OnMouseClick(in_EventData, in_pCaller);
}

void GConnectingWindow::SetDisplayText(const GString& in_sText)
{
   m_sInitialText = in_sText;
}

void GConnectingWindow::SetCancelEvent(FSM::EEvents::Enum in_eEvent)
{
   m_eCancelEvent = in_eEvent;
   m_pCancelBtn->Enabled(m_eCancelEvent != FSM::EEvents::NoEvent);
}

void GConnectingWindow::Animate()
{
   // Add 1 dot per second, reset after 3 dots
   REAL64 l_fTime = g_Joshua.Clock();

   GString l_sText = m_sInitialText;

   while(l_fTime - m_fShowTime > 4)
   {
      m_fShowTime += 4;
   }

   UINT32 l_iNbDots = (UINT32) (l_fTime - m_fShowTime);

   for(UINT32 i = 0;i < l_iNbDots;i ++)
   {
      l_sText += ".";
   }

   m_pConnectLabel->Text(l_sText);
}
