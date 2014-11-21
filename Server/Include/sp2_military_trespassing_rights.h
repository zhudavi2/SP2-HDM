#ifndef _EHE_ACTIONS_MILITARY_TRESPASSING_RIGHTS
#define _EHE_ACTIONS_MILITARY_TRESPASSING_RIGHTS


namespace EHEActions
{


class GMilitaryTrespassingRights : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GMilitaryTrespassingRights* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	ENTITY_ID FindTarget() const;
	
};


};

#endif