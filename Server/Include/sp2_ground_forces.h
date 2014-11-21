#ifndef _EHE_ACTIONS_GROUND_FORCES
#define _EHE_ACTIONS_GROUND_FORCES


namespace EHEActions
{


class GBuildGroundForces : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GBuildGroundForces* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
	
	bool ReadyToStepForward() const;
private:
	INT32 m_iUnitInProductionId;
};

class GDisbandGroundForces : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GDisbandGroundForces* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};

class GTrainGroundForces : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GTrainGroundForces* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif