#ifndef _EHE_OBJECTIVES_HAVE_HIGH_TELECOM_LEVEL
#define _EHE_OBJECTIVES_HAVE_HIGH_TELECOM_LEVEL


namespace EHEObjectives
{


class GHaveHighTelecomLevel : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GHaveHighTelecomLevel* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif