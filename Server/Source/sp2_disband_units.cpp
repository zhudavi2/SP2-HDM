#include "golem_pch.hpp"


using namespace EHEActions;


GDisbandUnits* GDisbandUnits::Clone() const
{
   return new GDisbandUnits( *this ); 
}


void GDisbandUnits::Execute()
{
	GAI::DisbandUnits(m_iSource);	
}
