#ifndef _EHE_ACTIONS_BUILD_NEW_DESIGN
#define _EHE_ACTIONS_BUILD_NEW_DESIGN


namespace EHEActions
{


class GBuildNewDesign : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GBuildNewDesign* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif