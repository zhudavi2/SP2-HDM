#include "golem_pch.hpp"


using namespace EHEObjectives;


GEnemiesLowStability* GEnemiesLowStability::Clone() const
{
   return new GEnemiesLowStability( *this );
}


OBJECTIVE_LEVEL GEnemiesLowStability::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	//Get the list of enemies
	REAL32 l_fTotalStability = 0.f;
	UINT32 l_iNbEnemies = 0;
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<= l_iNbCountry; i++)
	{		
		if(in_iEntity == i || !g_ServerDAL.CountryIsValid(i))
			continue;
			
		if(g_ServerDAL.RelationBetweenCountries(in_iEntity,i) < SP2::c_fRelationsEnnemy)
		{
			l_fTotalStability += g_ServerDAL.CountryData(i)->GvtStabilityExpected();
			l_iNbEnemies++;
		}	
			
	}

	if(!l_iNbEnemies)
		return 1.f;

	REAL32 l_fAverageStability = 1.f - (l_fTotalStability / (REAL32)l_iNbEnemies);
	return GAI::AdjustToEHEObjective(l_fAverageStability);
}

OBJECTIVE_LEVEL GEnemiesLowStability::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}