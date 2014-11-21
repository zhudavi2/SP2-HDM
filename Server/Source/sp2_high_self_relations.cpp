#include "golem_pch.hpp"


using namespace EHEObjectives;


GHighSelfRelations* GHighSelfRelations::Clone() const
{
   return new GHighSelfRelations( *this );
}


OBJECTIVE_LEVEL GHighSelfRelations::CurrentLevel( ENTITY_ID in_iEntity ) const
{
   REAL32 l_fRelationTotal = 0;
	UINT32 l_iNbCountries = 0;
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<= l_iNbCountry; i++)
	{		
		if(in_iEntity == i || !g_ServerDAL.CountryIsValid(i))
			continue;
		
		l_fRelationTotal += ((REAL32)(l_iNbCountry - g_ServerDAL.CountryData(i)->MilitaryRank()) / (REAL32)(l_iNbCountry-1)) * 
									g_ServerDAL.RelationBetweenCountries(in_iEntity,i);
		l_iNbCountries++;
	}

	if(l_iNbCountry > 0)
		l_fRelationTotal /= (REAL32)l_iNbCountries;
	else
		return 1.f;

	//relations should be between 100 and -100. let's say the worst case is an overall of -30, and best an overall of +30
	if(l_fRelationTotal > 30.f)
		return 1.f;
	else if (l_fRelationTotal < -30.f)
		return -1.f;
	
	return l_fRelationTotal / 30.f;
	
}


OBJECTIVE_LEVEL GHighSelfRelations::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}