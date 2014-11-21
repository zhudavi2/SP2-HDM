#ifndef _EHE_OBJECTIVES_ENEMIES_NO_MONEY
#define _EHE_OBJECTIVES_ENEMIES_NO_MONEY


namespace EHEObjectives
{


class GEnemiesNoMoney : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GEnemiesNoMoney* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif