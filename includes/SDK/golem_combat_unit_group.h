/**************************************************************
*
* golem_combat_unit_group.h
*
* Description
* ===========
*  Describes the interface of class GUnitGroup
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#pragma once

namespace SDK
{
   namespace Event
   {
      class GSyncCombatData;
   }

   namespace Combat
   {
      class GUnit;
      class GArena;

      /*!
       * A unit group is a bunch of units that behave like
       * one.
       **/
      class GOLEM_SDK_API GUnitGroup
      {  
         friend class GUnitManager;
         friend class SDK::Event::GSyncCombatData;
         friend class SDK::Combat::GArena;

      public:
         GUnitGroup();
         virtual ~GUnitGroup();

         const GVector2D<REAL32>&   Position() const;

         const vector<GUnit*>&      Units() const;
         GUnit*                     Unit(const UINT32 in_iID) const;

         //! Get the owner id of this group
         UINT32            OwnerId() const;
         void					OwnerId(const UINT32 in_iID);
         UINT32            Id() const;
         void              Id(const UINT32 in_iID);

         //! Function called when group enters a combat
         virtual void      EnterCombat(GArena* in_pCombat);

         //! Function called when group is no longer in combat
         virtual void      LeaveCombat();

         //! Function called when group content is modified
         virtual void      ContentChanged();

         virtual bool      Serialize(GIBuffer&) const;
         virtual bool      Unserialize(GOBuffer&);

      protected:
         void              Position(const GVector2D<REAL32>& in_Position);

      private:

         void              UnitAdd(GUnit* in_pUnit);
         bool              UnitRemove(GUnit* in_pUnit);
         bool              UnitReplace(GUnit* in_pNewUnit);
         void              Units(const vector<GUnit*>& in_vUnits);

         UINT32            m_iId;
         UINT32            m_iOwnerID;
         GVector2D<REAL32> m_Position;
         vector<GUnit*>    m_vUnits;

      };//End class GUnitGroup


   };//end namespace Combat
}
