#include "golem_pch.hpp"


using namespace EHEObjectives;


GHaveMilitaryAccess* GHaveMilitaryAccess::Clone() const
{
   return new GHaveMilitaryAccess( *this );
}


OBJECTIVE_LEVEL GHaveMilitaryAccess::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	//Get the list of enemies
	set<UINT32> l_Enemies;
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<= l_iNbCountry; i++)
	{		
		if(in_iEntity == i || !g_ServerDAL.CountryIsValid(i))
			continue;
			
		if(g_ServerDAL.RelationBetweenCountries(in_iEntity,i) < SP2::c_fRelationsEnnemy)
			l_Enemies.insert(i);		
	}
	 
	if(l_Enemies.size() == 0)
		return 1.f;
	
	//Find starting region, for checks
	UINT32 l_iSourceRegion = 0;
	const SCityInfo* l_pCapitalSource = g_ServerDAL.CapitalCity(in_iEntity);
	if(l_pCapitalSource)
		l_iSourceRegion = l_pCapitalSource->m_iRegionId;
	else if(g_ServerDAL.CountryPoliticalControl(in_iEntity).size())
		l_iSourceRegion = *g_ServerDAL.CountryPoliticalControl(in_iEntity).begin();
	else if(g_ServerDAL.CountryMilitaryControl(in_iEntity).size())
		l_iSourceRegion = *g_ServerDAL.CountryMilitaryControl(in_iEntity).begin();
	else
		return -1.f;

	UINT32 l_iNbOfCountriesWithAccess = 0;
	REAL32 l_fRatioAccess = 1.f;
	for(set<UINT32>::iterator it = l_Enemies.begin(); it != l_Enemies.end(); it++)
	{
		//Does this country has access to a region of the enemy?
		UINT32 l_iDestinationRegion = 0;
		const SCityInfo* l_pCapital = g_ServerDAL.CapitalCity(*it);
		if(l_pCapital)
			l_iDestinationRegion = l_pCapital->m_iRegionId;
		else if(g_ServerDAL.CountryPoliticalControl(*it).size())
			l_iDestinationRegion = *g_ServerDAL.CountryPoliticalControl(*it).begin();
		else if(g_ServerDAL.CountryMilitaryControl(*it).size())
			l_iDestinationRegion = *g_ServerDAL.CountryMilitaryControl(*it).begin();
		else
		{
			l_iNbOfCountriesWithAccess++;
			continue;
		}

		if(g_ServerDCL.MoveHostileFuturePossible(in_iEntity,l_iSourceRegion,l_iDestinationRegion))
			l_iNbOfCountriesWithAccess++;
	}

	l_fRatioAccess = (REAL32)l_iNbOfCountriesWithAccess / (REAL32)l_Enemies.size();

	return GAI::AdjustToEHEObjective(l_fRatioAccess);

}

OBJECTIVE_LEVEL GHaveMilitaryAccess::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}