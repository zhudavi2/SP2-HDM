#ifndef _EHE_ACTIONS_CHANGE_TO_COMMUNISM
#define _EHE_ACTIONS_CHANGE_TO_COMMUNISM


namespace EHEActions
{


class GChangeToCommunism : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GChangeToCommunism* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	REAL64 m_fWhenToStepForward;

	bool ReadyToStepForward() const;
};


};

#endif