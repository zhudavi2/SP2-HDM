#include "golem_pch.hpp"


using namespace EHEObjectives;


GHaveHighHumanDevelopment* GHaveHighHumanDevelopment::Clone() const
{
   return new GHaveHighHumanDevelopment( *this );
}


OBJECTIVE_LEVEL GHaveHighHumanDevelopment::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	REAL32 l_fHD = GWorldBehavior::FindHumanDevelopmentFactor(in_iEntity);
	l_fHD = GAI::AdjustToEHEObjective(l_fHD);
	return l_fHD;
}

OBJECTIVE_LEVEL GHaveHighHumanDevelopment::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}