/**************************************************************
*
* sp2_state_machine.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_state_machine.h"

GStateMachine::GStateMachine()
{
   m_iCurState = c_iCurrentState;
}

void GStateMachine::AddState(GStateSPtr in_pState)
{
   gassert(in_pState.get(), "Invalid state added to state machine" );
   gassert(m_pStateObjs.count(in_pState->ID() ) == 0, "State already exists");
   m_pStateObjs[in_pState->ID() ] = in_pState;
}

UINT32 GStateMachine::CurrentState() const
{
   return m_iCurState;
}

GStateSPtr GStateMachine::State(UINT32 in_iID)
{
   if(m_pStateObjs.count(in_iID) )
   {
      return m_pStateObjs[in_iID];
   }

   return GStateSPtr(NULL);
}

void GStateMachine::SendEvent(UINT32 in_iEvent)
{
   m_EventMutex.Lock();
   m_vPendingEvents.push_back(in_iEvent);
   m_EventMutex.Unlock();

   g_Joshua.Log(GString(L"Received event : ") + GString(in_iEvent) );
}

void GStateMachine::Start(UINT32 in_iStartState)
{
   gassert(m_pStateObjs.count(in_iStartState) > 0, "Invalid start state");

   m_iCurState = in_iStartState;
   m_pStateObjs[in_iStartState]->OnEnter();
}

bool GStateMachine::IsStarted() const
{
   return m_iCurState != c_iCurrentState;
}

void GStateMachine::Iterate()
{
   if(IsStarted() )
   {
      gassert(m_pStateObjs.count(m_iCurState) > 0, "Invalid start state");
      GState* l_pState = m_pStateObjs[m_iCurState].get();

      // If there are events that were logged, 
      // send the top one to the current state
      m_EventMutex.Lock();
      if(!m_vPendingEvents.empty() )
      {
         UINT32 l_iEvent = m_vPendingEvents.front();
         m_vPendingEvents.pop_front();

         // Event could create a state change
         l_pState = ChangeState(l_pState->OnEvent(l_iEvent), l_pState);
      }
      m_EventMutex.Unlock();

      // Iterate the current state only if state machine was not ended
      if(l_pState)
      {
         // Iteration could create a state change
         ChangeState(l_pState->OnIterate(), l_pState);
      }
   }
}

SP2::GState* GStateMachine::ChangeState(UINT32 in_iNewState, SP2::GState* in_pCurState)
{
   // Nothing to do if state is current
   if(in_iNewState != c_iCurrentState)
   {
      // Tell current state to leave
      in_pCurState->OnLeave();

      // We could be asked to terminate the state machine
      if(in_iNewState == c_iTerminate)
      {
         in_pCurState = NULL;
         m_iCurState = c_iCurrentState;
      }
      else
      {
         // Enter new state
         gassert(m_pStateObjs.count(in_iNewState) > 0, "Invalid state ID to change to");
         m_iCurState = in_iNewState;
         in_pCurState = m_pStateObjs[in_iNewState];
         in_pCurState->OnEnter();

         g_Joshua.Log(GString(L"Changing to state : ") + GString(m_iCurState) );
      }
   }
   return in_pCurState;
}
