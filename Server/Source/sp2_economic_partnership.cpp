#include "golem_pch.hpp"


using namespace EHEActions;


GEconomicPartnership* GEconomicPartnership::Clone() const
{
   return new GEconomicPartnership( *this ); 
}


void GEconomicPartnership::Execute()
{
	GAI::CreateOrJoinTreaty(m_iSource,ETreatyType::EconomicPartnership);
}

bool GEconomicPartnership::CheckHardConditions() const
{	
	if(GAI::m_NbTreatiesByType[ETreatyType::EconomicPartnership] > 3)
		return false;
	return true;
}