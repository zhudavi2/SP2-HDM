#include "golem_pch.hpp"


using namespace EHEObjectives;


GHaveNoDebt* GHaveNoDebt::Clone() const
{
   return new GHaveNoDebt( *this );
}


OBJECTIVE_LEVEL GHaveNoDebt::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iEntity);
	REAL64 l_fRevenues = l_pCountryData->BudgetRevenues();
	REAL64 l_fExpenses = l_pCountryData->BudgetExpenses();
	REAL64 l_fMoneyAvailable = l_pCountryData->MonetarySupplyAmount();

	if(l_fMoneyAvailable >= l_fExpenses || l_fExpenses == 0.f)
		return 1.f;
	if(l_fMoneyAvailable < 0.f && fabs((l_fMoneyAvailable * DEBT_INTEREST_RATE)) > l_fRevenues) 
		return -1.f;

	if(l_fMoneyAvailable >= 0.f)
	{
		//between 0 and 1, 
		return (REAL32)(l_fMoneyAvailable / l_fExpenses);
	}
	else
	{
		if(l_fRevenues == 0.f)
			return -1.f;
		return (REAL32)(fabs((l_fMoneyAvailable * DEBT_INTEREST_RATE)) / l_fRevenues);
	}
}

OBJECTIVE_LEVEL GHaveNoDebt::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}