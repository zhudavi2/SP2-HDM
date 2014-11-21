#ifndef _EHE_ACTIONS_ALLIANCE
#define _EHE_ACTIONS_ALLIANCE


namespace EHEActions
{


class GAlliance : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GAlliance* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif