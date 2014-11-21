#ifndef _EHE_ACTIONS_LOWER_MINIMUM_WAGE
#define _EHE_ACTIONS_LOWER_MINIMUM_WAGE


namespace EHEActions
{


class GLowerMinimumWage : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GLowerMinimumWage* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif