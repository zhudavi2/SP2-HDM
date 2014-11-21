#include "golem_pch.hpp"


using namespace EHEActions;


GResearchPartnership* GResearchPartnership::Clone() const
{
   return new GResearchPartnership( *this ); 
}


void GResearchPartnership::Execute()
{
	GAI::CreateOrJoinTreaty(m_iSource,ETreatyType::ResearchPartnership);
}

bool GResearchPartnership::CheckHardConditions() const
{	
	if(GAI::m_NbTreatiesByType[ETreatyType::ResearchPartnership] > 3)
		return false;
	
	Random::GQuick l_Rand;
   l_Rand.Seed( m_iSource * (UINT32)g_Joshua.GameTime() );
   if(l_Rand.RandomReal() < 0.05f)
		return true;

	return false;
}