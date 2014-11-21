#ifndef _EHE_ACTIONS_CREATE_SPECOPS_MISSION
#define _EHE_ACTIONS_CREATE_SPECOPS_MISSION


namespace EHEActions
{


class GCreateSpecopsMission : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GCreateSpecopsMission* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif