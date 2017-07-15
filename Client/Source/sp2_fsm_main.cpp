/**************************************************************
*
* sp2_fsm_main.cpp
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

#include "../include/sp2_fsm_main.h"

#include "../include/sp2_background_earth.h"
#include "../include/sp2_fsm_connecting.h"
#include "../include/sp2_fsm_disconnected.h"
#include "../include/sp2_fsm_playing.h"
#include "../include/sp2_fsm_movies.h"

#include "../include/sp2_generic_message_window.h"

FSM::GMain::GMain()
{
   AddState(GStateSPtr(new FSM::GInit) );
   AddState(GStateSPtr(new FSM::GMovies) );
   AddState(GStateSPtr(new FSM::GDisplayUnofficial) );
   AddState(GStateSPtr(new FSM::GLoadData) );
   AddState(GStateSPtr(new FSM::GDisconnected) );
   AddState(GStateSPtr(new FSM::GConnecting) );
   AddState(GStateSPtr(new FSM::GPlaying) );
   AddState(GStateSPtr(new FSM::GQuit) );
}

UINT32 FSM::GInit::OnEvent(UINT32 in_iEventID)
{
   // Quit if there is an initialization error
   if(in_iEventID == EEvents::InitError)
   {
      return EStates::Quit;
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GInit::OnEnter()
{
   // Initialize everybody
   g_ClientDCL.InitGame();
}

UINT32 FSM::GInit::OnIterate()
{
   return EStates::Movies;
}

UINT32 FSM::GDisplayUnofficial::OnEvent(UINT32 in_iEventID)
{
   // Quit if there is an initialization error
   if(in_iEventID == EEvents::InitError)
   {
      return EStates::Quit;
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GDisplayUnofficial::OnEnter()
{
   // Display a message box if mod is not official
   if(!g_Joshua.IsOfficial() )
   {
      g_ClientDDL.GenericMessageWindowSpawn();
      g_ClientDDL.GenericMessageWindow()->Show(g_ClientDAL.GetString(102309), NULL);
   }
}

UINT32 FSM::GDisplayUnofficial::OnIterate()
{
   if(!g_ClientDDL.GenericMessageWindow() )
   {
      return EStates::Disconnected;
   }
   else if(!g_ClientDDL.GenericMessageWindow()->Visible() )
   {
      return EStates::Disconnected;
   }

   return __super::OnIterate();
}

void FSM::GDisplayUnofficial::OnLeave()
{
}

UINT32 FSM::GLoadData::OnEvent(UINT32 in_iEventID)
{
   // Quit if there is an initialization error
   if(in_iEventID == EEvents::InitError)
   {
      return EStates::Quit;
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GLoadData::OnEnter()
{
   // Initialize everybody
   g_ClientDCL.LoadGameData();
}

UINT32 FSM::GLoadData::OnIterate()
{
   return EStates::DisplayUnofficial;
}

void FSM::GLoadData::OnLeave()
{
   if(g_ClientDDL.BackgroundEarth())
      g_ClientDDL.BackgroundEarth()->Show();
   g_ClientDDL.SplashScreenKill();
}

UINT32 FSM::GQuit::OnIterate()
{
   return GStateMachine::c_iTerminate;
}
