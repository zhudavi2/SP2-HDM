#ifndef _EHE_ACTIONS_RESEARCH_PARTNERSHIP
#define _EHE_ACTIONS_RESEARCH_PARTNERSHIP


namespace EHEActions
{


class GResearchPartnership : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GResearchPartnership* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif