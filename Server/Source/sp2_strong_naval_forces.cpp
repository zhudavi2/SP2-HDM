#include "golem_pch.hpp"


using namespace EHEObjectives;


GStrongNaval* GStrongNaval::Clone() const
{
   return new GStrongNaval( *this );
}


OBJECTIVE_LEVEL GStrongNaval::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	INT32 l_iNbCountries = (INT32)g_ServerDAL.NbCountry();
	if(l_iNbCountries == 1)
		return 1.f;
	INT32 l_iInfantry = 0;
	INT32 l_iGround = 0;
	INT32 l_iAir = 0;
	INT32 l_iNaval = 0;
	g_ServerDAL.FindUnitTypeRanks(in_iEntity,l_iInfantry,l_iGround,l_iAir,l_iNaval);

	REAL32 l_fRankRatio = (REAL32)(l_iNbCountries -  l_iNaval) / (REAL32)(l_iNbCountries - 1);

	return GAI::AdjustToEHEObjective(l_fRankRatio);
}

OBJECTIVE_LEVEL GStrongNaval::OptimalLevel( ENTITY_ID in_iEntity ) const
{
	vector<Hector::GVector2D<REAL32> > l_vWaypoints;
	g_ServerDCL.FindNavalWaypointsAdjacentToCountry(in_iEntity,l_vWaypoints);

	if(l_vWaypoints.size() == 0)
		return -1.f;
   
	return 0.75f;
}