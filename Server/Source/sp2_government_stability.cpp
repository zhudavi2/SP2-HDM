#include "golem_pch.hpp"


using namespace EHEObjectives;


GGovernmentStability* GGovernmentStability::Clone() const
{
   return new GGovernmentStability( *this );
}


OBJECTIVE_LEVEL GGovernmentStability::CurrentLevel( ENTITY_ID in_iEntity ) const
{

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iEntity);

	return GAI::AdjustToEHEObjective(l_pCountryData->GvtStabilityExpected());
	
}


OBJECTIVE_LEVEL GGovernmentStability::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}