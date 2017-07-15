/**************************************************************
*
* sp2_fsm_playing.h
*
* Description
* ===========
*  Description of the Playing game state, which itself 
*  contains a sub state machine.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_FSM_PLAYING_H
#define _SP2_FSM_PLAYING_H

#include "sp2_fsm_disconnected.h"

namespace SP2
{
   namespace FSM
   {
      // States specific to the playing state sub FSM
      namespace EPlayingStates
      {
         enum Enum
         {
            InGame = EDisconnectedStates::Count,
            SavingGame,
            Credits,
            WaitingReset,
            Count,
         };
      }

      class GPlaying : public GSubStateMachine
      {
         UINT32 m_iResetStateID;

      public:
         GPlaying();

         UINT32 ID()
         {
            return FSM::EStates::Playing;
         }

         UINT32 StartSubState() const;

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         UINT32 OnIterate();
         void OnLeave();
      };

      class GInGame : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EPlayingStates::InGame;
         }

         UINT32 OnEvent(UINT32 in_iEventID);
      };

      class GSavingGame : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EPlayingStates::SavingGame;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         UINT32 OnIterate();
         void OnLeave();
      };

      class GCredits : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EPlayingStates::Credits;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         UINT32 OnIterate();
         void OnLeave();
      };

      class GWaitingReset : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EPlayingStates::WaitingReset;
         }

         void OnEnter();
         UINT32 OnIterate();
         void OnLeave();
      };
   }
}

#endif // #ifndef _SP2_FSM_PLAYING_H
