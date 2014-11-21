#ifndef _EHE_ACTIONS_RAISE_RESOURCE_TAXES
#define _EHE_ACTIONS_RAISE_RESOURCE_TAXES


namespace EHEActions
{


class GRaiseResourceTaxes : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GRaiseResourceTaxes* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif