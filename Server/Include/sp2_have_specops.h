#ifndef _EHE_OBJECTIVES_HAVE_SPECOPS
#define _EHE_OBJECTIVES_HAVE_SPECOPS


namespace EHEObjectives
{


class GHaveSpecops : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GHaveSpecops* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif