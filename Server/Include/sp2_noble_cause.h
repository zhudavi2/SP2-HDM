#ifndef _EHE_ACTIONS_NOBLE_CAUSE
#define _EHE_ACTIONS_NOBLE_CAUSE


namespace EHEActions
{


class GNobleCause : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GNobleCause* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif