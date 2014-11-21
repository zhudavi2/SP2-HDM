#include "golem_pch.hpp"


using namespace EHEObjectives;


GHaveAMDS* GHaveAMDS::Clone() const
{
   return new GHaveAMDS( *this );
}


OBJECTIVE_LEVEL GHaveAMDS::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	if(g_ServerDAL.CountryData(in_iEntity)->AMDSLevel() >= 1.f)
		return 1.f;
	return -1.f;
}

OBJECTIVE_LEVEL GHaveAMDS::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   //If country has any enemies, and the enemies have nukes, it's very important, otherwise it is less importants
	UINT32 l_iNbCountries = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<= l_iNbCountries; i++)
	{
		if(g_ServerDAL.CountryIsValid(i))
		{
			if(g_ServerDAL.RelationBetweenCountries(i,in_iEntity) < SP2::c_fRelationsEnnemy &&
				g_ServerDAL.TotalMissileValue(i) > 0.f)
				return 1.f;
		}
	}
			
	return -1.f;
}