#ifndef _EHE_ACTIONS_RAISE_TAXES
#define _EHE_ACTIONS_RAISE_TAXES


namespace EHEActions
{


class GRaiseTaxes : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GRaiseTaxes* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	REAL64 m_fWhenToStepForward;

	bool ReadyToStepForward() const;

};


};

#endif