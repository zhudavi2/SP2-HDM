/**************************************************************
*
* golem_country_data_holder.h
*
* Description
* ===========
*	Contains	all the important	data used by the AI for	a specific country
*
* Owner
* =====
*	Alain	Bellehumeur
*
* Copyright	 (C) 2004, Laboratoires	Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_COUNTRY_DATA_H_
#define _GOLEM_COUNTRY_DATA_H_

#include "sp2_advisor.h"

namespace SP2	
{	
   struct GDesignFormat
   {
      EUnitType::Enum      m_eType;
      GString              m_sFormat;
      UINT32               m_iNextID;
   };

   struct GCountryDataOverwriteInfo
   {
       REAL32 m_fHumanDevelopment;
       REAL64 m_fOldGdpPerCapita;
       REAL64 m_fNewGdpPerCapita;
   };

	/*!
	*	Research	Information, contains all the	information	relative	to	the research budget,
	*	the progression of the research etc, for a given country
	**/
   class	GResearchInformation : public GResearchInformationItf , public SDK::GGameDataNode
	{
	private:
		//! Returns	the new progression in the	given	area
		REAL64 ComputeProgression(REAL64	in_fBudget,REAL64	in_fActualLevel,const REAL64&	in_fClock);

	public:

      GResearchInformation();
      ~GResearchInformation();

		void Update(const	REAL64& in_fClock, REAL64 in_fBudgetExpenseResearch, REAL64 in_fBonusToResearch);

		bool Load(UINT32 in_iCountryID);
		bool Save(UINT32 in_iCountryID, vector<GSString>& in_vQueries);

		REAL64 m_fLastCheckClock;

      // GGameDataNode implementation
      virtual bool OnSave(GIBuffer& io_Buffer);
      virtual bool OnLoad(GOBuffer& io_Buffer);
      virtual bool OnNew(GDatabase* in_pDatabase);
   };


	/*!
    * Class	to	represent a	country and	its data	in	Superpower 2.
    * Contains all important information about a country for the Artificial Intelligence calculations
    **/
   class	GCountryData : public GCountryDataItf , public SDK::GGameDataNode
	{
		friend class GDataControlLayer;
      friend class GServer;

      static GCountryDataItf* New();
		
   public:
		GCountryData();
		~GCountryData();

		//!Fetch	the information from	the database. 
		bool FetchCountryData(const ENTITY_ID in_iCountryID);

		//!Saves	the data	for that	country,	into the	DB
		bool CommitCountryData(IndexedData::GWriter& in_StringTable, vector<GSString>& in_vQueries, UINT32& in_iLanguageStatusIterator, UINT32& in_iReligionStatusIterator, UINT32& in_iCovertOpsIterator);

		//! Will	calculate the data for a given country	based	on	the information of its provinces. in_bFirstTime is true if it's the first time this is called in the game.
		void SynchronizeWithRegions(bool in_bFirstTime = false);

		//! Will readjust the expenses based on that ratio. A ratio of 1 won't change any expenses. A ratio of 2 doubles all expenses. A ratio of 0 put all expenses to 0
		void ReadjustDesiredExportsImports(REAL64 l_fChangeRatio);

      vector<GDesignFormat>& DesignFormat(EUnitType::Enum in_eType);

		//! Readjust the exports and imports to 0;
		void InitExportsImports();

		void IterateDemand();

      void           StartNuclearSubmarineReloadClock();
      void           ReloadSubmarinesNuclearMissiles();
      
      //! Add a fresh, new, ready to kill missile.
      void AddNewMissile(GNuclearMissile &in_NewMissile);
		//! Returns the advisor for that country
		GAdvisor*		Advisor();

		//! Add an advisor to this country (when a player takes control of a country)
		void				AddAdvisor(EAdvisorStatus::Enum in_eEconomic, EAdvisorStatus::Enum in_ePolitic, EAdvisorStatus::Enum in_eMilitary);

		//! Removes the advisor of a country (when a player leaves the game)
		void				RemoveAdvisor();

      //! Adds the nuclear missiles that are in that submarine to the list of nuclear missiles for that country
      void CreateMissilesForNuclearSubmarine(const UINT32 in_iNuclearSubmarineID);
      //! Removes all the nuclear missiles that were in that submarine
      void RemoveMissilesForNuclearSubmarine(const UINT32 in_iNuclearSubmarineID);

		//!Return the last world behavior iteration of a country	(days	since	the beginning of the	game)
		void				LastIteration(REAL64	in_fLastIteration);
		REAL64			LastIteration();

		//! Higher number means that country is very individualism
		REAL32		   IndividualismCollectivism() const;
		//! Higher number means that country is very male
		REAL32		   MaleFemale() const;
		//! Higher number means that country has a high hierarchical distance
		REAL32		   HierarchicalDistance() const;
      void           IterateCovertActionCells();

      // GGameDataNode implementation
      virtual bool OnSave(GIBuffer& io_Buffer);
      virtual bool OnLoad(GOBuffer& io_Buffer);
      virtual bool OnNew(GDatabase* in_pDatabase);
      virtual void OnClean();

	protected:
	private:
      
		//Resources	data
      REAL32         m_fReloadNuclearSubmarinesClock;
      GAdvisor*      m_pAdvisor;
	   REAL64			m_fLastIteration;

      //Contains the design formats for that country
      vector<vector<GDesignFormat> >   m_vDesignFormat;
		
		//AI related variables
		REAL32			m_fAIHierarchicalDistance;
		REAL32			m_fAIMaleFemale;
		REAL32			m_fAIIndividualism;

      static const UINT8 c_iResourceGvtControled = 1;
      static const UINT8 c_iResourceLegal = 2;
      static const UINT8 c_iResourceMeetsComsuption = 4;

      //Array for easier initialization.
      static const GCountryDataOverwriteInfo c_countryDataOverwriteInfo[];

	};	//	End GCountryData class

};	//End	namespace SP2

#endif //_GOLEM_COUNTRY_DATA_H_

