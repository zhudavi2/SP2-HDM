#include "golem_pch.hpp"


using namespace EHEActions;


GIllegalAbortion* GIllegalAbortion::Clone() const
{
   return new GIllegalAbortion( *this ); 
}


void GIllegalAbortion::Execute()
{
	g_ServerDCL.ChangeCountryInternalLaw(m_iSource,EInternalLaws::Abortion,false);
}

bool GIllegalAbortion::CheckHardConditions() const
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	if(l_pCountryData->InternalLaw(EInternalLaws::Abortion))
		return true;
	
	return false;
}