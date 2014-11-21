#ifndef _EHE_ACTIONS_REMOVE_PRESSURE
#define _EHE_ACTIONS_REMOVE_PRESSURE


namespace EHEActions
{


class GRemovePressure : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GRemovePressure* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif