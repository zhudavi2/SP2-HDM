#include "golem_pch.hpp"


using namespace EHEObjectives;


GHaveHighTelecomLevel* GHaveHighTelecomLevel::Clone() const
{
   return new GHaveHighTelecomLevel( *this );
}


OBJECTIVE_LEVEL GHaveHighTelecomLevel::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	REAL32 l_fTelecomLevel = g_ServerDAL.CountryData(in_iEntity)->TelecomLevelExpected();
	l_fTelecomLevel = GAI::AdjustToEHEObjective(l_fTelecomLevel);

	return l_fTelecomLevel;
}

OBJECTIVE_LEVEL GHaveHighTelecomLevel::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}