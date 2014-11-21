#ifndef _EHE_OBJECTIVES_HAVE_MISSILES
#define _EHE_OBJECTIVES_HAVE_MISSILES


namespace EHEObjectives
{


class GHaveMissiles : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GHaveMissiles* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif