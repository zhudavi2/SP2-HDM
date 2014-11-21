#include "golem_pch.hpp"


using namespace EHEActions;


GCommonMarket* GCommonMarket::Clone() const
{
   return new GCommonMarket( *this ); 
}


void GCommonMarket::Execute()
{
	GAI::CreateOrJoinTreaty(m_iSource,ETreatyType::CommonMarket);
	m_fWhenToStepForward = g_Joshua.GameTime() + 90.f;
}

bool GCommonMarket::CheckHardConditions() const
{	
	if(GAI::m_NbTreatiesByType[ETreatyType::CommonMarket] > 3)
		return false;

	Random::GQuick l_Rand;
   l_Rand.Seed( m_iSource * (UINT32)g_Joshua.GameTime() );
   if(l_Rand.RandomReal() < 0.05f)
		return true;

	return false;
}

bool GCommonMarket::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}