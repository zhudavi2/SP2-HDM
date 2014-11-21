#ifndef _EHE_ACTIONS_DECLARE_MARTIAL_LAW
#define _EHE_ACTIONS_DECLARE_MARTIAL_LAW


namespace EHEActions
{


class GDeclareMartialLaw : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GDeclareMartialLaw* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif