#ifndef _EHE_ACTIONS_MOVE_TO_ATTACK
#define _EHE_ACTIONS_MOVE_TO_ATTACK


namespace EHEActions
{


class GMoveToAttack : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GMoveToAttack* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();	

};


};

#endif