#include "golem_pch.hpp"


using namespace EHEActions;


GIllimitedNbChildren* GIllimitedNbChildren::Clone() const
{
   return new GIllimitedNbChildren( *this ); 
}


void GIllimitedNbChildren::Execute()
{
	g_ServerDCL.ChangeCountryInternalLaw(m_iSource,EInternalLaws::NumberOfChildren,true);
	m_fWhenToStepForward = g_Joshua.GameTime() + 150.f;
}

bool GIllimitedNbChildren::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GIllimitedNbChildren::CheckHardConditions() const
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	if(!l_pCountryData->InternalLaw(EInternalLaws::NumberOfChildren))
		return true;
	
	return false;
}