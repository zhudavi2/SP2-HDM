#include "golem_pch.hpp"


using namespace EHEActions;


GEconomicAidGive* GEconomicAidGive::Clone() const
{
   return new GEconomicAidGive( *this ); 
}


void GEconomicAidGive::Execute()
{
	GAI::CreateOrJoinTreaty(m_iSource,ETreatyType::EconomicAid);
}

bool GEconomicAidGive::CheckHardConditions() const
{	
	return true;
}


GEconomicAidReceive* GEconomicAidReceive::Clone() const
{
   return new GEconomicAidReceive( *this ); 
}


void GEconomicAidReceive::Execute()
{
	GAI::CreateOrJoinTreaty(m_iSource,ETreatyType::EconomicAid,2);
}

bool GEconomicAidReceive::CheckHardConditions() const
{	
	if(GAI::m_NbTreatiesByType[ETreatyType::EconomicAid] > 3)
		return false;
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	if(l_pCountryData->BudgetRevenues() > l_pCountryData->BudgetExpenses())
		return false;
	return true;
}