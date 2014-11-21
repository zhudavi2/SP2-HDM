#ifndef _EHE_ACTIONS_NAVAL_FORCES
#define _EHE_ACTIONS_NAVAL_FORCES


namespace EHEActions
{


class GBuildNavalForces : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GBuildNavalForces* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
	
	bool ReadyToStepForward() const;
private:
	INT32 m_iUnitInProductionId;
};

class GDisbandNavalForces : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GDisbandNavalForces* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};

class GTrainNavalForces : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GTrainNavalForces* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif