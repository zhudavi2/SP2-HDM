#include "golem_pch.hpp"


using namespace EHEActions;


GResearchAMDS* GResearchAMDS::Clone() const
{
   return new GResearchAMDS( *this ); 
}


void GResearchAMDS::Execute()
{
	g_ServerDCL.StartAMDSResearch(m_iSource);
}

bool GResearchAMDS::CheckHardConditions() const
{	
	return true;
}