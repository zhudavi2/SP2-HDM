#include "golem_pch.hpp"


using namespace EHEObjectives;


GHaveMissiles* GHaveMissiles::Clone() const
{
   return new GHaveMissiles( *this );
}


OBJECTIVE_LEVEL GHaveMissiles::CurrentLevel( ENTITY_ID in_iEntity ) const
{
   return 0;
}


OBJECTIVE_LEVEL GHaveMissiles::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 0;
}