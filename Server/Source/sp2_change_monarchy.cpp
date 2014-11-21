#include "golem_pch.hpp"


using namespace EHEActions;


GChangeToMonarchy* GChangeToMonarchy::Clone() const
{
   return new GChangeToMonarchy( *this ); 
}


void GChangeToMonarchy::Execute()
{
	g_ServerDCL.ChangeGovernmentType(m_iSource,
			(EGovernmentType::Enum)g_ServerDAL.CountryData(m_iSource)->GvtType(),
			 EGovernmentType::Monarchy);
	m_fWhenToStepForward = g_Joshua.GameTime() + 365.f;
}

bool GChangeToMonarchy::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GChangeToMonarchy::CheckHardConditions() const
{	
	EGovernmentType::Enum l_CurrentType = (EGovernmentType::Enum)g_ServerDAL.CountryData(m_iSource)->GvtType();
	if(l_CurrentType == EGovernmentType::Monarchy)
		return false;

	Random::GQuick l_Rand;
   l_Rand.Seed( m_iSource * (UINT32)g_Joshua.GameTime() );
   if(l_Rand.RandomReal() < 0.1f)
		return true;

	return false;
}