#ifndef _EHE_ACTIONS_LEGAL_ABORTION
#define _EHE_ACTIONS_LEGAL_ABORTION


namespace EHEActions
{


class GLegalAbortion : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GLegalAbortion* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif