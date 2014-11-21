#ifndef _EHE_ACTIONS_TRADE_INS
#define _EHE_ACTIONS_TRADE_INS


namespace EHEActions
{


class GTradeInRegion : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GTradeInRegion* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
	
	ENTITY_ID FindTarget() const;
};

class GTradeInResearch : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GTradeInResearch* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif