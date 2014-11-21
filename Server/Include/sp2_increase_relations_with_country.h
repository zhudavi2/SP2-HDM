#ifndef _EHE_ACTIONS_INCREASE_RELATIONS_WITH_COUNTRY
#define _EHE_ACTIONS_INCREASE_RELATIONS_WITH_COUNTRY


namespace EHEActions
{


class GIncreaseRelationsWithCountry : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GIncreaseRelationsWithCountry* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif