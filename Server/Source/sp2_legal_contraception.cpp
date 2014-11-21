#include "golem_pch.hpp"


using namespace EHEActions;


GLegalContraception* GLegalContraception::Clone() const
{
   return new GLegalContraception( *this ); 
}


void GLegalContraception::Execute()
{
	g_ServerDCL.ChangeCountryInternalLaw(m_iSource,EInternalLaws::Contraception,true);
}

bool GLegalContraception::CheckHardConditions() const
{	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	if(!l_pCountryData->InternalLaw(EInternalLaws::Contraception))
		return true;
	
	return false;
}