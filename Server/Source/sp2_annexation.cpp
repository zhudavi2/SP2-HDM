#include "golem_pch.hpp"


using namespace EHEActions;


GAnnexation* GAnnexation::Clone() const
{
   return new GAnnexation( *this ); 
}


void GAnnexation::Execute()
{
	//GAI::CreateOrJoinTreaty(m_iSource,ETreatyType::Annexation);
	//m_fWhenToStepForward = g_Joshua.GameTime() + 180.f;	

	//MultiMOD
	const vector<GRegionControl>& l_vControls = g_ServerDAL.RegionControlArray();
	const set<UINT32>& l_MilitaryRegions = g_ServerDAL.CountryMilitaryControl(m_iSource);

	for(set<UINT32>::const_iterator it = l_MilitaryRegions.begin(); it != l_MilitaryRegions.end(); it++)
	{
		if(l_vControls[*it].m_iPolitical != m_iSource)
		{
			UINT32 l_iEnemy = l_vControls[*it].m_iPolitical;
			if(g_ServerDAL.DiplomaticStatus(l_iEnemy, m_iSource) == EDiplomaticStatus::Hostile)
			continue;

			if(GAI::ShouldAnnexCountry(m_iSource, l_iEnemy))
			{
				UINT32 l_pConditions[ETreatyConditions::ItemCount];
				Memory::Fill<UINT32>(l_pConditions, 0, ETreatyConditions::ItemCount);
				set<UINT32> l_SideATreaty;
				set<UINT32> l_SideBTreaty;
				set<UINT32> l_SidePressureTreaty;
				GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
				GString l_sCountryName = g_ServerDAL.GetString(l_pCountryData->NameID());

				l_SideATreaty.insert(m_iSource);
				l_SideBTreaty.insert(l_iEnemy);
				GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::Annexation, l_sCountryName);
				g_ServerDCL.CreateNewTreaty(m_iSource, l_SideATreaty, l_SideBTreaty, l_SidePressureTreaty,
				ETreatyType::Annexation, false, l_sTreatyName, l_pConditions);

				m_fWhenToStepForward = g_Joshua.GameTime() + 180.f;
				return;
			}
		}
	}
}

bool GAnnexation::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GAnnexation::CheckHardConditions() const
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