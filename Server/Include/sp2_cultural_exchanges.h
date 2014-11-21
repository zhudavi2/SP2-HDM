#ifndef _EHE_ACTIONS_CULTURAL_EXCHANGES
#define _EHE_ACTIONS_CULTURAL_EXCHANGES


namespace EHEActions
{


class GCulturalExchanges : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GCulturalExchanges* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif