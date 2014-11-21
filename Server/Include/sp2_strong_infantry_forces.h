#ifndef _EHE_OBJECTIVES_STRONG_INFANTRY
#define _EHE_OBJECTIVES_STRONG_INFANTRY


namespace EHEObjectives
{


class GStrongInfantry : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GStrongInfantry* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif