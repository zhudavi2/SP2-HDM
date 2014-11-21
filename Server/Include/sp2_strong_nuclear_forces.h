#ifndef _EHE_OBJECTIVES_STRONG_NUCLEAR
#define _EHE_OBJECTIVES_STRONG_NUCLEAR


namespace EHEObjectives
{


class GStrongNuclear : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GStrongNuclear* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif