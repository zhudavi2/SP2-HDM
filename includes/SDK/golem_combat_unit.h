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

#ifndef _GOLEM_COMBAT_UNIT_H_
#define _GOLEM_COMBAT_UNIT_H_

namespace SDK
{
   namespace Event
   {
      class GSyncCombatData;
   }

   namespace Combat
   {

     /*!
      * Unit reprentation, can be inherited, and since must be sent via the network
      * the Class must implement serialize and unserialize if it has been inherited.
      **/
      class GOLEM_SDK_API GUnit : public GSerializable
      {
         friend class GUnitManager;
         friend class GUnitGroup;
         friend class GArena;

         //Unit combat functions
         virtual bool   HitCheck(const GUnit* in_pUnit) = 0; //!<Checks if unit Hits the other unit
         virtual UINT32 Damage  (const GUnit* in_pUnit) = 0; //!<Return damage done to the other unit, in terms of HP
         virtual UINT32 CounterAttack(const GUnit* in_pUnit, UINT32 in_iDamage){return 0;} //!<Returns the damage done in the counter attack, in terms of HP
         virtual void   EnterCombat(const GArena*){};
         virtual void   LeaveCombat(){};



      public:
         GUnit();
         virtual ~GUnit();

         void Id(const UINT32 in_iID);

         //Set / Get the design
         void  Design(const Design::GUnit* const in_pDesign);
         inline const Design::GUnit* Design() const {  gassert(m_pDesign,"Design shouldnt be NULL"); return m_pDesign;   };

         //Set / Get Id
         inline UINT32 Id() const;

         //Set / Get the quantity of units of this type present in this GUnit

         inline UINT32 HP() const { return m_iHP;} ;
         void  HP(const UINT32 in_iHP);

         GUnitGroup* Group() const;
         void        Group(GUnitGroup* in_pGroup);

         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

      private:
         UINT32                           m_iId;
         UINT32                           m_iHP;
         const Design::GUnit*             m_pDesign;
         GUnitGroup*                      m_pGroup;

      protected:
      private:
      };//End class GUnit
   };//end namespace Combat
}

#endif //_GOLEM_COMBAT_UNIT_H_
