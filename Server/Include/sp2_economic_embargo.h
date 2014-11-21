#ifndef _EHE_ACTIONS_ECONOMIC_EMBARGO
#define _EHE_ACTIONS_ECONOMIC_EMBARGO


namespace EHEActions
{


class GEconomicEmbargo : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GEconomicEmbargo* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	ENTITY_ID FindTarget() const;

	bool CheckHardConditions() const;

	REAL64 m_fWhenToStepForward;

	bool ReadyToStepForward() const;
};


};

#endif