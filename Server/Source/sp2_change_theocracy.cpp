#include "golem_pch.hpp"


using namespace EHEActions;


GChangeToTheocracy* GChangeToTheocracy::Clone() const
{
   return new GChangeToTheocracy( *this ); 
}


void GChangeToTheocracy::Execute()
{
	g_ServerDCL.ChangeGovernmentType(m_iSource,
			(EGovernmentType::Enum)g_ServerDAL.CountryData(m_iSource)->GvtType(),
			 EGovernmentType::Theocracy);
	m_fWhenToStepForward = g_Joshua.GameTime() + 365.f;
}

bool GChangeToTheocracy::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GChangeToTheocracy::CheckHardConditions() const
{	
	EGovernmentType::Enum l_CurrentType = (EGovernmentType::Enum)g_ServerDAL.CountryData(m_iSource)->GvtType();
	if(l_CurrentType == EGovernmentType::Theocracy)
		return false;
	
	Random::GQuick l_Rand;
   l_Rand.Seed( m_iSource * (UINT32)g_Joshua.GameTime() );
   if(l_Rand.RandomReal() < 0.05f)
		return true;

	return false;
}