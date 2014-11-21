#include "golem_pch.hpp"


using namespace EHEActions;


GChangeToCommunism* GChangeToCommunism::Clone() const
{
   return new GChangeToCommunism( *this ); 
}


void GChangeToCommunism::Execute()
{
	g_ServerDCL.ChangeGovernmentType(m_iSource,
			(EGovernmentType::Enum)g_ServerDAL.CountryData(m_iSource)->GvtType(),
			 EGovernmentType::Communist);
	m_fWhenToStepForward = g_Joshua.GameTime() + 365.f;
}

bool GChangeToCommunism::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GChangeToCommunism::CheckHardConditions() const
{	
	EGovernmentType::Enum l_CurrentType = (EGovernmentType::Enum)g_ServerDAL.CountryData(m_iSource)->GvtType();
	if(l_CurrentType == EGovernmentType::Communist)
		return false;

	Random::GQuick l_Rand;
   l_Rand.Seed( m_iSource * (UINT32)g_Joshua.GameTime() );
   if(l_Rand.RandomReal() < 0.1f)
		return true;

	return false;
}