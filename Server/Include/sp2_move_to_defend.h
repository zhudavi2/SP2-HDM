#ifndef _EHE_ACTIONS_MOVE_TO_DEFEND
#define _EHE_ACTIONS_MOVE_TO_DEFEND


namespace EHEActions
{


class GMoveToDefend : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GMoveToDefend* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif