#ifndef _EHE_ACTIONS_LOWER_TAXES
#define _EHE_ACTIONS_LOWER_TAXES


namespace EHEActions
{


class GLowerTaxes : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GLowerTaxes* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	REAL64 m_fWhenToStepForward;

	bool ReadyToStepForward() const;
};


};

#endif