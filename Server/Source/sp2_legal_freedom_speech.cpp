#include "golem_pch.hpp"


using namespace EHEActions;


GLegalFreedomOfSpeech* GLegalFreedomOfSpeech::Clone() const
{
   return new GLegalFreedomOfSpeech( *this ); 
}


void GLegalFreedomOfSpeech::Execute()
{
	g_ServerDCL.ChangeCountryInternalLaw(m_iSource,EInternalLaws::FreedomOfSpeech,true);
}

bool GLegalFreedomOfSpeech::CheckHardConditions() const
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	if(!l_pCountryData->InternalLaw(EInternalLaws::FreedomOfSpeech))
		return true;
	
	return false;
}