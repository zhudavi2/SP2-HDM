/**************************************************************
*
* sp2_server.h
*
* Description
* ===========
*  Class that represents the game server.
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _SP2_SERVER_H
#define _SP2_SERVER_H


namespace SP2
{
   //Forward Declaration
   class GGeneralEventHandler;
   class GEconomicEventHandler;
   class GMilitaryEventHandler;

   namespace EGlobalTaxSpecialType
   {
      enum Enum
      {
         ExportAll = 0,
         MeetDomestic,
         ImportAll,
         PrivatizeAll,
         ItemCount,
      };
   }

   struct GAnarchyConfig
   {
       REAL32 m_fChanceDueToStability;
       REAL32 m_fExpectedStabilityLowerLimit;
       REAL32 m_fExpectedStabilityUpperLimit;
       REAL32 m_fStabilityLowerLimit;
   };

   struct GCivilWarConfig
   {
       ENTITY_ID m_iRebelsId;
       REAL32 m_fChance;
       REAL32 m_fControlChance;
       REAL32 m_fAnnexChance;
   };

   /*!
   * Superpower 2 Game Server Main class
   **/
   class GServer : public SDK::GGameServerItf , public SDK::GGameDataNode
   {

   public:
      GServer();
      ~GServer();
      SDK::GAME_MSG   Initialize();
      SDK::GAME_MSG   Iterate(void* param);
      SDK::GAME_MSG   Shutdown();
      SDK::GStringTable * StrTable();

      //Function called when a player has joined
      void OnNewPlayerJoined(SDK::GPlayer* in_pPlayer);
      //! Handle disconnections
      void OnPlayerDisconnect(SDK::GPlayer* in_pPlayer);

      //! Gets the data access layer instance
      /*!
       * Get the Data Access Layer Object
       * @return GDataAccessLayerServer: A reference to the DAL
       **/
      inline GDataAccessLayerServer& DAL() { return m_DAL; }

      //! Get the IDs of the human players
      //const vector<UINT32>& HumanPlayers();

      //! Initialize the combat engine, register types etc.
      bool CombatEngineInit();
      //! Shutdown the combat engine
      bool CombatEngineShutdown();

      void SendCountryList(INT32 = SDK::Event::ESpecialTargets::BroadcastHumanPlayers);
      void SendPlayersList(INT32 = SDK::Event::ESpecialTargets::BroadcastHumanPlayers);
      

      //! Get the data control layer
      inline GDataControlLayer& DCL() { return m_DCL; }

      bool GameStarted();
      void GameStarted(bool in_bGameStarted);
      void TryStartGame(void);

      //! Returns true only if game data is loaded
      bool GameLoaded() const;

      void ActivateAIPlayers(void);
      void ActivateReadyHumanPlayers(void);
      UINT32 IsCountryChoosen(UINT16 in_iCountryID);
      void SendInitialDataToPlayer(SDK::GPlayer* in_pPlayer);
      void SendCombatInfo(UINT32 in_iPlayerID);
      void SyncronizeMilitary(INT32 in_iTarget,REAL64 in_fActualClock);
      void SendGameStartEvent(INT32 in_Target = SDK::Event::ESpecialTargets::BroadcastActivePlayers);

      void RequestNewGame(const GString& in_sNewGameDB);
      void RequestNewGame(const GString& in_sNewGameDB, UINT32 in_iSource);
      void RequestLoadGame(const GString& in_sLoadGameFile);
      void RequestLoadGame(const GString& in_sLoadName, UINT32 in_iSource);
      void RequestSaveGame(const GString& in_sSaveGameFile);
      void RequestSaveGame(const GString& in_sSaveName, UINT32 in_iSource);
      void RequestSomeKickingAction(const GString& in_sPlayerNameToKick);
      void RequestSomeKickingAction(UINT32 in_iPlayerID);
      void RequestSomeBanningAction(const GString& in_sPlayerNameToBan);
      void RequestSomeBanningAction(UINT32 in_iPlayerID);


      GCountries& Countries(){return m_Countries;}

      GWorldBehavior& WorldBehavior(){return m_WorldBehavior;}

      inline REAL64 LoadClockAdjust() const { return m_fLoadClockAdjust; }

      inline bool UpdateStaticOnly() const { return m_bUpdateStaticOnly; }
      inline bool CleanVariableOnly() const { return m_bCleanVariableOnly; }
      inline REAL64 NextGameSpeed() const { return m_fNextGameSpeed; }

		REAL32 AIAggressiveness();
		void AIAggressiveness(REAL32 in_fAIAggressiveness);

      void SendChatMessage(INT32 in_iSource, INT32 in_iTarget, const GString& in_sMessage) const;

      /*!
      * Helper method to synchronize a player's own country data with client
      * @param in_Player: Player, target of sync event. Non-const because GPlayer::Id and ModID are non-const
      * @param in_bSendAll: True if all data should be sent, false if only dirty data should be sent
      **/
      void SynchronizePlayerCountryData(SDK::GPlayer& in_Player, bool in_bSendAll) const;

      bool ActivateAllDatabaseCountries() const;

      inline bool    AllowAIAssumeDebt() const { return m_bAllowAIAssumeDebt; }
      inline bool    AllowHumanClientStates() const { return m_bAllowHumanClientStates; }
      inline bool    AllowDefenderAttackAttackerTerritory() const { return m_bAllowDefenderAttackAttackerTerritory; }

      GAnarchyConfig AnarchyConfig() const;

      inline REAL32  AnnexationRelationLossPercent() const { return m_fAnnexationRelationLossPercent; }
      inline bool    AutoCovertMissions() const { return m_bAutoCovertMissions; }

      inline GCivilWarConfig CivilWarConfig() const { return m_CivilWarConfig; }

      inline REAL32  CombatThresholdSquare() const { return m_fCombatThresholdSquare; }
      inline bool    ContinueAfterEconomicFailure() const { return m_bContinueAfterEconomicFailure; }

      bool ContinueAfterElectionLoss() const;
      bool CountryNeedsRegions() const;

      inline bool    DisableNuclearOnOccupy() const { return m_bDisableNuclearOnOccupy; }
      inline bool    DisbandAMDSOnOccupy() const { return m_bDisbandAMDSOnOccupy; }

      //! Maximum global resource tax rate
      REAL32 GlobalTaxLimit() const;
      void   GlobalTaxLimit(REAL32 in_fGlobalTaxLimit);

      inline INT32   GlobalTaxSpecial(EGlobalTaxSpecialType::Enum in_eGlobalTaxSpecial) { return m_GlobalTaxSpecials.at(in_eGlobalTaxSpecial); }

	  REAL64 GvtTypeProductionModifier(EGovernmentType::Enum in_iGvtType) const;

      //! Maximum income tax rate per government type
      REAL64 IncomeTaxLimit(EGovernmentType::Enum in_eGovernmentType) const;
      void   IncomeTaxLimit(EGovernmentType::Enum in_eGovernmentType, REAL64 in_fIncomeTaxLimit);

      bool IncreaseDeathRateForAgingPopulation() const;

      inline bool    LogBankruptcies() const { return m_bLogBankruptcies; }

      //! Maximum number of covert cells each country can have in each foreign country, 0 means no limit
      INT32 MaximumCellsInForeignCountry() const;
      void  MaximumCellsInForeignCountry(INT32 in_iMaxCells);

      inline GString Message() const { return m_sMessage; }
      inline REAL32  MilitaryUpkeepPercentages(EUnitCategory::Enum in_eUnitCategory, ETrainingLevel::Enum in_eTrainingLevel) const { return m_mMilitaryUpkeepPercentages.at(in_eUnitCategory).at(in_eTrainingLevel); }
      inline bool    NavalRuleEnabled() const { return m_bNavalRuleEnabled; }
      inline REAL32  NuclearMissileRangePercentage() const { return m_fNuclearMissileRangePercentage; }
      inline REAL32  NuclearUpkeepPercentage() const { return m_fNuclearUpkeepPercentage; }
      inline REAL32  OccupiedRegionPercentageForNuclear() const { return m_fOccupiedRegionPercentageForNuclear; }
      inline REAL32  ProductionLossOnAnnex() const { return m_fProductionLossOnAnnex; }

      //! Maximum tax rate for any resource
      REAL32 ResourceTaxLimit() const;
      void   ResourceTaxLimit(REAL32 in_fResourceTaxLimit);

      inline REAL32  TributePercent() const { return m_fTributePercent; }

      bool UseNewExportMechanics() const;

      //! Player data helpers
      void AddPlayer(INT32 in_iId, UINT32 in_iPassword);
      bool PlayerExists(INT32 in_iId) const;
      UINT32 PlayerPassword(INT32 in_iId) const;

   protected:
      //! Trigger load game flag
      void GameLoaded(bool in_bGameStarted);

      //! Start the game
      bool StartGame();

      //! Load our mod command line parameters
      void LoadGameOptions();

   private:
      //! Initialize the AI (register actions, objectives and entities in the EHE)
      bool InitAI();
      void RegisterObjectives();
      void RegisterActions();


      //! Function that will receive the UDP Packets
      void OnUDPPacketReception(int in_iPacketSize, void* in_pPointer);

      
      //Handle general game events		
      GGeneralEventHandler    m_EventHandler;
		GAdvisorEventHandler    m_AdvisorEventHandler;
      GEconomicEventHandler   m_EconomicGameEventHandler;
      GPoliticEventHandler    m_PoliticGameEventHandler;
      GMilitaryEventHandler   m_MilitaryGameEventHandler;
		GTreatyEventHandler		m_TreatyGameEventHandler;
      GUnitProductionEventHandler   m_UnitProductionGameEventHandler;

      GAIGeneralEventHandler  m_AIGeneralEventHandler;

      //  Game Data
      //-------------
      GCountries  m_Countries; //Country Id = Vector index + 1
      SDK::GStringTable m_StrTable;
      GFile m_StrTableFile;

      //! DAL
      GDataAccessLayerServer  m_DAL;
      //! DCL
      GWorldBehavior          m_WorldBehavior;
      GDataControlLayer m_DCL;

      REAL64         m_fPreviousSyncDataUpdate;
      REAL64         m_fHalfSecondClock;
      REAL64         m_fFiveSecondsClock;
		REAL64		   m_fCombatIterationPeriod;
		REAL64		   m_fNextCombatIterationTime;
		UINT32			m_iMilitaryRankCountryToIterate;
      multimap<INT32,UINT32> m_OrderCountriesByMilitaryRank;
		multimap<INT32,UINT32>::iterator m_IteratorMilitaryRank;

		bool           m_bGameStarted;

      UINT32         m_iNuclearMissilesSubmarineCountryToReload;

      GString ConsoleServerCommandsHandler(const GString &, const vector<GString> &);

      // GGameDataNode implementation
      virtual bool OnSave(GIBuffer& io_Buffer);
      virtual bool OnLoad(GOBuffer& io_Buffer);
      virtual bool OnNew(GDatabase* in_pDatabase);
      virtual void OnClean();

      bool           m_bGameLoaded;

      struct GSaveRequest
      {
         UINT32      m_iSourceID;
         GString     m_sSaveName;
         GString     m_sSaveFile;
      };

      list<GSaveRequest> m_vPendingSaves;
      void SaveGame(const GSaveRequest& in_SaveInfo);
      void LoadGame();
      void NewGame();

      //! Inform game lobby of new player arrival/departure
      void InformPlayerJoined(SDK::GPlayer* in_pPlayer);
      void InformPlayerLeft(SDK::GPlayer* in_pPlayer);

      bool           m_bPendingLoad;
      UINT32         m_iLoadsource;
      GString        m_sLoadName;
      GString        m_sLoadFileName;

      bool           m_bPendingNew;
      UINT32         m_iNewsource;
      GString        m_sNewGameDB;
      GString        m_sCurrentDB;

      REAL64         m_fLoadClockAdjust;
      REAL64         m_fNextGameSpeed;
      REAL64         m_fLastSpeed;
		REAL32			m_fAIAggressiveness;
      bool           m_bUpdateStaticOnly;
      bool           m_bCleanVariableOnly;		

      //! HDM configuration
      void InitializeDefaultHdmConfig();
      void LoadHdmConfig();

      bool m_bActivateAllDatabaseCountries;
      bool m_bAllowAIAssumeDebt;
      bool m_bAllowHumanClientStates;
      bool m_bAllowDefenderAttackAttackerTerritory;

      GAnarchyConfig m_AnarchyConfig;

      REAL32 m_fAnnexationRelationLossPercent;
      bool   m_bAutoCovertMissions;
      
      GCivilWarConfig m_CivilWarConfig;

      REAL32 m_fCombatThresholdSquare;
      bool   m_bContinueAfterEconomicFailure;
      bool   m_bContinueAfterElectionLoss;
      bool   m_bCountryNeedsRegions;
      REAL32 m_fDedicatedServerAutosavePeriod;
      bool   m_bDedicatedServerAutosaveToJoshuaFolder;
      bool   m_bDisableNuclearOnOccupy;
      bool   m_bDisbandAMDSOnOccupy;
      REAL64 m_fTimeOfLastAutosave;
      REAL32 m_fGlobalTaxLimit;

      //! INT32 to eliminate precision issues with REAL32
      map<EGlobalTaxSpecialType::Enum, INT32> m_GlobalTaxSpecials;

	  map<EGovernmentType::Enum, REAL64> m_mGvtTypeProductionModifiers;
      map<EGovernmentType::Enum, REAL64> m_IncomeTaxLimits;

      bool    m_bIncreaseDeathRateForAgingPopulation;
      bool    m_bLogBankruptcies;
      INT32   m_iMaximumCellsInForeignCountry;
      GString m_sMessage;

      map<EUnitCategory::Enum, map<ETrainingLevel::Enum, REAL32>> m_mMilitaryUpkeepPercentages;

      bool   m_bNavalRuleEnabled;
      REAL32 m_fNuclearMissileRangePercentage;
      REAL32 m_fNuclearUpkeepPercentage;
      REAL32 m_fOccupiedRegionPercentageForNuclear;
      REAL32 m_fProductionLossOnAnnex;
      REAL32 m_fResourceTaxLimit;
      REAL32 m_fTributePercent;
      bool   m_bUseNewExportMechanics;

      //! Keep track of players who have joined, and other data
      map<INT32, UINT32> m_mPlayerData;

      //! Helper for changing the admin player; returns true if successful
      bool ChangeAdminPlayer(SDK::GPlayer* in_pPlayer);
   };

}

#define g_SP2Server ((SP2::GServer*)g_Joshua.Server())
#define g_ServerDAL ((SP2::GServer*)g_Joshua.Server())->DAL()
#define g_ServerDCL ((SP2::GServer*)g_Joshua.Server())->DCL()

#endif