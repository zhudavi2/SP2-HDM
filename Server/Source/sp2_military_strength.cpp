#include "golem_pch.hpp"


using namespace EHEObjectives;


GMilitaryStrength* GMilitaryStrength::Clone() const
{
   return new GMilitaryStrength( *this );
}


OBJECTIVE_LEVEL GMilitaryStrength::CurrentLevel( ENTITY_ID in_iEntity ) const
{
	REAL32 l_fMilitaryLevel = GAI::AdjustToEHEObjective(SP2::GWorldBehavior::FindMilitaryForceFactor(in_iEntity));

	return l_fMilitaryLevel;	
}


OBJECTIVE_LEVEL GMilitaryStrength::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 0;
}