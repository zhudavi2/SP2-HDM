#ifndef _EHE_ACTIONS_ILLEGAL_ABORTION
#define _EHE_ACTIONS_ILLEGAL_ABORTION


namespace EHEActions
{


class GIllegalAbortion : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GIllegalAbortion* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif