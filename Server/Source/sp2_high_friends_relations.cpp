#include "golem_pch.hpp"


using namespace EHEObjectives;


GHighFriendsRelations* GHighFriendsRelations::Clone() const
{
   return new GHighFriendsRelations( *this );
}


OBJECTIVE_LEVEL GHighFriendsRelations::CurrentLevel( ENTITY_ID in_iEntity ) const
{
/*!
 * 1 == Every countries love the entity's allies
 * -1 == Every countries hate the entity's allies
 **/


	REAL32 l_fRelations = 0.f;
	REAL32 l_fNbCountries = 0.f;
	
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();

	set<UINT32> l_vFriends;	
	
	for(UINT32 i=1; i<= l_iNbCountry; i++)
	{
		if(in_iEntity == i || !g_ServerDAL.CountryIsValid(i))
			continue;
		if(g_ServerDAL.RelationBetweenCountries(in_iEntity,i) > SP2::c_fRelationsLike)
			l_vFriends.insert(i);
	}
	for(set<UINT32>::iterator it = l_vFriends.begin();
		 it != l_vFriends.end(); it++)
	{
		for(UINT32 i=1; i<= l_iNbCountry; i++)
		{
			if(*it == i || !g_ServerDAL.CountryIsValid(*it))
				continue;
			l_fRelations += g_ServerDAL.RelationBetweenCountries(*it,i);
			l_fNbCountries += 1.f;
		}
	}	

	if(l_fNbCountries <= 0.f)
		return 0.f;

	l_fRelations /= l_fNbCountries;
	l_fRelations /= 100.f;

	return l_fRelations;
}


OBJECTIVE_LEVEL GHighFriendsRelations::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 0.f;
}