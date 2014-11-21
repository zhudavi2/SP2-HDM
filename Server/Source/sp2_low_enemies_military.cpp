#include "golem_pch.hpp"


using namespace EHEObjectives;


GLowEnemiesMilitary* GLowEnemiesMilitary::Clone() const
{
   return new GLowEnemiesMilitary( *this );
}


OBJECTIVE_LEVEL GLowEnemiesMilitary::CurrentLevel( ENTITY_ID in_iEntity ) const
{
/*!
* 1 == The ennemies will certainly lose
* -1 == The ennemies will have no problems fighting their wars
**/   

	set<UINT32> l_vEnnemies;
	set<UINT32> l_vAlliesSide;
	set<UINT32> l_vEnnemiesSide;
	REAL32 l_fAlliesStrength = 0.f;
	REAL32 l_fEnnemiesStrength = 0.f;

	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();

	for(UINT32 i=1; i<=l_iNbCountry; i++)
	{
		if(i == in_iEntity || !g_ServerDAL.CountryIsValid(i))
			continue;
		if(g_ServerDAL.RelationBetweenCountries(i,in_iEntity) < SP2::c_fRelationsEnnemy)
			l_vEnnemies.insert(i);
	}

	for(set<UINT32>::const_iterator l_Itr = l_vEnnemies.begin();
		 l_Itr != l_vEnnemies.end(); l_Itr++)
	{
		g_ServerDAL.IsAtWarWith(*l_Itr,l_vAlliesSide);		
	}
	for(set<UINT32>::const_iterator l_Itr = l_vAlliesSide.begin();
		 l_Itr != l_vAlliesSide.end(); l_Itr++)
	{
		g_ServerDAL.IsAtWarWith(*l_Itr,l_vEnnemiesSide);		
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


OBJECTIVE_LEVEL GLowEnemiesMilitary::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 0.f;
}