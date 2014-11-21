#ifndef _EHE_OBJECTIVES_HAVE_GOOD_RESEARCH_LEVEL
#define _EHE_OBJECTIVES_HAVE_GOOD_RESEARCH_LEVEL


namespace EHEObjectives
{


class GHaveGoodResearchLevel : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GHaveGoodResearchLevel* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif