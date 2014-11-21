#ifndef _EHE_OBJECTIVES_HAVE_MILITARY_ACCESS
#define _EHE_OBJECTIVES_HAVE_MILITARY_ACCESS


namespace EHEObjectives
{


class GHaveMilitaryAccess : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GHaveMilitaryAccess* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif