#include "golem_pch.hpp"


using namespace EHEActions;


GAssumeForeignDebtGive* GAssumeForeignDebtGive::Clone() const
{
   return new GAssumeForeignDebtGive( *this ); 
}


void GAssumeForeignDebtGive::Execute()
{
	GAI::CreateOrJoinTreaty(m_iSource,ETreatyType::AssumeForeignDebt);
}

bool GAssumeForeignDebtGive::CheckHardConditions() const
{	
	return true;
}

GAssumeForeignDebtReceive* GAssumeForeignDebtReceive::Clone() const
{
   return new GAssumeForeignDebtReceive( *this ); 
}


void GAssumeForeignDebtReceive::Execute()
{
	GAI::CreateOrJoinTreaty(m_iSource,ETreatyType::AssumeForeignDebt,2);
}

bool GAssumeForeignDebtReceive::CheckHardConditions() const
{	
	if(GAI::m_NbTreatiesByType[ETreatyType::AssumeForeignDebt] > 3)
		return false;
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	if(l_pCountryData->BudgetRevenues() > l_pCountryData->BudgetExpenses())
		return false;
	return true;
}