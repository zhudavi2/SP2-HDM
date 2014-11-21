#ifndef _EHE_ACTIONS_COVERT_ACTIONS_EHE
#define _EHE_ACTIONS_COVERT_ACTIONS_EHE


namespace EHEActions
{

	const REAL32 c_fWillingnessToTry = 0.15f;

class GAssassination : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GAssassination* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	ENTITY_ID FindTarget() const;
};

class GEspionageCivilian : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GEspionageCivilian* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	ENTITY_ID FindTarget() const;
};

class GEspionageMilitary : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GEspionageMilitary* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	ENTITY_ID FindTarget() const;
};

class GSabotageCivilian : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GSabotageCivilian* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	ENTITY_ID FindTarget() const;
};

class GSabotageMilitary : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GSabotageMilitary* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	ENTITY_ID FindTarget() const;
};

class GTerrorism : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GTerrorism* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	ENTITY_ID FindTarget() const;
};

class GCoupEtat : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GCoupEtat* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

	ENTITY_ID FindTarget() const;
};

class GCreateSpecOpsCells : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GCreateSpecOpsCells* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};

class GDisbandSpecOpsCells : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GDisbandSpecOpsCells* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif