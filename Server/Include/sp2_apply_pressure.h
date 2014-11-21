#ifndef _EHE_ACTIONS_APPLY_PRESSURE
#define _EHE_ACTIONS_APPLY_PRESSURE


namespace EHEActions
{


class GApplyPressure : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GApplyPressure* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif