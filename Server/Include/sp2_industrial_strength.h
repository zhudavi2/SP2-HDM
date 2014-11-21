#ifndef _EHE_OBJECTIVES_INDUSTRIAL_STRENGTH
#define _EHE_OBJECTIVES_INDUSTRIAL_STRENGTH


namespace EHEObjectives
{


class GIndustrialStrength : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GIndustrialStrength* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif