#include "golem_pch.hpp"


using namespace EHEActions;


GAlliance* GAlliance::Clone() const
{
   return new GAlliance( *this ); 
}


void GAlliance::Execute()
{
	GAI::CreateOrJoinTreaty(m_iSource,ETreatyType::Alliance);
}

bool GAlliance::CheckHardConditions() const
{	
	if(GAI::m_NbTreatiesByType[ETreatyType::Alliance] > 3)
		return false;
	return true;
}