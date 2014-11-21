#include "golem_pch.hpp"


using namespace EHEObjectives;


GHaveHighNationalSecurity* GHaveHighNationalSecurity::Clone() const
{
   return new GHaveHighNationalSecurity( *this );
}


OBJECTIVE_LEVEL GHaveHighNationalSecurity::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	REAL32 l_fSecurity = g_ServerDCL.FindNationalSecurity(in_iEntity);
	//Adjust security between -1 and 1
	if(l_fSecurity >= 0.95f)
		return 1.f;
	else if(l_fSecurity >= 0.5f)
	{
		return (( 1.f/0.45f ) * l_fSecurity) - 1.11111111111f;
	}
	else if(l_fSecurity >= 0.f)
	{
		return (2.f * l_fSecurity) - 1.f;
	}

	return -1.f;


}

OBJECTIVE_LEVEL GHaveHighNationalSecurity::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   //If country has any enemies, and the enemies have nukes, it's very important, otherwise it is less importants
	UINT32 l_iNbCountries = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<= l_iNbCountries; i++)
	{
		if(g_ServerDAL.CountryIsValid(i))
		{
			if(g_ServerDAL.RelationBetweenCountries(i,in_iEntity) < SP2::c_fRelationsEnnemy)
				return 1.f;
		}
	}
			
	return -1.f;
}