#ifndef _EHE_OBJECTIVES_STRONG_GROUND
#define _EHE_OBJECTIVES_STRONG_GROUND


namespace EHEObjectives
{


class GStrongGround : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GStrongGround* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif