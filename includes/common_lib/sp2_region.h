/**************************************************************
*
* sp2_region.h
*
* Description
* ===========
*  Contains the data for every region in the game. 
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _SP2_REGION_H_
#define _SP2_REGION_H_

using namespace SDK::EHE;

namespace SP2
{
   class GRegion : public GSerializable
   {		
   public:
		GRegion();
      ~GRegion();

		void Id(UINT32 in_iRegionID);
		UINT32 Id() const;

		void OwnerId(ENTITY_ID in_iOwnerID);
		ENTITY_ID OwnerId() const;

		void OwnerMilitaryId(ENTITY_ID in_iOwnerMilitaryId);
		ENTITY_ID OwnerMilitaryId() const;

		void NameId(INT32 in_iNameID);
		INT32 NameId() const;

		void PercentageValue(REAL32 in_fPercentageValue);
		REAL32 PercentageValue() const;

		void AreaLand(REAL32 in_fAreaLand);
		REAL32 AreaLand() const;

		void AreaWater(REAL32 in_fAreaWater);
		REAL32 AreaWater() const;

		void AreaTotal(REAL32 in_fAreaTotal);
		REAL32 AreaTotal() const;

		void Population15(INT64 in_iPopulation15);
		INT64 Population15() const;

		void Population1565(INT64 in_iPopulation1565);
		INT64 Population1565() const;

		void Population65(INT64 in_iPopulation65);
		INT64 Population65() const;

		INT64 Population() const;

		UINT32 HistoricalClaim() const;
		void   HistoricalClaim(UINT32);

		void Infrastructure(REAL32 in_fInfrastructure);
		REAL32 Infrastructure() const;

		void TelecomLevel(REAL32 in_fTelecomLevel);
		REAL32 TelecomLevel() const;

		void TourismIncome(REAL64 in_fTourismIncome);
		REAL64 TourismIncome() const;

		void ArableLand(REAL32 in_fArableLand);
		REAL32 ArableLand() const;

		void ForestLand(REAL32 in_fForestLand);
		REAL32 ForestLand() const;

		void ParksLand(REAL32 in_fParksLand);
		REAL32 ParksLand() const;

		void NotUsedLand(REAL32 in_fNotUsedLand);
		REAL32 NotUsedLand() const;

		void HighestPoint(INT32 in_iHighestPoint);
		INT32 HighestPoint() const;

		void LowestPoint(INT32 in_iLowestPoint);
		INT32 LowestPoint() const;

		void StandardElevation(INT32 in_iStandardElevation);
		INT32 StandardElevation() const;

		void Coastlines(INT32 in_iCoastlines);
		INT32 Coastlines() const;


		//! Change the pourcentage of a given  ID
		void	ReligionChangePopulation(INT32 in_iID, INT64 in_iReligionPopulation);
		//! Change the pourcentage of a given Religion ID
		INT64	ReligionGetPopulation(INT32 in_iReligionID);
		//! Inserts a religion into a country
		void InsertReligion(INT32 in_iID, INT32 in_iReligionPopulation);
		void InsertLanguage(INT32 in_iID, INT32 in_iLanguagePopulation);

		//! Change the pourcentage of a given  ID
		void	LanguageChangePopulation(INT32 in_iID, INT64 in_iLanguagePopulation);
		//! Change the pourcentage of a given Language ID
		INT64	LanguageGetPopulation(INT32 in_iLanguageID);

		void GetReligions(GReligionList& out_Religions);
		void GetLanguages(GLanguageList& out_Languages);

		REAL32 RandomEvent(SP2::ERandomEvents::Enum in_Event) const;
		void RandomEvent(SP2::ERandomEvents::Enum in_Event, REAL32 in_fNewProbability);

		//! Gives/Sets the production for the specified resource
		REAL64			ResourceProduction(SP2::EResources::Enum in_ResourceID) const;
		void				ResourceProduction(SP2::EResources::Enum in_ResourceID, REAL64 in_fNewProduction);
		//! Gives/Sets the demand for the specified resource
		//REAL64			ResourceDemand(SP2::EResources::Enum in_ResourceID) const;
		//void			ResourceDemand(SP2::EResources::Enum in_ResourceID, REAL64 in_fNewDemand);

      // Data serialization functions
      virtual bool Serialize(GIBuffer&) const;
      virtual bool Unserialize(GOBuffer&);
         			
   private:				
		//Dynamic values
		ENTITY_ID	m_iOwnerId;		
		ENTITY_ID	m_iOwnerMilitaryId;
		INT64			m_iPopulation15;
		INT64			m_iPopulation1565;
		INT64			m_iPopulation65;
		REAL32		m_fPercentageValue;
		REAL32		m_fInfrastructure;
		REAL32		m_fTelecomLevel;
		REAL64		m_fTourismIncome;
		REAL64		m_pProduction[EResources::ItemCount];
		REAL32		m_pRandomEvents[ERandomEvents::ItemCount];
		//REAL64		m_pDemand[EResources::ItemCount];
		
		
		GReligionList m_Religions;//hash of <religion_id, pair<population,status> >
		GLanguageList m_Languages;//hash of <language_id, pair<population,status> >
		   
		
		//Static values
		INT32			m_iNameId;
		UINT32		m_iRegionId;
		UINT32		m_iHistoricalClaim;
		REAL32		m_fAreaLand;
		REAL32		m_fAreaWater;
		REAL32		m_fAreaTotal;
		REAL32		m_fArableLand;
		REAL32		m_fNotUsedLand;
		REAL32		m_fParksLand;
		REAL32		m_fForestLand;		
		INT32			m_iHighestPoint;
		INT32			m_iLowestPoint;
		INT32			m_iStandardElevation;
		INT32			m_iCoastlines;
	}; //end class GRegion

}

#endif //_SP2_REGION_H_