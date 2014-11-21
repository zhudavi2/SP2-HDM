#ifndef _EHE_ACTIONS_HUMAN_DEVELOPMENT
#define _EHE_ACTIONS_HUMAN_DEVELOPMENT


namespace EHEActions
{


class GHumanDevelopmentCollaboration : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GHumanDevelopmentCollaboration* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	REAL64 m_fWhenToStepForward;

	bool ReadyToStepForward() const;
};


};

#endif