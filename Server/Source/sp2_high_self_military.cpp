#include "golem_pch.hpp"


using namespace EHEObjectives;


GHighSelfMilitary* GHighSelfMilitary::Clone() const
{
   return new GHighSelfMilitary( *this );
}


OBJECTIVE_LEVEL GHighSelfMilitary::CurrentLevel( ENTITY_ID in_iEntity ) const
{
   return 0;
}


OBJECTIVE_LEVEL GHighSelfMilitary::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 0;
}