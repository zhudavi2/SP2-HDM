#ifndef _EHE_ACTIONS_BUY_USED_UNITS
#define _EHE_ACTIONS_BUY_USED_UNITS


namespace EHEActions
{


class GBuyUsedUnits : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GBuyUsedUnits* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif