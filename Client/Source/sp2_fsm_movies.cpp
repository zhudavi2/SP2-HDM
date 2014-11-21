/**************************************************************
*
* sp2_fsm_movies.cpp
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

#include "../include/sp2_fsm_movies.h"

FSM::GMovies::GMovies()
{
   // Create the sub state machine content
   AddState(GStateSPtr(new GLogoDC) );
   AddState(GStateSPtr(new GLogoGolem) );
   AddState(GStateSPtr(new GIntro) );
}

UINT32 FSM::GMovies::StartSubState() const
{
   return FSM::EMoviesState::LogoDC;
}

UINT32 FSM::GMovies::OnIterate()
{
   if(!IsStarted() )
   {
      return FSM::EStates::LoadData;
   }

   return __super::OnIterate();
}

UINT32 FSM::GLogoDC::OnEvent(UINT32 in_iEventID)
{
//#ifdef GOLEM_DEBUG
   if(in_iEventID == EEvents::CancelMovie)
   {
      return FSM::EMoviesState::LogoGolem;
   }
//#endif

   return __super::OnEvent(in_iEventID);
}

void FSM::GLogoDC::OnEnter()
{
}

UINT32 FSM::GLogoDC::OnIterate()
{
   return __super::OnIterate();
}

void FSM::GLogoDC::OnLeave()
{
}

UINT32 FSM::GLogoGolem::OnEvent(UINT32 in_iEventID)
{
//#ifdef GOLEM_DEBUG
   if(in_iEventID == EEvents::CancelMovie)
   {
      return FSM::EMoviesState::Intro;
   }
//#endif

   return __super::OnEvent(in_iEventID);
}

void FSM::GLogoGolem::OnEnter()
{
}

UINT32 FSM::GLogoGolem::OnIterate()
{
   return __super::OnIterate();
}

void FSM::GLogoGolem::OnLeave()
{
}

UINT32 FSM::GIntro::OnEvent(UINT32 in_iEventID)
{
   if(in_iEventID == EEvents::CancelMovie)
   {
      return GStateMachine::c_iTerminate;
   }

   return __super::OnEvent(in_iEventID);
}

void FSM::GIntro::OnEnter()
{
}

UINT32 FSM::GIntro::OnIterate()
{
   return __super::OnIterate();
}

void FSM::GIntro::OnLeave()
{
}

