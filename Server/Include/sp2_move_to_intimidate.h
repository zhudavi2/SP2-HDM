#ifndef _EHE_ACTIONS_MOVE_TO_INTIMIDATE
#define _EHE_ACTIONS_MOVE_TO_INTIMIDATE


namespace EHEActions
{


class GMoveToIntimidate : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GMoveToIntimidate* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif