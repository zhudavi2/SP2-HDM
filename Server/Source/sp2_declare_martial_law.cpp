#include "golem_pch.hpp"


using namespace EHEActions;


GDeclareMartialLaw* GDeclareMartialLaw::Clone() const
{
   return new GDeclareMartialLaw( *this ); 
}


void GDeclareMartialLaw::Execute()
{
	g_ServerDCL.ChangeMartialLaw(m_iSource,true);
}

bool GDeclareMartialLaw::CheckHardConditions() const
{	
	if(g_ServerDAL.CountryData(m_iSource)->MartialLaw())
		return false;

	return true;
}