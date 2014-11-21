#ifndef _EHE_ACTIONS_RAISE_INTEREST_RATE
#define _EHE_ACTIONS_RAISE_INTEREST_RATE


namespace EHEActions
{


class GRaiseInterestRate : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GRaiseInterestRate* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif