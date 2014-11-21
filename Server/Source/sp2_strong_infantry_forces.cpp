#include "golem_pch.hpp"


using namespace EHEObjectives;


GStrongInfantry* GStrongInfantry::Clone() const
{
   return new GStrongInfantry( *this );
}


OBJECTIVE_LEVEL GStrongInfantry::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	INT32 l_iNbCountries = (INT32)g_ServerDAL.NbCountry();
	if(l_iNbCountries == 1)
		return 1.f;
	INT32 l_iInfantry = 0;
	INT32 l_iGround = 0;
	INT32 l_iAir = 0;
	INT32 l_iNaval = 0;
	g_ServerDAL.FindUnitTypeRanks(in_iEntity,l_iInfantry,l_iGround,l_iAir,l_iNaval);

	REAL32 l_fRankRatio = (REAL32)(l_iNbCountries -  l_iInfantry) / (REAL32)(l_iNbCountries - 1);

	return GAI::AdjustToEHEObjective(l_fRankRatio);
}

OBJECTIVE_LEVEL GStrongInfantry::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 0.75f;
}