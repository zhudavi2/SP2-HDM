/**************************************************************
*
* sp2_ai.h
*
* Description
* ===========
* Hold GCountryData information for the country being iterated
* This class will be used to control everything the AI should do during each iteration
*
* Owner
* =====
*  Alain Bellehumeur
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _GOLEM_SP2_AI_H_
#define _GOLEM_SP2_AI_H_

#include "sp2_country_data_holder.h"

namespace SP2
{

	//Percentage of regions left before nuking
	const REAL32 c_fPercentageOfRegionsLeft = 0.6f;

	//Maximum years for building
	const REAL64 c_fLimitYears = 5.f;
	const REAL64 c_fLimitYearsTimeOfWar = 2.f;

	//Ratio needed for a stable navy
	const REAL32 c_fPatrolCraftRatio = 0.4f;
	const REAL32 c_fFrigateRatio = 0.25f;
	const REAL32 c_fDestroyerRatio = 0.15f;
	const REAL32 c_fAttackSubmarineRatio = 0.1f;
	const REAL32 c_fAircraftCarriersRatio = 0.05f;
	const REAL32 c_fBallisticMissileSubmarineRatio = 0.05f;

	//Ratio needed for a stable air force
	const REAL32 c_fTransportHelicopterRatio = 0.38f;
	const REAL32 c_fFighterAircraftRatio = 0.22f;
	const REAL32 c_fAttackHelicopterRatio = 0.2f;
	const REAL32 c_fAttackAircraftRatio = 0.1f;
	const REAL32 c_fASWHelicoptertRatio = 0.05f;
	const REAL32 c_fBomberRatio = 0.05f;

	//Ratio needed for a stable ground force
	const REAL32 c_fInfantryVehicleRatio = 0.3f;
	const REAL32 c_fArtilleryGunRatio = 0.3f;
	const REAL32 c_fTankRatio = 0.2f;
	const REAL32 c_fAirDefenseRatio = 0.15f;
	const REAL32 c_fMobileLauncherRatio = 0.05f;
	
	class GAction;

   class GAI
   {
   protected:
   public:
      GAI();
      ~GAI();		

		//Execute all SP2 functions relative to AI iteration
		//Before the country is iterated
		//Is used through a function pointer inside golem_joshua
		bool ExecuteNewAIIteration(const INT16 in_iCountryID);

		//Execute all SP2 functions relative to AI iteration
		//After the country is iterated
		//Is used through a function pointer inside golem_joshua
		bool ExecuteFinishAIIteration(const INT16 in_iCountryID);		

		static void HandleTradeOffer(GTradeData& in_Trade);

		//Utility function, used by iterations to make sure a data is between 1 and -1.
		//Used by objective levels
		static REAL32 VerifyValues(REAL32 l_fLevel);

		//! Puts a variable that was between 0 and 1 to -1 and 1
		static REAL32 AdjustToEHEObjective(REAL32 l_fLevel);

		//! Quick function that create a peace treaty for the AI
		static void AskForTradeEmbargo(UINT32 in_iCountryCreatingEmbargo, UINT32 in_iCountrySufferingTheEmbargo);		

		//! Quick function that create a war for the AI
		static void AskForWar(UINT32 in_iCountryLookingToDeclareWar, UINT32 in_iTarget, bool in_bOffensiveWar);

		//! AI function that return true if a country should annex another one
		static bool ShouldAnnexCountry(UINT32 in_iCountryLookingToAnnex, UINT32 in_iTarget);

		//! Create a new design with maximum research points
		static SP2::GUnitDesign* GAI::CreateNewOptimalDesign(UINT32 in_iCountryID, EUnitCategory::Enum in_eCategory, EUnitType::Enum in_eType);

		//! The country will deploy its units
		static void DeployUnits(UINT32 in_iCountryDeployingUnits);

		//! The country will disband non-necessary units
		static void DisbandUnits(UINT32 in_iCountryDisbandingUnits,SP2::EUnitCategory::Enum in_eCategory = SP2::EUnitCategory::ItemCount);

		//! The country will try to join an existing treaty of that type, or create a new one if none exist
		static void CreateOrJoinTreaty(UINT32 in_iCountryID, ETreatyType::Enum in_TypeOfTreaty, UINT32 in_iSideToJoin = 1);

		static ETreatyRefusal::Enum CountryShouldJoinTreaty(UINT32 in_iCountryID, UINT32 in_iTreatyID, UINT32 in_iSideToJoin = 1);

		//! The country will buy necessary units. 
		/*!
		*	@param in_iCountry: Country that builds the units
		*	@in_fStrength: Strength, between 1 and 5, that represents how many units a country will buy
		*	@return It will return the ID in the production queue, or UINT_MAX if it bought units
		**/
		static UINT32 BuildOrBuyUnits(UINT32 in_iCountry, REAL32 in_fStrength, EUnitCategory::Enum in_Category = EUnitCategory::ItemCount);

		//! The country will build, or not, units if it feels it needs them to beat its ennemy
		static void OrderBuildUnitsAgainstEnnemy(UINT32 in_iCountryBuilding, UINT32 in_iEnnemy);

		static UINT32 FindBestTargetForNextMission(UINT32 in_iCountryID);

		//! Return the best available cell for the current job
		static GCovertActionCell* FindBestCellForJob(UINT32 in_iCountryDoingTheAttack, UINT32 in_iTarget);

		//! The country examines which war it should join or leave
		static void ExamineWarAndPeacePossibilities(UINT32 in_iCountryID);

		//! Check if that AI Country wants to launch nuke in self defense
		static void PossiblyLaunchNukeInSelfDefense(UINT32 in_iCountryID);

		static UINT8 m_NbTreatiesByType[ETreatyType::ItemCount];
	private:
		static REAL32 MaximumCostByUnit(UINT32 in_iCountry, UINT32 in_iDesired);
		static void FindPossibleDesign(UINT32 in_iCountry, EUnitType::Enum in_eTypeToBuild,multimap<REAL32,SP2::GUnitDesign*>& out_mPossibleDesigns);
		static UINT32 ExecuteBuildUnits(UINT32 in_iCountry,
												  UINT32 in_iDesired,
												  EUnitCategory::Enum in_eCategory,
												  EUnitType::Enum in_eUnitType,
												  REAL32 in_fMaximumCostByUnit,
												  UINT8* in_Research,
												  const multimap<REAL32,SP2::GUnitDesign*>& in_PossibleDesign);

		void ExamineLeavingTreaties(UINT32 in_iCountryID) const;		
		void ExamineUnitMovement(UINT32 in_iCountryID) const;
		void ExamineBudgetReadjustment(UINT32 in_iCountryID) const;
		void ExamineCovertActionCells(UINT32 in_iCountryID) const;

		void ExamineBudgetCuts(UINT32 in_iCountryID) const;
		void ExamineBudgetFundings(UINT32 in_iCountryID) const;		

		//! Build that amount of ground units
		static UINT32 BuildGroundUnits(UINT32 in_iCountry, UINT32 in_iDesired);
		//! Build that amount of air units
		static UINT32 BuildAirUnits(UINT32 in_iCountry, UINT32 in_iDesired);
		//! Build that amount of naval units
		static UINT32 BuildNavalUnits(UINT32 in_iCountry, UINT32 in_iDesired);
		
		//! Buy that amount of units for that unit type
		static bool BuyUnits(UINT32 in_iCountry, EUnitType::Enum in_eUnitType, UINT32 in_iDesired, REAL32 in_fStartingPrice);
		
		static ETreatyRefusal::Enum HandleMilitaryAccessOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);
		static ETreatyRefusal::Enum HandleAnnexationOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);
		static ETreatyRefusal::Enum HandleFreeRegionOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);
		static ETreatyRefusal::Enum HandleRequestWarOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);
		static ETreatyRefusal::Enum HandleNobleCauseOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);
		static ETreatyRefusal::Enum HandleCulturalExchangesOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);
		static ETreatyRefusal::Enum HandleHumanDevelopmentCollaborationOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);	
		static ETreatyRefusal::Enum HandleEconomicPartnershipOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);	
		static ETreatyRefusal::Enum HandleAssumeForeignDebtOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);
		static ETreatyRefusal::Enum HandleMilitaryPresenceRemovalOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);
		static ETreatyRefusal::Enum HandleEconomicEmbargoOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);	
		static ETreatyRefusal::Enum HandleEconomicAidOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);	
		static ETreatyRefusal::Enum HandleCommonMarketOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);
		static ETreatyRefusal::Enum HandleResearchPartnershipOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);
		static ETreatyRefusal::Enum HandleWeaponTradeOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);
		static ETreatyRefusal::Enum HandleWeaponTradeEmbargoOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);
		static ETreatyRefusal::Enum HandleAllianceOffer(GTreaty* in_pTreaty, ENTITY_ID in_iCountry, UINT32 in_iSideToJoin);

		static bool ShouldCountryContinueOffensiveWar(const GWar& in_War, UINT32 in_iCountryID);
		static bool ShouldCountryContinueDefensiveWar(const GWar& in_War, UINT32 in_iCountryID);

        //! Returns 1 if it should join the attackers, 2 if it should join the defenders, otherwise will not join
		static UINT32 ShouldCountryJoinWar(const GWar& in_War, UINT32 in_iCountryID);

		static void FillConditions(UINT32 in_iCountryID, UINT32* in_pConditions, UINT32 l_iNbCountriesPossible);		

   };
};

#endif //_GOLEM_SP2_AIITERATION_H_