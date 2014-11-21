#include "golem_pch.hpp"


using namespace EHEActions;


GFreeRegion* GFreeRegion::Clone() const
{
   return new GFreeRegion( *this ); 
}


void GFreeRegion::Execute()
{
	GAI::CreateOrJoinTreaty(m_iSource,ETreatyType::FreeRegion);
}

bool GFreeRegion::CheckHardConditions() const
{	
	//Check if a this country has control over another region

	const vector<GRegionControl>& l_vControls = g_ServerDAL.RegionControlArray();
	const set<UINT32>& l_MilitaryRegions = g_ServerDAL.CountryMilitaryControl(m_iSource);

	for(set<UINT32>::const_iterator it = l_MilitaryRegions.begin();
		 it != l_MilitaryRegions.end(); it++)
	{
		if(l_vControls[*it].m_iPolitical != m_iSource)
			return true;
	}

	return false;
}