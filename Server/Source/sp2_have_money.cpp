#include "golem_pch.hpp"


using namespace EHEObjectives;


GHaveMoney* GHaveMoney::Clone() const
{
   return new GHaveMoney( *this );
}


OBJECTIVE_LEVEL GHaveMoney::CurrentLevel( ENTITY_ID in_iEntity ) const
{		

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iEntity);
	REAL64 l_fRevenues = l_pCountryData->BudgetRevenues();
	REAL64 l_fExpenses = l_pCountryData->BudgetExpenses();

	if(l_fExpenses == 0.f)
		return 1.f;

	REAL64 l_fRatio = l_fRevenues / l_fExpenses;

	if(l_fRatio >= 2.f)
		return 1.f;
	else if(l_fRatio >= 1.f)
		return (REAL32)l_fRatio - 1.f;
	else if(l_fRatio >= 0.5f)
		return (REAL32)((2.f * l_fRatio) - 2.f);
	else
		return -1.f;	
}


OBJECTIVE_LEVEL GHaveMoney::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 0.75f;
}