#ifndef _EHE_ACTIONS_CONTROL_RESOURCE
#define _EHE_ACTIONS_CONTROL_RESOURCE


namespace EHEActions
{


class GPrivateResourceControl : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GPrivateResourceControl* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	REAL64 m_fWhenToStepForward;

	bool ReadyToStepForward() const;
};

class GPublicResourceControl : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GPublicResourceControl* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	REAL64 m_fWhenToStepForward;

	bool ReadyToStepForward() const;
};


};

#endif