#ifndef _EHE_ACTIONS_TRAIN_UNITS
#define _EHE_ACTIONS_TRAIN_UNITS


namespace EHEActions
{


class GTrainUnits : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GTrainUnits* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();
};


};

#endif