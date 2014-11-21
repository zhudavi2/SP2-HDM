#ifndef _EHE_ACTIONS_ASK_FOR_HELP
#define _EHE_ACTIONS_ASK_FOR_HELP


namespace EHEActions
{


class GAskForHelp : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GAskForHelp* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif