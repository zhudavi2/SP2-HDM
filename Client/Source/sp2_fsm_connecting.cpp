/**************************************************************
*
* sp2_fsm_connecting.cpp
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

#include "../include/sp2_fsm_connecting.h"

#include "../include/sp2_connecting_window.h"
#include "../include/sp2_game_lobby_window.h"
#include "../include/sp2_game_scenario_window.h"
#include "../include/sp2_start_single_player_window.h"

FSM::GConnecting::GConnecting()
{
   // Create the sub state machine content
   AddState(GStateSPtr(new GWaitingConnect) );
   AddState(GStateSPtr(new GChoosingOptions) );
   AddState(GStateSPtr(new GSetupTutorial) );
   AddState(GStateSPtr(new GReceivingData) );
   AddState(GStateSPtr(new GWaitingGame) );
   AddState(GStateSPtr(new GInLobby) );
   AddState(GStateSPtr(new GCreatingGame) );
   AddState(GStateSPtr(new GLoadingGame) );
}

UINT32 FSM::GConnecting::StartSubState() const
{
   // When connecting, by default we want to connect to a server
   // except when re-starting a single player or loading a game,
   // where the client is already connected
   UINT32 l_iStartState = EConnectingStates::WaitingConnect;
   if(g_Joshua.Connected() )
   {
      if(g_SP2Client->LoadingGame() )
      {
         l_iStartState = EConnectingStates::LoadingGame;
      }
      else if(g_SP2Client->CurrentGameType() != EGameTypes::JoinMultiPlayer)
      {
         l_iStartState = EConnectingStates::CreatingGame;
      }
      else
      {
         l_iStartState = EConnectingStates::ReceivingData;
      }
   }
   return l_iStartState;
}

UINT32 FSM::GConnecting::OnEvent(UINT32 in_iEventID)
{
   if( (in_iEventID == EEvents::Disconnection) || 
       (in_iEventID == EEvents::CancelConnect) ||
       (in_iEventID == EEvents::CountryDataFailed) ||
       (in_iEventID == EEvents::QuitBtn) ||
       (in_iEventID == EEvents::CancelNewOrLoad) || 
       (in_iEventID == EEvents::ErrorNewOrLoadGame) )
   {
      g_SP2Client->LoadGame(false);
      if(g_SP2Client->CurrentGameType() == EGameTypes::Tutorial)
      {
         GTutorials::Instance().StopTutorial();
      }

      // Make sure we terminate the connection with the server on multiplayer games
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

      if(in_iEventID == EEvents::QuitBtn)
      {
         return EStates::Quit;
      }

      return EStates::Disconnected;
   }
   else if(in_iEventID == EEvents::StartGame)
   {
      g_SP2Client->LoadGame(false);
      return EStates::Playing;
   }

   // Make sure to forward events to the sub-state machine
   return __super::OnEvent(in_iEventID);
}

UINT32 FSM::GWaitingConnect::OnEvent(UINT32 in_iEventID)
{
   if(in_iEventID == EEvents::Connected)
   {
		g_Joshua.Log(L"BOOYAAA: " + GString(g_Joshua.Client()->Id()) + L"  --   " + GString(g_ClientDAL.m_DataOptionsWindow.m_fAIAggressiveness));
		//Send Game Event
		if(g_Joshua.Client()->Id() > 0)
		{
			SDK::GGameEventSPtr l_Evt = CREATE_GAME_EVENT(SP2::Event::GChangeAIAggressiveness);
			l_Evt->m_iSource = g_Joshua.Client()->Id();
			l_Evt->m_iTarget = SDK::Event::ESpecialTargets::Server;
			SP2::Event::GChangeAIAggressiveness* l_pEvt = (SP2::Event::GChangeAIAggressiveness*)l_Evt.get();
			l_pEvt->AIAggressiveness(g_ClientDAL.m_DataOptionsWindow.m_fAIAggressiveness);
			g_Joshua.RaiseEvent(l_Evt);
		}

      if(g_SP2Client->CurrentGameType() == EGameTypes::JoinMultiPlayer)
      {
         return EConnectingStates::ReceivingData;
      }
      else
      {
         if(g_SP2Client->LoadingGame() )
         {
            return EConnectingStates::LoadingGame;
         }
         else
         {
            return EConnectingStates::CreatingGame;
         }
      }
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GWaitingConnect::OnEnter()
{
   g_SP2Client->LimitFPS(true);
   g_ClientDDL.ConnectingWindowSpawn();
   if(g_SP2Client->CurrentGameType() == EGameTypes::JoinMultiPlayer)
   {
      g_ClientDDL.ConnectingWindow()->SetDisplayText(g_ClientDAL.GetString(EStrId::ConnectingGame) );
   }
   else
   {
      g_ClientDDL.ConnectingWindow()->SetDisplayText(g_ClientDAL.GetString(EStrId::StartingGame) );
   }
   g_ClientDDL.ConnectingWindow()->SetCancelEvent(FSM::EEvents::CancelConnect);
}

UINT32 FSM::GWaitingConnect::OnIterate()
{
   g_ClientDDL.ConnectingWindow()->Animate();

   return __super::OnIterate();
}

void FSM::GWaitingConnect::OnLeave()
{
   g_SP2Client->LimitFPS(false);
   g_ClientDDL.ConnectingWindowKill();
}

UINT32 FSM::GChoosingOptions::OnEvent(UINT32 in_iEventID)
{
   if(in_iEventID == EEvents::OptionSelected)
   {
      return EConnectingStates::WaitingGame;
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GChoosingOptions::OnEnter()
{
   switch(g_SP2Client->CurrentGameType() )
   {
   case EGameTypes::SinglePlayer:
      //Create single player window
      g_ClientDDL.StartSinglePlayerWindowSpawn();
      g_ClientDDL.StartSinglePlayerWindow()->Show();
      g_ClientDCL.RequestCountriesRanks();

      break;
   case EGameTypes::Scenario:
      //Create scenario window and show it...
      g_ClientDDL.GameScenarioWindowSpawn();
      g_ClientDDL.GameScenarioWindow()->Show();

      break;
   default:
      gassert(false, "Invalid game type for choosing options state");
   }
}

void FSM::GChoosingOptions::OnLeave()
{
   g_ClientDDL.StartSinglePlayerWindowKill();
   g_ClientDDL.GameScenarioWindowKill();
}

UINT32 FSM::GReceivingData::OnEvent(UINT32 in_iEventID)
{
   if(in_iEventID == EEvents::DataReceived)
   {
      m_iStartFlag |= 1;
   }
   else if(in_iEventID == EEvents::CountryDataReceived)
   {
      m_iStartFlag |= 2;
   }

   if(m_iStartFlag == 3)
   {
      if(g_SP2Client->LoadingGame() )
      {
         gassert( (g_SP2Client->CurrentGameType() == EGameTypes::SinglePlayer) ||
                  (g_SP2Client->CurrentGameType() == EGameTypes::HostMultiPlayer) ||
                  (g_SP2Client->CurrentGameType() == EGameTypes::Scenario),
                  "Loading a game in an invalid game mode...");

         if(g_SP2Client->CurrentGameType() != EGameTypes::HostMultiPlayer)
         {
            return EConnectingStates::WaitingGame;
         }
         else
         {
            return EConnectingStates::InLobby;
         }
      }
      else
      {
         // Different path is taken depending on current game type
         switch(g_SP2Client->CurrentGameType() )
         {
         case EGameTypes::SinglePlayer:
         case EGameTypes::Scenario:
            return EConnectingStates::ChoosingOptions;

         case EGameTypes::HostMultiPlayer:
         case EGameTypes::JoinMultiPlayer:
            return EConnectingStates::InLobby;

         case EGameTypes::Tutorial:
            return EConnectingStates::SetupTutorial;

         default:
            gassert(false, "Invalid game type for wating connection state");
         }
      }
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GReceivingData::OnEnter()
{
   g_SP2Client->LimitFPS(true);
   m_iStartFlag = 0;
   g_ClientDCL.ReceiveData();

   g_ClientDDL.GameLobby()->ShowWaitLabel();
}

void FSM::GReceivingData::OnLeave()
{
   g_SP2Client->LimitFPS(false);
   // Will be used by subsequent windows to display the available countries
   g_ClientDCL.RequestCountryListToServer();

   // The "wait" label will be hidden when the data will be received
}

UINT32 FSM::GSetupTutorial::OnIterate()
{
   g_ClientDCL.SetupTutorial();

   return EConnectingStates::WaitingGame;
}

void FSM::GWaitingGame::OnEnter()
{
   g_ClientDDL.GameLobby()->ShowWaitLabel();
}

void FSM::GWaitingGame::OnLeave()
{
   g_ClientDDL.GameLobby()->HideWaitLabel();
}

void FSM::GInLobby::OnEnter()
{
   g_ClientDDL.GameLobby()->Show(EGameLobbyType::MultiPlayer);

   // The following line must ALWAYS be done before :
   //     g_ClientDCL.SystemRequestCountriesToServer();
   // (which is done automaticly when server is connected)

   //Send the request to receive the available countries
   g_ClientDCL.SystemRequestAvailablesCountriesToServer();
   g_ClientDCL.RequestCountriesRanks();
}

void FSM::GInLobby::OnLeave()
{
   g_ClientDDL.GameLobby()->Hide();
}

UINT32 FSM::GCreatingGame::OnEvent(UINT32 in_iEventID)
{
   if(in_iEventID == FSM::EEvents::EndNewGame)
   {
      return EConnectingStates::ReceivingData;
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GCreatingGame::OnEnter()
{
   g_SP2Client->LimitFPS(true);
   g_ClientDCL.CreateNewGame();

   g_ClientDDL.ConnectingWindowSpawn();
   g_ClientDDL.ConnectingWindow()->SetDisplayText(g_ClientDAL.GetString(EStrId::StartingGame) );
   g_ClientDDL.ConnectingWindow()->SetCancelEvent(FSM::EEvents::CancelNewOrLoad);
}

UINT32 FSM::GCreatingGame::OnIterate()
{
   g_ClientDDL.ConnectingWindow()->Animate();

   return __super::OnIterate();
}

void FSM::GCreatingGame::OnLeave()
{
   g_SP2Client->LimitFPS(false);
   g_ClientDDL.ConnectingWindowKill();
   g_ClientDCL.DisplayNewResult();
}

UINT32 FSM::GLoadingGame::OnEvent(UINT32 in_iEventID)
{
   if(in_iEventID == FSM::EEvents::EndLoadGame)
   {
      return EConnectingStates::ReceivingData;
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GLoadingGame::OnEnter()
{
   g_SP2Client->LimitFPS(true);
   g_ClientDCL.LoadSavedGameOnServer();

   g_ClientDDL.ConnectingWindowSpawn();
   g_ClientDDL.ConnectingWindow()->SetDisplayText(g_ClientDAL.GetString(EStrId::LoadingGame) );
   g_ClientDDL.ConnectingWindow()->SetCancelEvent(FSM::EEvents::CancelNewOrLoad);
}

UINT32 FSM::GLoadingGame::OnIterate()
{
   g_ClientDDL.ConnectingWindow()->Animate();

   return __super::OnIterate();
}

void FSM::GLoadingGame::OnLeave()
{
   g_SP2Client->LimitFPS(false);
   g_ClientDDL.ConnectingWindowKill();
   g_ClientDCL.DisplayLoadResult();
}
