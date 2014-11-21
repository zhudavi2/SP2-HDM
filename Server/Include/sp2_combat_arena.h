/**************************************************************
*
* golem_arena.h
*
* Description
* ===========
*  Describes the interface of class GArena
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SP2_ARENA_H_
#define _GOLEM_SP2_ARENA_H_

namespace SP2
{
const REAL32 c_fTargetScoreCantHitForNow = -1.0f;
const REAL32 c_fTargetScoreWillNeverHit  = -2.0f;

   /*!
   * SP2 representation of a combat Arena
   **/
   class GArena : public ::SDK::Combat::GArena
   {
      friend class GUnitMover;
      friend class GServer;
   public:
      GArena();
      ~GArena();

      //!Event called when units are killed, no changes must be done on the unit since the generic part of the engine will do them
      void OnUnitsHit(const SDK::Combat::GUnit* in_pAttackedUnit, UINT32 in_iDamageTaken, const SDK::Combat::GUnit* in_pAttacker); 
      
      //! Iterate Call everttime the actor is updated
      void OnIterate(void);

      //! Adds a unit to the combat
      bool AddGroup(SDK::Combat::GUnitGroup*  in_pGroup);
      bool RemoveGroup(SDK::Combat::GUnitGroup*  in_pGroup);


      //! Update the action (attacking, defending) of a unit type in the arena
      bool UpdateUnitAction(UINT32 in_iCountryID,UINT32 in_iUnitTypeID,bool in_bAttack);

   protected:

      virtual bool Unserialize(GOBuffer& io_Buffer);

   private:
      static SDK::Combat::GArena* New();

      //! Find which side the group should be on
      ECombatSide::Enum ComputeCombatSide(SP2::GUnitGroupEx* in_pGroup);

      //! Pure virtual function automatically called so the attacker picks a target
      SDK::Combat::GUnit* SelectTarget(const SDK::Combat::GUnit* in_pAttacker);
      
      //! Pure virtual function automatically called so the unit moves around in the arena
      void   MoveUnit(const SDK::Combat::GUnit* in_pMovingUnit);
      
      //! Pure virtual function automatically called to know which one is the next unit to act (given the actual time). while it returns something it is called
      GUnit* NextActingUnit(REAL32& out_fForceModifier);

      //! Notifies an action to the human clients
      void NotifyActionToClients(ECombatAction::Enum in_eAction,UINT32 in_ID1,UINT32 in_ID2,UINT32 in_iValue);

      SDK::Combat::EBattleStatus::Enum CombatStatus();

      //! Computes the covered distance given the time and the speed, using an arctan equation
      REAL32 CoveredDistance(REAL64 in_fClockDelta,REAL32 in_fSpeed);

      //! To evaluate if the target is a good one
      REAL32 TargetScore(SP2::GUnit* in_pAttacker,SP2::GActorInfo* in_pAttackerActor,SP2::GUnit* in_pTarget,SP2::GActorInfo* in_pTargetActor);

      //! Get the average speed for a given unit type (actually calculates it each time)
      REAL32 AverageSpeed(UINT32 in_iCountryID, SP2::GUnitType* in_pType);

      //! Gets the position of the Furthest unit given the side of the unit
      REAL32 FurthestUnit(ECombatSide::Enum in_Side);

      //! Refill the missiles of the given unit
      void RefillMissiles(const SP2::GUnit* in_pUnit);

      //! Updates the arena information unit losses
      void UpdateArenaInfoUnitLosses(ECombatSide::Enum in_eSide,
                                     EUnitCategory::Enum in_eCategory,
                                     UINT32 in_iLosses);




      list<SDK::Combat::GUnitGroup*>::iterator m_UnitGroupIterator;
      UINT32                                   m_iActuallyIteratedUnit;

   };

};//End namespace SP2
#endif //_GOLEM_ARENA_H_
