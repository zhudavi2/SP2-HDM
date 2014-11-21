/**************************************************************
*
* sp2_fsm_connecting.h
*
* Description
* ===========
*  Description of the Connecting game state, which itself 
*  contains a sub state machine.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_FSM_CONNECTING_H_
#define _SP2_FSM_CONNECTING_H_

#include "sp2_fsm_main.h"

namespace SP2
{
   namespace FSM
   {
      // States specific to the connecting state sub FSM
      namespace EConnectingStates
      {
         enum Enum
         {
            WaitingConnect = EStates::Count,
            CreatingGame,
            LoadingGame,
            ChoosingOptions,
            SetupTutorial,
            ReceivingData,
            WaitingGame,
            InLobby,
            Count,
         };
      }

      class GConnecting : public GSubStateMachine
      {
      public:
         GConnecting();

         UINT32 ID()
         {
            return FSM::EStates::Connecting;
         }

         UINT32 StartSubState() const;

         UINT32 OnEvent(UINT32 in_iEventID);
      };

      class GWaitingConnect : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EConnectingStates::WaitingConnect;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         UINT32 OnIterate();
         void OnLeave();
      };

      class GCreatingGame : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EConnectingStates::CreatingGame;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         UINT32 OnIterate();
         void OnLeave();
      };

      class GLoadingGame : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EConnectingStates::LoadingGame;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         UINT32 OnIterate();
         void OnLeave();
      };

      class GChoosingOptions : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EConnectingStates::ChoosingOptions;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         void OnLeave();
      };

      class GSetupTutorial : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EConnectingStates::SetupTutorial;
         }

         UINT32 OnIterate();
      };

      class GReceivingData : public SP2::GState
      {
         INT32 m_iStartFlag;

      public:
         UINT32 ID()
         {
            return FSM::EConnectingStates::ReceivingData;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         void OnLeave();
      };

      class GWaitingGame : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EConnectingStates::WaitingGame;
         }

         void OnEnter();
         void OnLeave();
      };

      class GInLobby : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EConnectingStates::InLobby;
         }

         void OnEnter();
         void OnLeave();
      };
   }
}

#endif // #ifndef _SP2_FSM_CONNECTING_H_
