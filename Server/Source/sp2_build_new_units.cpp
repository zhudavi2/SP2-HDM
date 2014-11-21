#include "golem_pch.hpp"


using namespace EHEActions;


GBuildNewUnits* GBuildNewUnits::Clone() const
{
   return new GBuildNewUnits( *this ); 
}


void GBuildNewUnits::Execute()
{		
	m_iUnitInProductionId = GAI::BuildOrBuyUnits(m_iSource,m_fStrength);	
}

bool GBuildNewUnits::ReadyToStepForward() const
{
	if(m_iUnitInProductionId == UINT_MAX)
		return true;
   if (g_ServerDCL.UnitProductionProgress(m_iSource,m_iUnitInProductionId) >= 1.0f)
      return true;
	return false;
}
