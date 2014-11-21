#ifndef _SP2_CONSTANTS__H
#define _SP2_CONSTANTS__H

namespace SP2
{
   //Version Number displayed in the main screen and when typing Version in the console
   //-----------------------------------------------------------------
   const UINT32  c_iSP2GameType = MAKEFOURCC('S', 'P', '2', '\0');
   const UINT32 c_iSP2DataVersion = 7;

	const GString NUMBER_PATTERN = L"[number]";
	const GString STRING_PATTERN = L"[string]";
	const GString UNIT_PATTERN = L"[unit]";	


   const GString c_sSaveExtServer = L".gss";
   const GString c_sSaveExtClient = L".gsc";
   const UINT32 c_iGameSaveLoadErrorNotAdmin = 0x80000000;

   const GString c_sMovieLogoDC = L"data/movies/ng.mpg";
   const GString c_sMovieLogoGolem = L"data/movies/golem.mpg";
   const GString c_sMovieIntro = L"data/movies/sp2.mpg";

   const GString c_sMovieCreditsPol = L"data/movies/CP.mpg";
   const GString c_sMovieCreditsMil = L"data/movies/CM.mpg";
   const GString c_sMovieCreditsEco = L"data/movies/CE.mpg";

   // Set Database and save location
   const GString c_sDatabaseLocation = "data/database.gdb";
   const GString c_sSaveGameLocation = "save/";
   
   const GString * const c_sMovieCredits[3] = 
   {
      &c_sMovieCreditsPol,
      &c_sMovieCreditsMil,
      &c_sMovieCreditsEco,
   };

	namespace ETrainingLevel
	{
		enum Enum
		{
			Recruit = 0,
			Regular,
			Veteran,
			Elite,
			ItemCount
		};
	};


   //! Constants that defines the training modifiers
   const REAL32 c_TrainingModifiers[ETrainingLevel::ItemCount] =
   {
       0.5f ,
       1.0f ,
       1.25f,
       2.0f 
   };

   /*!
    * List the possible types of treaties
    **/
   namespace ETreatyType
   {
      enum Enum
      {
         Undefined = -1,
         Alliance = 0,
         War,
         MilitaryAccess,
         DELETED1,
         DELETED2,
			DELETED3,
			EconomicPartnership,
			CommonMarket,
			EconomicAid,
			AssumeForeignDebt,
			EconomicEmbargo,
			WeaponTrade,
			WeaponTradeEmbargo,
			HumanDevelopmentCollaboration,
			CulturalExchanges,
			ResearchPartnership,
			RequestMilitaryPresenceRemoval,
			NobleCause,
			Annexation,
			FreeRegion,
			ItemCount
      };
   }

	namespace ERandomEvents
	{
		enum Enum
		{
			ActsOfNatureBeginning,		//no "Acts of Nature" should be before that
			Avalanche,
			Blizzard,
			ColdWave,
			Cyclone,
			Drought,
			Earthquake,
			Flood,
			HeathWave,
			Hurricane,
			InsectInfestation,
			Landslide,
			Storm,
			TidalWave,
			Tornado,
			Tsunami,
			Typhoon,
			VolcanicEruption,
			WildFire,
			ActsOfNatureEnd,	//end of "Acts of Nature"
			EconomyBeginning, //no "Economy" before that
			StockMarketCrash,
			EconomicDepression,
			EconomicBoom,
			PoorHarvest,
			BountifulHarvest,
			LivestockEpidemic,
			ResourceDiscoveryOre,
			ResourceDiscoveryEnergy,
			ResourceDiscoveryWealth,
			EconomyEnd, //end of "Economy"
			DemographyBeginning, //no "Demography" before that
			Epidemic,
			PopulationBoom,
			Famine,
			DemographyEnd, //end of "Demography"
			EnvironmentBeginning, //no "Environment" before that
			NuclearMeltdown,
			ChemicalSpill,
			IndustrialAccident,	
			EnvironmentEnd, //end of "Environment"
			PoliticsBeginning, //no "Politics" before that
			AntiGlobalizationProtest,
			PoliticsEnd, //end of "Politics"
			ItemCount
		};
	};

	namespace EAdvisorActions
	{
		enum Enum
		{
			RaiseTaxes = 1,
			LowerTaxes,
			DeclareMartialLaw,
			ChangeToDemocracy,
			ChangeToMonarchy,
			ChangeToCommunism,
			ChangeToTheocracy,
			ChangeToDictatorship,
			GiveBetterStatusRelLang,
			GiveWorseStatusRelLang,
			RaiseInterestRate,
			LowerInterestRate,
			LegalFreedomSpeech,
			IllegalFreedomSpeech,
			LegalFreedomDemonstration,
			IllegalFreedomDemonstration,
			IllimitedNbChildren,
			LimitedNbChildren,
			IncreaseProduction,
			LegalAbortion,
			IllegalAbortion,
			LegalContraception,
			IllegalContraception,
			Assassination,
			TrainSpecops,
			DisbandSpecops,
			DisbandNuke,
			LaunchNuke,
			NuclearizeCountry,
			CreateSpecops,
			WarDeclaration,
			Alliance,
			MilitaryTrespassingRight,
			Annexation,
			RaiseResourceTaxes,
			LowerResourceTaxes,
			FreeRegion,
			CulturalExchanges,
			NobleCause,
			ResearchPartnership,
			HumanDevelopmentCollaboration,
			EconomicPartnership,
			CommonMarket,
			EconomicAidGive,
			EconomicAidReceive,
			AssumeForeignDebtGive,
			AssumeForeignDebtReceive,
			EconomicEmbargo,
			WeaponTrade,
			WeaponTradeEmbargo,
			ResearchAmds,
			TrainInfantry,
			EnrollInfantry,
			DisbandInfantry,
			TrainGroundUnits,
			BuildGroundUnits,
			DisbandGroundUnits,
			TrainAirUnits,
			BuildAirUnits,
			DisbandAirUnits,
			TrainNavalUnits,
			BuildNavalUnits,
			DisbandNavalUnits,
			PrivateControlResource,
			PublicControlResource,
			EspionageCivilian,
			EspionageMilitary,
			Terrorism,
			CoupEtat,
			SabotageCivilian,
			SabotageMilitary,
			CloseEmigration,
			CloseImmigration,
			OpenEmigration,
			OpenImmigration,
			TradeInRegion,
			TradeInResearch,
			ParkUnits,
			Undefined
		};
	};

	namespace EAdvisorObjectives
	{
		enum Enum
		{
			PopulationSupport = 1,
			HaveSpace,
			GovernmentStability,
			EconomyTooLow,
			EconomyTooHigh,
			HavePositiveBudget,
			MeetResourceDemand,
			HaveAMDS,
			HaveMilitaryAllies,
			LowCorruptionLevel,
			EnemiesLowStability,
			EnemiesNoMoney,
			EnemiesNoMilitary,			
			HighSelfRelations,
			HighNationalSecurity,
			HighHumanDevelopment,
			HighTelecomLevel,
			HighInfrastructure,
			HighEconomicHealth,
			HaveNoDebt,
			StrongGroundForces,
			StrongInfantryForces,
			StrongAirForces,
			StrongNavalForces,
			StrongNuclearForces,
			GoodResearchLevel,
			MilitaryAccess,

			TooManyMilitaryUnits,		//Non ehe objectives
			Undefined
		};
	};

	const Hector::DesignPattern::GSubjectNotification  c_iNotificationLeadingPoliticalPartyChangeAfterElections = 0x04;
   const Hector::DesignPattern::GSubjectNotification  c_iNotificationOnSuccessfulCoupEtat                      = 0x05;
   const Hector::DesignPattern::GSubjectNotification  c_iNotificationOnNukeLaunched                            = 0x06;
   const Hector::DesignPattern::GSubjectNotification  c_iWorldPeaceStatusChanged                               = 0x07;

   //! Every playable sphere
   namespace ESphere
	{
		enum Enum
		{
			Economic = 1,
			Politic,
         Military,
         Demographic,
         Count         
      };
   };

	/*!
	* Enum of all internal laws
	**/

	namespace EInternalLaws
	{
		enum Enum
		{
			FreedomOfSpeech ,
			FreedomOfDemonstration,
			WomenSuffrage,
			NumberOfChildren,
			Contraception,
			Abortion,
			ChildLabor,
			Polygamy,
			SameSexMarriage,
			ItemCount	//Must be the last one
		};
	};

   namespace EUnitDesignCharacteristics
   {
      enum Enum
      {
         Undefined = -1,
         GunRange,
         GunPrecision,
         GunDamage,
         MissileRange,
         MissilePrecision,
         MissileDamage,
         MissilePayload,
         Speed,
         Stealth,
         Sensors,
         CounterMesures,
         Armor,
         ItemCount //Must be the last item
      };
   };

	const REAL64 c_fTechnologieCost[10] = 
	{
		48750000000.f,
		60000000000.f,
		78000000000.f,
		97500000000.f,
		195000000000.f,
		390000000000.f,
		780000000000.f,
		1560000000000.f,
		3120000000000.f,
		6240000000000.f
	};

   const UINT32 c_iUnitDesignCharacteristicsStringID[EUnitDesignCharacteristics::ItemCount] = 
   {
      100195, //GunRange
      100196, //GunPrecision
      100197, //GunDamage
      100198, //MissileRange
      100199, //MissilePrecision
      100200, //MissileDamage
      100201, //MissilePayload
      100202, //Speed
      100203, //Stealth
      100204, //Sensors
      100205, //CounterMesures
      100206, //Armor
   };

   const UINT32 c_iAINbObjectives = 27;

	/*!
	* Enum to give	the desired	resource	id
	**/
	namespace EResources
	{
		enum Enum
		{
         //If you add items in here and wish to have a context menu, make sure you check
         //the EContextMenuEntries enumeration, and please, do not reorder this list just for fun
         //if so, the new order change must be reflected in EContextMenuEntries
			Cereals = 0,
			Vegetable_Fruits,
			Meat,
			Dairy,
			Tobacco,
			Drugs,
			Electricity,
			Fossile_Fuels,
			Wood_Paper,
			Minerals,
			Iron_Steel,
			Precious_Stones,
			Fabrics,
			Plastics,
			Chemicals,
			Pharmaceuticals,
			Appliances,
			Vehicules,
			Machinery,
			Commodities,
			Luxury_Commodities,
			Construction,
			Engineering,
			Health_Care,
			Retail,
			Legal_Services,
			Marketing_Advertising,
			ItemCount		//Should always be the last one
		};		

	};//End namespace	EResources

	//Yearly modifiers for each resources
	const REAL64 c_pResourcesYearlyGain[EResources::ItemCount] = 
   {
      0.01f,
		0.01f,
		0.01f,
		0.01f,
		0.01f,
		0.01f,
		0.02f,
		0.02f,
		0.01f,
		0.01f,
		0.01f,
		0.01f,
		0.1f,
		0.1f,
		0.1f,
		0.1f,
		0.13f,
		0.13f,
		0.13f,
		0.13f,
		0.13f,
		0.16f,
		0.16f,
		0.16f,
		0.16f,
		0.16f,
		0.16f,       
   };	

	namespace EContinent
   {
      enum Enum
      {
			America = 1,
			Europe,
			Asia,
			Oceania,
			Africa,
			Undefined
      };
   }

	namespace EGeoGroups
	{
		enum Enum
		{
			WesternEurope = 1,
			CentralAmerica,
			SouthernAmerica,
			EasternEurope,
			EasternAsia,
			FarEast,
			MiddleEast,
			NorthernAfrica,
			WesternAfrica,
			EasternAfrica,
			SouthernAfrica,
			PacificIslands,
			NorthernAmerica,
			SouthernOceania,
			Undefined
		};
	}

	/*!
	* Enum to give	the gvt type id
	**/
	namespace EGovernmentType
	{
		enum Enum
		{
			Anarchy = 1,
			Conquered,
			Protectorate,
			Theocracy,
			MilitaryDictatorship,
			Monarchy,
			Communist,
			SinglePartyDemocracy,
			MultiPartyDemocracy,
			ItemCount
		};		

	};//End namespace	EGovernmentType

   namespace ECharacteristicToHighlight 
   { 
      enum Enum
      {
         Geographic,
         CountryEconomicalStrength,
         CountryEmpire,
			PoliticalControl,
         CountryRelations,
         CountryExceedingResources,
         CountryGrossDomesticProduct,
         CountryGrossDomesticProductGrowth,
         CountryHumanDevelopment,
         CountryMilitaryStrength,
         CountryNuclearStrength,
         CountryPoliticalStrength,
         CountryPopulationGrowth,
         CountryPopulationStabilty,
         CountryPopulationSupport,
         HumanControlledCountries,
         RegionPopulationDensity,
         RegionPopulationTotal,      
         RegionResourcesProduction //must be the last item if we want: EContextMenuEntries to remain valid
      };
   }

   namespace EThematicMapRegionDescription
   {
      enum Enum
      {
         Unknown            = -1,
         PlayerControlled   = 0,
         Allied,
         Hostile,
         MilitaryControlled,
         PoliticallyControlled,
         TakenBy,
         TakenFrom,
			MilitaryAccess,
         ItemCount
      };
   };

   namespace EMilitaryStatus
   {
      enum Enum
      {
         Parked,    //must be the 1st element
         Ready,     //must be the 2nd element  --> Because of 
         Fortified, //must be the 3rd element
         Moving,    //must be the 4th element
         CanJoinCombat,
         InDeployement = CanJoinCombat,
         CountVisible,
         Attacking = CountVisible, // Units that can be visible
         Training,
         Upgrading,
         ForSale,
         InTransit,
         ReadyToDeploy,
         CountTotal                // This should always be the last item
      };
   }

   const REAL32 c_GroupMilitaryStatusModifiers[4] = 
   {
      0.25f, //parked
      1.f,   //ready
      1.5f,  //fortified
      1.f    //moving
   };

   namespace ERefusedMoveReason
   {
      enum Enum
      {
         Unknowm,
         NoDiplomaticSupport,
         NoMilitarySupport,
         NoAirSupport,
         NoWaterSupport,
         NoLandSupport,
         Count,
      };
   }

	namespace EAdvisorStatus
	{
		enum Enum
		{
			Manual,
			Automatic,
			Confirmation
		};
	};

	namespace EEHECategory
	{
		enum Enum
		{
			Military,
			Politic,
			Economic,
			Undefined
		};
	};

   namespace EMilitaryAction
   {
      typedef UINT32 Flags;
      const Flags Default       = 0;
      const Flags Attack        = 1;
      const Flags MergeWith     = 2;
      const Flags BombardRegion = 4;
   }	

	namespace EBudgetExpenses
	{
		enum Enum
		{
			Telecom,
			Research,
			Propaganda,
			Infrastructure,
			IMF,
			Healthcare,
			Government,
			Environment,
			Education,
			Tourism,
			ItemCount
		};
	};

	//Cost modifier for unit statuses
	const REAL32 READY			= 0.25f;
	const REAL32 FORTIFIED		= 0.25f;
	const REAL32 PARKED			= 0.1f;
	const REAL32 FORSALE			= 0.1f;
	const REAL32 TRAINING		= 2.5f;
	const REAL32 HUMANDEVMOD	= 0.25f;

	//Maximum distance of clipping
	const REAL32 c_fDistanceMaxWhenSpecificTarget = 100.f;

	//Accuracy of Information constants
	const REAL32 c_fSameRegionAccuracyBonus = 0.2f;

	//Necessity to liberate and change the government type of a country
	const REAL32 c_fPercentageNecessaryToChangeGovernmentType = 0.75f;

   //Cell constants
   const REAL64 c_fCellCreationCost = 100000000.f;
   
   const REAL64 c_fCellUpkeepBase   = 10000000.f;
   const REAL64 c_fCellUpkeepModifierDormant       = 0.5f;
   const REAL32 c_fCellUpkeepModifierRecruit       = 0.75f;
   const REAL32 c_fCellUpkeepModifierRegular       = 1.f;
   const REAL32 c_fCellUpkeepModifierVeteran       = 4.f;
   const REAL32 c_fCellUpkeepModifierElite         = 10.f;

   const REAL64 c_fCellTrainingCostRegular         = 2000000.f;
   const REAL64 c_fCellTrainingCostVeteran         = 10000000.f;
   const REAL64 c_fCellTrainingCostElite           = 100000000.f;
   
   const REAL64 c_fCellMissionCostEspionage        = 10000000.f;
   const REAL64 c_fCellMissionCostSabotage         = 100000000.f;
   const REAL64 c_fCellMissionCostSocialUpheaval   = 50000000.f;
   const REAL64 c_fCellMissionCostAssassination    = 10000000.f;
   const REAL64 c_fCellMissionCostTerrorism        = 500000.f;
   const REAL64 c_fCellMissionCostCoupEtat         = 1000000000.f;
   
   const INT8   c_iCellCreationTimeMonthRecruit    = 1;
   const INT8   c_iCellCreationTimeMonthRegular    = 2;
   const INT8   c_iCellCreationTimeMonthVeteran    = 4;
   const INT8   c_iCellCreationTimeMonthElite      = 8;
   
   const INT8   c_iCellMissionPrepareTimeMonthEspionnage     = 3;
   const INT8   c_iCellMissionPrepareTimeMonthSabotage       = 4;
   const INT8   c_iCellMissionPrepareTimeMonthSocialUpheaval = 3;
   const INT8   c_iCellMissionPrepareTimeMonthAssassination  = 2;
   const INT8   c_iCellMissionPrepareTimeMonthTerrorism      = 6;
   const INT8   c_iCellMissionPrepareTimeMonthCoupEtat       = 12;

	//Unit type upkeep modifiers
	const REAL32 c_fUnitUpkeepModifierRecruit = 0.75f;
	const REAL32 c_fUnitUpkeepModifierRegular = 1.f;
	const REAL32 c_fUnitUpkeepModifierVeteran = 1.5f;
	const REAL32 c_fUnitUpkeepModifierElite = 3.f;

	//AI Action  modifiers
	const REAL32 c_fAIActionDoubleMinus = 0.f;
	const REAL32 c_fAIActionMinus = 0.25f;
	const REAL32 c_fAIActionNeutral = 0.5f;
	const REAL32 c_fAIActionPlus = 0.75f;
	const REAL32 c_fAIActionDoublePlus = 1.f;	

   typedef stdext::hash_map<INT32, REAL32> GRegionDataVector;		
   typedef GRegionDataVector::value_type GRegionDataPair;

	//! hash_map<RELIGION_ID, pair<POPULATION,STATUS> >
	typedef stdext::hash_map<INT32, INT64 >	GReligionList;	
	//! hash_map<LANGUAGE_ID, pair<POPULATION,STATUS> >
	typedef stdext::hash_map<INT32, INT64 >	GLanguageList;

   //Config filenames
   CWSTR CLIENT_CONFIG              = L"sp2_cfg_client.xml";
   CWSTR SERVER_CONFIG              = L"sp2_cfg_server.xml";
   CWSTR COMMON_CONFIG              = L"/data/config/common.xml";

	//Nuclear research constants
	const REAL64 c_fNuclearResearchTime	= 1095.f;
	const REAL64 c_fNuclearResearchCost = 25000000000.f;

   //Picture ID Reference
   const INT32 PIC_GRADIENT_GREEN      = 550;
   const INT32 PIC_GRADIENT_RED        = 552;

   const INT32 c_iUpdatableValueIncrementDelay = 3;

   const INT16 c_iCombatPacketIdentifier = (INT16)0x1000;

	const REAL32 c_fMaximumDistanceForDeployment = 500.f;

   const REAL32 c_fCombatThresholdSquare = 0.25f*0.25f;

	//Used units constants
	const REAL32 c_fUsedUnitsRebates = 0.75f;

	//Gvt controlled constant
	const REAL64 c_fGvtControlledResourceMod = 0.25f;

	//Public controlled resource penalty
	const REAL64 c_fPublicResourcePenalty = 0.6f;

	//Corruption strength
	const REAL64 c_fCorruptionStrength = 0.1f;

	//Personal Income Tax Constant
	const REAL64 c_fPersonalIncomeTaxConstant = 4.f;

   //Superpower 2 Economic constants
	const REAL64 DEBT_INTEREST_RATE		= 0.1f;
   const REAL32 c_fProductionRefundRate= 0.90f;
	const REAL64 c_fMaximumPourcPaidByEconomicAid = 0.01f;
	const UINT32 c_iCountriesReceivingIMF = 50;
	const REAL64 c_fFixedCostForTreaties = 100000000;
	const REAL64 c_fAdditionalCostForTreatiesByCountry = 10000000;

	//Relations based on amds and nuclear research
	const REAL32 c_fRelationsLossInAMDSstart = 35.f;
	const REAL32 c_fRelationsLossInNuclearStart = 55.f;
	

	//Treaties variables
	const REAL32 c_fBonusRelationsWhenSigningAssumeForeignDebt = 10.f;
	const REAL32 c_fYearlyLossOfRelationsNuclearDisarmament = 2.f;
	const REAL64 c_fIterationPeriodRelationTreaties = 30.f;	
	const REAL32 c_fHumanDevelopmentCollaborationTreatyBonus = 0.001f;	
	const REAL32 c_fTreatyActivationRatio = 0.6f;
	const REAL32 c_fYearlyRelationsGainCulturalExchange	= 1.f;
	const REAL32 c_fYearlyRelationsGainNobleCause			= 1.f;
	const REAL32 c_fYearlyRelationsLossNobleCause			= -2.f;
	const REAL64 c_fMaximumLargerSizeEconomicPartnership	= 10.f;
	const REAL64 c_fGainInPourcentageForMaximumSizeEP		= 0.001f;
	const REAL64 c_fIterationPeriodNukeNews					= 2.f;
	const REAL32 c_fRelationLossAnnualyFromPressure			= 15.f;
	const REAL32 c_fRelationLossAnnualyFromMilitaryRemoval = 50.f;
	const REAL32 c_fRelationLossLeavingTreaty					= 10.f;
	const REAL64 c_fMaximumPourcOfRevenuesGivenToForeignDebt = 0.05f;
	const REAL64 c_fMinimumPourcOfRevenuesGivenToForeignDebt = 0.01f;
	const REAL32 c_fMinimumPourcOfRatioWhenComparingMilitaryRemoval = 0.5f;
	const REAL32 c_fMaximumPourcOfRatioWhenComparingMilitaryRemoval = 1.f;
	const REAL64 c_fResearchBonusInResearchPartnerships = 0.01f;

	//Conditions variables
	const REAL32 c_fConditionsMilitaryStrengthSimilar			= 0.1f;
	const REAL32 c_fConditionsMilitaryStrengthNotSoSimilar	= 0.25f;
	const INT16	 c_iConditionsEconomicStrengthSimilar			= 5;
	const INT16	 c_iConditionsEconomicStrengthNotSoSimilar	= 20;
	const REAL32 c_fConditionsRelationsFriendlyEnter			= 50.f;
	const REAL32 c_fConditionsRelationsFriendlyLeave			= 25.f;
	const REAL32 c_fConditionsRelationsNeutralEnter				= 0.f;
	const REAL32 c_fConditionsRelationsNeutralLeave				= -25.f;
	const REAL64 c_fConditionsResearchLow							= 0.25f;
	const REAL64 c_fConditionsResearchAverage						= 0.5f;
	const REAL64 c_fConditionsResearchHigh							= 0.75f;

	//Relations variables
	const REAL32 c_fRelationsLove			= 80.f;
	const REAL32 c_fRelationsFriend		= 60.f;
	const REAL32 c_fRelationsLike			= 40.f;
	const REAL32 c_fRelationsGood			= 20.f;
	const REAL32 c_fRelationsBad			= -20.f;
	const REAL32 c_fRelationsDislike		= -40.f;
	const REAL32 c_fRelationsEnnemy		= -60.f;
	const REAL32 c_fRelationsHate			= -80.f;
	const REAL32 c_fLossRelations			= 10.f;
	const REAL32 c_fLossRelationsAllies	= 15.f;
	const REAL32 c_fLossRelationsMaximum = 0.f;
	const REAL32 c_fLossRelationsNuke	= 20.f;
	const REAL32 c_fRelationsGvtTypeSame			= 10.f;
	const REAL32 c_fRelationsGvtTypeSameMonarchy	= 15.f;
	const REAL32 c_fRelationsGvtTypeClose			= 5.f;
	const REAL32 c_fRelationsGvtTypeNeutral		= 0.f;
	const REAL32 c_fRelationsGvtTypeFar				= -10.f;
	const REAL32 c_fRelationsGvtTypeVeryFar		= -20.f;
	const REAL32 c_fRelationsChangeWomenSuffrage	= 10.f;
	const REAL32 c_fRelationsChangeFreedomOfDemonstration	= 20.f;
	const REAL32 c_fRelationsChangeFreedomOfSpeech	= 20.f;
	const REAL32 c_fRelationsChangeChildLabour		= 20.f;
	const REAL32 c_fRelationsChangeSameSexMarriage	= 10.f;
	const REAL32 c_fRelationsDifferenceInLaw			= 5.f;
	const REAL32 c_fRelationsDifferenceEconomyIdeology = 10.f;
	
	//Approval loss constants
	const REAL32 c_fApprovalChangeOfficialToLegal = 0.15f;
	const REAL32 c_fApprovalChangeLegalToIllegal = 0.25f;
	const REAL32 c_fApprovalRatioLowerPersonalTax = 0.5f;
   const REAL32 c_fApprovalRatioRaisePersonalTax = 1.75f;
	const REAL32 c_fApprovalRatioChangeMinimumWage = 0.25f;
	const REAL32 c_fApprovalRatioChangeBudgetExpense = 0.25f;

	//Stability loss constants
	const REAL32 c_fStabilityChangeOfficialToLegal = 0.20f;
	const REAL32 c_fStabilityChangeLegalToIllegal = 0.30f;
	const REAL32 c_fStabilityLossWarDeclarationOffensive = -0.1f;
	const REAL32 c_fStabilityLossWarDeclarationDefensive = -0.02f;
	const REAL32 c_fStabilityCombatStarted = -0.01f;
	const REAL32 c_fStabilityChangeMartialLaw = 0.04f;
	const REAL32 c_fStabilityLossGvtType = -0.2f;
	
	//Anarchy minimum and maximum stability limit
	const REAL32 c_fStabilityAnarchyLowerLimit	= 0.1f;
	const REAL32 c_fStabilityAnarchyHigherLimit	= 0.2f;
	
	//Law constant (left-right ideology)
	const REAL32 c_fLawIdeologyFreedomOfSpeech			= 0.125f;
	const REAL32 c_fLawIdeologyFreedomOfDemonstration	= 0.10f;
	const REAL32 c_fLawIdeologyChildLabor					= 0.075f;
	const REAL32 c_fLawIdeologyWomenSuffrage				= 0.05f;
	const REAL32 c_fLawIdeologyNbOfChildren				= 0.05f;
	const REAL32 c_fLawIdeologyContraception				= 0.025f;
	const REAL32 c_fLawIdeologyAbortion						= 0.025f;
	const REAL32 c_fLawIdeologyPolygamy						= 0.025f;
	const REAL32 c_fLawIdeologySameSexMarriage			= 0.025f;

	//Budget sliders constant
	const REAL64 c_fBudgetTelecomValue			= 0.04f;
	const REAL64 c_fBudgetResearchValue			= 0.09f;
	const REAL64 c_fBudgetPropagandaValue		= 0.03f;
	const REAL64 c_fBudgetInfrastructureValue	= 0.07f;
	const REAL64 c_fBudgetIMFValue				= 0.04f;
	const REAL64 c_fBudgetHealthcareValue		= 0.25f;
	const REAL64 c_fBudgetGovernmentValue		= 0.14f;
	const REAL64 c_fBudgetEnvironmentValue		= 0.1f;
	const REAL64 c_fBudgetEducationValue		= 0.23f;
	const REAL64 c_fBudgetTourismValue			= 0.01f;
	const REAL64 c_fBudgetCalculatingConstant = 0.15f; // taxe at 30% / 4  * 2

	//Enroll population constant
	const REAL32 c_fPourcPopulationAbleToEnroll = 0.75f;

	//Value of a region in money
	const REAL64 c_fRegionValueModifier = 1000.f;

	const REAL32 c_fRelationsGainTrade = 50.f;

	//Number of days before resetting the last nuke
	const REAL64 c_fDaysBeforeResettingNukeHistory = 180.f;
	//Loss of relations when a country is the first to use nukes
	const REAL32 c_fLossOfRelationsWhenFirstUsingNukes = 40.f;

	//Internal laws constant
	const REAL32 c_fPolygamyBirthRateChange	= 1.2f;
	const REAL32 c_fContraceptionBirthRateChange	= 2.f;
	const REAL32 c_fAbortionBirthRateChange	= 0.75f;
	const REAL32 c_fNbChildrenBirthRateChange	= 0.5f;
	const REAL64 c_fProductionBonusChildLabour = 1.1f;
	const REAL32 c_fFreedomOfSpeechStabilityBonus = 0.02f;
	const REAL32 c_fFreedomOfDemonstrationStabilityBonus = 0.03f;
	const REAL32 c_fPropagandaEffectFreedomOfSpeech	 = 2.f;
	const REAL32 c_fPropagandaEffectFreedomOfDemonstration = 2.f;

	//
	//Government type constants
	//
	//Production modifier
   const REAL64 c_pGvtTypeProductionModifier[EGovernmentType::ItemCount] = 
   {
      0.f,
      0.f,     //anarchy
      0.f,     //conquered
      0.f,     //protectorat
      0.75f,   //theocracy 
      0.4f,    //military dictatorship
      0.7f,    //monarchy
      0.6f,    //communism
      0.8f,    //singlepartydemocracy
      1.f,     //multipartydemocracy
   };	
	//Demand modifier
   const REAL64 c_pGvtTypeDemandModifier[EGovernmentType::ItemCount] = 
   {
      0.f,
      0.4f,     //anarchy
      0.f,     //conquered
      0.f,     //protectorat
      0.75f,   //theocracy 
      0.4f,    //military dictatorship
      0.75f,    //monarchy
      0.5f,    //communism
      0.8f,    //singlepartydemocracy
      1.f,     //multipartydemocracy
   };		
	//Corruption modifier
	const REAL32 c_pGvtTypeCorruptionModifier[EGovernmentType::ItemCount] = 
   {
      0.f,
      2.f,     //anarchy
      0.f,     //conquered
      0.f,     //protectorat
      0.4f,   //theocracy 
      1.f,    //military dictatorship
      0.6f,    //monarchy
      0.5f,    //communism
      0.75f,    //singlepartydemocracy
      0.2f,     //multipartydemocracy
   };	
	//Coup d'Etat modifier
	const REAL32 c_pGvtTypeCoupEtatModifier[EGovernmentType::ItemCount] = 
   {
      0.f,
      2.f,     //anarchy
      0.f,     //conquered
      0.f,     //protectorat
      0.8f,   //theocracy 
      1.f,    //military dictatorship
      0.6f,    //monarchy
      0.4f,    //communism
      0.2f,    //singlepartydemocracy
      0.1f,     //multipartydemocracy
   };	
	
	//Military Impact modifier	
	const REAL32 c_pGvtTypeMilitaryModifier[EGovernmentType::ItemCount] = 
   {
      0.f,
      0.f,     //anarchy
      0.f,     //conquered
      0.f,     //protectorat
      0.25f,   //theocracy 
      -0.25f,    //military dictatorship
      0.4f,    //monarchy
      1.f,    //communism
      0.75f,    //singlepartydemocracy
      1.f,     //multipartydemocracy
   };	
	//Approval mod when declaring war
	const REAL32 c_pGvtTypeApprovalWarModifier[EGovernmentType::ItemCount] = 
   {
      0.f,
      1.f,     //anarchy
      0.f,     //conquered
      0.f,     //protectorat
      0.3f,   //theocracy 
      0.2f,    //military dictatorship
      0.4f,    //monarchy
      0.6f,    //communism
      0.6f,    //singlepartydemocracy
      1.f,     //multipartydemocracy
   };		
	//Government spendings mod
	const REAL32 c_pGvtTypeGvtSpendingsModifier[EGovernmentType::ItemCount] = 
   {
      0.f,
      0.f,     //anarchy
      0.f,     //conquered
      0.f,     //protectorat
      0.7f,   //theocracy 
      0.6f,    //military dictatorship
      0.7f,    //monarchy
      1.f,    //communism
      0.7f,    //singlepartydemocracy
      0.9f,     //multipartydemocracy
   };		
	//Military dictatorship specials
	const REAL32 c_fGvtTypeMilitaryUpkeepRebates = 0.75f;
	const REAL32 c_fGvtTypeMilitaryChangeStatusBonus = 0.5f;
	
	const REAL64 c_fRelationRateIterationPeriod = 3650.f; //10 years
	const REAL64 c_fApprovalIterationPeriod = 180.f; // 6 months
	const REAL64 c_fStabilityIterationPeriod = 730.f; // 2 years
	const REAL64 c_fCorruptionIterationPeriod = 1095.f; // 3 years
	const REAL64 c_fTelecomIterationPeriod = 1460.f; // 4 years
	const REAL64 c_fInfrastrucutureIterationPeriod = 1460.f; // 4 years
	const REAL64 c_fBirthRateIterationPeriod = 1460.f; // 4 years
	const REAL64 c_fDeathRateIterationPeriod = 1460.f; // 4 years
	const REAL64 c_fEmigrationRateIterationPeriod = 120.f; // 4 months
	const REAL64 c_fInflationRateIterationPeriod = 120.f; // 4 months

   //Superpower 2 Earth dimensions
   //----------------------------------------------------
   const REAL32 EARTH_CIRCONFERENCE_AT_EQUATOR = 40074.78f;

   //Superpower 2 Military constants
   const REAL32 c_fConquerThreshold = 0.8f;
   const UINT32 c_iMaxResearchLevel =   10;

   //Superpower2 time variables
   //----------------------------------------------------
   CWSTR c_sStartingYearCfg = L"Starting_Year";
   CWSTR c_sStartingMonthCfg = L"Starting_Month";
   CWSTR c_sStartingDayCfg = L"Starting_Day";
   CWSTR c_sStartingHourCfg = L"Starting_Hour";

   //Game speed constants
   const REAL64 c_fSP2GamePausedSpeed  = 0;
   const REAL64 c_fSP2SlowestTimeSpeed = 1.   /  45.; //in days per second
   const REAL64 c_fSP2NormalTimeSpeed  = 1.   /  10.; 
   const REAL64 c_fSP2HighTimeSpeed    = 1.   /   2.; 
   const REAL64 c_fSP2HighestTimeSpeed = 1.   /  1.; 

   const REAL64 c_fSP2NormalTimeSpeedInv   = 1. / c_fSP2NormalTimeSpeed;
   const REAL64 c_fSP2TimeToTenthOfSecs    = 24. * 60. * 60. * 10.; // In 1/10th of secs per days
//   const INT32 SP2_TIME_SCALE_DEFAULT  = 87600; //In tenths of seconds //1hr = 1year
   const REAL64 c_fDaysForRegionAnnex  = 180.f;

   const UINT32 c_iCombatSimulationSlowDownFactor = 4;
   // Various other constants
   //-----------------------------------------------------
   CWSTR LOCAL_IP_ADDRESS            = L"127.0.0.1";


   //////////////////////////////////////////////////////////////
   //NODE VALUES INSIDE THE CLIENT XML CONFIG FILE
   //////////////////////////////////////////////////////////////
   CWSTR DIALOG_DEFINITION_FILE  = L"Dialog_Definition_File";
   CWSTR c_sBkgndNode            = L"Background_Earth";
   CWSTR DEFAULT_DIALOG_DEF_FILE = L"sp2_dialog.xml";
   CWSTR MAP_PATH                = L"Map_Data_Path";
   CWSTR ADMIN_MAP_CONFIG        = L"Admin_Config";
   CWSTR ELEVATION_MAP_CONFIG    = L"Elevation_Config";
   CWSTR WATER_MAP_CONFIG        = L"Water_Config";
   CWSTR COASTLINES_MAP_CONFIG   = L"Coastlines_Config";
   CWSTR RIVER_MAP_CONFIG        = L"Rivers_Config";
   CWSTR ROAD_MAP_CONFIG         = L"Roads_Config";
   CWSTR LAND_COVER_MAP_CONFIG   = L"Land_Cover_Config";
   CWSTR REGION_GRAPH            = L"Region_Graph";
   CWSTR FLAG_FOLDER             = L"Flag_Path";
   CWSTR FLAG_DEFAULT_FOLDER     = L"/data/textures/flags/";
   CWSTR FLAG_SMALL_FOLDER       = L"Flag_Small_Path";
   CWSTR FLAG_SMALL_DEFAULT_FOLDER = L"/data/textures/flags/small/";
   CWSTR ADVISOR_POLITIC_NODE    = L"Advisor_Politic";
   CWSTR ADVISOR_POLITIC_DEFAULT = L"Advisor.XML";

//Possible Optimizations
   CWSTR DISPLAY_ADVISOR_NODE    = L"Display_Advisor";
   CWSTR LOAD_MAP_ENGINE_NODE    = L"Load_Map_Engine";
	CWSTR SAVE_COUNTRY_DATA_NODE  = L"Save_Country_Data";
   CWSTR LOAD_REGION_TEXTURES    = L"Load_Region_Textures";
   CWSTR PLAY_MUSIC              = L"Play_Music";
   CWSTR PLAY_INTRO              = L"Play_Intro";
   CWSTR INTRO                   = L"Intro";

//Game Options
	CWSTR PLAYER_NAME					= L"Player_Name";
   CWSTR GEOMETRIC_DETAIL			= L"Geometric_Detail";
	CWSTR ZOOM_LEVEL					= L"Zoom_Level";
   CWSTR TEXTURE_DETAIL				= L"Texture_Detail";
   CWSTR CLOUDS						= L"Clouds";
   CWSTR SUN							= L"Sun";
   CWSTR MOON							= L"Moon";
	CWSTR DAY_LIGHT_CYCLE         = L"Day_Light_Cycle";
	CWSTR AUDIO_VOLUME            = L"Audio_Volume";
	CWSTR SOUND_VOLUME            = L"Sound_Volume";
   //AutoSave Option
   CWSTR AUTOSAVE_ENABLED        = L"Autosave_Enabled";
   CWSTR AUTOSAVE_INCREMENTAL    = L"Autosave_Incremental";
   CWSTR AUTOSAVE_FREQUENCY      = L"Autosave_Frequency";
   CWSTR AUTOSAVE_FILENAME       = L"Autosave_Filename";
   CWSTR AUTOSAVE_COUNTER        = L"Autosave_Counter";

//////////////////////////////////////////////////////////////
// TAGS IN THE SERVER XML CONFIG FILE
//////////////////////////////////////////////////////////////
   CWSTR AI                    = L"AI";
   CWSTR ITERATION_PERIOD      = L"ITERATION_PERIOD";
	CWSTR COMBAT_ITERATION_PERIOD = L"COMBAT_ITERATION_PERIOD";
   CWSTR ENTITIES_FILE         = L"ENTITIES_FILE";
   CWSTR MILITARY_NODE         = L"Military";
   CWSTR UNIT_TYPES_NODE        = L"UnitTypes";
   CWSTR UNIT_DESIGN_NODE        = L"UnitDesign";

   CWSTR SERVER_GAMEPLAY_CONSTANTS_NODE = L"Gameplay";
   CWSTR AMDS_RESEARCH_COST_NODE        = L"AMDS_RESEARCH_COST";
   CWSTR AMDS_RESEARCH_TIME_NODE        = L"AMDS_RESEARCH_TIME";
   CWSTR AMDS_LONG_RANGE_SUCCESS_NODE   = L"AMDSLongRangeSuccessChances";
   CWSTR AMDS_SHORT_RANGE_SUCCESS_NODE  = L"AMDSShortRangeSuccessChances";
   CWSTR NUCLEAR_MISSILE_SPEED_NODE     = L"NukeSpeed";
   CWSTR SELLING_UNIT_COST_RATIO        = L"SellingUnitCostRatio";
	CWSTR AI_AGGRESSIVENESS					 = L"AI_AGGRESSIVENESS";

   CWSTR PRODUCTION_TABLE_NODE           = L"ProductionTable";

   CWSTR MILITARY_RESEARCH               = L"Research";
   CWSTR MILITARY_RESEARCH_DAYS          = L"NbDays";
   CWSTR MILITARY_RESEARCH_DAYS_LVL      = L"Lvl";
   CWSTR MILITARY_RESEARCH_NORMAL_BUDGET = L"NormalResearchBudget";


//////////////////////////////////////////////////////////////
// TAGS IN THE COMMON XML CONFIG FILE
//////////////////////////////////////////////////////////////
   CWSTR  INFANTRY_VALUE_NODE           = L"InfantryValue";
   const  UINT32 c_iInfantryDefaultValue       = 5000;

   const GString c_sDefaultFont = "Lucida Sans Unicode";
   const UINT32  c_iDefaultFontSize = 9;

//////////////////////////////////////////////////////////////
//DIALOGS DEFINITIONS
//////////////////////////////////////////////////////////////
   const INT32 CONTEXT_MENU_BORDER_SIZE      = 3;

   CWSTR SP2_SELECTED_COUNTRY_WINDOW_MORE_INFO_BTN = L"btnCountryMoreInfo";

   //! Map painting values
const REAL32        SP2_POPULATION_DENSITY_CONSTANT    = 25.0f;
const REAL32        SP2_POPULATION_DENSITY_HIGHEST_LIMIT= 50000;
const GColorRGBReal SP2_PAINT_MAP_LOW_VALUE_COLOR      = GColorRGBReal(0.0f,0.0f,1.0f,1.0f);
const GColorRGBReal SP2_PAINT_MAP_HIGH_VALUE_COLOR     = GColorRGBReal(1.0f,0.0f,0.0f,1.0f);

   //! ERROR MESSAGES
   CWSTR ERROR_CANT_FIND_FILE    =  L"Cannot find SP2 file: ";
   CWSTR ERROR_EXTRACT_FILE      =  L"Error extracting file: ";
   CWSTR ERROR_PARSING_XML_FILE  =  L"Error parsing the xml file: ";
   CWSTR ERROR_CANT_PROCESS_FILE =  L"Cannot process file : ";
   CWSTR ERROR_INIT_MAP_ENGINE   =  L"Error loading the map engine: ";


   //! WARNING MESSAGES
   CWSTR WARNING_DIALOG_FILE_OK =   L"Strange, there is a missing object in the dialog xml file : ";

   //////////////////////////////////////////////////////////////////////////////
   //                         DEFAULT VALUES
   //////////////////////////////////////////////////////////////////////////////
   CWSTR DEFAULT_MAX_PLAYERS      = L"8";
   CWSTR DEFAULT_GAME_TIME_LIMIT  = L"0";
   CWSTR DEFAULT_MINIMUM_PING     = L"0";
   const UINT32 c_iDefaultServerNameStrId = 101097;


   //Combat Efficiency table file
   CWSTR COMBAT_EFFICIENCY_TABLE_FILE = L"/data/military/efficiency.dat";
   //UnitType characteristicBy level file
   CWSTR UNIT_TYPE_CHARACTERISTIC_BY_LEVEL_FILE = L"/data/military/types_characteristics.xml";


   CWSTR c_sBombardmentPointsFile = L"/data/military/bombard.bin";




   //Max Characteristic level
   const INT8  MAX_CHARACTERISTIC_LEVEL  = 10;


   const UINT32 c_iBattleArenaSize = 100;


   const UINT32 c_iMilitaryUnitsNuclearTargetCostRatio = 100000;


   //Ballistic Missile Submarine Unit Type ID
   const UINT32 c_iNuclearSubmarineUnitTypeID = 18;


	//Random Events variables
	const INT64		c_iMinimumPeopleKilledInAvalanche = 5;
	const REAL32	c_fVariationPeopleKilledInAvalanche = 5.f;
	const INT64		c_iMinimumPeopleKilledInBlizzard = 10;
	const REAL32	c_fVariationPeopleKilledInBlizzard = 10.f;
	const REAL64	c_fPercCerealsChangeInBlizzard = -0.02f;
	const REAL64	c_fPercMeatChangeInBlizzard = -0.02f;
	const REAL64	c_fAmountChangePerDeadInBlizzard = -100000.f;		
	const REAL64	c_fPercCerealsChangeInColdWave = -0.02f;
	const REAL64	c_fPercMeatChangeInColdWave = -0.02f;
	const REAL64	c_fPercCerealsChangeInCyclone = -0.02f;
	const REAL32	c_fPercInfrastructureChangeInCyclone = -0.05f;
	const INT64		c_iMinimumPeopleKilledInCyclone = 5;
	const REAL32	c_fVariationPeopleKilledInCyclone = 5.f;
	const REAL64	c_fAmountChangePerDeadInCyclone = -1000000.f;
	const REAL64	c_fPercCerealsChangeInDrought = -0.02f;
	const REAL64	c_fPercMeatChangeInDrought = -0.02f;
	const REAL64	c_fAmountChangeInDrought = -5000000.f;
	const REAL64	c_fPercAllResourceChangeInEarthquake = -0.02f;
	const INT64		c_iMinimumPeopleKilledInEarthquake = 10;
	const REAL32	c_fVariationPeopleKilledInEarthquake = 1000.f;
	const REAL64	c_fAmountChangePerDeadInEarthquake = -1000000.f;
	const REAL64	c_fPercCerealsChangeInFlood = -0.02f;
	const REAL64	c_fPercMeatChangeInFlood = -0.02f;
	const INT64		c_iMinimumPeopleKilledInFlood = 2;
	const REAL32	c_fVariationPeopleKilledInFlood = 10.f;
	const REAL64	c_fAmountChangePerDeadInFlood = -1000000.f;
	const INT64		c_iMinimumPeopleKilledInHeatWave = 2;
	const REAL32	c_fVariationPeopleKilledInHeatWave = 10.f;
	const REAL64	c_fAmountChangePerDeadInHeatWave = -100000.f;	
	const REAL64	c_fPercCerealsChangeInHurricane = -0.02f;
	const REAL32	c_fPercInfrastructureChangeInHurricane = -0.05f;
	const INT64		c_iMinimumPeopleKilledInHurricane = 5;
	const REAL32	c_fVariationPeopleKilledInHurricane = 5.f;
	const REAL64	c_fAmountChangePerDeadInHurricane = -1000000.f;
	const REAL64	c_fPercCerealsChangeInInsects = -0.05f;
	const REAL64	c_fPercMeatChangeInInsects = -0.02f;
	const REAL64	c_fAmountChangeInInsects = -2500000.f;
	const REAL32	c_fPercInfrastructureChangeInLandslide = -0.02f;
	const INT64		c_iMinimumPeopleKilledInLandslide = 2;
	const REAL32	c_fVariationPeopleKilledInLandslide = 10.f;
	const REAL64	c_fAmountChangePerDeadInLandslide = -100000.f;
	const REAL32	c_fPercInfrastructureChangeInStorm = -0.02f;
	const INT64		c_iMinimumPeopleKilledInStorm = 0;
	const REAL32	c_fVariationPeopleKilledInStorm = 2.f;
	const REAL64	c_fAmountChangePerDeadInStorm = -1000000.f;
	const INT64		c_iMinimumPeopleKilledInTidalWave = 50;
	const REAL32	c_fVariationPeopleKilledInTidalWave = 500.f;
	const REAL64	c_fAmountChangePerDeadInTidalWave = -1000000.f;
	const INT64		c_iMinimumPeopleKilledInTornado = 5;
	const REAL32	c_fVariationPeopleKilledInTornado = 5.f;
	const REAL64	c_fAmountChangePerDeadInTornado = -1000000.f;
	const INT64		c_iMinimumPeopleKilledInTsunami = 25;
	const REAL32	c_fVariationPeopleKilledInTsunami = 250.f;
	const REAL64	c_fAmountChangePerDeadInTsunami = -1000000.f;
	const REAL64	c_fPercCerealsChangeInTyphoon = -0.02f;
	const REAL32	c_fPercInfrastructureChangeInTyphoon = -0.05f;
	const INT64		c_iMinimumPeopleKilledInTyphoon = 5;
	const REAL32	c_fVariationPeopleKilledInTyphoon = 25.f;
	const REAL64	c_fAmountChangePerDeadInTyphoon = -1000000.f;
	const INT64		c_iMinimumPeopleKilledInVolcanic = 0;
	const REAL32	c_fVariationPeopleKilledInVolcanic = 20.f;
	const REAL64	c_fPercWoodChangeInWildfire = -0.05f;
	const INT64		c_iMinimumPeopleKilledInWildfire = 1;
	const REAL32	c_fVariationPeopleKilledInWildfire = 10.f;	
	const REAL64	c_fAmountChangePerDeadInWildfire = -100000.f;
	const REAL64	c_fPercAllResourceChangeInStockMarketCrash = -0.02f;
	const REAL64	c_fPercAllResourceChangeInStockMarketCrashFriend = -0.01f;
	const REAL32	c_fPopulationApprovalChangeInStockMarketCrash = -0.1f;
	const REAL64	c_fPercAllResourceChangeInEconomicDepression = -0.04f;
	const REAL64	c_fPercAllResourceChangeInEconomicDepressionFriend = -0.01f;
	const REAL32	c_fPopulationApprovalChangeInEconomicDepression = -0.1f;
	const REAL64	c_fPercWealthChangeInEconomicDepression = -0.01f;
	const REAL64	c_fPercWealthChangeInEconomicDepressionFriend = -0.005f;
	const REAL64	c_fPercAllResourceChangeInEconomicBoom = 0.06f;
	const REAL64	c_fPercAllResourceChangeInEconomicBoomFriend = 0.02f;
	const REAL32	c_fPopulationApprovalChangeInEconomicBoom = 0.1f;
	const REAL64	c_fPercWealthChangeInEconomicBoom = 0.02f;
	const REAL64	c_fPercWealthChangeInEconomicBoomFriend = 0.01f;
	const REAL64	c_fPercCerealsChangeInPoorHarvest = -0.05f;
	const REAL64	c_fPercMeatChangeInPoorHarvest = -0.02f;
	const REAL64	c_fPercDairyChangeInPoorHarvest = -0.02f;
	const REAL64	c_fPercVegetablesChangeInPoorHarvest = -0.02f;
	const REAL64	c_fPercCerealsChangeInBountifulHarvest = 0.06f;
	const REAL64	c_fPercMeatChangeInBountifulHarvest = 0.03f;
	const REAL64	c_fPercDairyChangeInBountifulHarvest = 0.03f;
	const REAL64	c_fPercMeatChangeInLivestockEpidemic = -0.04f;
	const REAL64	c_fPercDairyChangeInLivestockEpidemic = -0.02f;
	const REAL64	c_fPercWealthChangeInLivestockEpidemic = -0.005f;
	const REAL32	c_fPopulationApprovalChangeInLivestockEpidemic = -0.05f;
	const REAL32	c_fPopulationApprovalChangeInResourceEnergy = 0.07f;
	const REAL64	c_fFixChangeInResourceProductionEnergyFossileFuels = 15000000.f;
	const REAL64	c_fFixChangeInResourceProductionEnergyElectricity =  10000000.f;
	const REAL32	c_fPopulationApprovalChangeInResourceOre = 0.07f;
	const REAL64	c_fFixChangeInResourceProductionOreIronSteel = 20000000.f;
	const REAL64	c_fFixChangeInResourceProductionOreMinerals =  10000000.f;
	const REAL64	c_fFixChangeInResourceProductionOrePrecStones = 1000000.f;
	const REAL32	c_fPopulationApprovalChangeInResourceWealth = 0.07f;
	const REAL64	c_fFixChangeInResourceProductionWealth = 100000000.f;
	const INT64		c_iMinimumPeopleKilledInEpidemic = 50;
	const REAL32	c_fVariationPeopleKilledInEpidemic = 500.f;
	const INT64		c_iMinimumPeopleKilledInFamine = 10;
	const REAL32	c_fVariationPeopleKilledInFamine = 50.f;
	const REAL64	c_fAmountChangePerDeadInEpidemic = -1000000.f;
	const REAL32	c_fPopulationApprovalChangeInEpidemic = -0.1f;
	const REAL32	c_fPopulationBoostInPopulationBoom = 0.02f;
	const REAL64	c_fPercCerealsChangeInFamine = -0.05f;
	const REAL64	c_fPercMeatChangeInFamine = -0.05f;
	const REAL64	c_fPercDairyChangeInFamine = -0.05f;
	const REAL64	c_fPercVegetablesChangeInFamine = -0.05f;
	const REAL32	c_fPopulationApprovalChangeInFamine = -0.15f;
	const REAL64	c_fPercElectricityChangeInNuclearMeltdown = -0.05f;
	const REAL64	c_fAmountChangeInNuclearMeltdown = -500000000.f;	
	const REAL32	c_fRelationsLossInNuclearMeltdown = -5.f;
	const REAL64	c_fPercElectricityChangeInChemicalSpill = -0.02f;
	const REAL64	c_fPercChemicalsChangeInChemicalSpill = -0.02f;
	const REAL64	c_fAmountChangeInChemicalSpill = -250000000.f;	
	const REAL64	c_fPercElectricityChangeInIndustrialAccident = -0.02f;
	const REAL64	c_fPercChemicalsChangeInIndustrialAccident = -0.02f;
	const REAL64	c_fAmountChangeInIndustrialAccident = -50000000.f;	
	const REAL32	c_fPopulationApprovalChangeInAntiGlobalProtest = -0.1f;
	const REAL32	c_fPopulationStabilityChangeInAntiGlobalProtest = -0.1f;
   const REAL32   c_fNukeLaunchDelaySeconds                       = 2.0f;;


   //////////////////////////////////////////////
   //       BOUNDARIES
   //////////////////////////////////////////////
   const REAL32		InterestRate_LowerCap				= 0.00f;
	const REAL32		InterestRate_UpperCap				= 1.f;
	const REAL32		MinimumWage_LowerCap						= 0.1f;
	const REAL32		MinimumWage_UpperCap						= 20.0f; // do we really need a cap? (Alain: I think we do)
	const REAL32		PersonalTaxes_LowerCap					= 0.f;
	const REAL32		PersonalTaxes_UpperCap					= 1.f;


   //////////////////////////////////////////////////////////////////////////////
   //                        MILITARY DESIGNS CONSTANTS
   //////////////////////////////////////////////////////////////////////////////

   const int c_iMaxDesignPieceCount = 3;

   const int c_iNbMilitaryColors = 8;
   const GColorRGBInt c_MilitaryColorPalette[c_iNbMilitaryColors] = 
   {
      GColorRGBInt( 66,  66,  66, 255), // Black;
      GColorRGBInt( 35,  70,  45, 255), // Forest;
      GColorRGBInt( 24,  65,  24, 255), // Olive;
      GColorRGBInt( 72,  54,  28, 255), // Brown;
      GColorRGBInt( 86,  42,  14, 255), // Red;
      GColorRGBInt(140,  96,  19, 255), // Yellow;
      GColorRGBInt(168, 137,  96, 255), // Sand;
      GColorRGBInt(128, 128, 128, 255), // Gray;
   };

   //////////////////////////////////////////////////////////////////////////////
   //                        HISTORY GRAPH CONSTANTS
   //////////////////////////////////////////////////////////////////////////////

   //Relations history number of values
   const UINT32  c_iRelationHistoryNumberOfValuesToKeep = 20;

   //Update period ( Delay In delay in days between each update)
   const REAL32  c_fRelationHistoryUpdatePeriod = 7.f; //once a week

   // Period between client update of historical data
   const REAL32 c_fHistoryUpdatePeriod = 1.f; // 1 day

   // Number of historical values to remember
   const UINT32 c_iHistoryMaxValues = 365 * 2; // 2 years

   // Lists all tracked history values for graphs
   namespace EHistoryValues
   {
      enum Enum
      {
         // Politic
         Approval,
         Stability,
         Corruption,
         OverallRelations,

         // Military
         ConvForces,
         StratForces,
         Upkeep,
         Research,

         // Economy
         EconomicHealth,
         GDPCapita,
         Budget,
         ResProduction,
         ResBalance,

         // Other
         Population,
         HumanDevel,
         Infrastructures,
         Telecom,

         TotalCount
      };
   }

   // Describe the type of history value
   namespace EHistoryValueType
   {
      enum Enum
      {
         NoUnit,
         Money,
         Percent,
         Relation,
         Research,
      };
   }

   // Associate each a type to each history value
   // Must be in sync with EHistoryValues::Enum content
   const EHistoryValueType::Enum c_HistoryValueTypes[EHistoryValues::TotalCount] =
   {
      EHistoryValueType::Percent,      // Approval
      EHistoryValueType::Percent,      // Stability
      EHistoryValueType::Percent,      // Corruption
      EHistoryValueType::Relation,     // OverallRelations
      EHistoryValueType::Money,        // ConvForces
      EHistoryValueType::Money,        // StratForces
      EHistoryValueType::Money,        // Upkeep
      EHistoryValueType::Research,     // Research
      EHistoryValueType::Percent,      // EconomicHealth
      EHistoryValueType::Money,        // GDPCapita
      EHistoryValueType::Money,        // Budget
      EHistoryValueType::Money,        // ResProduction
      EHistoryValueType::Money,        // ResBalance
      EHistoryValueType::NoUnit,       // Population
      EHistoryValueType::Percent,      // HumanDevel
      EHistoryValueType::Percent,      // Infrastructures
      EHistoryValueType::Percent,      // Telecom
   };

   namespace EHistoryMarkerType
   {
      enum Enum
      {
         GainPoliticalControl,
         LoosePoliticalControl,
         GainMilitaryControl,
         LooseMilitaryControl,
         DeclareWar,
         DeclarePeace,
         ReceiveNuke,
         ChangeGovermentType,
         LaunchNuke,
         IncreaseTaxes,
         DecreaseTaxes,
         CombatStarted,

         IncreaseMinimumWage,
         DecreaseMinimumWage,
         PoliticPartyLegal,
         PoliticPartyIllegal,
         ReligionLanguageLegal,
         ReligionLanguageIllegal,
         ReligionLanguageOfficial,
         DeclareMartialLaw,
         RemoveMartialLaw,
         CovertOpSuccess,
         CovertOpFailure,
         CovertOpVictim,
         IncreaseResourceProduction,
         IncreaseInterestLevel,
         DecreaseInterestLevel,
         ResourceIsPublic,
         ResourceIsPrivate,
         RandomEvent,
         TreatySigned,
         TreatyCanceled,
         IncreaseMilitaryArsenal,
         DecreaseMilitaryArsenal,
         ParkUnit,
         FortifyUnit,
         ReadyUnit,
         IncreaseResourceTax,
         DecreaseResourceTax,
         TrainUnit,
         ModifyLaw,
         IncreaseGlobalResourceTax,
         DecreaseGlobalResourceTax,

         TotalCount
      };
   }

   namespace EHistoryMarkerValueType
   {
      enum Enum
      {
         None,
         StringID,
         NumberReal,
         NumberInt,
      };
   }

   // This array MUST be in sync with EHistoryMarkerType content
   const EHistoryMarkerValueType::Enum c_HistoryMarkerValueTypes[EHistoryMarkerType::TotalCount] =
   {
      EHistoryMarkerValueType::NumberInt,       // GainPoliticalControl
      EHistoryMarkerValueType::NumberInt,       // LoosePoliticalControl
      EHistoryMarkerValueType::NumberInt,       // GainMilitaryControl
      EHistoryMarkerValueType::NumberInt,       // LooseMilitaryControl
      EHistoryMarkerValueType::NumberInt,       // DeclareWar
      EHistoryMarkerValueType::NumberInt,       // DeclarePeace
      EHistoryMarkerValueType::NumberInt,       // ReceiveNuke
      EHistoryMarkerValueType::StringID,        // ChangeGovermentType
      EHistoryMarkerValueType::NumberInt,       // LaunchNuke
      EHistoryMarkerValueType::NumberReal,      // IncreaseTaxes
      EHistoryMarkerValueType::NumberReal,      // DecreaseTaxes
      EHistoryMarkerValueType::NumberInt,       // CombatStarted
                                                
      EHistoryMarkerValueType::NumberReal,      // IncreaseMinimumWage
      EHistoryMarkerValueType::NumberReal,      // DecreaseMinimumWage
      EHistoryMarkerValueType::StringID,        // PoliticPartyLegal
      EHistoryMarkerValueType::StringID,        // PoliticPartyIllegal
      EHistoryMarkerValueType::StringID,        // ReligionLanguageLegal
      EHistoryMarkerValueType::StringID,        // ReligionLanguageIllegal
      EHistoryMarkerValueType::StringID,        // ReligionLanguageOfficial
      EHistoryMarkerValueType::None,            // DeclareMartialLaw
      EHistoryMarkerValueType::None,            // RemoveMartialLaw
      EHistoryMarkerValueType::StringID,        // CovertOpSuccess
      EHistoryMarkerValueType::StringID,        // CovertOpFailure
      EHistoryMarkerValueType::StringID,        // CovertOpVictim
      EHistoryMarkerValueType::NumberReal,      // IncreaseResourceProduction
      EHistoryMarkerValueType::NumberReal,      // IncreaseInterestLevel
      EHistoryMarkerValueType::NumberReal,      // DecreaseInterestLevel
      EHistoryMarkerValueType::NumberReal,      // ResourceIsPublic
      EHistoryMarkerValueType::NumberReal,      // ResourceIsPrivate
      EHistoryMarkerValueType::StringID,        // RandomEvent
      EHistoryMarkerValueType::StringID,        // TreatySigned
      EHistoryMarkerValueType::StringID,        // TreatyCanceled
      EHistoryMarkerValueType::NumberReal,      // IncreaseMilitaryArsenal
      EHistoryMarkerValueType::NumberReal,      // DecreaseMilitaryArsenal
      EHistoryMarkerValueType::NumberReal,      // ParkUnit
      EHistoryMarkerValueType::NumberReal,      // FortifyUnit
      EHistoryMarkerValueType::NumberReal,      // ReadyUnit
      EHistoryMarkerValueType::NumberReal,      // IncreaseResourceTax
      EHistoryMarkerValueType::NumberReal,      // DecreaseResourceTax
      EHistoryMarkerValueType::NumberReal,      // TrainUnit
      EHistoryMarkerValueType::StringID,        // ModifyLaw
      EHistoryMarkerValueType::NumberReal,      // IncreaseGlobalResourceTax
      EHistoryMarkerValueType::NumberReal,      // DecreaseGlobalResourceTax
   };

   // This array MUST be in sync with EHistoryMarkerType content
   const bool c_HistoryMarkerIsMergeable[EHistoryMarkerType::TotalCount] =
   {
      true,    // GainPoliticalControl
      true,    // LoosePoliticalControl
      true,    // GainMilitaryControl
      true,    // LooseMilitaryControl
      true,    // DeclareWar
      true,    // DeclarePeace
      true,    // ReceiveNuke
      false,   // ChangeGovermentType
      true,    // LaunchNuke
      true,    // IncreaseTaxes
      true,    // DecreaseTaxes
      true,    // CombatStarted

      true,    // IncreaseMinimumWage
      true,    // DecreaseMinimumWage
      false,   // PoliticPartyLegal
      false,   // PoliticPartyIllegal
      false,   // ReligionLanguageLegal
      false,   // ReligionLanguageIllegal
      false,   // ReligionLanguageOfficial
      false,   // DeclareMartialLaw
      false,   // RemoveMartialLaw
      false,   // CovertOpSuccess
      false,   // CovertOpFailure
      false,   // CovertOpVictim
      true,    // IncreaseResourceProduction
      true,    // IncreaseInterestLevel
      true,    // DecreaseInterestLevel
      true,    // ResourceIsPublic
      true,    // ResourceIsPrivate
      false,   // RandomEvent
      false,   // TreatySigned
      false,   // TreatyCanceled
      true,    // IncreaseMilitaryArsenal
      true,    // DecreaseMilitaryArsenal
      true,    // ParkUnit
      true,    // FortifyUnit
      true,    // ReadyUnit
      true,    // IncreaseResourceTax
      true,    // DecreaseResourceTax
      true,    // TrainUnit
      false,   // ModifyLaw
      true,    // IncreaseGlobalResourceTax
      true,    // DecreaseGlobalResourceTax
   };

   const UINT32 c_HistoryChangeGovermentTypeMarkerValueCount = 7;
   const UINT32 c_HistoryIncreaseTaxesMarkerValueCount = 4;
   const UINT32 c_HistoryDecreaseTaxesMarkerValueCount = 4;
   const UINT32 c_HistoryDeclareWarMarkerValueCount = 4;
   const UINT32 c_HistoryDeclarePeaceMarkerValueCount = 4;
   const UINT32 c_HistoryLaunchNukeMarkerValueCount = 2;
   const UINT32 c_HistoryReceiveNukeMarkerValueCount = 6;
   const UINT32 c_HistoryCombatStartedMarkerValueCount = 3;
   const UINT32 c_HistoryGainMilitaryControlMarkerValueCount = 4;
   const UINT32 c_HistoryLooseMilitaryControlMarkerValueCount = 4;
   const UINT32 c_HistoryGainPoliticalControlMarkerValueCount = 6;
   const UINT32 c_HistoryLoosePoliticalControlMarkerValueCount = 6;

   const UINT32 c_HistoryIncreaseMinimumWageMarkerValueCount = 3;
   const UINT32 c_HistoryDecreaseMinimumWageMarkerValueCount = 3;
   const UINT32 c_HistoryPoliticPartyLegalMarkerValueCount = 2;
   const UINT32 c_HistoryPoliticPartyIllegalMarkerValueCount = 2;
   const UINT32 c_HistoryReligionLanguageLegalMarkerValueCount = 3;
   const UINT32 c_HistoryReligionLanguageIllegalMarkerValueCount = 3;
   const UINT32 c_HistoryReligionLanguageOfficialMarkerValueCount = 3;
   const UINT32 c_HistoryDeclareMartialLawMarkerValueCount = 3;
   const UINT32 c_HistoryRemoveMartialLawMarkerValueCount = 3;
   const UINT32 c_HistoryCovertOpSuccessMarkerValueCount = 5;
   const UINT32 c_HistoryCovertOpFailureMarkerValueCount = 1;
   const UINT32 c_HistoryCovertOpVictimMarkerValueCount = 5;
   const UINT32 c_HistoryIncreaseResourceProductionMarkerValueCount = 5;
   const UINT32 c_HistoryIncreaseInterestLevelMarkerValueCount = 5;
   const UINT32 c_HistoryDecreaseInterestLevelMarkerValueCount = 5;
   const UINT32 c_HistoryResourceIsPublicMarkerValueCount = 4;
   const UINT32 c_HistoryResourceIsPrivateMarkerValueCount = 4;
   const UINT32 c_HistoryRandomEventMarkerValueCount = 6;
   const UINT32 c_HistoryTreatySignedMarkerValueCount = 8;
   const UINT32 c_HistoryTreatyCanceledMarkerValueCount = 8;
   const UINT32 c_HistoryIncreaseMilitaryArsenalMarkerValueCount = 4;
   const UINT32 c_HistoryDecreaseMilitaryArsenalMarkerValueCount = 4;
   const UINT32 c_HistoryParkUnitMarkerValueCount = 2;
   const UINT32 c_HistoryFortifyUnitMarkerValueCount = 2;
   const UINT32 c_HistoryReadyUnitMarkerValueCount = 2;
   const UINT32 c_HistoryIncreaseResourceTaxMarkerValueCount = 4;
   const UINT32 c_HistoryDecreaseResourceTaxMarkerValueCount = 4;
   const UINT32 c_HistoryTrainUnitMarkerValueCount = 3;
   const UINT32 c_HistoryModifyLawMarkerValueCount = 4;
   const UINT32 c_HistoryIncreaseGlobalResourceTaxMarkerValueCount = 4;
   const UINT32 c_HistoryDecreaseGlobalResourceTaxMarkerValueCount = 4;

   const char c_UnitTypeXMLName[20][32] = 
   {
      "Infantry",
      "InfantryVehicle",
      "AirDefense",
      "MobileLauncher",
      "Tank",
      "ArtilleryGun",
      "AttackHelicopter",
      "TransportHelicopter",
      "ASWHelicopter",
      "FighterAircraft",
      "AttackAircraft",
      "Bomber",
      "PatrolCraft",
      "Corvette",
      "Frigate",
      "Destroyer",
      "AttackSubmarine",
      "BallisticMissileSubmarine",
      "AircraftCarrier",
      "BallisticMissile"
   };



   // Associates markers with number of values
   // This array MUST be in sync with EHistoryMarkerType content
   const UINT32 c_HistoryMarkerValueCount[EHistoryMarkerType::TotalCount] = 
   {
      c_HistoryGainPoliticalControlMarkerValueCount,
      c_HistoryLoosePoliticalControlMarkerValueCount,
      c_HistoryGainMilitaryControlMarkerValueCount,
      c_HistoryLooseMilitaryControlMarkerValueCount,
      c_HistoryDeclareWarMarkerValueCount, 
      c_HistoryDeclarePeaceMarkerValueCount, 
      c_HistoryReceiveNukeMarkerValueCount, 
      c_HistoryChangeGovermentTypeMarkerValueCount,
      c_HistoryLaunchNukeMarkerValueCount, 
      c_HistoryIncreaseTaxesMarkerValueCount, 
      c_HistoryDecreaseTaxesMarkerValueCount, 
      c_HistoryCombatStartedMarkerValueCount,

      c_HistoryIncreaseMinimumWageMarkerValueCount,
      c_HistoryDecreaseMinimumWageMarkerValueCount,
      c_HistoryPoliticPartyLegalMarkerValueCount,
      c_HistoryPoliticPartyIllegalMarkerValueCount,
      c_HistoryReligionLanguageLegalMarkerValueCount,
      c_HistoryReligionLanguageIllegalMarkerValueCount,
      c_HistoryReligionLanguageOfficialMarkerValueCount,
      c_HistoryDeclareMartialLawMarkerValueCount,
      c_HistoryRemoveMartialLawMarkerValueCount,
      c_HistoryCovertOpSuccessMarkerValueCount,
      c_HistoryCovertOpFailureMarkerValueCount,
      c_HistoryCovertOpVictimMarkerValueCount,
      c_HistoryIncreaseResourceProductionMarkerValueCount,
      c_HistoryIncreaseInterestLevelMarkerValueCount,
      c_HistoryDecreaseInterestLevelMarkerValueCount,
      c_HistoryResourceIsPublicMarkerValueCount,
      c_HistoryResourceIsPrivateMarkerValueCount,
      c_HistoryRandomEventMarkerValueCount,
      c_HistoryTreatySignedMarkerValueCount,
      c_HistoryTreatyCanceledMarkerValueCount,
      c_HistoryIncreaseMilitaryArsenalMarkerValueCount,
      c_HistoryDecreaseMilitaryArsenalMarkerValueCount,
      c_HistoryParkUnitMarkerValueCount,
      c_HistoryFortifyUnitMarkerValueCount,
      c_HistoryReadyUnitMarkerValueCount,
      c_HistoryIncreaseResourceTaxMarkerValueCount,
      c_HistoryDecreaseResourceTaxMarkerValueCount,
      c_HistoryTrainUnitMarkerValueCount,
      c_HistoryModifyLawMarkerValueCount,
      c_HistoryIncreaseGlobalResourceTaxMarkerValueCount,
      c_HistoryDecreaseGlobalResourceTaxMarkerValueCount,
   };

   // Associates ChangeGovermentType marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryChangeGovermentTypeMarkerValues[c_HistoryChangeGovermentTypeMarkerValueCount] = 
   {
      EHistoryValues::Stability,
      EHistoryValues::OverallRelations,
      EHistoryValues::GDPCapita,
      EHistoryValues::Budget,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
      EHistoryValues::HumanDevel,
   };

   // Associates IncreaseTaxes marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryIncreaseTaxesMarkerValues[c_HistoryIncreaseTaxesMarkerValueCount] = 
   {
      EHistoryValues::Approval,
      EHistoryValues::Corruption,
      EHistoryValues::EconomicHealth,
      EHistoryValues::Budget,
   };

   // Associates DecreaseTaxes marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryDecreaseTaxesMarkerValues[c_HistoryDecreaseTaxesMarkerValueCount] = 
   {
      EHistoryValues::Approval,
      EHistoryValues::Corruption,
      EHistoryValues::EconomicHealth,
      EHistoryValues::Budget,
   };

   // Associates DeclareWar marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryDeclareWarMarkerValues[c_HistoryDeclareWarMarkerValueCount] = 
   {
      EHistoryValues::Stability,
      EHistoryValues::OverallRelations,
      EHistoryValues::ResBalance,
      EHistoryValues::HumanDevel,
   };

   // Associates DeclarePeace marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryDeclarePeaceMarkerValues[c_HistoryDeclarePeaceMarkerValueCount] = 
   {
      EHistoryValues::Stability,
      EHistoryValues::OverallRelations,
      EHistoryValues::ResBalance,
      EHistoryValues::HumanDevel,
   };

   // Associates LaunchNuke marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryLaunchNukeMarkerValues[c_HistoryLaunchNukeMarkerValueCount] = 
   {
      EHistoryValues::OverallRelations,
      EHistoryValues::StratForces,
   };

   // Associates ReceiveNuke marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryReceiveNukeMarkerValues[c_HistoryReceiveNukeMarkerValueCount] = 
   {
      EHistoryValues::ConvForces,
      EHistoryValues::Upkeep,
      EHistoryValues::GDPCapita,
      EHistoryValues::Budget,
      EHistoryValues::Population,
      EHistoryValues::HumanDevel,
   };

   // Associates CombatStarted marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryCombatStartedMarkerValues[c_HistoryCombatStartedMarkerValueCount] = 
   {
      EHistoryValues::ConvForces,
      EHistoryValues::Upkeep,
      EHistoryValues::Budget,
   };

   // Associates GainMilitaryControl marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryGainMilitaryControlMarkerValues[c_HistoryGainMilitaryControlMarkerValueCount] = 
   {
      EHistoryValues::GDPCapita,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
      EHistoryValues::HumanDevel,
   };

   // Associates LooseMilitaryControl marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryLooseMilitaryControlMarkerValues[c_HistoryLooseMilitaryControlMarkerValueCount] = 
   {
      EHistoryValues::GDPCapita,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
      EHistoryValues::HumanDevel,
   };

   // Associates GainPoliticalControl marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryGainPoliticalControlMarkerValues[c_HistoryGainPoliticalControlMarkerValueCount] = 
   {
      EHistoryValues::StratForces,
      EHistoryValues::GDPCapita,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
      EHistoryValues::Population,
      EHistoryValues::HumanDevel,
   };

   // Associates LoosePoliticalControl marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryLoosePoliticalControlMarkerValues[c_HistoryLoosePoliticalControlMarkerValueCount] = 
   {
      EHistoryValues::StratForces,
      EHistoryValues::GDPCapita,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
      EHistoryValues::Population,
      EHistoryValues::HumanDevel,
   };

   // Associates IncreaseMinimumWage marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryIncreaseMinimumWageMarkerValues[c_HistoryIncreaseMinimumWageMarkerValueCount] = 
   {
      EHistoryValues::Approval,
      EHistoryValues::EconomicHealth,
      EHistoryValues::Budget,
   };

   // Associates DecreaseMinimumWage marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryDecreaseMinimumWageMarkerValues[c_HistoryDecreaseMinimumWageMarkerValueCount] = 
   {
      EHistoryValues::Approval,
      EHistoryValues::EconomicHealth,
      EHistoryValues::Budget,
   };

   // Associates PoliticPartyLegal marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryPoliticPartyLegalMarkerValues[c_HistoryPoliticPartyLegalMarkerValueCount] = 
   {
      EHistoryValues::Approval,
      EHistoryValues::Stability,
   };

   // Associates PoliticPartyIllegal marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryPoliticPartyIllegalMarkerValues[c_HistoryPoliticPartyIllegalMarkerValueCount] = 
   {
      EHistoryValues::Approval,
      EHistoryValues::Stability,
   };

   // Associates ReligionLanguageLegal marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryReligionLanguageLegalMarkerValues[c_HistoryReligionLanguageLegalMarkerValueCount] = 
   {
      EHistoryValues::Approval,
      EHistoryValues::Stability,
      EHistoryValues::OverallRelations,
   };

   // Associates ReligionLanguageIllegal marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryReligionLanguageIllegalMarkerValues[c_HistoryReligionLanguageIllegalMarkerValueCount] = 
   {
      EHistoryValues::Approval,
      EHistoryValues::Stability,
      EHistoryValues::OverallRelations,
   };

   // Associates ReligionLanguageOfficial marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryReligionLanguageOfficialMarkerValues[c_HistoryReligionLanguageOfficialMarkerValueCount] = 
   {
      EHistoryValues::Approval,
      EHistoryValues::Stability,
      EHistoryValues::OverallRelations,
   };

   // Associates DeclareMartialLaw marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryDeclareMartialLawMarkerValues[c_HistoryDeclareMartialLawMarkerValueCount] = 
   {
      EHistoryValues::Approval,
      EHistoryValues::Stability,
      EHistoryValues::OverallRelations,
   };

   // Associates RemoveMartialLaw marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryRemoveMartialLawMarkerValues[c_HistoryRemoveMartialLawMarkerValueCount] = 
   {
      EHistoryValues::Approval,
      EHistoryValues::Stability,
      EHistoryValues::OverallRelations,
   };

   // Associates CovertOpSuccess marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryCovertOpSuccessMarkerValues[c_HistoryCovertOpSuccessMarkerValueCount] = 
   {
      EHistoryValues::OverallRelations,
      EHistoryValues::Research,
      EHistoryValues::EconomicHealth,
      EHistoryValues::GDPCapita,
      EHistoryValues::ResProduction,
   };

   // Associates CovertOpFailure marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryCovertOpFailureMarkerValues[c_HistoryCovertOpFailureMarkerValueCount] = 
   {
      EHistoryValues::OverallRelations,
   };

   // Associates CovertOpVictim marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryCovertOpVictimMarkerValues[c_HistoryCovertOpVictimMarkerValueCount] = 
   {
      EHistoryValues::Stability,
      EHistoryValues::OverallRelations,
      EHistoryValues::EconomicHealth,
      EHistoryValues::GDPCapita,
      EHistoryValues::ResProduction,
   };

   // Associates IncreaseResourceProduction marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryIncreaseResourceProductionMarkerValues[c_HistoryIncreaseResourceProductionMarkerValueCount] = 
   {
      EHistoryValues::EconomicHealth,
      EHistoryValues::GDPCapita,
      EHistoryValues::Budget,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
   };

   // Associates IncreaseInterestLevel marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryIncreaseInterestLevelMarkerValues[c_HistoryIncreaseInterestLevelMarkerValueCount] = 
   {
      EHistoryValues::EconomicHealth,
      EHistoryValues::GDPCapita,
      EHistoryValues::Budget,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
   };

   // Associates DecreaseInterestLevel marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryDecreaseInterestLevelMarkerValues[c_HistoryDecreaseInterestLevelMarkerValueCount] = 
   {
      EHistoryValues::EconomicHealth,
      EHistoryValues::GDPCapita,
      EHistoryValues::Budget,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
   };

   // Associates ResourceIsPublic marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryResourceIsPublicMarkerValues[c_HistoryResourceIsPublicMarkerValueCount] = 
   {
      EHistoryValues::GDPCapita,
      EHistoryValues::Budget,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
   };

   // Associates ResourceIsPrivate marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryResourceIsPrivateMarkerValues[c_HistoryResourceIsPrivateMarkerValueCount] = 
   {
      EHistoryValues::GDPCapita,
      EHistoryValues::Budget,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
   };

   // Associates RandomEvent marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryRandomEventMarkerValues[c_HistoryRandomEventMarkerValueCount] = 
   {
      EHistoryValues::GDPCapita,
      EHistoryValues::Budget,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
      EHistoryValues::Population,
      EHistoryValues::Infrastructures,
   };

   // Associates TreatySigned marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryTreatySignedMarkerValues[c_HistoryTreatySignedMarkerValueCount] = 
   {
      EHistoryValues::OverallRelations,
      EHistoryValues::Research,
      EHistoryValues::EconomicHealth,
      EHistoryValues::GDPCapita,
      EHistoryValues::Budget,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
      EHistoryValues::HumanDevel,
   };

   // Associates TreatyCanceled marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryTreatyCanceledMarkerValues[c_HistoryTreatyCanceledMarkerValueCount] = 
   {
      EHistoryValues::OverallRelations,
      EHistoryValues::Research,
      EHistoryValues::EconomicHealth,
      EHistoryValues::GDPCapita,
      EHistoryValues::Budget,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
      EHistoryValues::HumanDevel,
   };

   // Associates IncreaseMilitaryArsenal marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryIncreaseMilitaryArsenalMarkerValues[c_HistoryIncreaseMilitaryArsenalMarkerValueCount] = 
   {
      EHistoryValues::ConvForces,
      EHistoryValues::StratForces,
      EHistoryValues::Upkeep,
      EHistoryValues::Budget,
   };

   // Associates DecreaseMilitaryArsenal marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryDecreaseMilitaryArsenalMarkerValues[c_HistoryDecreaseMilitaryArsenalMarkerValueCount] = 
   {
      EHistoryValues::ConvForces,
      EHistoryValues::StratForces,
      EHistoryValues::Upkeep,
      EHistoryValues::Budget,
   };

   // Associates ParkUnit marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryParkUnitMarkerValues[c_HistoryParkUnitMarkerValueCount] = 
   {
      EHistoryValues::Upkeep,
      EHistoryValues::Budget,
   };

   // Associates ReadyUnit marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryReadyUnitMarkerValues[c_HistoryReadyUnitMarkerValueCount] = 
   {
      EHistoryValues::Upkeep,
      EHistoryValues::Budget,
   };

   // Associates FortifyUnit marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryFortifyUnitMarkerValues[c_HistoryFortifyUnitMarkerValueCount] = 
   {
      EHistoryValues::Upkeep,
      EHistoryValues::Budget,
   };

   // Associates IncreaseResourceTax marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryIncreaseResourceTaxMarkerValues[c_HistoryIncreaseResourceTaxMarkerValueCount] = 
   {
      EHistoryValues::GDPCapita,
      EHistoryValues::Budget,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
   };

   // Associates DecreaseResourceTax marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryDecreaseResourceTaxMarkerValues[c_HistoryDecreaseResourceTaxMarkerValueCount] = 
   {
      EHistoryValues::GDPCapita,
      EHistoryValues::Budget,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
   };

   // Associates IncreaseGlobalResourceTax marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryIncreaseGlobalResourceTaxMarkerValues[c_HistoryIncreaseGlobalResourceTaxMarkerValueCount] = 
   {
      EHistoryValues::GDPCapita,
      EHistoryValues::Budget,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
   };

   // Associates DecreaseGlobalResourceTax marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryDecreaseGlobalResourceTaxMarkerValues[c_HistoryDecreaseGlobalResourceTaxMarkerValueCount] = 
   {
      EHistoryValues::GDPCapita,
      EHistoryValues::Budget,
      EHistoryValues::ResProduction,
      EHistoryValues::ResBalance,
   };

   // Associates TrainUnit marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryTrainUnitMarkerValues[c_HistoryTrainUnitMarkerValueCount] = 
   {
      EHistoryValues::ConvForces,
      EHistoryValues::Upkeep,
      EHistoryValues::Budget,
   };

   // Associates ModifyLaw marker with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum c_HistoryModifyLawMarkerValues[c_HistoryModifyLawMarkerValueCount] = 
   {
      EHistoryValues::Approval,
      EHistoryValues::Stability,
      EHistoryValues::OverallRelations,
      EHistoryValues::Population,
   };

   // Associates all markers with values
   // This array MUST be in sync with EHistoryValues content
   const EHistoryValues::Enum * const c_HistoryMarkerValues[EHistoryMarkerType::TotalCount] = 
   {
      c_HistoryGainPoliticalControlMarkerValues,
      c_HistoryLoosePoliticalControlMarkerValues,
      c_HistoryGainMilitaryControlMarkerValues,
      c_HistoryLooseMilitaryControlMarkerValues,
      c_HistoryDeclareWarMarkerValues,
      c_HistoryDeclarePeaceMarkerValues,
      c_HistoryReceiveNukeMarkerValues,
      c_HistoryChangeGovermentTypeMarkerValues,
      c_HistoryLaunchNukeMarkerValues,
      c_HistoryIncreaseTaxesMarkerValues,
      c_HistoryDecreaseTaxesMarkerValues,
      c_HistoryCombatStartedMarkerValues,

      c_HistoryIncreaseMinimumWageMarkerValues,
      c_HistoryDecreaseMinimumWageMarkerValues,
      c_HistoryPoliticPartyLegalMarkerValues,
      c_HistoryPoliticPartyIllegalMarkerValues,
      c_HistoryReligionLanguageLegalMarkerValues,
      c_HistoryReligionLanguageIllegalMarkerValues,
      c_HistoryReligionLanguageOfficialMarkerValues,
      c_HistoryDeclareMartialLawMarkerValues,
      c_HistoryRemoveMartialLawMarkerValues,
      c_HistoryCovertOpSuccessMarkerValues,
      c_HistoryCovertOpFailureMarkerValues,
      c_HistoryCovertOpVictimMarkerValues,
      c_HistoryIncreaseResourceProductionMarkerValues,
      c_HistoryIncreaseInterestLevelMarkerValues,
      c_HistoryDecreaseInterestLevelMarkerValues,
      c_HistoryResourceIsPublicMarkerValues,
      c_HistoryResourceIsPrivateMarkerValues,
      c_HistoryRandomEventMarkerValues,
      c_HistoryTreatySignedMarkerValues,
      c_HistoryTreatyCanceledMarkerValues,
      c_HistoryIncreaseMilitaryArsenalMarkerValues,
      c_HistoryDecreaseMilitaryArsenalMarkerValues,
      c_HistoryParkUnitMarkerValues,
      c_HistoryFortifyUnitMarkerValues,
      c_HistoryReadyUnitMarkerValues,
      c_HistoryIncreaseResourceTaxMarkerValues,
      c_HistoryDecreaseResourceTaxMarkerValues,
      c_HistoryTrainUnitMarkerValues,
      c_HistoryModifyLawMarkerValues,
      c_HistoryIncreaseGlobalResourceTaxMarkerValues,
      c_HistoryDecreaseGlobalResourceTaxMarkerValues,
   };

#define DEBUG_VALIDATE_COORDINATE(in_Vector2D)\
   {\
   gassert((in_Vector2D.x >= -180) && (in_Vector2D.x <= 180) &&  (in_Vector2D.y >= -90) && (in_Vector2D.y <= 90), "Invalid coordinate, lat, long" __FILE__);\
   }
}


#endif