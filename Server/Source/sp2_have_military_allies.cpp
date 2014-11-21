#include "golem_pch.hpp"


using namespace EHEObjectives;


GHaveMilitaryAllies* GHaveMilitaryAllies::Clone() const
{
   return new GHaveMilitaryAllies( *this );
}


OBJECTIVE_LEVEL GHaveMilitaryAllies::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	//1 = 5x more units than enemies
	//0 = same ratio
	//-1 = 5x less units than enemies

	REAL32 l_fAlliesStrength = g_ServerDAL.TotalUnitsValueWithAllies(in_iEntity);
	UINT32 l_iNbCountries = g_ServerDAL.NbCountry();
	set<UINT32> l_Enemies;

	for(UINT32 i=1; i<= l_iNbCountries; i++)
	{
		if(g_ServerDAL.CountryIsValid(i))
		{
			if(g_ServerDAL.RelationBetweenCountries(i,in_iEntity) < SP2::c_fRelationsEnnemy)
			{
				l_Enemies.insert(i);
				g_ServerDAL.CountryAllies(i,l_Enemies);
			}
		}
	}

	REAL32 l_fStrengthOfEnemies = 0.f;
	for(set<UINT32>::iterator it = l_Enemies.begin();
		 it != l_Enemies.end(); it++)
	{
		l_fStrengthOfEnemies += g_ServerDAL.TotalUnitsValue(*it);
	}

	if(l_fStrengthOfEnemies == 0.f)
		return 1.f;

	REAL32 l_fRatio = l_fAlliesStrength / l_fStrengthOfEnemies;

	if(l_fRatio >= 5.f)
		return 1.f;
	else if(l_fRatio >= 1.f)
		return (0.25f * l_fRatio) - 0.25f;
	else if(l_fRatio >= 0.2f)
		return (1.25f * l_fRatio) - 1.25f;
	
	return -1.f;

}

OBJECTIVE_LEVEL GHaveMilitaryAllies::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}