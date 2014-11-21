#ifndef _EHE_ACTIONS_EMIGRATION_IMMIGRATION
#define _EHE_ACTIONS_EMIGRATION_IMMIGRATION


namespace EHEActions
{


class GOpenImmigration : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GOpenImmigration* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};

class GCloseImmigration : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GCloseImmigration* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};

class GOpenEmigration : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GOpenEmigration* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};

class GCloseEmigration : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GCloseEmigration* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif