/**************************************************************
*
* sp2_client.h
*
* Description
* ===========
*  Hold the GClient class implemantation
*  This class will be used to handle different game states
*
* Owner
* =====
*  François Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SP2_GCLIENT_H_
#define _GOLEM_SP2_GCLIENT_H_

#include "sp2_client_advisor.h"
#include "sp2_request_manager.h"


#include "sp2_data_access_layer.h"
#include "sp2_data_control_layer.h"
#include "sp2_data_display_layer.h"

#include "sp2_combat_event_handler.h"
#include "sp2_economic_event_handler.h"
#include "sp2_general_event_handler.h"
#include "sp2_military_event_handler.h"
#include "sp2_politic_event_handler.h"
#include "sp2_treaty_event_handler.h"
#include "sp2_unit_production_event_handler.h"
#include "sp2_country_data_client.h"
#include "sp2_adaptive_music_player.h"


#define g_SP2Client ((SP2::GClient*)g_Joshua.Client())
#define g_ClientDDL ((SP2::GClient*)g_Joshua.Client())->DDL()
#define g_ClientDCL ((SP2::GClient*)g_Joshua.Client())->DCL()
#define g_ClientDAL ((SP2::GClient*)g_Joshua.Client())->DAL()



namespace SP2
{  
   // Forward declarations
   namespace FSM
   {
      class GMain;
      class GConnecting;
      class GInit;
      class GDisconnected;
   }

   //! List all possible game types
   namespace EGameTypes
   {
      enum Enum
      {
         None,
         SinglePlayer,
         HostMultiPlayer,
         JoinMultiPlayer,
         Scenario,
         Tutorial,
         Count,   // Must always be last
      };
   }

   /*!
    * Game client for SP2, consists of a state machine that creates / reacts
    * to events.
    **/
   class GClient : public GGameClientItf
   {
      friend class GDataControlLayer;
      friend class GDataAccessLayerClient;
      friend class GConnectingWindow;
      friend class FSM::GConnecting;
      friend class FSM::GInit;
      friend class FSM::GDisconnected;
      friend class FSM::GPlaying;
      friend class GSubmenuSingle;
      friend class GSubmenuMulti;
      friend class GMainMenu;

   private:  
      //! State machine for SP2
      SP2::FSM::GMain*              m_pStateMachine;

      //! DAL
      SP2::GDataAccessLayerClient   m_DAL;
      //! DDL
      SP2::GDataDisplayLayer        m_DDL;
      //! DCL
      SP2::GDataControlLayer        m_DCL;      

      EGameTypes::Enum              m_eCurrentGameType;

      //! Sounds
      SP2::GSP2Sound*               m_pSoundBox;

      //! Musics
      AdaptiveMusic::GThemePlayer*                      m_pAdaptiveMusicPlayer;

      //! General game event handler
      SP2::GGeneralEventHandler     m_EventHandler;

      //! Economic, politic and military game event handler
      SP2::GEconomicEventHandler       m_EconomicEventHandler;
      SP2::GPoliticEventHandler        m_PoliticEventHandler;
      SP2::GMilitaryEventHandler       m_MilitaryEventHandler;
      SP2::GUnitProductionEventHandler m_UnitProductionEventHandler;

      SP2::GTreatyEventHandler   m_TreatyEventHandler;

      void ChangeGameType(EGameTypes::Enum in_eNewGameType);
      

      void RegisterElements();

      GString ConsoleMainCommandsHandler(const GString &, const vector<GString> &);

      //! Function that will receive the UDP Packets
      virtual void OnUDPPacketReception(GOBuffer &, const sockaddr_in & l_From);

      multimap<REAL64,SP2::ESoundEventID::Enum> m_DelayedSound;

      // Private, so only data control can access it
      void LoadGame(bool in_bLoading) { m_bLoadingGame = in_bLoading; }
      bool                       m_bLoadingGame;
      bool                       m_bCheckForEngineConnection;
      bool                       m_bLimitFPS;

      UINT32                     m_iScenarioSphere;
      bool                       m_bScenarioIsHard;
      bool                       m_bPlayCredits;
      UINT32                     m_iSavedFSMEvent;

   protected:
   public:
      GClient();
      ~GClient();

      // Change musical theme immediatly
      void ImmediatlyChangeMusicalTheme(AdaptiveMusic::ESP2MusicalTheme::Enum in_eTheme);

      AdaptiveMusic::GThemePlayer* AdaptiveMusicPlayer(void) { return m_pAdaptiveMusicPlayer;};

      //! Plays the given sound if the soundbox exists, else ignores the event
      void PlaySound(SP2::ESoundEventID::Enum in_eSoundEventID);

      //! Plays a sound with delay
      void PlaySoundWithDelay(SP2::ESoundEventID::Enum in_eSoundEventID, REAL32 in_fDelay, SP2::ESoundContext::Enum in_eContext);

      //! Plays a sound with a game time delay
      void PlaySoundWithGametimeDelay(SP2::ESoundEventID::Enum in_eSoundEventID,REAL64 in_fTimeToPlay);

      //! Return true if the sound is playing
      bool IsSoundPlaying(SP2::ESoundEventID::Enum in_eSoundEventID, SP2::ESoundContext::Enum in_eContext);


      //! Gets the data access layer instance
      /*!
       * Gets the data access layer instance
       * @return GDataAccessLayer the unique instance of the DAL
       **/
      inline SP2::GDataAccessLayerClient& DAL() { return m_DAL; }

      //! Gets the data display layer instance
      /*!
       * Gets the data display layer instance
       * @return GDataDisplayLayer the unique instance of the DDL
       **/
      inline SP2::GDataDisplayLayer& DDL() { return m_DDL; }

      //! Gets the data control layer instance
      /*!
       * Gets the data control layer instance
       * @return GDataControlLayer the unique instance of the DCL
       **/
      inline SP2::GDataControlLayer& DCL() { return m_DCL; }

      GSP2Sound* SoundBox(void);

      class GChat * Chat();

      virtual void OnChatMessage(INT32, const GString &, bool in_bPrivate);

      //Pure virtual functions to define a Game Client
      GAME_MSG Initialize();           //Initialize everything
	   GAME_MSG Iterate(void* param);   //Iterate function (called every frame)
      void IterateSound(void);         //Iterate over pending sounds.
      GAME_MSG Shutdown();             //Shuts down the client

      //Virtual function, that mentions when the client gets connected to the server
      void OnConnectedToServer(INT32 in_iPlayerID);
      void OnServerDisconnected(ConnectionLostReason::Enum in_eReason);
      void OnConnectionError(ConnectionLostReason::Enum in_eReason);
      void GClient::DisplayConnectionError(ConnectionLostReason::Enum in_eReason);
      void OnGlobalHotkey(const GUI::GEventData &);
      void OnNewServerListed(bool in_bIsLocal, const GListedServer & in_Server);

	  //RGAPI
	  void OnRGAPILoginCompleted(bool bSuccess);
	  void OnRGAPIRegisterCompleted(bool bSuccess);

      void RestartGame();
      void StopGame();
      void QuitGame();

      bool LoadingGame() const { return m_bLoadingGame; }

      UINT32 CurrentFSMState();
      EGameTypes::Enum CurrentGameType();

      void LimitFPS(bool in_bLimit);

      void ScenarioSphere(UINT32 in_iSphereID, bool in_bIsHard);

      UINT32 CurrentScenarioSphere() const { return m_iScenarioSphere; }
      bool ScenarioIsHard() const { return m_bScenarioIsHard; }

      void PlayCredits(bool in_bPlay) { m_bPlayCredits = in_bPlay; }
      bool PlayCredits() const { return m_bPlayCredits; }

      void ContinueCurrentGame(UINT32 in_iFSMEvent);
      void ContinueCurrentGame();

   }; //end of class GClient

}; //End namespace SP2

#endif //_GOLEM_SP2_GCLIENT_H_