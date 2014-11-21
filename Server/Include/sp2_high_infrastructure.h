#ifndef _EHE_OBJECTIVES_HAVE_HIGH_INFRASTRUCTURE
#define _EHE_OBJECTIVES_HAVE_HIGH_INFRASTRUCTURE


namespace EHEObjectives
{


class GHaveHighInfrastructure : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GHaveHighInfrastructure* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif