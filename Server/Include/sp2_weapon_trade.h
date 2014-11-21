#ifndef _EHE_ACTIONS_WEAPON_TRADE
#define _EHE_ACTIONS_WEAPON_TRADE


namespace EHEActions
{


class GWeaponTrade : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GWeaponTrade* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};

class GWeaponTradeEmbargo : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GWeaponTradeEmbargo* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif