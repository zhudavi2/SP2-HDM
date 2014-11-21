#ifndef _EHE_ACTIONS_LEGAL_FREEDOM_OF_SPEECH
#define _EHE_ACTIONS_LEGAL_FREEDOM_OF_SPEECH


namespace EHEActions
{


class GLegalFreedomOfSpeech : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GLegalFreedomOfSpeech* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif