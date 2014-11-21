#include "golem_pch.hpp"


using namespace EHEObjectives;


GEnemiesNoMoney* GEnemiesNoMoney::Clone() const
{
   return new GEnemiesNoMoney( *this );
}


OBJECTIVE_LEVEL GEnemiesNoMoney::CurrentLevel( ENTITY_ID in_iEntity ) const
{	
	hash_map<OBJECTIVE_ID, GObjective*> l_Obj = GManager::Objectives();	 
	GObjective* l_pHaveMoney = l_Obj[EHEObjectives::Ids::HAVE_POSITIVE_BUDGET];


	REAL32 l_fTotalMoney = 0.f;
	UINT32 l_iNbEnemies = 0;
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<= l_iNbCountry; i++)
	{		
		if(in_iEntity == i || !g_ServerDAL.CountryIsValid(i))
			continue;
			
		if(g_ServerDAL.RelationBetweenCountries(in_iEntity,i) < SP2::c_fRelationsEnnemy)
		{
			l_fTotalMoney += l_pHaveMoney->CurrentLevel(i);
			l_iNbEnemies++;
		}				
	}

	if(!l_iNbEnemies)
		return 1.f;

	REAL32 l_fAverageMoney = l_fTotalMoney / (REAL32)l_iNbEnemies;

	return l_fAverageMoney;

}

OBJECTIVE_LEVEL GEnemiesNoMoney::OptimalLevel( ENTITY_ID in_iEntity ) const
{
   return 1.f;
}