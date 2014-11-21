#ifndef _EHE_ACTIONS_INCREASE_CONTROL_ON_POPULATION
#define _EHE_ACTIONS_INCREASE_CONTROL_ON_POPULATION


namespace EHEActions
{


class GIncreaseControlOnPopulation : public SDK::EHE::GAction
{
   // object must be cloneable (deep copy) for registration in the manager
   GIncreaseControlOnPopulation* Clone() const;

   // let the entity execute the action on the specified target with the given strength
   void Execute();

	bool CheckHardConditions() const;

private:

	void ExecuteRaiseSuffrage(const GEntity* in_pEntity) const;
	void ExecuteCloseEmigration(const GEntity* in_pEntity) const;
	void ExecuteCloseImmigration(const GEntity* in_pEntity) const;
	void ExecuteChangeLaws(const GEntity* in_pEntity) const;
	void ExecuteStatusReligion(const GEntity* in_pEntity) const;
	void ExecuteStatusLanguage(const GEntity* in_pEntity) const;
};


};

#endif