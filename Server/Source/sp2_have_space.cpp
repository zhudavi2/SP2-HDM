#include "golem_pch.hpp"


using namespace EHEObjectives;


GHaveSpace* GHaveSpace::Clone() const
{
   return new GHaveSpace( *this );
}


OBJECTIVE_LEVEL GHaveSpace::CurrentLevel( ENTITY_ID in_iEntity ) const
{
	REAL32 l_fFactor = GWorldBehavior::FindPopulationDensityFactor(in_iEntity);
	return GAI::AdjustToEHEObjective(l_fFactor);	
}


OBJECTIVE_LEVEL GHaveSpace::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}