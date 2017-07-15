/**************************************************************
*
* sp2_fsm_disconnected.cpp
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

#include "../include/sp2_fsm_disconnected.h"

#include "../include/sp2_main_menu.h"
#include "../include/sp2_submenu_single.h"
#include "../include/sp2_submenu_multi.h"
#include "../include/sp2_connecting_window.h"

FSM::GDisconnected::GDisconnected()
{
   // Create the sub state machine content
   AddState(GStateSPtr(new GInMainMenu) );
   AddState(GStateSPtr(new GInSubMenuSP) );
   AddState(GStateSPtr(new GInSubMenuMP) );
   AddState(GStateSPtr(new GInHostWindow) );
   AddState(GStateSPtr(new GInJoinWindow) );
}

UINT32 FSM::GDisconnected::StartSubState() const
{
   return EDisconnectedStates::InMainMenu;
}

UINT32 FSM::GDisconnected::OnEvent(UINT32 in_iEventID)
{
   switch(in_iEventID)
   {
   case EEvents::QuitBtn:
      return EStates::Quit;

   case EEvents::TutorialBtn:
      g_ClientDCL.ConnectToGame(EGameTypes::Tutorial);
      return EStates::Connecting;

   case EEvents::StartSinglePlayerBtn:
      g_ClientDCL.ConnectToGame(EGameTypes::SinglePlayer);
      return EStates::Connecting;

   case EEvents::ScenarioBtn:
      g_ClientDCL.ConnectToGame(EGameTypes::Scenario);
      return EStates::Connecting;

   case EEvents::HostStartBtn:
      g_ClientDCL.ConnectToGame(EGameTypes::HostMultiPlayer);
      return EStates::Connecting;

   case EEvents::JoinConnectBtn:
      g_ClientDCL.ConnectToGame(EGameTypes::JoinMultiPlayer);
      return EStates::Connecting;

   case EEvents::LoadGame:
      g_ClientDCL.ConnectToGame(g_SP2Client->CurrentGameType() );
      return EStates::Connecting;

   case EEvents::ConnectAtStartup:
      return EStates::Connecting;
   }

   return __super::OnEvent(in_iEventID);
}

UINT32 FSM::GInMainMenu::OnEvent(UINT32 in_iEventID)
{
   if(in_iEventID == EEvents::SPMenuBtn)
   {
      return EDisconnectedStates::InSubMenuSP;
   }
   else if(in_iEventID == EEvents::MPMenuBtn)
   {
      return EDisconnectedStates::InSubMenuMP;
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GInMainMenu::OnEnter()
{
   g_ClientDDL.MainMenu()->Show();
}

void FSM::GInMainMenu::OnLeave()
{
   g_ClientDDL.MainMenu()->Hide();
}

UINT32 FSM::GInSubMenuSP::OnEvent(UINT32 in_iEventID)
{
   if(in_iEventID == EEvents::SubMenuBackBtn)
   {
      return EDisconnectedStates::InMainMenu;
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GInSubMenuSP::OnEnter()
{
   if(!g_ClientDDL.SubmenuSingle())
      g_ClientDDL.SubmenuSingleSpawn();

   g_ClientDDL.SubmenuSingle()->Show();
}

void FSM::GInSubMenuSP::OnLeave()
{
   g_ClientDDL.SubmenuSingle()->Hide();
}

UINT32 FSM::GInSubMenuMP::OnEvent(UINT32 in_iEventID)
{
   if(in_iEventID == EEvents::SubMenuBackBtn)
   {
      return EDisconnectedStates::InMainMenu;
   }
   else if(in_iEventID == EEvents::StartMultiplayerBtn)
   {
      return EDisconnectedStates::InHostWindow;
   }
   else if(in_iEventID == EEvents::JoinMultiplayerBtn)
   {
      return EDisconnectedStates::InJoinWindow;
   }
   else if (in_iEventID == EEvents::CancelConnect) // cancel connection to RGAPI
   {
      g_Joshua.Client()->RGAPIDisconnect();
      g_ClientDDL.ConnectingWindowKill();
      //return EDisconnectedStates::InSubMenuMP;
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GInSubMenuMP::OnEnter()
{
   if(!g_ClientDDL.SubmenuMulti())
      g_ClientDDL.SubmenuMultiSpawn();

   g_ClientDDL.SubmenuMulti()->Show();
}

void FSM::GInSubMenuMP::OnLeave()
{
   g_ClientDDL.SubmenuMulti()->Hide();
}

UINT32 FSM::GInSubMenuMP::OnIterate()
{
   if(g_ClientDDL.ConnectingWindow() && g_ClientDDL.ConnectingWindow()->Visible())
      g_ClientDDL.ConnectingWindow()->Animate();

   return __super::OnIterate();
}

UINT32 FSM::GInHostWindow::OnEvent(UINT32 in_iEventID)
{
   if(in_iEventID == EEvents::HostMPCancel)
   {
      return EDisconnectedStates::InJoinWindow;
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GInHostWindow::OnEnter()
{
   g_ClientDDL.HostMPGameWindowSpawn();
}

void FSM::GInHostWindow::OnLeave()
{
   g_ClientDDL.HostMPGameWindowKill();
}

UINT32 FSM::GInJoinWindow::OnEvent(UINT32 in_iEventID)
{
   if(in_iEventID == EEvents::JoinMPCancel)
   {
      return EDisconnectedStates::InSubMenuMP;
   }
   else if(in_iEventID == EEvents::StartMultiplayerBtn)
   {
      return EDisconnectedStates::InHostWindow;
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GInJoinWindow::OnEnter()
{
   g_ClientDDL.JoinMPGameWindowSpawn();
}

void FSM::GInJoinWindow::OnLeave()
{
   g_ClientDDL.JoinMPGameWindowKill();
}