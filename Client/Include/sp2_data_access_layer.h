/**************************************************************
*
* sp2_data_access_layer.h
*
* Description
* ===========
*  Hold the GDataAccessLayer class implementation
*  This class will be used to access all data we need.
*
* Owner
* =====
*  Francois Durand, Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _GOLEM_SP2_GDALCLIENT_H_
#define _GOLEM_SP2_GDALCLIENT_H_

#include "sp2_design_variations_holder.h"
#include "sp2_country_data_client.h"
#include "sp2_history_marker.h"
#include "sp2_research_information_holder.h"
#include "sp2_unit_path_manager.h"
#include "sp2_game_scenario.h"


#define DATAHOLDER_SIZE 200

namespace SP2
{
   const REAL32 c_fBombardmentMinimalExplosionTime = 2.0f;
   struct GDisplayedBombardedRegion
   {
      UINT32                     m_iRefCount;
      UINT32                     m_iRegionId;
      vector<GVector2D<REAL32> > m_vBombardmentPoints;

      vector<REAL32>             m_vDisplayedClock;

      GDisplayedBombardedRegion()
      {
         m_iRefCount        = 0;
         m_iRegionId        = 0;
      }
   };


   // Foward Declaration
   namespace Event
   {
      class GHistoryMarkers;
   }


/*
	struct SNewsInfo
   {
		GString	m_sNews;
		UINT32	m_iCountryA;
      UINT32	m_iCountryB;
		REAL64	m_fGameTime;
      UINT8		m_iType;
		UINT8		m_iPriority;
   };
*/	
	struct SGenericDataHolder
   {
      INT16  m_iRankedCountryId;
      INT16  m_iPoliticRank;
      INT16  m_iEconomicRank;
      INT16  m_iMilitaryRank;

      bool   m_bSaveConfirmationAlwaysApply;
      bool   m_bSaveConfirmationDoNotShow;
   };

   struct GDataBaseWindow
   {
      GDataBaseWindow()
      {
         Left = -1;
         Top  = -1;
         PreviouslyShown = false;
      }

      INT16  Left;
      INT16  Top;
      UINT32 PreviouslyShown;
   };

   struct GDataEconomicWindow : public GDataBaseWindow
   {    
      REAL64 Expense;    
      REAL64 Income;     
      REAL64 AvailableMoney;       
      REAL32 RessRatio; 
      REAL32 EconomicModel;   
		REAL32 EconomicHealth;  
   };

   struct GDataPoliticWindow : public GDataBaseWindow
   {
      INT32    GvtType;
      REAL32   PoliticalIdeology;
      REAL32   GvtApproval;
      REAL32   GvtStability;
      REAL32   CorruptionLevel;
		REAL32   Pressure;
      bool     MartialLaw;
   };

   struct GDataTreatiesWindow : public GDataBaseWindow
   {
      hash_map<UINT32, GTreaty> m_Treaties;
   };

   struct GDataTreatyDetailedWindow : public GDataBaseWindow
   {
   };

   struct GDataMilitaryWindow : public GDataBaseWindow
   {
      REAL64 MaxNuclearMissileDamage;
      REAL64 BudgetNuclear;

      //Missile defense system status
      REAL64 AMDSLevel;
      bool GetData;
      bool StartedAmdsResearch;
      bool StartedNuclearResearch;
   };

   struct GDataBudgetWindow : public GDataBaseWindow
   {
      INT64  Population;

      REAL64 BudgetRevenueTax;
      REAL64 BudgetRevenueTrade;
      REAL64 BudgetRevenueImf;
      REAL64 BudgetRevenueTourism;

      REAL64 BudgetExpenseInfrastructure;
      REAL64 BudgetExpensePropaganda;
      REAL64 BudgetExpenseEnvironment;
      REAL64 BudgetExpenseHealthcare;
      REAL64 BudgetExpenseEducation;
      REAL64 BudgetExpenseTelecom;
      REAL64 BudgetExpenseGovernment;
      REAL64 BudgetExpenseSecurity;
      REAL64 BudgetExpenseDiplomacy;
      REAL64 BudgetExpenseImports;
      REAL64 BudgetExpenseDebt;
      REAL64 BudgetExpenseImf;
      REAL64 BudgetExpenseResearch;
      REAL64 BudgetExpenseUnitUpkeep;
		REAL64 BudgetExpenseTourism;
      REAL64 AvailableMoney;
      
      bool   m_bChanged;

      GDataBudgetWindow()
      {
         m_bChanged = false;
      }
   };

   struct GDataInternalLaws : public GDataBaseWindow
   {
      vector<INT32>           m_viId;
      vector<INT32>           m_viStatus;
      vector<REAL32>          m_vfPerc;
      vector<bool>            m_vbChanged;
      vector<GUI::GComboBox*> m_vpCombo;
      vector<UINT8>           m_viGvtType;
      vector<GUI::GComboBox*> m_vpLawsCbo;

      REAL64                  m_fImmigration;
      REAL64                  m_fEmigration;
      bool                    m_vbLaws[EInternalLaws::ItemCount];
      INT16                   m_iGvtType;
      bool                    m_bImmigrationClosed;
      bool                    m_bEmigrationClosed;
   };
   
   struct GDataConstitutionalForm : public GDataBaseWindow
   {
      INT64 NextElection;

      INT32 CapitalId;
      INT32 GvtType;
      bool  MartialLaw;
   };

   struct GDataEconomicHealthWindow : public GDataBaseWindow
   {
      REAL64 m_fIncomeTax;
      REAL64 m_fInterestLevel;
      REAL64 m_fUnemploymentRate;
      REAL64 m_fPovertyRate;
      REAL64 m_fGdp;
      UINT64 m_iPopulation;
      REAL64 m_fInflation;
      REAL64 m_fInflationVar;
   };

   struct GDataResourcesWindow : public GDataBaseWindow
   {
      GString m_fResourceName[EResources::ItemCount];

      REAL64 m_fProduction[EResources::ItemCount];
      REAL64 m_fConsumption[EResources::ItemCount];
      REAL64 m_fTrade[EResources::ItemCount];
      REAL64 m_fPartGdp[EResources::ItemCount];
      REAL64 m_fMarketShare[EResources::ItemCount];
      REAL64 m_fMarketAvailability[EResources::ItemCount];
      REAL64 m_fDesired[EResources::ItemCount];
      REAL64 m_fActual[EResources::ItemCount];
      
      REAL32 m_fSectorTax[EResources::ItemCount];
      REAL32 m_fGlobalTaxMod;
      
      bool   m_bTrade[EResources::ItemCount];
      bool   m_bManagement[EResources::ItemCount];
      bool   m_bStatus[EResources::ItemCount];
      bool   m_bMeetConsumption[EResources::ItemCount];
      bool   m_bDirty[EResources::ItemCount];
   };

   struct GDataCountryInformationWindow : public GDataBaseWindow
   {
      INT32  m_iCountryID;
      
      REAL32 m_fArableLand;
      REAL32 m_fForestLand;
      REAL32 m_fParksLand;
      REAL32 m_fUnusableLand;

      INT64  m_iPop15;
      INT64  m_iPop1565;
      INT64  m_iPop65;

      REAL32 m_fLandArea;
      REAL32 m_fWaterArea;
      REAL32 m_fTotalArea;

      REAL32 m_fHumanDev;
      REAL32 m_fHumanDevAverage;

      INT32  m_iClimateStid;

      REAL32 m_fBirthRate;
      REAL32 m_fDeathRate;
      REAL32 m_fInfrastructure;
      REAL32 m_fTelecommunications;
   };

   struct GTreatyNotifyData
   {
      INT32 m_iTreatyID;
      bool  m_bToRemove;
   };

   namespace EQualityLevel
   {
      enum Enum
      {
         Low,
         Medium,
         High,
      };
   }

   namespace EZoomLevel
   {
      enum Enum
      {
         None,
         Elevation,
         ElevationWater,
         All,
      };
   }

   struct GDataOptionsWindow
   {
      INT16 m_iColorDepth;
      INT16 m_iResolutionX;
      INT16 m_iResolutionY;      

      REAL32 m_fMusicVolume;
      REAL32 m_fSoundVolume;
		REAL32 m_fAIAggressiveness;

      EZoomLevel::Enum m_eZoomLevel;
      EQualityLevel::Enum m_eTextureQuality;
      EQualityLevel::Enum m_eGeometricDetail;

      GString m_sPlayerName;
     
      bool m_bClouds;
      bool m_bSun;
      bool m_bMoon;
      bool m_bDayCycle;

      bool m_bGraphicModeDirty;
      bool m_bGraphicDetailsDirty;
      bool m_bSoundDirty;
      bool m_bPlayerNameDirty;
      bool m_bGeometricDetailDirty;
      
      GDataOptionsWindow()
      {
         //CGFX_Renderer_Init_Mode mode;
         //g_Joshua.Renderer()->Get_Current_Mode(&mode);
         
         //m_pData->m_iResolutionX     = mode.m_Resolution.x;
         //m_pData->m_iResolutionY     = mode.m_Resolution.y;
         //m_pData->m_iColorDepth      = mode.m_FrameBufferBitDepth;
         m_iResolutionX     = 1024;
         m_iResolutionY     = 768;
         m_iColorDepth      = 32;
         m_eTextureQuality  = EQualityLevel::High;
         m_eZoomLevel       = EZoomLevel::All;
         m_eGeometricDetail = EQualityLevel::Medium;
         m_fMusicVolume     = 1.0f;
         m_fSoundVolume     = 1.0f;
         m_bClouds          = true;
         m_bDayCycle        = true;
         m_bMoon            = true;
         m_bSun             = true;
         
         m_bGraphicModeDirty     = false;
         m_bGraphicDetailsDirty  = false;
         m_bSoundDirty           = false;
         m_bPlayerNameDirty      = false;
         m_bGeometricDetailDirty = false;
      }
   };

   namespace EOverlayType
   {
      enum Enum
      {
         Unit,
         Combat,
         Bombardment,
         City,
      };
   }

   namespace ENotifyWindowMsg
   {
      enum Enum
      {
         Advisor, 

         Economic,
         EconomicLvl1,
         EconomicHealth,
         Resources,
         Budget,

         Politic,
         PoliticLvl1,
         Treaties,
         ConstitutionalForm,
         InternalLaws,

         Military,
         MilitaryLvl1,
         UnitDesign,
         ResearchWindow,

         HistoryValuesOnly,
         HistoryMarkersOnly,
         HistoryValuesAndMarkers,

         CovertActions,
         War,
      };
   }

   struct SScreenOverlayInfo
   {
      UINT32               m_iId;
      EOverlayType::Enum   m_eType;
      GVector2D<INT16>     m_Pos;
      GVector2D<INT16>     m_Size;
   };

   struct SWindowDataDirty
   {
      bool m_bBudget;
      bool m_bEconomicHealth;
      bool m_bEconomic;
      bool m_bResources;
      bool m_bInternalLaws;
      bool m_bConstitutionalForm;

      SWindowDataDirty()
      {
         m_bBudget = false;
         m_bEconomicHealth = false;
         m_bEconomic = false;
         m_bResources = false;
         m_bInternalLaws = false;
         m_bConstitutionalForm = false;
      }
   };

   struct SDataMissionOver
   {
      UINT32											m_iCellID;
		UINT32											m_iCellSourceCountry;
		GString											m_sCellName;
		ECovertActionsMissionType::Enum			m_eType;
		ECovertActionsTargetSector::Enum			m_eTargetSector;
		ECovertActionsMissionComplexity::Enum	m_eComplexity;
		INT32												m_iResourceOrUnitType;
		UINT32											m_iTargetCountry;
		UINT32											m_iFramedCountry;
		bool												m_bSuccessful;
		bool												m_bCaptured;
   };


   class GDataAccessLayerClient : public GDALInterface, DesignPattern::GObserver, public DesignPattern::GSubject
   {

      friend class GDataControlLayer;
   public:      
      GDataAccessLayerClient();
      ~GDataAccessLayerClient();

      GString MilitaryStatusToString(EMilitaryStatus::Enum in_eStatus, const SP2::GUnit* in_pUnit);

      vector<GArenaInfo*> ArenaInfos(UINT32 in_iCountryID) const;

      bool Initialize(const GString& in_sXMLFilename);
      bool Shutdown();

      SGenericDataHolder & GenericDataHolder();

      bool DialogDefinitionFile(GFile& out_File);

      const hash_map<UINT32,GUnitInTraining> & UnitInTraining(){return m_UnitInTraining;}
      
      //Old stuffz
      bool FetchCountryInfo(INT32 in_iCountryID);
      bool FetchInternalLawsInfo(INT32 in_iCountryID);

      bool FetchEconomicWindowInfo(INT32 in_iCountryID);
      bool FetchEconomicHealthWindowInfo(INT32 in_iCountryID);
      bool FetchPoliticWindowInfo(INT32 in_iCountryID);
      bool FetchConstitutionalFormInfo(INT32 in_iCountryID);
      bool FetchCountryRanks(INT32 in_iCountryID);
      bool FetchResourcesInfo(INT32 in_iCountryID);

		bool FetchCountriesRelations();

      GString FindTreatyName(ETreatyType::Enum in_eType);

		void InsertPendingNews(const News::GInfo& in_News);
		const News::GInfo* GetNextNews() const;
		void IterateNews();

      //void FetchMissileList();

      //! Return the country info with the specified name
      const GCountry& Country(GString in_sName) const;
      const GCountry& CountryFromCode(GString in_sCode) const;

      //! Return the country info with the specified ID
      const GCountry& Country(INT32 in_iID) const;

      INT32 CountryID(GString in_sCountryCode);

      INT16 ControlledCountryID();
      void ControlledCountryID(INT16 in_iControlledCountryID);

      vector<GCountry> & Countries();

      GString CountryFlagSmallPath(UINT32 in_iID);

      bool RegionsCharacteristic(ECharacteristicToHighlight::Enum in_Charact, INT32 in_iCountryID);

      //Set / Get the combat that is displayed in the overall window
      void DisplayedOverallCombat(const UINT32 in_iArenaID);
      UINT32 DisplayedOverallCombat() const;

      void DisplayedInfoCombat(const UINT32 in_iArenaID);
      UINT32 DisplayedInfoCombat() const;

      vector<SScreenOverlayInfo> & VisibleUnits();
      vector<SScreenOverlayInfo> & VisibleCombats();
      vector<SScreenOverlayInfo> & VisibleCities();

      void SelectUnit(UINT32 in_iID, bool = false);
      void UnselectUnit(UINT32 in_iID);
      void UnselectUnits();
      const set<UINT32> & SelectedUnitsID() const;

      list<UINT32> & UnitsForSale(void);
      bool IsUnitForSale(UINT32 in_iUnitId);

      void SelectCombat(UINT32 in_iID);
      UINT32 SelectedCombatID() const;

		//! Gives the relation of the client player with that other country
		REAL32 RelationWithCountry(UINT32 in_iCountryID);

      void RequestChangeSelectedCountry(INT16 in_iID);

      void ReceiveCountryStaticData(SP2::Event::GReceiveCountryStaticData* in_pData);
      void ReceiveInitialData(Event::GSendData* in_pEvent);
		void SetRelations(SP2::Event::GEventFetchCountriesRelations* in_pEvent);

      //! Change the military control of the specified region
      void ChangeMilitaryControl(UINT32 in_iRegionID, UINT32 in_iNewControlID);

      //! Change the political control
      void ChangePoliticalControl(UINT32 in_iRegionID, UINT32 in_iNewControlID);

      GResearchInformationHolder m_ResearchInfo;

      const hash_map<UINT32, GLaunchedNuke>& LaunchedNukes() const;
      bool RemoveLaunchedNuke(UINT32 in_iNukeID);
      bool AddLaunchedNuke(const GLaunchedNuke& in_NukeInfo);
      bool LoadUnitDesign();

      //Notify client that treaties has changed
      void NotifyTreatyChanged(const GTreatyNotifyData & in_Data);
      void NotifyWarChanged(UINT32 in_iWarID);

      void NotifyMilitaryControlChanged();

      GUnitPathManager& PathManager() { return m_PathManager; }

      GDesignVariationHolder DesignVariationHolder;

      GDataMilitaryWindow DataMilitaryWindow;

      GDataEconomicWindow DataEconomicWindow;
      GDataEconomicHealthWindow DataEconomicHealthWindow;
      GDataBudgetWindow   DataBudgetWindow;
      GDataResourcesWindow DataResourcesWindow;
      GDataPoliticWindow  DataPoliticWindow;
      GDataConstitutionalForm DataConstitutionalForm;
      GDataInternalLaws   DataInternalLawsWindow[3];
      GDataTreatiesWindow m_DataTreatiesWindow;
      GDataTreatyDetailedWindow m_DataTreatyDetailsWindow;
      GDataCountryInformationWindow m_DataCountryInformationWindow;
      GDataOptionsWindow m_DataOptionsWindow;

      GCountryDataClient m_PlayerCountryData;
      map<INT32, SDataMissionOver> m_DataMissionOver;

      SWindowDataDirty  m_WindowDataDirty;

      //! Get Region name
      GString RegionName(UINT32 in_RegionID);

      //! Gets the region name string id
      UINT32 RegionNameStringId(UINT32 in_iRegionID);

      //! Return the name of a city.
      /*!
       * @param in_iCityID    : ID of the city we want the name.
       * @return String containing the name of the city.
       **/
      const GString& CityName(UINT32 in_iCityID) const;

      //! Update the information about a city.
      /*!
       * @param in_ModifiedCity  : Info about the city modifications.
       **/
      void UpdateCity(const SCityInfo& in_ModifiedCity);

      //! Update the information about a region annex.
      /*!
       * @param in_ModifiedAnnex : Info about the annex modifications.
       **/
      void UpdateRegionAnnex(const GRegionAnnex& in_ModifiedAnnex);

      //! Remove a region annex.
      /*!
       * @param in_iRegionID     : ID of the region that stopped annexing.
       **/
      void RemoveRegionAnnex(UINT32 in_iRegionID);

      //! Destroy a country
      /*!
       * @param   in_iCountryID  : ID of the country to destroy
       **/
      void DestroyCountryEntity(UINT32 in_iCountryID);

      //! Add a treaty
      /*!
       * @param   in_Treaty      : Treaty to add
       **/
      void AddTreaty(const GTreaty& in_Treaty);

      //! Remove a treaty
      /*!
       * @param   in_iTreatyID   : ID of the treaty to remove
       **/
      void RemoveTreaty(UINT32 in_iTreatyID);

		void AddWar(const GWar& in_War);

		void RemoveWar(UINT32 in_iWarID);

      //! Add this war to the current war list
      /*!
       * @param   in_NewWar      : New war
       **/
      void AddWarPair(const SWarPair& in_NewWar);

      //! Remove this war to the current war list
      /*!
       * @param   in_NewPeace    : New peace (old war)
       **/
      void RemoveWarPair(const SWarPair& in_NewPeace);

      //! Update diplomatic relations following a data update
      void UpdateWarStatus();

      bool Cleanup();

      void UpdatePlayerCountryData(GCountryDataItf * in_CtryData);

      //! Fetch the history values of the specified variable
      const vector<REAL32>& HistoryValues(EHistoryValues::Enum in_eValueType) const;

      //! Fetch the history markers of the specified variable
      const vector<GHistoryMarker>& HistoryMarkers(EHistoryValues::Enum in_eValueType) const;

      //! Retrieve the value to add to the markers to access the corresponding values
      INT32 HistoryMarkerOffset(EHistoryValues::Enum in_eValueType) const;

      //! Begin saving historical graph values
      void BeginHistoryValuesIterations();

      //! Stop saving historical graph values
      void StopHistoryValuesIterations();

      //! Update the content of history graph values
      void IterateHistoryValues();

      //! Update the markers using the received game event
      void HistoryMarkersUpdate(SP2::Event::GHistoryMarkers* in_pEvent);

      //! Convert a marker type & value into text
      GString HistoryMarkerToText(UINT32 in_iType, REAL32 in_fValue);

		//! Request the political control thematic map (locally handled)
      void ThematicMapRequestPoliticalControl(UINT32 in_iCountryID);

      //! Request the country empire thematic map (locally handled)
      void ThematicMapRequestCountryEmpire(UINT32 in_iCountryID);

      //! Request the human controlled countries thematic map (locally handled)
      void ThematicMapRequestHumanControlledCountries();

      void ReadScenarios(GString in_FileName);
      void CleanScenarios(void);
      void Scenario(GSmartPtr<GGameScenario> in_pScenario);
      GSmartPtr<GGameScenario> Scenario(void);

      vector<GSmartPtr<GGameScenario> >& Scenarios(void);

      GString StringReplaceNewLineChar(GString in_sText);

      //Apply game options
      void ApplyOptions();

      void SendAdvisorSettings(EAdvisorStatus::Enum in_eEconomic, EAdvisorStatus::Enum in_ePolitic, EAdvisorStatus::Enum in_eMilitary, 
                               SP2::GRequestObject* in_pRequestObj = NULL);
      EAdvisorStatus::Enum m_eAdvisorStatusEconomic;
      EAdvisorStatus::Enum m_eAdvisorStatusPolitic;
      EAdvisorStatus::Enum m_eAdvisorStatusMilitary;

      hash_map<UINT32,GUnitInTraining>       m_UnitInTraining;
      
      hash_map<UINT32, INT32> m_TreatiesMail;
      hash_map<UINT32, INT32> m_CellsIdReady;

      //! our trades mails array
      hash_map<INT32, SP2::GTradeData> m_TradesMail;		
      //! Add a new Trade Mail and returns its mail id
      INT32 AddTradeMail(const SP2::GTradeData &in_Trade);

		//! Trade Refusal hash map
		hash_map<INT32, SP2::GTreatyRefusalDetails> m_TreatyRefusalMail;		
		//! Add a new Treaty Refusal Mail and returns its mail id
		INT32 AddTreatyRefusalMail(const SP2::GTreatyRefusalDetails & in_TreatyDetails);
		void RemoveTreatyRefusalMail(INT32 in_MailID);

      //! Remove a Trade Mail
      void RemoveTradeMail(INT32 in_MailID); 
      //! Do we have a trade mail pending with this id ?
      bool GotTradeMailWithId(INT32 in_MailID);

      //! our combat overs mails array
      hash_map<INT32, SP2::GArenaInfo> m_CombatsOverMail;
      //! Add add new combat over mail and returns its mail id
      INT32 AddCombatOverMail(const SP2::GArenaInfo &in_ArenaInfo);
      //! Remove a combat over Mail
      void RemoveCombatOverMail(INT32 in_MailID);
      //! Do we have a combat over mail pending with this id ? 
      bool GotCombatOverMailWithId(INT32 in_MailID);

      const GString& PlayerName() const { return m_sPlayerName; }

      void PlayerName(const GString& in_sNewPlayerName);
      void CountryActivated(INT32 in_iCountryID, bool in_bActivated);

      
      // This is used for music adaptation
      UINT32 m_iTotalMarkerReceivedSoFar;     


      hash_map<UINT32,GDisplayedBombardedRegion>& BombardedRegions();

      bool GameIsOver(void);
      void GameIsOver(bool in_bIsGameOver);

   private:
		vector<REAL32>		m_vRelations;
      vector<GCountry>  m_Countries;
      bool              m_bGameIsOver;

      //Apply game options
      void ApplyOptionsGraphicDetails();
      void ApplyOptionsSound();
      void ApplyOptionsGraphicMode();
      void ApplyOptionsGeometricDetail();
      void ApplyOptionsPlayer();


      HANDLE   m_iRecvCountryDataThreadHandle;

      DB::GTableHolder<DATAHOLDER_SIZE> m_DataHolder;

      vector<UINT32> m_vRegionNames;

      INT16 m_iControlledCountryID;
      SGenericDataHolder m_GenericDataHolder;

      UINT32 m_iDisplayedOverallCombatID;
      UINT32 m_iDisplayedInfoCombatID;

      vector<GSmartPtr<GGameScenario> >  m_vScenarios;
      GSmartPtr<GGameScenario>           m_pScenario;

      //! Unit fast identification & localisation data
      vector<SScreenOverlayInfo> m_VisibleUnits;
      vector<SScreenOverlayInfo> m_VisibleCombats;
      vector<SScreenOverlayInfo> m_VisibleCities;
      set<UINT32>    m_SelectedUnitsID;
      UINT32         m_iSelectedCombatID;

      //vector<GNuclearMissile> m_vMissileList;

      vector<GString> m_vCityNames;

      // Launched nuke data
      hash_map<UINT32, GLaunchedNuke>  m_LaunchedNukes;

      GUnitPathManager     m_PathManager;

		vector<News::GInfo>		m_vPendingNews;

      //! Notification when your subject change
      void OnSubjectChange(DesignPattern::GSubject &in_pSubject, const DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );

      //! Validate and change the selection according to a change in the unit manager
      void ValidateAndUpdateSelection(void);
      void CheckCellsReadyForMission();

      //! Read all the objectives for a scnearios
      void ReadObjectives(CTreeXMLNode* in_pScenarioObjectivesNode,GGameScenario *in_pScenario);

      // History graph values & markers
      vector<REAL32>          m_pHistoryGraphValues[EHistoryValues::TotalCount];
      vector<GHistoryMarker>  m_pHistoryGraphMarkers[EHistoryValues::TotalCount];
      UINT32                  m_iHistoryMarkerTimeOffset;
      REAL32                  m_fLastHistoryValuesUpdate;

      list<UINT32>            m_ListOfUnitsForSale;

      bool                    m_bBeginHistoryIterations;
      bool                    m_bHistoryIterationsIsRunning;

      GString                 m_sPlayerName;


      hash_map<UINT32,GDisplayedBombardedRegion> m_BombardedRegions;


   };

}

#endif //_GOLEM_SP2_GDALCLIENT_H_