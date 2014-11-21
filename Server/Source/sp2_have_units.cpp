#include "golem_pch.hpp"


using namespace EHEObjectives;


GHaveUnits* GHaveUnits::Clone() const
{
   return new GHaveUnits( *this );
}


OBJECTIVE_LEVEL GHaveUnits::CurrentLevel( ENTITY_ID in_iEntity ) const
{
   return 0;
}


OBJECTIVE_LEVEL GHaveUnits::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 0;
}