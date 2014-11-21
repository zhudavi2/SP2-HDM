#ifndef _EHE_ACTIONS_COMMON_MARKET
#define _EHE_ACTIONS_COMMON_MARKET


namespace EHEActions
{


class GCommonMarket : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GCommonMarket* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	REAL64 m_fWhenToStepForward;

	bool ReadyToStepForward() const;
};


};

#endif