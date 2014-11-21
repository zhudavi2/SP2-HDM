#include "golem_pch.hpp"


using namespace EHEObjectives;


GHaveHighEconomicHealth* GHaveHighEconomicHealth::Clone() const
{
   return new GHaveHighEconomicHealth( *this );
}


OBJECTIVE_LEVEL GHaveHighEconomicHealth::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	REAL32 l_fEconomicHealth = GWorldBehavior::FindEconomicHealth(in_iEntity);
	l_fEconomicHealth = GAI::AdjustToEHEObjective(l_fEconomicHealth);
	return l_fEconomicHealth;
}

OBJECTIVE_LEVEL GHaveHighEconomicHealth::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}