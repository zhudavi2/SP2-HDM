#ifndef _EHE_OBJECTIVES_ECONOMY_TOO_HIGH
#define _EHE_OBJECTIVES_ECONOMY_TOO_HIGH


namespace EHEObjectives
{


class GEconomyTooHigh : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GEconomyTooHigh* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif