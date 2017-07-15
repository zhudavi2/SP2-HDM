/**************************************************************
*
* sp2_system_bar_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_system_bar_window.h"

const GString GSystemBarWindow::TypeName(L"System_Bar_Window");

GUI::GBaseObject* GSystemBarWindow::New()
{
	return (GUI::GBaseObject*) new GSystemBarWindow();
}


GSystemBarWindow::GSystemBarWindow() : GFrame()
{
   m_bTooltipShown = false;
}

GSystemBarWindow::~GSystemBarWindow()
{
}

bool GSystemBarWindow::OnCreate()
{
	__super::OnCreate();

   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);
   Position(mode.m_Resolution.x - Width(), 0);

   m_pSlidingTrack = g_ClientDDL.AnimationCreateSliding(0.5f, 0.26f, mode.m_Resolution.x - Width(), mode.m_Resolution.x - 36, Model()->Get_Transform(), false);

   m_pObjF1Btn  = (GUI::GButton*)Child(L"btnF1");
   m_pObjF2Btn  = (GUI::GButton*)Child(L"btnF2");
   m_pObjF3Btn  = (GUI::GButton*)Child(L"btnF3");
   m_pObjF4Btn  = (GUI::GButton*)Child(L"btnF4");
   m_pObjF5Btn  = (GUI::GButton*)Child(L"btnF5");
   m_pObjF10Btn = (GUI::GButton*)Child(L"btnF10");

   m_pObjF1Lbl  = (GUI::GLabel*)Child(L"txtF1");
   m_pObjF2Lbl  = (GUI::GLabel*)Child(L"txtF2");
   m_pObjF3Lbl  = (GUI::GLabel*)Child(L"txtF3");
   m_pObjF4Lbl  = (GUI::GLabel*)Child(L"txtF4");
   m_pObjF5Lbl  = (GUI::GLabel*)Child(L"txtF5");
   m_pObjF10Lbl = (GUI::GLabel*)Child(L"txtF10");

   m_pObjMainTog = (GUI::GToggle*)Child(L"togMain");

   m_pObjMainTog->ToggleState(true);

	return true;
}

void GSystemBarWindow::OnShow()
{
   m_pObjF1Btn->Enabled(true); 
   m_pObjF2Btn->Enabled(true); 
   m_pObjF3Btn->Enabled(true); 
   m_pObjF4Btn->Enabled(true); 
   m_pObjF5Btn->Enabled(true); 
   m_pObjF10Btn->Enabled(true);

   switch(g_SP2Client->CurrentGameType())
   {
   case EGameTypes::HostMultiPlayer:
      m_pObjF4Btn->Enabled(false); 
      break;
   case EGameTypes::JoinMultiPlayer:
      m_pObjF3Btn->Enabled(false); 
      m_pObjF4Btn->Enabled(false); 
      break;
   case EGameTypes::Scenario:
   case EGameTypes::SinglePlayer:
      m_pObjF2Btn->Enabled(false);
      break;
   case EGameTypes::Tutorial:
      m_pObjF1Btn->Enabled(false); 
      m_pObjF2Btn->Enabled(false); 
      m_pObjF3Btn->Enabled(false); 
      m_pObjF4Btn->Enabled(false); 
      m_pObjF5Btn->Enabled(false); 
      m_pObjF10Btn->Enabled(false);
      break;
   }
   
   __super::OnShow();
}

bool GSystemBarWindow::OnDestroy()
{
   SAFE_RELEASE(m_pSlidingTrack);

   return __super::OnDestroy();
}

GUI::EEventStatus::Enum GSystemBarWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if( in_pCaller )
   {
      if( in_pCaller == m_pObjMainTog )
      {
         if( !m_pObjMainTog->ToggleState() )
            g_ClientDDL.AnimationPlaySliding(GFX::PLAYBACK_FORWARD, m_pSlidingTrack);
         else
            g_ClientDDL.AnimationPlaySliding(GFX::PLAYBACK_REVERSE, m_pSlidingTrack);
      }
      else if(in_pCaller == m_pObjF1Btn)
      {
         GEventData l_Data;
         l_Data.Keyboard.Actor = VK_F1;
         g_SP2Client->OnGlobalHotkey(l_Data);
         g_Joshua.GUIManager()->TooltipText( m_pObjF1Btn->TooltipText() );
         g_Joshua.GUIManager()->TooltipHide();
         g_Joshua.GUIManager()->TooltipShow();
         m_bTooltipShown = true;
      }
      else if(in_pCaller == m_pObjF2Btn)
      {
         GEventData l_Data;
         l_Data.Keyboard.Actor = VK_F2;
         g_SP2Client->OnGlobalHotkey(l_Data);
      }
      else if(in_pCaller == m_pObjF3Btn)
      {
         GEventData l_Data;
         l_Data.Keyboard.Actor = VK_F3;
         g_SP2Client->OnGlobalHotkey(l_Data);
      }
      else if(in_pCaller == m_pObjF4Btn)
      {
         GEventData l_Data;
         l_Data.Keyboard.Actor = VK_F4;
         g_SP2Client->OnGlobalHotkey(l_Data);
      }
      else if(in_pCaller == m_pObjF5Btn)
      {
         GEventData l_Data;
         l_Data.Keyboard.Actor = VK_F5;
         g_SP2Client->OnGlobalHotkey(l_Data);
      }
      else if(in_pCaller == m_pObjF10Btn)
      {
         GEventData l_Data;
         l_Data.Keyboard.Actor = VK_F10;
         g_SP2Client->OnGlobalHotkey(l_Data);
      }
   }

   return __super::OnMouseClick(in_EventData, in_pCaller);
}

EEventStatus::Enum GSystemBarWindow::OnMouseMove(const GEventData & in_EventData, GBaseObject* in_pCaller)
{
   if(m_bTooltipShown)
   {
      g_ClientDDL.TooltipWindow()->Hide();
      m_bTooltipShown = false;
   }

   return __super::OnMouseMove(in_EventData, in_pCaller);
}