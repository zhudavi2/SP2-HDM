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

namespace SP2
{
   class GDataControlLayer :public GDCLInterface, public Hector::DesignPattern::GSubject , public SDK::GGameDataNode
   {
		friend class GEconomicEventHandler;
   private:
      GUnitMover              m_UnitMover;
      GUnitProductor          m_UnitProductor;
      GGameObjectiveEvaluator m_GameObjectiveEvaluator;

      // Used by the news, to avoid sending nuke news every 2 seconds
		set<UINT32>									m_vNewsNukesAttacks;

		hash_map<UINT32,INT64>		  m_NumberOfDeadByCountry;
      hash_set<UINT32>             m_UnitListForSell;

      // For nuclear attack
      bool                       m_bNuclearVulnerabilityByStatus[EMilitaryStatus::CountTotal];

		hash_map<UINT32,REAL32>		m_TotalEnemiesByCountries;

		void LogNewAction(GString in_sNewAction);

		//! It will verify if that war should be ended or not. Return true if treaty needs to be removed
		bool VerifyPeaceStatus(const GWar* in_pWar, const set<UINT32>& in_WarsToRemove);		

		//! Adding a country here will tell the world behavior to print out news of a nuke launched
		void AddNewsNukeLaunched(ENTITY_ID in_iCountry);

		//! Adding casualties will tell the world behavior to print out news
		void AddCasualtiesToCountry(UINT32 in_iCountryID, UINT32 in_iCasualties);		

		//The confirm functions are to make sure the news is sent to the news ticker, and that World Behavior changes are made
		void ConfirmChangeCapitalID(UINT32 in_iCountry,UINT32 in_iCapital);
		bool ConfirmChangeMilitaryControl(UINT32 in_iRegionID, UINT32 in_iNewControlID);
		bool ConfirmChangePoliticalControl(UINT32 in_iRegionID, UINT32 in_iNewControlID);		

		//! Peace declaration by in_iFirstCountry to in_iSecondCountry. 
		bool DeclarePeace(ENTITY_ID in_iFirstCountry, ENTITY_ID in_iSecondCountry, bool in_bPrintNews);

		//! War declaration by in_iAttackingCountry to in_iDefendingCountry
		bool DeclareWar(ENTITY_ID in_iAttackingCountry, ENTITY_ID in_iDefendingCountry, const set<UINT32>& in_AttackingCountries, bool in_bOffensiveWar, bool in_bLossOfRelations);

		//! Execute the immediate effects of a treaty. It will delete ponctual treaties once they are done.
		void ExecuteTreaty(UINT32 in_iTreatyID);

		//! Execute the loss of relations for an annexation
		void LoseRelationsAnnexation(UINT32 in_iCountryAnnexing, UINT32 in_iCountryAnnexed);

		//! Execute the loss of relations for a war declaration
		void LoseRelationsWarDeclaration(UINT32 in_iCountryDeclaringWar, UINT32 in_iCountryBeingDeclaredWarOn, const set<UINT32>& in_CountriesAttackingWithAttacker);

		//! Find national security level of a country
		REAL32 FindNationalSecurity(
			UINT32 in_iNbCellsInDefense,
			REAL32 in_fPopulationLevel,
			REAL32 in_fAverageTrainingCell) const;

		void ExecuteInternalLawWomenSuffrage(UINT32 in_iCountryID,bool in_bStatus);
		void ExecuteInternalLawSameSexMarriage(UINT32 in_iCountryID,bool in_bStatus);
		void ExecuteInternalLawPolygamy(UINT32 in_iCountryID,bool in_bStatus);
		void ExecuteInternalLawNumberOfChildren(UINT32 in_iCountryID,bool in_bStatus);
		void ExecuteInternalLawFreedomOfSpeech(UINT32 in_iCountryID,bool in_bStatus);
		void ExecuteInternalLawFreedomOfDemonstration(UINT32 in_iCountryID,bool in_bStatus);
		void ExecuteInternalLawContraception(UINT32 in_iCountryID,bool in_bStatus);
		void ExecuteInternalLawChildLabor(UINT32 in_iCountryID,bool in_bStatus);
		void ExecuteInternalLawAbortion(UINT32 in_iCountryID,bool in_bStatus);
		//! Remove relations with other countries based on a change from internal laws
		void RemoveRelationsFromInternalLaws(UINT32 in_iCountryID, bool in_bStatus, EInternalLaws::Enum in_Law);

   protected:
      //! Conquer a country if conditions are met when a region changes military control.
      /*!
       * @param in_PreviousControl  : Previous control of the region
       * @param in_pRegion          : Current info of the region
       **/
      void ConquerCountry(GRegionControl& in_PreviousControl, GRegion* in_pRegion);

      /*!
       * \brief Change the political control of a region.
       *
       * @param in_iRegionID  : Id of the region to change the political control
       * @param in_NewControl : New controler for the region
       **/
      bool ChangeRegionPoliticalControl(const UINT32 in_iRegionID,
                                        const UINT32 in_NewControl,
													 bool in_bForceChange = false);

      /*!
       * \brief Changes the capital of a country if necessary
       *
       * @param in_iRegionID  : Id of the region that has just been conquered (militaryly or politicaly)
       **/
      void ChangeCountryCapital(const UINT32 in_iRegionID);

      //! Cancel the annexion of a region
      void CancelRegionAnnex(UINT32 in_iRegionID);		

		//! Returns a random resource
		EResources::Enum RandomResource(UINT32 in_iSeed) const;

      //! Launch a single nuclear attack, returns the launched nuke
      bool LaunchNuclearAttack(const GNukeLaunchRequest&,GLaunchedNuke& out_LaunchedNuke);

   public:
      GDataControlLayer(GDALInterface* in_pDAL);
      ~GDataControlLayer();

      void Initialize();
      void Shutdown();

		void TradeAccept(GTradeData& in_TradeToAccept);
		void TradeRefuse(GTradeData& in_TradeToRefuse);
		void TradeProposal(GTradeData& in_Trade);

		void ClearUnitProductionForCountry(UINT32 in_iCountryID);

		void LiberateRegions(UINT32 in_iCountryLiberating,
									UINT32 in_iCountryLiberated);

		REAL32 PressureStrengthTreaty(const GTreaty* in_pTreaty, UINT32 in_iCountryID) const;

		REAL32 FindCovertOpsSuccessRate(ECovertActionsMissionComplexity::Enum in_eComplexity,
												  ECovertActionCellTraining::Enum in_eTraining,
												  ECovertActionsMissionType::Enum in_eMissionType,
												  ECovertActionsTargetSector::Enum in_eTargetSector,
												  bool in_bFraming,
												  UINT32 in_iTarget,
												  INT32 in_iSpecificSector = 0,
                                                  REAL32 in_fTotalTrainingOfAdditionalCells = 0) const;

		//! Should be used only in the destroy country function
		void LeaveWar(UINT32 in_iCountryID, UINT32 in_iWarID);

		//! It will change the country opinion on a war
		void ChangeOpinionOnWar(UINT32 in_iCountryID, UINT32 in_iWarID, bool in_bWantsPeace);

		//! For the specified, it will fill the two sets with the possible countries
		void CountryCanOrderNewUnitsFrom(UINT32 in_iOrderingCountry, set<UINT32>& out_PossibleCountries) const;

		REAL32 TotalUnitsValueThatHateCountry(UINT32 in_iCountryID);

		//! Iterate the newest exports/imports desired for that country
		void IterateDesiredExportsImports(UINT32 in_iCountryID);

		void IterateBudgetRevenueTax(UINT32 in_iCountryID);
		void IterateBudgetExpenseSecurity(UINT32 in_iCountryID);

		void IterateBudgetUnitUpkeep(UINT32 in_iCountryID);

		//! Execute operations on wars
		void IterateWars();

		//! Returns the upkeep fee for a unit
		REAL32 UpkeepFeeForSingleUnit(const SP2::GUnit* in_pUnit) const;

		//! Returns true if that region id touch other countries than the in_iCountryID
		bool RegionTouchesOtherCountryOrWater(UINT32 in_iCountryID, UINT32 in_iRegionID) const;

		//! The attacking country will occupy every region of the target country. It the target country is the same as the attacking, it will defend every region. If there are any additional countries, it will attack regions controlled by that country too
		void OccupyCountry(UINT32 in_iCountryAttacking, UINT32 in_iCountryTarget,const set<UINT32>& in_vAdditionalEnnemies);

		UINT32 SplitUnitGroup(SP2::GUnitGroup* in_pUnitGroup,
								  vector<SP2::GUnit>& in_vUnitsToMove);
		
		//! Will the update the cost for every country inside a treaty
		void UpdateCostByCountryInsideTreaty(UINT32 in_iTreatyID);

      //! Change the military control of a region.
      void IterateCountryControl();

		//! Give the revenues from all of its region, only to be used for the economical failure checks
		REAL64 GetRevenuesForEconomicFailure(UINT32 in_iCountryID) const;

		//! Sets the eligible countries for that treaty
		void SetEligibleCountries(UINT32 in_iTreatyID, bool in_bCheckForNotAtWar);

      //! Iterate the covert actions cells
      void IterateCovertActionCells();

		REAL32 FindNationalSecurity(UINT32 in_iCountryID) const;

		REAL32 CalculateProbabilityOfAction(const GCountryData* in_pCountryData, REAL32 in_fIndividualism, REAL32 in_fHierarchicalDistance, REAL32 in_fMale, REAL32 in_fIncertitudeControl);

		void CalculateMarketAvailability();

		//! Change the status of every unit group of that country
		void ChangeAllUnitGroupStatus(UINT32 in_iCountry, EMilitaryStatus::Enum in_NewStatus);

      /*!
       * \brief Test if the military control of a region must take place.
       *
       * @param in_iRegionID  : Id of the region to update the military control
       * @param in_NewControl : Id of the country attempting to take control of the region
       **/
      void UpdateMilitaryControl(UINT32 in_iRegionID, UINT32 in_iCountryID);

      /*!
       * \brief Change the military control of a region.
       *
       * @param in_iRegionID  : Id of the region to change the military control
       * @param in_NewControl : New controler for the region
       * @param in_bTestConquer  : True if the region change must test for country conquer
       **/
      bool ChangeRegionMilitaryControl(const UINT32 in_iRegionID,
                                       const UINT32 in_NewControl,
                                       bool in_bTestConquer = true);

		//! Returns true if that country is admissible to that treaty
		bool CountryCanJoinTreaty(UINT32 in_iCountryID, UINT32 in_iTreatyID, bool in_bCheckForRelations);

		//! Returns true if the treaty can join a treaty with those conditions, and those members
		/*!
       * @param in_iCountryID  : Id of the country
       * @param in_pConditions : Array of conditions
       * @param in_vListOfCountries  : Countries on side A and B, for that treaty
       **/
		bool CountryCanJoinThoseConditions(UINT32 in_iCountryID, const UINT32* in_pConditions, const set<UINT32>& in_vListOfCountries, bool in_bCheckForRelations);

		//! Will look for countries needing suspension, or cancel suspension
		void ExecuteSuspension(UINT32 in_iTreatyID);

      //! Prepare the annexion of a region
      void AnnexRegion(UINT32 in_iCountryID, UINT32 in_iRegionID);

		//! Returns the yearly unit production for a country
		REAL64 CountryYearlyUnitProduction(UINT32 in_iCountryID) const;

		//! Returns true if the country has at least 1 research rank in all required research category, for that unit category
		bool CanBuildUnitDesign(UINT32 in_iCountryID, EUnitCategory::Enum in_eCategory) const;

      //! Get the unit movement creator object
      GUnitMover& UnitMover()
      {
         return m_UnitMover;
      }

		//! Fill the vector with the locations of naval waypoints adjacent to that country
		void FindNavalWaypointsAdjacentToCountry(UINT32 in_iCountryID, vector<Hector::GVector2D<REAL32> >& out_Positions) const;		

		//! The country will join this war, on that particular side
		bool JoinAWar(UINT32 in_iCountryID, UINT32 in_iWarToJoin, UINT32 i_iSide, bool in_bWantToContinueWar);

		//! Change the martial law to TRUE or FALSE
		bool ChangeMartialLaw(ENTITY_ID in_iCountryID, bool in_bMartialLawStatus);

		//! Change the government type of a country.
		bool ChangeGovernmentType(ENTITY_ID in_iCountryID, 
										  EGovernmentType::Enum in_iPreviousGvtType,
										  EGovernmentType::Enum in_iNewGvtType);

		//! Change the status of a language. 
		bool ChangeLanguageStatus(ENTITY_ID in_iCountryID,
										  INT32 in_iLanguageID,
										  INT32 in_iOldStatus,
										  INT32 in_iNewStatus);

		//! Change the status of a religion. 
		bool ChangeReligionStatus(ENTITY_ID in_iCountryID,
										  INT32 in_iReligionID,
										  INT32 in_iOldStatus,
										  INT32 in_iNewStatus);
		
		//! Change the status of a political party. The Party ID is the ID of that party in the DB.
		bool ChangePoliticalPartyStatus(ENTITY_ID in_iCountryID,
												  INT32 in_iPartyID,
												  INT32 in_iOldStatus,
												  INT32 in_iNewStatus);		

		//! Close or open the emigration. If you want to close it, EmigrationStatus = true. To open it, EmigrationStatus = false.
		bool ChangeEmigration(ENTITY_ID in_iCountryID, bool in_bEmigrationStatus);

		//! Close or open the Immigration. If you want to close it, ImmigrationStatus = true. To open it, ImmigrationStatus = false.
		bool ChangeImmigration(ENTITY_ID in_iCountryID, bool in_bImmigrationStatus);

		//! Change the status of a law (true == legal, false == illegal) for a country (true == illimited, false = limited for Nb of Children)
		void ChangeCountryInternalLaw(UINT32 in_iCountryID, 
												EInternalLaws::Enum in_eLaw, 
												bool in_bStatus);		

		//! Change a ratio for a budget expense
		bool ChangeBudgetExpenseRatio(UINT32 in_iCountryID, 
												EBudgetExpenses::Enum in_Expense, 
												REAL64 in_fNewRatio);

		//! Change the interest rate of a country
		bool ChangeInterestRate(ENTITY_ID in_iCountryID, 
										REAL32 l_fOldInterestRate, 
										REAL32 l_fNewInterestRate);

		//! Change the personal income tax of a country
		bool ChangePersonalIncomeTax(ENTITY_ID in_iCountryID, 
											  REAL64 l_fOldPersonalIncomeTax, 
											  REAL64 l_fNewPersonalIncomeTax);

		//! Change the global tax mod of a country
		bool ChangeGlobalModTax(ENTITY_ID in_iCountryID, REAL32 in_fNewTax);

		//! A country will start its nuclear research
		void StartNuclearResearch(UINT32 in_iCountryID);

		//! Change the country stability of a country by some pourcentage. If proportional is set to true, the % is calculated from the existing stability value.
		/*
		Example: Country A has a stability of .86. 
			- Executing ChangeCountryStability(A,0.04,false) changes the stability to 0.9
			- Executing ChangeCountryStability(A,0.04,true) changes the stability to 0.8944 (.86 * 1.04)
		*/
		bool ChangeCountryStability(ENTITY_ID in_iCountryID, REAL32 in_fStabilityChanges, bool in_bProportional = true);

		//! Change the country approval of a country by some pourcentage. 
		bool ChangeCountryApproval(ENTITY_ID in_iCountryID, REAL32 in_fApprovalChanges);

		//! Change how a resource is controlled, public or private
		bool ChangeResourceGvtControlled(ENTITY_ID in_iCountryID, EResources::Enum in_iResource, bool in_bControlled);

		//! Change resource tax
		bool ChangeResourceTax(ENTITY_ID in_iCountryID, EResources::Enum in_iResource, REAL32 in_fNewTax);

		//! Change resource import desired
		bool ChangeResourceImportDesired(ENTITY_ID in_iCountryID, EResources::Enum in_iResource, REAL64 in_fNewDesired);

		//! Change resource export desired
		bool ChangeResourceExportDesired(ENTITY_ID in_iCountryID, EResources::Enum in_iResource, REAL64 in_fNewDesired);

		//! Change resource to legal or illegal (legal = true, illegal = false)
		bool ChangeResourceStatus(ENTITY_ID in_iCountryID, EResources::Enum in_iResource, bool in_bNewStatus);		

		//! Disband Units, return true if success
		bool DisbandUnits(ENTITY_ID in_iOwnerID, SDK::Combat::GUnit* in_pUnit);

		//! Remove production from a country. It will remove production in every regions of that country
		void RemoveProductionFromCountry(UINT32 in_iCountryID, REAL64 in_fLossOfProduction, EResources::Enum in_iResource);

		//! Add production from a country. It will add production in every regions of that country
		void AddProductionToCountry(UINT32 in_iCountryID, REAL64 in_fGainOfProduction, EResources::Enum in_iResource);

		//! Removes x number of person from a country. Return number of people actually removed
		/*!
		*	@param in_iCountryID: ID to remove population from
		*	@param in_iNbPopulation: number of person to remove
		*	@param in_bEnroll: true if we are removing for infantry, false otherwise
		**/
		INT64 RemovePopulationFromCountry(UINT32 in_iCountryID, INT64 in_iNbPopulation, bool in_bEnroll = false);

		//! Removes x number of person from a region. Return true if success
		/*!
		*	@param in_iRegionID: region ID to remove population from
		*	@param in_iNbPopulation: number of person to remove
		*	@param in_bEnroll: true if we are removing for infantry, false otherwise
		**/
		bool RemovePopulationFromRegion(UINT32 in_iRegionID, INT64 in_iNbPopulation, bool in_bEnroll = false);

		//! Add population to that region
		bool AddPopulationToRegion(UINT32 in_iRegionID, INT64 in_iNbPopulation);

      //----------UNIT PRODUCTION METHODS-----------

      //! Build a new unit, returns the ID of the production queue entry
      INT32 BuildUnits(UINT32 in_iBuildingCountryID,
                       UINT32 in_iDestinationCountryID,
                       UINT32 in_iDesignID,
                       UINT32 in_iQty,
                       UINT8  in_iProductionSpeed,
                       EUnitProductionPriority::Enum in_ePriority);

      //! Cancel a unit production
      bool CancelUnitProduction(UINT32 in_iBuildingCountryID, 
                                UINT32 in_iCancellingCountryID, 
                                UINT32 in_iProductionRequestID,
										  bool in_bSabotage = false);

      //! Change a unit production priority level
      bool ChangeUnitProductionPriority(UINT32 in_iBuildingCountryID, 
                                UINT32 in_iProductionRequestID,
                                EUnitProductionPriority::Enum in_eNewPriority);


		//! The attackers declare a war against the defending side
		bool DeclareNewWar(const set<UINT32>& in_AttackingCountries, UINT32 in_iMasterAttacking, UINT32 in_iDefendingCountry);

      //! Sell a unit
      void SellUnit(UINT32 in_iUnitIdToTrain);

      //! Buy a unit
      bool BuyUnit(UINT32 in_iCountryId,UINT32 in_iUnitID,UINT32 in_iQty);

      //! Spliting is fun (I am tired)
      GUnit* SplitUnit(GUnit* in_pUnitToSplit, INT32 in_iQuantityToGive, GUnitGroup* in_pDestinationGroup);

      //! Return true if units is for sale
      bool IsUnitForSale(UINT32 in_iUnitIdToTrain);

      //! Return a reference to the list of units that are for sale
      const hash_set<UINT32>& ForSaleUnitList(void);

      //! Gets the production percentage
      REAL32 UnitProductionProgress(UINT32 in_iCountryID,UINT32 in_iProductionRequestID);

      //! Send Production Queue
      void SendProductionQueueToCountry(UINT32  in_iCountryID);

      //! Deploy units at a specific position
      void DeployUnits(UINT32  in_iCountryID,list<UINT32> &in_UnitListToDeploy, GVector2D<REAL32> in_DeployementCoordinate);

      //! Deploy a missile from the production group
      void DeployMissileFromUnitGroup(SP2::GUnitGroup* in_pGroup,list<UINT32>& in_UnitListToDeploy, GVector2D<REAL32> in_DeployementCoordinate);
      
		//! Adds in_iNbPopulaton to the population of a country, after having disbanded infantry (they will all be put to the 15-65 year-range)
		bool AddPopulationAfterDisbandInfantry(UINT32 in_iCountry, INT64 in_iNbPopulation);

      //------- UNIT PRODUCTION METHODS, END ---------

		//! Remove a certain amount of money from a country
		bool TakeMoney(ENTITY_ID in_iCountryID, REAL64 in_fAmountToRemove);
		//! Give a certain amount of money to a country
		bool GiveMoney(ENTITY_ID in_iCountryID, REAL64 in_fAmountToGive);


      REAL32 MissileRearmingDelay(const SP2::GUnitType* in_pUnitType) const;


      //! Advances the unit production
      void IterateUnitProduction();
      void IterateResearch(REAL64 in_fClock);
      void IterateNukes();

		//! Execute an increase production 
		void IncreaseProduction(UINT32 in_iCountryID, EResources::Enum in_eResource, REAL64 in_fPourcOfIncrease);

      bool CreateUnitDesign(UINT32 in_iID,
                            UINT32 in_iDesignerID,
                            GUnitType* in_pType,
                            const GString& in_sName,
									 INT32 in_iNameID,
                            UINT8 in_iSpeedAndManeuverability,
                            UINT8 in_iSensors,
                            UINT8 in_iGunRange,
                            UINT8 in_iGunPrecision,
                            UINT8 in_iGunDamage,
                            UINT8 in_iMissilePayload,
                            UINT8 in_iMissileRange,
                            UINT8 in_iMissilePrecision,
                            UINT8 in_iMissileDamage,
                            UINT8 in_iStealth,
                            UINT8 in_iCounterMesures,
                            UINT8 in_iArmor,
                            INT16 * in_iPiece = NULL,
                            INT16 in_iColor = -1);

      //! Starts the AMDS research
      bool StartAMDSResearch(UINT32 in_iCountryID);

      //! Launches nuclear attacks
      UINT32 LaunchNuclearAttacks(const vector<GNukeLaunchRequest>&);

      //! Compute the civilian casualties of a nuke (and perform modifications)
      UINT32 NuclearCasualtiesCivilExecute(const GNukeTarget& in_Target,const multimap<REAL32,GNuclearMissile>& in_Missiles);

      //! Compute the military casualties of a nuke (and perform modifications)
      UINT32 NuclearCasualtiesMilitaryExecute(const GNukeTarget& in_Target,const multimap<REAL32,GNuclearMissile>& in_Missiles);

      //! Broadcast the given packet to all the human players
      void BroadcastUDPPacket(void* in_pPacket, short in_iSize);

      //! Change the capital of a country, making sure the change is valid
      void ChangeCapitalID(UINT32 in_iCountryID, UINT32 in_iCapitalID);

		//! Create a new treaty. 
		UINT32 CreateNewTreaty(ENTITY_ID in_iCountryCreator,
									const set<ENTITY_ID>& in_vSideA,
									const set<ENTITY_ID>& in_vSideB,
									const set<ENTITY_ID>& in_vPressure,
									ETreatyType::Enum in_iType,
									bool in_bPrivate,
									GString& in_sName,
									const UINT32* in_pConditions);

		//! Join an existing treaty. Side 1: A, Side 2: B, Side 3: Pressure
		void JoinTreaty(ENTITY_ID in_iCountry,
							 UINT32 in_iTreatyID,
							 UINT32 in_iSide);
		
		//! Leave an existing treaty
		void LeaveTreaty(ENTITY_ID in_iCountry,
							  UINT32 in_iTreatyID,
							  bool in_bClearOriginal = false);

      //! Declares war to all military units that are in range at the given position
      void DeclareWarToNeutralUnitsWithinRange(UINT32 in_iCountryID,
                                               const GVector2D<REAL32>& in_Position,
                                               REAL32 in_fSquareRangeRange); 

		//! Send news to the clients
      void SendNews(const News::GInfo& in_News);

		//!Send nuke news
		void SendNewsNukes();

		//!Send cities casualties nuke
		void SendNewsCasualties();

      //! Debug method to log military data incoherences
      void LogMilitaryDataIncoherences(bool l_bFixIncoherences);

      //! Perform the retreat of all the units of a country
      bool RetreatCountryFromCombat(SP2::GArena* in_pArena,UINT32 in_iCountry,GVector2D<REAL32> &in_MoveToPosition);
      bool RetreatCountryFromCombat(SP2::GArena* in_pArena,UINT32 in_iCountry);

      //! Higher level retreat
      void RetreatFromCombat(SP2::GArenaInfo* in_pArenaInfo,UINT32 in_iCountry);

      GGameObjectiveEvaluator& GameObjectiveEvaluator();

      //! Updates the objectives for the players with the newly setted objectives
      void UpdateGameObjectives();

      //! Execute the covert action mission for the cell (all values are contained in it). Return true if the cell must be destroyed
      bool ExecuteMission(GCovertActionCell& in_Cell);

		//! Gives the average human development in the world, as if the whole world were one country
		REAL32 AverageHumanDevelopment() const;


      //! Request naval units (in parameter) to bombard the given region
      bool NavalUnitsBombardRegion(const vector<SDK::Combat::GUnitGroup*>& in_vGroups,UINT32 in_iRegionId);

      //! Sum up the characteristics needed to perform (attack) offshore bombardment
      UINT32 BombardmentAttackSumCharacteristics(SP2::GUnitGroup* in_pGroup);

      //! Sum up the characteristics needed to defend from offshore bombardment
      UINT32 BombardmentDefendSumCharacteristics(SP2::GUnitGroup* in_pGroup);


      //! Lists points in the region where explosions can be
      void BombardmentExplosionsPointsInRegion(UINT32 in_iRegionId,vector<GVector2D<REAL32> >& out_vPoints) const ;



      void SendRelationHistory(UINT32 in_iPlayerID,UINT32 in_iCountry1,UINT32 in_iCountry2);

      //! Acknowledge a request through an id to a given target
      void AcknowledgeRequest(UINT64 in_iRID, INT32 in_iTarget);


      // GGameDataNode implementation
      virtual bool OnSave(GIBuffer& io_Buffer);
      virtual bool OnLoad(GOBuffer& io_Buffer);
      virtual bool OnNew(GDatabase* in_pDatabase);
      virtual void OnClean();

      //! Send the building country list
      bool SendBuildingCountriesList(UINT32 in_iCountryId);

      //! Send the building country list to a list of country
      void SendBuildingCountriesList(const set<UINT32> &in_sCountries);

      //! Update the world peace status
      void UpdateWorldPeaceStatus(bool in_bWorldAtPeace);
 
      //! split the nuclear missiles from the in_pUnitSrc between the in_pUnitSrc and in_pUnitDest
      void GiveMissilesFromUnit(GUnit* in_pUnitSrc, GUnit* in_pUnitDest, REAL32 in_fRatioToGive);

      //! Helper method to change a country's name
      void ChangeCountryName(ENTITY_ID in_iCountryID, const GString& in_sNewName);
   };
};

#endif //_GOLEM_SP2_GDCL_H_