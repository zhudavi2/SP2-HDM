#ifndef _EHE_ACTIONS_LEGAL_CONTRACEPTION
#define _EHE_ACTIONS_LEGAL_CONTRACEPTION


namespace EHEActions
{


class GLegalContraception : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GLegalContraception* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif