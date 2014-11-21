#ifndef _EHE_OBJECTIVES_HAVE_HIGH_HUMAN_DEVELOPMENT
#define _EHE_OBJECTIVES_HAVE_HIGH_HUMAN_DEVELOPMENT


namespace EHEObjectives
{


class GHaveHighHumanDevelopment : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GHaveHighHumanDevelopment* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif