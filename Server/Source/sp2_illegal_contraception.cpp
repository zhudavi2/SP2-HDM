#include "golem_pch.hpp"


using namespace EHEActions;


GIllegalContraception* GIllegalContraception::Clone() const
{
   return new GIllegalContraception( *this ); 
}


void GIllegalContraception::Execute()
{
	g_ServerDCL.ChangeCountryInternalLaw(m_iSource,EInternalLaws::Contraception,false);
}

bool GIllegalContraception::CheckHardConditions() const
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	if(l_pCountryData->InternalLaw(EInternalLaws::Contraception))
		return true;
	
	return false;
}