#include "golem_pch.hpp"


using namespace EHEObjectives;


GPopulationSupport* GPopulationSupport::Clone() const
{
   return new GPopulationSupport( *this );
}


OBJECTIVE_LEVEL GPopulationSupport::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iEntity);

	return GAI::AdjustToEHEObjective(l_pCountryData->GvtApprovalExpected());
}


OBJECTIVE_LEVEL GPopulationSupport::OptimalLevel( ENTITY_ID in_iEntity ) const
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iEntity);
	if((EGovernmentType::Enum)l_pCountryData->GvtType() == EGovernmentType::MultiPartyDemocracy)
		return 1.f;
	
	return 0.f;
}