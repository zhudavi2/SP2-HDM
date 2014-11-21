#ifndef _EHE_ACTIONS_DISBAND_NUKE
#define _EHE_ACTIONS_DISBAND_NUKE


namespace EHEActions
{


class GDisbandNuke : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GDisbandNuke* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif