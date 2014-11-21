#include "golem_pch.hpp"


using namespace EHEObjectives;


GStrongNuclear* GStrongNuclear::Clone() const
{
   return new GStrongNuclear( *this );
}


OBJECTIVE_LEVEL GStrongNuclear::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	REAL32 l_fMissileValue = g_ServerDAL.TotalMissileValue(in_iEntity);
	//if(l_fMissileValue == 0.f)
	//	return -1.f;

	REAL32 l_fTotalWorldNukes = 0.f;
	UINT32 l_iNbCountriesWithNukes = 0;
	UINT32 l_iNbCountries = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<=l_iNbCountries; i++)
	{
		if(!g_ServerDAL.CountryIsValid(i) || i == in_iEntity)
			continue;
		REAL32 l_fValue = g_ServerDAL.TotalMissileValue(i);
		if(l_fValue > 0.f)
		{
			l_fTotalWorldNukes += l_fValue;
			l_iNbCountriesWithNukes++;
		}
	}
	REAL32 l_fAverage = 0.f;
	if(l_iNbCountriesWithNukes > 0)
	{
		l_fAverage = l_fTotalWorldNukes / (REAL32)l_iNbCountriesWithNukes;
	}
	else
		return 1.f;

	REAL32 l_fRatio = 0.f;
	if(l_fAverage == 0.f)
		return 1.f;

	l_fRatio = l_fMissileValue / l_fAverage;

	if(l_fRatio >= 2.f)
		return 1.f;
	else if(l_fRatio >= 1.f)
		return l_fRatio - 1.f;
	else if(l_fRatio >= 0.5f)
		return (2.f * l_fRatio) - 2.f;
	else
		return -1.f;
	
}

OBJECTIVE_LEVEL GStrongNuclear::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}