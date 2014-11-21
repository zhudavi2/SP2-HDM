#ifndef _EHE_ACTIONS_INCREASE_PRODUCTION
#define _EHE_ACTIONS_INCREASE_PRODUCTION


namespace EHEActions
{


class GIncreaseProduction : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GIncreaseProduction* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	void Conditions( vector<GCondition>& out_Conditions ) const;
};


};

#endif