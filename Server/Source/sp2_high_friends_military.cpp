#include "golem_pch.hpp"


using namespace EHEObjectives;


GHighFriendsMilitary* GHighFriendsMilitary::Clone() const
{
   return new GHighFriendsMilitary( *this );
}


OBJECTIVE_LEVEL GHighFriendsMilitary::CurrentLevel( ENTITY_ID in_iEntity ) const
{
/*!
 * 1 == The allies will have no problems fighting their wars
 * -1 == The allies will certainly lose
 **/   

	set<UINT32> l_vAllies;
	set<UINT32> l_vAlliesSide;
	set<UINT32> l_vEnnemiesSide;
	REAL32 l_fAlliesStrength = 0.f;
	REAL32 l_fEnnemiesStrength = 0.f;
	g_ServerDAL.CountryAllies(in_iEntity,l_vAllies);

	for(set<UINT32>::const_iterator l_Itr = l_vAllies.begin();
		 l_Itr != l_vAllies.end(); l_Itr++)
	{
		g_ServerDAL.IsAtWarWith(*l_Itr,l_vEnnemiesSide);		
	}
	for(set<UINT32>::const_iterator l_Itr = l_vEnnemiesSide.begin();
		 l_Itr != l_vEnnemiesSide.end(); l_Itr++)
	{
		g_ServerDAL.IsAtWarWith(*l_Itr,l_vAlliesSide);		
	}
	
	//Calculate the strength on both side
	for(set<UINT32>::const_iterator l_Itr = l_vAlliesSide.begin();
		 l_Itr != l_vAlliesSide.end(); l_Itr++)
	{
		l_fAlliesStrength += g_ServerDAL.TotalUnitsValue(*l_Itr);		
	}
	for(set<UINT32>::const_iterator l_Itr = l_vEnnemiesSide.begin();
		 l_Itr != l_vEnnemiesSide.end(); l_Itr++)
	{
		l_fEnnemiesStrength += g_ServerDAL.TotalUnitsValue(*l_Itr);		
	}

	REAL32 l_fRatio = 0.f;
	if(l_fEnnemiesStrength > 0.f)
	{
		l_fRatio = min(l_fAlliesStrength / l_fEnnemiesStrength,2.f);
	}
	else
	{
		if (l_fEnnemiesStrength == l_fAlliesStrength)
			l_fRatio = 1.f;
	}
	l_fRatio -= 1.f;
	
	return l_fRatio;
}


OBJECTIVE_LEVEL GHighFriendsMilitary::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 0;
}