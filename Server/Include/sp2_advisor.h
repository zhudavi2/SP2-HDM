/**************************************************************
*
* sp2_advisor.h
*
* Description
* ===========
*  Contains advisor information, by country
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SP2_ADVISOR_H_
#define _GOLEM_SP2_ADVISOR_H_

namespace SP2
{	
	class GPlan;

   class GAdvisor
   {
   protected:
   public:
      GAdvisor(EAdvisorStatus::Enum in_eEconomic, EAdvisorStatus::Enum in_ePolitic, EAdvisorStatus::Enum in_eMilitary);
      ~GAdvisor();				

		EAdvisorStatus::Enum MilitaryStatus() const;
		EAdvisorStatus::Enum EconomicStatus() const;
		EAdvisorStatus::Enum PoliticStatus() const;

		void MilitaryStatus(EAdvisorStatus::Enum);
		void EconomicStatus(EAdvisorStatus::Enum);
		void PoliticStatus(EAdvisorStatus::Enum);

		//! Give order to refuse every plan in the hash_map. The AI will execute none
		void CancelPendingPlans();

		//! Accept all pending plans. To be called when a player leaves the game
		void AcceptAllPendingPlans();

		//! Accept the plan for the specified objective id
		void AcceptPendingPlan(OBJECTIVE_ID);

		//! Add a plan for an objective
		/*!
		*	@param in_iObjective : Objective id for that plan
		*	@param in_pPlan: Plan that the advisor will follow
		**/
		void AddPlanToObjective(OBJECTIVE_ID in_iObjective, GPlan* in_pPlan);

		//! Gives the pending plan for a specified objective
		const SP2::GPlan* GetPlan(OBJECTIVE_ID) const;

		//! Get the status of a pending plan
		EPlanStatus::Enum GetPlanStatus(OBJECTIVE_ID) const;

		//! Remove existing plan for that objective id
		void RemovePlan(OBJECTIVE_ID);

	private:		

		EAdvisorStatus::Enum m_eEconomicStatus;
		EAdvisorStatus::Enum m_eMilitaryStatus;
		EAdvisorStatus::Enum m_ePoliticStatus;
		
		//! Hash map containing, for each objective, a plan and its status waiting to be answer by the player
		hash_map<OBJECTIVE_ID ,pair<SP2::GPlan*,EPlanStatus::Enum> > m_Plans;

   };
};

#endif //_GOLEM_SP2_ADVISOR_H_