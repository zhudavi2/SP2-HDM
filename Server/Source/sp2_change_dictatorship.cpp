#include "golem_pch.hpp"


using namespace EHEActions;


GChangeToDictatorship* GChangeToDictatorship::Clone() const
{
   return new GChangeToDictatorship( *this ); 
}


void GChangeToDictatorship::Execute()
{
	g_ServerDCL.ChangeGovernmentType(m_iSource,
			(EGovernmentType::Enum)g_ServerDAL.CountryData(m_iSource)->GvtType(),
			 EGovernmentType::MilitaryDictatorship);
	m_fWhenToStepForward = g_Joshua.GameTime() + 365.f;
}

bool GChangeToDictatorship::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GChangeToDictatorship::CheckHardConditions() const
{	
	EGovernmentType::Enum l_CurrentType = (EGovernmentType::Enum)g_ServerDAL.CountryData(m_iSource)->GvtType();
	if(l_CurrentType == EGovernmentType::MilitaryDictatorship)
		return false;
	return true;
}