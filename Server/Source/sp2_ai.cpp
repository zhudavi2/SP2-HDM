/**************************************************************
*
* sp2_ai.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

UINT8 GAI::m_NbTreatiesByType[ETreatyType::ItemCount];

//! Default constructor
GAI::GAI()
{
}

//! destructor
GAI::~GAI()
{
}

//Execute all SP2 functions relative to AI iteration
//Before the country is iterated
//Is used through a function pointer inside golem_joshua
//Returns true if success, in that case if the country is activated
bool GAI::ExecuteNewAIIteration(const INT16 in_iCountryID)
{
	if(!g_ServerDAL.CountryIsValid((UINT32)in_iCountryID))
		return false;


	//Calculate the number of treaty that country is in
	Memory::Fill<UINT8>(m_NbTreatiesByType,0,ETreatyType::ItemCount);

	const hash_map<UINT32, GTreaty>& l_Treaties = g_ServerDAL.Treaties();
	UINT32 l_iCountrySide = 0;
	for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
		it != l_Treaties.end();++ it)
	{
		const GTreaty& l_CurTreaty = it->second;
		l_iCountrySide = l_CurTreaty.CountrySide((UINT32)in_iCountryID);
		if( l_iCountrySide == 1 || (l_CurTreaty.SideBCanLeave() && l_iCountrySide == 2) )
			m_NbTreatiesByType[l_CurTreaty.Type()]++;		
	}

	return true;
}

//Execute all SP2 functions relative to AI iteration
//After the country is iterated
//Is used through a function pointer inside golem_joshua
//Returns true if success
bool GAI::ExecuteFinishAIIteration(const INT16 in_iCountryID)
{	
	gassert(g_Joshua.ActivePlayerByModID(in_iCountryID),"GAI::ExecuteFinishAIIteration: Unexisting player");
	
	GAdvisor* l_pAdvisor = g_ServerDAL.CountryData(in_iCountryID)->Advisor();

	if(!l_pAdvisor || l_pAdvisor->PoliticStatus() != EAdvisorStatus::Manual)
	{
		ExamineLeavingTreaties(in_iCountryID);
		ExamineWarAndPeacePossibilities(in_iCountryID);
	}	

	if(!l_pAdvisor || l_pAdvisor->MilitaryStatus() != EAdvisorStatus::Manual)
	{
		ExamineUnitMovement(in_iCountryID);
	}	

	if(!l_pAdvisor || l_pAdvisor->MilitaryStatus() == EAdvisorStatus::Automatic)
	{
		ExamineCovertActionCells(in_iCountryID);
	}	

	if(!l_pAdvisor || l_pAdvisor->EconomicStatus() != EAdvisorStatus::Manual)
	{
		ExamineBudgetReadjustment(in_iCountryID);
	}	

	return true;
}

void GAI::PossiblyLaunchNukeInSelfDefense(UINT32 in_iCountryID)
{	
	//The country has no nukes, so it can't send any of course
	if(g_ServerDAL.TotalMissileValue(in_iCountryID) <= 0.f)
		return;
	
	//Check how many political regions this country has, and how many it has lost
	//If it has less than 20% under its control, it will nuke its attacker
	const set<UINT32>& l_Political = g_ServerDAL.CountryPoliticalControl(in_iCountryID);
	const vector<GRegionControl>& l_vControls = g_ServerDAL.RegionControlArray();

	REAL32 l_fTotal = 0.f;
	REAL32 l_fQty   = 0.f;

	UINT32 l_iNbCountries = g_ServerDAL.NbCountry();
	vector<UINT32> l_NbControllingCountries;
	l_NbControllingCountries.resize( l_iNbCountries+1, 0 );

	for(set<UINT32>::const_iterator it = l_Political.begin();
		 it != l_Political.end(); it++)
	{
		if(l_vControls[*it].m_iMilitary == l_vControls[*it].m_iPolitical)
			l_fQty += 1.f;
		else
			l_NbControllingCountries[l_vControls[*it].m_iMilitary]++;
		l_fTotal += 1.f;
	}
		

	if(l_fTotal == 0.f)
		return;
	
	REAL32 l_fRatio = l_fQty / l_fTotal;

	if(l_fRatio > ((1.f - g_SP2Server->AIAggressiveness()) * SP2::c_fPercentageOfRegionsLeft))
		return;

    //Disallow nuking based on the general occupied region percentage.
    if(l_fRatio > 1.f - g_SP2Server->OccupiedRegionPercentageForNuclear())
		return;

	//The country will launch nuke against the country that has stolen the most regions
	UINT32 l_iMaxCountry = 0;
	UINT32 l_iNbRegions = 0;
	for(UINT32 i=1; i <= l_iNbCountries; i++)
	{
		if(l_NbControllingCountries[i] > l_iNbRegions)
		{
			l_iMaxCountry = i;
			l_iNbRegions = l_NbControllingCountries[i];
		}
	}
	if(l_iMaxCountry)
	{
		//Launch Nuke against that country
		GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
		UINT32 l_iMegatons = 0;

		GJoinedVectors<SP2::GNuclearMissile> l_Missiles = l_pCountryData->NuclearMissiles();
	   
		for(GJoinedVectors<SP2::GNuclearMissile>::GIterator l_It = l_Missiles.Begin();
			l_It != l_Missiles.End() ;
			l_It++)
		{
			l_iMegatons += l_It->m_iQty * l_It->Megatons();
		}
	   
		g_CombatPlanner.AddNukeToWaitingList(in_iCountryID,l_iMaxCountry,l_iMegatons,ENuclearTargetType::Military);
	}
	

}

REAL32 GAI::VerifyValues(REAL32 l_fLevel)
{
	if (l_fLevel > 1.0f)
		return 1.0f;
	
	if (l_fLevel < -1.0f)
		return -1.0f;
	
	return l_fLevel;
}

REAL32 GAI::AdjustToEHEObjective(REAL32 l_fLevel)
{
	return (l_fLevel * 2.f) - 1.f;
}

void GAI::ExamineCovertActionCells(UINT32 in_iCountryID) const
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);	
	set<UINT32> l_CellsToRemove;
	vector<GCovertActionCell>& l_vCells = l_pCountryData->CovertActionCells();
	for(UINT32 i=0; i < l_vCells.size(); i++)
	{		
		if(l_vCells[i].ActualState() == ECovertActionsCellState::Dormant && l_vCells[i].AssignedCountry() == in_iCountryID)
			l_vCells[i].ChangeState(ECovertActionsCellState::Active);
        else if(l_vCells[i].ActualState() == ECovertActionsCellState::ReadyToExecute)
        {
            const ENTITY_ID l_iTarget = l_vCells[i].AssignedCountry();
            if(g_ServerDAL.CountryData(l_iTarget)->Activated())
            {
                if(g_ServerDCL.ExecuteMission(l_vCells[i]))
                    l_CellsToRemove.insert(l_vCells[i].ID());
                l_vCells[i].ChangeState(ECovertActionsCellState::Active);
            }
            else
            {
                GDZLOG(EDZLogLevel::Always, L"Fixing save: " + l_pCountryData->NameAndIDForLog() + L" is cancelling a mission against " + GString(l_iTarget) + L" due to target inactivity");
                l_vCells[i].CancelAction();
                l_vCells[i].AssignedCountry(in_iCountryID);
            }
		}
			
	}

	for(set<UINT32>::iterator it = l_CellsToRemove.begin();
		 it != l_CellsToRemove.end(); it++)
	{
		l_pCountryData->RemoveCovertActionCell( l_pCountryData->CovertActionCell(*it) );
	}
}

void GAI::AskForTradeEmbargo(UINT32 in_iCountryCreatingEmbargo, UINT32 in_iCountrySufferingTheEmbargo)
{
	if(g_ServerDAL.TradeEmbargo(in_iCountryCreatingEmbargo,in_iCountrySufferingTheEmbargo))
		return;

	GCountryData* l_pSource = g_ServerDAL.CountryData(in_iCountryCreatingEmbargo);
	GCountryData* l_pTarget = g_ServerDAL.CountryData(in_iCountrySufferingTheEmbargo);

	if(l_pSource->EconomicRank() > l_pTarget->EconomicRank())
	{
		set<UINT32> l_vSideA;
		set<UINT32> l_vSideB;
		set<UINT32> l_vPressure;
		l_vSideA.insert(in_iCountryCreatingEmbargo);
		l_vSideB.insert(in_iCountrySufferingTheEmbargo);

		UINT32 l_Conditions[ETreatyConditions::ItemCount];
		Memory::Fill<UINT32>(l_Conditions,0,ETreatyConditions::ItemCount);

      GString l_sName = g_ServerDAL.TreatyName(ETreatyType::EconomicEmbargo,g_ServerDAL.GetString(g_ServerDAL.CountryData(in_iCountryCreatingEmbargo)->NameID()));
		g_ServerDCL.CreateNewTreaty(in_iCountryCreatingEmbargo,l_vSideA,l_vSideB,
			l_vPressure,ETreatyType::EconomicEmbargo,false,l_sName,l_Conditions);
	}
	else
	{
		//The economy is weaker. It will try to join another country's embargo
		const hash_map<UINT32, GTreaty>& l_Treaties = g_ServerDAL.Treaties();

		for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
			it != l_Treaties.end();++ it)
		{
			const GTreaty& l_CurTreaty = it->second;
			if(l_CurTreaty.Type() == ETreatyType::EconomicEmbargo)
			{						
				if(l_CurTreaty.CountrySide(in_iCountrySufferingTheEmbargo) == 2)
				{
					if(l_CurTreaty.CountrySide(in_iCountryCreatingEmbargo) == 2)	
						continue;
					if(l_CurTreaty.MembersSideA(true).find(in_iCountryCreatingEmbargo) !=
						l_CurTreaty.MembersSideA(true).end())
						continue;
					if(l_CurTreaty.Private() && !l_CurTreaty.IsAnOriginalA(in_iCountryCreatingEmbargo))
						continue;
					//If we get here, it means the country could join that treaty
					if(g_ServerDCL.CountryCanJoinTreaty(in_iCountryCreatingEmbargo,l_CurTreaty.ID(),true))
					{
						g_ServerDCL.JoinTreaty(in_iCountryCreatingEmbargo,l_CurTreaty.ID(),1);
						return;
					}
				}				
			}
		}
	}
}

void GAI::AskForWar(UINT32 in_iCountryLookingToDeclareWar, UINT32 in_iTarget, bool in_bOffensiveWar)
{	
	//Change that confidence, based on the country's stats and behaviors
	REAL32 l_fConfidence = 0.5f;
	l_fConfidence += (1.f - g_ServerDAL.CountryData(in_iCountryLookingToDeclareWar)->MaleFemale()) * (1.f - g_SP2Server->AIAggressiveness());

	//Check first if an existing "Request war declaration" exist towards that target.
	const hash_map<UINT32, GTreaty>& l_Treaties = g_ServerDAL.Treaties();
	
	bool l_bTreatyIsOk = true;
	set<UINT32> l_vSideBAllies;
	set<UINT32> l_vTreatiesToSign;

	for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
       it != l_Treaties.end();++ it)
   {
		const GTreaty& l_CurTreaty = it->second;
		if(l_CurTreaty.Type() == ETreatyType::War && 
			(l_CurTreaty.CountrySide(in_iCountryLookingToDeclareWar) == 1 || l_CurTreaty.IsAnOriginalA(in_iCountryLookingToDeclareWar)) &&
			l_CurTreaty.CountrySide(in_iTarget) == 2 )
		{
			l_bTreatyIsOk = true;
			l_vSideBAllies.clear();
			//This treaty is a possibilty. AI must check if it dislikes every country on Side B, 
			//and if it feels Side A is strong enough. If the AI has any ennemy on side A, it will also not accept
			set<UINT32> l_SideA = l_CurTreaty.MembersOriginalsA();
			set<UINT32> l_SideB = l_CurTreaty.MembersOriginalsB();

			REAL32 l_fStrengthSideA = 0.f;
			REAL32 l_fStrengthSideB = 0.f;

			for(set<UINT32>::const_iterator l_Itr = l_SideA.begin();
				 l_Itr != l_SideA.end(); l_Itr++)
			{
				if(g_ServerDAL.RelationBetweenCountries(*l_Itr,in_iCountryLookingToDeclareWar) <= SP2::c_fRelationsBad)
				{
					l_bTreatyIsOk = false;
					break;
				}					
				l_fStrengthSideA += g_ServerDAL.TotalUnitsValue(*l_Itr);
			}

			if(!l_bTreatyIsOk)
				continue;

			for(set<UINT32>::const_iterator l_Itr = l_SideB.begin();
				 l_Itr != l_SideB.end(); l_Itr++)
			{
				if(g_ServerDAL.RelationBetweenCountries(*l_Itr,in_iCountryLookingToDeclareWar) >= SP2::c_fRelationsGood)
				{
					l_bTreatyIsOk = false;
					break;
				}					
				l_vSideBAllies.insert(*l_Itr);
				g_ServerDAL.CountryAllies(*l_Itr,l_vSideBAllies);
			}

			if(!l_bTreatyIsOk)
				continue;

			//Erase the attacking country, so that we do not calculte it in the allies, if it's there
			l_vSideBAllies.erase(in_iCountryLookingToDeclareWar);

			for(set<UINT32>::const_iterator l_Itr = l_vSideBAllies.begin();
				 l_Itr != l_vSideBAllies.end(); l_Itr++)
			{
				l_fStrengthSideB += g_ServerDAL.TotalUnitsValue(*l_Itr);
			}

			if(l_fStrengthSideA > (l_fConfidence * l_fStrengthSideB))
				l_vTreatiesToSign.insert(l_CurTreaty.ID());
		}
	}

	for(set<UINT32>::const_iterator l_Itr = l_vTreatiesToSign.begin();
		 l_Itr != l_vTreatiesToSign.end();
		 l_Itr++)
	{
		g_ServerDCL.JoinTreaty(in_iCountryLookingToDeclareWar,*l_Itr,1);
	}

	//Do not need to create new war treaties
	if(l_vTreatiesToSign.size() > 0)
		return;
	
	//Ai hasn't found any war declaration treaties it likes. It will create one, with its target on side B
	// and with its allies on side A
	set<UINT32> l_vSideA;
	set<UINT32> l_vSideB;
	set<UINT32> l_vPressure;
	l_vSideA.insert(in_iCountryLookingToDeclareWar);
	l_vSideB.insert(in_iTarget);
	REAL32 l_fTotalValueA = g_ServerDAL.TotalUnitsValue(in_iCountryLookingToDeclareWar);
	multimap<REAL32,UINT32> l_mPossibilities;	

	for(UINT32 i=1; i<= g_ServerDAL.NbCountry(); i++)
	{
		if(i == in_iCountryLookingToDeclareWar || i == in_iTarget || !g_ServerDAL.CountryIsValid(i))
			continue;
		if(g_ServerDAL.RelationBetweenCountries(i,in_iCountryLookingToDeclareWar) >= SP2::c_fRelationsBad &&
			g_ServerDAL.RelationBetweenCountries(i,in_iTarget) <= SP2::c_fRelationsHate)
			l_mPossibilities.insert(make_pair(-g_ServerDAL.RelationBetweenCountries(i,in_iCountryLookingToDeclareWar),i));
	}

	for(multimap<REAL32,UINT32>::const_iterator l_Itr = l_mPossibilities.begin();
		 l_Itr != l_mPossibilities.end();
		 l_Itr++)
	{
		l_vSideA.insert(l_Itr->second);
		l_fTotalValueA += g_ServerDAL.TotalUnitsValue(l_Itr->second);
	}

	UINT32 l_Conditions[ETreatyConditions::ItemCount];
	Memory::Fill<UINT32>(l_Conditions,0,ETreatyConditions::ItemCount);	

	GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::War,g_ServerDAL.GetString(g_ServerDAL.CountryData(in_iCountryLookingToDeclareWar)->NameID()));
	g_ServerDCL.CreateNewTreaty(in_iCountryLookingToDeclareWar,l_vSideA,l_vSideB,l_vPressure,ETreatyType::War,false,l_sTreatyName,l_Conditions);
}

void GAI::ExamineLeavingTreaties(UINT32 in_iCountryID) const
{

	const hash_map<UINT32, GTreaty>& l_Treaties = g_ServerDAL.Treaties();

   for(hash_map<UINT32, GTreaty>::const_iterator it = l_Treaties.begin();
       it != l_Treaties.end();++ it)
   {
		const GTreaty& l_CurTreaty = it->second;

		UINT32 l_iSide = l_CurTreaty.CountrySide(in_iCountryID);
		
		if(!( l_iSide == 1 || (l_iSide == 2 && l_CurTreaty.SideBCanLeave()) ))
			continue;

		//Country should leave?
		if(CountryShouldJoinTreaty(in_iCountryID,l_CurTreaty.ID(),l_iSide) != ETreatyRefusal::Accepted)
		{
			g_ServerDCL.LeaveTreaty(in_iCountryID,l_CurTreaty.ID());
			return;
		}
		//Country should apply pressure?
		if(CountryShouldJoinTreaty(in_iCountryID,l_CurTreaty.ID(),3) == ETreatyRefusal::Accepted)
		{
			g_ServerDCL.JoinTreaty(in_iCountryID,l_CurTreaty.ID(),3);
			return;
		}
	}
}

bool GAI::ShouldCountryContinueOffensiveWar(const GWar& in_War, UINT32 in_iCountryID)
{
	const REAL32 ImportanceStrength	= 0.5f;
	const REAL32 ImportanceMoney		= 0.3f;
	const REAL32 ImportanceStability	= 0.2f;

	GTimeSpan l_TimeSpent = g_ServerDAL.ActualDate() - in_War.Date();
	if(in_War.MasterAttacking() != in_iCountryID)
		return false;

	//If the war has been going on for less than a month, the country will continue fighting
	if(l_TimeSpent.Days() < 31.f)
		return true;
	
	if(l_TimeSpent.Days() > 1825.f)
		return false;

	//Rank between 0 and 1 the status of each variable
	REAL32 l_fStrength	= (SDK::EHE::GManager::Objective(EHEObjectives::Ids::ENEMIES_NO_MILITARY_FORCES)->CurrentLevel(in_iCountryID) + 1.f) * 0.5f;
	REAL32 l_fMoney		= (SDK::EHE::GManager::Objective(EHEObjectives::Ids::HAVE_POSITIVE_BUDGET)->CurrentLevel(in_iCountryID) + 1.f) * 0.5f;
	REAL32 l_fStability	= (SDK::EHE::GManager::Objective(EHEObjectives::Ids::GOVERNMENT_STABILITY)->CurrentLevel(in_iCountryID) + 1.f) * 0.5f;

	REAL32 l_fScore = (l_fStrength * ImportanceStrength) + (l_fMoney * ImportanceMoney)
		+ (l_fStability * ImportanceStability);

	Random::GQuick l_Rand;
	l_Rand.Seed( (UINT32)(l_fStrength * l_fMoney * l_fStability * (REAL32)g_Joshua.GameTime() * (REAL32)in_iCountryID) );

	if(l_Rand.RandomReal() > l_fScore)
	{
		//Will ask for peace
		return false;
	}	
	else
	{
		//Will want to continue war
		return true;
	}
}

bool GAI::ShouldCountryContinueDefensiveWar(const GWar& in_War, UINT32 in_iCountryID)
{
	if(in_War.MasterDefending() != in_iCountryID)
		return false;

	GTimeSpan l_TimeSpent = g_ServerDAL.ActualDate() - in_War.Date();
	if(l_TimeSpent.Days() > 1825.f)
		return false;

	//If the country has no ground units left, it will surrender
	//Check the forces on the defending side
	const set<UINT32>& l_Defenders = in_War.DefendingSide();
	const set<UINT32>& l_Attackers = in_War.AttackingSide();
	REAL32 l_fGroundForcesDef = 0.f;
	REAL32 l_fGroundForcesAtt = 0.f;

	for(set<UINT32>::const_iterator it = l_Defenders.begin();
		 it != l_Defenders.end(); it++)
	{
		l_fGroundForcesDef += g_ServerDAL.TotalUnitsValue(*it,EUnitCategory::Air) + 
									 g_ServerDAL.TotalUnitsValue(*it,EUnitCategory::Ground) + 
									 g_ServerDAL.TotalUnitsValue(*it,EUnitCategory::Infantry);
	}
	for(set<UINT32>::const_iterator it = l_Attackers.begin();
		 it != l_Attackers.end(); it++)
	{
		l_fGroundForcesAtt += g_ServerDAL.TotalUnitsValue(*it,EUnitCategory::Air) + 
									 g_ServerDAL.TotalUnitsValue(*it,EUnitCategory::Ground) + 
									 g_ServerDAL.TotalUnitsValue(*it,EUnitCategory::Infantry);
	}

	
	if(l_fGroundForcesDef <= 1.f || 
	  (l_fGroundForcesAtt / l_fGroundForcesDef > 100.f))
	{
		return false;		
	}

	//If the attackers have stolen at least 1 regions, it will refuse peace
	const vector <GRegionControl>& l_vControls = g_ServerDAL.RegionControlArray();
	const set<UINT32>& l_PoliticalControl = g_ServerDAL.CountryPoliticalControl(in_iCountryID);

	for(set<UINT32>::const_iterator it = l_PoliticalControl.begin();
		 it != l_PoliticalControl.end(); it++)
	{
		if(l_Attackers.count( l_vControls[*it].m_iMilitary ))
			return true;			
	}

	return false;	
}

UINT32 GAI::ShouldCountryJoinWar(const GWar& in_War, UINT32 in_iCountryID)
{
	gassert(in_War.AttackingSide().size(), "Should have at least one member");
	gassert(in_War.DefendingSide().size(), "Should have at least one member");

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	GCountryData* l_pCountryDataDefending = g_ServerDAL.CountryData(in_War.MasterDefending());

	//Calculate relations towards both sides
	REAL32 l_fRelationsAttackers = 0.f;
	const set<UINT32>& l_Attackers = in_War.AttackingSide();
	REAL32 l_fRelationsDefenders = 0.f;

	for(set<UINT32>::const_iterator it = l_Attackers.begin();
		 it != l_Attackers.end(); it++)
	{
		l_fRelationsAttackers += g_ServerDAL.RelationBetweenCountries(in_iCountryID,*it);
	}
	l_fRelationsAttackers /= (REAL32)l_Attackers.size();

	l_fRelationsDefenders = g_ServerDAL.RelationBetweenCountries(in_iCountryID,in_War.MasterDefending());

	if(g_ServerDAL.DiplomaticStatus(in_iCountryID,in_War.MasterDefending()) == EDiplomaticStatus::Allied)
		l_fRelationsDefenders += 50.f;

	REAL32 l_fLowestRelations = -90.f + (20.f * g_SP2Server->AIAggressiveness());	

	if(l_fRelationsAttackers > l_fLowestRelations ||
		l_fRelationsDefenders < SP2::c_fRelationsLike) //MultiMOD - SP2::c_fRelationsEnnemy)
		return 0;

	REAL32 l_fTotalValuesEnemies = g_ServerDCL.TotalUnitsValueThatHateCountry(in_War.MasterAttacking());
	REAL32 l_fTotalValuesAttackers = 0.f;
	for(set<UINT32>::const_iterator it = l_Attackers.begin();
		it != l_Attackers.end(); it++)
	{
		l_fTotalValuesAttackers += g_ServerDAL.TotalUnitsValue(*it);
	}
	
	REAL32 l_fDifferenceValue = l_fRelationsAttackers - l_fRelationsDefenders;
	if(l_fTotalValuesEnemies > l_fTotalValuesAttackers * 0.2f && 
		l_fDifferenceValue <= -80.f)   
	{
		//The country has bad enough relations to have a war.. but does it really want to ?
		UINT32 l_iNbOfPointsInFavour = 0;


		hash_set<INT32> l_iGeoGroupAttacker; 
		//Fill the geogroup of the target
		const set<UINT32>& l_RegionsAttacker = g_ServerDAL.CountryPoliticalControl(in_War.MasterDefending());
		for(set<UINT32>::const_iterator it = l_RegionsAttacker.begin();
			 it != l_RegionsAttacker.end(); it++)
		{
			l_iGeoGroupAttacker.insert((UINT32)g_ServerDAL.RegionGeoGroup(*it));
		}
		
		bool l_bSameGeoGroup = false;
		const set<UINT32>& l_RegionsAI = g_ServerDAL.CountryPoliticalControl(in_iCountryID);
		for(set<UINT32>::const_iterator it = l_RegionsAI.begin();
			it != l_RegionsAI.end(); it++)
		{
			if(l_iGeoGroupAttacker.count((UINT32)g_ServerDAL.RegionGeoGroup(*it)) > 0)
			{
				l_bSameGeoGroup = true;
				break;
			}
		}

		if(g_ServerDAL.RelationBetweenCountries(in_iCountryID,in_War.MasterDefending()) > 30.f)
			l_iNbOfPointsInFavour++;

		if(l_bSameGeoGroup)
			l_iNbOfPointsInFavour++;

		if(g_ServerDAL.DiplomaticStatus(in_iCountryID,in_War.MasterDefending()) == EDiplomaticStatus::Allied)
			l_iNbOfPointsInFavour += 2;

		if(g_ServerDAL.TotalUnitsValue(in_iCountryID) > g_ServerDAL.TotalUnitsValue(in_War.MasterAttacking()) ||
			g_ServerDAL.TotalMissileValue(in_iCountryID) > g_ServerDAL.TotalMissileValue(in_War.MasterAttacking()))
			l_iNbOfPointsInFavour++;

		if(l_pCountryDataDefending->EconomicRank() <= 15)
			l_iNbOfPointsInFavour++;

		if(l_pCountryData->GvtStability() > 0.4f)
			l_iNbOfPointsInFavour++;

		if(l_pCountryData->BudgetRevenues() > (l_pCountryData->BudgetExpenses() * 0.8f))
			l_iNbOfPointsInFavour++;

		if( abs(l_pCountryData->CountryEconomicModel() - l_pCountryDataDefending->CountryEconomicModel()) <= 0.1f)
			l_iNbOfPointsInFavour++;

		if(l_iNbOfPointsInFavour > 3)
			return 2;
	}
	else if(l_fTotalValuesEnemies < (l_fTotalValuesAttackers + g_ServerDAL.TotalUnitsValue(in_iCountryID)) * (0.3f + (1.f - g_SP2Server->AIAggressiveness()) * 0.65f) &&
			  l_fRelationsAttackers > SP2::c_fRelationsBad &&
			  l_fRelationsDefenders < SP2::c_fRelationsHate)
	{
		//Check if move is possible towards defending master
		const SP2::SCityInfo* l_pCapitalAI = g_ServerDAL.CapitalCity(in_iCountryID);
		if(l_pCapitalAI)
		{
			const SP2::SCityInfo* l_pCapitalEnemy = g_ServerDAL.CapitalCity(in_War.MasterDefending());
			if(l_pCapitalEnemy)
			{
				if(g_ServerDCL.MoveHostileFuturePossible(in_iCountryID,l_pCapitalAI->m_iRegionId,l_pCapitalEnemy->m_iRegionId))
				{
					return 1;
				}
			}
			else
			{
				//If the enemy has no capital, find a military region
				const set<UINT32>& l_RegionsPolitic = g_ServerDAL.CountryMilitaryControl(in_War.MasterDefending());
				if(l_RegionsPolitic.size())
				{
					if(g_ServerDCL.MoveHostileFuturePossible(in_iCountryID,l_pCapitalAI->m_iRegionId,*(l_RegionsPolitic.begin())))
					{
						return 1;
					}
				}
			}
		}
	}

	return 0;
}

void GAI::ExamineWarAndPeacePossibilities(UINT32 in_iCountryID)
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	const UINT8* l_pStatuses = g_ServerDAL.CountryDiplomaticStatuses(in_iCountryID);

	const hash_map<UINT32,GWar>& l_Wars = g_ServerDAL.CurrentWars();
	set<UINT32> l_WarsToJoinAttackers;
	set<UINT32> l_WarsToJoinDefenders;
	set<UINT32> l_WarsToGetOutAttackers;
	set<UINT32> l_WarsToGetOutDefenders;
	//Cycle through the current wars, and examine which one should the country join or leave
	for(hash_map<UINT32,GWar>::const_iterator it = l_Wars.begin();
			it != l_Wars.end(); it++)
	{
		const GWar& l_CurWar = it->second;
		if(l_CurWar.AttackingSide().count(in_iCountryID))
		{
			if(ShouldCountryContinueOffensiveWar(l_CurWar,in_iCountryID))
				l_WarsToJoinAttackers.insert(it->first);
			else if(l_CurWar.MasterAttacking() == in_iCountryID)
				l_WarsToGetOutAttackers.insert(it->first);
		}
		else if(l_CurWar.DefendingSide().count(in_iCountryID))
		{
			if(ShouldCountryContinueDefensiveWar(l_CurWar,in_iCountryID))
				l_WarsToJoinDefenders.insert(it->first);
			else if(l_CurWar.MasterDefending() == in_iCountryID)
				l_WarsToGetOutDefenders.insert(it->first);

			//A country will also give trespassing rights to its allies in war
			if(l_CurWar.MasterDefending() == in_iCountryID)
			{
				for(set<UINT32>::const_iterator itr = l_CurWar.DefendingSide().begin();
					 itr != l_CurWar.DefendingSide().end(); itr++)
				{
					if((EDiplomaticStatus::Enum)l_pStatuses[*itr] == EDiplomaticStatus::Neutral &&
						g_ServerDAL.RelationBetweenCountries(*itr,in_iCountryID) > 0.f &&				
						g_ServerDAL.CountryData(*itr)->MilitaryRank() < l_pCountryData->MilitaryRank())
					{
						//Create military access treaty
						GString l_sNewTreatyName = g_ServerDAL.TreatyName(ETreatyType::MilitaryAccess,g_ServerDAL.GetString(g_ServerDAL.CountryData(in_iCountryID)->NameID()));
						UINT32 l_Conditions[ETreatyConditions::ItemCount];
						Memory::Fill<UINT32>(l_Conditions,0,ETreatyConditions::ItemCount);
						l_Conditions[ETreatyConditions::RelationsNotAtWar] = EConditionStatusRelation::NotAtWar;
						set<UINT32> l_SideAMembers;
						set<UINT32> l_SideBMembers;
						set<UINT32> l_SidePressure;
						l_SideBMembers.insert(*itr);
						l_SideAMembers.insert(in_iCountryID);
						g_ServerDCL.CreateNewTreaty(in_iCountryID,l_SideAMembers,l_SideBMembers,l_SidePressure,ETreatyType::MilitaryAccess,true,l_sNewTreatyName,l_Conditions);
					}
				}
			}
		}
		else
		{
			UINT32 l_iDecision = ShouldCountryJoinWar(l_CurWar,in_iCountryID);
			if(l_iDecision == 1)
			{
				l_WarsToJoinAttackers.insert(it->first);
			}
			else if(l_iDecision == 2)
			{
				l_WarsToJoinDefenders.insert(it->first);
			}
		}
	}
	
	//Iterate the wars the country join on attacker side
	for(set<UINT32>::iterator it = l_WarsToJoinAttackers.begin();
		  it != l_WarsToJoinAttackers.end(); it++)
	{
		GWar* l_pWar = g_ServerDAL.War(*it);
		if(!l_pWar)
			continue;
		if(l_pWar->AttackingSide().count(in_iCountryID))
		{
			g_ServerDCL.ChangeOpinionOnWar(in_iCountryID,*it,false);
		}
		else
		{
			g_ServerDCL.JoinAWar(in_iCountryID,*it,1,true);
		}
		GAI::OrderBuildUnitsAgainstEnnemy(in_iCountryID,l_pWar->MasterDefending());
		GAI::DeployUnits(in_iCountryID);		
	}

	//Iterate the wars the country join on defender side
	for(set<UINT32>::iterator it = l_WarsToJoinDefenders.begin();
		  it != l_WarsToJoinDefenders.end(); it++)
	{
		GWar* l_pWar = g_ServerDAL.War(*it);
		if(!l_pWar)
			continue;
		if(l_pWar->DefendingSide().count(in_iCountryID))
		{
			g_ServerDCL.ChangeOpinionOnWar(in_iCountryID,*it,false);
		}
		else
		{
			g_ServerDCL.JoinAWar(in_iCountryID,*it,2,true);
		}
		GAI::OrderBuildUnitsAgainstEnnemy(in_iCountryID,l_pWar->MasterAttacking());
		GAI::DeployUnits(in_iCountryID);		
	}

	//Iterate the wars where the country leave the attacker side
	for(set<UINT32>::iterator it = l_WarsToGetOutAttackers.begin();
		  it != l_WarsToGetOutAttackers.end(); it++)
	{
		g_ServerDCL.ChangeOpinionOnWar(in_iCountryID,*it,true);
	}

	//Iterate the wars where the country leave the defender side
	for(set<UINT32>::iterator it = l_WarsToGetOutDefenders.begin();
		  it != l_WarsToGetOutDefenders.end(); it++)
	{
		g_ServerDCL.ChangeOpinionOnWar(in_iCountryID,*it,true);		
	}
}

void GAI::ExamineUnitMovement(UINT32 in_iCountryID) const
{	
	const REAL64 c_fRatioUnitsNeededHome = 0.5f;

	DeployUnits(in_iCountryID);	

	//If the country is at war, do not do anything here, since the combat manager is taking care of it
	if(g_ServerDAL.IsAtWar(in_iCountryID))
		return;		

	const vector<GRegionControl>& l_vControl = g_ServerDAL.RegionControlArray();
	const set<UINT32>& l_UnitGroups = g_Joshua.UnitManager().CountryUnitGroups(in_iCountryID);

	const set<UINT32>& l_RegionsPolitical = g_ServerDAL.CountryPoliticalControl(in_iCountryID);	
	vector<SP2::GUnitGroup*> l_vUnitsThatMightMoveTowardsCountry;
	set<UINT32> l_UnitsThatMoved;
	vector<SP2::GUnitGroup*> l_vUnitsThatMightPark;
	multimap<INT64,GRegion*> l_mRegionPossibilities;

	REAL64 l_fTotalValue = 0.f;
	REAL64 l_fTotalValueInsideCountry = 0.f;

	for(set<UINT32>::const_iterator it = l_UnitGroups.begin();
		 it != l_UnitGroups.end(); it++)
	{
		SP2::GUnitGroup* l_pUnitGroup =  (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*it);
		if(l_pUnitGroup->IsNaval())
			continue;
		REAL64 l_fTempValue = l_pUnitGroup->Value();
		EMilitaryStatus::Enum l_eStatus = l_pUnitGroup->Status();	
		if(l_eStatus == EMilitaryStatus::Fortified ||
			l_eStatus == EMilitaryStatus::Ready ||
			l_eStatus == EMilitaryStatus::Moving ||
			l_eStatus == EMilitaryStatus::Parked)
		{
			if(l_eStatus != EMilitaryStatus::Moving)
				l_vUnitsThatMightPark.push_back(l_pUnitGroup);

			if(l_vControl[((SP2::GUnitGroupEx*)l_pUnitGroup)->m_iRegionID].m_iPolitical == in_iCountryID)
				l_fTotalValueInsideCountry += l_fTempValue;
			else
				l_vUnitsThatMightMoveTowardsCountry.push_back(l_pUnitGroup);
		}
		l_fTotalValue += l_fTempValue;
	}

	if(l_fTotalValue <= 0.f)
	{
		//No units, leave	
		return;
	}
	
	UINT32 l_iUnitGroupsByRegions = 0;
	if(l_fTotalValueInsideCountry / l_fTotalValue < c_fRatioUnitsNeededHome)
	{
		//We need to move units, fill up the region possibilities
		for(set<UINT32>::const_iterator it = l_RegionsPolitical.begin();
			 it != l_RegionsPolitical.end(); it++)
		{
			GRegion* l_pRegion = g_ServerDAL.GetGRegion(*it);
			l_mRegionPossibilities.insert(make_pair(-l_pRegion->Population(),l_pRegion));
		}
		if(l_RegionsPolitical.size() > 0)
			l_iUnitGroupsByRegions = (UINT32)((REAL32)l_vUnitsThatMightMoveTowardsCountry.size() / (REAL32)l_RegionsPolitical.size()) + 1;
	}
	
	//Needs to move more units towards the home country
	UINT32 l_iIteratorUnitGroups = 0;
	for(multimap<INT64,GRegion*>::iterator it = l_mRegionPossibilities.begin();
		 ((it != l_mRegionPossibilities.end()) && (l_iIteratorUnitGroups < l_vUnitsThatMightMoveTowardsCountry.size())
		 && (l_fTotalValueInsideCountry / l_fTotalValue < c_fRatioUnitsNeededHome)); 
		 it++)
	{
		for(UINT32 i=0; ( (i < l_iUnitGroupsByRegions) && (l_iIteratorUnitGroups < l_vUnitsThatMightMoveTowardsCountry.size())); i++)
		{
			
			g_ServerDCL.UnitMover().MoveUnitToRegion(l_vUnitsThatMightMoveTowardsCountry[l_iIteratorUnitGroups]->Id(),
				it->second->Id(), EMilitaryAction::MergeWith);							
			l_UnitsThatMoved.insert(l_vUnitsThatMightMoveTowardsCountry[l_iIteratorUnitGroups]->Id());
			l_fTotalValueInsideCountry += l_vUnitsThatMightMoveTowardsCountry[l_iIteratorUnitGroups]->Value();
			l_iIteratorUnitGroups++;
		}
	}

	//Change the status of remaining unit group to parked
	for(UINT32 i=0; i<l_vUnitsThatMightPark.size(); i++)
	{
		SP2::GUnitGroup* l_pUnitGroup = l_vUnitsThatMightPark[i];

		if(l_UnitsThatMoved.find(l_pUnitGroup->Id()) != l_UnitsThatMoved.end())
			continue;

		if(l_pUnitGroup->Status() == EMilitaryStatus::Moving)
			continue;

        gassert(!l_pUnitGroup->IsNaval(), L"Naval unit group " + GString(l_pUnitGroup->Id()) + L" of " + g_ServerDAL.CountryData(in_iCountryID)->NameAndIDForLog() + L" shouldn't have been added to l_vUnitsThatMightPark");

		UINT32 l_iRegionID = ((SP2::GUnitGroupEx*)l_pUnitGroup)->m_iRegionID;
		
		if(l_iRegionID == 0)
		{
			//Unit is somehow over the water. Let's move it towards a friendly region			
			continue;
		}

		UINT32 l_iRegionPoliticalControl = l_vControl[l_iRegionID].m_iPolitical;
		UINT32 l_iRegionMilitaryControl = l_vControl[l_iRegionID].m_iMilitary;
		if(l_iRegionPoliticalControl == in_iCountryID &&
			l_iRegionMilitaryControl == in_iCountryID)
		{
            if(l_pUnitGroup->Status() != EMilitaryStatus::Parked)
			    g_ServerDCL.UnitMover().ChangeUnitState((SDK::Combat::GUnitGroup*)l_pUnitGroup,EMilitaryStatus::Parked);
		}
		else if((l_iRegionPoliticalControl == in_iCountryID) !=
                (l_iRegionMilitaryControl  == in_iCountryID))
		{
            if(l_pUnitGroup->Status() != EMilitaryStatus::Fortified)
			    g_ServerDCL.UnitMover().ChangeUnitState((SDK::Combat::GUnitGroup*)l_pUnitGroup,EMilitaryStatus::Fortified);
		}
		else if(l_iRegionPoliticalControl != in_iCountryID &&
				  l_iRegionMilitaryControl != in_iCountryID)
		{
            if(l_pUnitGroup->Status() != EMilitaryStatus::Ready)
			    g_ServerDCL.UnitMover().ChangeUnitState((SDK::Combat::GUnitGroup*)l_pUnitGroup,EMilitaryStatus::Ready);
		}
	}	

	//Finally, check if two immobiles unit groups are closed to each other, and if so, merge them
	hash_map<UINT32,Hector::GVector2D<REAL32> > l_UnitsToMove;
	set<UINT32> l_UnitsBeingMovedTo;	
	for(set<UINT32>::const_iterator it = l_UnitGroups.begin();
		 it != l_UnitGroups.end(); it++)
	{
		SP2::GUnitGroup* l_pUnitGroup =  (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*it);
		if(l_UnitsBeingMovedTo.find(l_pUnitGroup->Id()) != l_UnitsBeingMovedTo.end() ||
			!(l_pUnitGroup->Status() == EMilitaryStatus::Fortified || l_pUnitGroup->Status() == EMilitaryStatus::Ready ||
			 l_pUnitGroup->Status() == EMilitaryStatus::Parked))
			continue;
		if(l_pUnitGroup->IsNaval())
		{
			for(set<UINT32>::const_iterator it2 = l_UnitGroups.begin();
				it2 != l_UnitGroups.end(); it2++)
			{
				SP2::GUnitGroup* l_pUnitGroup2 =  (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*it2);
				if(l_pUnitGroup2->Id() != l_pUnitGroup->Id() && l_pUnitGroup->IsNaval() && 
					l_UnitsToMove.find(l_pUnitGroup2->Id()) == l_UnitsToMove.end() &&
					(l_pUnitGroup2->Status() == EMilitaryStatus::Fortified || l_pUnitGroup2->Status() == EMilitaryStatus::Ready ||
					 l_pUnitGroup2->Status() == EMilitaryStatus::Parked) &&
					 GDALInterface::DistanceBetween2PointsLatLong(l_pUnitGroup->Position(),l_pUnitGroup2->Position()) <= 5.f)
				{
					//Move the first unit group to that unit group
					l_UnitsBeingMovedTo.insert(l_pUnitGroup2->Id());
					l_UnitsToMove[l_pUnitGroup->Id()] = l_pUnitGroup2->Position();
				}
			}
		}
		else 
		{
			for(set<UINT32>::const_iterator it2 = l_UnitGroups.begin();
				it2 != l_UnitGroups.end(); it2++)
			{
				SP2::GUnitGroup* l_pUnitGroup2 =  (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*it2);
				if(l_pUnitGroup2->Id() != l_pUnitGroup->Id() && !l_pUnitGroup->IsNaval() && 
					l_UnitsToMove.find(l_pUnitGroup2->Id()) == l_UnitsToMove.end() &&
					(l_pUnitGroup2->Status() == EMilitaryStatus::Fortified || l_pUnitGroup2->Status() == EMilitaryStatus::Ready ||
					 l_pUnitGroup2->Status() == EMilitaryStatus::Parked) &&
					 GDALInterface::DistanceBetween2PointsLatLong(l_pUnitGroup->Position(),l_pUnitGroup2->Position()) <= 5.f)
				{
					//Move the first unit group to that unit group
					l_UnitsBeingMovedTo.insert(l_pUnitGroup2->Id());
					l_UnitsToMove[l_pUnitGroup->Id()] = l_pUnitGroup2->Position();
				}
			}
		}
	}

	for(hash_map<UINT32,Hector::GVector2D<REAL32> >::iterator it = l_UnitsToMove.begin();
		 it != l_UnitsToMove.end(); it++)
	{
		vector<SDK::Combat::GUnitGroup*> l_Vector;
		l_Vector.push_back(g_Joshua.UnitManager().UnitGroup(it->first));
		g_ServerDCL.UnitMover().MoveUnits(l_Vector,it->second,EMilitaryAction::MergeWith);
	}	

}

void GAI::ExamineBudgetReadjustment(UINT32 in_iCountryID) const
{	
	if(g_ServerDAL.CountryData(in_iCountryID)->BudgetExpenses() >= 
			g_ServerDAL.CountryData(in_iCountryID)->BudgetRevenues())
		ExamineBudgetCuts(in_iCountryID);
	else
		ExamineBudgetFundings(in_iCountryID);
}

void GAI::ExamineBudgetCuts(UINT32 in_iCountryID) const
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(l_pCountryData->BudgetExpenses() <= 0.f)
		return;

	bool l_bIsAtWar = g_ServerDAL.IsAtWar(in_iCountryID);

	REAL32 l_fImportance[EBudgetExpenses::ItemCount];
	Memory::Fill<REAL32>(l_fImportance,1.f,EBudgetExpenses::ItemCount);
		
	if(l_pCountryData->BudgetRevenues() < l_pCountryData->BudgetExpenseDebt())
	{
		//Drop all expenses by 50%
		const REAL64 c_fDropRatioUrgent = 0.5f;
		g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Education,l_pCountryData->BudgetExpenseEducationRatio() * c_fDropRatioUrgent);
		g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Environment,l_pCountryData->BudgetExpenseEnvironmentRatio() * c_fDropRatioUrgent);
		g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Government,l_pCountryData->BudgetExpenseGovernmentRatio() * c_fDropRatioUrgent);
		g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Healthcare,l_pCountryData->BudgetExpenseHealthcareRatio() * c_fDropRatioUrgent);
		g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::IMF,l_pCountryData->BudgetExpenseIMFRatio() * c_fDropRatioUrgent);
		g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Infrastructure,l_pCountryData->BudgetExpenseInfrastructureRatio() * c_fDropRatioUrgent);
		g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Propaganda,l_pCountryData->BudgetExpensePropagandaRatio() * c_fDropRatioUrgent);
		g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Research,l_pCountryData->BudgetExpenseResearchRatio() * c_fDropRatioUrgent);
		g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Telecom,l_pCountryData->BudgetExpenseTelecomRatio() * c_fDropRatioUrgent);
		g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Tourism,l_pCountryData->BudgetExpenseTourismRatio() * c_fDropRatioUrgent);
	}
	else
	{
		REAL64 l_fDropRatioNormal = 0.1f;
		const REAL64 c_fNextDrops = 0.005f;
		
		//Go by priority		
		l_fImportance[EBudgetExpenses::Propaganda] += 
			g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionPlus,
															  SP2::c_fAIActionPlus,
															  SP2::c_fAIActionDoublePlus) - 0.5f;
		l_fImportance[EBudgetExpenses::Environment] += 
			g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionMinus,
															  SP2::c_fAIActionDoubleMinus,
															  SP2::c_fAIActionDoubleMinus) - 0.5f;
		l_fImportance[EBudgetExpenses::Healthcare] += 
			g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionMinus,
															  SP2::c_fAIActionDoubleMinus,
															  SP2::c_fAIActionNeutral) - 0.5f;
		l_fImportance[EBudgetExpenses::Education] += 
			g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionMinus,
															  SP2::c_fAIActionDoubleMinus,
															  SP2::c_fAIActionNeutral) - 0.5f;
		l_fImportance[EBudgetExpenses::Telecom] += 
			g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionDoublePlus,
															  SP2::c_fAIActionMinus,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionNeutral) - 0.5f;
		l_fImportance[EBudgetExpenses::Government] += 
			g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionPlus,
															  SP2::c_fAIActionMinus,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionPlus) - 0.5f;
		l_fImportance[EBudgetExpenses::IMF] += 
			g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionDoubleMinus,
															  SP2::c_fAIActionDoubleMinus) - 0.5f;
		l_fImportance[EBudgetExpenses::Research] += 
			g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionNeutral,
															  SP2::c_fAIActionPlus,
															  SP2::c_fAIActionPlus) - 0.5f;
		l_fImportance[EBudgetExpenses::Tourism] += 
			g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															  SP2::c_fAIActionPlus,
															  SP2::c_fAIActionMinus,
															  SP2::c_fAIActionMinus,
															  SP2::c_fAIActionDoubleMinus) - 0.5f;

		if(l_bIsAtWar)
		{
			l_fImportance[EBudgetExpenses::Research] += 0.5f;
			l_fImportance[EBudgetExpenses::IMF] -= 0.5f;
			l_fImportance[EBudgetExpenses::Tourism] -= 0.5f;
		}		

		multimap<REAL32,EBudgetExpenses::Enum> l_mExpenses;
		for(UINT32 i=0; i < EBudgetExpenses::ItemCount; i++)
			l_mExpenses.insert(make_pair(l_fImportance[i],(EBudgetExpenses::Enum)i));

		REAL64 l_fRevenues = l_pCountryData->BudgetRevenues();

		for(multimap<REAL32,EBudgetExpenses::Enum>::iterator it = l_mExpenses.begin();
			 (it != l_mExpenses.end()) && (l_pCountryData->BudgetExpenses() > l_fRevenues);
			 it++)
		{
			switch(it->second)
			{
			case EBudgetExpenses::Education:
				g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Education,l_pCountryData->BudgetExpenseEducationRatio() - l_fDropRatioNormal); 
				break;
			case EBudgetExpenses::Environment:
				g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Environment,l_pCountryData->BudgetExpenseEnvironmentRatio() - l_fDropRatioNormal); 
				break;
			case EBudgetExpenses::Government:
				g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Government,l_pCountryData->BudgetExpenseGovernmentRatio() - l_fDropRatioNormal); 
				break;
			case EBudgetExpenses::Healthcare:
				g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Healthcare,l_pCountryData->BudgetExpenseHealthcareRatio() - l_fDropRatioNormal); 				
				break;
			case EBudgetExpenses::IMF:
				g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::IMF,l_pCountryData->BudgetExpenseIMFRatio() - l_fDropRatioNormal); 
				break;
			case EBudgetExpenses::Infrastructure:
				g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Infrastructure,l_pCountryData->BudgetExpenseInfrastructureRatio() - l_fDropRatioNormal); 
				break;
			case EBudgetExpenses::Propaganda:
				g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Propaganda,l_pCountryData->BudgetExpensePropagandaRatio() - l_fDropRatioNormal); 
				break;
			case EBudgetExpenses::Research:
				g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Research,l_pCountryData->BudgetExpenseResearchRatio() - l_fDropRatioNormal); 
				break;
			case EBudgetExpenses::Telecom:
				g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Telecom,l_pCountryData->BudgetExpenseTelecomRatio() - l_fDropRatioNormal); 
				break;
			case EBudgetExpenses::Tourism:
				g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Tourism,l_pCountryData->BudgetExpenseTourismRatio() - l_fDropRatioNormal); 
				break;
			}
			l_fDropRatioNormal -= c_fNextDrops;
		}	
	}

	return;
}
void GAI::ExamineBudgetFundings(UINT32 in_iCountryID) const
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	if(l_pCountryData->BudgetExpenses() <= 0.f)
		return;
	bool l_bIsAtWar = g_ServerDAL.IsAtWar(in_iCountryID);

	REAL32 l_fImportance[EBudgetExpenses::ItemCount];
	Memory::Fill<REAL32>(l_fImportance,1.f,EBudgetExpenses::ItemCount);
	
    const GPoliticalParty* const l_pLeaderParty = l_pCountryData->LeaderParty();
    REAL32 l_fMinimumPourcAllowedToArmy = 0.25f;
    if(l_pLeaderParty != nullptr)
        l_fMinimumPourcAllowedToArmy *= l_pLeaderParty->PoliticalIdeology();
    else
    {
        GDZLOG(EDZLogLevel::Error, l_pCountryData->NameAndIDForLog() + L" has no leading political party");
        l_fMinimumPourcAllowedToArmy *= 0.5f;
    }

	if(l_pCountryData->BudgetExpenseUnitUpkeep() /  l_pCountryData->BudgetExpenses() < (REAL64)l_fMinimumPourcAllowedToArmy
		&& !l_bIsAtWar)
	{
		//Ask to create more units. The country can afford it
	}

	
	const REAL64 c_fMargeProfits = 0.95f;
	REAL64 l_fRaiseRatioNormal = 0.05f;
	const REAL64 c_fNextRaise = 0.005f;
	//Go by priority
	l_fImportance[EBudgetExpenses::Propaganda] += 
		0.5f - g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															SP2::c_fAIActionNeutral,
															SP2::c_fAIActionPlus,
															SP2::c_fAIActionPlus,
															SP2::c_fAIActionDoublePlus);
	l_fImportance[EBudgetExpenses::Environment] += 
		0.5f - g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															SP2::c_fAIActionNeutral,
															SP2::c_fAIActionMinus,
															SP2::c_fAIActionDoubleMinus,
															SP2::c_fAIActionDoubleMinus);
	l_fImportance[EBudgetExpenses::Healthcare] += 
		0.5f - g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															SP2::c_fAIActionNeutral,
															SP2::c_fAIActionMinus,
															SP2::c_fAIActionDoubleMinus,
															SP2::c_fAIActionNeutral);
	l_fImportance[EBudgetExpenses::Education] += 
		0.5f - g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															SP2::c_fAIActionNeutral,
															SP2::c_fAIActionMinus,
															SP2::c_fAIActionDoubleMinus,
															SP2::c_fAIActionNeutral);
	l_fImportance[EBudgetExpenses::Telecom] += 
		0.5f - g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															SP2::c_fAIActionDoublePlus,
															SP2::c_fAIActionMinus,
															SP2::c_fAIActionNeutral,
															SP2::c_fAIActionNeutral);
	l_fImportance[EBudgetExpenses::Government] += 
		0.5f - g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															SP2::c_fAIActionPlus,
															SP2::c_fAIActionMinus,
															SP2::c_fAIActionNeutral,
															SP2::c_fAIActionPlus);
	l_fImportance[EBudgetExpenses::IMF] += 
		0.5f - g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															SP2::c_fAIActionNeutral,
															SP2::c_fAIActionNeutral,
															SP2::c_fAIActionDoubleMinus,
															SP2::c_fAIActionDoubleMinus);
	l_fImportance[EBudgetExpenses::Research] += 
		0.5f - g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															SP2::c_fAIActionNeutral,
															SP2::c_fAIActionNeutral,
															SP2::c_fAIActionPlus,
															SP2::c_fAIActionPlus);
	l_fImportance[EBudgetExpenses::Tourism] += 
		0.5f - g_ServerDCL.CalculateProbabilityOfAction(l_pCountryData,
															SP2::c_fAIActionPlus,
															SP2::c_fAIActionMinus,
															SP2::c_fAIActionMinus,
															SP2::c_fAIActionDoubleMinus);

	if(l_bIsAtWar)
	{
		l_fImportance[EBudgetExpenses::Research] -= 0.5f;
		l_fImportance[EBudgetExpenses::IMF] += 0.5f;
		l_fImportance[EBudgetExpenses::Tourism] += 0.5f;
	}

	multimap<REAL32,EBudgetExpenses::Enum> l_mExpenses;
	for(UINT32 i=0; i < EBudgetExpenses::ItemCount; i++)
		l_mExpenses.insert(make_pair(l_fImportance[i],(EBudgetExpenses::Enum)i));

	REAL64 l_fRevenues = l_pCountryData->BudgetRevenues() * c_fMargeProfits;

	for(multimap<REAL32,EBudgetExpenses::Enum>::iterator it = l_mExpenses.begin();
			(it != l_mExpenses.end()) && (l_pCountryData->BudgetExpenses() < l_fRevenues);
			it++)
	{		
		switch(it->second)
		{
		case EBudgetExpenses::Education:
			g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Education,l_pCountryData->BudgetExpenseEducationRatio() + l_fRaiseRatioNormal); 
			break;
		case EBudgetExpenses::Environment:
			g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Environment,l_pCountryData->BudgetExpenseEnvironmentRatio() + l_fRaiseRatioNormal); 
			break;
		case EBudgetExpenses::Government:
			g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Government,l_pCountryData->BudgetExpenseGovernmentRatio() + l_fRaiseRatioNormal); 
			break;
		case EBudgetExpenses::Healthcare:
			g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Healthcare,l_pCountryData->BudgetExpenseHealthcareRatio() + l_fRaiseRatioNormal); 
			break;
		case EBudgetExpenses::IMF:
			g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::IMF,l_pCountryData->BudgetExpenseIMFRatio() + l_fRaiseRatioNormal); 
			break;
		case EBudgetExpenses::Infrastructure:
			g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Infrastructure,l_pCountryData->BudgetExpenseInfrastructureRatio() + l_fRaiseRatioNormal); 
			break;
		case EBudgetExpenses::Propaganda:
			g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Propaganda,l_pCountryData->BudgetExpensePropagandaRatio() + l_fRaiseRatioNormal); 
			break;
		case EBudgetExpenses::Research:
			g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Research,l_pCountryData->BudgetExpenseResearchRatio() + l_fRaiseRatioNormal); 
			break;
		case EBudgetExpenses::Telecom:
			g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Telecom,l_pCountryData->BudgetExpenseTelecomRatio() + l_fRaiseRatioNormal); 
			break;
		case EBudgetExpenses::Tourism:
			g_ServerDCL.ChangeBudgetExpenseRatio(in_iCountryID,EBudgetExpenses::Tourism,l_pCountryData->BudgetExpenseTourismRatio() + l_fRaiseRatioNormal); 
			break;
		}
		l_fRaiseRatioNormal -= c_fNextRaise;
	}	

}

bool GAI::ShouldAnnexCountry(UINT32 in_iCountryLookingToAnnex, UINT32 in_iTarget)
{
	const SP2::SCityInfo* l_pCapital = g_ServerDAL.CapitalCity(in_iCountryLookingToAnnex);
	if(!l_pCapital)
		return true;

	UINT32 l_iCapitalRegion = l_pCapital->m_iRegionId;

	//Check if any of those regions touches the wanting country
	UINT32 l_iNbOfRegions = 0;
	REAL32 l_fDecision = 0.f;
	REAL32 l_fRegionsWeight = 0.f;
	
	//Politically-controlled regions for the giving country
	const set<UINT32>& l_vRegionsPolitical = g_ServerDAL.CountryPoliticalControl(in_iTarget);
	
	//Iterate through the regions of that country, and check if the regions being annexed (maybe) are worth it
	for(set<UINT32>::const_iterator l_RegionItr = l_vRegionsPolitical.begin();
	(l_RegionItr != l_vRegionsPolitical.end());
	l_RegionItr++)
	{
		UINT32 l_iRegionID = *l_RegionItr;
		GRegion* l_pRegion = g_ServerDAL.GetGRegion(l_iRegionID);

		if(l_pRegion->OwnerMilitaryId() == in_iCountryLookingToAnnex)
		{
			l_iNbOfRegions++;
			l_fRegionsWeight += l_pRegion->PercentageValue();

			//If the region is adjacent to the 2 countries, the treaty has a higher possibility of being accepted by the AI
			if(g_ServerDAL.RegionTouchesCountry(l_iRegionID, in_iCountryLookingToAnnex))
				l_fDecision++;
			else if (g_ServerDCL.MovePossible(in_iCountryLookingToAnnex,l_iCapitalRegion,l_iRegionID))
				l_fDecision += 0.5;
			else
				l_fDecision -= 2.f;

			//MultiMOD
			for (UINT32 i = 1; i<= g_ServerDAL.NbCountry(); i++)
			{
				REAL32 l_fRelations = 0.f;
				l_fRelations = g_ServerDAL.RelationBetweenCountries(i, in_iCountryLookingToAnnex);
				if (in_iCountryLookingToAnnex == i)
					continue;

				if (l_fRelations >= SP2::c_fRelationsLove)
					l_fDecision +=0.5f;
				if (l_fRelations <= SP2::c_fRelationsLike)
					l_fDecision -= 0.1f * g_SP2Server->AnnexationRelationLossPercent();
			}
		}
	}

	if(l_iNbOfRegions == 0)
		return false;

    GDZLOG(EDZLogLevel::Info1, g_ServerDAL.CountryData(in_iCountryLookingToAnnex)->NameAndIDForLog() + L"'s possible annexation of " + g_ServerDAL.CountryData(in_iTarget)->NameAndIDForLog() + L": Decision " + GString(l_fDecision));

	if(l_fDecision <= 0.f)
		return false;
	
	return true;
}

SP2::GUnitDesign* GAI::CreateNewOptimalDesign(UINT32 in_iCountryID, EUnitCategory::Enum in_eCategory, EUnitType::Enum in_eType)
{
	SP2::GResearchInformation* l_pInfo = (SP2::GResearchInformation*)g_ServerDAL.CountryData(in_iCountryID)->ResearchInfo();	
   UINT8 l_iMaxResearch[EUnitDesignCharacteristics::ItemCount];


   //If the design cant be built
   if(!g_ServerDCL.CanBuildUnitDesign(in_iCountryID,in_eCategory))
      return NULL;


   //Fill with the max values
   for(UINT32 i = 0 ; i < EUnitDesignCharacteristics::ItemCount  ; i++)
   {
      l_iMaxResearch[i] = (UINT8)l_pInfo->m_fMaxValues[in_eCategory][i];
		if(l_iMaxResearch[i] == 0)
			return NULL;
   }


	switch(in_eCategory)
	{
   	case EUnitCategory::Naval:
      case EUnitCategory::Air:
		{
         	break;
		}
		case EUnitCategory::Ground:
		{
         l_iMaxResearch[EUnitDesignCharacteristics::Stealth]          = 0;
			l_iMaxResearch[EUnitDesignCharacteristics::CounterMesures]   = 0;
		}
		break;
	   case EUnitCategory::Nuclear:
		{

         l_iMaxResearch[EUnitDesignCharacteristics::GunRange]           = 0;
			l_iMaxResearch[EUnitDesignCharacteristics::GunPrecision]       = 0;
			l_iMaxResearch[EUnitDesignCharacteristics::GunDamage]          = 0;
         l_iMaxResearch[EUnitDesignCharacteristics::MissilePayload]     = 0;
			l_iMaxResearch[EUnitDesignCharacteristics::Stealth]            = 0;
			l_iMaxResearch[EUnitDesignCharacteristics::CounterMesures]     = 0;
			l_iMaxResearch[EUnitDesignCharacteristics::Armor]              = 0;	
			l_iMaxResearch[EUnitDesignCharacteristics::Speed]					= 0;
			l_iMaxResearch[EUnitDesignCharacteristics::Sensors]            = 0;	
		}	
		break;
	default:
		return NULL;
	}

	GString l_sUnitName = g_ServerDAL.DesignName(in_iCountryID,in_eType);
	UINT32 l_iDesignID = g_Joshua.UnitManager().NewUnitDesignID();
	g_ServerDCL.CreateUnitDesign(l_iDesignID,
											in_iCountryID,
											g_ServerDAL.UnitTypes()[in_eType - 1],
											l_sUnitName,
											-1,
                                 l_iMaxResearch[EUnitDesignCharacteristics::Speed],
                                 l_iMaxResearch[EUnitDesignCharacteristics::Sensors],
											l_iMaxResearch[EUnitDesignCharacteristics::GunRange],
											l_iMaxResearch[EUnitDesignCharacteristics::GunPrecision],
                                 l_iMaxResearch[EUnitDesignCharacteristics::GunDamage],
											l_iMaxResearch[EUnitDesignCharacteristics::MissilePayload],
											l_iMaxResearch[EUnitDesignCharacteristics::MissileRange],
											l_iMaxResearch[EUnitDesignCharacteristics::MissilePrecision],
											l_iMaxResearch[EUnitDesignCharacteristics::MissileDamage],
											l_iMaxResearch[EUnitDesignCharacteristics::Stealth],
											l_iMaxResearch[EUnitDesignCharacteristics::CounterMesures],
											l_iMaxResearch[EUnitDesignCharacteristics::Armor]
											);
	SP2::GUnitDesign* l_pNewDesign = (SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign(l_iDesignID);
	gassert(l_pNewDesign, "GAI::CreateNewOptimalDesign(): New design creation didn't really work");
	return l_pNewDesign;
}

void GAI::DeployUnits(UINT32 in_iCountryDeployingUnits)
{
	const UINT32 c_iMaxUnitGroups = 10;
	const UINT32 c_iMaxUnitGroupsNaval = 5;

	UINT32 l_iNbUnitGroupsGround = 0;
	UINT32 l_iNbUnitGroupsNaval = 0;	
	
	SP2::GUnitGroupEx* l_pGroup = g_ServerDCL.UnitMover().ProductionQueueUnitGroups()[in_iCountryDeployingUnits-1];
   if(!l_pGroup)
      return;

	const vector<GRegionControl>& l_vControl = g_ServerDAL.RegionControlArray();
	const set<UINT32>& l_UnitGroups = g_Joshua.UnitManager().CountryUnitGroups(in_iCountryDeployingUnits);

	for(set<UINT32>::const_iterator it = l_UnitGroups.begin(); 
		 it != l_UnitGroups.end(); it++)
	{
		SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*it);
		if(l_pUnitGroup->Id() == l_pGroup->Id())
			continue;
		if(l_pUnitGroup->IsNaval())
			l_iNbUnitGroupsNaval++;
		else
			l_iNbUnitGroupsGround++;
	}		
	
	const vector<SDK::Combat::GUnit*>& l_vUnits = l_pGroup->Units();

	//Deploy Units
	if(l_vUnits.size() > 0)
	{
		hash_map<UINT32,Hector::GVector2D<REAL32> > l_UnitGroupDestination;
		
		vector<Hector::GVector2D<REAL32> > l_vWaypoints;
		g_ServerDCL.FindNavalWaypointsAdjacentToCountry(in_iCountryDeployingUnits,l_vWaypoints);	

		const set<UINT32>& l_Military = g_ServerDAL.CountryMilitaryControl(in_iCountryDeployingUnits);

		for(vector<SDK::Combat::GUnit*>::const_iterator it = l_vUnits.begin();
			it != l_vUnits.end(); it++)
		{		
			SP2::GUnit* l_pUnit = (SP2::GUnit*)(*it);
			if(g_ServerDCL.UnitMover().IsUnitInTraining(l_pUnit->Id()))
				continue;
			if(g_ServerDCL.IsUnitForSale(l_pUnit->Id()))
				continue;
			
			EUnitType::Enum l_eType = (EUnitType::Enum)((SP2::GUnitDesign*)l_pUnit->Design())->Type()->Id();

			if(l_pUnit->IsNaval())
			{
				if(l_vWaypoints.size() > 0)
				{
					bool l_bCreateNewUnitGroup = false;
					if(l_iNbUnitGroupsNaval >= c_iMaxUnitGroupsNaval)
					{
						UINT32 l_iUnitGroupID = UINT_MAX;
						UINT32 l_iMinType = UINT_MAX;
						//Deploy unit into an existing unit group
						for(set<UINT32>::const_iterator it = l_UnitGroups.begin(); 
							it != l_UnitGroups.end(); it++)
						{					
							//Find the unit group with the lowest amount of that type of unit
							SP2::GUnitGroup * l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*it);		
							gassert(l_pUnitGroup, "GDataAccessLayerServer::TotalUnitType(): unit group invalid");
							if (l_pUnitGroup && l_pGroup->Id() != l_pUnitGroup->Id() && l_pUnitGroup->IsNaval())
							{						
								UINT32 l_iTotal = 0;
								vector<SDK::Combat::GUnit*>::const_iterator l_UnitsIt = l_pUnitGroup->Units().begin();
								while(l_UnitsIt != l_pUnitGroup->Units().end() && l_iMinType != 0)
								{
									SP2::GUnit*       l_pUnit   = (SP2::GUnit*)*l_UnitsIt;
									SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)l_pUnit->Design();
									
									//Check if of the correct type
									if((EUnitType::Enum)l_pDesign->Type()->Id() == l_eType)
									{
										l_iTotal += l_pUnit->Qty();
									}
									l_UnitsIt++;								
								}
								if(l_iTotal < l_iMinType)
								{
									l_iUnitGroupID = l_pUnitGroup->Id();
									l_iMinType = l_iTotal;
								}
							}											
						}	
						if(l_iUnitGroupID == UINT_MAX)
						{
							//No possible units have been found, let's create a unit group
							l_bCreateNewUnitGroup = true;
						}
						else
						{
							//Add units to that unit group id l_iUnitGroupID
							SDK::Combat::GUnitGroup* l_pUnitGroup = g_Joshua.UnitManager().UnitGroup(l_iUnitGroupID);
							l_UnitGroupDestination[l_pUnit->Id()] = Hector::GVector2D<REAL32>
																				(l_pUnitGroup->Position().x,
																				l_pUnitGroup->Position().y);
						}
					}
					else
					{				
						l_bCreateNewUnitGroup = true;				
					}
					if(l_bCreateNewUnitGroup)
					{								
						Random::GQuick l_Rand;
						l_Rand.Seed( l_pUnit->Id() * in_iCountryDeployingUnits );
						UINT32 l_iPosition = (UINT32)floor((l_Rand.RandomReal() * l_vWaypoints.size()));
						l_UnitGroupDestination[l_pUnit->Id()] = l_vWaypoints[l_iPosition];
						l_iNbUnitGroupsNaval++;
					}
				}
			}
			else
			{			
				bool l_bCreateNewUnitGroup = false;
				if(l_iNbUnitGroupsGround >= c_iMaxUnitGroups)
				{
					UINT32 l_iUnitGroupID = UINT_MAX;
					UINT32 l_iMinType = UINT_MAX;
					//Deploy unit into an existing unit group
					for(set<UINT32>::const_iterator it = l_UnitGroups.begin(); 
						it != l_UnitGroups.end(); it++)
					{					
						//Find the unit group with the lowest amount of that type of unit
						SP2::GUnitGroup * l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*it);		
						gassert(l_pUnitGroup, "GDataAccessLayerServer::TotalUnitType(): unit group invalid");
						if (l_pUnitGroup)
						{
							if( l_vControl[((SP2::GUnitGroupEx*)l_pUnitGroup)->m_iRegionID].m_iMilitary == in_iCountryDeployingUnits &&
								 l_vControl[((SP2::GUnitGroupEx*)l_pUnitGroup)->m_iRegionID].m_iPolitical == in_iCountryDeployingUnits)
							{
								UINT32 l_iTotal = 0;
								vector<SDK::Combat::GUnit*>::const_iterator l_UnitsIt = l_pUnitGroup->Units().begin();
								while(l_UnitsIt != l_pUnitGroup->Units().end() && l_iMinType != 0)
								{
									SP2::GUnit*       l_pUnit   = (SP2::GUnit*)*l_UnitsIt;
									SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)l_pUnit->Design();
									
									//Check if of the correct type
									if((EUnitType::Enum)l_pDesign->Type()->Id() == l_eType)
									{
										l_iTotal += l_pUnit->Qty();
									}
									l_UnitsIt++;								
								}
								if(l_iTotal < l_iMinType)
								{
									l_iUnitGroupID = l_pUnitGroup->Id();
									l_iMinType = l_iTotal;
								}
							}											
						}	
					}	
					if(l_iUnitGroupID == UINT_MAX)
					{
						//No possible units have been found, let's create a unit group
						l_bCreateNewUnitGroup = true;
					}
					else
					{
						//Add units to that unit group id l_iUnitGroupID
						SDK::Combat::GUnitGroup* l_pUnitGroup = g_Joshua.UnitManager().UnitGroup(l_iUnitGroupID);
						l_UnitGroupDestination[l_pUnit->Id()] = Hector::GVector2D<REAL32>
																			(l_pUnitGroup->Position().x,
																			l_pUnitGroup->Position().y);
					}
				}
				else
				{				
					l_bCreateNewUnitGroup = true;
					
				}
				if(l_bCreateNewUnitGroup)
				{
					if(l_Military.size())
					{
						UINT32 l_iRegionID = *(l_Military.begin());
						Random::GQuick l_Rand;
						l_Rand.Seed( l_pUnit->Id() * in_iCountryDeployingUnits );
						UINT32 l_iPosition = (UINT32)floor((l_Rand.RandomReal() * l_Military.size()));
						set<UINT32>::const_iterator l_Itr = l_Military.begin();
						for(UINT32 i=0; i<l_Military.size(); i++)
						{
							if(i == l_iPosition)
							{
								l_iRegionID = *l_Itr;
								break;
							}
							l_Itr++;
						}
						l_UnitGroupDestination[l_pUnit->Id()] = Hector::GVector2D<REAL32>(g_ServerDCL.UnitMover().RegionLocation( l_iRegionID ));																			

					}
					l_iNbUnitGroupsGround++;
				}

			}		
		}	
		
		for(hash_map<UINT32,Hector::GVector2D<REAL32> >::iterator it = l_UnitGroupDestination.begin();
			it != l_UnitGroupDestination.end(); it++)
		{
			list<UINT32> l_UnitToDeploy;
			l_UnitToDeploy.push_back(it->first);
			g_ServerDCL.DeployUnits(in_iCountryDeployingUnits,l_UnitToDeploy,it->second);	
		}
		
	}

	return;
}

void GAI::DisbandUnits(UINT32 in_iCountryDisbandingUnits,SP2::EUnitCategory::Enum in_eCategory)
{
	INT32 l_iRankInfantry = 0;
	INT32 l_iRankGround = 0;
	INT32 l_iRankAir = 0;
	INT32 l_iRankNaval = 0;
	
	EUnitCategory::Enum l_CategoryToRemove = in_eCategory;

	if(l_CategoryToRemove == SP2::EUnitCategory::ItemCount)
	{

		g_ServerDAL.FindUnitTypeRanks(in_iCountryDisbandingUnits,l_iRankInfantry,l_iRankGround,l_iRankAir,l_iRankNaval);

		if(l_iRankNaval <= l_iRankGround && l_iRankNaval <= l_iRankAir && l_iRankNaval <= l_iRankInfantry)
			l_CategoryToRemove = EUnitCategory::Naval;
		else if(l_iRankAir <= l_iRankGround && l_iRankAir <= l_iRankNaval && l_iRankAir <= l_iRankInfantry)
			l_CategoryToRemove = EUnitCategory::Air;
		else if(l_iRankGround <= l_iRankAir && l_iRankGround <= l_iRankNaval && l_iRankGround <= l_iRankInfantry)
			l_CategoryToRemove = EUnitCategory::Ground;
		else
			l_CategoryToRemove = EUnitCategory::Infantry;

	}

	multimap<REAL32,UINT32> l_UnitsPossibleToSale;
	//Make a list of units that may be sold
	const set<UINT32>& l_vUnitGroupsID = g_Joshua.UnitManager().CountryUnitGroups(in_iCountryDisbandingUnits);
	for(set<UINT32>::const_iterator l_It = l_vUnitGroupsID.begin();
			l_It != l_vUnitGroupsID.end();
			l_It++)
	{
		SDK::Combat::GUnitGroup* l_pUnitGroup = g_Joshua.UnitManager().UnitGroup((*l_It));
		if(((SP2::GUnitGroup*)l_pUnitGroup)->Status() == EMilitaryStatus::InDeployement ||
			((SP2::GUnitGroup*)l_pUnitGroup)->Status() == EMilitaryStatus::Attacking)
			continue;
		for(vector<SDK::Combat::GUnit*>::const_iterator l_It2 = l_pUnitGroup->Units().begin();
				l_It2 != l_pUnitGroup->Units().end();
				l_It2++)
		{
			SDK::Combat::GUnit* l_pUnitThatMayBeDisbanded = *l_It2;
			if( ((SP2::GUnitDesign*)l_pUnitThatMayBeDisbanded->Design())->Type()->Category() != l_CategoryToRemove ||
				 g_ServerDCL.IsUnitForSale( l_pUnitThatMayBeDisbanded->Id() ) ||
				 g_ServerDCL.UnitMover().IsUnitInTraining( l_pUnitThatMayBeDisbanded->Id() ))
				 continue;
			l_UnitsPossibleToSale.insert( make_pair<REAL32,UINT32>(((SP2::GUnitDesign*)l_pUnitThatMayBeDisbanded->Design())->Cost(),l_pUnitThatMayBeDisbanded->Id())); 
		}
	}	

	if(l_UnitsPossibleToSale.size() > 0)
	{
		SDK::Combat::GUnit* l_pUnit = g_Joshua.UnitManager().Unit(l_UnitsPossibleToSale.begin()->second);
		if(l_CategoryToRemove == EUnitCategory::Infantry)
			g_ServerDCL.DisbandUnits(l_pUnit->Group()->OwnerId(),l_pUnit);
		else
			g_ServerDCL.SellUnit(l_pUnit->Id());
	}	
}

UINT32 GAI::BuildOrBuyUnits(UINT32 in_iCountry, REAL32 in_fStrength, EUnitCategory::Enum in_Category)
{
	const UINT32 c_iNbBoatsByStrength = 1;
	const UINT32 c_iNbPlanesByStrength = 5;
	const UINT32 c_iNbGroundsByStrength = 10;

    // Have countries with higher revenue, build or buy more
    const GCountryData* const l_pCountryData = g_ServerDAL.CountryData(in_iCountry);
    in_fStrength *= static_cast<REAL32>(log(max(1.0, l_pCountryData->BudgetRevenues())) + 1.0);
    GDZLOG(EDZLogLevel::Info2, l_pCountryData->NameAndIDForLog() + L"wants to build or buy " +
           ((EUnitCategory::Infantry <= in_Category && in_Category <= EUnitCategory::Nuclear) ?
           g_ServerDAL.GetString(c_iUnitCategoryStringID[in_Category]) :
           (L"category " + GString(in_Category))) + L" units " +
           L"with strength " + GString(in_fStrength));

	if(in_Category == EUnitCategory::Air)
		return BuildAirUnits(in_iCountry, max(1,(UINT32)(in_fStrength*c_iNbPlanesByStrength)));
	else if(in_Category == EUnitCategory::Naval)
		return BuildNavalUnits(in_iCountry, max(1,(UINT32)(in_fStrength*c_iNbBoatsByStrength)));	
	else if(in_Category == EUnitCategory::Ground)
		return BuildGroundUnits(in_iCountry, max(1,(UINT32)(in_fStrength*c_iNbGroundsByStrength)));
	else
	{
		vector<Hector::GVector2D<REAL32> > l_vWaypoints;
		g_ServerDCL.FindNavalWaypointsAdjacentToCountry(in_iCountry,l_vWaypoints);

		INT32 l_iRankInfantry = 0;
		INT32 l_iRankGround = 0;
		INT32 l_iRankAir = 0;
		INT32 l_iRankNaval = 0;
		
		g_ServerDAL.FindUnitTypeRanks(in_iCountry,l_iRankInfantry,l_iRankGround,l_iRankAir,l_iRankNaval);

		if(l_vWaypoints.size() == 0)
			l_iRankNaval = 0;
		
		if(l_iRankGround >= l_iRankAir && l_iRankGround >= l_iRankNaval)
			return BuildGroundUnits(in_iCountry, max(1,(UINT32)(in_fStrength*c_iNbGroundsByStrength)));
		else if(l_iRankAir >= l_iRankGround && l_iRankAir >= l_iRankNaval)
			return BuildAirUnits(in_iCountry, max(1,(UINT32)(in_fStrength*c_iNbPlanesByStrength)));
		else if(l_iRankNaval >= l_iRankGround && l_iRankNaval >= l_iRankAir)
			return BuildNavalUnits(in_iCountry, max(1,(UINT32)(in_fStrength*c_iNbBoatsByStrength)));	
			
		return UINT_MAX;
	}

	
}

REAL32 GAI::MaximumCostByUnit(UINT32 in_iCountry, UINT32 in_iDesired)
{
	REAL64 l_fYearlyProduction = g_ServerDCL.CountryYearlyUnitProduction(in_iCountry);
	if(g_ServerDAL.IsAtWar(in_iCountry))
		return (REAL32)(l_fYearlyProduction * SP2::c_fLimitYearsTimeOfWar / (REAL64)in_iDesired);
	else
		return (REAL32)(l_fYearlyProduction * SP2::c_fLimitYears / (REAL64)in_iDesired);
}

void GAI::FindPossibleDesign(UINT32 in_iCountry, EUnitType::Enum in_eTypeToBuild, multimap<REAL32,SP2::GUnitDesign*>& out_mPossibleDesigns)
{
	//Find the list of possible designs
	const hash_map<UINT32, SDK::Combat::Design::GUnit*>& l_Designs = g_Joshua.UnitManager().UnitDesigns();
	for(hash_map<UINT32, SDK::Combat::Design::GUnit*>::const_iterator it = l_Designs.begin();
		 it != l_Designs.end(); it++)
	{
		SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)it->second;
		if((EUnitType::Enum)l_pDesign->Type()->Id() == in_eTypeToBuild &&
			(l_pDesign->DesignerID() == 0 || l_pDesign->DesignerID() == in_iCountry))
		{
			out_mPossibleDesigns.insert(make_pair(-l_pDesign->Cost(),l_pDesign));
		}		
	}
	
}

UINT32 GAI::ExecuteBuildUnits(UINT32                                    in_iCountry,
										UINT32                                    in_iDesired,
										EUnitCategory::Enum                       in_eCategory, 
										EUnitType::Enum                           in_eUnitType,
										REAL32                                    in_fMaximumCostByUnit,
										UINT8*                                    in_Research,
										const multimap<REAL32,SP2::GUnitDesign*>& in_mPossibleDesigns)
{
    const GCountryData* const l_pCountryData = g_ServerDAL.CountryData(in_iCountry);
	bool l_bBuySuccesful = false;

	if(in_mPossibleDesigns.size() > 0)
	{
		//Does a better design exist on the market? If so, buy it.
        GDZLOG(EDZLogLevel::Info2, l_pCountryData->NameAndIDForLog() + L"will try to buy its " +
               GString(in_iDesired) + L" " +
               g_ServerDAL.GetString(c_iUnitCategoryStringID[in_eCategory]) + L" units");
		l_bBuySuccesful = BuyUnits(in_iCountry,in_eUnitType,in_iDesired,-(in_mPossibleDesigns.begin()->first));		
	}
	else
	{			
		//We don't have any units like this yet			
      REAL32 l_fUnitCost = g_ServerDCL.CalculateUnitCost(in_eUnitType,
                                                          in_Research[EUnitDesignCharacteristics::Speed],
                                                          in_Research[EUnitDesignCharacteristics::Sensors],
                                                          in_Research[EUnitDesignCharacteristics::GunRange],
                                                          in_Research[EUnitDesignCharacteristics::GunPrecision],
                                                          in_Research[EUnitDesignCharacteristics::GunDamage],
                                                          in_Research[EUnitDesignCharacteristics::MissilePayload],
                                                          in_Research[EUnitDesignCharacteristics::MissileRange],
                                                          in_Research[EUnitDesignCharacteristics::MissilePrecision],
                                                          in_Research[EUnitDesignCharacteristics::MissileDamage],
                                                          in_Research[EUnitDesignCharacteristics::Stealth],
                                                          in_Research[EUnitDesignCharacteristics::CounterMesures],
                                                          in_Research[EUnitDesignCharacteristics::Armor]);
		l_bBuySuccesful = BuyUnits(in_iCountry,in_eUnitType,in_iDesired,l_fUnitCost);
	}
	if(!l_bBuySuccesful)
	{
		//If we couldn't find any design to buy, we'll build some for that country
		if(in_mPossibleDesigns.size() > 0)
		{
			//Build the design that we can afford to build	
			for(multimap<REAL32,SP2::GUnitDesign*>::const_iterator it = in_mPossibleDesigns.begin();
					it != in_mPossibleDesigns.end(); it++)
			{
				SP2::GUnitDesign* l_pUnitDesign = it->second;
				if((-it->first) <= in_fMaximumCostByUnit)
				{
					//Build that unit
					if(it == in_mPossibleDesigns.begin())
					{
						//Check if the country could create a better design, with its technical advances
                  if(in_Research[EUnitDesignCharacteristics::Speed]           > l_pUnitDesign->SpeedAndManeuverability() ||
                     in_Research[EUnitDesignCharacteristics::Sensors]         > l_pUnitDesign->Sensors() ||
                     in_Research[EUnitDesignCharacteristics::GunRange]        > l_pUnitDesign->GunRange() ||
                     in_Research[EUnitDesignCharacteristics::GunPrecision]    > l_pUnitDesign->GunPrecision() ||
							in_Research[EUnitDesignCharacteristics::GunDamage]       > l_pUnitDesign->GunDamage() ||
                     in_Research[EUnitDesignCharacteristics::MissilePayload]  > l_pUnitDesign->MissilePayload() ||
							in_Research[EUnitDesignCharacteristics::MissileRange]    > l_pUnitDesign->MissileRange() ||
                     in_Research[EUnitDesignCharacteristics::MissilePrecision]> l_pUnitDesign->MissilePrecision() ||
							in_Research[EUnitDesignCharacteristics::MissileDamage]   > l_pUnitDesign->MissileDamage() ||
							in_Research[EUnitDesignCharacteristics::Stealth]         > l_pUnitDesign->Stealth() ||
							in_Research[EUnitDesignCharacteristics::CounterMesures]  > l_pUnitDesign->CounterMesures() ||
							in_Research[EUnitDesignCharacteristics::Armor]           > l_pUnitDesign->Armor())
						{
							SP2::GUnitDesign* l_pNewDesign = GAI::CreateNewOptimalDesign(in_iCountry,in_eCategory,in_eUnitType);
							if(l_pNewDesign != NULL)
							{
                                GDZLOG(EDZLogLevel::Info2, l_pCountryData->NameAndIDForLog() +
                                       L"has created a new design, " +
                                       l_pNewDesign->Name() + L", " +
                                       L"for its " + GString(in_iDesired) + L" " +
                                       g_ServerDAL.GetString(c_iUnitCategoryStringID[in_eCategory]) +
                                       L" units");

                                if(l_pNewDesign->Cost() <= in_fMaximumCostByUnit)
                                {                        
								    //Build that unit
                                    GDZLOG(EDZLogLevel::Info2, l_pCountryData->NameAndIDForLog() +
                                           L"will build its new design " + l_pNewDesign->Name());
								    return g_ServerDCL.BuildUnits(in_iCountry,in_iCountry,l_pNewDesign->Id(),in_iDesired,0,EUnitProductionPriority::Normal);
                                }
							}		
						}

					}
                    GDZLOG(EDZLogLevel::Info2, l_pCountryData->NameAndIDForLog() +
                           L"will build its existing design " + l_pUnitDesign->Name());
					return g_ServerDCL.BuildUnits(in_iCountry,in_iCountry,l_pUnitDesign->Id(),in_iDesired,0,EUnitProductionPriority::Normal);
				}
			}
		}
		else
		{
			//The AI has no design for that type. It'll build the best design it can afford, then build units from that design
			SP2::GUnitDesign* l_pNewDesign = GAI::CreateNewOptimalDesign(in_iCountry,in_eCategory,in_eUnitType);
			if(l_pNewDesign && l_pNewDesign->Cost() <= in_fMaximumCostByUnit)
			{
				//Build that unit
                GDZLOG(EDZLogLevel::Info2, l_pCountryData->NameAndIDForLog() +
                       L"will build its new " +
                       g_ServerDAL.GetString(c_iUnitTypeStringID[in_eUnitType]) +
                       L" design " + l_pNewDesign->Name());
				return g_ServerDCL.BuildUnits(in_iCountry,in_iCountry,l_pNewDesign->Id(),in_iDesired,0,EUnitProductionPriority::Normal);
			}			
		}
	}
	else
	{
		//We did buy unit. Go home!
		return UINT_MAX;
	}
    GDZLOG(EDZLogLevel::Info2, l_pCountryData->NameAndIDForLog() + L" did not build or buy " +
           GString(in_iDesired) + L" " +
           g_ServerDAL.GetString(c_iUnitCategoryStringID[in_eCategory]) +
           L" units");
	return UINT_MAX;
}

UINT32 GAI::BuildAirUnits(UINT32 in_iCountry, UINT32 in_iDesired)
{
   SP2::GResearchInformation* l_pInfo = (SP2::GResearchInformation*)g_ServerDAL.CountryData(in_iCountry)->ResearchInfo();
   UINT8      l_Research[EUnitDesignCharacteristics::ItemCount];
   for(UINT32 i = 0; i < EUnitDesignCharacteristics::ItemCount ; i++)
   {
      l_Research[i] = (UINT8)floor(l_pInfo->m_fMaxValues[EUnitCategory::Air][i]);
   }

	REAL32 l_fMaximumCostByUnit = MaximumCostByUnit(in_iCountry,in_iDesired);	

	UINT32 l_iNbTransportHelicopter = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::TransportHelicopter);
	UINT32 l_iNbFighterAircraft = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::FighterAircraft);
	UINT32 l_iNbAttackHelicopter = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::AttackHelicopter);
	UINT32 l_iNbAttackAircraft = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::AttackAircraft);	
	UINT32 l_iNbBomber = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::Bomber);

	UINT32 l_iNbPlanes = l_iNbTransportHelicopter + l_iNbFighterAircraft + l_iNbAttackHelicopter
		+ l_iNbAttackAircraft + l_iNbBomber;	
	
	EUnitType::Enum l_eTypeToBuild;

	if( (l_iNbPlanes == 0) || ((REAL32)l_iNbTransportHelicopter / (REAL32)l_iNbPlanes < SP2::c_fTransportHelicopterRatio) )
	{
		//Build Transport Helicopters	
		l_eTypeToBuild = EUnitType::TransportHelicopter;
	}	
	else if((REAL32)l_iNbFighterAircraft / (REAL32)l_iNbPlanes < SP2::c_fFighterAircraftRatio)
	{
		//Build Fighter Aircrafts
		l_eTypeToBuild = EUnitType::FighterAircraft;
	}	
	else if((REAL32)l_iNbAttackHelicopter / (REAL32)l_iNbPlanes < SP2::c_fAttackHelicopterRatio)
	{
		//Build Attack Helicopters
		l_eTypeToBuild = EUnitType::AttackHelicopter;
	}	
	else if((REAL32)l_iNbAttackAircraft / (REAL32)l_iNbPlanes < SP2::c_fAttackAircraftRatio)
	{
		//Build Attack Aircrafts
		l_eTypeToBuild = EUnitType::AttackAircraft;
	}	
	else
	{
		//Build Bomber
		l_eTypeToBuild = EUnitType::Bomber;
	}

	multimap<REAL32,SP2::GUnitDesign*> l_mPossibleDesigns;	
	FindPossibleDesign(in_iCountry,l_eTypeToBuild,l_mPossibleDesigns);
	
	return ExecuteBuildUnits(in_iCountry,in_iDesired,EUnitCategory::Air,l_eTypeToBuild,l_fMaximumCostByUnit,l_Research,l_mPossibleDesigns);	
}

UINT32 GAI::BuildGroundUnits(UINT32 in_iCountry, UINT32 in_iDesired)
{
	SP2::GResearchInformation* l_pInfo = (SP2::GResearchInformation*)g_ServerDAL.CountryData(in_iCountry)->ResearchInfo();
   UINT8      l_Research[EUnitDesignCharacteristics::ItemCount];
   for(UINT32 i = 0; i < EUnitDesignCharacteristics::ItemCount ; i++)
   {
      l_Research[i] = (UINT8)floor(l_pInfo->m_fMaxValues[EUnitCategory::Ground][i]);
   }
   l_Research[EUnitDesignCharacteristics::Stealth]        = 0;
   l_Research[EUnitDesignCharacteristics::CounterMesures] = 0;

	REAL32 l_fMaximumCostByUnit = MaximumCostByUnit(in_iCountry,in_iDesired);	

	UINT32 l_iNbInfantryVehicle = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::InfantryVehicle);
	UINT32 l_iNbArtilleryGun = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::ArtilleryGun);
	UINT32 l_iNbTank = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::Tank);
	UINT32 l_iNbAirDefense = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::AirDefense);
	UINT32 l_iNbMobileLauncher = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::MobileLauncher);

	UINT32 l_iNbGrounds = l_iNbInfantryVehicle + l_iNbArtilleryGun + l_iNbTank
		+ l_iNbAirDefense + l_iNbMobileLauncher;	
	
	EUnitType::Enum l_eTypeToBuild;

	if( (l_iNbGrounds == 0) || ((REAL32)l_iNbInfantryVehicle / (REAL32)l_iNbGrounds < SP2::c_fInfantryVehicleRatio) )
	{
		//Build Infantry Vehicles
		l_eTypeToBuild = EUnitType::InfantryVehicle;
	}	
	else if((REAL32)l_iNbArtilleryGun / (REAL32)l_iNbGrounds < SP2::c_fArtilleryGunRatio)
	{
		//Build Artillery Guns
		l_eTypeToBuild = EUnitType::ArtilleryGun;
	}	
	else if((REAL32)l_iNbTank / (REAL32)l_iNbGrounds < SP2::c_fTankRatio)
	{
		//Build Tanks
		l_eTypeToBuild = EUnitType::Tank;
	}	
	else if((REAL32)l_iNbAirDefense / (REAL32)l_iNbGrounds < SP2::c_fAirDefenseRatio)
	{
		//Build Air Defenses
		l_eTypeToBuild = EUnitType::AirDefense;
	}		
	else
	{
		//Build Mobile Launchers
		l_eTypeToBuild = EUnitType::MobileLauncher;
	}

	multimap<REAL32,SP2::GUnitDesign*> l_mPossibleDesigns;	
	FindPossibleDesign(in_iCountry,l_eTypeToBuild,l_mPossibleDesigns);

	return ExecuteBuildUnits(in_iCountry,in_iDesired,EUnitCategory::Ground,l_eTypeToBuild,l_fMaximumCostByUnit,l_Research,l_mPossibleDesigns);
}

UINT32 GAI::BuildNavalUnits(UINT32 in_iCountry, UINT32 in_iDesired)
{
	SP2::GResearchInformation* l_pInfo = (SP2::GResearchInformation*)g_ServerDAL.CountryData(in_iCountry)->ResearchInfo();
   UINT8      l_Research[EUnitDesignCharacteristics::ItemCount];
   for(UINT32 i = 0; i < EUnitDesignCharacteristics::ItemCount ; i++)
   {
      l_Research[i] = (UINT8)floor(l_pInfo->m_fMaxValues[EUnitCategory::Naval][i]); 
   }

	REAL32 l_fMaximumCostByUnit = MaximumCostByUnit(in_iCountry,in_iDesired);	

	UINT32 l_iNbAircraftCarrier = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::AircraftCarrier);
	UINT32 l_iNbAttackSubmarine = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::AttackSubmarine);
	UINT32 l_iNbBallisticMissileSubmarine = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::BallisticMissileSubmarine);
	UINT32 l_iNbDestroyer = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::Destroyer);
	UINT32 l_iNbFrigate = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::Frigate);
	UINT32 l_iNbPatrolCraft = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::PatrolCraft);
   UINT32 l_iNbASWHelicoptert = g_ServerDAL.TotalUnitType(in_iCountry,EUnitType::ASWHelicopter);

	UINT32 l_iNbBoats = l_iNbAircraftCarrier + l_iNbAttackSubmarine + l_iNbBallisticMissileSubmarine
		+ l_iNbDestroyer + l_iNbFrigate + l_iNbPatrolCraft + l_iNbASWHelicoptert;	
	
	EUnitType::Enum l_eTypeToBuild;

	if( (l_iNbBoats == 0) || ((REAL32)l_iNbPatrolCraft / (REAL32)l_iNbBoats < SP2::c_fPatrolCraftRatio) )
	{
		//Build patrol crafts	
		l_eTypeToBuild = EUnitType::PatrolCraft;
	}	
	else if((REAL32)l_iNbFrigate / (REAL32)l_iNbBoats < SP2::c_fFrigateRatio)
	{
		//Build frigates
		l_eTypeToBuild = EUnitType::Frigate;
	}	
	else if((REAL32)l_iNbDestroyer / (REAL32)l_iNbBoats < SP2::c_fDestroyerRatio)
	{
		//Build Destroyers
		l_eTypeToBuild = EUnitType::Destroyer;
	}	
	else if((REAL32)l_iNbAttackSubmarine / (REAL32)l_iNbBoats < SP2::c_fAttackSubmarineRatio)
	{
		//Build Attack Submarines
		l_eTypeToBuild = EUnitType::AttackSubmarine;
	}	
	else if((REAL32)l_iNbAircraftCarrier / (REAL32)l_iNbBoats < SP2::c_fAircraftCarriersRatio)
	{
		//Build Aircraft Carriers
		l_eTypeToBuild = EUnitType::AircraftCarrier;
	}
	else if((REAL32)l_iNbASWHelicoptert / (REAL32)l_iNbBoats < SP2::c_fASWHelicoptertRatio)
	{
		//Build ASW Helicopters
		l_eTypeToBuild = EUnitType::ASWHelicopter;
	}
	else
	{
		//Build Ballistic missiles submarines
		l_eTypeToBuild = EUnitType::BallisticMissileSubmarine;
	}

	multimap<REAL32,SP2::GUnitDesign*> l_mPossibleDesigns;	
	FindPossibleDesign(in_iCountry,l_eTypeToBuild,l_mPossibleDesigns);
	
	return ExecuteBuildUnits(in_iCountry,in_iDesired,EUnitCategory::Naval,l_eTypeToBuild,l_fMaximumCostByUnit,l_Research,l_mPossibleDesigns);	
}

bool GAI::BuyUnits(UINT32 in_iCountry, EUnitType::Enum in_eUnitType, UINT32 in_iDesired, REAL32 in_fStartingPrice)
{	
	const REAL64 c_fMaxLossConstant = 1.1f;		

	const hash_map<UINT32,SDK::Combat::Design::GUnit*>& l_Designs = g_Joshua.UnitManager().UnitDesigns();	

	const bool* l_pWeaponTradeEmbargos = g_ServerDAL.WeaponTradeEmbargos();
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	
	REAL64 l_fBonusAtWar = 0.f;
	if(g_ServerDAL.IsAtWar(in_iCountry))
		l_fBonusAtWar = 0.15f;
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountry);
	REAL32 l_fMaxAmountToSpend = (REAL32)(max(0.f,(((l_pCountryData->BudgetRevenues() * (c_fMaxLossConstant+l_fBonusAtWar)) - l_pCountryData->BudgetExpenses()) / 0.275f)) + max(0.f,l_pCountryData->MonetarySupplyAmount()));
		
	multimap<REAL32,SP2::GUnit*> l_PossibleUnitToBuy;
	const hash_set<UINT32>& l_ForSale = g_ServerDCL.ForSaleUnitList();
	for(list<UINT32>::const_iterator it = l_ForSale.begin();
		 it != l_ForSale.end(); it++)
	{
		SP2::GUnit* l_pUnit = (SP2::GUnit*)g_Joshua.UnitManager().Unit(*it);		
		gassert(l_pUnit,"GAI::BuyUnits(): Unit inside the sale list is invalid");
		//No trade embargos, good unit design, and price is good
		if(!l_pWeaponTradeEmbargos[l_pUnit->Group()->OwnerId() * (l_iNbCountry + 1) + in_iCountry ] &&
		  ((EUnitType::Enum)((SP2::GUnitDesign*)(l_pUnit->Design()))->Type()->Id()) == in_eUnitType &&
			 l_pUnit->Design()->Cost() > in_fStartingPrice)
		{
			l_PossibleUnitToBuy.insert(make_pair<REAL32,SP2::GUnit*>(-l_pUnit->Design()->Cost(),l_pUnit));
		}
	}
	
	if(l_PossibleUnitToBuy.size() > 0)
	{
		UINT32 l_iQtyLookingToBuy = 0;
		REAL32 l_fTotalAmountOfSale = 0.f;
		for(multimap<REAL32,SP2::GUnit*>::iterator it = l_PossibleUnitToBuy.begin();
			 it != l_PossibleUnitToBuy.end(); it++)
		{
			l_iQtyLookingToBuy = min(it->second->Qty(),in_iDesired);
			l_fTotalAmountOfSale = l_iQtyLookingToBuy * -it->first;
			if(l_fTotalAmountOfSale > l_fMaxAmountToSpend)
				continue;
			if(g_ServerDCL.BuyUnit(in_iCountry,it->second->Id(),l_iQtyLookingToBuy))
				return true;
		}
	}

	//Try to order new units from another country, since we haven't been able to buy some
	set<UINT32> l_PossibleCountries;
	g_ServerDCL.CountryCanOrderNewUnitsFrom(in_iCountry,l_PossibleCountries);
	
	//Check the list of possible designs	
	multimap<REAL32,SP2::GUnitDesign*> l_PossibleDesigns;
	for(hash_map<UINT32,SDK::Combat::Design::GUnit*>::const_iterator it = l_Designs.begin();
		 it != l_Designs.end(); it++)
	{
		SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)it->second;	
		if(((EUnitType::Enum)l_pDesign->Type()->Id()) != in_eUnitType)
			continue;
		if(l_PossibleCountries.find(l_pDesign->DesignerID()) != l_PossibleCountries.end())
		{
			l_PossibleDesigns.insert(make_pair<REAL32,SP2::GUnitDesign*>(-l_pDesign->Cost(),l_pDesign));
		}
	}
	REAL32 l_fTotalAmountOfSale = 0.f;
	//Buy the most expensive design that country can afford
	for(multimap<REAL32,SP2::GUnitDesign*>::iterator it = l_PossibleDesigns.begin();
			 it != l_PossibleDesigns.end(); it++)
	{
		SP2::GUnitDesign* l_pDesign = it->second;
		l_fTotalAmountOfSale = -(it->first) * in_iDesired;
		if(l_fTotalAmountOfSale > l_fMaxAmountToSpend)
			continue;

        GDZLOG(EDZLogLevel::Info2, l_pCountryData->NameAndIDForLog() +
               L"will buy its " + GString(in_iDesired) + L" " + l_pDesign->Name() +
               L" from country ID " + GString(l_pDesign->DesignerID()) + L", " +
               g_ServerDAL.CountryData(l_pDesign->DesignerID())->NameAndIDForLog());
		g_ServerDCL.BuildUnits(l_pDesign->DesignerID(),in_iCountry,l_pDesign->Id(),in_iDesired,0,EUnitProductionPriority::Normal);
		return true;		
	}

	return false;
}

void GAI::OrderBuildUnitsAgainstEnnemy(UINT32 in_iCountryBuilding, UINT32 in_iEnnemy)
{
	//First, deploy units that are for sale
	SP2::GUnitGroupEx* l_pGroup = g_ServerDCL.UnitMover().ProductionQueueUnitGroups()[in_iCountryBuilding-1];		
   if(!l_pGroup)
      return;

	const vector<SDK::Combat::GUnit*>& l_vUnits = l_pGroup->Units();
	for(UINT32 i=0; i<l_vUnits.size(); i++)
	{
		if(g_ServerDCL.IsUnitForSale(l_vUnits[i]->Id()))
			g_ServerDCL.SellUnit(l_vUnits[i]->Id());
	}	

	REAL32 l_fRatio = 0.f;
	REAL32 l_fValueAIAndAllies = g_ServerDAL.TotalUnitsValueWithAllies(in_iCountryBuilding);							
	if(l_fValueAIAndAllies > 0.f)
	{
		l_fRatio = min(5.f,g_ServerDAL.TotalUnitsValueWithAllies(in_iEnnemy) / l_fValueAIAndAllies);
	}
	else
	{
		l_fRatio = 5.f;
	}
	if(l_fRatio > 1.f)
	{
		//Buy units
		GAI::BuildOrBuyUnits(in_iCountryBuilding,l_fRatio);	
	}		
}

void GAI::CreateOrJoinTreaty(UINT32 in_iCountryID, ETreatyType::Enum in_TypeOfTreaty, UINT32 in_iSideToJoin)
{
	switch(in_TypeOfTreaty)
	{
	case ETreatyType::Annexation:
	case ETreatyType::War:
	case ETreatyType::RequestMilitaryPresenceRemoval:
	case ETreatyType::FreeRegion:
	case ETreatyType::EconomicEmbargo:
	case ETreatyType::MilitaryAccess:
		return;
	}

	//Check if an existing treaty of that type exists
	const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();
	
	for(hash_map<UINT32,GTreaty>::const_iterator it = l_Treaties.begin();
		 it != l_Treaties.end(); it++)
	{
		const GTreaty& l_CurTreaty = it->second;
		if(l_CurTreaty.Type() != in_TypeOfTreaty)
			continue;
		if(l_CurTreaty.Private() && l_CurTreaty.CountrySide(in_iCountryID) != in_iSideToJoin)
			continue;
		if(l_CurTreaty.IsSuspended(in_iCountryID))
			continue;
		if(!g_ServerDCL.CountryCanJoinTreaty(in_iCountryID,l_CurTreaty.ID(), true))
			continue;

		//If we are here, the country can join the treaty if it wants to
		if(CountryShouldJoinTreaty(in_iCountryID,l_CurTreaty.ID(),in_iSideToJoin) == ETreatyRefusal::Accepted)
		{
			g_ServerDCL.JoinTreaty(in_iCountryID,l_CurTreaty.ID(),in_iSideToJoin);
			return;
		}
	}

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);	
	const UINT8* l_pDiplomaticStatus = g_ServerDAL.CountryDiplomaticStatuses(in_iCountryID);
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	REAL32 l_fRelations = 0.f;
	set<UINT32> l_vCountriesLove;
	set<UINT32> l_vCountriesFriends;
	set<UINT32> l_vCountriesLike;
	set<UINT32> l_vCountriesGood;
	set<UINT32> l_vCountriesNeutral;
	set<UINT32> l_vCountriesHate;
	for(UINT32 i=1; i<= l_iNbCountry; i++)
	{
		if(i == in_iCountryID || !g_ServerDAL.CountryIsValid(i))
			continue;
		l_fRelations = g_ServerDAL.RelationBetweenCountries(i,in_iCountryID);
		if(l_fRelations > SP2::c_fRelationsLove)
			l_vCountriesLove.insert(i);
		if (l_fRelations > SP2::c_fRelationsFriend)		
			l_vCountriesFriends.insert(i);			
		if (l_fRelations > SP2::c_fRelationsLike)		
			l_vCountriesLike.insert(i);
		if (l_fRelations > SP2::c_fRelationsGood)
			l_vCountriesGood.insert(i);
		if (l_fRelations > SP2::c_fRelationsBad)
			l_vCountriesNeutral.insert(i);
		if (l_fRelations < SP2::c_fRelationsHate)
			l_vCountriesHate.insert(i);
	}


	//If we are here, we couldn't join any existing treaty, we'll have to create a new one
	GString l_sNewTreatyName = g_ServerDAL.TreatyName(in_TypeOfTreaty,g_ServerDAL.GetString(g_ServerDAL.CountryData(in_iCountryID)->NameID()));
	UINT32 l_Conditions[ETreatyConditions::ItemCount];
	Memory::Fill<UINT32>(l_Conditions,0,ETreatyConditions::ItemCount);	
	bool l_bPrivate = true;
	set<UINT32> l_SideAMembers;
	set<UINT32> l_SideBMembers;
	set<UINT32> l_SidePressure;

	switch(in_TypeOfTreaty)
	{

	case ETreatyType::Alliance:
		{
			UINT32 l_iNbNewAllies = 0;
			//Will create an alliance treaty
			for(set<UINT32>::const_iterator l_Itr = l_vCountriesFriends.begin();
				l_Itr != l_vCountriesFriends.end(); l_Itr++)
			{
				GCountryData* l_pCountryThatMayJoin = g_ServerDAL.CountryData(*l_Itr);
				if(l_pCountryThatMayJoin->Activated() && abs(l_pCountryThatMayJoin->MilitaryRank() - l_pCountryData->MilitaryRank()) <= 5)
				{
					if(l_pDiplomaticStatus[*l_Itr] != EDiplomaticStatus::Allied)
						l_iNbNewAllies++;
					l_SideAMembers.insert(*l_Itr);
				}
			}
			l_SideAMembers.insert(in_iCountryID);

			//Must not be at war
			l_Conditions[ETreatyConditions::RelationsNotAtWar] = EConditionStatusRelation::NotAtWar;

			if(l_SideAMembers.size() <= 1 || l_iNbNewAllies == 0)
				return;
		}
		break;	
	case ETreatyType::CulturalExchanges:
		{
			l_bPrivate = false;
			for(set<UINT32>::const_iterator l_Itr = l_vCountriesNeutral.begin();
				l_Itr != l_vCountriesNeutral.end(); l_Itr++)
			{
				GCountryData* l_pCountryThatMayJoin = g_ServerDAL.CountryData(*l_Itr);
				if(l_pCountryThatMayJoin->Activated() && abs(l_pCountryThatMayJoin->PoliticRank() - l_pCountryData->PoliticRank()) <= 10)
				{
					if(l_pCountryThatMayJoin->BudgetRevenues() > l_pCountryThatMayJoin->BudgetExpenses())
						l_SideAMembers.insert(*l_Itr);
				}
			}
			l_SideAMembers.insert(in_iCountryID);

			//Must not be at war
			l_Conditions[ETreatyConditions::RelationsNotAtWar] = EConditionStatusRelation::NotAtWar;
			if(l_SideAMembers.size() <= 1)
				return;
		}
		break;
	case ETreatyType::NobleCause:
		{
			l_bPrivate = false;
			for(set<UINT32>::const_iterator l_Itr = l_vCountriesGood.begin();
				l_Itr != l_vCountriesGood.end(); l_Itr++)
			{
				GCountryData* l_pCountryThatMayJoin = g_ServerDAL.CountryData(*l_Itr);
				if(l_pCountryThatMayJoin->Activated() && abs(l_pCountryThatMayJoin->PoliticRank() - l_pCountryData->PoliticRank()) <= 10)
				{
					if(l_pCountryThatMayJoin->BudgetRevenues() > l_pCountryThatMayJoin->BudgetExpenses())
						l_SideAMembers.insert(*l_Itr);
				}
			}
			l_SideAMembers.insert(in_iCountryID);
			//Must not be at war
			l_Conditions[ETreatyConditions::RelationsNotAtWar] = EConditionStatusRelation::NotAtWar;
			if(l_SideAMembers.size() <= 1)
				return;
		}
		break;
	case ETreatyType::ResearchPartnership:
		{
			FillConditions(in_iCountryID,l_Conditions,l_vCountriesGood.size());
			
			
			if(l_pCountryData->BudgetExpenseResearchRatio() >= SP2::c_fConditionsResearchHigh)
				l_Conditions[ETreatyConditions::ResearchLevelSimilitude] = EConditionStatusResearch::High;
			else if(l_pCountryData->BudgetExpenseResearchRatio() >= SP2::c_fConditionsResearchAverage)
				l_Conditions[ETreatyConditions::ResearchLevelSimilitude] = EConditionStatusResearch::Average;
			else if(l_pCountryData->BudgetExpenseResearchRatio() >= SP2::c_fConditionsResearchLow)
				l_Conditions[ETreatyConditions::ResearchLevelSimilitude] = EConditionStatusResearch::Low;
			else
				l_Conditions[ETreatyConditions::ResearchLevelSimilitude] = EConditionStatusResearch::NoLimit;

			if(g_ServerDCL.CountryCanJoinThoseConditions(in_iCountryID,l_Conditions,l_SideAMembers,false))
				l_SideAMembers.insert(in_iCountryID);
			
			l_SideAMembers.insert(l_vCountriesGood.begin(),l_vCountriesGood.end());
			
			for(set<UINT32>::const_iterator l_Itr = l_vCountriesGood.begin();
				l_Itr != l_vCountriesGood.end(); l_Itr++)
			{
				GCountryData* l_pCountryThatMayJoin = g_ServerDAL.CountryData(*l_Itr);
				if(g_ServerDCL.CountryCanJoinThoseConditions(*l_Itr,l_Conditions,l_SideAMembers, false) && 
					l_pCountryThatMayJoin->Activated() && 
					abs(l_pCountryThatMayJoin->EconomicRank() - l_pCountryData->EconomicRank()) <= 5)
					l_SideAMembers.insert(*l_Itr);
			}

			if(l_SideAMembers.size() <= 1)
				return;
		}
		break;
	case ETreatyType::HumanDevelopmentCollaboration:
		{
			//Any conditions in that treaty?	
			FillConditions(in_iCountryID,l_Conditions,l_vCountriesLike.size());

			//Will create a human development collaboration treaty
			if(g_ServerDCL.CountryCanJoinThoseConditions(in_iCountryID,l_Conditions,l_SideAMembers, false))
				l_SideAMembers.insert(in_iCountryID);

			for(set<UINT32>::const_iterator l_Itr = l_vCountriesLike.begin();
				l_Itr != l_vCountriesLike.end(); l_Itr++)
			{
				if(g_ServerDAL.CountryData(*l_Itr)->HumanDevelopment() >= l_pCountryData->HumanDevelopment() &&
					g_ServerDCL.CountryCanJoinThoseConditions(*l_Itr,l_Conditions,l_SideAMembers, false))
					l_SideAMembers.insert(*l_Itr);
			}			

			if(l_SideAMembers.size() <= 1)	
				return;	
		}
		break;
	case ETreatyType::EconomicPartnership:
		{
			l_SideAMembers.insert(in_iCountryID);
			for(set<UINT32>::const_iterator l_Itr = l_vCountriesLike.begin();
				l_Itr != l_vCountriesLike.end(); l_Itr++)
			{
				GCountryData* l_pCountryThatMayJoin = g_ServerDAL.CountryData(*l_Itr);
				if(l_pCountryThatMayJoin->Activated() && abs(l_pCountryThatMayJoin->EconomicRank() - l_pCountryData->EconomicRank()) <= 5)
				{
					l_SideAMembers.insert(*l_Itr);
				}
			}

			bool l_bPrivate = true;
			if(l_pCountryData->EconomicRank() > 100)
				l_bPrivate = false;								
			
			//Must not be at war
			l_Conditions[ETreatyConditions::RelationsNotAtWar] = EConditionStatusRelation::NotAtWar;

			if(l_SideAMembers.size() <= 1)					
					return;			
		}
		break;

	case ETreatyType::CommonMarket:
		{
			l_SideAMembers.insert(in_iCountryID);
			for(set<UINT32>::const_iterator l_Itr = l_vCountriesGood.begin();
				 l_Itr != l_vCountriesGood.end(); l_Itr++)
			{
				if(abs(g_ServerDAL.CountryData(*l_Itr)->EconomicRank() - l_pCountryData->EconomicRank()) <= 5)
					l_SideAMembers.insert(*l_Itr);				
			}			
			//Must not be at war
			l_Conditions[ETreatyConditions::RelationsNotAtWar] = EConditionStatusRelation::NotAtWar;
			if(l_SideAMembers.size() <= 1)					
				return;
		}
		break;
	case ETreatyType::EconomicAid:
		{
			if(in_iSideToJoin == 1)
			{
				l_SideAMembers.insert(in_iCountryID);
				//Find a good country to help, which is the country with the highest economic rank
				UINT32 l_iCountryID = 0;
				INT32 l_iHighestRank = 0;
				for(set<UINT32>::const_iterator l_Itr = l_vCountriesGood.begin();
					l_Itr != l_vCountriesGood.end(); l_Itr++)
				{
					if(g_ServerDAL.CountryData(*l_Itr)->EconomicRank() > l_iHighestRank)
					{
						l_iCountryID = *l_Itr;
						l_iHighestRank = g_ServerDAL.CountryData(*l_Itr)->EconomicRank();
					}
				}
				//Must not be at war
				l_Conditions[ETreatyConditions::RelationsNotAtWar] = EConditionStatusRelation::NotAtWar;

				if(l_iCountryID)
					l_SideBMembers.insert(l_iCountryID);
				else
					return;
			}
			else
			{
				if (l_vCountriesFriends.size() > 0)
				{
					//Will create an Economic Aid treaty
					l_SideBMembers.insert(in_iCountryID);
					for(set<UINT32>::const_iterator l_Itr = l_vCountriesFriends.begin();
						l_Itr != l_vCountriesFriends.end(); l_Itr++)
					{
						if(g_ServerDAL.CountryData(*l_Itr)->BudgetRevenues() > g_ServerDAL.CountryData(*l_Itr)->BudgetExpenses())
							l_SideAMembers.insert(*l_Itr);
					}
					if(l_SideAMembers.size() == 0)
						return;
					//Must not be at war
					l_Conditions[ETreatyConditions::RelationsNotAtWar] = EConditionStatusRelation::NotAtWar;
				}
				else
					return;
			}
		}
		break;
	case ETreatyType::AssumeForeignDebt:
		{
			if(in_iSideToJoin == 1)
			{
				l_SideAMembers.insert(in_iCountryID);
				//Find a good country to help, which is the country with the lowest debt
				UINT32 l_iCountryID = 0;
				REAL64 l_fLowestDebt = FLT_MAX;
				for(set<UINT32>::const_iterator l_Itr = l_vCountriesGood.begin();
					l_Itr != l_vCountriesGood.end(); l_Itr++)
				{
					if(g_ServerDAL.CountryData(*l_Itr)->MonetarySupplyAmount() < l_fLowestDebt && 
						g_ServerDAL.CountryData(*l_Itr)->MonetarySupplyAmount() < 0.f)
					{
						l_iCountryID = *l_Itr;
						l_fLowestDebt = g_ServerDAL.CountryData(*l_Itr)->MonetarySupplyAmount();
					}
				}
				if(l_iCountryID)
					l_SideBMembers.insert(l_iCountryID);
				else
					return;

			}
			else
			{
				l_SideBMembers.insert(in_iCountryID);
				for(set<UINT32>::const_iterator l_Itr = l_vCountriesGood.begin();
					l_Itr != l_vCountriesGood.end(); l_Itr++)
				{
					GCountryData* l_pPossibility = g_ServerDAL.CountryData(*l_Itr);
					if(l_pPossibility->EconomicRank() >= l_pCountryData->EconomicRank() &&
						l_pPossibility->MonetarySupplyAmount() >= abs(l_pCountryData->MonetarySupplyAmount()) &&
						l_pPossibility->BudgetRevenues() > l_pPossibility->BudgetExpenses())
						l_SideAMembers.insert(*l_Itr);			
				}
				if(l_SideAMembers.size() <= 0)
					return;
			}
		}
		break;
	case ETreatyType::WeaponTrade:
		{
			l_SideAMembers.insert(in_iCountryID);
			if (l_vCountriesFriends.size() > 0)
			{
				for(set<UINT32>::const_iterator l_Itr = l_vCountriesFriends.begin();
					l_Itr != l_vCountriesFriends.end(); l_Itr++)
				{
					GCountryData* l_pCountryThatMayJoin = g_ServerDAL.CountryData(*l_Itr);
					if(l_pCountryThatMayJoin->Activated() && abs(l_pCountryThatMayJoin->MilitaryRank() - l_pCountryData->MilitaryRank()) <= 5)
					{
						l_SideAMembers.insert(*l_Itr);
					}
				}
				//Must not be at war
				l_Conditions[ETreatyConditions::RelationsNotAtWar] = EConditionStatusRelation::NotAtWar;

				if(l_SideAMembers.size() <= 1)
					return;
			}
			else
				return;
		}
		break;
	case ETreatyType::WeaponTradeEmbargo:
		{
			if(l_vCountriesHate.size() > 0)
			{
				for(set<UINT32>::const_iterator l_Itr = l_vCountriesHate.begin();
					l_Itr != l_vCountriesHate.end(); l_Itr++)
				{
					if(!g_ServerDAL.WeaponTradeEmbargo(in_iCountryID,*l_Itr))
					{
						l_SideAMembers.insert(in_iCountryID);
						l_SideAMembers.insert(l_vCountriesLove.begin(),l_vCountriesLove.end());
						l_SideBMembers.insert(*l_Itr);
						break;
					}
				}
				if(l_SideAMembers.size() < 1 || l_SideBMembers.size() < 1)
					return;
			}
			else
				return;
		}
		break;
	}

	g_ServerDCL.CreateNewTreaty(in_iCountryID,l_SideAMembers,l_SideBMembers,l_SidePressure,in_TypeOfTreaty,l_bPrivate,l_sNewTreatyName,l_Conditions);

}

ETreatyRefusal::Enum GAI::CountryShouldJoinTreaty(UINT32 in_iCountryID, UINT32 in_iTreatyID, UINT32 in_iSideToJoin)
{

	GTreaty* l_pTreaty = g_ServerDAL.Treaty(in_iTreatyID);
	if(!l_pTreaty)
		return ETreatyRefusal::Unknown;

	ETreatyRefusal::Enum l_eJoin = ETreatyRefusal::Accepted;

	switch(l_pTreaty->Type())
	{
	case ETreatyType::Alliance:
		l_eJoin = HandleAllianceOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::MilitaryAccess:
		l_eJoin = HandleMilitaryAccessOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::Annexation:
		l_eJoin = HandleAnnexationOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::FreeRegion:
		l_eJoin = HandleFreeRegionOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::War:
		l_eJoin = HandleRequestWarOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::NobleCause:
		l_eJoin = HandleNobleCauseOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::CulturalExchanges:
		l_eJoin = HandleCulturalExchangesOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::HumanDevelopmentCollaboration:
		l_eJoin = HandleHumanDevelopmentCollaborationOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::EconomicPartnership:
		l_eJoin = HandleEconomicPartnershipOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::AssumeForeignDebt:
		l_eJoin = HandleAssumeForeignDebtOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::RequestMilitaryPresenceRemoval:
		l_eJoin = HandleMilitaryPresenceRemovalOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::EconomicEmbargo:
		l_eJoin = HandleEconomicEmbargoOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::EconomicAid:
		l_eJoin = HandleEconomicAidOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::CommonMarket:
		l_eJoin = HandleCommonMarketOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::ResearchPartnership:
		l_eJoin = HandleResearchPartnershipOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::WeaponTrade:
		l_eJoin = HandleWeaponTradeOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;
	case ETreatyType::WeaponTradeEmbargo:
		l_eJoin = HandleWeaponTradeEmbargoOffer(l_pTreaty,in_iCountryID,in_iSideToJoin);
		break;	
		
	}

	return l_eJoin;

}

ETreatyRefusal::Enum GAI::HandleAllianceOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{
	if(in_iSideToJoin == 2 || in_iSideToJoin == 3)
		return ETreatyRefusal::Unknown;

	const set<UINT32>& l_vSideA = in_pTreaty->MembersSideA(false);

	REAL32 l_fPressureRatio = g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry);

	bool l_bAlreadyInsideTreaty = false;
	if(in_pTreaty->MembersSideA(true).count(in_iCountry) == 1  || in_pTreaty->IsSuspended(in_iCountry))
		l_bAlreadyInsideTreaty = true;

	bool l_bBadRelations = false;

	REAL32 l_fWorseRelationsPossible = SP2::c_fRelationsFriend;
	if(l_bAlreadyInsideTreaty)
		l_fWorseRelationsPossible = SP2::c_fRelationsBad;
	else
		l_fWorseRelationsPossible -= (40.f * l_fPressureRatio);

	//Check if the country has any ennemies inside that treaty
	for(set<UINT32>::const_iterator l_Itr = l_vSideA.begin();
		 l_Itr != l_vSideA.end();
		 l_Itr++)
	{
		if(*l_Itr == in_iCountry)
			continue;
		if(!in_pTreaty->IsSuspended(*l_Itr) && g_ServerDAL.RelationBetweenCountries(in_iCountry,*l_Itr) < l_fWorseRelationsPossible)
		{
			l_bBadRelations = true;
			break;
		}
	}
	if(l_bBadRelations)
	{
		//The country doesn't accept that treaty for sure. 
		return ETreatyRefusal::RelationsAreTooLow;
	}

	//! \todo Find more stuff to decide if the country joins or not
	return ETreatyRefusal::Accepted;
}

ETreatyRefusal::Enum GAI::HandleMilitaryAccessOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{
	if(in_iSideToJoin == 3)
		return ETreatyRefusal::Unknown;

	UINT32 l_iOtherCountry = 0;
	bool l_bAlreadyInsideTreaty = false;
	if(in_iSideToJoin == 2)
	{
      if(!in_pTreaty->MembersSideA(false).empty() )
      {
		   l_iOtherCountry = *(in_pTreaty->MembersSideA(false).begin());

		   if(in_pTreaty->MembersSideB(true).count(in_iCountry) == 1  || in_pTreaty->IsSuspended(in_iCountry))
			   l_bAlreadyInsideTreaty = true;
      }
	}
	else
	{
      if(!in_pTreaty->MembersSideB(false).empty() )
      {
		   l_iOtherCountry = *(in_pTreaty->MembersSideB(false).begin());			

		   if(in_pTreaty->MembersSideA(true).count(in_iCountry) == 1  || in_pTreaty->IsSuspended(in_iCountry))
			   l_bAlreadyInsideTreaty = true;
      }
   }

   if(l_iOtherCountry == 0)
      return ETreatyRefusal::Unknown;	

	REAL32 l_fRelations = g_ServerDAL.RelationBetweenCountries(in_iCountry,l_iOtherCountry);
	REAL32 l_fPressureRatio = g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry);

	REAL32 l_fMinimumRelations = SP2::c_fRelationsLike;
	if(l_bAlreadyInsideTreaty)
		l_fMinimumRelations = SP2::c_fRelationsBad;
	else
		l_fMinimumRelations -= (30.f * l_fPressureRatio);

	//If they are not friends, they will never accept
	if(l_fRelations < l_fMinimumRelations)
	{
		return ETreatyRefusal::RelationsAreTooLow;
	}

	//If the other country is at war (or hate) against the AI's friends, or AI's allies, the AI will not accept the treaty
	for(UINT32 i=1; i<=g_ServerDAL.NbCountry(); i++)
	{
		if(i == l_iOtherCountry || i == in_iCountry)
			continue;
		if( (g_ServerDAL.RelationBetweenCountries(i,l_iOtherCountry) < SP2::c_fRelationsHate) &&
			 (g_ServerDAL.RelationBetweenCountries(i,in_iCountry) > SP2::c_fRelationsLike) )
		{
			return ETreatyRefusal::HostileRelationsWithOneOfItsFriend;
		}
	}

	return ETreatyRefusal::Accepted;
}

ETreatyRefusal::Enum GAI::HandleAnnexationOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{
	if(in_iSideToJoin == 2 || in_iSideToJoin == 3)
		return ETreatyRefusal::Unknown;

   // ensure we have still valid countries in treaty
   if(in_pTreaty->MembersOriginalsA().empty() || in_pTreaty->MembersOriginalsB().empty())
      return ETreatyRefusal::Unknown;

	UINT32 l_iCountryWantingRegions = *(in_pTreaty->MembersOriginalsA().begin());
	UINT32 l_iCountryGivingRegions  = *(in_pTreaty->MembersOriginalsB().begin());

	if(GAI::ShouldAnnexCountry(l_iCountryWantingRegions,l_iCountryGivingRegions))
		return ETreatyRefusal::Accepted;

	const SCityInfo* l_pCapitalInfo = g_ServerDAL.CapitalCity(l_iCountryWantingRegions);
	UINT32 l_iCapitalRegion = 0;
	if(l_pCapitalInfo)
		l_iCapitalRegion = l_pCapitalInfo->m_iRegionId;

	//Check if any of those regions touches the wanting country
	UINT32 l_iNbOfRegions = 0;
	REAL32 l_fDecision = 0.f;
	REAL32 l_fRegionsWeight = 0.f;
	
	//Politically-controlled regions for the giving country
	const set<UINT32>& l_vRegionsPolitical = g_ServerDAL.CountryPoliticalControl(l_iCountryGivingRegions);
	
	//Iterate through the regions of that country, and check if the regions being annexed (maybe) are worth it
	for(set<UINT32>::const_iterator l_RegionItr = l_vRegionsPolitical.begin();
	(l_RegionItr != l_vRegionsPolitical.end());
	l_RegionItr++)
	{
		UINT32 l_iRegionID = *l_RegionItr;
		GRegion* l_pRegion = g_ServerDAL.GetGRegion(l_iRegionID);

		if(l_pRegion->OwnerMilitaryId() == l_iCountryWantingRegions)
		{
			l_iNbOfRegions++;
			l_fRegionsWeight += l_pRegion->PercentageValue();

			//If the region is adjacent to the 2 countries, the treaty has a higher possibility of being accepted by the AI
			if(g_ServerDAL.RegionTouchesCountry(l_iRegionID, l_iCountryWantingRegions))
				l_fDecision++;
			else if (l_iCapitalRegion && g_ServerDCL.MovePossible(l_iCountryWantingRegions,l_iCapitalRegion,l_iRegionID))
				l_fDecision += 0.5;
			else
				l_fDecision -= 2.f;		
		}
	}

	if(l_iNbOfRegions == 0)
		return ETreatyRefusal::Unknown;

	if(in_iCountry == l_iCountryWantingRegions)
	{
		if(l_fDecision <= 0.f)
			return ETreatyRefusal::RegionsGainedAreNotAcceptable;
		return ETreatyRefusal::Accepted;
	}	
	return ETreatyRefusal::Unknown;
}

ETreatyRefusal::Enum GAI::HandleFreeRegionOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{
	if(in_iSideToJoin == 3)
		return ETreatyRefusal::Unknown;

   // ensure we have still valid countries in treaty
   if(in_pTreaty->MembersOriginalsA().empty() || in_pTreaty->MembersOriginalsB().empty())
      return ETreatyRefusal::Unknown;

	//Does Side A give the regions back to Side B?
	UINT32 l_iCountryGivingRegionsBack = *(in_pTreaty->MembersOriginalsA().begin());
	UINT32 l_iCountryWantingRegionsBack = *(in_pTreaty->MembersOriginalsB().begin());

	if(l_iCountryWantingRegionsBack == in_iCountry)
	{
		//I don't see any reasons why a country wouldn't want to get its regions back. 	
		return ETreatyRefusal::Accepted;
	}

	REAL32 l_fRelations = g_ServerDAL.RelationBetweenCountries(l_iCountryGivingRegionsBack,l_iCountryWantingRegionsBack);
	REAL32 l_fPressureRatio = g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry);

	UINT32 l_iNbOfRegions = 0;
	REAL32 l_fRegionsWeight = 0.f;

	//Politically-controlled regions for the giving country
	const set<UINT32>& l_vRegionsPolitical = g_ServerDAL.CountryPoliticalControl(l_iCountryWantingRegionsBack);
	
	//Iterate through the regions of that country, and check if the regions being liberated are worth keeping
	for(set<UINT32>::const_iterator l_RegionItr = l_vRegionsPolitical.begin();
	(l_RegionItr != l_vRegionsPolitical.end());
	l_RegionItr++)
	{
		UINT32 l_iRegionID = *l_RegionItr;
		GRegion* l_pRegion = g_ServerDAL.GetGRegion(l_iRegionID);

		if(l_pRegion->OwnerMilitaryId() == l_iCountryGivingRegionsBack)
		{
			l_iNbOfRegions++;
			l_fRegionsWeight += l_pRegion->PercentageValue();
		}
	}
	if(l_iNbOfRegions == 0)
		return ETreatyRefusal::NoRegionsToFree;

	if(l_iCountryGivingRegionsBack == in_iCountry)
	{		
		if(l_fRelations < (SP2::c_fRelationsDislike - (10.f * l_fPressureRatio)))
		{
			return ETreatyRefusal::RelationsAreTooLow;
		}

		if(l_fRelations > (SP2::c_fRelationsLove - (40.f * l_fPressureRatio)))
		{
			return ETreatyRefusal::Accepted;
		}

		//check the military forces of the giving side, and the military forces of the asking side
		REAL32 l_fTotalValueGivingBack = g_ServerDAL.TotalUnitsValueWithAllies(l_iCountryGivingRegionsBack);
		REAL32 l_fTotalValueWantingBack = g_ServerDAL.TotalUnitsValueWithAllies(l_iCountryWantingRegionsBack);
		
		if(l_fTotalValueGivingBack > l_fTotalValueWantingBack)
			return ETreatyRefusal::WantToKeepRegions;

		if(l_fRegionsWeight >= 0.25f && l_fTotalValueGivingBack > (1.5f * l_fTotalValueWantingBack))
			return ETreatyRefusal::WantToKeepRegions;

		//! \todo Find more stuff here, I'm out of ideas

		return ETreatyRefusal::Accepted;

	}

	return ETreatyRefusal::NoRegionsWouldBeGained;
}

ETreatyRefusal::Enum GAI::HandleRequestWarOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{
	if(in_iSideToJoin == 2 || in_iSideToJoin == 3)
		return ETreatyRefusal::Unknown;	

   // ensure we have still valid countries in treaty
   if(in_pTreaty->MembersOriginalsB().empty())
   {
      gassert(0, "Should have at least 1 country on side B");
      return ETreatyRefusal::Unknown;
   }

	const set<UINT32>& l_vSideAttacking = in_pTreaty->MembersOriginalsA();
	const set<UINT32>& l_vSideDefending = in_pTreaty->MembersOriginalsB();

	REAL32 l_fSumOfRelations = 0.f;
	REAL32 l_fSumOfRelationsAllies = 0.f;
	REAL32 l_fTotalUnitsEnnemies = 0.f;
	REAL32 l_fTotalUnitsAllies = 0.f;

	//If the country doesn't have ennemies on the defending side, he will not accept
	for(set<UINT32>::const_iterator it = l_vSideAttacking.begin();
		 it != l_vSideAttacking.end();
		 it++)
	{
		UINT32 l_iCountryAttacking = *it;
		if(l_iCountryAttacking != in_iCountry)
		{
			l_fSumOfRelationsAllies += g_ServerDAL.RelationBetweenCountries(in_iCountry,l_iCountryAttacking);
		}		
	}

	if(l_vSideAttacking.size() > 1)
		l_fSumOfRelationsAllies /= (REAL32)(l_vSideAttacking.size() - 1);
	else
		l_fSumOfRelationsAllies = 0.f;

	//If the country doesn't have ennemies on the defending side, he will not accept
	for(set<UINT32>::const_iterator it = l_vSideDefending.begin();
		 it != l_vSideDefending.end();
		 it++)
	{
		UINT32 l_iCountryDefending = *it;
		l_fSumOfRelations += g_ServerDAL.RelationBetweenCountries(in_iCountry,l_iCountryDefending);		
		l_fTotalUnitsEnnemies += g_ServerDAL.TotalUnitsValue(l_iCountryDefending);
	}
	l_fSumOfRelations /= (REAL32)l_vSideDefending.size();
	
	REAL32 l_fPressureRatio = g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry);

	REAL32 l_fLimit = (SP2::c_fRelationsHate + (l_fPressureRatio * 20.f)) + (l_fSumOfRelationsAllies * 0.5f) - 10.f + (20.f * g_SP2Server->AIAggressiveness());
	l_fLimit = min(l_fLimit,100.f);
	l_fLimit = max(l_fLimit,-100.f);

	if(l_fSumOfRelations > l_fLimit)
	{
		return ETreatyRefusal::TooGoodRelationsWithTheEnemies;
	}

	bool l_bAlreadyInsideTreaty = false;
	if(in_pTreaty->MembersSideA(true).count(in_iCountry) == 1  || in_pTreaty->IsSuspended(in_iCountry))
		l_bAlreadyInsideTreaty = true;

	for(set<UINT32>::const_iterator it = l_vSideAttacking.begin();
		it != l_vSideAttacking.end();
		it++)
	{
		l_fTotalUnitsAllies += g_ServerDAL.TotalUnitsValue(*it);
	}

	if(l_bAlreadyInsideTreaty)
		l_fTotalUnitsEnnemies *= (1.f - g_SP2Server->AIAggressiveness());

	//If, at the start, there are not enough allies, it's not a good idea to start a war. 
	if(l_fTotalUnitsAllies < l_fTotalUnitsEnnemies)
		return ETreatyRefusal::AlliedForcesAreTooWeak;

	return ETreatyRefusal::Accepted;  
}

ETreatyRefusal::Enum GAI::HandleNobleCauseOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{
	if(in_iSideToJoin == 3 || in_iSideToJoin == 2 )
		return ETreatyRefusal::Unknown;

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountry);

	bool l_bAlreadyInsideTreaty = false;
	if(in_pTreaty->MembersSideA(true).count(in_iCountry) == 1  || in_pTreaty->IsSuspended(in_iCountry))
		l_bAlreadyInsideTreaty = true;

	if(l_pCountryData->BudgetExpenses() > l_pCountryData->BudgetRevenues() && 
		!l_bAlreadyInsideTreaty)
		return ETreatyRefusal::UpkeepTooHigh;	

	if(g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry) > 0.5f)
		return ETreatyRefusal::Accepted;

	//Check if any friends are not present. If that's the case, the country will not join the treaty, 
	//since it doesn't want to lose relations.
	bool l_bFoundFriend = false;
	for(UINT32 i=1; i <= g_ServerDAL.NbCountry(); i++)
	{
		if(in_iCountry == i)
			continue;
		if(g_ServerDAL.RelationBetweenCountries(in_iCountry,i) > SP2::c_fRelationsLike &&
			in_pTreaty->MembersSideA(false).count(i) == 0)
		{
			l_bFoundFriend = true;
			break;
		}

	}

	if(l_bFoundFriend)
	{
		return ETreatyRefusal::DoesNotWantToLoseRelationsWithOtherFriendlyCountries;
	}

	return ETreatyRefusal::Accepted;
	
}

ETreatyRefusal::Enum GAI::HandleCulturalExchangesOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{
	if(in_iSideToJoin == 3)
		return ETreatyRefusal::Unknown;

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountry);
	if(l_pCountryData->BudgetRevenues() > l_pCountryData->BudgetExpenses())
		return ETreatyRefusal::Accepted;
	else
	{
		if(g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry) > 0.5f)
			return ETreatyRefusal::Accepted;

		//If the country is already inside, accept it
		bool l_bAlreadyInsideTreaty = false;
		if(in_pTreaty->MembersSideA(true).count(in_iCountry) == 1  || in_pTreaty->IsSuspended(in_iCountry))
			l_bAlreadyInsideTreaty = true;
		if( l_bAlreadyInsideTreaty )
			return ETreatyRefusal::Accepted;
		return ETreatyRefusal::UpkeepTooHigh;
	}
}

ETreatyRefusal::Enum GAI::HandleHumanDevelopmentCollaborationOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{
	if(in_iSideToJoin == 3)
		return ETreatyRefusal::Unknown;

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountry);
	const set<UINT32>& l_vSideA = in_pTreaty->MembersSideA(false);
	
	REAL32 l_fSumOfRelations = 0.f;
	REAL32 l_fMaxHumanDevelopment = l_pCountryData->HumanDevelopment();
	REAL32 l_fMinRelations = 0.f;

	UINT32 l_iNbCountries = 0;

	//If the country doesn't like the other countries, it will not accept
	for(set<UINT32>::const_iterator it = l_vSideA.begin();
		 it != l_vSideA.end();
		 it++)
	{
		UINT32 l_iCountryA = *it;
		if(l_iCountryA != in_iCountry)
		{
			l_fSumOfRelations += g_ServerDAL.RelationBetweenCountries(in_iCountry,l_iCountryA);		
			l_fMinRelations = min(l_fMinRelations,g_ServerDAL.RelationBetweenCountries(in_iCountry,l_iCountryA));
			l_fMaxHumanDevelopment = max(g_ServerDAL.CountryData(l_iCountryA)->HumanDevelopment(),l_fMaxHumanDevelopment);
			l_iNbCountries++;
		}
	}
	if(!l_iNbCountries)
		return ETreatyRefusal::Unknown;
	l_fSumOfRelations /= (REAL32)l_iNbCountries;
	
	REAL32 l_fPressureRatio = g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry);

	if(l_fSumOfRelations <= (SP2::c_fRelationsDislike - (20.f * l_fPressureRatio)) || l_fMinRelations < SP2::c_fRelationsHate)
	{
		return ETreatyRefusal::RelationsAreTooLow;
	}

	//! \todo Check what to do with high/low human development level. 

	return ETreatyRefusal::Accepted;

}

ETreatyRefusal::Enum GAI::HandleEconomicPartnershipOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{
	if(in_iSideToJoin == 3)
		return ETreatyRefusal::Unknown;

	const set<UINT32>& l_vSideA = in_pTreaty->MembersSideA(false);
	
	REAL32 l_fSumOfRelations = 0.f;
	REAL32 l_fMinRelations = 0.f;
	UINT32 l_iNbCountries = 0;

	//If the country doesn't like the other countries, it will not accept
	for(set<UINT32>::const_iterator it = l_vSideA.begin();
		 it != l_vSideA.end();
		 it++)
	{
		UINT32 l_iCountryA = *it;
		if(l_iCountryA != in_iCountry)
		{
			l_fSumOfRelations += g_ServerDAL.RelationBetweenCountries(in_iCountry,l_iCountryA);		
			l_fMinRelations = min(l_fMinRelations,g_ServerDAL.RelationBetweenCountries(in_iCountry,l_iCountryA));
			l_iNbCountries++;
		}
	}
	if(!l_iNbCountries)
		return ETreatyRefusal::Unknown;
	l_fSumOfRelations /= (REAL32)l_iNbCountries;
	
	REAL32 l_fPressureRatio = g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry);

	if(l_fSumOfRelations <= (SP2::c_fRelationsDislike - (20.f * l_fPressureRatio)) || l_fMinRelations < SP2::c_fRelationsHate)
	{
		return ETreatyRefusal::RelationsAreTooLow;
	}	

	//! \todo Check low or high resource production for that country 

	return ETreatyRefusal::Accepted;

}

ETreatyRefusal::Enum GAI::HandleAssumeForeignDebtOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{
	if(in_iSideToJoin == 3)
		return ETreatyRefusal::Unknown;

   // ensure we have still valid countries in treaty
   if(in_pTreaty->MembersOriginalsA().empty() || in_pTreaty->MembersOriginalsB().empty())
      return ETreatyRefusal::Unknown;

	const set<UINT32>& l_vSideA = in_pTreaty->MembersOriginalsA();
	const set<UINT32>& l_vSideB = in_pTreaty->MembersOriginalsB();

	gassert(l_vSideA.size() > 0, "Should have at least one member on side A");
	gassert(l_vSideB.size() > 0, "Should have at least one member on side B");

	if(in_iSideToJoin == 1)
	{
		GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountry);
		if(l_pCountryData->BudgetExpenses() > l_pCountryData->BudgetRevenues())
			return ETreatyRefusal::UpkeepTooHigh;

		REAL32 l_fRelations = 0.f;
		REAL32 l_fMinRelations = 0.f;
		REAL64 l_fDebt = 0.f;
		//The AI must love every country, and not have any ennemies
		for(set<UINT32>::const_iterator l_ItrB = l_vSideB.begin();
			 l_ItrB != l_vSideB.end();
			 l_ItrB++)
		{
            //Don't allow AI to assume players' debt if the setting disallows it.
            const SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayerByModID(*l_ItrB);
            if(!g_SP2Server->AllowAIAssumeDebt() && l_pPlayer)
                return ETreatyRefusal::Unknown;

			if(g_ServerDAL.CountryData(*l_ItrB)->EconomicRank() < l_pCountryData->EconomicRank())
				return ETreatyRefusal::WillNotHelpCountriesInBetterShapeThanItsOwnCountry;

			l_fRelations += g_ServerDAL.RelationBetweenCountries(*l_ItrB,in_iCountry);
			l_fMinRelations = min(g_ServerDAL.RelationBetweenCountries(*l_ItrB,in_iCountry),l_fMinRelations);
			l_fDebt += min(g_ServerDAL.CountryData(*l_ItrB)->MonetarySupplyAmount(),0.f);
		}
		l_fRelations /= (REAL32)l_vSideB.size();
		REAL32 l_fPressureRatio = g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry);
		if( (l_fRelations < (SP2::c_fRelationsLove - (20.f * l_fPressureRatio))) || (l_fMinRelations < (SP2::c_fRelationsDislike - (20.f * l_fPressureRatio))) )
			return ETreatyRefusal::RelationsAreTooLow;
		
		//Can it afford paying off their debts?
		//Calculate the sum this country should pay. 
		REAL32 l_fEconomicStrength = 0.f;
		REAL64 l_fPartOfDebt = 0.f;
		//first we need to calculate the economic stength of the whole treaty
		for(set<UINT32>::const_iterator l_Itr = l_vSideA.begin();
			 l_Itr != l_vSideA.end();
			 l_Itr++)
		{
			l_fEconomicStrength += g_ServerDAL.CountryData(*l_Itr)->EconomicStrength();
		}
		REAL64 l_fDebtRatio = (REAL64)(l_pCountryData->EconomicStrength() / l_fEconomicStrength);
		l_fPartOfDebt = l_fDebtRatio * l_fDebt;
		//Put the relations average between 0 and 1. 
		REAL64 l_fWillToPay = (REAL64)( (l_fRelations - SP2::c_fRelationsLike) / (100.f - SP2::c_fRelationsLike) );
		REAL64 l_fAmountWillingToPay = (SP2::c_fMinimumPourcOfRevenuesGivenToForeignDebt + ((SP2::c_fMaximumPourcOfRevenuesGivenToForeignDebt - SP2::c_fMinimumPourcOfRevenuesGivenToForeignDebt) * l_fWillToPay)) * 
												  l_pCountryData->BudgetRevenues();
		if(l_fAmountWillingToPay < l_fPartOfDebt)
			return ETreatyRefusal::CannotAffordTreaty;
		return ETreatyRefusal::Accepted;
	}
	else
	{
		for(set<UINT32>::const_iterator l_ItrA = l_vSideA.begin();
			 l_ItrA != l_vSideA.end();
			 l_ItrA++)
		{
			if(g_ServerDAL.RelationBetweenCountries(*l_ItrA,in_iCountry) < SP2::c_fRelationsHate)
				return ETreatyRefusal::RelationsAreTooLow;
		}
		return ETreatyRefusal::Accepted;
	}
}

ETreatyRefusal::Enum GAI::HandleMilitaryPresenceRemovalOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{
	if(in_iSideToJoin == 3)
		return ETreatyRefusal::Unknown;

   // ensure we have still valid countries in treaty
   if(in_pTreaty->MembersOriginalsA().empty() || in_pTreaty->MembersOriginalsB().empty())
      return ETreatyRefusal::Unknown;

	UINT32 l_iCountryAskingToLeave = *(in_pTreaty->MembersOriginalsA().begin());
	UINT32 l_iCountryInsideCountry = *(in_pTreaty->MembersOriginalsB().begin());
	
	REAL32 l_fTotalValueAsking = g_ServerDAL.TotalUnitsValueWithAllies(l_iCountryAskingToLeave);
	REAL32 l_fTotalValueInside = g_ServerDAL.TotalUnitsValueWithAllies(l_iCountryInsideCountry);
	REAL32 l_fRatio = 10.f;

	REAL32 l_fRelations = g_ServerDAL.RelationBetweenCountries(l_iCountryAskingToLeave,l_iCountryInsideCountry);	

	if(in_iCountry == l_iCountryAskingToLeave)
	{
		if(l_fRelations > SP2::c_fRelationsLike)
			return ETreatyRefusal::DoesNotMindTheOtherCountryMilitaryUnits;

		if(l_fTotalValueInside > 0.f)
			l_fRatio = l_fTotalValueAsking / l_fTotalValueInside;
		else
		{
			if(l_fTotalValueAsking == 0.f)
				l_fRatio = 1.f;
		}		

		REAL32 l_fPressureRatio = g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry);
		if(l_fRelations <= (SP2::c_fRelationsDislike + (20.f * l_fPressureRatio)))
		{
			REAL32 l_fMinimumRatioBeforeAccepting = 0.f;
			//Put relations between 0 and 1, -40: 0, -100: 1
			REAL32 l_fWillToAsk = (l_fRelations - SP2::c_fRelationsDislike) / (-100.f - SP2::c_fRelationsDislike);
			l_fMinimumRatioBeforeAccepting = (SP2::c_fMinimumPourcOfRatioWhenComparingMilitaryRemoval + ((SP2::c_fMaximumPourcOfRatioWhenComparingMilitaryRemoval - SP2::c_fMinimumPourcOfRatioWhenComparingMilitaryRemoval) * l_fWillToAsk));
			if(l_fRatio < l_fMinimumRatioBeforeAccepting)
				return ETreatyRefusal::Unknown;
			else
				return ETreatyRefusal::Accepted;
		}
		else
		{
			return ETreatyRefusal::Accepted;
		}

	}
	else if (in_iCountry == l_iCountryInsideCountry)
	{
		if(l_fRelations >= SP2::c_fRelationsLike)
		{
			return ETreatyRefusal::Accepted;
		}

		if(l_fTotalValueAsking > 0.f)
			l_fRatio = l_fTotalValueInside / l_fTotalValueAsking;
		else
		{
			if(l_fTotalValueInside == 0.f)
				l_fRatio = 1.f;
		}	

		if(l_fRelations <= SP2::c_fRelationsDislike)
		{
			REAL32 l_fMinimumRatioBeforeAccepting = 0.f;
			//Put relations between 0 and 1, -40: 0, -100: 1
			REAL32 l_fWillToAsk = (l_fRelations - SP2::c_fRelationsDislike) / (-100.f - SP2::c_fRelationsDislike);
			l_fMinimumRatioBeforeAccepting = (SP2::c_fMinimumPourcOfRatioWhenComparingMilitaryRemoval + ((SP2::c_fMaximumPourcOfRatioWhenComparingMilitaryRemoval - SP2::c_fMinimumPourcOfRatioWhenComparingMilitaryRemoval) * l_fWillToAsk));
			if(l_fRatio < l_fMinimumRatioBeforeAccepting)
				return ETreatyRefusal::Accepted;
			else
				return ETreatyRefusal::WantsToKeepItsUnitsThere;
				
		}
		else
		{
			//Relations are between -40 and 40. The country will sign it, to avoid making ennemies, or losing possible friends
			return ETreatyRefusal::Accepted;
		}
			
	}
	return ETreatyRefusal::Unknown;
}

ETreatyRefusal::Enum GAI::HandleEconomicAidOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{	
	
	if(in_iSideToJoin == 3)
		return ETreatyRefusal::Unknown;

	if(in_iSideToJoin == 2)
	{
		//Always join on second side
		return ETreatyRefusal::Accepted;
	}
	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountry);

	bool l_bAlreadyInsideTreaty = false;
	if(in_pTreaty->MembersSideA(true).count(in_iCountry) == 1  || in_pTreaty->IsSuspended(in_iCountry))
		l_bAlreadyInsideTreaty = true;	

	if(in_pTreaty->MembersSideA(false).size() == 0)
		return ETreatyRefusal::Unknown;

	const set<UINT32>& l_MembersOnSideB = in_pTreaty->MembersSideB(false);
	if(l_MembersOnSideB.size() == 0)
		return ETreatyRefusal::Unknown;

	REAL64 l_fTreatyCost = 0.f;
	REAL32 l_fTotalRelations = 0.f;
	REAL64 l_fSizeA = (REAL64)in_pTreaty->MembersSideA(false).size();	
	for(set<UINT32>::const_iterator it = l_MembersOnSideB.begin();
		 it != l_MembersOnSideB.end(); it++)
	{
		//No economic ranks lower
		if(g_ServerDAL.CountryData(*it)->EconomicRank() < l_pCountryData->EconomicRank())
			return ETreatyRefusal::SomeCountriesInsideTheTreatyAreTooWeakEconomically;
		l_fTreatyCost += g_ServerDAL.CountryData(*it)->BudgetExpenses() * SP2::c_fMaximumPourcPaidByEconomicAid / l_fSizeA ;
		l_fTotalRelations += g_ServerDAL.RelationBetweenCountries(*it,in_iCountry);
	}
	l_fTreatyCost = min(l_fTreatyCost,g_ServerDAL.CountryData(in_iCountry)->BudgetExpenses() * SP2::c_fMaximumPourcPaidByEconomicAid);
	l_fTreatyCost += in_pTreaty->CostForCountry(in_iCountry);
	l_fTotalRelations /= (REAL32)l_MembersOnSideB.size();
	REAL64 l_fRatioWillingToSpend = 1.f;

	REAL32 l_fPressureRatio = g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry);
	REAL32 l_fMinRelations = SP2::c_fRelationsLike;
	if(l_bAlreadyInsideTreaty)
		l_fMinRelations = SP2::c_fRelationsBad;
	else
		l_fMinRelations -= (20.f * l_fPressureRatio);

	if(l_fTotalRelations < l_fMinRelations)
	{
		return ETreatyRefusal::RelationsAreTooLow;
	}
	//Ratio between 1 and 1.05
	if(l_fTotalRelations > SP2::c_fRelationsGood)
		l_fRatioWillingToSpend = 1.05f;
	else
		l_fRatioWillingToSpend = 1.025f + (0.00125f* - l_fTotalRelations);
	
	if( (l_fTreatyCost + l_pCountryData->BudgetExpenses()) / (l_pCountryData->BudgetRevenues() + 1.f) > l_fRatioWillingToSpend)
		return ETreatyRefusal::UpkeepTooHigh;
	else
		return ETreatyRefusal::Accepted;

}
ETreatyRefusal::Enum GAI::HandleCommonMarketOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{	
	if(in_iSideToJoin == 3)
		return ETreatyRefusal::Unknown;

	const set<UINT32>& l_Members = in_pTreaty->MembersSideA(false);
	if(l_Members.size() == 0)
		return ETreatyRefusal::Unknown;
	
	bool l_bAlreadyInsideTreaty = false;
	if(in_pTreaty->MembersSideA(true).count(in_iCountry) == 1  || in_pTreaty->IsSuspended(in_iCountry))
		l_bAlreadyInsideTreaty = true;

	REAL64 l_fTreatyCost = in_pTreaty->CostForCountry(in_iCountry);	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountry);
	if( (l_fTreatyCost + l_pCountryData->BudgetExpenses()) / (l_pCountryData->BudgetRevenues() + 1.f) > 1.25f &&
		!l_bAlreadyInsideTreaty)
	{
		return ETreatyRefusal::UpkeepTooHigh;
	}


	//Find the list of countries that already have a common market with that country
	set<UINT32> l_ListOfCountriesAlreadyInCommonMarket;

	if(!l_bAlreadyInsideTreaty)
	{
		const hash_map<UINT32,GTreaty>& l_Treaties = g_ServerDAL.Treaties();
		for(hash_map<UINT32,GTreaty>::const_iterator it = l_Treaties.begin();
			it != l_Treaties.end(); it++)
		{
			const GTreaty& l_CurTreaty = it->second;
			if(l_CurTreaty.Active() &&
				l_CurTreaty.Type() == ETreatyType::CommonMarket &&
				l_CurTreaty.CountrySide(in_iCountry) == 1 &&
				!l_CurTreaty.IsSuspended(in_iCountry))
			{
				const set<UINT32>& l_Members = l_CurTreaty.MembersSideA(true);
				for(set<UINT32>::const_iterator itr = l_Members.begin();
					itr != l_Members.end(); itr++)
				{
					l_ListOfCountriesAlreadyInCommonMarket.insert(*itr);
				}
			}
		}
	}
	//If the country has good relations with members, and is not inside a common market with at least one of them, it'll sign it
	REAL32 l_fTotalRelations = 0.f;	
	bool l_bOkay = false;
	UINT32 l_iNbCountry = 0;
	for(set<UINT32>::const_iterator it = l_Members.begin();
		 it != l_Members.end(); it++)
	{
		if(*it != in_iCountry)
		{
			if(l_ListOfCountriesAlreadyInCommonMarket.count(*it) == 0)
				l_bOkay = true;
			l_fTotalRelations += g_ServerDAL.RelationBetweenCountries(in_iCountry,*it);
			l_iNbCountry++;
		}		
	}
	if(!l_iNbCountry)
		return ETreatyRefusal::Unknown;
	REAL32 l_fMinRelations = SP2::c_fRelationsGood;
	REAL32 l_fPressureRatio = g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry);
	if(l_bAlreadyInsideTreaty)
		l_fMinRelations = SP2::c_fRelationsBad;
	else
		l_fMinRelations -= (10.f * l_fPressureRatio);
	l_fTotalRelations /= (REAL32)l_iNbCountry;
	if(!l_bOkay || l_fTotalRelations < l_fMinRelations)
		return ETreatyRefusal::NoNeedToHaveAnotherCommonMarket;
	
	return ETreatyRefusal::Accepted;

}
ETreatyRefusal::Enum GAI::HandleResearchPartnershipOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{
	if(in_iSideToJoin == 3 || in_iSideToJoin == 2)
		return ETreatyRefusal::Unknown;

	const set<UINT32>& l_Members = in_pTreaty->MembersSideA(false);
	if(l_Members.size() <= 1)
		return ETreatyRefusal::Unknown;

	bool l_bAlreadyInsideTreaty = false;
	if(in_pTreaty->MembersSideA(true).count(in_iCountry) == 1  || in_pTreaty->IsSuspended(in_iCountry))
		l_bAlreadyInsideTreaty = true;

	REAL64 l_fTreatyCost = in_pTreaty->CostForCountry(in_iCountry);	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountry);
	if( (l_fTreatyCost + l_pCountryData->BudgetExpenses()) / (l_pCountryData->BudgetRevenues() + 1.f) > 1.1f &&
		!g_ServerDAL.IsAtWar(in_iCountry) && !l_bAlreadyInsideTreaty )
	{
		return ETreatyRefusal::UpkeepTooHigh;
	}	

	const UINT32* l_pConditions = in_pTreaty->Conditions();
	//If there are no limit, countries can leech research from other countries. It will only join in that case if 50% of countries have higher economic ranks
	REAL32 l_fRatioHigherEconomicRanks = 0.f;	
	REAL64 l_fNecessaryRatio = 0.f;
	switch(l_pConditions[ETreatyConditions::ResearchLevelSimilitude])
	{
	case EConditionStatusResearch::High:
		l_fRatioHigherEconomicRanks = 0.f; l_fNecessaryRatio = SP2::c_fConditionsResearchHigh; break;
	case EConditionStatusResearch::Average:
		l_fRatioHigherEconomicRanks = 0.2f; l_fNecessaryRatio = SP2::c_fConditionsResearchAverage; break;
	case EConditionStatusResearch::Low:
		l_fRatioHigherEconomicRanks = 0.3f; l_fNecessaryRatio = SP2::c_fConditionsResearchLow; break;
	case EConditionStatusResearch::NoLimit:
		l_fRatioHigherEconomicRanks = 0.5f; l_fNecessaryRatio = 0.f; break;
	}
		
	REAL32 l_fTotalRelations = 0.f;	
	UINT32 l_iNbCountries = 0;
	for(set<UINT32>::const_iterator it = l_Members.begin();
		 it != l_Members.end(); it++)
	{
		if(*it != in_iCountry)
		{
			l_fTotalRelations += g_ServerDAL.RelationBetweenCountries(in_iCountry,*it);
			if(g_ServerDAL.CountryData(*it)->EconomicRank() < l_pCountryData->EconomicRank())
				l_iNbCountries++;
		}		
	}
	if(!l_iNbCountries)
		return ETreatyRefusal::Unknown;
	l_fTotalRelations /= (REAL32)l_iNbCountries;
	REAL32 l_fRatioBetter = (REAL32)l_iNbCountries / (REAL32)(l_Members.size() - 1);
	REAL32 l_fPressureRatio = g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry);
	if(l_fTotalRelations >= (SP2::c_fRelationsGood - (20.f * l_fPressureRatio)))
	{
		l_fRatioHigherEconomicRanks -= 0.1f;
		l_fRatioHigherEconomicRanks = max(0.f,l_fRatioHigherEconomicRanks);
	}
	if(l_fRatioBetter >= l_fRatioHigherEconomicRanks)
	{
		//Now check if we can afford staying at that research condition
		if(l_pCountryData->BudgetExpenseResearchRatio() - l_fNecessaryRatio < 0.f)
			return ETreatyRefusal::CannotAffordToMeetResearchConditions;
		else
			return ETreatyRefusal::Accepted;
	}
	else
		return ETreatyRefusal::ResearchTreatyNotProfitableEnough;



}
ETreatyRefusal::Enum GAI::HandleWeaponTradeOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{
	if(in_iSideToJoin == 3 || in_iSideToJoin == 2)
		return ETreatyRefusal::Unknown;

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountry);
	if(l_pCountryData->EconomicRank() < 40 && !in_pTreaty->Private())
	{
		//! No leech wanted!
		return ETreatyRefusal::DoesNotWantAPublicTreaty;
	}
	REAL64 l_fTreatyCost = in_pTreaty->CostForCountry(in_iCountry);		
	if((l_fTreatyCost + l_pCountryData->BudgetExpenses()) / (l_pCountryData->BudgetRevenues() + 1.f) > 1.f)
	{
		return ETreatyRefusal::UpkeepTooHigh;
	}
	REAL32 l_fMinimumRelations = SP2::c_fRelationsFriend;
	REAL32 l_fPressureRatio = g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry);
	l_fMinimumRelations -= (20.f * l_fPressureRatio);
	//Now make sure we like everybody
	const set<UINT32>& l_Members = in_pTreaty->MembersSideA(false);
	for(set<UINT32>::const_iterator it = l_Members.begin();
		 it != l_Members.end(); it++)
	{
		if(g_ServerDAL.RelationBetweenCountries(*it,in_iCountry) < l_fMinimumRelations)
		{
			return ETreatyRefusal::RelationsAreTooLow;	
		}
	}

	return ETreatyRefusal::Accepted;

}
ETreatyRefusal::Enum GAI::HandleWeaponTradeEmbargoOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin) 
{
	if(in_iSideToJoin == 2 || in_iSideToJoin == 3)
		return ETreatyRefusal::Unknown;
	REAL64 l_fTreatyCost = in_pTreaty->CostForCountry(in_iCountry);	
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountry);
	if((l_fTreatyCost + l_pCountryData->BudgetExpenses()) / (l_pCountryData->BudgetRevenues() + 1.f) > 1.f)
	{
		return ETreatyRefusal::UpkeepTooHigh;
	}
	REAL32 l_fPressureRatio = g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry);
	//If the country is already doing with all of them, it will refuse
	//if(g_ServerDAL.WeaponTradeEmbargo(in_iCountry,))
	const set<UINT32>& l_Members = in_pTreaty->MembersSideA(false);
	for(set<UINT32>::const_iterator it = l_Members.begin();
		 it != l_Members.end(); it++)
	{
		if(g_ServerDAL.RelationBetweenCountries(*it,in_iCountry) > (SP2::c_fRelationsDislike + (20.f * l_fPressureRatio)))
		{
			return ETreatyRefusal::LikeTheOtherCountriesTooMuch;
		}
	}

	return ETreatyRefusal::Accepted;
}

ETreatyRefusal::Enum GAI::HandleEconomicEmbargoOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin)
{
	if(in_iSideToJoin == 3)
		return ETreatyRefusal::Unknown;

	const set<UINT32>& l_vSideA = in_pTreaty->MembersSideA(false);
	const set<UINT32>& l_vSideB = in_pTreaty->MembersSideB(false);

	if(in_iSideToJoin == 1)
	{
		set<UINT32> l_vAlliesOfSideA;
		set<UINT32> l_vAlliesOfSideB;

		//Side that will prevent trade

		//Check if only ennemies are on side B
		for(set<UINT32>::const_iterator l_ItrB = l_vSideB.begin();
			 l_ItrB != l_vSideB.end();
			 l_ItrB++)
		{
			g_ServerDAL.CountryAllies(*l_ItrB,l_vAlliesOfSideB);
			l_vAlliesOfSideB.insert(*l_ItrB);
			if(g_ServerDAL.RelationBetweenCountries(in_iCountry,*l_ItrB) > SP2::c_fRelationsDislike)
			{
				return ETreatyRefusal::LikeTheOtherCountriesTooMuch;
			}
		}

		//If that country cannot fufill 80% of its ressources, it will not make an embargo
		if(SP2::GWorldBehavior::FindResourceFactor(in_iCountry) < 0.8f)
			return ETreatyRefusal::OwnCountryIsInBadEconomicShapeAlready;
		
		//If there are ennemies on side A, that country will not join in

		for(set<UINT32>::const_iterator l_ItrA = l_vSideA.begin();
			 l_ItrA != l_vSideA.end();
			 l_ItrA++)
		{
			g_ServerDAL.CountryAllies(*l_ItrA,l_vAlliesOfSideA);
			l_vAlliesOfSideA.insert(*l_ItrA);

			if(*l_ItrA == in_iCountry)
				continue;

			REAL32 l_fPressureRatio = g_ServerDCL.PressureStrengthTreaty(in_pTreaty,in_iCountry);
			if(g_ServerDAL.RelationBetweenCountries(in_iCountry,*l_ItrA) <= (SP2::c_fRelationsDislike + (20.f * l_fPressureRatio)))
			{
				return ETreatyRefusal::RelationsAreTooLow;
			}
		}

		//If the other country has too many allies, the country will not accep to join in		
		REAL32 l_fTotalValueSideA = 0.f;
		REAL32 l_fTotalValueSideB = 0.f;

		for(set<UINT32>::const_iterator l_Itr = l_vAlliesOfSideA.begin();
			l_Itr != l_vAlliesOfSideA.end();
			l_Itr++)
		{
			l_fTotalValueSideA += g_ServerDAL.TotalUnitsValue(*l_Itr);
		}
		for(set<UINT32>::const_iterator l_Itr = l_vAlliesOfSideB.begin();
			l_Itr != l_vAlliesOfSideB.end();
			l_Itr++)
		{
			l_fTotalValueSideB += g_ServerDAL.TotalUnitsValue(*l_Itr);
		}

		if(l_fTotalValueSideB > l_fTotalValueSideA)
		{
			return ETreatyRefusal::TheOtherCountriesHaveATooStrongMilitary;
		}

		return ETreatyRefusal::Accepted;

	}	
	return ETreatyRefusal::Unknown;
}

void GAI::FillConditions(UINT32 in_iCountryID, UINT32* in_pConditions, UINT32 l_iNbCountriesPossible)
{
	UINT32 l_iCapitalRegionId = 0;
	EGeoGroups::Enum l_GeoGroup = EGeoGroups::Undefined;
	EContinent::Enum l_Continent = EContinent::Undefined;
	const SCityInfo* l_pCapitalInfo = g_ServerDAL.CapitalCity(in_iCountryID);
	if(l_pCapitalInfo)
	{
		l_iCapitalRegionId = l_pCapitalInfo->m_iRegionId;
		l_GeoGroup =  g_ServerDAL.RegionGeoGroup(l_iCapitalRegionId);
		l_Continent =  g_ServerDAL.RegionContinent(l_iCapitalRegionId);
	}
			
	if(l_iNbCountriesPossible > 100)
	{
		in_pConditions[ETreatyConditions::RelationsNotAtWar] = EConditionStatusRelation::NotAtWar;
	}
	else if (l_iNbCountriesPossible > 80)
	{
		in_pConditions[ETreatyConditions::RelationsNotAtWar] = EConditionStatusRelation::NotAtWar;
		if(l_pCapitalInfo)
		{
			switch(l_GeoGroup)
			{
			case EGeoGroups::CentralAmerica:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::CentralAmerica; break;
			case EGeoGroups::EasternAfrica:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::EasternAfrica; break;
			case EGeoGroups::EasternAsia:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::EasternAsia; break;
			case EGeoGroups::EasternEurope:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::EasternEurope; break;
			case EGeoGroups::FarEast:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::FarEast; break;
			case EGeoGroups::MiddleEast:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::MiddleEast; break;
			case EGeoGroups::NorthernAfrica:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::NorthernAfrica; break;
			case EGeoGroups::NorthernAmerica:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::NorthernAmerica; break;
			case EGeoGroups::PacificIslands:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::PacificIslands; break;
			case EGeoGroups::SouthernAfrica:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::SouthernAfrica; break;
			case EGeoGroups::SouthernAmerica:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::SouthernAmerica; break;
			case EGeoGroups::SouthernOceania:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::SouthernOceania; break;
			case EGeoGroups::WesternAfrica:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::WesternAfrica; break;
			case EGeoGroups::WesternEurope:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::WesternEurope; break;
			}
		}
	}
	else if (l_iNbCountriesPossible > 40)
	{
		in_pConditions[ETreatyConditions::RelationsNotAtWar] = EConditionStatusRelation::NotAtWar;
		if(l_pCapitalInfo)
		{
			switch(l_Continent)
			{
			case EContinent::Africa:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::Africa; break;				
			case EContinent::America:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::America; break;
			case EContinent::Asia:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::Asia; break;
			case EContinent::Europe:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::Europe; break;
			case EContinent::Oceania:
				in_pConditions[ETreatyConditions::GeographicProximity] = EConditionStatusGeo::Oceania; break;
			}
		}
	}		
}

UINT32 GAI::FindBestTargetForNextMission(UINT32 in_iCountryID)
{
	ENTITY_ID l_iTarget = 0;
	REAL32 l_fMinimumRelation = FLT_MAX;	
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();

	// Return the country with lowest relations
   for(ENTITY_ID i=1; i <= l_iNbCountry; i++)
	{
		if (i == in_iCountryID)
			continue;
		if(!g_ServerDAL.CountryIsValid(i))
			continue;		

		REAL32 l_fRelations = g_ServerDAL.RelationBetweenCountries(in_iCountryID,i);
		if((l_fRelations < l_fMinimumRelation) && 			
			l_fRelations < SP2::c_fRelationsEnnemy)
		{
			l_fRelations = l_fMinimumRelation;
			l_iTarget = i;
		}
	}
	return l_iTarget;
}

GCovertActionCell* GAI::FindBestCellForJob(UINT32 in_iCountryDoingTheAttack, UINT32 in_iTarget)
{
	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryDoingTheAttack);
	vector<GCovertActionCell>& l_vCells =  l_pCountryData->CovertActionCells();
	GCovertActionCell* l_pCellToExecute = NULL;
	for(UINT32 i=0; i<l_vCells.size(); i++)
	{
		ECovertActionsCellState::Enum l_eState = (ECovertActionsCellState::Enum)l_vCells[i].ActualState();
		UINT32 l_iAssignedCountry = l_vCells[i].AssignedCountry();
		if(l_vCells[i].SubsequentStates().size())
			continue;
		if(l_eState == ECovertActionsCellState::Active && l_iAssignedCountry == in_iTarget)
		{
			l_pCellToExecute = &l_vCells[i];
			break;
		}
		else if(l_eState == ECovertActionsCellState::Dormant && l_iAssignedCountry == in_iTarget && !l_pCellToExecute)
			l_pCellToExecute = &l_vCells[i];
		else if(l_eState == ECovertActionsCellState::Active && !l_pCellToExecute)
			l_pCellToExecute = &l_vCells[i];
		else if(l_eState == ECovertActionsCellState::Dormant && !l_pCellToExecute)
			l_pCellToExecute = &l_vCells[i];
	}
	return l_pCellToExecute;
}

void GAI::HandleTradeOffer(GTradeData& in_Trade)
{
	UINT32 in_iCountryID = in_Trade.GetTradeDataFromSide(false).CountryId();
	//The trade is pending, so let's see if the country on side B accepts it or not.
	if( in_Trade.EvaluateTrade(false) > ETradeEvaluationResults::Fair )
	{
		if(in_Trade.GetTradeDataFromSide(false).GetRegions().size() ==
			g_ServerDAL.CountryPoliticalControl(in_Trade.GetTradeDataFromSide(false).CountryId()).size() &&
			in_Trade.GetTradeDataFromSide(true).GetRegions().size() == 0	)
		{
			//Will not trade all of its political regions at the same time
			g_ServerDCL.TradeRefuse(in_Trade);
		}
		else
		{
			if(in_Trade.GetTradeDataFromSide(false).Money() + g_ServerDAL.CountryData(in_iCountryID)->BudgetExpenseDebt() > 
				(g_ServerDAL.CountryData(in_iCountryID)->BudgetRevenues() * 0.9f))
				g_ServerDCL.TradeRefuse(in_Trade);
			else
				g_ServerDCL.TradeAccept(in_Trade);
		}			
	}
	else
	{
		g_ServerDCL.TradeRefuse(in_Trade);
	}
}