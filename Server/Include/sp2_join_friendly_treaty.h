#ifndef _EHE_ACTIONS_JOIN_FRIENDLY_TREATY
#define _EHE_ACTIONS_JOIN_FRIENDLY_TREATY


namespace EHEActions
{

	const REAL32 c_fCriticalResourceFactorBeforeAskingEP = 0.9f;

class GJoinFriendlyTreaty : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GJoinFriendlyTreaty* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

private:
	void FillConditions(UINT32* in_pConditions, UINT32 l_iNbCountriesPossible);
};


};

#endif