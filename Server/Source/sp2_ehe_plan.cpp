/**************************************************************
*
* sp2_ehe_plan.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"


SDK::EHE::GPlan* SP2::GPlan::New()
{
	return new SP2::GPlan();
}


SP2::GPlan::GPlan() : SDK::EHE::GPlan()
{   
}

EPlanStatus::Enum SP2::GPlan::GoOnWithPlan()
{
	
	UINT32 l_iCountryID = ((SDK::EHE::GPlan*)this)->Entity();	
	UINT32 l_iObjectiveID = ((SDK::EHE::GPlan*)this)->Objective();		

	GAdvisor* l_pAdvisor = g_ServerDAL.CountryData(l_iCountryID)->Advisor();	

	if(!l_pAdvisor)
		return EPlanStatus::Ready;

	if(l_pAdvisor->GetPlan(l_iObjectiveID) == NULL)
	{
		//that plan was never asked to the player

		EEHECategory::Enum l_eCategory = g_ServerDAL.EHEObjectiveCategory(l_iObjectiveID);

		switch(l_eCategory)
		{
		case EEHECategory::Economic:
			if(l_pAdvisor->EconomicStatus() == EAdvisorStatus::Manual)
				return EPlanStatus::Cancel;
		case EEHECategory::Military:
			if(l_pAdvisor->MilitaryStatus() == EAdvisorStatus::Manual)
				return EPlanStatus::Cancel;
		case EEHECategory::Politic:
			if(l_pAdvisor->PoliticStatus() == EAdvisorStatus::Manual)
				return EPlanStatus::Cancel;
		}

		bool l_bContainsEconomic = false;
		bool l_bContainsPolitic = false;
		bool l_bContainsMilitary = false;

		vector<pair<UINT32,UINT32> > l_vActionsIds;
		const list<SDK::EHE::GAction*>& l_vActions = ((SDK::EHE::GPlan*)this)->ScheduledActions();

		for(list<SDK::EHE::GAction*>::const_iterator it = l_vActions.begin();
			 it != l_vActions.end(); it++)
		{
			l_vActionsIds.push_back(make_pair((*it)->Target(),(*it)->Id()));
		
			switch(g_ServerDAL.EHEActionCategory((*it)->Id()))
			{
			case EEHECategory::Economic:
				l_bContainsEconomic = true;
				break;
			case EEHECategory::Politic:
				l_bContainsPolitic = true;
				break;
			case EEHECategory::Military:
				l_bContainsMilitary = true;
				break;
			}
		}

		switch(l_eCategory)
		{
		case EEHECategory::Economic:
			if(l_pAdvisor->EconomicStatus() == EAdvisorStatus::Automatic)
			{
				if((l_bContainsPolitic && (l_pAdvisor->PoliticStatus() == EAdvisorStatus::Confirmation || l_pAdvisor->PoliticStatus() == EAdvisorStatus::Manual))
					||
					(l_bContainsMilitary && (l_pAdvisor->MilitaryStatus() == EAdvisorStatus::Confirmation || l_pAdvisor->MilitaryStatus() == EAdvisorStatus::Manual)))
				{
					l_pAdvisor->AddPlanToObjective(l_iObjectiveID,this);
					 return EPlanStatus::Hold;
				}
				return EPlanStatus::Ready;
			}
			break;
		case EEHECategory::Politic:
			if(l_pAdvisor->PoliticStatus() == EAdvisorStatus::Automatic)
			{
				if((l_bContainsEconomic && (l_pAdvisor->EconomicStatus() == EAdvisorStatus::Confirmation || l_pAdvisor->EconomicStatus() == EAdvisorStatus::Manual))
					||
					(l_bContainsMilitary && (l_pAdvisor->MilitaryStatus() == EAdvisorStatus::Confirmation || l_pAdvisor->MilitaryStatus() == EAdvisorStatus::Manual)))
				{
					l_pAdvisor->AddPlanToObjective(l_iObjectiveID,this);
					 return EPlanStatus::Hold;
				}
				return EPlanStatus::Ready;
			}
			break;
		case EEHECategory::Military:
			if(l_pAdvisor->MilitaryStatus() == EAdvisorStatus::Automatic)
			{
				if((l_bContainsPolitic && (l_pAdvisor->PoliticStatus() == EAdvisorStatus::Confirmation || l_pAdvisor->PoliticStatus() == EAdvisorStatus::Manual))
					||
					(l_bContainsEconomic && (l_pAdvisor->EconomicStatus() == EAdvisorStatus::Confirmation || l_pAdvisor->EconomicStatus() == EAdvisorStatus::Manual)))
				{
					l_pAdvisor->AddPlanToObjective(l_iObjectiveID,this);
					 return EPlanStatus::Hold;
				}
				return EPlanStatus::Ready;
			}
			break;
		}		
		
	
		//If we are here, we must ask confirmation
		l_pAdvisor->AddPlanToObjective(l_iObjectiveID,this);

		{
			GEntity* l_pEntity = SDK::EHE::GManager::Entity( l_iCountryID );
			hash_map<UINT32, REAL32> l_CurrentState;
			l_pEntity->ComputeState(l_CurrentState);

			hash_map<UINT32, REAL32> l_PredictedState;
			const SDK::EHE::GPlan& l_Plan = *((const SDK::EHE::GPlan*)this);
			l_pEntity->PredictState(l_CurrentState,l_Plan,l_PredictedState);
			
			vector<pair<UINT32,bool> > l_vObjectives;
			for(hash_map<UINT32,REAL32>::iterator it = l_PredictedState.begin();
				 it != l_PredictedState.end(); it++)
			{
				REAL32 l_fResultsPredicted = it->second;
				REAL32 l_fResultsOriginal = l_CurrentState[it->first];
				if(abs(l_fResultsPredicted-l_fResultsOriginal) < 0.000001f)
					continue;
				if(l_fResultsPredicted > l_fResultsOriginal)
					l_vObjectives.push_back(make_pair(it->first,true));
				else if(l_fResultsPredicted < l_fResultsOriginal)
					l_vObjectives.push_back(make_pair(it->first,false));
			}

			//Tell the player a plan should be executed by him/her
			SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GSendPlan);
			l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
			l_Event->m_iTarget = g_Joshua.ActivePlayerByModID(l_iCountryID)->Id();
			SP2::Event::GSendPlan* l_pSendPlan = (SP2::Event::GSendPlan*)l_Event.get();
			l_pSendPlan->m_TypeOfPlan = ETypeOfPlan::EHE;
			l_pSendPlan->m_iObjectiveID = l_iObjectiveID;
			l_pSendPlan->m_vActionsID = l_vActionsIds;
			l_pSendPlan->m_vObjectiveChanges = l_vObjectives;
			g_Joshua.RaiseEvent(l_Event); 
		}

		return EPlanStatus::Hold;
	}
	else
	{		
		//that plan was already inside
		switch(l_pAdvisor->GetPlanStatus(l_iObjectiveID))
		{
		case EPlanStatus::Ready:
			l_pAdvisor->RemovePlan(l_iObjectiveID);
			return EPlanStatus::Ready;
		case EPlanStatus::Cancel:
			l_pAdvisor->RemovePlan(l_iObjectiveID);
			return EPlanStatus::Cancel;
		case EPlanStatus::Hold:
			return EPlanStatus::Hold;
		}
	}	

	return EPlanStatus::Ready;

}
