/**************************************************************
*
* sp2_fsm_playing.cpp
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

#include "../include/sp2_fsm_playing.h"

#include "../include/sp2_background_earth.h"
#include "../include/sp2_connecting_window.h"
#include "../include/sp2_end_game_window.h"
#include "../include/sp2_main_menu.h"

FSM::GPlaying::GPlaying()
{
   m_iResetStateID = 0;

   // Create the sub state machine content
   AddState(GStateSPtr(new GInGame) );
   AddState(GStateSPtr(new GSavingGame) );
   AddState(GStateSPtr(new GCredits) );
}

UINT32 FSM::GPlaying::StartSubState() const
{
   return FSM::EPlayingStates::InGame;
}

UINT32 FSM::GPlaying::OnEvent(UINT32 in_iEventID)
{
   if(m_iResetStateID != 0)
   {
      return __super::OnEvent(in_iEventID);
   }

   if(!g_SP2Client->PlayCredits() )
   {
      if(in_iEventID == EEvents::StopGame)
      {
         if( (g_SP2Client->CurrentGameType() == EGameTypes::HostMultiPlayer) ||
             (g_SP2Client->CurrentGameType() == EGameTypes::JoinMultiPlayer) ||
             g_ClientDCL.ServerIsReseted() )
         {
            return EStates::Disconnected;
         }
         else
         {
            m_iResetStateID = EStates::Disconnected;
            g_ClientDCL.ResetServer();
         }
      }
      else if(in_iEventID == EEvents::Disconnection)
      {
         if(!g_ClientDDL.EndGameWindow() )
         {
            return EStates::Disconnected;
         }
         else if(!g_ClientDDL.EndGameWindow()->Visible() )
         {
            return EStates::Disconnected;
         }
      }
      else if(in_iEventID == EEvents::QuitBtn)
      {
         g_SP2Client->QuitGame();
         return EStates::Disconnected;
      }
      else if(in_iEventID == EEvents::RestartGame)
      {
         if( (g_SP2Client->CurrentGameType() == EGameTypes::HostMultiPlayer) ||
             (g_SP2Client->CurrentGameType() == EGameTypes::JoinMultiPlayer) ||
             g_ClientDCL.ServerIsReseted() )
         {
            return EStates::Disconnected;
         }
         else
         {
            m_iResetStateID = EStates::Connecting;
            g_ClientDCL.ResetServer();
         }
      }
      else if(in_iEventID == EEvents::LoadGame)
      {
         if( (g_SP2Client->CurrentGameType() == EGameTypes::HostMultiPlayer) ||
             (g_SP2Client->CurrentGameType() == EGameTypes::JoinMultiPlayer) ||
             g_ClientDCL.ServerIsReseted() )
         {
            return EStates::Disconnected;
         }
         else
         {
            m_iResetStateID = EStates::Connecting;
            g_ClientDCL.ResetServer();
         }
      }
   }
   else
   {
      g_SP2Client->ContinueCurrentGame(in_iEventID);
      SendEvent(FSM::EEvents::PlayCredits);
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GPlaying::OnEnter()
{
   __super::OnEnter();

   // Complete game initialization
   g_ClientDCL.BeginGame();
}

UINT32 FSM::GPlaying::OnIterate()
{
   if(m_iResetStateID != 0)
   {
      if(g_ClientDCL.ServerIsReseted() )
      {
         UINT32 l_iLastState = m_iResetStateID;
         m_iResetStateID = 0;
         return l_iLastState;
      }
   }

   // Iterate the game
   g_ClientDCL.IterateGame();

   return __super::OnIterate();
}

void FSM::GPlaying::OnLeave()
{
   // Make sure we terminate the connection with the server in multiplayer
   if( (g_SP2Client->CurrentGameType() == EGameTypes::HostMultiPlayer) ||
       (g_SP2Client->CurrentGameType() == EGameTypes::JoinMultiPlayer) )
   {
      g_Joshua.Disconnect();
      g_Joshua.KillServer();
   }
   else
   {
      g_ClientDCL.ResetServer();
   }

   // Make sure to clean any play data
   g_ClientDCL.CleanGame();
   g_ClientDDL.MainMenu()->Hide();   

   __super::OnLeave();
}

UINT32 FSM::GInGame::OnEvent(UINT32 in_iEventID)
{
   if(in_iEventID == FSM::EEvents::SaveGame)
   {
      return FSM::EPlayingStates::SavingGame;
   }
   else if(in_iEventID == FSM::EEvents::PlayCredits)
   {
      return FSM::EPlayingStates::Credits;
   }

   return __super::OnEvent(in_iEventID);
}

UINT32 FSM::GSavingGame::OnEvent(UINT32 in_iEventID)
{
   if(in_iEventID == FSM::EEvents::EndSaveGame)
   {
      return FSM::EPlayingStates::InGame;
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GSavingGame::OnEnter()
{
   g_ClientDDL.ConnectingWindowSpawn();
   g_ClientDDL.ConnectingWindow()->SetDisplayText(g_ClientDAL.GetString(EStrId::SavingGame) );
   g_ClientDDL.ConnectingWindow()->SetCancelEvent(FSM::EEvents::NoEvent);
}

UINT32 FSM::GSavingGame::OnIterate()
{
   g_ClientDDL.ConnectingWindow()->Animate();

   return __super::OnIterate();
}

void FSM::GSavingGame::OnLeave()
{
   g_ClientDDL.ConnectingWindowKill();
   g_ClientDCL.DisplaySaveResult();
}

UINT32 FSM::GCredits::OnEvent(UINT32 in_iEventID)
{
   if(in_iEventID == FSM::EEvents::CancelMovie)
   {
      g_SP2Client->ContinueCurrentGame();
      return FSM::EPlayingStates::InGame;
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GCredits::OnEnter()
{
}

UINT32 FSM::GCredits::OnIterate()
{
   return __super::OnIterate();
}

void FSM::GCredits::OnLeave()
{
}
