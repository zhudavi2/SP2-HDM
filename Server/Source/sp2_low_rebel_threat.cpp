#include "golem_pch.hpp"


using namespace EHEObjectives;


GLowRebelThreat* GLowRebelThreat::Clone() const
{
   return new GLowRebelThreat( *this );
}


OBJECTIVE_LEVEL GLowRebelThreat::CurrentLevel( ENTITY_ID in_iEntity ) const
{
   return 0;
}


OBJECTIVE_LEVEL GLowRebelThreat::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 0;
}