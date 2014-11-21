#ifndef _EHE_OBJECTIVES_LOW_ENEMIES_MILITARY
#define _EHE_OBJECTIVES_LOW_ENEMIES_MILITARY


namespace EHEObjectives
{


class GLowEnemiesMilitary : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GLowEnemiesMilitary* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif