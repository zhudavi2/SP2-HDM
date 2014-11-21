#ifndef _EHE_ACTIONS_NUCLEARIZE_COUNTRY
#define _EHE_ACTIONS_NUCLEARIZE_COUNTRY


namespace EHEActions
{


class GNuclearizeCountry : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GNuclearizeCountry* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif