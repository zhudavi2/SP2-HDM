#include "golem_pch.hpp"


using namespace EHEActions;


GCulturalExchanges* GCulturalExchanges::Clone() const
{
   return new GCulturalExchanges( *this ); 
}


void GCulturalExchanges::Execute()
{
	GAI::CreateOrJoinTreaty(m_iSource,ETreatyType::CulturalExchanges);
}

bool GCulturalExchanges::CheckHardConditions() const
{	
	if(GAI::m_NbTreatiesByType[ETreatyType::CulturalExchanges] > 3)
		return false;
	return true;
}