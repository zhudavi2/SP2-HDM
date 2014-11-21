#ifndef _EHE_OBJECTIVES_HAVE_SPACE
#define _EHE_OBJECTIVES_HAVE_SPACE


namespace EHEObjectives
{


class GHaveSpace : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GHaveSpace* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif