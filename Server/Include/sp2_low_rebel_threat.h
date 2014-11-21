#ifndef _EHE_OBJECTIVES_LOW_REBEL_THREAT
#define _EHE_OBJECTIVES_LOW_REBEL_THREAT


namespace EHEObjectives
{


class GLowRebelThreat : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GLowRebelThreat* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif