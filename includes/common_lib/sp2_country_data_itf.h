/**************************************************************
*
* golem_countrydataitf.h
*
* Description
* ===========
*  Describes the interface of class GCountryDataItf
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_COUNTRYDATAITF_H_
#define _GOLEM_COUNTRYDATAITF_H_

namespace SP2
{
   class GCovertActionCell;
	/*!
 	 *	Class	that contains all	specific	info for	a political	party
	 **/
   class	GPoliticalParty : public GSerializable
	{
	public:
		GPoliticalParty();
		~GPoliticalParty();

		void Id(INT32 in_iPartyID);
		INT32	Id() const;

		void CountryId(ENTITY_ID in_iCountryID);
		ENTITY_ID CountryId() const;

		void NameId(INT32	in_iNameID);
		INT32	NameId()	const;

		void PercentageValue(REAL32 in_fPercentageValue);
		REAL32 PercentageValue() const;

		void Status(INT32	in_iStatus);
		INT32	Status()	const;

		void PoliticalIdeology(REAL32	in_fPoliticalIdeology);
		REAL32 PoliticalIdeology()	const;

      void GvtType(SP2::EGovernmentType::Enum in_iGvtType);
		SP2::EGovernmentType::Enum GvtType() const;

		bool	InPower() const;
		void	InPower(bool);

      void Clear();

      bool Serialize(GIBuffer&) const;
      bool Unserialize(GOBuffer&);

	private:						
		INT32			m_iPartyID;
		ENTITY_ID	m_iCountryID;
		INT32			m_iNameId;
		INT32			m_iStatus;
		REAL32		m_fPercentageValue;
		REAL32		m_fPoliticalIdeology;
		SP2::EGovernmentType::Enum m_iGvtType;
		bool			m_bInPower;
	};	//End	GPoliticalParty class


	/*!
	*	Research	Information, contains all the	information	relative	to	the research budget,
	*	the progression of the research etc, for a given country
	**/
   class	GResearchInformationItf : public GSerializable
	{		
	private:

	public:

      GResearchInformationItf();
      ~GResearchInformationItf();

      bool Serialize(GIBuffer&) const;
      bool Unserialize(GOBuffer&);

      void UpdateGroundBudget(REAL64 in_fNewValue);
      void UpdateAirBudget(REAL64 in_fNewValue);
      void UpdateNavalBudget(REAL64 in_fNewValue);
      void UpdateNuclearBudget(REAL64 in_fNewValue);


      GResearchInformationItf& operator =(const GResearchInformationItf& in_Original);

      static REAL64 TimeToCompletion(REAL64 in_fActualValue,REAL64 in_fBudget);

      REAL64 Total() const;
      
      //1st	level	Budget Information
		REAL64 m_fBudgetGround;
		REAL64 m_fBudgetAir;
		REAL64 m_fBudgetNaval;
		REAL64 m_fBudgetNuclear;

		//Detailed budget	information
      REAL64 m_fBudget   [EUnitCategory::ItemCount][EUnitDesignCharacteristics::ItemCount];
      //Maximum values for	the country	research, 
		//we use	a REAL64	cause	we	put the research progression in 
		//this variable directly (5.1	means	10% of progression between	5 and	6)
      REAL32 m_fMaxValues[EUnitCategory::ItemCount][EUnitDesignCharacteristics::ItemCount];
	};


   /*!
   * Class that holds the generic country data information
   **/
   class GCountryDataItf : public GSerializable
   {
   public:
      GCountryDataItf();
      virtual ~GCountryDataItf();

      bool Serialize(GIBuffer&) const;
      bool Unserialize(GOBuffer&);


      void ClearModifications();
      //! Tells if country data is dirty (one of the category must be true)
      bool Dirty() const;

      void FlagCovertActionsCellsAsDirty();

		//! Find the modifier to apply on the expenses
		REAL64 FindInflationModifierOnExpenses() const;

		//! Find the modifier to apply on the revenues
		REAL64 FindInflationModifierOnRevenues() const;

		//Country Id Get/Set
		void				CountryID(const INT16 in_iCountryID);
		INT16				CountryID()	const;

		//Interest Level Get/Set
		void				InterestLevel(const REAL32	in_fInterestLevel);
		REAL32			InterestLevel() const;

		//Personal Income	Tax Get/Set
		void				PersonalIncomeTax(const	REAL64 in_fPersonalIncomeTax);
		REAL64			PersonalIncomeTax() const;

		//Monetary Supply	Amount Get/Set
		void				MonetarySupplyAmount(const	REAL64 in_fMonetarySupplyAmount);
		REAL64			MonetarySupplyAmount() const;		

		//Pop	15	 Get/Set
		INT64				Pop15() const;
      void           Pop15(INT64 in_iValue){m_iPop15 = in_iValue;}

		//Pop	15	65	 Get/Set
		INT64				Pop1565() const;
      void           Pop1565(INT64 in_iValue){m_iPop1565 = in_iValue;}

		//Pop	65	 Get/Set
		INT64				Pop65() const;
      void           Pop65(INT64 in_iValue){m_iPop65 = in_iValue;}

		//Telecom Level Get/Set
		REAL32			TelecomLevel()	const;
      void           TelecomLevel(REAL32 in_fValue){m_fTelecomLevel = in_fValue;}

		//Telecom Level Expected Get/Set
		REAL32			TelecomLevelExpected()	const;
      void           TelecomLevelExpected(REAL32 in_fValue){m_fTelecomLevelExpected = in_fValue;}

		//Gvt	Approval	Get/Set
		void				GvtApproval(const	REAL32 in_fGvtApproval);
		REAL32			GvtApproval() const;

		//Gvt	Approval Expected Get/Set
		void				GvtApprovalExpected(const	REAL32 in_fGvtApprovalExpected);
		REAL32			GvtApprovalExpected() const;

		//Inflation	Level	Get/Set
		void				InflationLevel(const	REAL32 in_fInflationLevel);
		REAL32			InflationLevel() const;

		//Infrastructure Get/Set
		REAL32			Infrastructure() const;
      void 				Infrastructure(REAL32 in_fValue){m_fInfrastructure = in_fValue;}

		//Infrastructure Expected Get/Set
		REAL32			InfrastructureExpected() const;
      void 				InfrastructureExpected(REAL32 in_fValue){m_fInfrastructureExpected = in_fValue;}

		//Demand total, used by budget window
		REAL64			TotalDemand() const;

		//GDP	Value	Get
		REAL64			GDPValue() const;		
      void           GDPValue(REAL64 in_fValue){m_fGDPValue = in_fValue;}

		//GDP	Value Base Get
		REAL64			GDPValueBase() const;		
      void           GDPValueBase(REAL64 in_fValueBase){m_fGDPValueBase = in_fValueBase;}

      REAL64         GDPPerCapita() const;

		//Growth	Rate Get
		REAL32			GrowthRate() const;
      void           GrowthRate(REAL32 in_fValue){m_fGDPGrowth = in_fValue;}

		//Birth Rate Get/Set
		void				BirthRate(const REAL32 in_fBirthRate);
		REAL32			BirthRate()	const;		

		//Death Rate Get/Set
		void				DeathRate(const REAL32 in_fDeathRate);
		REAL32			DeathRate()	const;		

		//Birth Rate Expected Get/Set
		void				BirthRateExpected(const REAL32 in_fBirthRateExpected);
		REAL32			BirthRateExpected()	const;		

		//Death Rate Expected Get/Set
		void				DeathRateExpected(const REAL32 in_fDeathRateExpected);
		REAL32			DeathRateExpected()	const;		

		//Inflation	Growth Get/Set
		void				InflationLevelExpected(const REAL32 in_fInflationLevelExpected);
		REAL32			InflationLevelExpected()	const;		

		//Human Development Get/Set
		void				HumanDevelopment(const REAL32	in_fHumanDevelopment);
		REAL32			HumanDevelopment() const;

		//Human Development Get/Set
		void				HumanDevelopmentAvg(const REAL32	in_fHumanDevelopmentAvg);
		REAL32			HumanDevelopmentAvg() const;

		//Overall Diplomatic Relations Get/Set
		void				OverallDiplomaticRelations(const REAL32 in_fOverallDiplomaticRelations);
		REAL32			OverallDiplomaticRelations() const;

		//Pressure Get/Set
		void				Pressure(const REAL32 in_fPressure);
		REAL32			Pressure() const;

		//National Security
		void				NationalSecurity(const REAL32 in_fNationalSecurity);
		REAL32			NationalSecurity() const;

		//Budget	Expense Security Get/Set
		void				BudgetExpenseSecurity(const REAL64 in_fBudgetExpenseSecurity);
		REAL64			BudgetExpenseSecurity()	const;

		//Budget	Expense Corruption Get/Set
		void				BudgetExpenseCorruption(const REAL64 in_fBudgetExpenseCorruption);
		REAL64			BudgetExpenseCorruption()	const;

		//Budget	Expense Pressure Get/Set
		void				BudgetExpenseDiplomacy(const REAL64 in_fBudgetExpenseDiplomacy);
		REAL64			BudgetExpenseDiplomacy()	const;

		//Budget	Expense Imports Get/Set
		void				BudgetExpenseImports(const	REAL64 in_fBudgetExpenseImports);
		REAL64			BudgetExpenseImports() const;

		//Budget	Expense Debt Get/Set
		REAL64			BudgetExpenseDebt() const;

		//Budget	Expense Unit Upkeep Get/Set
		void				BudgetExpenseUnitUpkeep(const	REAL64 in_fBudgetExpenseUnitUpkeep);
		REAL64			BudgetExpenseUnitUpkeep() const;
		
		//Budget	Expense Telecom Get/Set
		void				BudgetExpenseTelecomRatio(const	REAL64 in_fBudgetExpenseRatioTelecom);
		REAL64			BudgetExpenseTelecom() const;
		REAL64			BudgetExpenseTelecomOnAverage() const;
		REAL64			BudgetExpenseTelecomRatio() const;

		//Budget	Expense Infrastructure Get/Set		
		void				BudgetExpenseInfrastructureRatio(const REAL64 in_fBudgetExpenseRatioInfrastructure);
		REAL64			BudgetExpenseInfrastructure()	const;
		REAL64			BudgetExpenseInfrastructureOnAverage()	const;
		REAL64			BudgetExpenseInfrastructureRatio()	const;

		//Budget	Expense Propaganda Get/Set
		void				BudgetExpensePropagandaRatio(const REAL64 in_fBudgetExpensePropagandaRatio);
		REAL64			BudgetExpensePropaganda()	const;
		REAL64			BudgetExpensePropagandaOnAverage()	const;
		REAL64			BudgetExpensePropagandaRatio()	const;

		//Budget	Expense IMF	Get/Set
		void				BudgetExpenseIMFRatio(const REAL64	in_fBudgetExpenseRatioIMF);
		REAL64			BudgetExpenseIMF() const;
		REAL64			BudgetExpenseIMFOnAverage() const;
		REAL64			BudgetExpenseIMFRatio() const;

		//Returns
		REAL64         BudgetExpenseGovernmentReturn(REAL64 in_fWantedRatio) const;
		REAL64         BudgetExpenseIMFReturn(REAL64 in_fWantedRatio) const;
		REAL64         BudgetExpenseTelecomReturn(REAL64 in_fWantedRatio) const;
		REAL64         BudgetExpenseInfrastructureReturn(REAL64 in_fWantedRatio) const;
		REAL64         BudgetExpenseHealthcareReturn(REAL64 in_fWantedRatio) const;
		REAL64         BudgetExpenseEducationReturn(REAL64 in_fWantedRatio) const;
		REAL64         BudgetExpenseTourismReturn(REAL64 in_fWantedRatio) const;
		REAL64         BudgetExpenseEnvironmentReturn(REAL64 in_fWantedRatio) const;
		REAL64         BudgetExpensePropagandaReturn(REAL64 in_fWantedRatio) const;
		REAL64         BudgetExpenseResearchReturn(REAL64 in_fWantedRatio) const;

		//Budget	Expense Government Get/Set
		void				BudgetExpenseGovernmentRatio(const	REAL64 in_fBudgetExpenseRatioGovernment);      
		REAL64			BudgetExpenseGovernment() const;		
		REAL64			BudgetExpenseGovernmentOnAverage() const;		
		REAL64			BudgetExpenseGovernmentRatio() const;	

		//Budget	Expense Healthcare Get/Set
		void				BudgetExpenseHealthcareRatio(const	REAL64 in_fBudgetExpenseRatioHealthcare);
		REAL64			BudgetExpenseHealthcare() const;
		REAL64			BudgetExpenseHealthcareOnAverage() const;
		REAL64			BudgetExpenseHealthcareRatio() const;

		//Budget	Expense Education	Get/Set
		void				BudgetExpenseEducationRatio(const REAL64	in_fBudgetExpenseRatioEducation);
		REAL64			BudgetExpenseEducation() const;
		REAL64			BudgetExpenseEducationOnAverage() const;
		REAL64			BudgetExpenseEducationRatio() const;

		//Budget	Expense Research Get/Set
		void				BudgetExpenseResearchRatio(const REAL64 in_fBudgetExpenseRatioResearch);
		REAL64			BudgetExpenseResearch()	const;		
		REAL64			BudgetExpenseResearchOnAverage()	const;
		REAL64			BudgetExpenseResearchRatio()	const;	

		//Budget	Expense Environment Get/Set
		void				BudgetExpenseEnvironmentRatio(const REAL64 in_fBudgetExpenseRatioEnvironment);
		REAL64			BudgetExpenseEnvironment()	const;
		REAL64			BudgetExpenseEnvironmentOnAverage()	const;
		REAL64			BudgetExpenseEnvironmentRatio()	const;

		//Budget	Expense Tourism Get/Set
		void				BudgetExpenseTourismRatio(const	REAL64 in_fBudgetExpenseRatioTourism);
		REAL64			BudgetExpenseTourism() const;
		REAL64			BudgetExpenseTourismOnAverage() const;
		REAL64			BudgetExpenseTourismRatio() const;

		//Budget	Revenue Tax	Get/Set
		void				BudgetRevenueTax(const REAL64	in_fBudgetRevenueTax);
		REAL64			BudgetRevenueTax() const;

		//Budget	Revenue Exports Get/Set
		void				BudgetRevenueTrade(const	REAL64 in_fBudgetRevenueTrade);
		REAL64			BudgetRevenueTrade() const;

		//Budget	Revenue IMF	Get/Set
		void				BudgetRevenueIMF(const REAL64	in_fBudgetRevenueIMF);
		REAL64			BudgetRevenueIMF() const;

		//Budget	Revenue Tourism Get/Set
		void				BudgetRevenueTourism(const	REAL64 in_fBudgetRevenueTourism);
		REAL64			BudgetRevenueTourism() const;

		void				EconomicActivity(const REAL64 in_fEconomicActivity);
		REAL64			EconomicActivity() const;

		//Mili Manpower Available Get/Set
		void				MiliManpowerAvailable(const INT64 in_fMiliManpowerAvailable);
		INT64				MiliManpowerAvailable()	const;

		//Pop	15	65	Unemployed Get/Set
		void				Pop1565Unemployed(const	REAL32 in_fPop1565Unemployed);
		REAL32			Pop1565Unemployed() const;

		//Pop	In	Poverty Get/Set
		void				PopInPoverty(const REAL32 in_fPopInPoverty);
		REAL32			PopInPoverty()	const;

		//Gvt	Stability Get/Set
		void				GvtStability(const REAL32 in_fGvtStability);
		REAL32			GvtStability()	const;				

		//Gvt	Stability Expected Get/Set
		void				GvtStabilityExpected(const REAL32 in_fGvtStabilityExpected);
		REAL32			GvtStabilityExpected()	const;				

		//GDP	Growth Get/Set
		void				GDPGrowth(const REAL32 in_fGDPGrowth);
		REAL32			GDPGrowth()	const;

		//Martial law
		void				MartialLaw(const bool in_bMartialLaw);
		bool				MartialLaw() const;


      void  AddCovertActionCell(GCovertActionCell& in_Cell, bool in_bLoading = false);
      void  RemoveCovertActionCell(const GCovertActionCell& in_Cell);
      vector<GCovertActionCell>& CovertActionCells();

		//Religion vector
		const SP2::GReligionList& GetReligions() const;
      void SetReligions(const SP2::GReligionList & in_List){m_Religions = in_List;}
		//! Change the	status of a	given	ID
		void	ReligionChangeStatus(INT32	in_iReligionID, INT32 in_iReligionStatus);
		//! Get the	status of a	given	Religion	ID
		INT32	ReligionGetStatus(INT32	in_iReligionID);
		//! Get the	Population of a given Religion ID
		INT64	ReligionGetPopulation(INT32 in_iReligionID);
		//! Get the	pourcentage	of	the population	that follow	that religion
		REAL32 ReligionGetPourcentage(INT32	in_iReligionID);
		//! Return true if this	country has	a population that	follows that religion.
		bool	HasReligion(INT32	in_iReligionID);

		//Language vector
		const SP2::GLanguageList& GetLanguages() const;
      void SetLanguages(const SP2::GLanguageList& in_List){m_Languages = in_List;}
		//! Change the	status of a	given	ID
		void	LanguageChangeStatus(INT32	in_iLanguageID, INT32 in_iLanguageStatus);
		//! Get the	status of a	given	Language	ID
		INT32	LanguageGetStatus(INT32	in_iLanguageID);
		//! Get the	Population of a given Language ID
		INT64	LanguageGetPopulation(INT32 in_iLanguageID);
		//! Get the	pourcentage	of	the population	that speak that language
		REAL32 LanguageGetPourcentage(INT32	in_iLanguageID);
		//! Return true if this	country has	a population that	speaks that	language.
		bool	HasLanguage(INT32	in_iLanguageID);

		//Party vector
		const	vector< GPoliticalParty	>&	PoliticalParty() const;
      void	PoliticalParty(const vector< GPoliticalParty	>& in_list){m_vPoliticalParty = in_list;}
		//! Change the	status of a	given	Party	ID
		void	PoliticalPartyChangeStatus(INT32	in_iPoliticalPartyID, INT32 in_iPoliticalPartyStatus);

		//! Get the status of a political party
		INT32 PoliticalPartyGetStatus(INT32 in_iPoliticalPartyID) const;


      GCovertActionCell& CovertActionCell(UINT32 in_iID);

		//pointer to the political	party	leading the	country
		const	GPoliticalParty* LeaderParty() const;

		//Suffrage age
		void				Suffrage(const	INT32	in_iSuffrage);
		INT32				Suffrage() const;

		//Emigration Closed
		void				EmigrationClosed(const bool in_bEmigrationClosed);
		bool				EmigrationClosed() const;

		//Immigration Closed
		void				ImmigrationClosed(const	bool in_bImmigrationClosed);
		bool				ImmigrationClosed() const;	

		//Government Type
		void				GvtType(const INT32 in_iGvtType);
		INT32				GvtType() const;

		//Propaganda Level
		void				PropagandaLevel(const REAL64	in_fPropaganda);
		REAL64			PropagandaLevel() const;		

		//Next Election
		void				NextElection(const GDateTime in_NextElection);
		GDateTime		NextElection()	const;

		//Global Tax Mod
		void				GlobalTaxMod(REAL32 in_fGlobalTaxMod);
		REAL32			GlobalTaxMod()	const;

		//Budget	Revenues	Get
		REAL64			BudgetRevenues() const;
      void  			BudgetRevenues(REAL64 l_fVal){m_fBudgetRevenues = l_fVal;}

		//Budget	Expenses	Get
		REAL64			BudgetExpenses() const;
      void  			BudgetExpenses(REAL64 l_fVal){m_fBudgetExpenses = l_fVal;}

		//Pop	Political Ideology
		void				PopPoliticalIdeology(const	REAL32 in_fPopPoliticalIdeology);
		REAL32			PopPoliticalIdeology() const;

		//Pop	Economic	Model
		void				PopEconomicModel(const REAL32	in_fPopEconomicModel);
		REAL32			PopEconomicModel() const;

		//Country Political Ideology	
		REAL32			CountryPoliticalIdeology()	const;
      void	   		CountryPoliticalIdeology(REAL32 in_fVal);

		//Country Economic Model
		REAL32			CountryEconomicModel() const;
		void  			CountryEconomicModel(REAL32 in_fVal){m_fCountryEconomicModel = in_fVal;}

		//Population
		INT64				Population() const;
      void           Population(INT64 in_iValue) {m_iPopulation = in_iValue;}

		//Name ID
		INT32				NameID() const;
      void           NameID(INT32 in_iValue){m_iNameID = in_iValue;}

		//Population under political control only
		INT64				PopulationPoliticalControl() const;
      void 				PopulationPoliticalControl(INT64 in_iVal){m_iPopulationPoliticalControl = in_iVal;}

		//Emigration Level Get/Set
		void				EmigrationLevel(const REAL32 in_fEmigrationLevel);
		REAL32			EmigrationLevel()	const;

		//Emigration Level Expected Get/Set
		void				EmigrationLevelExpected(const REAL32 in_fEmigrationLevelExpected);
		REAL32			EmigrationLevelExpected()	const;

		//Immigration Level Get/Set
		void				ImmigrationLevel(const REAL32	in_fImmigrationLevel);
		REAL32			ImmigrationLevel() const;

		//Corruption Level Get/Set
		void				CorruptionLevel(const REAL32 in_fCorruptionLevel);
		REAL32			CorruptionLevel()	const;

		//Corruption Level Expected Get/Set
		void				CorruptionLevelExpected(const REAL32 in_fCorruptionLevelExpected);
		REAL32			CorruptionLevelExpected()	const;

		//Land stats Get
		REAL32			AreaTotal()	const;	
		REAL32			AreaLandTotal() const;
		REAL32			AreaWaterTotal() const;

      void			   AreaTotal(REAL32 in_fValue){m_fAreaTotal = in_fValue;}	
      void           AreaLandTotal(REAL32 in_fValue){m_fAreaLandTotal = in_fValue;}
      void           AreaWaterTotal(REAL32 in_fValue){m_fAreaWaterTotal = in_fValue;}

      
      REAL32			ArableLandLevel()	const;
		REAL32			ForestLandLevel()	const;
		REAL32			ParksLandLevel() const;
		REAL32			NotUsedLandLevel() const;

      void			   ArableLandLevel(REAL32 in_fValue) {m_fArableLandLevel = in_fValue;}	
      void           ForestLandLevel(REAL32 in_fValue) {m_fForestLandLevel = in_fValue;}
      void           ParksLandLevel(REAL32 in_fValue)  {m_fParksLandLevel = in_fValue;}
      void			   NotUsedLandLevel(REAL32 in_fValue){m_fNotUsedLandLevel = in_fValue;}	

      INT32 HighestPoint(){return m_iHighestPoint;}
	   INT32 LowestPoint(){return m_iLowestPoint;}
	   INT32 Coastlines(){return m_iCoastlines;}
	   INT32 StandardElevation(){return m_iStandardElevation;}
      
      void HighestPoint(INT32 in_iVal){m_iHighestPoint = in_iVal;}
      void LowestPoint(INT32 in_iVal){m_iLowestPoint = in_iVal;}
	   void Coastlines(INT32 in_iVal){m_iCoastlines = in_iVal;}
	   void StandardElevation(INT32 in_iVal){m_iStandardElevation = in_iVal;}


		//! AMDS	Level	getter/setter
		REAL32			AMDSLevel()	const;
      void           AMDSLevel(REAL32 in_fLevel);

		//!Political party currently in charge
		void				LeadingPoliticalPartyID(const	INT32	in_iPoliticalParty);
		INT32				LeadingPoliticalPartyID() const;

		//! Returns the political party ID for that type of government.
      INT32				LeadingPoliticalPartyOfType(SP2::EGovernmentType::Enum in_iGvtType) const;

		//! Returns the name id for the climate of that country
		INT32				ClimateNameID() const;

		//!Country group
		void				CountryGroup(const	INT32	in_iGroup);
		INT32				CountryGroup() const;

		//! Gets the economic health
		REAL32			EconomicHealth() const;
		//! Sets the economic health
		void				EconomicHealth(REAL32 in_fEconomicHealth);
	
		//Activated or not
		void				Activated(const bool in_bActivated);
		bool				Activated() const;		

		//Nuclear Ready or not
		void				NuclearReady(const REAL32 in_bNuclearReady);
		REAL32			NuclearReady() const;		

		void AcceptForeignOrder(const bool in_bAcceptForeignOrder);
		bool AcceptForeignOrder() const;

      void Clear();

		//! Nuclear missile Getter
      const vector<SP2::GNuclearMissile>&  GroundBasedNuclearMissiles() const;
      const GNuclearMissile&               NuclearMissile(UINT32 in_iId);
      const vector<SP2::GNuclearMissile>&  SubmarineBasedNuclearMissiles() const;
      GJoinedVectors<SP2::GNuclearMissile> NuclearMissiles();
      //void NuclearMissiles(SP2::ENuclearMissileType::Enum in_eMissileType,vector<SP2::GNuclearMissile>& out_vNuclearMissiles) const;
      void NuclearMissilesSet(ENuclearMissileType::Enum in_eMissileType, vector<GNuclearMissile>& in_vNuclearMissiles);

		GResearchInformationItf* ResearchInfo();
		const GResearchInformationItf* ResearchInfo() const;


		INT16	PoliticRank() const;
		void	PoliticRank(INT16	in_iRank);
		INT16	EconomicRank() const;
		void	EconomicRank(INT16 in_iRank);
		INT16	MilitaryRank() const;
		void	MilitaryRank(INT16 in_iRank);

		REAL32	PoliticStrength() const;
		void		PoliticStrength(REAL32	in_fStrength);
		REAL32	EconomicStrength() const;
		void		EconomicStrength(REAL32 in_fStrength);
		REAL32	MilitaryStrength() const;
		void		MilitaryStrength(REAL32 in_fStrength);

      set<INT32> & ReligionOfficialStatus(){return m_ReligionOfficialStatus;}
	   set<INT32> & ReligionIllegalStatus(){return m_ReligionIllegalStatus;}
	   set<INT32> & LanguageOfficialStatus(){return m_LanguageOfficialStatus;}
	   set<INT32> & LanguageIllegalStatus(){return m_LanguageIllegalStatus;}

      void ReligionOfficialStatus(set<INT32> & in_Set){m_ReligionOfficialStatus = in_Set;}
      void ReligionIllegalStatus(set<INT32> & in_Set) {m_ReligionIllegalStatus  = in_Set;}
      void LanguageOfficialStatus(set<INT32> & in_Set){m_LanguageOfficialStatus = in_Set;}
      void LanguageIllegalStatus(set<INT32> & in_Set) {m_LanguageIllegalStatus  = in_Set;}

		/*
		* Resources functions
		**/
		//! Gives the gdp for the specified resource
		REAL32			ResourceGDP(SP2::EResources::Enum in_ResourceID) const;
		//! Sets the gdp for the specified resource
		void				ResourceGDP(SP2::EResources::Enum in_ResourceID, REAL32 in_fNewGDP);
		//! Gives the production for the specified resource
		REAL64			ResourceProduction(SP2::EResources::Enum in_ResourceID) const;
		//! Gives the demand for the specified resource
		REAL64			ResourceDemand(SP2::EResources::Enum in_ResourceID) const;
		//! Gives the legality for the specified resource
		bool				ResourceLegal(SP2::EResources::Enum in_ResourceID) const;
		//! Sets the legality for the specified resource
		void				ResourceLegal(SP2::EResources::Enum in_ResourceID, bool in_bNewLegal);
		//! Gives the control for the specified resource
		bool				ResourceGvtCtrl(SP2::EResources::Enum in_ResourceID) const;
		//! Sets the control for the specified resource
		void				ResourceGvtCtrl(SP2::EResources::Enum in_ResourceID, bool in_bGvtCtrl);
		//! Gives the taxes for the specified resource
		REAL32			ResourceTaxes(SP2::EResources::Enum in_ResourceID) const;
		//! Sets the taxes for the specified resource
		void				ResourceTaxes(SP2::EResources::Enum in_ResourceID, REAL32 in_fTaxes);
		//! Gives the desired Import for the specified resource
		REAL64			ResourceImportDesired(SP2::EResources::Enum in_ResourceID) const;
		//! Sets the ImportDesired for the specified resource
		void				ResourceImportDesired(SP2::EResources::Enum in_ResourceID, REAL64 in_fImportDesired);
		//! Gives the desired Export for the specified resource
		REAL64			ResourceExportDesired(SP2::EResources::Enum in_ResourceID) const;
		//! Sets the desired Export for the specified resource
		void				ResourceExportDesired(SP2::EResources::Enum in_ResourceID, REAL64 in_fExportDesired);
		//! Gives the import for the specified resource
		REAL64			ResourceImport(SP2::EResources::Enum in_ResourceID) const;
		//! Sets the Import for the specified resource
		void				ResourceImport(SP2::EResources::Enum in_ResourceID, REAL64 in_fImport);
		//! Gives the export for the specified resource
		REAL64			ResourceExport(SP2::EResources::Enum in_ResourceID) const;
		//! Sets the Export for the specified resource
		void				ResourceExport(SP2::EResources::Enum in_ResourceID, REAL64 in_fExport);
		//! Gives the market share for the specified resource 
		REAL32			ResourceMarketShare(SP2::EResources::Enum in_ResourceID) const;
		//! Sets the market share for the specified resource
		void				ResourceMarketShare(SP2::EResources::Enum in_ResourceID, REAL32 in_fMarketShare);
		//! Gives the status of domestic consumption for the specified resource
		bool				ResourceMeetConsumption(SP2::EResources::Enum in_ResourceID) const;
		//! Sets the status of domestic consumption for the specified resource
		void				ResourceMeetConsumption(SP2::EResources::Enum in_ResourceID, bool in_bMeet);

      //! Gets the resource balance (negative means Needs, positive means has too much)
      REAL64         ResourceBalance(EResources::Enum in_eResource) const;

		//! Gives the production for all resources
		REAL64			ResourceProduction() const;

		//! Gives the resource balance for all resource (negative means Needs, positive means has too much)
		REAL64			ResourceBalance() const;

      //! Returns the accuracy of infomation towards that country
		REAL32			AccuracyInformation(UINT32 in_iCountryID) const;
		void				AccuracyInformation(UINT32 in_iCountryID, REAL32 in_fAccuracy);

      REAL64 ResourceProductionModifier();
      REAL64 ResourceDemandModifier();
      void   ResourceProductionModifier(REAL64 in_fValue);
      void   ResourceDemandModifier(REAL64 in_fValue);
		
		//! Gets the status of the specified law
		bool				InternalLaw(SP2::EInternalLaws::Enum in_iLawType) const;
		//! Sets the status of a specified law
		void				InternalLaw(SP2::EInternalLaws::Enum in_iLawType, bool in_bStatus);

      void           ChangePoliticalPartyPercentageValue(const INT32 in_iPoliticalParty, REAL32 in_fPerc);
		
		void				ChangePoliticalPartyInPower(const INT32 in_iPoliticalParty, bool in_bInPower);

      //! Gets the current conventionnal forces
      REAL32 ConvertionalForces() const;

      //! Gets the current strategic forces
      REAL32 StrategicForces() const;

      SP2::EAdvisorStatus::Enum AdvisorMilitaryStatus(){return m_eAdvisorMilitaryStatus;}
      SP2::EAdvisorStatus::Enum AdvisorPoliticStatus() {return m_eAdvisorPoliticStatus;}
      SP2::EAdvisorStatus::Enum AdvisorEconomicStatus(){return m_eAdvisorEconomicStatus;}
      
      void AdvisorMilitaryStatus(SP2::EAdvisorStatus::Enum in_eValue){m_eAdvisorMilitaryStatus = in_eValue; m_bOtherDataDirty = true;}
      void AdvisorPoliticStatus(SP2::EAdvisorStatus::Enum in_eValue) {m_eAdvisorPoliticStatus = in_eValue;  m_bOtherDataDirty = true;}
      void AdvisorEconomicStatus(SP2::EAdvisorStatus::Enum in_eValue){m_eAdvisorEconomicStatus = in_eValue; m_bOtherDataDirty = true;}

		static REAL64 m_pResourceMarketAvailability[SP2::EResources::ItemCount];   //Array	of	market availability for every resource
      static REAL32 m_fHumanDevelopmentAvg;
      mutable REAL64 m_fWhenSent;


   protected:

      bool SerializeResourceData(GIBuffer&) const;
      bool SerializeEconomicData(GIBuffer&) const;
      bool SerializeBudgetData(GIBuffer&) const;
      bool SerializeNuclearData(GIBuffer&)  const;
      bool SerializeOtherData(GIBuffer&)    const;
      bool SerializeCovertActionCells(GIBuffer&) const;
      bool UnserializeResourceData(GOBuffer&);
      bool UnserializeEconomicData(GOBuffer&);
      bool UnserializeBudgetData(GOBuffer&);
      bool UnserializeNuclearData(GOBuffer&);
      bool UnserializeOtherData(GOBuffer&);
      bool UnserializeCovertActionCells(GOBuffer&);

      void UpdateResearchExpenses();

      //Resource data
      REAL64			m_pResourceProduction[SP2::EResources::ItemCount];	   //Array of resources	production
	   REAL64			m_pResourceDemand[SP2::EResources::ItemCount];        //Array	of	resources demand
	   REAL64			m_pResourceExport[SP2::EResources::ItemCount];        //Array	of	resources export
		REAL32			m_pResourceMarketShare[SP2::EResources::ItemCount];   //Array	of	market share for every resource		
	   REAL64			m_pResourceExportDesired[SP2::EResources::ItemCount]; //Array of resources desired export
	   REAL64			m_pResourceImport[SP2::EResources::ItemCount];        //Array	of	resources import
	   REAL64			m_pResourceImportDesired[SP2::EResources::ItemCount]; //Array of resources desired import
	   bool				m_pResourceLegal[SP2::EResources::ItemCount];	      //Array of legality of resources
	   bool				m_pResourceGvtControl[SP2::EResources::ItemCount];	   //Array of control of resources
	   bool				m_pResourceMeetDomesticConsumption[SP2::EResources::ItemCount]; //Array of meeting domestic consumptions
	   REAL32			m_pResourceTaxes[SP2::EResources::ItemCount];	      //Array of resources	taxes
	   REAL32			m_pResourceGDP[SP2::EResources::ItemCount];	         //Array of resources	GDP
	   REAL64			m_fResourceProductionModifier;
	   REAL64			m_fResourceDemandModifier;
	   //End	Resources data

	   //Internal laws
	   bool				m_pInternalLaws[SP2::EInternalLaws::ItemCount];	   

	   INT32				m_iClimateNameID;
		INT16				m_iCountryID;	
	   INT32				m_iNameID;		
	   INT16				m_iPoliticRank;
	   INT16				m_iEconomicRank;
	   INT16				m_iMilitaryRank;
	   REAL32			m_fPoliticStrength;
	   REAL32			m_fEconomicStrength;
	   REAL32			m_fMilitaryStrength;
	   REAL32			m_fInterestLevel;
	   REAL64			m_fPersonalIncomeTax;
	   REAL64			m_fMonetarySupplyAmount;	   
	   INT64				m_iPop15;
	   INT64				m_iPop1565;
	   INT64				m_iPop65;
	   REAL32			m_fTelecomLevel;
		REAL32			m_fTelecomLevelExpected;
	   REAL32			m_fGvtApproval;
		REAL32			m_fGvtApprovalExpected;
	   REAL32			m_fInflationLevel;
	   REAL32			m_fInflationLevelExpected;
	   REAL32			m_fInfrastructure;
		REAL32			m_fInfrastructureExpected;
	   REAL64			m_fGDPValue;
		REAL64			m_fDemandTotal;
		REAL64			m_fGDPValueBase;
	   REAL32			m_fBirthRate;
		REAL32			m_fBirthRateExpected;
	   REAL32			m_fDeathRate;
		REAL32			m_fDeathRateExpected;
	   REAL32			m_fHumanDevelopment;      
		REAL32			m_fOverallDiplomaticRelations;
		REAL32			m_fPressure;
		REAL32			m_fNationalSecurity;

	   REAL64			m_fBudgetExpenseSecurity;
	   REAL64			m_fBudgetExpenseDiplomacy;
	   REAL64			m_fBudgetExpenseImports;
	   REAL64			m_fBudgetExpenseUnitUpkeep;
		REAL64			m_fBudgetExpenseCorruption;

	   REAL64			m_fBudgetExpenseRatioTelecom;
	   REAL64			m_fBudgetExpenseRatioResearch;
	   REAL64			m_fBudgetExpenseRatioInfrastructure;
	   REAL64			m_fBudgetExpenseRatioIMF;
	   REAL64			m_fBudgetExpenseRatioGovernment;		
	   REAL64			m_fBudgetExpenseRatioHealthcare;
	   REAL64			m_fBudgetExpenseRatioEducation;
	   REAL64			m_fBudgetExpenseRatioEnvironment;
	   REAL64			m_fBudgetExpenseRatioPropaganda;
	   REAL64			m_fBudgetExpenseRatioTourism;

	   REAL64			m_fBudgetRevenueTax;
	   REAL64			m_fBudgetRevenueTrade;
	   REAL64			m_fBudgetRevenueIMF;
	   REAL64			m_fBudgetRevenueTourism;

	   INT64				m_iMiliManpowerAvailable;
	   REAL32			m_fPop1565Unemployed;
	   REAL32			m_fPopInPoverty;
	   REAL32			m_fGvtStability;	
		REAL32			m_fGvtStabilityExpected;	
	   REAL32			m_fGDPGrowth;
	   INT32				m_iSuffrage;
	   INT32				m_iGvtType;		
	   REAL64			m_fBudgetExpenses;
	   REAL64			m_fBudgetRevenues;
	   REAL32			m_fPopPoliticalIdeology;
	   REAL32			m_fPopEconomicModel;
	   INT64				m_iPopulation;
	   INT64				m_iPopulationPoliticalControl;
	   REAL32			m_fEmigrationLevel;
		REAL32			m_fEmigrationLevelExpected;
	   REAL32			m_fImmigrationLevel;
	   REAL64			m_fPropagandaLevel;
	   REAL32			m_fCorruptionLevel;
		REAL32			m_fCorruptionLevelExpected;		
	   REAL32			m_fAreaTotal;
	   REAL32			m_fAreaWaterTotal;
	   REAL32			m_fAreaLandTotal;
	   REAL32			m_fArableLandLevel;
	   REAL32			m_fForestLandLevel;
	   REAL32			m_fParksLandLevel;
	   REAL32			m_fNotUsedLandLevel;
	   REAL32			m_fEconomicHealth;
		REAL64			m_fEconomicActivity;
	   INT32				m_iHighestPoint;
	   INT32				m_iLowestPoint;
	   INT32				m_iCoastlines;
	   INT32				m_iStandardElevation;
	   INT32				m_iPoliticalParty;
	   REAL32			m_fCountryPoliticalIdeology;
	   REAL32			m_fCountryEconomicModel;
	   REAL32			m_fAMDSLevel;
	   INT32				m_iCountryGroup;
	   REAL32			m_fGlobalTaxMod;

	   GDateTime		m_NextElection;
      SP2::GReligionList	m_Religions;
	   SP2::GLanguageList	m_Languages;
		hash_map<UINT32,REAL32>	m_AccuracyOfInformation; // Not used by server
	   set<INT32>		m_ReligionOfficialStatus;
	   set<INT32>		m_ReligionIllegalStatus;
	   set<INT32>		m_LanguageOfficialStatus;
	   set<INT32>		m_LanguageIllegalStatus;
	   vector< GPoliticalParty	>	m_vPoliticalParty;
      vector< SP2::GNuclearMissile	>	m_vGroundBasedNuclearMissiles;
      vector< SP2::GNuclearMissile	>	m_vSubmarineBasedNuclearMissiles;		

      SP2::EAdvisorStatus::Enum  m_eAdvisorMilitaryStatus;
      SP2::EAdvisorStatus::Enum  m_eAdvisorPoliticStatus;
      SP2::EAdvisorStatus::Enum  m_eAdvisorEconomicStatus;

      vector<GCovertActionCell>        m_vCovertActionCells;      


	   GResearchInformationItf*	m_pResearchInformation;
	   bool				            m_bEmigrationClosed;
	   bool				            m_bImmigrationClosed;
	   bool				            m_bMartialLaw;
	   bool				            m_bActivated;
		bool								m_bAcceptForeignOrder;
		REAL32							m_fNuclearReady;


public:
      // --- Dirty markers ---
      bool           m_bResourceDataDirty;
      bool           m_bNuclearMissilesDataDirty;
      bool           m_bOtherDataDirty;
      bool           m_bEconomicDataDirty;
      bool           m_bBudgetDataDirty;
      mutable bool   m_bCovertActionCellsDirty;
      //__________________________



   };
};
#endif //_GOLEM_COUNTRYDATAITF_H_