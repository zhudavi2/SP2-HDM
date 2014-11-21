#ifndef _EHE_OBJECTIVES_STRONG_AIR
#define _EHE_OBJECTIVES_STRONG_AIR


namespace EHEObjectives
{


class GStrongAir : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GStrongAir* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif