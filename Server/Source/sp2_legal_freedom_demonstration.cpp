#include "golem_pch.hpp"


using namespace EHEActions;


GLegalFreedomOfDemonstration* GLegalFreedomOfDemonstration::Clone() const
{
   return new GLegalFreedomOfDemonstration( *this ); 
}


void GLegalFreedomOfDemonstration::Execute()
{
	g_ServerDCL.ChangeCountryInternalLaw(m_iSource,EInternalLaws::FreedomOfDemonstration,true);
	m_fWhenToStepForward = g_Joshua.GameTime() + 180.f;
}

bool GLegalFreedomOfDemonstration::ReadyToStepForward() const
{
	if(g_Joshua.GameTime() < m_fWhenToStepForward)
		return false;

	return true;
}

bool GLegalFreedomOfDemonstration::CheckHardConditions() const
{	
	Random::GQuick l_Rand;
   l_Rand.Seed( m_iSource * (UINT32)g_Joshua.GameTime() );
   if(l_Rand.RandomReal() > 0.2f)
		return false;

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	if(!l_pCountryData->InternalLaw(EInternalLaws::FreedomOfDemonstration))
		return true;
	
	return false;
}