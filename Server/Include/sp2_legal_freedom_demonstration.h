#ifndef _EHE_ACTIONS_LEGAL_FREEDOM_DEMONSTRATION
#define _EHE_ACTIONS_LEGAL_FREEDOM_DEMONSTRATION


namespace EHEActions
{


class GLegalFreedomOfDemonstration : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GLegalFreedomOfDemonstration* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	REAL64 m_fWhenToStepForward;

	bool ReadyToStepForward() const;
};


};

#endif