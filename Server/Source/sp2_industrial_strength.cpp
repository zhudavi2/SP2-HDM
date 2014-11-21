#include "golem_pch.hpp"


using namespace EHEObjectives;


GIndustrialStrength* GIndustrialStrength::Clone() const
{
   return new GIndustrialStrength( *this );
}


OBJECTIVE_LEVEL GIndustrialStrength::CurrentLevel( ENTITY_ID in_iEntity ) const
{
	REAL32 l_fResFactor = GWorldBehavior::FindResourceFactor(in_iEntity);
	return GAI::AdjustToEHEObjective(l_fResFactor);
}


OBJECTIVE_LEVEL GIndustrialStrength::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}