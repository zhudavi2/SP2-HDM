#ifndef _EHE_ACTIONS_LOWER_RESOURCE_TAXES
#define _EHE_ACTIONS_LOWER_RESOURCE_TAXES


namespace EHEActions
{


class GLowerResourceTaxes : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GLowerResourceTaxes* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif