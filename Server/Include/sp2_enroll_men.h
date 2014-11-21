#ifndef _EHE_ACTIONS_ENROLL_MEN
#define _EHE_ACTIONS_ENROLL_MEN


namespace EHEActions
{


class GEnrollMen : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GEnrollMen* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool ReadyToStepForward() const;

	bool CheckHardConditions() const;
private:
	INT32 m_iUnitInProductionId;
};

class GTrainInfantry : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GTrainInfantry* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};

class GDisbandInfantry : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GDisbandInfantry* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif