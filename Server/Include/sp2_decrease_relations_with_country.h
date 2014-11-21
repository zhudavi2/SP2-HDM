#ifndef _EHE_ACTIONS_DECREASE_RELATIONS_WITH_COUNTRY
#define _EHE_ACTIONS_DECREASE_RELATIONS_WITH_COUNTRY


namespace EHEActions
{


class GDecreaseRelationsWithCountry : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GDecreaseRelationsWithCountry* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif