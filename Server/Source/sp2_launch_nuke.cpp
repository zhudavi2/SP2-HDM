#include "golem_pch.hpp"


using namespace EHEActions;


GLaunchNuke* GLaunchNuke::Clone() const
{
   return new GLaunchNuke( *this ); 
}


void GLaunchNuke::Execute()
{

	if(m_iTarget == 0)
		return;

	//Check how many megatons to send, usually 10% of current nukes
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(m_iSource);
	UINT32 l_iMegatons = 0;

   GJoinedVectors<SP2::GNuclearMissile> l_Missiles = l_pCountryData->NuclearMissiles();
   
   for(GJoinedVectors<SP2::GNuclearMissile>::GIterator l_It = l_Missiles.Begin();
       l_It != l_Missiles.End() ;
       l_It++)
   {
      l_iMegatons += l_It->m_iQty * l_It->Megatons();
   }
   
	UINT32 l_iMegatonsSending = (UINT32)((REAL32)l_iMegatons * 0.1f);
	g_CombatPlanner.AddNukeToWaitingList(m_iSource,m_iTarget,l_iMegatonsSending,ENuclearTargetType::Military);

}

ENTITY_ID GLaunchNuke::FindTarget() const
{
	ENTITY_ID l_iCountryToNuke = 0;
	REAL32 l_fMinimumRelation = FLT_MAX;	
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();	

	//! \todo Adjust the will to declare war based on country's stats
	// Return the country with lowest relations
   for(ENTITY_ID i=1; i <= l_iNbCountry; i++)
	{
		if (i == m_iSource || !g_ServerDAL.CountryIsValid(i))
			continue;

		REAL32 l_fRelations = g_ServerDAL.RelationBetweenCountries(m_iSource,i);
		if( l_fRelations < l_fMinimumRelation && 
			 l_fRelations < SP2::c_fRelationsEnnemy )
		{
			l_fRelations = l_fMinimumRelation;
			l_iCountryToNuke = i;
		}
	}
	return l_iCountryToNuke;
}

bool GLaunchNuke::CheckHardConditions() const
{
	if(!g_ServerDAL.CountryData(m_iSource)->NuclearReady())
		return false;

	//The nuking country must have at least 1 nuke
	if(g_ServerDAL.TotalMissileValue(m_iSource) <= 0.f)
		return false;

	//The nuking must have at least one enemy
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();	

   for(ENTITY_ID i=1; i <= l_iNbCountry; i++)
	{
		if (i == m_iSource || !g_ServerDAL.CountryIsValid(i))
			continue;

		REAL32 l_fRelations = g_ServerDAL.RelationBetweenCountries(m_iSource,i);
		if(l_fRelations < SP2::c_fRelationsEnnemy)
		{
			return true;
		}
	}
	return false;

}