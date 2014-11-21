#ifndef _EHE_ACTIONS_AIR_FORCES
#define _EHE_ACTIONS_AIR_FORCES


namespace EHEActions
{


class GBuildAirForces : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GBuildAirForces* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	bool ReadyToStepForward() const;
private:
	INT32 m_iUnitInProductionId;
};

class GDisbandAirForces : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GDisbandAirForces* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};

class GTrainAirForces : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GTrainAirForces* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif