#include "golem_pch.hpp"


using namespace EHEObjectives;


GLowCorruptionLevel* GLowCorruptionLevel::Clone() const
{
   return new GLowCorruptionLevel( *this );
}


OBJECTIVE_LEVEL GLowCorruptionLevel::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	REAL32 l_fCorruption = 1.f - g_ServerDAL.CountryData(in_iEntity)->CorruptionLevel();
	return GAI::AdjustToEHEObjective(l_fCorruption);
}

OBJECTIVE_LEVEL GLowCorruptionLevel::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}