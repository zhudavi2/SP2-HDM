#include "golem_pch.hpp"


using namespace EHEObjectives;


GHaveSpecops* GHaveSpecops::Clone() const
{
   return new GHaveSpecops( *this );
}


OBJECTIVE_LEVEL GHaveSpecops::CurrentLevel( ENTITY_ID in_iEntity ) const
{
   return 0;
}


OBJECTIVE_LEVEL GHaveSpecops::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 0;
}