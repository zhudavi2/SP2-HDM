#include "golem_pch.hpp"


using namespace EHEActions;


GIllegalFreedomOfSpeech* GIllegalFreedomOfSpeech::Clone() const
{
   return new GIllegalFreedomOfSpeech( *this ); 
}


void GIllegalFreedomOfSpeech::Execute()
{
	g_ServerDCL.ChangeCountryInternalLaw(m_iSource,EInternalLaws::FreedomOfSpeech,false);
}

bool GIllegalFreedomOfSpeech::CheckHardConditions() const
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	if(l_pCountryData->InternalLaw(EInternalLaws::FreedomOfSpeech))
		return true;
	
	return false;
}