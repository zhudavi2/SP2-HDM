#ifndef _EHE_OBJECTIVES_MILITARY_STRENGTH
#define _EHE_OBJECTIVES_MILITARY_STRENGTH


namespace EHEObjectives
{


class GMilitaryStrength : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GMilitaryStrength* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif