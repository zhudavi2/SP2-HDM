#include "golem_pch.hpp"


using namespace EHEActions;


GHumanDevelopmentCollaboration* GHumanDevelopmentCollaboration::Clone() const
{
   return new GHumanDevelopmentCollaboration( *this ); 
}


void GHumanDevelopmentCollaboration::Execute()
{
	GAI::CreateOrJoinTreaty(m_iSource,ETreatyType::HumanDevelopmentCollaboration);

	m_fWhenToStepForward = g_Joshua.GameTime() + 270.f;	
}

bool GHumanDevelopmentCollaboration::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GHumanDevelopmentCollaboration::CheckHardConditions() const
{	
	if(GAI::m_NbTreatiesByType[ETreatyType::HumanDevelopmentCollaboration] > 3)
		return false;
	return true;
}