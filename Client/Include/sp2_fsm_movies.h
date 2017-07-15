/**************************************************************
*
* sp2_fsm_movies.h
*
* Description
* ===========
*  Description of the Movies game state, which itself 
*  contains a sub state machine.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_FSM_MOVIES_H_
#define _SP2_FSM_MOVIES_H_

#include "sp2_fsm_playing.h"

namespace SP2
{
   namespace FSM
   {
      // States specific to the movies state sub FSM
      namespace EMoviesState
      {
         enum Enum
         {
            LogoDC = EPlayingStates::Count,
            LogoGolem,
            Intro,
            Count,
         };
      }

      class GMovies : public GSubStateMachine
      {
      public:
         GMovies();

         UINT32 ID()
         {
            return FSM::EStates::Movies;
         }

         UINT32 StartSubState() const;

         UINT32 OnIterate();
      };

      class GLogoDC : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EMoviesState::LogoDC;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         UINT32 OnIterate();
         void OnLeave();
      };

      class GLogoGolem : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EMoviesState::LogoGolem;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         UINT32 OnIterate();
         void OnLeave();
      };

      class GIntro : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EMoviesState::Intro;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         UINT32 OnIterate();
         void OnLeave();
      };

   }
}

#endif // #ifndef _SP2_FSM_MOVIES_H_
