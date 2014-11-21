#ifndef _EHE_ACTIONS_ILLEGAL_FREEDOM_OF_SPEECH
#define _EHE_ACTIONS_ILLEGAL_FREEDOM_OF_SPEECH


namespace EHEActions
{


class GIllegalFreedomOfSpeech : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GIllegalFreedomOfSpeech* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif