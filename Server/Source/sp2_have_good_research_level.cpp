#include "golem_pch.hpp"


using namespace EHEObjectives;


GHaveGoodResearchLevel* GHaveGoodResearchLevel::Clone() const
{
   return new GHaveGoodResearchLevel( *this );
}


OBJECTIVE_LEVEL GHaveGoodResearchLevel::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	GResearchInformation* l_pInfo = (GResearchInformation*)g_ServerDAL.CountryData(in_iEntity)->ResearchInfo();

   REAL64 l_fTotalResearch = l_pInfo->Total();
	REAL64 l_fAverageResearch = l_fTotalResearch / 38.f;

	if(l_fAverageResearch >= 10.f)
		return 1.f;
	else if (l_fAverageResearch <= 0.f)
		return -1.f;

	return GAI::AdjustToEHEObjective((REAL32)l_fAverageResearch / 10.f);
}

OBJECTIVE_LEVEL GHaveGoodResearchLevel::OptimalLevel( ENTITY_ID in_iEntity ) const
{
	if(g_ServerDAL.IsAtWar(in_iEntity))
		return 1.f;
   return 0.f;
}