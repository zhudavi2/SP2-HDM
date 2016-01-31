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
#ifndef _GOLEM_SP2_GDALSERVER_H_
#define _GOLEM_SP2_GDALSERVER_H_

#include "sp2_country_data_holder.h"

//Forward declarations
class GXMLNode;


namespace SP2
{
   class GDCLInterface;

   /*!
   * Data access layer
   **/
   class GDataAccessLayerServer : public GDALInterface, public SDK::GGameDataNode
   {
      friend class GDataControlLayer;
      friend class GServer;

   public:
      GDataAccessLayerServer();
      ~GDataAccessLayerServer();

      //! Initializes the Data Access Layer
      GDatabase* LoadDatabase(const GString &in_sDBFilename);

      //! Initialize
      bool Initialize(const GString& in_sConfigFile);

      //! Shutdown
      bool Shutdown();

		REAL64 RegionValue(UINT32 in_iRegionID) const;

      //! Gets a reference to the dialog definition file
      bool DialogDefinitionFile(GFile& out_File);

      EError DBGetCountryList(GTable &out_RSet);

      set<INT32> GetAffectedCountries(const GVector2D<REAL32> & in_Center, REAL32 radius);

      // returns a set of pairs<RegionID, CityID> containing each city included in the circle <in_Center, radius>
      //set<pair<INT32,INT32> > GetAffectedRegionsAndCities(const GVector2D<REAL32> & in_Center, REAL32 radius);      

      //! Delete all the active units
      void ActiveUnitsClear();		

      void CalculateCountryInfo();

		bool LoadRelationTable();//!<Load the relation table of the game. 

      //! Returns a premade design name for a unit type
		GString DesignName(UINT32 in_iCountryID, EUnitType::Enum in_eUnitType);

      //! Get the country Id associated with that region
      UINT32 CountryIDFromRegionID(const UINT32 in_iRegionID);

		REAL64 ResourceProductionModifier(UINT32 in_iCountryID, EGovernmentType::Enum in_Type) const;

      //! Return the relation between the source and the target. It tells how the sources likes/dislikes the target.
      REAL32 RelationBetweenCountries(ENTITY_ID in_iCountryIDSource, 
                                      ENTITY_ID in_iCountryIDTarget);

		//! Return the expected relation between the source and the target. It tells how the sources should like/dislike the target.
      REAL32 RelationBetweenCountriesExpected(ENTITY_ID in_iCountryIDSource, 
															 ENTITY_ID in_iCountryIDTarget);

      //! Sets the relation between the source and the target. It tells how the sources likes/dislikes the target.
      bool RelationBetweenCountries(ENTITY_ID in_iCountryIDSource, 
                                    ENTITY_ID in_iCountryIDTarget, 
                                    REAL32 in_fRelation);

		GString RegionDescription(INT32 in_iRegionID);

      //!Return a pointer to the Country data for a country ID
      inline GCountryData* CountryData(ENTITY_ID in_iCountryID) const
      {
	         return (GCountryData*)&m_pCountryData[in_iCountryID];
      };


		//!Return a pointer to the Region for a region ID
      GRegion* GetGRegion(UINT32 in_iRegionID) const;

      //!Prints the relation table for this country towards every other country
      void PrintCountryRelations(ENTITY_ID in_iCountryID);				

		//!Calculate the expected relations between countries
		void CalculateExpectedRelations();

		//! Initialize the imports / exports of all countries to 0. Useful before doing a new Global Market iteration
		void InitCountriesExportsImports();

		//! Add a treaty to the treaty vector
		bool AddTreaty(GTreaty& in_NewTreaty);

		//! Add a war
		bool AddWar(GWar& in_NewWar);

		//! Remove a war
		void RemoveWar(UINT32 in_iWarID);

      //! Flag a treaty as modified
      void ModifyTreaty(UINT32 in_iTreatyID);

		//! Tell the client a war has been modified
		void ModifyWar(UINT32 in_iWarID);

		//! Removes a treaty
		void RemoveTreaty(UINT32 in_iTreatyID);

		//! Return a pointer to a GTreaty
		GTreaty* Treaty(UINT32 in_iTreatyID);

		//! Return a pointer to a GWar
		GWar* War(UINT32 in_iWarID);

		//! Put two countries hostile between each other once a war declaration is made. Should be called by DeclareWar() only. 
		void AddWarStatus(ENTITY_ID in_iFirstCountryID, ENTITY_ID in_iSecondCountryID);

		//! Put two countries neutral between each other once a peace declaration is made. Should be called by DeclarePeace() only. 
		void RemoveWarStatus(ENTITY_ID in_iFirstCountryID, ENTITY_ID in_iSecondCountryID);

		//! Returns the string table ID for a specific region.
      INT32 StringIdRegion(INT32 in_iRegionID);

      //! Change the military control of the specified region
      /*!
       * @param   in_iRegionID      : ID of the region that will change control
       * @param   in_iNewControlID  : ID of the new controler for the region
       * @return Boolean indicating success of operation
       **/
      bool ChangeMilitaryControl(UINT32 in_iRegionID, UINT32 in_iNewControlID);

      //! Change the political control
      /*!
       * @param   in_iRegionID      : ID of the region that will change control
       * @param   in_iNewControlID  : ID of the new controler for the region
       * @return Boolean indicating success of operation
       **/
      bool ChangePoliticalControl(UINT32 in_iRegionID, UINT32 in_iNewControlID);

      //! Annex a region to a country
      /*!
       * @param   in_iCountryID              : ID of the country annexing the region
       * @param   in_iRegionID               : ID of the region that is annexed
       * @param   in_fDaysBeforeCompletion   : Time it will take to annex the region
       **/
      void AddAnnexToRegion(UINT32 in_iCountryID, UINT32 in_iRegionID, REAL64 in_fDaysBeforeCompletion);

      //! Cancel a region annexion
      /*!
       * @param   in_iRegionID               : ID of the region to cancel annexion
       **/
      void RemoveAnnexOfRegion(UINT32 in_iRegionID);

      //! Destory a country
      /*!
       * @param   in_iCountryID              : ID of the country to destroy
       * @param   in_iConquerorID            : ID of the country that conquered the country
       **/
      void DestroyCountryEntity(UINT32 in_iCountryID, UINT32 in_iConquerorID);

		//! Returns true if a country is still valid. 
		bool CountryIsValid(UINT32 in_iCountryID) const;

      void UpdateCapitalID(UINT32 in_iCountryID, UINT32 in_iCapitalID);

      //! Load the server config file gameplay constants
      bool LoadServerConfigGameplayConstants();

      bool UpdateCitiesInfo();
      bool UpdateCapitalsInfo();

		//! Remove population from a city. Will automatically adjust the region population.
		UINT32 RemovePopulationFromCity(UINT32 in_iCasualties, UINT32 in_iCityID, UINT32 in_iRegionID);

      void FillReceiveCountryStaticData(SP2::Event::GReceiveCountryStaticData* in_pData);

      //! Fill the event containing first data set sent to players
      void FillInitialData(SP2::Event::GSendData* in_pSendEvent,UINT32 in_iPlayerModId);

      //! Reset the content of the iteration data update
      void ResetUpdateData();

      //! Update all human players with data changes
      void SendUpdateData();

      //! Reset the content of the unit value update
      void ResetUpdateUnitValue();

      //! Update unit value of clients
      void SendUpdateUnitValue();

		//! Return the market Production for a specified resource
		REAL64 MarketProduction(EResources::Enum in_iResource) const;
		const REAL64* MarketProduction() const;
		void   MarketProduction(EResources::Enum in_iResource, REAL64 in_fAmount);

		//! Return the market Demand for a specified resource
		REAL64 MarketDemand(EResources::Enum in_iResource) const;
		const REAL64* MarketDemand() const;
		void   MarketDemand(EResources::Enum in_iResource, REAL64 in_fAmount);

		//! Re-Calculate the world production and demand
		void CalculateWorldProductionDemand();

		//! Returns true if the two countries are inside the same country group
		bool InsideTheSameCountryGroup(UINT32 in_iCountryA, UINT32 in_iCountryB) const;

		//! Will update human development bonus. Should be called only when editing, removing, or adding a Human Development Collaboration treaty
		void UpdateHumanDevelopmentBonus();

		//! Will update Economic Partnership bonus. Should be called only when editing, removing, or adding a Economic Partnership treaty
		void UpdateEconomicPartnershipBonus();

		//! Returns the yearly help, in %, that a country receives from its Human Development Collaboration treaties
		REAL32 HumanDevelopmentBonus(UINT32 in_iCountryID);

		//! Returns the yearly bonus, in %, that a country receives from its Economic Partnership treaties
		const REAL64* CountryEconomicPartnership(UINT32 in_iCountryID) const;

      //! Access the player objective list
      hash_map<UINT32,vector<GGameObjective> >& PlayersObjectives(){return m_PlayerObjectives;}
      //! Sets the player objective list
      void                                      PlayersObjectives(SDK::GPlayer* in_pPlayer,const vector<GGameObjective>& in_vObjectives);

      void                          GameObjectives(const vector<GGameObjective>& in_vObjectives);
      const vector<GGameObjective>& GameObjectives() const;


      // Foreign country that can build units for other country
      list<UINT16>& BuildingCountryList(void);

      //! Creates the game objectives for the player
      vector<GGameObjective> AlwaysPresentObjectives();

      //! Add a human player
      void AddHumanPlayer(SDK::GPlayer* in_pPlayer);

      //! Remove a human player
      void RemoveHumanPlayer(SDK::GPlayer* in_pPlayer);

      //! Rember action in an historical marker for the player
      void AddHistoricalMarker(UINT32 in_iModID, 
                               EHistoryMarkerType::Enum in_eType,
                               REAL32 in_fValue);

      //! Sends all remembered actions since begin of iteration
      void SendHistory();

		//! Find the population level, starting from 1, for a country
		REAL32 PopulationLevel(UINT32 in_iCountryID) const;

		//! Return the total value (in $) of missiles for a specific country
		REAL32 TotalMissileValue(UINT32 in_iCountryID) const;

		//! Overall relations of a country. Between -100 and 100
		REAL32 OverallRelations(UINT32 in_iCountryID) const;

		//! Gives the actual research level of a country
		UINT8 CountryResearchStatus(UINT32 in_iCountryID, 
											 EUnitCategory::Enum in_UnitCategory, 
											 EUnitDesignCharacteristics::Enum in_ResearchCategory) const;

		//! Updates the accuracy of information for that country
		void UpdateAccuracyOfInformation(UINT32 in_iCountryID);

      //! Updates the relation history
      void UpdateRelationsHistory();

      //! Update the building countries for human players
      void UpdateBuildingCountries();

      //! check world peace status
      void CheckWorldPeace();

      void RegisterForRelationsHistoryFetch(UINT32 in_iPlayerID,UINT32 in_iCountry1,UINT32 in_iCountry2);
      void UnregisterForRelationsHistoryFetch(UINT32 in_iPlayerID);

      //! Fetch the relation history
      GSymmetricMatrix<REAL32>* RelationHistory();

		//! Synchronize the treaties
		void SyncronizeTreaties();

		void AddFutureGameEventCreateNewTreaty(UINT32 in_iCountryID, UINT32 in_iTreatyID);

      void DirtyCountryUnitsServer(UINT32 in_iCountryID);
      
      UINT32* MilitaryResearchTimeInDays() const;
      REAL64 MilitaryResearchStdBudget() const;

		void IterateRegionSynchronization();
		void AddCountryToSynchronize(UINT32 in_iCountryID);

		//Methods for history of first nuked launched
		void		GDataAccessLayerServer::LastCountryThatNuked(UINT32 in_iCountryID);
		UINT32	LastCountryThatNuked();
		void		DateOfLastNuke(const GDateTime& in_Date);
		const		GDateTime& DateOfLastNuke();

      void UpdateCountryUnitAndMissilesValues(void);

		//! Access the treaty history list
      hash_map<UINT32, hash_map<UINT32,ETreatyRefusal::Enum> >& TreatyRefusalHistory(){return m_DecisionHistory;}

      const GDateTime& ConfigZeroDate() const
      {
         return m_ConfigZeroDate;
      }

      //! Checks if country can assign more cells to a target country, based on server config
      bool CountryCanAssignCovertCellToTarget(UINT32 in_iSource, UINT32 in_iTarget);

   private:
      //-----------------------------------------------
      //  Military data loading functions
      //-----------------------------------------------
      //! Loads all military data for the game
      bool LoadMilitaryData();

      bool LoadUnitDesigns();//!<Load the unit designs of the game
      bool LoadUnits(); //!<Load the units of the game.
      //-----------------------------------------------
      //-----------------------------------------------
      // Politic data loading functions
      //-----------------------------------------------
      //! Loads all politic data for the game
      bool LoadPoliticData();

      bool LoadTreaties(); //!<Load the Treaties of the game            
      bool LoadCountryData();//!<Load the Country Data of the game.		
		bool LoadRegions(); //!<Load the Regions of the game.
      //-----------------------------------------------		

		//Utility function to print info on a country
		void DebugPrintCountryInfo(UINT32 in_iCountryID);		
      
		//Country information
		GCountryData* m_pCountryData;

      //-- Relations and history relation values --
      //GSymmetricMatrix<REAL32> m_RelationTable;
      GSymmetricMatrix<REAL32> m_pRelationHistory[c_iRelationHistoryNumberOfValuesToKeep];
      REAL64                   m_fNextRelationHistoryUpdateGameTime;
      //! Player ID that wishes to receive an update of the relations, between the 2 countries identified by the pair
      hash_map<UINT32, pair<UINT32,UINT32> > m_RelationHistoryPlayerWishUpdates;
      //   --  end relations and history relation values --
      REAL32* m_pRelationTable;
		REAL32* m_pRelationTableExpected;
		REAL32* m_pRelationTableModifier;
		REAL32* m_pHumanDevelopmentBonus;
		REAL64* m_pEconomicPartnershipBonus;
		

      //! Last AMDS research update clock
      REAL64 m_fAMDSResearchLastCheckClock;
      //! Long Range Interceptions Success chances
      REAL32 m_fAMDSLongRangeInterceptionSuccess;
      //! Short Range Interceptions Success chances
      REAL32 m_fAMDSShortRangeInterceptionSuccess;
      //! Time for a missile to traverse half of the earth, in gameplay time
      REAL32 m_fNuclearMissileSpeed;
     
		UINT32				m_iNextTreatyID;		
		UINT32				m_iNextWarID;

		//!Set of treaties id to synchronize at the next iteration
		set<UINT32>			m_TreatiesToSynchronize;
		//!Set of treaties id to synchronize, treaties that are deleted
		set<UINT32>			m_TreatiesToDelete;

		//!Set of countries to synchronize with regions
		set<UINT32>			m_CountriesToSynchronize;

		//!Vector of pair representing the events needing to be sent, first is the country ID, second is treaty ID
		list<pair<UINT32,UINT32> >	m_vCreateNewTreatyEvents;		

      // List of country that can build units for other people 
      list<UINT16>      m_BuildingCountryList;

      //! Launched nukes, key is the time they hit their target, amds stopped nukes are not in that list
      multimap<REAL64,GLaunchedNuke> m_LaunchedNukes;		

		// Regions info
		GRegion* m_Regions;	//array of region pointers

      SDK::GGameEventSPtr        m_pUpdateEventSmartPtr;
      SP2::Event::GUpdateData*   m_pUpdateEvent;

      SDK::GGameEventSPtr        m_pUpdateUnitEventSmartPtr;
      SP2::Event::GUpdateUnitValue* m_pUpdateUnitEvent;

      // Historical markers (actions), per human player
      hash_map<UINT32, SDK::GGameEventSPtr>  m_HistoricMarkersEvents;

      //! vecotr of possible world peace treaties id
      vector<UINT32> m_vPossibleWorldPeaceTreatiesID;

		//World market availability
		REAL64			m_pWorldAvailability[EResources::ItemCount];   //Array of resources	availability
		REAL64			m_pWorldProduction[EResources::ItemCount];		//Array of world production
		REAL64			m_pWorldDemand[EResources::ItemCount];			//Array of world demand

      //List of objectives for the players, key: Human player id
      hash_map<UINT32,vector<GGameObjective> > m_PlayerObjectives;
      //! General list of objectives, unapplied to anybody
      vector<GGameObjective>                   m_vGameObjectives;

		//! Hash Map of treaty decision. The first key is the treaty id, the second key is the country id
		hash_map<UINT32, hash_map<UINT32,ETreatyRefusal::Enum> > m_DecisionHistory;

		UINT32		m_iFirstCountryThatNuked;
		GDateTime	m_DateOfFirstNuke;
      GDateTime   m_ConfigZeroDate;

      // GGameDataNode implementation
      virtual bool OnSave(GIBuffer& io_Buffer);
      virtual bool OnLoad(GOBuffer& io_Buffer);
      virtual bool OnNew(GDatabase* in_pDatabase);
      virtual void OnClean();
   };
}

#endif //_GOLEM_SP2_GDALSERVER_H_