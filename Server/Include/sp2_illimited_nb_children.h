#ifndef _EHE_ACTIONS_ILLIMITED_NB_CHILDREN
#define _EHE_ACTIONS_ILLIMITED_NB_CHILDREN


namespace EHEActions
{


class GIllimitedNbChildren : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GIllimitedNbChildren* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
	REAL64 m_fWhenToStepForward;

	bool ReadyToStepForward() const;
};


};

#endif