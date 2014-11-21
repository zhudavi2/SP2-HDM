#include "golem_pch.hpp"


using namespace EHEActions;


GDeclareWar* GDeclareWar::Clone() const
{
   return new GDeclareWar( *this ); 
}

ENTITY_ID GDeclareWar::FindTarget() const
{
   ENTITY_ID l_iCountryToAttack = 0;
	REAL32 l_fMinimumRelation = FLT_MAX;	
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();

	REAL32 l_fLowerRelations =  - 90.f + (20.f * g_SP2Server->AIAggressiveness());

	//Check first if any of its allies are in war.
	{
		set<UINT32> l_vAllies;
		set<UINT32> l_vEnnemies;
		g_ServerDAL.CountryAllies(m_iSource,l_vAllies);
		for(set<UINT32>::const_iterator l_Itr = l_vAllies.begin();
			l_Itr != l_vAllies.end(); l_Itr++)
		{		
			l_vEnnemies.clear();
			g_ServerDAL.IsAtWarWith(*l_Itr,l_vEnnemies);
			for(set<UINT32>::const_iterator l_ItrE = l_vEnnemies.begin();
				 l_ItrE != l_vEnnemies.end(); l_ItrE++)
			{	
				if(g_ServerDAL.RelationBetweenCountries(m_iSource,*l_ItrE) < l_fLowerRelations
					&& g_ServerDAL.DiplomaticStatus(m_iSource,*l_ItrE) != EDiplomaticStatus::Hostile)
					return *l_ItrE;
			}	
		}
	}

	//! \todo Adjust the will to declare war based on country's stats
	// Return the country with lowest relations
   for(ENTITY_ID i=1; i <= l_iNbCountry; i++)
	{
		if (i == m_iSource)
			continue;
		if(!g_ServerDAL.CountryIsValid(i))
			continue;
		REAL32 l_fRelations = g_ServerDAL.RelationBetweenCountries(m_iSource,i);
		if((l_fRelations < l_fMinimumRelation) && 
			(g_ServerDAL.DiplomaticStatus(m_iSource,i) != EDiplomaticStatus::Hostile) && 
			l_fRelations < SP2::c_fRelationsEnnemy)
		{
			l_fRelations = l_fMinimumRelation;
			l_iCountryToAttack = i;
		}
	}
	
	//MultiMOD
	//Check if we can reach the enemy, check what it's relations with the world excluding rebels are (if higher than ours, do not 
	//declare war!
	//
	//Then check it's military status & nuke power
	
	REAL32 l_fRelationsUsWithWorld = 0;
	REAL32 l_fRelationsEnemyWithWorld = 0;
	REAL32 l_fWillingnessToInvade = 1.f;

	SP2::GCountryData* EnemyData = g_ServerDAL.CountryData(l_iCountryToAttack);
	SP2::GCountryData* OurData = g_ServerDAL.CountryData(m_iSource);

	for (ENTITY_ID i = 1; i <= (g_ServerDAL.NbCountry()); i++)
	{
		if (i == m_iSource)
			continue;
		if (i == l_iCountryToAttack)
			continue;
		if (!g_ServerDAL.CountryIsValid(i))
			continue;

		REAL32 l_fRelationsEnemyWithCurrentCountry = g_ServerDAL.RelationBetweenCountries(i, l_iCountryToAttack);
		REAL32 l_fRelationsUsWithCurrentCountry = g_ServerDAL.RelationBetweenCountries(i, m_iSource);
		l_fRelationsEnemyWithWorld = l_fRelationsEnemyWithWorld + l_fRelationsEnemyWithCurrentCountry;
		l_fRelationsUsWithWorld = l_fRelationsUsWithWorld + l_fRelationsUsWithCurrentCountry;
	}
		
	if (l_fRelationsUsWithWorld >= l_fRelationsEnemyWithWorld)
		l_fWillingnessToInvade += 1.5f;		
	if(g_ServerDAL.CountryData(l_iCountryToAttack)->NuclearReady())
		l_fWillingnessToInvade -= 1.f;
	if(g_ServerDAL.CountryData(m_iSource)->NuclearReady())
		l_fWillingnessToInvade += 1.f;
	if(OurData->MilitaryStrength() >= EnemyData->MilitaryStrength())
		l_fWillingnessToInvade += 0.5f;
	if (g_ServerDCL.MoveHostilePossible(m_iSource, g_ServerDAL.CapitalRegion(m_iSource), g_ServerDAL.CapitalRegion((UINT32)l_iCountryToAttack)))
		l_fWillingnessToInvade += 1.f;
	
		
	if (l_fWillingnessToInvade > 0.f)
		return l_iCountryToAttack;

	return NULL;
}


void GDeclareWar::Execute()
{
	if(m_iTarget == 0)
		return;
	
	GCountryData* l_pTarget = g_ServerDAL.CountryData(m_iTarget);
	
	if(!l_pTarget->Activated())
		return;

	GAI::AskForWar(m_iSource,m_iTarget,true);
	
}

bool GDeclareWar::ReadyToStepForward() const
{
	if(m_iTarget == 0)
		return true;

	GCountryData* l_pTarget = g_ServerDAL.CountryData(m_iTarget);
	
	if(!l_pTarget->Activated())
		return true;

	if (g_ServerDAL.DiplomaticStatus(m_iSource,m_iTarget) == EDiplomaticStatus::Hostile)
		return false;
	else
		return true;
}

bool GDeclareWar::CheckHardConditions() const
{
	if(m_iTarget == 0)
		return false;

	GCountryData* l_pTarget = g_ServerDAL.CountryData(m_iTarget);
	
	if(!l_pTarget->Activated())
		return false;

	REAL32 l_fConfidence = 0.25f + (1.f - g_SP2Server->AIAggressiveness());	//lower the number, the least he cares if he is outnumbered
	if (g_ServerDAL.TotalUnitsValue(m_iSource) > (l_fConfidence * g_ServerDAL.TotalUnitsValue(m_iTarget)))
		return true;
	else
		return false;
}