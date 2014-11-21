#include "golem_pch.hpp"


using namespace EHEActions;


GChangeToDemocracy* GChangeToDemocracy::Clone() const
{
   return new GChangeToDemocracy( *this ); 
}


void GChangeToDemocracy::Execute()
{
	if(!g_ServerDCL.ChangeGovernmentType(m_iSource,
			(EGovernmentType::Enum)g_ServerDAL.CountryData(m_iSource)->GvtType(),
			EGovernmentType::MultiPartyDemocracy))
	{
		g_ServerDCL.ChangeGovernmentType(m_iSource,
			(EGovernmentType::Enum)g_ServerDAL.CountryData(m_iSource)->GvtType(),
			 EGovernmentType::SinglePartyDemocracy);
	}

	m_fWhenToStepForward = g_Joshua.GameTime() + 365.f;
}

bool GChangeToDemocracy::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GChangeToDemocracy::CheckHardConditions() const
{	
	EGovernmentType::Enum l_CurrentType = (EGovernmentType::Enum)g_ServerDAL.CountryData(m_iSource)->GvtType();
	if(l_CurrentType == EGovernmentType::MultiPartyDemocracy || 
		l_CurrentType == EGovernmentType::SinglePartyDemocracy)
		return false;

	Random::GQuick l_Rand;
   l_Rand.Seed( m_iSource * (UINT32)g_Joshua.GameTime() );
   if(l_Rand.RandomReal() < 0.1f)
		return true;

	return false;
}