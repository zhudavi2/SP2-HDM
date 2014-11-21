#ifndef _EHE_ACTIONS_TRAIN_SPECOPS
#define _EHE_ACTIONS_TRAIN_SPECOPS


namespace EHEActions
{


class GTrainSpecops : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GTrainSpecops* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif