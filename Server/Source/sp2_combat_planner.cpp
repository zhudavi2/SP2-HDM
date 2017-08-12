/**************************************************************
*
* sp2_combat_planner.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#include "golem_pch.hpp"

GCombatPlanner& GCombatPlanner::Instance()
{
   static GCombatPlanner l_Instance;
   return l_Instance;
}

GCombatPlanner::GCombatPlanner()
{	
	m_QuickRandom.Seed(clock());
	m_fNextIteration = 0.0f;
	m_MilitaryPlan = NULL;
}

GCombatPlanner::~GCombatPlanner()
{
	SAFE_DELETE_ARRAY(m_MilitaryPlan);
}

void GCombatPlanner::Init()
{
   g_ServerDAL.DataManager()->RegisterNode(this);
}

void GCombatPlanner::Iterate(UINT32 in_iCountryID)
{
	m_fTotalUnitsValue = g_ServerDAL.TotalUnitsValue(in_iCountryID);
	m_vListOfEnnemies.clear();
	g_ServerDAL.IsAtWarWith(in_iCountryID,m_vListOfEnnemies);
	
	IterateWaitingNukes();

		
	GAdvisor* l_pAdvisor = g_ServerDAL.CountryData(in_iCountryID)->Advisor();	

   if( (!l_pAdvisor || l_pAdvisor->MilitaryStatus() != EAdvisorStatus::Manual) )
	{		
		if(m_vListOfEnnemies.size() > 0)
		{	
			if(!l_pAdvisor || l_pAdvisor->MilitaryStatus() == EAdvisorStatus::Automatic)
				GAI::PossiblyLaunchNukeInSelfDefense(in_iCountryID);

			if(!m_vPlansBuilt[in_iCountryID]) 
			{
				//Must choose which regions he wants to conquer
				BuildMilitaryPlan(in_iCountryID);
			}	

			const set<UINT32>& l_UnitGroups = g_Joshua.UnitManager().CountryUnitGroups(in_iCountryID);

			//Find the countries which are on its side in this war
			set<UINT32> l_AlliesInWar;
			for(set<UINT32>::iterator it = m_vListOfEnnemies.begin();
				it != m_vListOfEnnemies.end(); it++)
			{
				set<UINT32> l_Enemies;
				g_ServerDAL.IsAtWarWith(*it,l_Enemies);
				l_AlliesInWar.insert(l_Enemies.begin(),l_Enemies.end());
			}	

			//Fill the set of unit group pointers
			hash_set<SP2::GUnitGroup*> l_AlliedUnitGroups;
			{
				//For each of our unit groups
				for(set<UINT32>::const_iterator l_OurUnitsIt =  l_UnitGroups.begin();
														l_OurUnitsIt != l_UnitGroups.end();
														l_OurUnitsIt++)
				{
					l_AlliedUnitGroups.insert((SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_OurUnitsIt));
				}

				for(set<UINT32>::const_iterator  l_CountryAllies = l_AlliesInWar.begin();
															l_CountryAllies != l_AlliesInWar.end();
															l_CountryAllies++)
				{
					//Fetch the unit group for the ally
					const set<UINT32>& l_vCountryUnitGroups = g_Joshua.UnitManager().CountryUnitGroups(*l_CountryAllies);
					//For each group, add it to the allied unit group list
					for(set<UINT32>::const_iterator l_GroupIt = l_vCountryUnitGroups.begin();
															l_GroupIt != l_vCountryUnitGroups.end();
															l_GroupIt++)
					{
						l_AlliedUnitGroups.insert((SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_GroupIt));
					}
				}
			}//end fill the set of unit group pointers


			FollowPlan(in_iCountryID,&m_MilitaryPlan[in_iCountryID],l_AlliesInWar,l_AlliedUnitGroups);
			IterateNavalUnits(in_iCountryID,m_vListOfEnnemies,l_AlliedUnitGroups);
		}
	}  
}

/*!
 * @return an int number between 0 and in_iRangeOfRandom (excluded)
 **/

UINT32 GCombatPlanner::GetRandom(UINT32 in_iRangeOfRandom)
{
	return (UINT32)m_QuickRandom.RandomReal((REAL32)in_iRangeOfRandom);
}

void GCombatPlanner::AdjustMilitaryPlan(ENTITY_ID in_iAttackingCountry)
{
	BuildMilitaryPlan(in_iAttackingCountry);	
}

UINT32 GCombatPlanner::FollowPlanInsideDefensiveRegion(UINT32 in_iRegionID, UINT32 in_iAttackingCountry, bool in_bHostile, const hash_set<SP2::GUnitGroup*>& in_AlliedUnitGroups)
{
	bool l_bFoundArena = false;	

	//If the region is controlled by a hostile nation, defend it
	if(in_bHostile)	
	{
		return FindBestAttackers(in_iAttackingCountry,in_iRegionID,true,in_AlliedUnitGroups);
	}
	else
	{
		for(stdext::hash_map<UINT32, SDK::Combat::GArenaInfo*>::const_iterator l_It = g_Joshua.UnitManager().ArenaInfos().begin();
			(l_It != g_Joshua.UnitManager().ArenaInfos().end() && !l_bFoundArena);
			l_It++)
		{
			SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)l_It->second;
			if(l_pArenaInfo && l_pArenaInfo->m_iRegionID == in_iRegionID)
			{
				//A fight has been found inside that region, let's check if it contains ennemies
				for(set<UINT32>::const_iterator l_Country = m_vListOfEnnemies.begin(); 
					l_Country != m_vListOfEnnemies.end();
					l_Country++)
				{
					SP2::GActorInfo* l_pActor = l_pArenaInfo->Actor(*l_Country);
					if(l_pActor)
					{
						l_bFoundArena = true;
						break;
					}
				}
			}
			
		}
		if(l_bFoundArena)
		{
			return FindBestAttackers(in_iAttackingCountry,in_iRegionID,true,in_AlliedUnitGroups);
		}
	}

	
	//If we have not found possible targets yet, check if ennemies are inside that region
	vector<UINT32> l_vEnnemyUnitGroups;
	for(set<UINT32>::const_iterator l_Itr = m_vListOfEnnemies.begin();
		(l_Itr != m_vListOfEnnemies.end()) && (l_vEnnemyUnitGroups.size() == 0);
		l_Itr++)
	{
		g_ServerDCL.UnitMover().UnitGroupsInsideRegion(in_iRegionID,l_vEnnemyUnitGroups,*l_Itr);		
	}	
	if(l_vEnnemyUnitGroups.size() > 0)
	{
		return FindBestAttackers(in_iAttackingCountry,in_iRegionID,true,in_AlliedUnitGroups);
	}			

	return 0;
}

void GCombatPlanner::FollowPlan(ENTITY_ID in_iAttackingCountry, 
										  GMilitaryPlan* in_pMilitaryPlan,
										  const set<UINT32>& in_AlliesInWar,
										  const hash_set<SP2::GUnitGroup*>& in_AlliedUnitGroups)
{	
	const UINT32 c_iNbRegionsToAlwaysDefend = 2;
	
	const vector<GRegionControl>& l_vRegionsControl = g_ServerDAL.RegionControlArray();
	const UINT8* l_Statuses = g_ServerDAL.CountryDiplomaticStatuses(in_iAttackingCountry);	
	UINT32 l_iNbUnitGroupsAttacking = 0;
	UINT32 l_iNbUnitGroupsDecided = 0;
	const set<UINT32>& l_UnitGroups = g_Joshua.UnitManager().CountryUnitGroups(in_iAttackingCountry);
	for(set<UINT32>::const_iterator l_Itr = l_UnitGroups.begin();
		 l_Itr != l_UnitGroups.end();
		 l_Itr++)
	{
		SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_Itr);
		if(StatusReadyForMovement(l_pUnitGroup->Status()) && !l_pUnitGroup->IsNaval())			
			l_iNbUnitGroupsAttacking++;
	}
	
	//Check which regions he must self defend. (its own regions)
	const multimap<REAL32,UINT32>& l_RegionsToSelfDefend = in_pMilitaryPlan->RegionsToSelfDefend();

	//Check which regions he must defend. 
	const multimap<REAL32,UINT32>& l_RegionsToDefend = in_pMilitaryPlan->RegionsToDefend();	

	//Check the regions he must attack
	const multimap<REAL32,UINT32>& l_RegionsToAttack = in_pMilitaryPlan->RegionsToAttack();	

	//Go through the own country region, and defend them first
	multimap<REAL32,UINT32>::const_iterator l_RegionSelfDefItr = l_RegionsToSelfDefend.begin();				
	while(l_RegionSelfDefItr != l_RegionsToSelfDefend.end() && l_iNbUnitGroupsDecided < l_iNbUnitGroupsAttacking)
	{			
		l_iNbUnitGroupsDecided += FollowPlanInsideDefensiveRegion(l_RegionSelfDefItr->second,in_iAttackingCountry,(EDiplomaticStatus::Enum)l_Statuses[l_vRegionsControl[l_RegionSelfDefItr->second].m_iMilitary] == EDiplomaticStatus::Hostile,in_AlliedUnitGroups);
		l_RegionSelfDefItr++;
	}

	//Send X units as defense unit, just in case, as reserve
	l_RegionSelfDefItr = l_RegionsToSelfDefend.begin();
	UINT32 l_iNbRegionsPassedThrough = 0;
	while(l_iNbRegionsPassedThrough < min(c_iNbRegionsToAlwaysDefend,l_iNbUnitGroupsAttacking) && l_RegionSelfDefItr != l_RegionsToSelfDefend.end())
	{
		l_iNbUnitGroupsDecided += FindBestAttackers(in_iAttackingCountry,l_RegionSelfDefItr->second,true,in_AlliedUnitGroups);
		l_iNbRegionsPassedThrough++;
		l_RegionSelfDefItr++;
	}

	//Go through your ally region list. Decide which unit group should move towards those regions.
	multimap<REAL32,UINT32>::const_iterator l_RegionDefItr = l_RegionsToDefend.begin();				
	while(l_RegionDefItr != l_RegionsToDefend.end() && l_iNbUnitGroupsDecided < l_iNbUnitGroupsAttacking)
	{			
		l_iNbUnitGroupsDecided += FollowPlanInsideDefensiveRegion(l_RegionDefItr->second,in_iAttackingCountry,(EDiplomaticStatus::Enum)l_Statuses[l_vRegionsControl[l_RegionDefItr->second].m_iMilitary] == EDiplomaticStatus::Hostile,in_AlliedUnitGroups);
		l_RegionDefItr++;
	}	

	//Go through the ennemy region list. Decide which unit group should move towards those regions.
	multimap<REAL32,UINT32>::const_iterator l_RegionAtkItr = l_RegionsToAttack.begin();				
	while(l_RegionAtkItr != l_RegionsToAttack.end() && l_iNbUnitGroupsDecided < l_iNbUnitGroupsAttacking)
	{
		UINT32 l_iRegionID = l_RegionAtkItr->second;
		UINT32 l_iCountryOwner = l_vRegionsControl[l_iRegionID].m_iMilitary;

		//If a country is fighting an offensive war against that country, he'll try to its regions
		if(m_vFightingOffensive[in_iAttackingCountry][l_iCountryOwner])
		{
			//If the region is controlled by a hostile nation, attack it
			if((EDiplomaticStatus::Enum)l_Statuses[l_iCountryOwner] == EDiplomaticStatus::Hostile)
				l_iNbUnitGroupsDecided += FindBestAttackers(in_iAttackingCountry,l_iRegionID,false,in_AlliedUnitGroups);
		}
		
		l_RegionAtkItr++;
	}
}

UINT32 GCombatPlanner::FindBestAttackers(ENTITY_ID in_iCountryID, UINT32 in_iRegionID, bool in_bDefensive, const hash_set<SP2::GUnitGroup*>& in_AlliedUnitGroups)
{	
    GUnitMover& l_UnitMover = g_ServerDCL.UnitMover();

	SP2::GUnitGroupEx* l_pGroup = g_ServerDCL.UnitMover().ProductionQueueUnitGroups()[in_iCountryID-1];
   if(!l_pGroup)
      return 0;

	const vector<GRegionControl>& l_vRegionsControl = g_ServerDAL.RegionControlArray();
	const UINT8* l_Statuses = g_ServerDAL.CountryDiplomaticStatuses(in_iCountryID);	
	const vector<UINT32>& l_vRegionCities = g_ServerDAL.CitiesInRegion(in_iRegionID);

	UINT32 l_iNbUnitGroupsMoved = 0;

   //First, find the value of ennemy units inside that region
	//Then, fill l_EnnemyGroupList with the ID's of every ennemy unit groups inside that region
	//! \todo Instead of looking for units directly on that region, check for units near it (sphere of influence)
	REAL64 l_fTotalValueEnnemies = 0.f;
	set<UINT32> l_EnnemyGroupList;
	vector<UINT32> l_vEnnemyUnitGroups;
	for(set<UINT32>::const_iterator l_Itr = m_vListOfEnnemies.begin();
		 l_Itr != m_vListOfEnnemies.end();
		 l_Itr++)
	{
		g_ServerDCL.UnitMover().UnitGroupsInsideRegion(in_iRegionID,l_vEnnemyUnitGroups,*l_Itr);		
	}	
	SDK::Combat::GUnitGroup* l_pEnnemyUnitGroup;
	for(vector<UINT32>::iterator l_GroupItr = l_vEnnemyUnitGroups.begin();
		 l_GroupItr != l_vEnnemyUnitGroups.end();
		 l_GroupItr++)
	{
		l_pEnnemyUnitGroup = g_Joshua.UnitManager().UnitGroup(*l_GroupItr);
		l_fTotalValueEnnemies += ((SP2::GUnitGroup*)l_pEnnemyUnitGroup)->Value();
		l_EnnemyGroupList.insert(*l_GroupItr);
	}	

	//Secondly, we cycle through the available unit group to check which one is available and strong enough to get that region
	//Points are reduced if the country is far away
	multimap<REAL32,UINT32> l_PossibleGroups;
	const set<UINT32>& l_UnitGroups = g_Joshua.UnitManager().CountryUnitGroups(in_iCountryID);	
	REAL32 l_fTempPower = 0.f;
	REAL32 l_fDistance = 0.f;
	bool l_bMoving = false;	
	vector<SDK::Combat::GUnitGroup*> l_GroupsAlreadyInsideRegion;

	for(set<UINT32>::const_iterator l_GroupItr = l_UnitGroups.begin();
		l_GroupItr != l_UnitGroups.end();
		l_GroupItr++)	
	{
		SDK::Combat::GUnitGroup* l_pUnitGroup = g_Joshua.UnitManager().UnitGroup(*l_GroupItr);		
		l_bMoving = false;

		//If the group is the constructor group, skip it
		if(l_pGroup->Id() == l_pUnitGroup->Id())
			continue;
		
		//If it is changing status, we do not choose it.
		if(((SP2::GUnitGroup*)l_pUnitGroup)->Status() != ((SP2::GUnitGroup*)l_pUnitGroup)->NextStatus())
			continue;

		//If the unit group is a naval one, let's skip it also
		if(((SP2::GUnitGroup*)l_pUnitGroup)->IsNaval())
			continue;

		//If the move from the unit group location to the region is impossible, let's jump to the next unit group
		if (!(StatusReadyForMovement(((SP2::GUnitGroup*)l_pUnitGroup)->Status()) && 
			 g_ServerDCL.MovePossible(in_iCountryID,((GUnitGroupEx*)l_pUnitGroup)->m_iRegionID,in_iRegionID))) 
			 continue;
			 

		//If the unit group is not currently trying to change status,
		// or not in a fight,
		// then Calculate its relative power		
      if(RegionNeedsReinforcement(l_pUnitGroup,in_iRegionID,in_AlliedUnitGroups,l_fTotalValueEnnemies) )
		{						
			//If the unit group is moving to a hostile region, do not make it a possibility to move, except if the region belongs to the attacking country
			if(((SP2::GUnitGroup*)l_pUnitGroup)->Path().Points().size() > 0)
			{
				l_bMoving = true;
				if( l_Statuses[l_vRegionsControl[((SP2::GUnitGroup*)l_pUnitGroup)->Path().Points().back().m_iRegionID].m_iMilitary] 
						== EDiplomaticStatus::Hostile )
				{
					if(!in_bDefensive)
					{
						l_iNbUnitGroupsMoved++;
						continue;
					}	
				}					
			}
			
			if(l_fTotalValueEnnemies == 0.f)
			{
				l_fTempPower = 1.f;				
				if(l_vRegionCities.begin() == l_vRegionCities.end())
				{
					//If no cities are inside, the only other choice to get the distance
					//between the unit group and the region is with this method
					l_fDistance = g_ServerDAL.DistanceBetween2PointsLatLong(l_pUnitGroup->Position(),
						g_ServerDCL.UnitMover().RegionLocation(in_iRegionID));
				}
				else
				{
					//If there are cities inside that regions, then let's find the distance
					//! \todo Find the closest city, not a random one
					l_fDistance = g_ServerDAL.DistanceBetween2PointsLatLong(l_pUnitGroup->Position(),
                  g_ServerDAL.City(l_vRegionCities[0] )->m_Position);
				}
			}
			else
			{
				//Power is the ratio between that unit and the units presents inside
				l_fTempPower = (REAL32)(((SP2::GUnitGroup*)l_pUnitGroup)->Value() / l_fTotalValueEnnemies);
				//Distance between this unit and an enemy inside that region
				//! \todo Find the closest ennemy, not a random one
				l_fDistance = g_ServerDAL.DistanceBetween2PointsLatLong(l_pUnitGroup->Position(),
						g_Joshua.UnitManager().UnitGroup((*l_EnnemyGroupList.begin()))->Position());
			}

			//Let's put the group power between 0 and 1
			l_fTempPower = min(1.f,l_fTempPower);
			//Let's put that distance between 0 and 1
			l_fDistance /= (EARTH_CIRCONFERENCE_AT_EQUATOR / 2.f);
			l_fDistance = 1.f - l_fDistance;
			//The final result is 50% strength and 50% distance
			l_fTempPower = (0.5f * l_fTempPower) + (0.5f * l_fDistance);			
			//If the unit is moving, it's 25% less likely to be chosen
			if(l_bMoving)
				l_fTempPower *= 0.75f;

			l_PossibleGroups.insert(make_pair(-l_fTempPower,*l_GroupItr));			
		}
		else
		{
			GUnitGroupEx* l_pUnitEx = (GUnitGroupEx*)l_pUnitGroup;
			if(l_pUnitEx->m_iRegionID == in_iRegionID)
				l_GroupsAlreadyInsideRegion.push_back(l_pUnitGroup);
		}
	}	

	if(l_PossibleGroups.size() == 0 && (EDiplomaticStatus::Enum)l_Statuses[l_vRegionsControl[in_iRegionID].m_iMilitary] == EDiplomaticStatus::Hostile)
	{
		//Maybe a unit is already inside that region
		if(l_GroupsAlreadyInsideRegion.size() > 0)
		{
			g_ServerDCL.UnitMover().MoveUnits(l_GroupsAlreadyInsideRegion,l_GroupsAlreadyInsideRegion[0]->Position());
		}
		return l_GroupsAlreadyInsideRegion.size();
	}

	REAL32 l_fTotalPower = 0.f;
	vector<SDK::Combat::GUnitGroup*> l_MovingGroups;
	set<UINT32> l_GroupsToMove;
	GVector2D<REAL32> l_Destination;	

	//Now, we actually move the necessary units towards that region
	for(multimap<REAL32,UINT32>::const_iterator l_Itr = l_PossibleGroups.begin();
		l_Itr != l_PossibleGroups.end();
		l_Itr++)
	{
		SDK::Combat::GUnitGroup* l_pUnitGroup = g_Joshua.UnitManager().UnitGroup(l_Itr->second);
		if(!l_pUnitGroup)
			continue;			
		l_GroupsToMove.insert(l_pUnitGroup->Id());		
		l_iNbUnitGroupsMoved++;
		if(l_fTotalValueEnnemies == 0.f)
		{
			//If there are no cities, let the unit move towards one of the vertex center
			if(!l_vRegionCities.size())
			{
				l_Destination = l_UnitMover.RegionLocation(in_iRegionID);
			}
			else
			{
				//Find the closest city
				if(!l_UnitMover.FindClosestCityLocation(l_pUnitGroup->Position(),in_iRegionID,l_Destination))
					continue;
			}
			break;
		}
		else
		{
			//Find the closest ennemy unit group
			if(!FindClosestEnnemyLocation(l_pUnitGroup->Position(),l_EnnemyGroupList,l_Destination))
				continue;	
			l_fTotalPower += (REAL32)(((SP2::GUnitGroup*)l_pUnitGroup)->Value() / l_fTotalValueEnnemies);
			
		}				
		if(l_fTotalPower > 1.5f)
			break;
	}
	if(l_GroupsToMove.size() != 0)
	{
        if(GDZDebug::LogEnabled())
        {
            const Map::GMultiface* const l_pMF = g_ServerDAL.CountryView()->Locate(l_Destination.x, l_Destination.y);
			if(l_pMF == nullptr)
			{
				const GRegionEx* l_pRegion = g_ServerDAL.GetGRegion(in_iRegionID);
				GDZLOG(EDZLogLevel::Error, l_pRegion->Name() + L" should contain or be near point (" + GString(l_Destination.x) + L", " + GString(l_Destination.y) + L") but can't locate Multiface");

				const GVector2D<REAL32>& l_RegionLocation = l_UnitMover.RegionLocation(in_iRegionID);
				GDZLOG(EDZLogLevel::Error, L"Region location: " + GString(l_RegionLocation.x) + L", " + GString(l_RegionLocation.y));
				
                if(!l_vRegionCities.empty())
                {
                    GDZLOG(EDZLogLevel::Error, L"Cities in region:");
                    for(auto it = l_vRegionCities.cbegin(); it < l_vRegionCities.cend(); ++it)
                    {
                        const SCityInfo* const l_pCity = g_ServerDAL.City(*it);
                        const GString l_sName = g_ServerDAL.GetString(l_pCity->m_iNameStringID);
                        const GVector2D<REAL32>& l_CityLocation = l_pCity->m_Position;
                        GDZLOG(EDZLogLevel::Error, l_sName + L" (" + GString(*it) + L"): " + GString(l_CityLocation.x) + L", " + GString(l_CityLocation.y));
                    }
                }
                else
                    GDZLOG(EDZLogLevel::Error, L"No cities in region");

                if(!l_vEnnemyUnitGroups.empty())
                {
                    GDZLOG(EDZLogLevel::Error, L"Enemy groups nearby:");
                    for(auto it = l_vEnnemyUnitGroups.cbegin(); it < l_vEnnemyUnitGroups.cend(); ++it)
                    {
                        const SDK::Combat::GUnitGroup* const l_pGroup = g_Joshua.UnitManager().UnitGroup(*it);
                        const GString l_sCountryName = g_ServerDAL.CountryData(l_pGroup->OwnerId())->NameAndIDForLog();
                        const GVector2D<REAL32>& l_GroupLocation = l_pGroup->Position();
                        GDZLOG(EDZLogLevel::Error, L"Group " + GString(*it) + L" of " + l_sCountryName + L": " + GString(l_GroupLocation.x) + L", " + GString(l_GroupLocation.y));
                    }
                }
                else
                    GDZLOG(EDZLogLevel::Error, L"No enemy groups nearby");
			}
        }

		for(set<UINT32>::iterator it = l_GroupsToMove.begin();
			 it != l_GroupsToMove.end(); it++)
		{
			SDK::Combat::GUnitGroup* l_pUnitGroup = g_Joshua.UnitManager().UnitGroup(*it);
			if(l_pUnitGroup->Position() != l_Destination)
				l_MovingGroups.push_back(l_pUnitGroup);
		}
		if(l_MovingGroups.size())
		{
			g_ServerDCL.UnitMover().MoveUnits(l_MovingGroups,l_Destination);
		}
	}

	return l_iNbUnitGroupsMoved;
}

bool GCombatPlanner::StatusReadyForMovement(SP2::EMilitaryStatus::Enum l_iStatus)
{
	if (l_iStatus == SP2::EMilitaryStatus::Fortified ||
		 l_iStatus == SP2::EMilitaryStatus::Parked ||
		 l_iStatus == SP2::EMilitaryStatus::Ready)
	 return true;

	return false;
		 
}

bool GCombatPlanner::RegionNeedsReinforcement(const SDK::Combat::GUnitGroup* in_pAttackingGroup, UINT32 in_iRegionID,const hash_set<SP2::GUnitGroup*>& in_vAlliesGroup, REAL64 in_fTotalEnnemyValue)
{

	//If the group is already inside that region, we say it is needed
	if(((GUnitGroupEx*)in_pAttackingGroup)->m_iRegionID == in_iRegionID)
		return true;
	
	REAL64 l_fTotalAllyValue = 0.f;

	//Now, let's calculate the force of allies troups inside, or moving towards that region
	//! \todo: We do not check allies unit groups. We only check the country group right now. We should add the allies groups to in_vAlliesGroup
   for(hash_set<SP2::GUnitGroup*>::const_iterator l_GroupItr = in_vAlliesGroup.begin();
		l_GroupItr != in_vAlliesGroup.end();
		l_GroupItr++)	
	{
		SDK::Combat::GUnitGroup* l_pUnitGroup = *l_GroupItr;		
		//If the unit is in combat, or is not moving, or is moving inside that region, it's considered to be inside		
		const vector<SP2::GUnitPathPoint>& l_vPoints = ((SP2::GUnitGroup*)l_pUnitGroup)->Path().Points();
		if(l_vPoints.size() == 0)
		{
			//Units is not moving. So if it is not inside that region, we do not count it.
			//! \todo Check if is changing status. It might be going there after all			
			if(((GUnitGroupEx*)l_pUnitGroup)->m_iRegionID == in_iRegionID)
				l_fTotalAllyValue += ((SP2::GUnitGroup*)l_pUnitGroup)->Value();
		}
		else
		{
			//The unit is moving. Let's check its destination. If it is towards that region, let's count that group too.						
			UINT32 l_iRegionDestination = l_vPoints.back().m_iRegionID;
			if(l_iRegionDestination == in_iRegionID)
				l_fTotalAllyValue += ((SP2::GUnitGroup*)l_pUnitGroup)->Value();

		}
		if(l_fTotalAllyValue > 1.5f * in_fTotalEnnemyValue)
				break;
	}

	if(l_fTotalAllyValue > 1.5f * in_fTotalEnnemyValue) 
		return false;

	return true;
}

/*!
 * @param	in_GroupLocation:		Compare which ennemy group is closest to that location
 *	@param	in_vEnnemyGroupList:	The list of ennemy groups inside that region
 *	@param	out_EnnemyLocation:	The city location will be stored there
 *
 *	@return	true if a city is found
 **/
bool GCombatPlanner::FindClosestEnnemyLocation(const GVector2D<REAL32>& in_GroupLocation, 
															set<UINT32>& in_vEnnemyGroupList,
															GVector2D<REAL32>& out_EnnemyLocation)
{
	REAL32 l_fMinDistance = -1.f;
	REAL32 l_fDistance = 0.;
	SDK::Combat::GUnitGroup* l_pClosestEnnemy = NULL;

	for(set<UINT32>::const_iterator l_Itr = in_vEnnemyGroupList.begin();
		 l_Itr != in_vEnnemyGroupList.end();
		 l_Itr++)
	{	
		SDK::Combat::GUnitGroup* l_pUnitGroup = g_Joshua.UnitManager().UnitGroup(*l_Itr);
		if(!l_pUnitGroup)
			continue;		
		l_fDistance = g_ServerDAL.DistanceBetween2PointsLatLong(in_GroupLocation,
																					l_pUnitGroup->Position());
		if(l_fDistance < l_fMinDistance || l_fMinDistance == -1.f)
		{
			l_fMinDistance = l_fDistance;
			l_pClosestEnnemy = l_pUnitGroup;
		}			
	}
	if(l_fMinDistance != -1.f)
	{
		out_EnnemyLocation = l_pClosestEnnemy->Position();		
		return true;
	}

	//If no destination is found, default value is current location
	out_EnnemyLocation = in_GroupLocation;
	return false;

}

void GCombatPlanner::BuildMilitaryPlan(ENTITY_ID in_iAttackingCountry)
{	
	//Unit groups of attacking country
	const set<UINT32>& l_UnitGroups = g_Joshua.UnitManager().CountryUnitGroups(in_iAttackingCountry);	

	//Plan is being constructed
	m_vPlansBuilt[in_iAttackingCountry] = true;
	GMilitaryPlan* l_pMilitaryPlan = &m_MilitaryPlan[in_iAttackingCountry];
	l_pMilitaryPlan->ClearPlans();
	
	set<UINT32> l_vAllies;
	g_ServerDAL.CountryAllies(in_iAttackingCountry,l_vAllies);	

	//Before constructing the attacking plans, let's calculate the forces of ennemies nations, 
	//and allies nations
	REAL32 l_fAlliesForces = 0.f;
	REAL32 l_fEnnemyForces = 0.f;
	set<UINT32> l_vEnnemiesOfYourEnnemies;
		
	for(set<UINT32>::const_iterator l_Country = m_vListOfEnnemies.begin();
		 l_Country != m_vListOfEnnemies.end();
		 l_Country++)
	{		
		l_fEnnemyForces += g_ServerDAL.TotalUnitsValue(*l_Country);
		
		//Ennemies of your ennemies are your allies
		set<UINT32> l_Enemies;
		g_ServerDAL.IsAtWarWith(*l_Country,l_Enemies);
		l_vEnnemiesOfYourEnnemies.insert(l_Enemies.begin(),l_Enemies.end());
	}
	for(set<UINT32>::const_iterator l_Country = l_vEnnemiesOfYourEnnemies.begin();
		 l_Country != l_vEnnemiesOfYourEnnemies.end();
		 l_Country++)
	{
		l_fAlliesForces += g_ServerDAL.TotalUnitsValue(*l_Country);
	}

	l_vAllies.insert(l_vEnnemiesOfYourEnnemies.begin(),l_vEnnemiesOfYourEnnemies.end());

	//Iterate through the ennemy countries
	for(set<UINT32>::const_iterator l_Country = m_vListOfEnnemies.begin();
		 l_Country != m_vListOfEnnemies.end();
		 l_Country++)
	{
		UINT32 l_iEnnemy = *l_Country;
		
		//Military-controlled regions for that country
		const set<UINT32>& l_vRegionsMilitary = g_ServerDAL.CountryMilitaryControl(l_iEnnemy);

		//Check if this country is going to follow a defensive war, or an offensive war against this ennemy
		bool l_bOffensiveWar = false;				
		for(set<UINT32>::const_iterator l_RegionItr = l_vRegionsMilitary.begin();
			(l_RegionItr != l_vRegionsMilitary.end() &&  !l_bOffensiveWar);
			l_RegionItr++)
		{
			//To do so, check if atleast one unit group can reach at least one ennemy regions	
			for(set<UINT32>::const_iterator l_GroupItr = l_UnitGroups.begin();
			(l_GroupItr != l_UnitGroups.end() &&  !l_bOffensiveWar);
			l_GroupItr++)	
			{
				EMilitaryStatus::Enum l_iStatus = ((SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_GroupItr))->Status();
				if(!(l_iStatus == EMilitaryStatus::Attacking || l_iStatus == EMilitaryStatus::Parked
					|| l_iStatus == EMilitaryStatus::Moving || l_iStatus == EMilitaryStatus::Fortified
					|| l_iStatus == EMilitaryStatus::Ready))
					continue;
				GUnitGroupEx* l_pUnitGroup = (GUnitGroupEx*)g_Joshua.UnitManager().UnitGroup(*l_GroupItr);							
				if(g_ServerDCL.MoveHostilePossible(in_iAttackingCountry,l_pUnitGroup->m_iRegionID,*l_RegionItr))
					l_bOffensiveWar = true;
			}
		}
		
		if(l_bOffensiveWar)
		{
			//Iterate through the regions of that country, and add all their regions to your attack plan
			for(set<UINT32>::const_iterator l_RegionItr = l_vRegionsMilitary.begin();
			(l_RegionItr != l_vRegionsMilitary.end());
			l_RegionItr++)
			{
				UINT32 l_iRegionID = *l_RegionItr;

				//! \todo Better equation than only population!
				REAL32 l_fTempValue = (REAL32)g_ServerDAL.GetGRegion(l_iRegionID)->Population1565();
				
				//If the region is adjacent to the 2 countries, it has a higher possibility of being aimed by the AI
				if(g_ServerDAL.RegionTouchesCountry(l_iRegionID, in_iAttackingCountry))
					l_fTempValue *= 3.0f;

				//If the region originally belongs to this country, it has a higher possibility of being aimed
				if(g_ServerDAL.RegionControl(l_iRegionID).m_iPolitical == in_iAttackingCountry)					
					l_fTempValue *= 50000000.0f;

				//If it has historical claims, bonus too
				if(g_ServerDAL.GetGRegion(l_iRegionID)->HistoricalClaim() == in_iAttackingCountry)					
					l_fTempValue *= 50000000.0f;

				l_pMilitaryPlan->AddRegionToPlan(l_iRegionID,l_fTempValue);
			}
		}		
	}
	
	//Now that we added the regions to attack, let's add to the plan the regions to defend	
	//Add this country to that list
	l_vAllies.insert(in_iAttackingCountry);
	const UINT8* l_pDiplomaticStatusAttackingCountry = g_ServerDAL.CountryDiplomaticStatuses(in_iAttackingCountry);
	
	for(set<UINT32>::const_iterator l_Country = l_vAllies.begin();
		 l_Country != l_vAllies.end();
		 l_Country++)
	{
		UINT32 l_iAlly = *l_Country;

		REAL32 l_fBonusToRegions = 1.f;
		//If the country is allied, give bonus to all its regions
		if((EDiplomaticStatus::Enum)l_pDiplomaticStatusAttackingCountry[l_iAlly] == EDiplomaticStatus::Allied)
			l_fBonusToRegions = 5.f;		

		//Politically-controlled regions for that country
		const set<UINT32>& l_vRegionsPolitical = g_ServerDAL.CountryPoliticalControl(l_iAlly);
		
		//Iterate through the regions of that country, and add all their regions to your defense plan
		for(set<UINT32>::const_iterator l_RegionItr = l_vRegionsPolitical.begin();
		(l_RegionItr != l_vRegionsPolitical.end());
		l_RegionItr++)
		{			
			UINT32 l_iRegionID = *l_RegionItr;

			//! \todo Better equation than only population!
			REAL32 l_fTempValue = (REAL32)g_ServerDAL.GetGRegion(l_iRegionID)->Population1565();
			
			//If the region is adjacent to the 2 countries, it has a higher possibility of being aimed by the AI			
			if(g_ServerDAL.RegionTouchesCountry(l_iRegionID, in_iAttackingCountry))				
				l_fTempValue *= 5.0f;

			l_fTempValue *= l_fBonusToRegions; 

			if(in_iAttackingCountry == l_iAlly)
				l_pMilitaryPlan->AddRegionToSelfDefend(l_iRegionID,l_fTempValue);
			else
				l_pMilitaryPlan->AddRegionToDefend(l_iRegionID,l_fTempValue);
		}
	}	
}

void GCombatPlanner::MilitaryPlanExist(bool in_bExist, UINT32 in_iCountry)
{
	m_vPlansBuilt[in_iCountry] = in_bExist;	
}

void GCombatPlanner::AddNukeToWaitingList(UINT32 in_iCountrySource, 
							 						   UINT32 in_iCountryTarget,
														UINT32 in_iNewMegatons,
														ENuclearTargetType::Enum in_iType)
{
	bool l_bInserted = false;
	//check if that target country exit first.
	for(UINT32 i=0; i<m_vNukesToBeLaunched[in_iCountrySource].size(); i++)
	{
		if(m_vNukesToBeLaunched[in_iCountrySource][i].TargetCountry == in_iCountryTarget)
		{
			m_vNukesToBeLaunched[in_iCountrySource][i].Megatons += in_iNewMegatons;
			m_vNukesToBeLaunched[in_iCountrySource][i].Type = in_iType;
			l_bInserted = true;
		}
	}
	if(!l_bInserted)
	{
		GMegatonsWaiting l_Megatons;
		l_Megatons.Megatons = in_iNewMegatons;
		l_Megatons.TargetCountry = in_iCountryTarget;
		l_Megatons.Type = in_iType;
		m_vNukesToBeLaunched[in_iCountrySource].push_back(l_Megatons);
	}	
}

void GCombatPlanner::IterateWaitingNukes()
{
	for(UINT32 i=1; i<m_vNukesToBeLaunched.size(); i++)
	{
		if(m_vNukesToBeLaunched[i].size() == 0)
			continue;
		
		for(UINT32 j=0; j<m_vNukesToBeLaunched[i].size(); j++)
		{				
			UINT32 l_iNbMegatons = m_vNukesToBeLaunched[i][j].Megatons;
			UINT32 l_iTarget = m_vNukesToBeLaunched[i][j].TargetCountry;			
			ENuclearTargetType::Enum l_iType = m_vNukesToBeLaunched[i][j].Type;
			UINT32 l_iCurMegatons = 0;


         //Fetch the complete missile list
         vector<SP2::GNuclearMissile> l_vMissiles         = g_ServerDAL.CountryData(i)->GroundBasedNuclearMissiles();
         vector<SP2::GNuclearMissile> l_vSubBasedMissiles = g_ServerDAL.CountryData(i)->SubmarineBasedNuclearMissiles();
         l_vMissiles.insert(l_vMissiles.begin(),l_vSubBasedMissiles.begin(),l_vSubBasedMissiles.end());

         vector< pair<GNukeTarget, vector<GNuclearMissile> > > l_TargetMissilesList = g_ServerDCL.SelectNuclearTargetsAndAssignMissiles(l_vMissiles,l_iTarget,l_iType,l_vMissiles.size());

			for(UINT32 k=0;( k<l_TargetMissilesList.size()  && l_iCurMegatons < l_iNbMegatons); k++)
			{
				//Get it's list of missile in range
				vector<GNuclearMissile>* l_pMissileList = &(l_TargetMissilesList[k].second);

            vector<GNukeLaunchRequest> l_vLaunchRequest;

            for(UINT32 l=0; (l<l_pMissileList->size() && l_iCurMegatons < l_iNbMegatons) ; l++)
				{
					GNukeLaunchRequest l_Request;
					l_Request.m_iAttackerID = i;
					l_Request.m_iTargetID = l_iTarget;
					l_Request.m_iMissileID = l_pMissileList->at(l).m_iID;

               l_iCurMegatons += l_pMissileList->at(l).Megatons();					

					l_Request.m_TargetCoordinates = l_TargetMissilesList[k].first.m_Position;
					l_Request.m_TargetType = l_iType;

               l_vLaunchRequest.push_back(l_Request);
				}
            g_ServerDCL.LaunchNuclearAttacks(l_vLaunchRequest);
			}										
		}
		m_vNukesToBeLaunched[i].clear();
	}
	
}

void GCombatPlanner::WarOffensiveStatus(UINT32 in_iCountrySource,
													 UINT32 in_iCountryTarget,
													 bool in_bOffensive)
{
	m_vFightingOffensive[in_iCountrySource][in_iCountryTarget] = in_bOffensive;
}

bool GCombatPlanner::WarOffensiveStatus(UINT32 in_iCountrySource,
													 UINT32 in_iCountryTarget)
{
	return m_vFightingOffensive[in_iCountrySource][in_iCountryTarget];
}

void GCombatPlanner::BombardEnemy(UINT32 in_iCountryID, 
											 const set<UINT32>& in_ListOfEnnemies,
											 const vector<SP2::GUnitGroup*>& in_vNavalGroups)
{	
	multimap<INT64,GRegion*> l_RegionsToBombards;
	//Fill up the regions to bombard
	{
		for(set<UINT32>::const_iterator it = in_ListOfEnnemies.begin();
			it != in_ListOfEnnemies.end(); it++)
		{
			if(m_vFightingOffensive[in_iCountryID][*it])
			{
				vector<UINT32> l_vRegions = g_ServerDAL.BombardableRegions(*it);
				for(UINT32 i=0; i < l_vRegions.size(); i++)
				{
					GRegion* l_pRegion = g_ServerDAL.GetGRegion(l_vRegions[i]);
					l_RegionsToBombards.insert(make_pair<INT64,GRegion*>(-l_pRegion->Population(),l_pRegion));
				}
			}
		}
	}

	INT32 l_iIndexNavalGroups = 0;

	//Assign which region to the possible naval groups
	for(multimap<INT64,GRegion*>::iterator it = l_RegionsToBombards.begin();
		 it != l_RegionsToBombards.end() && l_iIndexNavalGroups < (INT32)in_vNavalGroups.size(); it++)
	{
		SP2::GUnitGroupEx* l_pUnitEx = (SP2::GUnitGroupEx*)in_vNavalGroups[l_iIndexNavalGroups];
		//Don't move a unit group if it is bombarding
		if(!(l_pUnitEx->m_ActionAtDest & EMilitaryAction::BombardRegion))
		{
			vector<SDK::Combat::GUnitGroup*> l_vGroupsToBombard;
			l_vGroupsToBombard.push_back( (SDK::Combat::GUnitGroup*)in_vNavalGroups[l_iIndexNavalGroups] );		
			g_ServerDCL.NavalUnitsBombardRegion(l_vGroupsToBombard,it->second->Id());
		}

		l_iIndexNavalGroups++;
	}
}

void GCombatPlanner::IterateNavalUnits(UINT32 in_iCountryID, 
													const set<UINT32>& in_ListOfEnnemies,
													const hash_set<SP2::GUnitGroup*>& in_AlliedUnitGroups)
{
	vector<SP2::GUnitGroup*> l_vNavalGroups;
	multimap<REAL64, SP2::GUnitGroup* > l_EnemiesNavalGroups;
	
	//Find the naval unit groups of country
	{
		const set<UINT32>& l_UnitGroupSet = g_Joshua.UnitManager().CountryUnitGroups(in_iCountryID);
		for(set<UINT32>::const_iterator it = l_UnitGroupSet.begin();
			it != l_UnitGroupSet.end(); it++)
		{
			SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*it);
			gassert(l_pUnitGroup,"Unit Group invalid");
			if(l_pUnitGroup->IsNaval() && 
				l_pUnitGroup->Status() == l_pUnitGroup->NextStatus() &&
				(l_pUnitGroup->Status() == EMilitaryStatus::Parked ||
				 l_pUnitGroup->Status() == EMilitaryStatus::Fortified ||
				 l_pUnitGroup->Status() == EMilitaryStatus::Ready ))
			{
				l_vNavalGroups.push_back(l_pUnitGroup);
			}
		}
		//No need to continue if it has no boats
		if(l_vNavalGroups.size() == 0)
			return;
	}	

	//Find the enemies' boats
	{
		for(set<UINT32>::const_iterator it = in_ListOfEnnemies.begin();
			it != in_ListOfEnnemies.end(); it++)
		{
			const set<UINT32>& l_UnitGroupSet = g_Joshua.UnitManager().CountryUnitGroups(*it);
			for(set<UINT32>::const_iterator it = l_UnitGroupSet.begin();
				it != l_UnitGroupSet.end(); it++)
			{
				SP2::GUnitGroup* l_pUnitGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*it);
				gassert(l_pUnitGroup,"Unit Group invalid");
				if(l_pUnitGroup->IsNaval() && 
				(	l_pUnitGroup->Status() == EMilitaryStatus::Parked ||
					l_pUnitGroup->Status() == EMilitaryStatus::Fortified ||
					l_pUnitGroup->Status() == EMilitaryStatus::Ready))
				{
					l_EnemiesNavalGroups.insert(make_pair<REAL64,SP2::GUnitGroup*>(-l_pUnitGroup->Value(),l_pUnitGroup));
				}
			}
		}
	}

		
	if(l_EnemiesNavalGroups.size() == 0)
	{
		BombardEnemy(in_iCountryID,in_ListOfEnnemies,l_vNavalGroups);
		return;
	}

	//MultiMOD
	//See if enemies are close enough to start combat

	//Attack the closest naval units
	for(multimap<REAL64, SP2::GUnitGroup* >::iterator it = l_EnemiesNavalGroups.begin();
		 it != l_EnemiesNavalGroups.end() && l_vNavalGroups.size() > 0; it++)
	{
		SP2::GUnitGroup* l_pEnemyUnitGroup = it->second;
		REAL64 l_fValueOfEnemy = l_pEnemyUnitGroup->Value();
		REAL64 l_fValueAllies = 0.f;

		//Find how many allied units are going towards it
		for(hash_set<SP2::GUnitGroup*>::const_iterator itA = in_AlliedUnitGroups.begin();
			 itA != in_AlliedUnitGroups.end() && l_fValueAllies < l_fValueOfEnemy; itA++)
		{
			if((*itA)->IsNaval() && (*itA)->Status() == EMilitaryStatus::Moving)
			{
				const vector<SP2::GUnitPathPoint>& l_vPoints = ((SP2::GUnitGroup*)(*itA))->Path().Points();	
				if(l_vPoints.back().m_Position == l_pEnemyUnitGroup->Position())
					l_fValueAllies += (*itA)->Value();
			}
		}

		while(l_fValueAllies < l_fValueOfEnemy && l_vNavalGroups.size() > 0)
		{
			REAL32 l_fMinDistance = FLT_MAX;
			SDK::Combat::GUnitGroup* l_pClosestUnitGroup = NULL;
			vector<SP2::GUnitGroup*>::iterator l_ItrToRemove = l_vNavalGroups.begin();
			vector<SP2::GUnitGroup*>::iterator itNaval = l_vNavalGroups.begin();
			while(itNaval != l_vNavalGroups.end())
			{
				SP2::GUnitGroup* l_pUnitGroup = *itNaval;
				REAL32 l_fDistance = g_ServerDAL.DistanceBetween2PointsLatLong(l_pUnitGroup->Position(),
																						l_pEnemyUnitGroup->Position());
				if(l_fDistance < l_fMinDistance)
				{
					l_fMinDistance = l_fDistance;
					l_pClosestUnitGroup = l_pUnitGroup;
					l_ItrToRemove = itNaval;
				}
				itNaval++;
			}
			if(l_pClosestUnitGroup)
			{
				vector<SDK::Combat::GUnitGroup*> l_vMoving;
				l_vMoving.push_back(l_pClosestUnitGroup);
				g_ServerDCL.UnitMover().MoveUnits(l_vMoving,l_pEnemyUnitGroup->Position());
				l_fValueAllies += ((SP2::GUnitGroup*)l_pClosestUnitGroup)->Value();
				l_vNavalGroups.erase(l_ItrToRemove);
			}
		}		
	}

}

bool GCombatPlanner::OnSave(GIBuffer& io_Buffer)
{
   io_Buffer << m_fNextIteration
             << m_vPlansBuilt
             << m_vFightingOffensive
             << m_vNukesToBeLaunched
             << m_QuickRandom;

	for(ENTITY_ID i = 1; i <= g_ServerDAL.NbCountry(); i++)
	{
      m_MilitaryPlan[i].Serialize(io_Buffer);
   }

   return true;
}

bool GCombatPlanner::OnLoad(GOBuffer& io_Buffer)
{
   io_Buffer >> m_fNextIteration
             >> m_vPlansBuilt
             >> m_vFightingOffensive
             >> m_vNukesToBeLaunched
             >> m_QuickRandom;

	for(ENTITY_ID i = 1; i <= g_ServerDAL.NbCountry(); i++)
	{
      m_MilitaryPlan[i].Unserialize(io_Buffer);
   }

   return true;
}

bool GCombatPlanner::OnNew(GDatabase* in_pDatabase)
{
	m_QuickRandom.Seed(clock());
	m_fNextIteration = 0.0f;

	m_vPlansBuilt.resize(g_ServerDAL.NbCountry() + 1);
	m_vFightingOffensive.resize(g_ServerDAL.NbCountry() + 1);
	m_vNukesToBeLaunched.resize(g_ServerDAL.NbCountry() + 1);
	for(UINT32 i = 1;i < m_vPlansBuilt.size();i ++)
	{
		m_vFightingOffensive[i].resize(g_ServerDAL.NbCountry() + 1);
	}

	SAFE_DELETE_ARRAY(m_MilitaryPlan);
	m_MilitaryPlan = new GMilitaryPlan[g_ServerDAL.NbCountry() + 1];

	for(ENTITY_ID i = 1;i <= (ENTITY_ID) g_ServerDAL.NbCountry();i ++)
	{
		m_MilitaryPlan[i].CountryId(i);
   }

   return true;
}

void GCombatPlanner::OnClean()
{
   if(!g_SP2Server->CleanVariableOnly() )
   {
	   SAFE_DELETE_ARRAY(m_MilitaryPlan);
   }

   m_vPlansBuilt.clear();
	m_vFightingOffensive.clear();
	m_vListOfEnnemies.clear();
	m_vNukesToBeLaunched.clear();
}
