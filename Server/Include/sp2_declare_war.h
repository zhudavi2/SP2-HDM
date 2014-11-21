#ifndef _EHE_ACTIONS_DECLARE_WAR
#define _EHE_ACTIONS_DECLARE_WAR


namespace EHEActions
{


class GDeclareWar : public SDK::EHE::GAction
{
   //! object must be cloneable (deep copy) for registration in the manager
   GDeclareWar* Clone() const;

	//! return the country Id where he is going to attack;
	ENTITY_ID FindTarget() const;

   //! let the entity execute the action on the specified target with the given strength
   void Execute();

	//! Return yes if the country isn't at war anymore
	bool ReadyToStepForward() const;

	//! Return yes if the country feels he can beat the other country
	bool CheckHardConditions() const;
};


};

#endif