#ifndef _EHE_ACTIONS_RAISE_MINIMUM_WAGE
#define _EHE_ACTIONS_RAISE_MINIMUM_WAGE


namespace EHEActions
{


class GRaiseMinimumWage : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GRaiseMinimumWage* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif