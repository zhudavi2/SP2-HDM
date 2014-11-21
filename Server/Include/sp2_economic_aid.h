#ifndef _EHE_ACTIONS_ECONOMIC_AID
#define _EHE_ACTIONS_ECONOMIC_AID


namespace EHEActions
{


class GEconomicAidGive : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GEconomicAidGive* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};

class GEconomicAidReceive : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GEconomicAidReceive* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif