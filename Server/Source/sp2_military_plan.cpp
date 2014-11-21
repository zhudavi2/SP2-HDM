/**************************************************************
*
* sp2_military_plan.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#include "golem_pch.hpp"

//! constructor
GMilitaryPlan::GMilitaryPlan()
{	
}

//! destructor
GMilitaryPlan::~GMilitaryPlan()
{
}

void GMilitaryPlan::CountryId(ENTITY_ID in_iCountryID)
{
	m_iCountryID = in_iCountryID;
}

ENTITY_ID GMilitaryPlan::CountryId() const
{
	return m_iCountryID;
}	

void GMilitaryPlan::AddRegionToPlan(UINT32 in_iRegionID, REAL32 in_fValue)
{
	gassert(in_iRegionID != 0, "Region should not be 0");
	m_vRegionsToAttack.insert(make_pair(-in_fValue,in_iRegionID));	
}

bool GMilitaryPlan::RegionAlreadyInPlan(UINT32 in_iRegionID) const
{
	for(multimap<REAL32,UINT32>::const_iterator l_Itr = m_vRegionsToAttack.begin();
		 l_Itr != m_vRegionsToAttack.end();
		 l_Itr++)
	{
		if(l_Itr->second == in_iRegionID)
			return true;
	}
	
	return false;
}


const multimap<REAL32,UINT32>& GMilitaryPlan::RegionsToAttack() const
{	
	return m_vRegionsToAttack;	
}

void GMilitaryPlan::AddRegionToDefend(UINT32 in_iRegionID, REAL32 in_fValue)
{
	gassert(in_iRegionID != 0, "Region should not be 0");
	m_vRegionsToDefend.insert(make_pair(-in_fValue,in_iRegionID));	
}

void GMilitaryPlan::AddRegionToSelfDefend(UINT32 in_iRegionID, REAL32 in_fValue)
{
	gassert(in_iRegionID != 0, "Region should not be 0");
	m_vRegionsToSelfDefend.insert(make_pair(-in_fValue,in_iRegionID));	
}

const multimap<REAL32,UINT32>& GMilitaryPlan::RegionsToDefend() const
{	
	return m_vRegionsToDefend;	
}

const multimap<REAL32,UINT32>& GMilitaryPlan::RegionsToSelfDefend() const
{	
	return m_vRegionsToSelfDefend;
}

void GMilitaryPlan::ClearPlans()
{
	m_vRegionsToAttack.clear();
	m_vRegionsToDefend.clear();
	m_vRegionsToSelfDefend.clear();
}

bool GMilitaryPlan::Serialize(GIBuffer& io_Buffer) const
{
   io_Buffer << m_iCountryID
             << m_vRegionsToAttack
             << m_vRegionsToDefend
             << m_vRegionsToSelfDefend;

   return true;
}

bool GMilitaryPlan::Unserialize(GOBuffer& io_Buffer)
{
   io_Buffer >> m_iCountryID
             >> m_vRegionsToAttack
             >> m_vRegionsToDefend
             >> m_vRegionsToSelfDefend;

   return true;
}
