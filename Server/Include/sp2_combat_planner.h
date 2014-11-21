/**************************************************************
*
* sp2_combat_planner.h
*
* Description
* ===========
*  Holds the information abouth the military plans of every country. 
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _SP2_COMBAT_PLANNER_H_
#define _SP2_COMBAT_PLANNER_H_

namespace SP2
{
	struct GMegatonsWaiting
	{
		UINT32 TargetCountry;
      UINT32 Megatons;
		ENuclearTargetType::Enum Type;      
	};
	
	class GMilitaryPlan;

   class GCombatPlanner : public SDK::GGameDataNode
   {
		GCombatPlanner();
      ~GCombatPlanner();
   public:
		//! Gets the unique Combat planner instance
      static GCombatPlanner& Instance();

		//! Init the combat planner 
		void Init();

		//! Iterate the decisions and planning of combat AI
		void Iterate(UINT32 in_iCountryID);

		//! Ask AI to create a military plan for a war between country 1 and country 2
		void MilitaryPlanExist(bool in_bExist, UINT32 in_iCountry);

		//! Add a nuke to waiting list. It will be launched at the next combat planner iteration
		void AddNukeToWaitingList(UINT32 in_iCountrySource, 
										  UINT32 in_iCountryTarget,
										  UINT32 in_iNewMegatons,
										  ENuclearTargetType::Enum in_iType);		

		//! Sets whether or not a country is fighting an offensive or defensive war against that other country
		void WarOffensiveStatus(UINT32 in_iCountrySource,
										UINT32 in_iCountryTarget,
										bool in_bOffensive);

		//! Gets whether or not a country is fighting an offensive or defensive war against that other country. True means an offensive war
		bool WarOffensiveStatus(UINT32 in_iCountrySource,
										UINT32 in_iCountryTarget);

	private:		
		//! Constructs a plan of regions he should conquer before offering peace. 
		void BuildMilitaryPlan(ENTITY_ID in_iAttackingCountry);

		//! Re-Adjust the military plan. The country might decide to ask for peace, or continue taking regions
		void AdjustMilitaryPlan(ENTITY_ID in_iAttackingCountry);

		//! The attacking country follows its military plan
		void FollowPlan(ENTITY_ID in_iAttackingCountry, 
							 GMilitaryPlan* l_pMilitaryPlan,
							 const set<UINT32>& in_AlliesInWar,
							 const hash_set<SP2::GUnitGroup*>& in_AlliedUnitGroups);

		//! Will assign the best unit groups of a country to attack or defend this region
		UINT32 FindBestAttackers(ENTITY_ID in_iCountryID, UINT32 in_iRegionID, bool in_bDefensive, const hash_set<SP2::GUnitGroup*>& in_AlliedUnitGroups);		

		//! Return the ennemy group location of the closest ennemy from the initial location
		bool FindClosestEnnemyLocation(const GVector2D<REAL32>& in_GroupLocation, 
			 									 set<UINT32>& in_vEnnemyGroupList,
												 GVector2D<REAL32>& out_EnnemyLocation);

		//! Returns true if that group is needed in that specific region. It is not needed if enough allie groups are inside or are in transit towards that region
		bool RegionNeedsReinforcement(const SDK::Combat::GUnitGroup* in_pAttackingGroup, UINT32 in_iRegionID,const hash_set<SP2::GUnitGroup*>& in_vAlliesGroup, REAL64 in_fTotalEnnemyValue);

		//! Return true if the status of a unit group let him move
		bool StatusReadyForMovement(SP2::EMilitaryStatus::Enum l_iStatus);

		void IterateWaitingNukes();

		//! A country will execute its defensive move inside that region
      UINT32 FollowPlanInsideDefensiveRegion(UINT32 in_iRegionID, UINT32 in_iAttackingCountry, bool in_bHostile, const hash_set<SP2::GUnitGroup*>& in_AlliedUnitGroups);

		//! That country will iterate its naval units against its ennemies;
		void IterateNavalUnits(UINT32 in_iCountryID, const set<UINT32>& in_ListOfEnnemies, const hash_set<SP2::GUnitGroup*>& in_AlliedUnitGroups);

		//! AI function that will send all naval groups of country towards enemies regions
		void BombardEnemy(UINT32 in_iCountryID, 
			const set<UINT32>& in_ListOfEnnemies,
			const vector<SP2::GUnitGroup*>& in_vNavalGroups);

		UINT32 GetRandom(UINT32 in_iMaximumValue);
		
		Random::GQuick                 m_QuickRandom;
		GMilitaryPlan*						 m_MilitaryPlan;
		REAL64								 m_fNextIteration;
		vector< bool >						 m_vPlansBuilt; //!< Contains the status of military plans. If set to false, no plans have been built for this country	
		vector<vector<bool> >			 m_vFightingOffensive; //!< Contains the war status of a country against any other countries. If false, it means it will fight a defensive war (not conquer any regions)
		REAL32								 m_fTotalUnitsValue; //!<Contains the units value for all countries
		set<UINT32>							 m_vListOfEnnemies; //!<Contains the ennemies for every countries		
		vector<vector<GMegatonsWaiting> >	m_vNukesToBeLaunched; //!<Nukes to be launched, set by countries

      // GGameDataNode implementation
      virtual bool OnSave(GIBuffer& io_Buffer);
      virtual bool OnLoad(GOBuffer& io_Buffer);
      virtual bool OnNew(GDatabase* in_pDatabase);
      virtual void OnClean();

	}; //end class GCombatPlanner

}

#define g_CombatPlanner SP2::GCombatPlanner::Instance()

#endif //_SP2_COMBAT_PLANNER_H_