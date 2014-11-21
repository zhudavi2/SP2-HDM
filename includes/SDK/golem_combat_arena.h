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

#ifndef _GOLEM_ARENA_H_
#define _GOLEM_ARENA_H_

namespace SDK
{
   namespace Combat
   {
      /*!
       * Enum to give the status the battle that is 
       * being executed in the arena.
       **/
      namespace EBattleStatus
      {
         enum Enum
         {
            NotOver,
            Over,
            Aborted,
            Tie
         };
      };

      /*!
       * Information on the combat that is happening
       **/
      class GOLEM_SDK_API GArenaInfo : public GSerializable
      {
      public:

         virtual bool Serialize(GIBuffer& out_Buffer) const;
         virtual bool Unserialize(GOBuffer& in_Buffer);


         UINT32                m_iId;
         GVector2D<REAL32>     m_Coordinate;
      };




      /*!
       * Battle arena, is an object to handle combats
       * between units.  It contains units that are fighting together
       * and over time, there will be a winner.
       **/
      class GOLEM_SDK_API GArena : public GSerializable
      {
         friend class GManager;
      public:

         //! Iterate Arena
         virtual void OnIterate(void) {};

         //! Adds a unit to the combat
         virtual bool AddGroup(Combat::GUnitGroup*  in_pGroup);
         virtual bool RemoveGroup(Combat::GUnitGroup*  in_pGroup);
         //!Event called when units are killed, no changes must be done on the unit since the generic part of the engine will do them
         virtual void OnUnitsHit(const Combat::GUnit* in_pAttackedUnit, UINT32 in_iDamageTaken,const Combat::GUnit* in_pAttacker){}; 

         //! Get information on the combat arena
         GArenaInfo* Info();

         const list<GUnitGroup*>& FightingGroups() const {return m_FightingGroups;}

         virtual bool Serialize(GIBuffer& out_Buffer) const;
         virtual bool Unserialize(GOBuffer& in_Buffer);

      protected:

         GArena();
         virtual ~GArena();

         //! Iterate the Arena
         EBattleStatus::Enum Iterate();
         //! Pure virtual function automatically called so the attacker picks a target
         virtual GUnit* SelectTarget(const GUnit* in_pAttacker) = 0;
         //! Pure virtual function automatically called so the unit moves around in the arena
         virtual void   MoveUnit(const GUnit* in_pMovingUnit) = 0;
         //! Pure virtual function automatically called to know which one is the next unit to act (given the actual time). while it returns something it is called, parameter out_fForceModifier is a modifier that must be applied to the damage (can be used to avoid performing actions too often)
         virtual GUnit* NextActingUnit(REAL32& out_fForceModifier) = 0;


         //! Checks the combat status (is over ? or not ? etc.)
         virtual EBattleStatus::Enum CombatStatus() = 0;

         //! List the IDs of the owner of the units present in the fight
         set<UINT32> ListPresentOwnerIDs();

         list<GUnitGroup*>      m_FightingGroups;
         GArenaInfo*            m_pArenaInfo;

      private:

         //! Applies the combat damage to the unit and cleans the group if it must
         void ApplyDamageAndCleanGroup(SDK::Combat::GUnit* in_pTarget,UINT32 in_iDamage);

      };//Eng class GArena


   };//end namespace Combat
}

#endif //_GOLEM_ARENA_H_
