#ifndef _EHE_OBJECTIVES_HAVE_HIGH_NATIONAL_SECURITY
#define _EHE_OBJECTIVES_HAVE_HIGH_NATIONAL_SECURITY


namespace EHEObjectives
{


class GHaveHighNationalSecurity : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GHaveHighNationalSecurity* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif