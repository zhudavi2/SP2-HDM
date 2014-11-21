#ifndef _EHE_OBJECTIVES_GOVERNMENT_STABILITY
#define _EHE_OBJECTIVES_GOVERNMENT_STABILITY


namespace EHEObjectives
{


class GGovernmentStability : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GGovernmentStability* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif