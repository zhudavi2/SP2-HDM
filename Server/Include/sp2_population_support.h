#ifndef _EHE_OBJECTIVES_POPULATION_SUPPORT
#define _EHE_OBJECTIVES_POPULATION_SUPPORT


namespace EHEObjectives
{


class GPopulationSupport : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GPopulationSupport* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif