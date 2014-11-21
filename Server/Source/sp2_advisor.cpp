/**************************************************************
*
* sp2_advisor.cpp
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

GAdvisor::GAdvisor(EAdvisorStatus::Enum in_eEconomic, EAdvisorStatus::Enum in_ePolitic, EAdvisorStatus::Enum in_eMilitary)
{
	m_eEconomicStatus = in_eEconomic;
	m_eMilitaryStatus = in_ePolitic;
	m_ePoliticStatus = in_eMilitary;
}

GAdvisor::~GAdvisor()
{
}

EAdvisorStatus::Enum GAdvisor::MilitaryStatus() const
{
	return m_eMilitaryStatus;
}
EAdvisorStatus::Enum GAdvisor::EconomicStatus() const
{
	return m_eEconomicStatus;
}
EAdvisorStatus::Enum GAdvisor::PoliticStatus() const
{
	return m_ePoliticStatus;
}

void GAdvisor::MilitaryStatus(EAdvisorStatus::Enum in_eStatus)
{
	m_eMilitaryStatus = in_eStatus;
}
void GAdvisor::EconomicStatus(EAdvisorStatus::Enum in_eStatus)
{
	m_eEconomicStatus = in_eStatus;
}
void GAdvisor::PoliticStatus(EAdvisorStatus::Enum in_eStatus)
{
	m_ePoliticStatus = in_eStatus;
}

void GAdvisor::CancelPendingPlans()
{
	for(hash_map<OBJECTIVE_ID ,pair<GPlan*,EPlanStatus::Enum> >::iterator it = m_Plans.begin();
		 it != m_Plans.end(); it++)
	{
		it->second.second = EPlanStatus::Cancel;
	}
}

void GAdvisor::AcceptAllPendingPlans()
{
	for(hash_map<OBJECTIVE_ID ,pair<GPlan*,EPlanStatus::Enum> >::iterator it = m_Plans.begin();
		 it != m_Plans.end(); it++)
	{
		it->second.second = EPlanStatus::Ready;
	}
}

void GAdvisor::AcceptPendingPlan(OBJECTIVE_ID in_iObjective)
{
	hash_map<OBJECTIVE_ID ,pair<GPlan*,EPlanStatus::Enum> >::iterator it = m_Plans.find(in_iObjective);
	if(it != m_Plans.end())
	{
		it->second.second = EPlanStatus::Ready;
	}
}

void GAdvisor::AddPlanToObjective(OBJECTIVE_ID in_iObjective, SP2::GPlan* in_pPlan)
{
	hash_map<OBJECTIVE_ID ,pair<GPlan*,EPlanStatus::Enum> >::iterator it = m_Plans.find(in_iObjective);
	if(it == m_Plans.end())
	{
		//We can now add the plan
		m_Plans[in_iObjective] = make_pair<GPlan*,EPlanStatus::Enum>(in_pPlan,EPlanStatus::Hold);		
	}
}

const SP2::GPlan* GAdvisor::GetPlan(OBJECTIVE_ID in_iObjective) const
{
	hash_map<OBJECTIVE_ID ,pair<GPlan*,EPlanStatus::Enum> >::const_iterator it = m_Plans.find(in_iObjective);	
	if(it == m_Plans.end())
		return NULL;
	return it->second.first;

}

EPlanStatus::Enum GAdvisor::GetPlanStatus(OBJECTIVE_ID in_iObjective) const
{
	hash_map<OBJECTIVE_ID ,pair<GPlan*,EPlanStatus::Enum> >::const_iterator it = m_Plans.find(in_iObjective);	
	if(it == m_Plans.end())
		return EPlanStatus::Undefined;
	return it->second.second;

}

void GAdvisor::RemovePlan(OBJECTIVE_ID in_iObjective)
{
	m_Plans.erase(in_iObjective);
}