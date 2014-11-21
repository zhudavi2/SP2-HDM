#include "golem_pch.hpp"


using namespace EHEObjectives;


GHaveHighInfrastructure* GHaveHighInfrastructure::Clone() const
{
   return new GHaveHighInfrastructure( *this );
}


OBJECTIVE_LEVEL GHaveHighInfrastructure::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	REAL32 l_fInfrastructure = g_ServerDAL.CountryData(in_iEntity)->InfrastructureExpected();
	l_fInfrastructure = GAI::AdjustToEHEObjective(l_fInfrastructure);

	return l_fInfrastructure;
}

OBJECTIVE_LEVEL GHaveHighInfrastructure::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}