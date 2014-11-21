/**************************************************************
*
* sp2_treaty.h
*
* Description
* ===========
*  Describes the interface of class GTreaty
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_TREATY_H_
#define _GOLEM_TREATY_H_

namespace SP2
{	
	namespace ETreatyRefusal
	{
		enum Enum
		{
			Accepted,
			UpkeepTooHigh,
			RelationsAreTooLow,
			SameTypeOfTreatyAlreadyExist,
			HostileRelationsWithOneOfItsFriend,
			NoRegionsWouldBeGained,
			RegionsGainedAreNotAcceptable,
			NoRegionsToFree,
			WantToKeepRegions,
			TooGoodRelationsWithTheEnemies,
			AlliedForcesAreTooWeak,
			DoesNotWantToLoseRelationsWithOtherFriendlyCountries,
			WillNotHelpCountriesInBetterShapeThanItsOwnCountry,
			CannotAffordTreaty,
			DoesNotMindTheOtherCountryMilitaryUnits,
			SomeCountriesInsideTheTreatyAreTooWeakEconomically,
			NoNeedToHaveAnotherCommonMarket,
			CannotAffordToMeetResearchConditions,
			DoesNotWantAPublicTreaty,
			LikeTheOtherCountriesTooMuch,
			OwnCountryIsInBadEconomicShapeAlready,
			TheOtherCountriesHaveATooStrongMilitary,
			WantsToKeepItsUnitsThere,
			ResearchTreatyNotProfitableEnough,
			Unknown
		};
	}

	namespace ETreatyConditions
   {
      enum Enum
      {
         GeographicProximity,
			MilitaryStrengthSimilitude,
			EconomicStrengthSimilitude,
			PoliticalSimilitude,
			ResearchLevelSimilitude,
			RelationsNotAtWar,
			ItemCount
      };
   }

	namespace EConditionStatus
   {
      enum Enum
      {
         NoLimit,
			Similar,
			NotSoSimilar,
			ItemCount
      };
   }

	namespace EConditionStatusRank
   {
      enum Enum
      {
         NoLimit,
			Top10,
			Top25,
			Top50,
			Top100,
			ItemCount
      };
   }

	namespace EConditionStatusResearch
   {
      enum Enum
      {
         NoLimit,
			Low,
			Average,
			High,			
			ItemCount
      };
   }

	namespace EConditionStatusPolitical
   {
      enum Enum
      {
         NoLimit,
			AllDemocracies,
			Communism,
			MilitaryDictatorship,
			Monarchy,
			MultiParty,
			SingleParty,
			Theocracy,
			ItemCount
      };
   }

	namespace EConditionStatusGeo
   {
      enum Enum
      {
         NoLimit,
			America,
			Europe,
			Asia,
			Oceania,
			Africa,
			WesternEurope,
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
			ItemCount
      };
   }

	namespace EConditionStatusRelation
   {
      enum Enum
      {
         NoLimit,
			NotAtWar,
			ItemCount
      };
   }


   /*!
    * List the possible types of diplomatic statuses
    **/
   namespace EDiplomaticStatus
   {
      enum Enum
      {
         Hostile,
         Neutral,
         Allied,
         MilitaryAccess,
         Count,
      };
   }

   /*!
    * Structure that represents two countries at war
    **/
   struct SWarPair
   {      
      UINT16			m_iFirstCountry;
      UINT16			m_iSecondCountry;

      SWarPair()
      {
      }

      SWarPair(UINT32 in_iFirstCountry, UINT32 in_iSecondCountry)
      {
         m_iFirstCountry = (UINT16) min(in_iFirstCountry, in_iSecondCountry);
         m_iSecondCountry = (UINT16) max(in_iFirstCountry, in_iSecondCountry);
      }

      bool operator == ( const SWarPair& in_Other ) const
      {
         return (UINT32) (*this) == (UINT32) in_Other;
      }

      bool operator <  ( const SWarPair& in_Other ) const
      {
         return (UINT32) (*this) < (UINT32) in_Other; 
      }

      operator size_t() const
      {
         return *( (UINT32*) this);
      }
   };

/*!
 * Class that represents a treaty between 2 countries
 **/
class GTreaty : public GSerializable
{
public:
   GTreaty();
   ~GTreaty();

   
   UINT32                  ID() const;                    //!<Get ID
   void                    ID(const UINT32);              //!<Set ID
	GString						Name() const;						 //!<Get the name of treaty
	void							Name(const GString&);			 //!<Set the new name of treaty
	INT32							NameID() const;
	void							NameID(INT32);
   ETreatyType::Enum       Type() const;                  //!<Get treaty type (see enum)
   void                    Type(const ETreatyType::Enum); //!<Set treaty type
   EDiplomaticStatus::Enum DiplomaticStatus() const;      //!<Get the diplomatic status associated with this treaty
	bool							Ponctual() const;					 //!<Return true if the treaty is ponctual (Is deleted once both side agrees)
   static bool	            Ponctual(const ETreatyType::Enum & in_eType);	//!<Return true if the treaty is ponctual (Is deleted once both side agrees)
	const UINT32*				Conditions() const;				 //!<Return the conditions
	void							AddCondition(ETreatyConditions::Enum in_eCondition, UINT32 in_iConditionStatus);	//!< Add, or modify, that condition. in_iCondition comes from the EConditionStatus* enum
	UINT32						Sides() const;						 //!<Get the number of sides (1 or 2) of the treaty
	UINT32						CountrySide(UINT32) const;		 //!<Get the side number of a member country. Returns 0 if the country has no sides. Side 1: A, Side 2: B, Side 3: Pressure
	void							AddMemberSideA(UINT32 in_iCountryID,bool in_bActive,bool in_bOriginal = false);	 //!<Add a member to side A, active is true if A has accepted treaty	
	void							AddMemberSideB(UINT32 in_iCountryID,bool in_bActive,bool in_bOriginal = false);	 //!<Add a member to side B, active is true if B has accepted treaty	
	void							AddMemberPressure(UINT32);		 //!<Add a member to pressure side
	void							RemoveMemberSideA(UINT32);		 //!<Remove a member from side A
	void							RemoveMemberSideB(UINT32);		 //!<Remove a member from side B
	void							RemoveMemberPressure(UINT32);	 //!<Remove a member from pressure side
   const set<UINT32>&		MembersSideA(bool in_bActive) const;	 //!<Get the members of Side A that has accepted (or not) the treaty   
	const set<UINT32>&		MembersSideB(bool in_bActive) const;    //!<Get the members of Side B that has accepted (or not) the treaty  
	const set<UINT32>&		MembersPressure() const;       //!<Get the members applying pressure   
	bool							Active() const;					 //!<Return true if the treay is activated	
	void							Active(bool);						 //!<Sets active to true
	bool							Private() const;					 //!<Gets private value
	void							Private(bool);						 //!<Sets private value
	bool							IsAnOriginalA(UINT32 in_iCountryID) const; //!<Returns true if the country is an original on side A
	bool							IsAnOriginalB(UINT32 in_iCountryID) const; //!<Returns true if the country is an original on side A
	bool							ShouldBeActive() const;			 //!<Returns true if the treaty should be activated
	bool							SideBCanLeave() const;			 //!<Returns true if the countries on side B have a say on being inside that treaty
	void							SuspendCountry(UINT32 in_iCountryID); //!<Will suspend a country
	void							CancelSuspension(UINT32 in_iCountryID); //!<Will cancel the suspension of a country
	bool							IsSuspended(UINT32 in_iCountryID) const; //!<Will return true if that country is currently suspended
   const set<UINT32> &     MembersOriginalsA() const;
   const set<UINT32> &     MembersOriginalsB() const;
	REAL64						CostForCountry(UINT32 in_iCountryID) const; //!<Will return the cost of that treaty for the country
	void							CostForCountry(UINT32 in_iCountryID, REAL64 in_fCost); //!<Will set the cost of that treaty for the country
	void							ClearCosts();
	void							Creator(UINT32);		//!< Sets the country that created the treaty
	UINT32						Creator() const;		//!< Gives the country that created the treaty
	void							ClearPressure();
	void							RemoveOriginal(UINT32 in_iCountryID); //!< Remove this country completely

	bool                    Serialize(GIBuffer&) const;    //!<Fill buffer for network transmission
   bool                    Unserialize(GOBuffer&);        //!<Fill object from network reception

	set<UINT32>&				EligibleCountries();	
	void							EligibleCountries(const set<UINT32>& in_EligibleCountries);
	bool							m_bEligibleDirty;


protected:
private:
	UINT32					m_pConditions[ETreatyConditions::ItemCount];	
	UINT32               m_iID;
	GString					m_sName;
	INT32						m_iNameID;
	UINT32					m_iCreatorCountry;
   set<UINT32>				m_vSideA;
	set<UINT32>				m_vSideAActive;
	set<UINT32>				m_vSideAOriginals;
	set<UINT32>				m_vSideB;
	set<UINT32>				m_vSideBActive;
	set<UINT32>				m_vSideBOriginals;
	set<UINT32>				m_vPressure;
	set<UINT32>				m_vSuspensionWasActive;
	set<UINT32>				m_vSuspensionWasNotActive;
	hash_map<UINT32,REAL64>	m_CostByCountry;
   ETreatyType::Enum    m_Type;
	set<UINT32>				m_EligibleCountries;
	bool						m_bPrivate;
	bool						m_bActive;			
};//end class GTreaty


/*!
 * Class that represents the treaty refusal reasons
 **/
class GTreatyRefusalDetails : public GSerializable
{
public:
   GTreatyRefusalDetails();
   ~GTreatyRefusalDetails();

	GString						TreatyName() const;				 //!<Get the name of treaty
	void							TreatyName(const GString&);			 //!<Set the new name of treaty

	vector<pair<UINT32,ETreatyRefusal::Enum> >&			CountriesThatRefused();	
	void							CountriesThatRefused(const vector<pair<UINT32,ETreatyRefusal::Enum> >& in_CountriesThatRefused);
	hash_set<UINT32>&			CountriesThatAccepted();	
	void							CountriesThatAccepted(const hash_set<UINT32>& in_CountriesThatAccepted);

	bool                    Serialize(GIBuffer&) const;    //!<Fill buffer for network transmission
   bool                    Unserialize(GOBuffer&);        //!<Fill object from network reception

protected:
private:
	GString			m_sTreatyName;

	//! Countries that refused
	vector<pair<UINT32,ETreatyRefusal::Enum> >		m_CountriesThatRefused;
	//! Countries that accepted
	hash_set<UINT32>											m_CountriesThatAccepted;				
};//end class GTreatyRefusalDetails

};//end namespace SP2
#endif //_GOLEM_TREATY_H_
