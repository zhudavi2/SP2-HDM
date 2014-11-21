#include "golem_pch.hpp"


using namespace EHEActions;


GNobleCause* GNobleCause::Clone() const
{
   return new GNobleCause( *this ); 
}


void GNobleCause::Execute()
{
	GAI::CreateOrJoinTreaty(m_iSource,ETreatyType::NobleCause);
}

bool GNobleCause::CheckHardConditions() const
{	
	return true;
}