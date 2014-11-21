/**************************************************************
*
* sp2_military_plan.h
*
* Description
* ===========
*  Contains the list of regions each country wants to attack.
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _SP2_MILITARY_PLAN_H_
#define _SP2_MILITARY_PLAN_H_

namespace SP2
{
   class GMilitaryPlan : public GSerializable
   {
   public:
      GMilitaryPlan();
      ~GMilitaryPlan();

		void CountryId(ENTITY_ID);
		ENTITY_ID CountryId() const;

		//! Returns true if a region is already inside a plan
		bool RegionAlreadyInPlan(UINT32 in_iRegionID) const;

		//! Add a region to a plan
		void AddRegionToPlan(UINT32 in_iRegionID, REAL32 in_fValue);

		//! Add a region to defend for that country. Contains regions for that country + regions from allies country.
		void AddRegionToDefend(UINT32 in_iRegionID, REAL32 in_fValue);

		//! Add a region to defend for that country. It contains the regions owned by the country only.
		void AddRegionToSelfDefend(UINT32 in_iRegionID, REAL32 in_fValue);

		//! fills up the outgoing multimap with the regions ID to attack for this country
		const multimap<REAL32,UINT32>& RegionsToAttack() const;		

		//! fills up the outgoing multimap with the regions ID to defend for this country
		const multimap<REAL32,UINT32>& RegionsToDefend() const;

		//! Returns self defend regions
		const multimap<REAL32,UINT32>& RegionsToSelfDefend() const;

		void ClearPlans();

      // GSerializable implementation
      virtual bool Serialize(GIBuffer&) const;
      virtual bool Unserialize(GOBuffer&);

	protected:
		ENTITY_ID m_iCountryID;
		multimap<REAL32,UINT32> m_vRegionsToAttack; //!< Contains the regions this country would like to attack, in order of importance		
		multimap<REAL32,UINT32> m_vRegionsToDefend;
		multimap<REAL32,UINT32> m_vRegionsToSelfDefend;

   };
};

#endif