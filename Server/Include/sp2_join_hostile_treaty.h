#ifndef _EHE_ACTIONS_JOIN_HOSTILE_TREATY
#define _EHE_ACTIONS_JOIN_HOSTILE_TREATY


namespace EHEActions
{


class GJoinHostileTreaty : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GJoinHostileTreaty* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif