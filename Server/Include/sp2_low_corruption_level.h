#ifndef _EHE_OBJECTIVES_LOW_CORRUPTION_LEVEL
#define _EHE_OBJECTIVES_LOW_CORRUPTION_LEVEL


namespace EHEObjectives
{


class GLowCorruptionLevel : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GLowCorruptionLevel* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif