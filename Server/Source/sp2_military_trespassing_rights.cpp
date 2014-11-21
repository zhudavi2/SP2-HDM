#include "golem_pch.hpp"


using namespace EHEActions;


GMilitaryTrespassingRights* GMilitaryTrespassingRights::Clone() const
{
   return new GMilitaryTrespassingRights( *this ); 
}


ENTITY_ID GMilitaryTrespassingRights::FindTarget() const
{

	
	//Get the list of enemies
	set<UINT32> l_Enemies;
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<= l_iNbCountry; i++)
	{		
		if(m_iSource == i || !g_ServerDAL.CountryIsValid(i))
			continue;
			
		if(g_ServerDAL.RelationBetweenCountries(m_iSource,i) < SP2::c_fRelationsEnnemy)
			l_Enemies.insert(i);		
	}
	 
	if(l_Enemies.size() == 0)
		return 0;
	
	//Find starting region, for checks
	UINT32 l_iSourceRegion = 0;
	const SCityInfo* l_pCapitalSource = g_ServerDAL.CapitalCity(m_iSource);
	if(l_pCapitalSource)
		l_iSourceRegion = l_pCapitalSource->m_iRegionId;
	else if(g_ServerDAL.CountryPoliticalControl(m_iSource).size())
		l_iSourceRegion = *g_ServerDAL.CountryPoliticalControl(m_iSource).begin();
	else if(g_ServerDAL.CountryMilitaryControl(m_iSource).size())
		l_iSourceRegion = *g_ServerDAL.CountryMilitaryControl(m_iSource).begin();
	else
		return 0;
	
	UINT32 l_iCountryLookingToGoThrough = 0;
	UINT32 l_iDestinationRegion = 0;
	for(set<UINT32>::iterator it = l_Enemies.begin(); it != l_Enemies.end(); it++)
	{
		//Does this country has access to a region of the enemy?		
		l_iDestinationRegion = 0;
		const SCityInfo* l_pCapital = g_ServerDAL.CapitalCity(*it);
		if(l_pCapital)
			l_iDestinationRegion = l_pCapital->m_iRegionId;
		else if(g_ServerDAL.CountryPoliticalControl(*it).size())
			l_iDestinationRegion = *g_ServerDAL.CountryPoliticalControl(*it).begin();
		else if(g_ServerDAL.CountryMilitaryControl(*it).size())
			l_iDestinationRegion = *g_ServerDAL.CountryMilitaryControl(*it).begin();
		else
			continue;

		if(!g_ServerDCL.MoveHostileFuturePossible(m_iSource,l_iSourceRegion,l_iDestinationRegion))
		{
			l_iCountryLookingToGoThrough = *it;
			break;
		}
	}

	if(!l_iCountryLookingToGoThrough)
		return 0;

	//We need to find a way to go to this country
	return g_ServerDCL.CountryToPassThroughTo(m_iSource,l_iSourceRegion,l_iDestinationRegion);
}

void GMilitaryTrespassingRights::Execute()
{
	if(m_iTarget)
	{
		GString l_sNewTreatyName = g_ServerDAL.TreatyName(ETreatyType::MilitaryAccess,g_ServerDAL.GetString(g_ServerDAL.CountryData(m_iSource)->NameID()));
		UINT32 l_Conditions[ETreatyConditions::ItemCount];
		Memory::Fill<UINT32>(l_Conditions,0,ETreatyConditions::ItemCount);
		set<UINT32> l_SideAMembers;
		set<UINT32> l_SideBMembers;
		set<UINT32> l_SidePressure;
		l_SideBMembers.insert(m_iSource);
		l_SideAMembers.insert(m_iTarget);
		g_ServerDCL.CreateNewTreaty(m_iSource,l_SideAMembers,l_SideBMembers,l_SidePressure,ETreatyType::MilitaryAccess,true,l_sNewTreatyName,l_Conditions);
	}	
}

bool GMilitaryTrespassingRights::CheckHardConditions() const
{	
	set<UINT32> l_Enemies;
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<= l_iNbCountry; i++)
	{		
		if(m_iSource == i || !g_ServerDAL.CountryIsValid(i))
			continue;
			
		if(g_ServerDAL.RelationBetweenCountries(m_iSource,i) < SP2::c_fRelationsEnnemy)
			l_Enemies.insert(i);		
	}
	 
	if(l_Enemies.size() == 0)
		return false;
	return true;
}

