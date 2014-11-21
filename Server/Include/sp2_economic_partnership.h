#ifndef _EHE_ACTIONS_ECONOMIC_PARTNERSHIP
#define _EHE_ACTIONS_ECONOMIC_PARTNERSHIP


namespace EHEActions
{


class GEconomicPartnership : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GEconomicPartnership* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif