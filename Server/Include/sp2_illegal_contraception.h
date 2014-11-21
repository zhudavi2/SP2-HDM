#ifndef _EHE_ACTIONS_ILLEGAL_CONTRACEPTION
#define _EHE_ACTIONS_ILLEGAL_CONTRACEPTION


namespace EHEActions
{


class GIllegalContraception : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GIllegalContraception* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif