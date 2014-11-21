#ifndef _EHE_OBJECTIVES_ENEMIES_LOW_STABILITY
#define _EHE_OBJECTIVES_ENEMIES_LOW_STABILITY


namespace EHEObjectives
{


class GEnemiesLowStability : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GEnemiesLowStability* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif