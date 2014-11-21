#ifndef _EHE_OBJECTIVES_HIGH_FRIENDS_MILITARY
#define _EHE_OBJECTIVES_HIGH_FRIENDS_MILITARY


namespace EHEObjectives
{


class GHighFriendsMilitary : public GObjective
{
   // object must be cloneable (deep copy) for registration in the manager
   GHighFriendsMilitary* Clone() const;

   // get current level for given entity
   OBJECTIVE_LEVEL CurrentLevel( ENTITY_ID in_iEntity ) const;

   // get the optimal level for given entity
   OBJECTIVE_LEVEL OptimalLevel( ENTITY_ID in_iEntity ) const;
};


};

#endif