#ifndef _EHE_ACTIONS_ASSUME_FOREIGN_DEBT
#define _EHE_ACTIONS_ASSUME_FOREIGN_DEBT


namespace EHEActions
{


class GAssumeForeignDebtGive : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GAssumeForeignDebtGive* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};

class GAssumeForeignDebtReceive: public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GAssumeForeignDebtReceive* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;
};


};

#endif