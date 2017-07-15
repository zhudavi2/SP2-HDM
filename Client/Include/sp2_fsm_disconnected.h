/**************************************************************
*
* sp2_fsm_disconnected.h
*
* Description
* ===========
*  Description of the Disconnected game state, which itself 
*  contains a sub state machine.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_FSM_DISCONNECTED_H_
#define _SP2_FSM_DISCONNECTED_H_

#include "sp2_fsm_connecting.h"

namespace SP2
{
   namespace FSM
   {
      // States specific to the disconnected state sub FSM
      namespace EDisconnectedStates
      {
         enum Enum
         {
            InMainMenu = EConnectingStates::Count,
            InSubMenuSP,
            InSubMenuMP,
            InHostWindow,
            InJoinWindow,
            Count,
         };
      }

      class GDisconnected : public GSubStateMachine
      {
      public:
         GDisconnected();

         UINT32 ID()
         {
            return FSM::EStates::Disconnected;
         }

         UINT32 StartSubState() const;

         UINT32 OnEvent(UINT32 in_iEventID);
      };

      class GInMainMenu : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EDisconnectedStates::InMainMenu;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         void OnLeave();
      };

      class GInSubMenuSP : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EDisconnectedStates::InSubMenuSP;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         void OnLeave();         
      };

      class GInSubMenuMP : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EDisconnectedStates::InSubMenuMP;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         void OnLeave();
         UINT32 OnIterate();
      };

      class GInHostWindow : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EDisconnectedStates::InHostWindow;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         void OnLeave();
      };

      class GInJoinWindow : public SP2::GState
      {
      public:
         UINT32 ID()
         {
            return FSM::EDisconnectedStates::InJoinWindow;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         void OnLeave();
      };
   }
}

#endif // #ifndef _SP2_FSM_DISCONNECTED_H_
