/**************************************************************
*
* sp2_data_control_layer.h
*
* Description
* ===========
* Hold the GDataControlLayer class implemantation
* This class will be used to control everything
*
* Owner
* =====
*  François Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _GOLEM_SP2_GDCL_H_
#define _GOLEM_SP2_GDCL_H_

#include "../include/sp2_tutorials.h"

namespace SP2
{
   namespace FSM
   {
      class GConnecting;
      class GDisconnected;
      class GInit;
      class GLoadData;
      class GPlaying;
      class GReceivingData;
   }

   namespace EGameTypes
   {
      enum Enum;
   }

   namespace EUnitMovementPermissions
   {
      const UINT32 c_iAnyUnitCanMove           = 0;
      const UINT32 c_iNoUnitCanMove            = 0xFFFFFFFF;
      const UINT32 c_iUnitCanMoveAnywhere      = 0;
      const UINT32 c_iUnitCantMoveToAnyCountry = 0xFFFFFFFF;
   }

   struct SEarthState
   {
      enum EAction
      {
         NONE = 0,
         ZOOM_ABS,
         ZOOM_REGION,
      };
      EAction     m_Action;
      REAL32      m_Zoom;
      REAL32      m_Latitude;
      REAL32      m_Longitude;

      bool operator != (const SEarthState& in_OtherState)
      {
         return (m_Zoom != in_OtherState.m_Zoom) || 
                (m_Latitude != in_OtherState.m_Latitude) || 
                (m_Longitude != in_OtherState.m_Longitude);
      }
   };

   struct GSaveGameData
   {
      GDateTime                  m_SaveDate;
      GDateTime                  m_GameDate;
      GSmartPtr<GGameScenario>   m_pScenario;
      UINT32                     m_iGameIsMp;
      UINT32                     m_iCountryID;
      GString                    m_sCountryName;
      GString                    m_sCountryFlagPath;
      GString                    m_sPlayerName;
      bool                       m_bInfoComplete;
   };


   struct GAutoSaveInformation
   {
      bool                       m_bAutoSave;
      GString                    m_sSaveFileName;
      UINT32                     m_iSaveFrequency;
      bool                       m_bIncrementalSave;
      REAL32                     m_fLastSaveTime;
      bool                       m_bNowAutoSaving;
      UINT32                     m_iSaveCount;
   };

   const DesignPattern::GSubjectNotification c_NotificationOnSelectedCountryChange = 1561344;

   class GDataControlLayer : public DesignPattern::GObserver, public DesignPattern::GSubject, public GDCLInterface, public SDK::GGameDataNode
   {
   private:
      friend class GConnectionThread;
      friend class GGameLobbyWindow;

      // Selected Country window vars
      INT32                      m_iSelectedCountryID;
      GString                    m_sSelectedCountryFlagPath;
      GDateTime                  m_StartTime;    
      bool                       m_bStartTimeSet;
      vector<UINT32>             m_RegionIds;     

      REAL64                     m_fFiveSecondsClock;

	  std::vector<std::string>	 m_PublicAndLocalIPs;
      GString                    m_sIP;
      GString                    m_sServerName;
      GString                    m_sPwd;
      UINT32                     m_iPort;
      UINT32                     m_iMaxPlayers;
      UINT32                     m_iTimeLimit;
      bool                       m_bPrivate;
      bool                       m_bDedicated;

      GClientAdvisor             m_ClientAdvisor;

      //! RequestManager
      SP2::GRequestManager       m_RequestManager;

		void								LoadGameOptions();

      //! AutoSave
      GAutoSaveInformation       m_AutoSaveInformation;

      // List of unit to train
      list<UINT32> m_UnitListToTrain;

      // Curent game being loaded or saved
      GString                    m_sCurrentGame;
      GSaveGameData              m_CurrentGameData;
      SDK::EGameDataError::Enum  m_eSaveLoadResult;

      // List of saved games, associated by name
      vector<pair<GString, GSaveGameData> > m_vSaveList;

      // GGameDataNode implementation
      virtual bool OnSave(GIBuffer& io_Buffer);
      virtual bool OnLoad(GOBuffer& io_Buffer);

      // limited movement for tutorial purposes
      UINT32 m_iSpecificUnitToMove;
      UINT32 m_iSpecificDestinationCountry;

      UINT32 m_iRefusedMovCount[SP2::ERefusedMoveReason::Count];

      UINT32 m_iEscapeKeyId;

   protected:
      // All the following methods should be called by state objects only
      friend FSM::GConnecting;
      friend FSM::GDisconnected;
      friend FSM::GInit;
      friend FSM::GLoadData;
      friend FSM::GPlaying;
      friend FSM::GReceivingData;

      bool CleanGame();
      void ConnectToGame(EGameTypes::Enum in_eGameType);

      // Called once at startup
      void InitGame();

      // Called once after intro movies were played
      void LoadGameData();

      // Called when ready to receive server game data
      void ReceiveData();

      // Called when game start
      void BeginGame();

      // Called at each game iteration while playing
      void IterateGame();

      // Called at each game iteration to specify theme and intensity
      void IterateMusicAdaptiveness(void);

      void UpdateCombatInfo();

   public:
      GDataControlLayer(GDALInterface* in_pDAL);
      ~GDataControlLayer();

      Map::GMultifaceView* CountryView();

      // Domestic policies window related events
      void DomesticPoliciesWindowSet();

      // update the current country units displays
      void UpdateCurrentCountryUnits();

      // Selected Country window related events
      INT32 SelectedCountryID();
      bool SelectedCountrySet(const SP2::GCountry& in_iCountry,bool in_bForce = false);
      bool SelectedCountrySetLookAt(const SP2::GCountry& in_iCountry,bool in_bForce = false);

      void WindowsEnabled(bool in_bEnabled);

      bool HandleGetCountryRanks(INT16 in_iEconomicRank, INT16 in_iPoliticRank);

      // System events
      bool RequestCountryListToServer();
      void ReceiveCountryListFromServer(SP2::Event::GReceiveCountryList* in_pData);
      void ReceiveStaticCountryDataError();

      void RequestStaticCountryDataToServer();
      void ReceiveStaticCountryDataFromServer(SP2::Event::GReceiveCountryStaticData* in_pData);

      bool SystemRequestAvailablesCountriesToServer();
      bool SystemSendPlayerInformationToServer(UINT32 in_iControlledCountryID,const GString& in_sPlayerName,SDK::GEPlayerStatus  in_ePlayerStatus, INT32 in_iPartyID = -1);
      void RequestCountriesRanks();

      void RegisterEscapeHotkey();
      void UnregisterEscapeHotkey();

      // Mail window handle
      static void ShowTreaty(void* in_iTreatyID);
		static void ShowTreatyRefusal(void* in_pMailID);
      static void ShowTrade(void* in_pMailID);
      static void ShowCombatOver(void* in_pMailID);
      static void ShowMissionReady(void* in_CellID);
      static void ShowMissionResult(void* in_pResultStruct);
      static void ShowMovementRefused(void* in_pRefusalID);

      //! AutoSave access
      GAutoSaveInformation& AutoSaveInformation(void);
      void AutoSave(void);

      //! Starts the given tutorial
      void StartTutorial(ETutorials::Enum in_eTutorial);

      //vector<UINT32> & CurrentRegionIds();
      vector<GString> & Currencies();

      //! Switch to the Start the multiplayer game state
      bool StartMultiplayerGame();

      //! Start a single player game
      bool StartSingleplayerGame();

      //! start a single player game with the scenario options
      bool StartCampaingSinglePlayerGame();

      // Spawn the host MP game server
      void StartHostServerGame();

      //! Join a multiplayer game
      bool JoinMultiplayerGame();

      //! Connect to a server
	  void Connect(const GString& in_sIP, UINT32 in_iPort, const GString& in_sModName, const GString& in_sPassword);
      void Connect(const GString& in_sIP, UINT32 in_iPort, const GString& in_sPassword, const std::vector<std::string>& in_vsIPS);

      //! Switch to the Playing State
      bool StartGame();

      //! Cancel connection
      void CancelConnect();

      //! Cancel join multiplayer game
      void CancelJoinMPGame();

      //! Cancel host multiplayer game
      void CancelHostMPGame();

      //! Called when tutorial game is started
      void SetupTutorial();

      //! Called when data has been received
      void InitialDataReceived();

      //! Ask server to reset the current game
      void ResetServer();

      //! Ask server to create the game
      void CreateNewGame();

      //! Ask server to load a game
      void LoadSavedGameOnServer();

      //! Save the current game
      void SaveGame(const GString& in_sSaveName);

      //! Load a previous current game
      void LoadGame(const GString& in_sLoadName);

      //! Receive the result of the game creation
      void FinalizeGameNew(EGameDataError::Enum in_eLoadResult);

      //! Receive the result of the game load
      void FinalizeGameLoad(EGameDataError::Enum in_eLoadResult);

      //! Called to display the result of the current load operation
      void DisplayLoadResult();

      //! Called to display the result of the current new operation
      void DisplayNewResult();

      //! Receive the result of the game save
      void FinalizeGameSave(EGameDataError::Enum in_eSaveResult);

      //! Called to display the result of the current save operation
      void DisplaySaveResult();

      //! Access the client advisor
      GClientAdvisor& ClientAdvisor();


      void BombardRegionWithSelectedUnits(UINT32 in_iRegionID);

	  //! Request the list of used unit
	  void RequestUsedUnit(void);

      //! Toggle the console display
      bool ConsoleDisplayToggle();

      //!Highlight all the regions of a country
      bool HighlightCountry(void * in_iCountryID);

      //Select the home country
      bool SelectHomeCountry();

      //! Limit unit movement
      void LimitMovementTo(UINT32 in_iUnitID = EUnitMovementPermissions::c_iAnyUnitCanMove, 
                           UINT32 in_iCountryID = EUnitMovementPermissions::c_iUnitCanMoveAnywhere);

      //Request a unit move
      bool RequestUnitMove(const vector<UINT32>& in_vUnitID, 
                           const GVector2D<REAL32>& in_Dest, 
                           EMilitaryAction::Flags in_Action);

      void RefuseMovement(UINT32 in_iUnitCount, 
                          SP2::ERefusedMoveReason::Enum in_eReason);

      bool ChangeUnitStatus(EMilitaryStatus::Enum in_eNewStatus);

      bool RequestGameSpeedChange(REAL64 in_fNewSpeed);

      //! Request the relation history between the 2 given countries (auto updates will be send until we stop requesting updates)
      void StartRequestRelationHistory(UINT32 in_iCountry1,UINT32 in_iCountry2);
      void EndRequestRelationHistory();

      //! Requests a unit production (sends an order to the server)
      void RequestUnitProduction(UINT32 in_iDesignID,UINT32 in_iQuantity,UINT32 in_iProductionSpeed,UINT32 in_iBuildingCountryID,EUnitProductionPriority::Enum in_ePriority);

      //!Display combat overall information
      bool DisplayCombatInfo(const Combat::GArenaInfo* in_pArena);
      bool DisplayCombatInfoSmall(const Combat::GArenaInfo* in_pArena);
      bool KillCombatOverview();
      bool KillCombatInfo();

      //! Bombardment notification, says when a bombardment started or stopped in a given region
      void OnBombardmentNotification(UINT32 in_iRegionID,bool in_bStarting,const vector<GVector2D<REAL32> >& in_vBombardedPoints);

      bool UpdateWorldPosition();
      bool UpdateWorldPosition(SEarthState & in_State);

      ECharacteristicToHighlight::Enum CurrentThematicMap;
      vector<GColorRGBReal> ThematicMapColorData;


      bool HandleCombatInformationUDPPacket(const GCombatInformationPacket& in_Packet);

      void OnSubjectChange(DesignPattern::GSubject& in_Subject,
                           const DesignPattern::GSubjectNotification& in_Notification,
                           void* in_pAdditional);

      //! retrieve the request manager of our dcl
      inline SP2::GRequestManager& RequestManager() { return m_RequestManager; }

      //! confirms the territory to occupy, confirm war declarations and raise event
      void OccupyTerritory(UINT32 in_iCountryID);

      //! Inform player the cost of training units
      void TrainUnits(list<UINT32>& in_ListOfUnitIdToTrain, GUI::GBaseObject* in_pGuiCallback, Hector::DesignPattern::GObserver *in_pWhoNeedToBeNotify);

      void DisbandUnits(list<UINT32>& in_ListOfUnitIdToDisband);

      //! Ask server to train units
      void RequestTraining(void);

      void BuildSavedGameList();

      const vector<pair<GString, GSaveGameData> >& SavedGameList() const
      {
         return m_vSaveList;
      }

      void SelectScenario(GSmartPtr<GGameScenario> in_pScenario, 
                          UINT32 in_iCountrySelection,
                          const GString& in_sPlayerName);

      //    variables needed to 
      UINT32                           m_iTerritoryToOccupy;
      set<UINT32>                      m_OccupyTerritoryPendingWarDeclarations;
      set<UINT32>                      m_OccupyTerritoryConfirmedWarDeclarations;
      SEarthState                      m_EarthState;
      const DesignPattern::GSubject*   m_pSubjectThatWillAnswerOccupyTerritoryQuestion;

      // Used at server connection to set current objectives
      vector<GGameObjective> m_vGameObjectives;

      bool ServerIsReseted() const;

      bool m_bServerIsReset;
   };

   struct SListMultiplayerGamesArgs
   {
      UINT8 m_iGameType;
      class GJoinMPGameWindow* m_pWindow;
   };

}

#endif //_GOLEM_SP2_GDCL_H_
