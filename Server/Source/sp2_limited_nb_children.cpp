#include "golem_pch.hpp"


using namespace EHEActions;


GLimitedNbChildren* GLimitedNbChildren::Clone() const
{
   return new GLimitedNbChildren( *this ); 
}


void GLimitedNbChildren::Execute()
{
	g_ServerDCL.ChangeCountryInternalLaw(m_iSource,EInternalLaws::NumberOfChildren,false);
	m_fWhenToStepForward = g_Joshua.GameTime() + 180.f;
}

bool GLimitedNbChildren::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GLimitedNbChildren::CheckHardConditions() const
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	if(l_pCountryData->InternalLaw(EInternalLaws::NumberOfChildren))
		return true;
	
	return false;
}