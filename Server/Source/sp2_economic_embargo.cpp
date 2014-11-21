#include "golem_pch.hpp"


using namespace EHEActions;


GEconomicEmbargo* GEconomicEmbargo::Clone() const
{
   return new GEconomicEmbargo( *this ); 
}

ENTITY_ID GEconomicEmbargo::FindTarget() const
{
   ENTITY_ID l_iCountryToEmbargo = 0;
	REAL32 l_fMinimumRelation = FLT_MAX;	
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();

	// Return the country with lowest relations
   for(ENTITY_ID i=1; i <= l_iNbCountry; i++)
	{
		if (i == m_iSource)
			continue;
		if(!g_ServerDAL.CountryIsValid(i))
			continue;		

		REAL32 l_fRelations = g_ServerDAL.RelationBetweenCountries(m_iSource,i);
		if((l_fRelations < l_fMinimumRelation) && 			
			l_fRelations < SP2::c_fRelationsDislike)
		{
			if(g_ServerDAL.TradeEmbargo(m_iSource,i))
				continue;
			l_fRelations = l_fMinimumRelation;
			l_iCountryToEmbargo = i;
		}
	}
	return l_iCountryToEmbargo;
}

void GEconomicEmbargo::Execute()
{	
	if(m_iTarget)
		GAI::AskForTradeEmbargo(m_iSource,m_iTarget);
	m_fWhenToStepForward = g_Joshua.GameTime() + 90.f;
}

bool GEconomicEmbargo::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GEconomicEmbargo::CheckHardConditions() const
{	
	if(GAI::m_NbTreatiesByType[ETreatyType::EconomicEmbargo] > 2)
		return false;
	if(m_iTarget > 0 && g_ServerDAL.TradeEmbargo(m_iSource,m_iTarget))
		return false;
	return true;
}