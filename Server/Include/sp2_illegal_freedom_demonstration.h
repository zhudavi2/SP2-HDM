#ifndef _EHE_ACTIONS_ILLEGAL_FREEDOM_DEMONSTRATION
#define _EHE_ACTIONS_ILLEGAL_FREEDOM_DEMONSTRATION


namespace EHEActions
{


class GIllegalFreedomOfDemonstration : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GIllegalFreedomOfDemonstration* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	REAL64 m_fWhenToStepForward;

	bool ReadyToStepForward() const;
};


};

#endif