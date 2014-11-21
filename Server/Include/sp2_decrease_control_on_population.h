#ifndef _EHE_ACTIONS_DECREASE_CONTROL_ON_POPULATION
#define _EHE_ACTIONS_DECREASE_CONTROL_ON_POPULATION


namespace EHEActions
{


class GDecreaseControlOnPopulation : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GDecreaseControlOnPopulation* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

private:

	void ExecuteLowerSuffrage(const GEntity* in_pEntity) const;
	void ExecuteOpenEmigration(const GEntity* in_pEntity) const;
	void ExecuteOpenImmigration(const GEntity* in_pEntity) const;
	void ExecuteChangeLaws(const GEntity* in_pEntity) const;
	void ExecuteStatusReligion(const GEntity* in_pEntity) const;
	void ExecuteStatusLanguage(const GEntity* in_pEntity) const;
};


};

#endif