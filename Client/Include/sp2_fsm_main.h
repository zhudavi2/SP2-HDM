/**************************************************************
*
* sp2_fsm_main.h
*
* Description
* ===========
*  Describes the main client finite state machine
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _GOLEM_FSM_MAIN_H_
#define _GOLEM_FSM_MAIN_H_

#include "sp2_state_machine.h"
#include "sp2_movie_player.h"

namespace SP2
{
   namespace FSM
   {
      namespace EStates
      {
         enum Enum
         {
            Init = 1,
            Movies,
            LoadData,
            DisplayUnofficial,
            Disconnected,
            Connecting,
            Playing,
            Quit,
            Count, // Should always be the last element
         };
      }

      namespace EEvents
      {
         enum Enum
         {
            NoEvent = 0,

            // Main menu buttons
            SPMenuBtn = 1,
            MPMenuBtn,              // 2   
            SubMenuBackBtn,         // 3

            StartSinglePlayerBtn,   // 4
            StartMultiplayerBtn,    // 5
            JoinMultiplayerBtn,     // 6
            ScenarioBtn,            // 7
            TutorialBtn,            // 8
            QuitBtn,                // 9

            // Join MP buttons
            JoinConnectBtn,
            JoinMPCancel,

            // Host MP buttons
            HostStartBtn,           // 12
            HostMPCancel,

            // Connection events
            ConnectAtStartup,       // 14
            CancelConnect,
            Disconnection,
            Connected,

            // Game events
            StartGame,              // 18

            // Save load events
            SaveGame,               // 19
            EndSaveGame,
            LoadGame,
            EndLoadGame,
            EndNewGame,
            ErrorNewOrLoadGame,
            CancelNewOrLoad,

            // Misc events
            CountryDataReceived,    // 26
            CountryDataFailed,
            DataReceived,           // 28
            OptionSelected,
            RestartGame,
            StopGame,
            InitError,
            CancelMovie,
            PlayCredits,

            Count,   // Must always be the last element
         };
      }

      class GMain : public SP2::GStateMachine
      {
      public:
         GMain();
      };

      /*!
       * Init state, that happens only at game startup
       **/
      class GInit : public GState
      {
         UINT32 ID()
         {
            return EStates::Init;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         UINT32 OnIterate();
      };

      /*!
       * Display Unofficial state, that happens juste before loading data 
       * if the current mod is not official
       **/
      class GDisplayUnofficial : public GState
      {
         UINT32 ID()
         {
            return EStates::DisplayUnofficial;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         UINT32 OnIterate();
         void OnLeave();
      };

      /*!
       * Load data state, that happens juste before displaying main menu
       **/
      class GLoadData : public GState
      {
         UINT32 ID()
         {
            return EStates::LoadData;
         }

         UINT32 OnEvent(UINT32 in_iEventID);

         void OnEnter();
         UINT32 OnIterate();
         void OnLeave();
      };

      /*!
       * Quit state, that happens only at game shutdown
       **/
      class GQuit : public GState
      {
      public:
         UINT32 ID()
         {
            return EStates::Quit;
         }

         UINT32 OnIterate();
      };

      // Forward declarations of states with sub FSM
      class GMovies;
      class GConnecting;
      class GPlaying;
   }
}

#endif // #ifndef _GOLEM_FSM_MAIN_H_
