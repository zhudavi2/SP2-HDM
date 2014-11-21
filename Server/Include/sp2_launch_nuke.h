#ifndef _EHE_ACTIONS_LAUNCH_NUKE
#define _EHE_ACTIONS_LAUNCH_NUKE


namespace EHEActions
{


class GLaunchNuke : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GLaunchNuke* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	//! return the country Id where he is going to nuke;
	ENTITY_ID FindTarget() const;

	bool CheckHardConditions() const;
};


};

#endif