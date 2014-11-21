#ifndef _EHE_ACTIONS_IMPROVE_STATUS_REL_LANG
#define _EHE_ACTIONS_IMPROVE_STATUS_REL_LANG


namespace EHEActions
{


class GImproveStatusRelLang : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GImproveStatusRelLang* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif