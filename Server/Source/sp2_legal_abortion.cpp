#include "golem_pch.hpp"


using namespace EHEActions;


GLegalAbortion* GLegalAbortion::Clone() const
{
   return new GLegalAbortion( *this ); 
}


void GLegalAbortion::Execute()
{
	g_ServerDCL.ChangeCountryInternalLaw(m_iSource,EInternalLaws::Abortion,true);
}

bool GLegalAbortion::CheckHardConditions() const
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	if(!l_pCountryData->InternalLaw(EInternalLaws::Abortion))
		return true;
	
	return false;
}