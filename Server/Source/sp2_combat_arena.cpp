/**************************************************************
*
* sp2_combat_arena.h
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

//#define LOG_COMBAT

SDK::Combat::GArena* SP2::GArena::New()
{
   return new SP2::GArena();
}


SP2::GArena::GArena()
{
   m_UnitGroupIterator     = m_FightingGroups.begin();
   m_iActuallyIteratedUnit = 0;
}

SP2::GArena::~GArena()
{
}

//! Adds a unit to the combat
bool SP2::GArena::AddGroup(SDK::Combat::GUnitGroup*  in_pGroup)
{
   SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)m_pArenaInfo;

   //Make sure naval units cant join combat on the ground and vice versa
   SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)in_pGroup;
   if((l_pArenaInfo->m_iRegionID == 0 && !l_pGroup->IsNaval()) || 
      (l_pArenaInfo->m_iRegionID != 0 && l_pGroup->IsNaval())
      )
   {
      return false;
   }



   //Check if the group can be placed in the combat and if so, which side it will be on
   ECombatSide::Enum l_GroupSide = ComputeCombatSide((SP2::GUnitGroupEx*)in_pGroup);
   if(l_GroupSide == ECombatSide::CantDetermine)
   {
      return false;
   }

   IF_RETURN(!__super::AddGroup(in_pGroup),false);

   
   SP2::GActorInfo* l_pActor = NULL;

   //Add the country to the list of the participating countries
   switch(l_GroupSide)
   {
      case ECombatSide::Attacker:
      {
         //Check if country does not already exist in attacker country list, if not, insert it
         if(find(l_pArenaInfo->m_vAttackerCountriesList.begin(),l_pArenaInfo->m_vAttackerCountriesList.end(),in_pGroup->OwnerId()) == l_pArenaInfo->m_vAttackerCountriesList.end())
         {
            l_pArenaInfo->m_vAttackerCountriesList.push_back((UINT16)in_pGroup->OwnerId());
         }
         break;
      }
      case ECombatSide::Defender:
      {
         //Check if country does not already exist in Defender country list, if not, insert it
         if(find(l_pArenaInfo->m_vDefenderCountriesList.begin(),l_pArenaInfo->m_vDefenderCountriesList.end(),in_pGroup->OwnerId()) == l_pArenaInfo->m_vDefenderCountriesList.end())
         {
            l_pArenaInfo->m_vDefenderCountriesList.push_back((UINT16)in_pGroup->OwnerId());
         }
         break;
      }
      default:
         gassert(0,"SP2::GArena::AddGroup, Default case not handled");
         break;
   }


   //Add the group to the list of groups present in the combat
   l_pArenaInfo->m_vUnitGroupIDs.push_back(in_pGroup->Id());
   g_Joshua.UnitManager().ModifyArenaInfo(l_pArenaInfo);

   //Make sure there is a unit representation for the new country
   {
      //Check if representation already exists
      l_pActor = l_pArenaInfo->Actor(in_pGroup->OwnerId());

      //Does not exist (Actor not found) add a new one
      if(!l_pActor)
      {
         //Create a new actor
         GActorInfo l_Actor;
         REAL32 l_fStartPosition;
         EUnitAction::Enum l_UnitAction;
         l_Actor.m_iCountryID = (UINT16)in_pGroup->OwnerId(); //Owner ID
         l_Actor.m_Side = l_GroupSide;

         //Set its Side (only 2 possible sides)
         if(l_GroupSide == ECombatSide::Attacker)
         {            
            l_fStartPosition = c_iBattleArenaSize;
            l_Actor.m_fMaxFrontLine = 0.0f;
            l_UnitAction = EUnitAction::Attacking;
         }
         else
         {
            l_fStartPosition = 0.0f;
            l_Actor.m_fMaxFrontLine = c_iBattleArenaSize;
            l_UnitAction = EUnitAction::Attacking;
         }
         
         //Set the starting unit position
         for(UINT32 i = 0 ; i < g_ServerDAL.UnitTypes().size(); i++)
         {
            GUnitTypeCbtInfo     l_CbtInfo;
            l_CbtInfo.m_eAction           = l_UnitAction;
            l_CbtInfo.m_fPosition         = l_fStartPosition;
            // TODO: Calculate the Strength ratio between the units
            l_CbtInfo.m_fUnitsValue       = 0;
            l_CbtInfo.m_iUnitQty          = 0;
            l_CbtInfo.m_fLastMovementTime = g_CombatManager.Clock();
            l_Actor.m_vUnitTypeInfo.push_back(l_CbtInfo);
         }

         //Add the actor to the actor list 
         l_pArenaInfo->m_Actors[l_Actor.m_iCountryID] = l_Actor;
         l_pActor = &l_pArenaInfo->m_Actors[l_Actor.m_iCountryID];
      }//end if Actor wasnt there
   }

   gassert(l_pActor,"There should be an actor for the given unit group in the Arena");
 
   //Calculate the unit value by Type
   {
      vector<SDK::Combat::GUnit*>::const_iterator l_UnitsIt = in_pGroup->Units().begin();
      while(l_UnitsIt != in_pGroup->Units().end())
      {
         SP2::GUnitDesign* l_pUnitDesign = (SP2::GUnitDesign*)(*l_UnitsIt)->Design();
         UINT32 l_iUnitQty = ((SP2::GUnit*)(*l_UnitsIt))->Qty();
         REAL32 l_fValue = l_pUnitDesign->Cost() * (REAL32)l_iUnitQty;
         l_pActor->m_vUnitTypeInfo[l_pUnitDesign->Type()->Id()-1].m_fUnitsValue += l_fValue;
         l_pActor->m_vUnitTypeInfo[l_pUnitDesign->Type()->Id()-1].m_iUnitQty    += l_iUnitQty;
         l_UnitsIt++;
      }
   }

   //Because new units joined the combat, actual unit cant hit condition might have changed
   //reset the flag 
   for(list<SDK::Combat::GUnitGroup*>::const_iterator l_GrpIt = m_FightingGroups.begin();
       l_GrpIt != m_FightingGroups.end();
       l_GrpIt++)
   {
      //For each unit of the group
      for(vector<SDK::Combat::GUnit*>::const_iterator l_UnitsIt = (*l_GrpIt)->Units().begin();
          l_UnitsIt != (*l_GrpIt)->Units().end();
          l_UnitsIt++)
      {
         SP2::GUnit* l_pUnit = (SP2::GUnit*)*l_UnitsIt;
         l_pUnit->m_pFightInfo->m_bWillNeverHitAnyActualUnit = false;
      }
   }



   //Set the group to iterate as the 1st group
   {
      m_UnitGroupIterator     = m_FightingGroups.begin();
      m_iActuallyIteratedUnit = 0;
   }


   return true;
}

/*!
* Pure virtual function automatically called so the attacker picks a target
* Selecting a target works differently if you are an Air unit and if you want
* to hit a air unit
**/
SDK::Combat::GUnit* SP2::GArena::SelectTarget(const SDK::Combat::GUnit* in_pAttacker)
{
    SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)Info();
   //Find the attacker Actor
   SP2::GActorInfo*  l_pAttackerActor  = l_pArenaInfo->Actor(in_pAttacker->Group()->OwnerId());
   gassert(l_pAttackerActor,"Actor does not exist for the given group ? Impossible");

   REAL32 l_fBestTargetScore = 0.0f;
   SP2::GUnit* l_pBestTarget = NULL;

   //Variable to know if a unit will be able to possibly hit a target 
   //among the available targets
   bool l_bCanPossiblyHitATargetLater = false;

   //For each enemy unit
   //Get its target score, if score is better than the actual
   //best target, replace the actual target
   list<SDK::Combat::GUnitGroup*>::iterator l_GroupIt = m_FightingGroups.begin();
   while(l_GroupIt != m_FightingGroups.end())
   {
      SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)*l_GroupIt;
      SP2::GActorInfo* l_pActor = NULL;
      //If the owner of the group is the same as the attacker owner,
      //we already have the pointer to the actor
      if(l_pGroup->OwnerId() == in_pAttacker->Group()->OwnerId())
      {
         l_pActor  = l_pAttackerActor;
      }
      else
      {
         l_pActor = l_pArenaInfo->Actor(l_pGroup->OwnerId());
      }
      gassert(l_pActor,"Actor does not exist for the given group ? Impossible");

      //Is the group an enemy group ?
      if(l_pActor->m_Side != l_pAttackerActor->m_Side)
      {
         //Enemy Group, process each unit of this group to see if we can attack them
         vector<SDK::Combat::GUnit*>::const_iterator l_UnitIt = l_pGroup->Units().begin();
         while(l_UnitIt != l_pGroup->Units().end())
         {
            REAL32 l_fTargetScore = TargetScore((SP2::GUnit*)in_pAttacker,l_pAttackerActor,(SP2::GUnit*)*l_UnitIt,l_pActor);
            //If the Target could be hit or is hit, set the variable saying
            //that it will be able to hit a target later on (possibly only out of range for now)
            if(l_fTargetScore != c_fTargetScoreWillNeverHit)
            {
               l_bCanPossiblyHitATargetLater = true;
            }
            //If best target yet
            if(l_fTargetScore > l_fBestTargetScore)
            {
               l_fBestTargetScore = l_fTargetScore;
               l_pBestTarget = (SP2::GUnit*)*l_UnitIt;
            }
            l_UnitIt++;            
         }
      }//end if unit is an enemy unit
      l_GroupIt++;
   }//End of the check for every unit that is not a air unit


   //If there is no target for now and Will never be able to hit one
   if(!l_pBestTarget && !l_bCanPossiblyHitATargetLater)
   {
      ((SP2::GUnit*)in_pAttacker)->m_pFightInfo->m_bWillNeverHitAnyActualUnit = true;
   }
   else
   {//Can or will be able to hit a unit at a given moment
      ((SP2::GUnit*)in_pAttacker)->m_pFightInfo->m_bWillNeverHitAnyActualUnit = false;
   }
   return l_pBestTarget;
}

/*!
 * Evaluates if a target is a good target for the attacker by giving a score to the target
 * The higher the score, the better the target is for the current attacker
 * @param in_pAttacker: Pointer to the attacker
 * @param in_pTarget: Pointer to the target
 * @return REAL32: Score of the current target
 **/
REAL32 SP2::GArena::TargetScore(SP2::GUnit* in_pAttacker,SP2::GActorInfo* in_pAttackerActor,SP2::GUnit* in_pTarget,SP2::GActorInfo* in_pTargetActor)
{
   const REAL32** l_pGunEfficiencyTable      = g_ServerDAL.GunCombatEfficiencyTable();
   const REAL32** l_pMissileEfficiencyTable  = g_ServerDAL.MissileCombatEfficiencyTable();

   SP2::GUnitDesign* l_pAttackerDesign = (SP2::GUnitDesign*)in_pAttacker->Design();
   SP2::GUnitDesign* l_pTargetDesign   = (SP2::GUnitDesign*)in_pTarget->Design();

   SP2::GUnitType* l_pAttackerUnitType = l_pAttackerDesign->Type();
   SP2::GUnitType* l_pTargetUnitType   = l_pTargetDesign->Type();

   //Fetch the training modifier and the unit group status modifier
   REAL32 l_fAttackerModifier = 0;
   REAL32 l_fTargetModifier   = 0;   
   {
      gassert(in_pAttacker->Training() <= ETrainingLevel::Elite,"In valid unit training");
      gassert(in_pTarget->Training() <= ETrainingLevel::Elite,"In valid unit training");
      l_fAttackerModifier = c_TrainingModifiers[in_pAttacker->Training()];
      l_fTargetModifier   = c_TrainingModifiers[in_pTarget->Training()];
   
      //Fetch and Validate the combat group status, should be ready, fortified or parked
      {
         EMilitaryStatus::Enum l_eAttackerGroupStatus = ((SP2::GUnitGroup*)in_pAttacker->Group())->CombatStatus();
         gassert(l_eAttackerGroupStatus == EMilitaryStatus::Parked || l_eAttackerGroupStatus == EMilitaryStatus::Ready ||  l_eAttackerGroupStatus == EMilitaryStatus::Fortified||  l_eAttackerGroupStatus == EMilitaryStatus::Moving,"SP2::GUnit::Damage Invalid military group status");
         EMilitaryStatus::Enum l_eTargetGroupStatus   = ((SP2::GUnitGroup*)in_pTarget->Group())->CombatStatus();
         gassert(l_eTargetGroupStatus == EMilitaryStatus::Parked || l_eTargetGroupStatus == EMilitaryStatus::Ready ||  l_eTargetGroupStatus == EMilitaryStatus::Fortified||  l_eTargetGroupStatus == EMilitaryStatus::Moving,"SP2::GUnit::Damage Invalid military group status");
         l_fTargetModifier   *= c_GroupMilitaryStatusModifiers[l_eTargetGroupStatus];
         l_fAttackerModifier *= c_GroupMilitaryStatusModifiers[l_eAttackerGroupStatus];
      }
   }

   //Check if the target can be hit using Missiles
   REAL32 l_fMissileEfficiency = l_pMissileEfficiencyTable[l_pAttackerUnitType->Id()-1][l_pTargetUnitType->Id()-1];

   //Check if the target can be hit using Gun
   REAL32 l_fGunEfficiency     = l_pGunEfficiencyTable[l_pAttackerUnitType->Id()-1][l_pTargetUnitType->Id()-1];

   //If no efficiency using missiles or guns against this type of unit
   if(!l_fMissileEfficiency && !l_fGunEfficiency)
      return c_fTargetScoreWillNeverHit;

   //Get the unit Missile Range
   REAL32 l_fMissileRange  = -1.0f;
   if(l_pAttackerDesign->MissileRange())
   {
      l_fMissileRange = l_pAttackerUnitType->MissileRangeByLevel()[l_pAttackerDesign->MissileRange()-1];
   }
   
   //Get the unit Gun Range
   REAL32 l_fGunRange = -1.0f;
   if(l_pAttackerDesign->GunRange())
   {
      l_fGunRange = l_pAttackerUnitType->GunRangeByLevel()[l_pAttackerDesign->GunRange()-1];
   }   

   //Calculate the distance between the units
   REAL32 l_fDistanceBetweenUnits = abs(in_pAttackerActor->m_vUnitTypeInfo[l_pAttackerUnitType->Id()-1].m_fPosition -
                                        in_pTargetActor->m_vUnitTypeInfo[l_pTargetUnitType->Id()-1].m_fPosition);

   //Check if unit can get in range
   {
      //If unit can never get in range
      if(l_fGunRange <= 0 && l_fMissileRange <= 0)
      {
         return c_fTargetScoreWillNeverHit;
      }

      //If unit is out of range for now but there is a possibility to get in range
      if( (l_fDistanceBetweenUnits > l_fGunRange) && (l_fDistanceBetweenUnits > l_fMissileRange) &&
         (l_pAttackerDesign->SpeedAndManeuverability() || l_pTargetDesign->SpeedAndManeuverability() )
         )
      {
         return c_fTargetScoreCantHitForNow;
      }

      //If unit is actually out of range and there is no possibility to get in range (by moving)
      if( (l_fDistanceBetweenUnits > l_fGunRange) && (l_fDistanceBetweenUnits > l_fGunRange) &&
         !l_pAttackerDesign->SpeedAndManeuverability() && ! l_pTargetDesign->SpeedAndManeuverability()
         )    
      {
         return c_fTargetScoreWillNeverHit;
      }
   }

   //______________________________________________________________________________________
   //
   //If we reach this point, the unit is "hittable", just a question of stealth
   //______________________________________________________________________________________

   //------------------------------
   //  Stealth Check
   //    Perform the stealth check, to see if the attacker can See the target
   //------------------------------
   bool l_bTargetVisible = false;
   {      
      UINT8  l_iAttackerSensorsLevel = l_pAttackerDesign->Sensors();
      REAL32 l_fAttackerSensors      = 0;
      if(l_iAttackerSensorsLevel)
      {
         l_fAttackerSensors = l_pAttackerUnitType->SensorsByLevel()[l_iAttackerSensorsLevel-1];
         l_fAttackerSensors *= l_fAttackerModifier;
      }
      REAL32 l_fDefenderStealth      = 0;
      if(l_pTargetDesign->Stealth())
      {
         l_fDefenderStealth = l_pTargetUnitType->StealthByLevel()[l_pTargetDesign->Stealth()-1];
         l_fDefenderStealth *= l_fTargetModifier;
      } 
      UINT8  l_iDefenderSpeedLevel   = l_pTargetDesign->SpeedAndManeuverability();
      l_iDefenderSpeedLevel          = (UINT8)(l_iDefenderSpeedLevel*l_fTargetModifier);
      REAL32 l_fAttackerChances      = l_fAttackerSensors - l_fDefenderStealth - ((l_iDefenderSpeedLevel-l_iAttackerSensorsLevel)*5);

      REAL32 l_fRandom = g_CombatManager.RandomReal()*100;
      //Simulate D20 roll, perfect 20 always hit, 0 always fail
      if(l_fRandom >= 95.f)
      {
         //Rolled 20
         l_bTargetVisible = true;
      }
      else if(l_fRandom <= 5.f)
      {
         //Rolled 0
         l_bTargetVisible = false;
      }
      else if(l_fRandom < l_fAttackerChances)
      {
         l_bTargetVisible = true;
      }
      else
      {
         l_bTargetVisible = false;
      }

      //If target is not visible
      if(!l_bTargetVisible)
      {
         return c_fTargetScoreCantHitForNow;         
      }
   }

   //Compute the score
   REAL32 l_fScore = c_fTargetScoreCantHitForNow;
   {
      EUnitWeapon::Enum l_WeaponToUse = in_pAttacker->SelectWeapon(in_pTarget,l_fDistanceBetweenUnits);
      switch(l_WeaponToUse)
      {
         case EUnitWeapon::Guns:
         {
            l_fScore = (l_fGunRange/max(0.1f,l_fDistanceBetweenUnits)*l_fGunEfficiency);
            break;
         }
         case EUnitWeapon::Missiles:
         {
            l_fScore = (l_fMissileRange/max(0.1f,l_fDistanceBetweenUnits)*l_fMissileEfficiency);
            break;
         }
         case EUnitWeapon::Undefined:
         {
            l_fScore = c_fTargetScoreCantHitForNow;
            break;
         }
         default:
         {
            gassert(0,"SP2::GArena::TargetScore -> Unhandled case in target score");
            l_fScore = c_fTargetScoreWillNeverHit;
            break;
         }
      }
   }
   return l_fScore;
}


/*!
 * Moves the unit in the GArena, position for the unit can go from
 * 0 to c_iBattleArenaSize and is stored in the GActorInfo by unit type.
 * If unit is defending, it moves back if it can
 * If unit is attacking, it moves forward if it has no units in range
 **/
void   SP2::GArena::MoveUnit(const SDK::Combat::GUnit* in_pMovingUnit)
{
   SP2::GUnit*       l_pUnit      = (SP2::GUnit*)in_pMovingUnit;
   SP2::GUnitDesign* l_pUnitDesign= (SP2::GUnitDesign*)l_pUnit->Design();
   gassert(l_pUnit->m_pFightInfo,"SP2::GArena::MoveUnit Unit has lost its fight info??");

   SP2::GArenaInfo*  l_pArenaInfo = (SP2::GArenaInfo*)m_pArenaInfo;
   SP2::GActorInfo*  l_pActorInfo = l_pArenaInfo->Actor(in_pMovingUnit->Group()->OwnerId());   
   gassert(l_pActorInfo,"There should be an actor defined for this Unit Owner, see SP2::GArena::AddGroup");


   //Check if the Unit's missile must be refilled
   RefillMissiles(l_pUnit);

   //1st check, if the unit will never by able to hit any unit that are actually present, dont move
   //This condition is evaluated every time we check if the unit can attack and selects a target
   if(l_pUnit->m_pFightInfo->m_bWillNeverHitAnyActualUnit && !l_pActorInfo->m_bRetreating)
   {
      return;
   }


   EUnitAction::Enum l_UnitAction = l_pActorInfo->m_vUnitTypeInfo[l_pUnitDesign->Type()->Id()-1].m_eAction;
 

   REAL32 l_fActualPosition = l_pActorInfo->m_vUnitTypeInfo[l_pUnitDesign->Type()->Id()-1].m_fPosition;
   REAL32 l_fNewPosition    = l_fActualPosition;

   REAL64 l_fLastMovementClock = l_pActorInfo->m_vUnitTypeInfo[l_pUnitDesign->Type()->Id()-1].m_fLastMovementTime;


   //Check if the unit type has already moved this "round", if no, there is a possibility to move them
   //Distance is updated each hour
   if( (g_CombatManager.Clock() - l_fLastMovementClock) < (0.5f / 24.f) )
   {
      return;
   }


   //If we get here, it means that the unit has the possibility to move, so update its
   //last movement clock, so the unit wont accumulate moving time if it does not move this round
   //So to access the last movement clock, you must use the l_fLastMovementClock variable
   l_pActorInfo->m_vUnitTypeInfo[l_pUnitDesign->Type()->Id()-1].m_fLastMovementTime = (REAL32)g_CombatManager.Clock();

   //----------------------------------------------------
   // Defending, move back if possible
   //----------------------------------------------------
   if(l_UnitAction == EUnitAction::Defending)
   {

      REAL32 l_fCoveredDistance = CoveredDistance(g_CombatManager.Clock()-l_fLastMovementClock,
                                                   AverageSpeed(in_pMovingUnit->Group()->OwnerId(),l_pUnitDesign->Type()));

      if(l_pActorInfo->m_Side == ECombatSide::Defender)
      {//If defender, moving back means going back to 0, so substract
         l_fNewPosition = max(0,l_fActualPosition-l_fCoveredDistance);
      }
      else
      {//Attacker, moving back means going back to 100, add
         l_fNewPosition = min(c_iBattleArenaSize,l_fActualPosition+l_fCoveredDistance);
      }    
   //----------------------------------------------------
   //End Defending, move backward
   //----------------------------------------------------      
   }
   else
   {//attacking
      //If we already are at the front line, dont move further
      if(l_fActualPosition == l_pActorInfo->m_fMaxFrontLine)
         return;
      
      //If unit is attacking but it has a possible target, dont move, else, advance
      {
         SP2::GUnit* l_pTarget = (SP2::GUnit*)SelectTarget(in_pMovingUnit);
         if(l_pTarget)
            return;
      }

      //------------------------------------------------------------------
      // Move forward
      //------------------------------------------------------------------
      //use the clock delta to know exactly how much time the unit has advanced
      REAL32 l_fCoveredDistance = CoveredDistance(g_CombatManager.Clock()-l_fLastMovementClock,
                                                   AverageSpeed(in_pMovingUnit->Group()->OwnerId(),l_pUnitDesign->Type()));

      if(l_pActorInfo->m_Side == ECombatSide::Defender)
      {//If defender, attacking means going toward 100, so add
         l_fNewPosition = min(l_pActorInfo->m_fMaxFrontLine,l_fActualPosition+l_fCoveredDistance);         

         //Check if there are enemy units that prevent us from advancing
         //Dont go further than the enemy unit that is the furthest
         if(l_fNewPosition >= l_pArenaInfo->m_fFurthestAttackerUnit)
         {
            l_fNewPosition  = l_pArenaInfo->m_fFurthestAttackerUnit;
         }
      }
      else
      {//Attacker, attacking means going toward 0, so substract
         l_fNewPosition = max(l_pActorInfo->m_fMaxFrontLine,l_fActualPosition-l_fCoveredDistance);         
         
         //Check if there are enemy units that prevent us from advancing
         //Dont go further than the enemy unit that is the furthest
         //Air units can go anywhere so only prevent from moving if it is not a Air Unit
         if( l_fNewPosition <= l_pArenaInfo->m_fFurthestDefenderUnit)
         {
            //If so, dont allow to move
            l_fNewPosition = l_pArenaInfo->m_fFurthestDefenderUnit;         }
      }
      //----------------------------------------------------
      //End move forward
      //----------------------------------------------------
   }//end if moving unit is in attacking mode, so move forward if no target

   //If the unit moved
   if(l_fNewPosition != l_fActualPosition)
   {
      //Update the position
      l_pActorInfo->m_vUnitTypeInfo[l_pUnitDesign->Type()->Id()-1].m_fPosition = l_fNewPosition;

      //Update the furthest unit
      {
         //Update furthest unit
         if(l_pActorInfo->m_Side == ECombatSide::Defender)
         {//If defender
            l_pArenaInfo->m_fFurthestDefenderUnit = FurthestUnit(ECombatSide::Defender);
         }
         else
         {//If attacker
            l_pArenaInfo->m_fFurthestAttackerUnit = FurthestUnit(ECombatSide::Attacker);
         }
      }//End of update furthest unit position

      //Notify the clients about the move
      //NotifyActionToClients(ECombatAction::Moving,l_pActorInfo->m_iCountryID,l_pUnitDesign->Type()->Id(),*(INT32*)&l_fNewPosition);  
   
      //Set the arena as modified since a unit has moved
      g_Joshua.UnitManager().ModifyArenaInfo(m_pArenaInfo);
#ifdef LOG_COMBAT
      g_Joshua.Log("Unit type: " + l_pUnitDesign->Type()->Name()+ " now at position " + GString(l_pActorInfo->m_vUnitTypeInfo[l_pUnitDesign->Type()->Id()-1].m_fPosition));
#endif
   }
}

/*!
* Pure virtual function automatically called to know which one is the next unit to act 
* (given the actual time). while it returns something it is called
* @param out_fForceModifier: Modifier for the force of the attack, 
*                            used when in the same iteration a unit could have iterated many times, 
*                            we simply set the force to the number of times the unit could 
*                            have iterated.  So when the damage will be applied, it will 
*                            cause more damage.
**/
GUnit* SP2::GArena::NextActingUnit(REAL32& out_fForceModifier)
{
   SP2::GUnit* l_pUnit = NULL;
   out_fForceModifier = 1.f;

   if(!m_FightingGroups.size())
      return NULL;


   //Make sure the m_UnitGroupIterator and the m_iActuallyIteratedUnit are valid
   {
      if(m_UnitGroupIterator == m_FightingGroups.end())
      {
         m_UnitGroupIterator     = m_FightingGroups.begin();
         m_iActuallyIteratedUnit = 0;
      }

      //If the actually iterated unit index unit isnt valid
      if(m_iActuallyIteratedUnit >= (*m_UnitGroupIterator)->Units().size())
      {
         m_UnitGroupIterator++;
         if(m_UnitGroupIterator == m_FightingGroups.end())
         {
            m_UnitGroupIterator = m_FightingGroups.begin();
         }
         m_iActuallyIteratedUnit = 0;
      }
   }

   //Continue the iteration where we were at
   while(m_UnitGroupIterator != m_FightingGroups.end())
   {
      UINT32 l_iStartValue = m_iActuallyIteratedUnit;
      for(UINT32 i = l_iStartValue ;i < (*m_UnitGroupIterator)->Units().size() ; i++)
      {
         l_pUnit = (SP2::GUnit*)(*m_UnitGroupIterator)->Units()[i];

         //There should be a fighting context information here
         gassert(l_pUnit->FightInfo(),"Should be a fight information");

         bool l_bUnitWillAttack = false;

         //Check if the unit can attack using its fire rate
         REAL32 l_fTimeBetweenAttacks = l_pUnit->FiringDelay();
         if(l_fTimeBetweenAttacks <= 0)
         {//cant shoot
            //skip the unit
         }
         else if(l_pUnit->FightInfo()->m_fLastActionClock < 0)
         {//Unit is performing an action for the 1st time
            l_bUnitWillAttack = true;
         }
         else if( (g_CombatManager.Clock() - l_pUnit->FightInfo()->m_fLastActionClock) >= l_fTimeBetweenAttacks)
         {
            l_bUnitWillAttack = true;
         }  

         //If the unit will attack
         if(l_bUnitWillAttack)
         {
            //Unit is ready to perform an action
            REAL64 l_fDelaySinceLastAttack = (g_CombatManager.Clock() - l_pUnit->FightInfo()->m_fLastActionClock);
            //Compute a force modifier for the action
            REAL32 l_fForceModifier = (REAL32)l_fDelaySinceLastAttack/l_fTimeBetweenAttacks;
            l_pUnit->FightInfo()->m_fLastActionClock = g_CombatManager.Clock();
            out_fForceModifier = l_fForceModifier;
            return l_pUnit;
         }

         gassert(m_iActuallyIteratedUnit == i,"Incoherence in combat iteration");
         m_iActuallyIteratedUnit++;
         
      }     
      m_iActuallyIteratedUnit = 0;
      m_UnitGroupIterator++;
   }

   m_UnitGroupIterator     = m_FightingGroups.begin();
   m_iActuallyIteratedUnit = 0;
   //No units are ready to perform an action
   return NULL;
}


/*!
 * Computes the covered distance in the combat
 * given the time and the speed, using an arctan equation
 * @param in_fClockDelta: Time (in days)
 * @param in_fSpeed: Speed (In Game Constants time)
 **/
REAL32 SP2::GArena::CoveredDistance(REAL64 in_fClockDelta, REAL32 in_fSpeed)
{
   if(!in_fSpeed)
      return 0;
   REAL32 l_fCoveredDistance = (REAL32) c_fSP2NormalTimeSpeedInv * ( (atanf(0.001f * in_fSpeed) * 21) + 0.4f);

   //Slow down the combat simulation
   l_fCoveredDistance /= c_iCombatSimulationSlowDownFactor;

   return (REAL32)(l_fCoveredDistance * in_fClockDelta);
}


/*!
 * Get the average speed for a given unit type (actually calculates it each time)
 * @param in_iCountryID: ID of the country to calculate
 * @param in_pType: Type of the unit we want the average speed for
 * @return REAL32: Average speed
 **/
REAL32 SP2::GArena::AverageSpeed(UINT32 in_iCountryID, SP2::GUnitType* in_pType)
{
   //Get the speed characteristic level array for that unit type
   const REAL32* l_pSpeedChar = in_pType->SpeedByLevel();

   REAL32 l_fAverageSpeed = 0;
   INT32  l_iUnitCount    = 0;

   list<SDK::Combat::GUnitGroup*>::iterator l_GroupIt = m_FightingGroups.begin();
   //For each group
   while(l_GroupIt != m_FightingGroups.end())
   {
      //If group is from the correct country
      if((*l_GroupIt)->OwnerId() == in_iCountryID)
      {
         vector<SDK::Combat::GUnit*>::const_iterator l_UnitIt = (*l_GroupIt)->Units().begin();
         //For each unit, 
         while(l_UnitIt != (*l_GroupIt)->Units().end())
         {
            SP2::GUnitDesign* l_pUnitDesign = (SP2::GUnitDesign*)(*l_UnitIt)->Design();
            //check if the correct unit type, if so, include it in the average speed computation
            if(l_pUnitDesign->Type()->Id() == in_pType->Id())
            {
               if(l_pUnitDesign->SpeedAndManeuverability())
               {
                  l_fAverageSpeed += l_pSpeedChar[l_pUnitDesign->SpeedAndManeuverability()-1];
                  l_iUnitCount++;
               }
            }
            l_UnitIt++;
         }
      }
      l_GroupIt++;
   }
   if(l_iUnitCount)
      return (l_fAverageSpeed/l_iUnitCount);
   else
      return 0;
}


/*!
 * Calculates the combat status, returns if combat is over, or not
 * @return if the combat is over or not
 **/
SDK::Combat::EBattleStatus::Enum SP2::GArena::CombatStatus()
{
   SP2::GArenaInfo* l_pInfo = (SP2::GArenaInfo*)Info(); 

   //If there are 2 countries on different sides, combat is not over
   {
      hash_map<UINT32,GActorInfo>::iterator l_QuickIt = l_pInfo->m_Actors.begin();
      hash_map<UINT32,GActorInfo>::iterator l_SlowIt = l_pInfo->m_Actors.begin();
      while(l_SlowIt != l_pInfo->m_Actors.end())
      {
         while(l_QuickIt != l_pInfo->m_Actors.end())
         {
            if(l_QuickIt->second.m_Side != l_SlowIt->second.m_Side)
            {
               return SDK::Combat::EBattleStatus::NotOver;
            }
            l_QuickIt++;
         }   
         l_SlowIt++;
      }

      if(l_pInfo->m_Actors.size())
      {
         //Set the winner of the battle
         l_pInfo->m_eWinnerSide = l_pInfo->m_Actors.begin()->second.m_Side;
   #ifdef LOG_COMBAT
         GString l_sWinner = "Undefined";
         if(l_pInfo->m_eWinnerSide == ECombatSide::Attacker)
            l_sWinner = L"Attackers";
         else if(l_pInfo->m_eWinnerSide == ECombatSide::Defender)
            l_sWinner = L"Defenders";
         g_Joshua.Log("Combat is Over, winner are the " + l_sWinner);
   #endif
      }
      else
      {
         l_pInfo->m_eWinnerSide = ECombatSide::CantDetermine;
      }
      return SDK::Combat::EBattleStatus::Over;
   }
}


/*!
 * Remove unit group, called by the combat manager when a group is "killed" so there are no more
 * units in it.  We use it to know if we must remove one actor from the actor list
 **/
bool SP2::GArena::RemoveGroup(SDK::Combat::GUnitGroup*  in_pGroup)
{
#ifdef GOLEM_DEBUG
   GDZLOG(GString(L"Removing group from combat : ") + GString(in_pGroup->Id() ),
          EDZLogLevel::Info2);
#endif

   //Remove the group from the unit group list
   SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)m_pArenaInfo;
   vector<UINT32>::iterator l_GroupIt = find(l_pArenaInfo->m_vUnitGroupIDs.begin(),
                                             l_pArenaInfo->m_vUnitGroupIDs.end(),
                                             in_pGroup->Id());
   if(l_GroupIt != l_pArenaInfo->m_vUnitGroupIDs.end())
   {
      l_pArenaInfo->m_vUnitGroupIDs.erase(l_GroupIt);

      //If the group to remove is the actually iterated country, restart the iteration from the beginning
	   if( in_pGroup == *m_UnitGroupIterator)
      {
         if(in_pGroup == *m_FightingGroups.begin())
         {
            m_UnitGroupIterator = (++m_FightingGroups.begin());
            m_iActuallyIteratedUnit = 0;
         }
         else
         {
            m_UnitGroupIterator = m_FightingGroups.begin();
            m_iActuallyIteratedUnit = 0;
         }
      }

      //Check if there are other units with this CountryID, if not, remove the actor
      //with this ID from the Actor List
      UINT32 l_iCountryID = in_pGroup->OwnerId();
      bool l_bFoundOne = false;
      list<SDK::Combat::GUnitGroup*>::iterator l_It = m_FightingGroups.begin();
      while(l_It != m_FightingGroups.end())
      {
         if( (*l_It != in_pGroup) && (l_iCountryID == (*l_It)->OwnerId()))
         {
            l_bFoundOne = true;
            break;
         }
         l_It++;
      }

      //If there are still units for this country, dont do anything
      if(!l_bFoundOne)
      {//No more units for this country, remove the actor from the list
         SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)Info();
         hash_map<UINT32,GActorInfo>::iterator l_ActorIt = l_pArenaInfo->m_Actors.begin();
         while(l_ActorIt != l_pArenaInfo->m_Actors.end())
         {
            if( l_ActorIt->second.m_iCountryID == l_iCountryID)
            {
               break;
            }
            l_ActorIt++;
         }

         gassert(l_ActorIt != l_pArenaInfo->m_Actors.end(),"We should have found one actor for the unit group, if we didnt, there is a big fucking problem, means we have units fighting for no sides in this combat");
         l_pArenaInfo->m_Actors.erase(l_ActorIt);
      }
      g_Joshua.UnitManager().ModifyArenaInfo(l_pArenaInfo);
   }

   //Perform the generic remove group things (calls leave combat on the group and units, removes the group from
   //the fighting groups and so on...
   __super::RemoveGroup(in_pGroup);

   return true;
}

/*!
 * Event called when units are hit, 
 *  no changes must be done on the unit since the generic part of the engine will do them
 * We use it to calculate the new unit values by unit type
 **/
void SP2::GArena::OnUnitsHit(const SDK::Combat::GUnit* in_pAttackedUnit, UINT32 in_iDamageTaken, const SDK::Combat::GUnit* in_pAttacker)
{
   //Notify that the unit was hit
   UINT32 l_iAttackerId = 0;
   if(in_pAttacker)
      l_iAttackerId = in_pAttacker->Id();
   NotifyActionToClients(ECombatAction::Hit,l_iAttackerId,in_pAttackedUnit->Id(),in_iDamageTaken);

   //Update the value
   bool l_bUnitCompletelyKilled = false;
   {
      SP2::GActorInfo* l_pActor = ((SP2::GArenaInfo*)m_pArenaInfo)->Actor(in_pAttackedUnit->Group()->OwnerId());
      //Compute the new unit quantity
      UINT32 l_iUnitQtyDelta     = 0;
      UINT32 l_iNewQty           = 0;
      {
         UINT32 l_iPreviousQty     = ((SP2::GUnit*)in_pAttackedUnit)->Qty();
         UINT32 l_iHPForOneUnit    = ((SP2::GUnitDesign*)in_pAttackedUnit->Design())->Type()->HP();
         UINT32 l_iDamage          = min(in_pAttackedUnit->HP(),in_iDamageTaken);
         UINT32 l_iNewHP           = in_pAttackedUnit->HP() - l_iDamage;
         l_iNewQty                 = (l_iNewHP + l_iHPForOneUnit - 1) / l_iHPForOneUnit;
         l_iUnitQtyDelta           = l_iPreviousQty - l_iNewQty;
      }

      //The delta unit is not necessary l_iNewUnitQty - l_iPreviousQty because units can die due to naval bombardments
      gassert(l_iUnitQtyDelta <= l_pActor->m_vUnitTypeInfo[((SP2::GUnitDesign*)in_pAttackedUnit->Design())->Type()->Id()-1].m_iUnitQty,"Call for JO");
      REAL32 l_fValueDelta      = (REAL32)l_iUnitQtyDelta * ((SP2::GUnitDesign*)in_pAttackedUnit->Design())->Cost();

      if(!l_iNewQty)
         l_bUnitCompletelyKilled = true;

      l_pActor->m_vUnitTypeInfo[((SP2::GUnitDesign*)in_pAttackedUnit->Design())->Type()->Id()-1].m_fUnitsValue -= l_fValueDelta ;
      l_pActor->m_vUnitTypeInfo[((SP2::GUnitDesign*)in_pAttackedUnit->Design())->Type()->Id()-1].m_iUnitQty    -= l_iUnitQtyDelta;

      //Update the Arena Info losses
      UpdateArenaInfoUnitLosses(l_pActor->m_Side,
                                 ((SP2::GUnitDesign*)in_pAttackedUnit->Design())->Type()->Category(),
                                 l_iUnitQtyDelta);
   }

   //If unit has been completely killed, update the furthest unit distance for the
   //side of that unit
   if(l_bUnitCompletelyKilled)
   {
      SP2::GArenaInfo*  l_pArenaInfo = (SP2::GArenaInfo*)m_pArenaInfo;
      GActorInfo* l_pActorInfo = l_pArenaInfo->Actor(in_pAttackedUnit->Group()->OwnerId());
      if(l_pActorInfo->m_Side == ECombatSide::Defender)
         l_pArenaInfo->m_fFurthestDefenderUnit = FurthestUnit(ECombatSide::Defender);
      else
         l_pArenaInfo->m_fFurthestAttackerUnit = FurthestUnit(ECombatSide::Attacker);
   }

   return;
}


/*!
 * Gets the position of the Furthest unit given the side of the unit
 * Should only be needed when moving back because we have to process each
 * unit to know its position, If you need a quick access to the furthest units position
 * you can use directly the ArenaInfo (m_pArenaInfo which must be typecasted in a SP2::GArenaInfo)
 *  SP2::GArenaInfo::m_fFurthestAttackerUnit
 * and 
 *  SP2::GArenaInfo::m_fFurthestDefenderUnit;
 * @param in_Side: Side of the units we want the furthest
 * @return Position of the furthest unit
 **/
REAL32 SP2::GArena::FurthestUnit(ECombatSide::Enum in_Side)
{
   REAL32 l_fFurthestPosition = 0;
   if(in_Side == ECombatSide::Attacker)
      l_fFurthestPosition = c_iBattleArenaSize;
   else
      l_fFurthestPosition = 0;
   
   SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)m_pArenaInfo;
   
   //For every actor, and every unit type, check the one that is the furthest
   hash_map<UINT32,GActorInfo>::const_iterator l_ActorIt = l_pArenaInfo->Actors().begin();
   while(l_ActorIt != l_pArenaInfo->Actors().end())
   {
      //If the actor is on the side we want
      if(l_ActorIt->second.m_Side == in_Side)
      {
         //For every unit type info
         vector<GUnitTypeCbtInfo>::const_iterator l_UnitTypeCbtInfoIt = l_ActorIt->second.m_vUnitTypeInfo.begin();
         while(l_UnitTypeCbtInfoIt != l_ActorIt->second.m_vUnitTypeInfo.end())
         {

            //Dont process the Air Units, dont process units that do not have any value
            if((*l_UnitTypeCbtInfoIt).m_iUnitQty)
            {
               if(in_Side == ECombatSide::Attacker)
               {
                  l_fFurthestPosition = min(l_fFurthestPosition,(*l_UnitTypeCbtInfoIt).m_fPosition);               
               }
               else
               {
                  l_fFurthestPosition = max(l_fFurthestPosition,(*l_UnitTypeCbtInfoIt).m_fPosition);
               }               
            }

            l_UnitTypeCbtInfoIt++;
         }
         
      }
      l_ActorIt++;
   }
   return l_fFurthestPosition;
}



/*!
 * Notifies an action to the human clients
 * Builds the packet and broadcasts it through the network
 **/
void SP2::GArena::NotifyActionToClients(ECombatAction::Enum in_eAction,UINT32 in_ID1,UINT32 in_ID2,UINT32 in_iValue)
{
   GIBuffer l_Packet;
   GCombatInformationPacket l_Data;
   l_Data.m_iAction     = in_eAction;
   l_Data.m_iArenaID    = (INT16)Info()->m_iId;
   l_Data.m_iID1        = (INT16)in_ID1;
   l_Data.m_iID2        = (INT16)in_ID2;
   l_Data.m_iValue      = in_iValue;   
   l_Packet << c_iCombatPacketIdentifier << l_Data;

   g_ServerDCL.BroadcastUDPPacket((void*)l_Packet.Data().get(), (short)l_Packet.Size());
   return;
}


/*!
 * Update the action (attacking, defending) of a unit type in the arena
 **/
bool SP2::GArena::UpdateUnitAction(UINT32 in_iCountryID,UINT32 in_iUnitTypeID,bool in_bAttack)
{
   SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)Info();
   SP2::GActorInfo* l_pActor     = l_pArenaInfo->Actor(in_iCountryID);
   if(!l_pActor)
   {
      g_Joshua.Log(L"Invalid actor, combat is over ?");
      return false;
   }

   EUnitAction::Enum l_eUnitAction = EUnitAction::Defending;
   if(!in_bAttack)
   {
      l_eUnitAction = EUnitAction::Defending;
   }
   else
   {
      l_eUnitAction = EUnitAction::Attacking;
   }
  
   l_pActor->m_vUnitTypeInfo[in_iUnitTypeID-1].m_eAction = l_eUnitAction;
   
   g_Joshua.UnitManager().ModifyArenaInfo(l_pArenaInfo);

#ifdef LOG_COMBAT
   GString l_sStatus = L"Defending";
   if(l_eUnitAction == EUnitAction::Attacking)
      l_sStatus = L"Attacking";
   g_Joshua.Log(L"Country : " + GString(in_iCountryID) +  L"Unit type " + g_ServerDAL.UnitTypes()[in_iUnitTypeID-1]->Name() + " Is now in status: " + l_sStatus);
#endif
   return true;
}


/*!
 * Refill the missiles of the given unit
 **/
void SP2::GArena::RefillMissiles(const SP2::GUnit* in_pUnit)
{
   //If there are no more missiles and they must be refilled
   if(!in_pUnit->m_pFightInfo->m_iMissileCount && in_pUnit->m_pFightInfo->m_bMustReloadMissilesWhenEmpty)
   {
      //Check if enough time passed so we can refill
      SP2::GUnitType* l_pType = ((SP2::GUnitDesign*)in_pUnit->Design())->Type();
      if((g_CombatManager.Clock() - in_pUnit->m_pFightInfo->m_fNoMoreMissilesClock) >= g_ServerDCL.MissileRearmingDelay(l_pType))
      {
         SP2::GUnitDesign* l_pUnitDesign = (SP2::GUnitDesign*)in_pUnit->Design();
         in_pUnit->m_pFightInfo->m_iMissileCount =  (INT32)l_pUnitDesign->Type()->MissilePayloadByLevel()[l_pUnitDesign->MissilePayload()-1];
      }
   }

}

/*!
 * Find which side the group should be on
 **/
ECombatSide::Enum SP2::GArena::ComputeCombatSide(SP2::GUnitGroupEx* in_pGroup)
{
   SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)m_pArenaInfo;

   //Check if there is already a unit for that actor in the combat, if so, will be on same side
   SP2::GActorInfo* l_pActor = l_pArenaInfo->Actor(in_pGroup->OwnerId());
   if(l_pActor)
   {
      return l_pActor->m_Side;
   }

   //If we reach this point, this country has no units in this combat yet

   //If we are the 1st person in the combat
   if(!l_pArenaInfo->Actors().size())
   {
      //Check our relations with the military control of the region
      UINT32 l_iRegionOwner = g_ServerDAL.RegionControl(in_pGroup->m_iRegionID).m_iMilitary;

      EDiplomaticStatus::Enum l_DiplomaticStatus = g_ServerDAL.DiplomaticStatus(in_pGroup->OwnerId(),l_iRegionOwner);
      switch(l_DiplomaticStatus)
      {
         case EDiplomaticStatus::Neutral:
         case EDiplomaticStatus::Allied:
         {
            return ECombatSide::Defender;
         }
         case EDiplomaticStatus::Hostile:
         {
            return ECombatSide::Attacker;
         }         
         default:
            return ECombatSide::CantDetermine;   
      }
   }
   else
   {
      //Check if we are allowed to join the fight
      //Cases when not allowed to join the fight:
      // 1 - We have allies on both sides
      // 2 - We have enemies on both sides
      bool l_bAlliesOnAttackerSide = false;
      bool l_bAlliesOnDefenderSide = false;
      bool l_bEnemiesOnDefenderSide = false;
      bool l_bEnemiesOnAttackerSide = false;
      //Process the actors and check if we are allowed to join
      for(hash_map<UINT32,GActorInfo>::const_iterator l_It = l_pArenaInfo->Actors().begin();
         l_It != l_pArenaInfo->Actors().end() ; 
         l_It++)
      {
         EDiplomaticStatus::Enum l_DiplomaticStatus = g_ServerDAL.DiplomaticStatus(in_pGroup->OwnerId(),l_It->second.m_iCountryID);
         switch(l_DiplomaticStatus)
         {
            case EDiplomaticStatus::Allied:
            {
               if(l_It->second.m_Side == ECombatSide::Attacker)
               {
                  l_bAlliesOnAttackerSide = true;
               }
               else
               {
                  l_bAlliesOnDefenderSide = true;
               }
               break;
            }
            case EDiplomaticStatus::Hostile:
            {
               if(l_It->second.m_Side == ECombatSide::Attacker)
               {
                  l_bEnemiesOnAttackerSide = true;
               }
               else
               {
                  l_bEnemiesOnDefenderSide = true;
               }
               break;
            }
            case EDiplomaticStatus::Neutral:
            default:
               break;         
         }
      }

      //If allies on both sides, cant join
      IF_RETURN(l_bAlliesOnAttackerSide && l_bAlliesOnDefenderSide,ECombatSide::CantDetermine);
      //If enemies on both sides, cant join
      IF_RETURN(l_bEnemiesOnAttackerSide && l_bEnemiesOnDefenderSide,ECombatSide::CantDetermine);
      //If no enemies or no allies, cant join
      IF_RETURN(!l_bEnemiesOnAttackerSide && !l_bEnemiesOnDefenderSide && !l_bAlliesOnAttackerSide && !l_bAlliesOnDefenderSide,ECombatSide::CantDetermine);

      //If we reach this point, we can join the fight, determine which side we will join
      IF_RETURN(l_bAlliesOnAttackerSide,ECombatSide::Attacker); 
      IF_RETURN(l_bAlliesOnDefenderSide,ECombatSide::Defender);
      IF_RETURN(l_bEnemiesOnAttackerSide,ECombatSide::Defender);
      IF_RETURN(l_bEnemiesOnDefenderSide,ECombatSide::Attacker);
   }

   //Should never be reached
   return ECombatSide::CantDetermine;
}


/*!
 * Updates the arena information unit losses,depending on the side of the units and
 * the unit category
 * @param in_eSide: Side of the units (either attacking or defending)
 * @param in_eCategory: Category for the units (ground,infantry,air,naval...)
 * @param in_iLosses: Qty of units lost
 **/
void SP2::GArena::UpdateArenaInfoUnitLosses(ECombatSide::Enum   in_eSide,
                                            EUnitCategory::Enum in_eCategory,
                                            UINT32              in_iLosses)
{
   gassert(in_eSide < ECombatSide::ItemCount,"Should be smaller");
   gassert(in_eCategory < EUnitCategory::ItemCount,"Should be smaller");

   SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)Info();
   l_pArenaInfo->m_pArenaLosses[in_eSide][in_eCategory] += in_iLosses;
   return;
}

void SP2::GArena::OnIterate(void)
{
   // For every actor in this arena, verify for retreats
   SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)m_pArenaInfo;

   vector<UINT32> l_vCountryToRetreatFromCombat;
   for(hash_map<UINT32,GActorInfo>::const_iterator l_It = l_pArenaInfo->Actors().begin();
       l_It != l_pArenaInfo->Actors().end();
       l_It++)
   {
      const GActorInfo& l_ActorInfo = l_It->second;

      // Make sure any casuality will update country unit values
      g_ServerDAL.DirtyCountryUnitsServer(l_ActorInfo.m_iCountryID);

      if(l_ActorInfo.m_bRetreating)
      {
         bool l_bRetreat = true;
         for(UINT32 j=0;j<l_ActorInfo.m_vUnitTypeInfo.size();j++)
         {
            GUnitTypeCbtInfo l_CombatInfo = l_ActorInfo.m_vUnitTypeInfo[j];
            if(l_ActorInfo.m_Side == ECombatSide::Defender)
            {
               if(l_CombatInfo.m_iUnitQty && l_CombatInfo.m_fPosition > 0)
               {
                  l_bRetreat = false;
                  break;
               }
            } 
            else
            {
               if(l_CombatInfo.m_iUnitQty && l_CombatInfo.m_fPosition < 100)
               {
                  l_bRetreat = false;
                  break;
               }

            }
         }
         if(l_bRetreat)
         {
            l_vCountryToRetreatFromCombat.push_back(l_ActorInfo.m_iCountryID);
         }
      }
   }
   for(UINT32 i = 0 ; i < l_vCountryToRetreatFromCombat.size() ; i++)
   {      
      g_ServerDCL.RetreatFromCombat(l_pArenaInfo,l_vCountryToRetreatFromCombat[i]); 
   }
}

bool SP2::GArena::Unserialize(GOBuffer& io_Buffer)
{
   __super::Unserialize(io_Buffer);

   for(list<SDK::Combat::GUnitGroup*>::const_iterator l_GrpIt = m_FightingGroups.begin();
       l_GrpIt != m_FightingGroups.end();
       l_GrpIt++)
   {
      SP2::GUnitGroupEx* l_pGroup = (SP2::GUnitGroupEx*) *l_GrpIt;
      gassert(l_pGroup->m_iArenaInfoId == Info()->m_iId, "Unit group not in same combat");
      l_pGroup->m_pArenaInfo = Info();
   }

   return true;
}
