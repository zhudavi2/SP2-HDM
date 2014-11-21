/**************************************************************
*
* golem_unit.h
*
* Description
* ===========
*  Describes the interface of class GUnit
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SP2_UNIT_H_
#define _GOLEM_SP2_UNIT_H_

namespace SP2
{

   namespace EUnitWeapon
   {
      enum Enum
      {
         Missiles,
         Guns,
         Undefined
      };
   };

   class GFightingInformation
   {
   public:
      GFightingInformation();
      ~GFightingInformation();
      REAL64 m_fLastActionClock;
      REAL64 m_fNoMoreMissilesClock;
      INT32  m_iMissileCount;
      bool   m_bMustReloadMissilesWhenEmpty;
      bool   m_bWillNeverHitAnyActualUnit;

   };


   class GArena;
   class GUnitMover;
   /*!
   * SP2 Representation of the unit inside SP2
   **/
   class GUnit : public ::SDK::Combat::GUnit
   {
      friend class SP2::GArena;
      friend class SP2::GUnitMover;
		friend class GDataControlLayer;
   public:
      GUnit();
      ~GUnit();


      bool IsNaval() const;

      static ::SDK::Combat::GUnit* New();

      GFightingInformation* FightInfo() const;

      UINT32 Qty() const;

      bool Serialize(GIBuffer&) const;
      bool Unserialize(GOBuffer&);

      bool SerializeFightInfo(GIBuffer&) const;
      bool UnserializeFightInfo(GOBuffer&);

      ETrainingLevel::Enum Training() const;
      void Training(ETrainingLevel::Enum in_eTraining);



      //! Calculate Firing Delay in secs
      REAL32 FiringDelay() const; 

      //! Set HP so that all units are "complete" (33.25 units goes to 34)
      void MaximizeHP();

      //! Indicate if the unit can be merged with the proposed one
      bool CanMergeWith(const SP2::GUnit* in_pUnit) const;

      //! Merge current unit with the quantity of the specified unit
      /*!
       * Please note that this function does not touch the unit that is passed to the function,
       * but it does indicate to the file manager that the current unit (this)
       * is modified.
       **/
      void SP2::GUnit::MergeWithUnit(const SP2::GUnit* in_pUnit);

   private:
      GFightingInformation* m_pFightInfo;

      ETrainingLevel::Enum m_eTraining;

      //Implementation of the virtual pure functions
      bool   HitCheck(const SDK::Combat::GUnit* in_pTarget);    //!<Checks if unit touches the other unit
      UINT32 Damage  (const SDK::Combat::GUnit* in_pTarget);    //!<Return damage done to the other unit
      void   EnterCombat(const SDK::Combat::GArena* in_pArena); //!<Enter combat event
      UINT32 CounterAttack(const SDK::Combat::GUnit* in_pUnit, UINT32 in_iDamage); //!<Performs a counter attack
      void   LeaveCombat();

      //! Check if we hit using missiles
      EUnitWeapon::Enum   SelectWeapon(SP2::GUnit* in_pTarget,REAL32 in_fDistance) const;
      REAL32              DamageReductionRatio(REAL32 in_fArmor);  
   };

};//end namespace SP2
#endif //_GOLEM_UNIT_H_
