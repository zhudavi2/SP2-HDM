#include "golem_pch.hpp"


using namespace EHEObjectives;


GEnemiesNoMilitary* GEnemiesNoMilitary::Clone() const
{
   return new GEnemiesNoMilitary( *this );
}


OBJECTIVE_LEVEL GEnemiesNoMilitary::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	REAL32 l_fTotalUnitsAI = g_ServerDAL.TotalUnitsValue(in_iEntity);
	REAL32 l_fTotalUnitsEnemies = 0.f;	
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<= l_iNbCountry; i++)
	{		
		if(in_iEntity == i || !g_ServerDAL.CountryIsValid(i))
			continue;
			
		if(g_ServerDAL.RelationBetweenCountries(in_iEntity,i) < SP2::c_fRelationsEnnemy)
			l_fTotalUnitsEnemies += g_ServerDAL.TotalUnitsValue(i);
	}

	if(l_fTotalUnitsEnemies == 0.f)
		return 1.f;

	REAL32 l_fRatio = l_fTotalUnitsAI / l_fTotalUnitsEnemies;

	if(l_fRatio >= 5.f)
		return 1.f;
	else if(l_fRatio >= 1.f)
		return (0.25f * l_fRatio) - 0.25f;
	else if(l_fRatio >= 0.2f)
		return (1.25f * l_fRatio) - 1.25f;
	else
		return -1.f;

}

OBJECTIVE_LEVEL GEnemiesNoMilitary::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}