/**************************************************************
*
* sp2_world_behavior.h
*
* Description
* ===========
*  class to manage the effects of actions on data and of data on other
*  data. It receives by xml the configs (defined has the importance of the data
*  and the normal goal for the world) and the manager computes the modification
*  factors.
*
* Owner
* =====
*  Jean-René Couture
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SP2_WORLDBEHAVIOR_H
#define _GOLEM_SP2_WORLDBEHAVIOR_H

namespace SP2
{

CWSTR DATAMODIFIERS				=L"DATA_MODIFIERS";
CWSTR FREQUENCY                =L"FREQUENCY";
CWSTR GOALMARKER				   =L"goal";

/*!
 * GWorldBehavior is handling two things: the way data affect each other when a country "iterates", and
 * the consequences of actions on data. We could possibly use that to compute problems, it'll have to be
 * determined.
 **/
class SP2::GRegion;
class GWorldBehavior : public Hector::DesignPattern::GSubject , public SDK::GGameDataNode
{
private:   
	GLogger* m_pLogger;	//!<logger used to write inside World_Behavior.log
	GLogger* m_pLoggerResources;	//!<logger used to write inside Resources.log
	REAL64 m_fNextIteration; //!< next country iteration; << HAS TO BE SAVED
	REAL64 m_fNextNewsIteration; //!< next news iteration; << HAS TO BE SAVED
	REAL64 m_fNextRelationsTreatiesIteration; //!< next relation-treaty iteration; << HAS TO BE SAVED
	REAL64 m_fLastBudgetIteration; //!< last time the budget was iterated for all countries << HAS TO BE SAVED	
	REAL64 m_fLastStabilityApprovalIteration; //!< last time the approval and stability was iterated for every country << HAS TO BE SAVED
	REAL64 m_fLastNukeReadyIteration;
   GCountryData* m_CountryData; //!<contains the pointer to the country data for the current country being iterated
   REAL32 m_fFrequency;    //!< between ]0..1] 1 being one year. The frequency in which the   
	REAL64			m_fIMFfunds;				   //Amount given by all countries to IMF 
	Random::GQuick m_QuickRandom;
   INT16 m_iInternalCounter;  //!< used inthe Iterate; << HAS TO BE SAVED
	bool m_bInit;		

    INT16 m_iHDILogStartingCountryID; //!< Helper to log every country's HDI every specified time period; 0 means not currently logging

	//! Fills the set with economic friends of a country
	void EconomicFriends(UINT32 in_iCountryID, set<UINT32>& out_Friends) const;

	//! Execute a market iteration, for a treaty or global market
	void ExecuteMarket(UINT32 in_iTreatyID, bool in_bWorldMarket, vector<REAL64>& in_vPool);

	//! Execute the elections of a country
	void ExecuteElections(UINT32 in_iCountryID);

	//! Will execute possible random events for that country
	void RandomEvents(UINT32 in_iCountryID);

	//! Will execute a specific random event to a region
	void ExecuteRandomEvent(UINT32 in_iRegionID, ERandomEvents::Enum in_Event);

	//! Iterate the elections
	void IterateForElections();

	//! Iterate the stability and approval for every country
	void IterateChangingVariables(REAL64 in_fGameTime);

	//! Iterate the budget for every country. 
	void IterateBudget(REAL64 in_fGameTime);

	//! Iterate the International Monetary funds
	void IterateIMF(REAL64 in_fGameTime);

	//! Iterate the countries that might get suspended
	void IterateTreatiesSuspension();

	//! Iterate the nuclear research level for every countries
	void IterateNuclearResearchLevel(REAL64 in_fGameTime);

	//! data getter functions from the sp2 database
   REAL32 GetDataREAL32(INT16 in_iCountryID, GString in_sTable, GString in_sDataname);   

	//!Returns a value between 0.0f and in_fMaximumValue (excluded)
	REAL32 GetRandom(REAL32 in_fMaximumValue);

	//! Return an integer value, from a float value. If float are under 1.0f, it won't automatically convert it to 0, it might still give 1.
	INT64 ReturnInteger64(REAL32 in_fValue);
	//! Return an integer value, from a float value. If float are under 1.0f, it won't automatically convert it to 0, it might still give 1.
	INT32 ReturnInteger32(REAL32 in_fValue);

   //! modifies all the data that affect other data
	bool CountryIterate(INT16 in_iCountryID);   

   //! Verify if a country is in economical failure
   bool VerifyEconomicFailure(INT16 in_iCountryID);   

   //! BankruptCountry
   void BankruptCountry(INT16 in_iCountryID);


	//Region iteration
   bool Iterate_Population(SP2::GRegion*);
   bool Iterate_Telecom_Level(SP2::GRegion*, REAL64 in_fGameTime);
	bool Iterate_Infrastructure(SP2::GRegion*, REAL64 in_fGameTime);
	//bool Iterate_Demand(SP2::GRegion*);	
	bool Iterate_Production(SP2::GRegion*);
	bool Iterate_Tourism(SP2::GRegion*);

	bool Iterate_Demand();

   // these functions are the data iteration functions  
   void Iterate_Gvt_Approval_Expected();          // <--
	void Iterate_Gvt_Stability_Expected();         // <--
	void Iterate_Corruption_Expected();            // <--
	void Iterate_Telecom_Expected();               // <--
	void Iterate_Infrastructure_Expected();        // <--
	void Iterate_Birth_Rate_Expected();
	void Iterate_Death_Rate_Expected();
	void Iterate_Emigration_Level_Expected();
	bool Iterate_Inflation_Level_Expected();   
   
   bool Iterate_Resources_Gdp();

   bool Iterate_Political_Ideology();             // <--
   bool Iterate_Economic_Model();                 // <--
   bool Iterate_Political_Party_Popularity();     // <--

   bool Iterate_Pop_15_65_Unemployed();           // <--
   bool Iterate_Human_Development();              // <--   
   
   bool Iterate_Pop_In_Poverty();                 

   bool Iterate_Pop_Economic_Model();             // <--
   bool Iterate_Pop_Political_Philosophy();       // <--		

	void IterateGlobalMarket();
	void IterateRelationsTreaties();
	void IterateLossRelationsFromPressure();
	void IterateEmigrationImmigration();

	bool Iterate_Budget_Expense_Security();   
	bool Iterate_Budget_Expense_Unit_Upkeep();
	bool Iterate_Budget_Revenue_Tax();

	//! Returns number between 0 and 1
	static REAL32 AdjustMod(REAL64 in_fMod);

public:
   GWorldBehavior();
   ~GWorldBehavior();

	//! called by the constructor and destructor, destroys all structures and
	//! resets all data to 0.
   bool Reset();        
   
   //! Loads the information of of the way data is to be used
	bool LoadConfigFile(const GString &in_sFilename, const GString &in_sPath = L"");

	//! Called by the server Iterate to call CountryIterate
   bool GlobalIterate(REAL64 in_fGameTime);   

	//! TODO: will need to take as parameter the structure of an action
   bool PerformActionConsequence();	

	//! Returns a number between 0 and 1, representing the density status of a country, 1 being perfect
	static REAL32 FindPopulationDensityFactor(ENTITY_ID in_iCountryID);
	//! Returns a number between 0 and 1, representing the resource status of a country, 1 being the best
	static REAL32 FindResourceFactor(ENTITY_ID in_iCountryID);
	//! Returns a number between 0 and 1, representing the primary resource status of a country, 1 being the best
	static REAL32 FindPrimaryResourceFactor(ENTITY_ID in_iCountryID);
	//! Returns a number between 0 and 1, representing the tertiary resource status of a country, 1 being the best
	static REAL32 FindTertiaryResourceFactor(ENTITY_ID in_iCountryID);

	//! Returns a number between 0 and 1, representing the HD status of a country, 1 being the highest
	static REAL32 FindHumanDevelopmentFactor(ENTITY_ID in_iCountryID);

	//! Returns a number between 0 and 1, representing the GDP/Pop status of a country, 1 meaning a really rich country
	static REAL32 FindGDPPopulationFactor(ENTITY_ID in_iCountryID);

	//! Returns a number between 0 and 1, representing the National Security Funding/Pop status of a country, 1 meaning a country that spends a lot to pay Homeland Security and its spy agency
	static REAL32 FindNationalSecurityFundingFactor(ENTITY_ID in_iCountryID);

	//! Returns a number between 0 and 1, representing the IMF Receiving/Gdp status of a country, 1 meaning a country doesnt receive a single cents, 0 means a country really needs those IMF funding
	static REAL32 FindIMFReceivingFactor(ENTITY_ID in_iCountryID);

	//! Returns a number between 0 and 1, representing the Interest Rate status of a country, 1 meaning interest rate is really high
	static REAL32 FindInterestRateFactor(ENTITY_ID in_iCountryID);

	//! Returns a number between 0 and 1, representing the Unemployment status of a country, 1 meaning everybody is working
	static REAL32 FindUnemploymentFactor(ENTITY_ID in_iCountryID);

	//! Returns 1 if the condition based on unemployement will raise the inflation
	static REAL32 FindUnemploymentFactorForInflation(ENTITY_ID in_iCountryID);

	//! Returns a number between 0 and 1, representing the GDP Growth status of a country, 1 meaning the country is growing extremely fast
	static REAL32 FindGDPGrowthFactor(ENTITY_ID in_iCountryID);

	//! Returns a number between 0 and 1, representing the Demographic Financing status of a country, 1 meaning the country spends a lot for his people
	static REAL32 FindDemographicFinancingFactor(ENTITY_ID in_iCountryID);	

	//! Returns a number between 0 and 1, representing the Personal Income Tax status of a country, 1 meaning the population pays almost no tax at all
	static REAL32 FindPersonalIncomeTaxFactor(ENTITY_ID in_iCountryID);

	//! Returns a number between 0 and 1, representing the Military Force status of a country, 1 meaning the contry has an incredible force
	static REAL32 FindMilitaryForceFactor(ENTITY_ID in_iCountryID);

	//! Returns a number between 0 and 1, representing the Inflation Level status of a country, 1 meaning almost no inflation
	static REAL32 FindInflationLevelFactor(ENTITY_ID in_iCountryID);

	//! Returns a number between 0 and 1, representing the Pop in Poverty status of a country, 1 meaning no one is poor
	static REAL32 FindPopulationInPovertyFactor(ENTITY_ID in_iCountryID);

	//! Returns the economic health, between 0 and 1, of a country
	static REAL32 FindEconomicHealth(ENTITY_ID in_iCountryID);

	//! Returns whether the population has good value from the taxes it pays
	static REAL32 FindTaxBudgetFactor(ENTITY_ID in_iCountryID);

   // GGameDataNode implementation
   virtual bool OnSave(GIBuffer& io_Buffer);
   virtual bool OnLoad(GOBuffer& io_Buffer);
   virtual bool OnNew(GDatabase* in_pDatabase);
   virtual void OnClean();
};

}; //end namespace sp2
#endif