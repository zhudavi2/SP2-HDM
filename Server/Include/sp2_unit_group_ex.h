/**************************************************************
*
* sp2_unit_group_ex.h
*
* Description
* ===========
*  Describes the interface of class SP2:GUnitGroupEx, which
*  completes the SP2::GUnitGroup class with processing 
*  information.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _SP2_UNIT_GROUP_EX_H_
#define _SP2_UNIT_GROUP_EX_H_

namespace SP2
{
   class GUnitGroupEx : public SP2::GUnitGroup
   {
   public:
      GUnitGroupEx();

      //! Construction function
      static ::SDK::Combat::GUnitGroup* New() { return new GUnitGroupEx; }

      // ::SDK::Combat::GUnitGroup method overload
      virtual void EnterCombat(SDK::Combat::GArena* in_pCombat);
      virtual void LeaveCombat();

      // Serialization overload
      virtual bool Serialize(GIBuffer& io_Buffer) const { return __super::Serialize(io_Buffer); }
      virtual bool Unserialize(GOBuffer& io_Buffer) { return __super::Unserialize(io_Buffer); }

      //! Current region where the group is located
      UINT32                  m_iRegionID;

      //! Action to perform when group reaches the destination
      EMilitaryAction::Flags  m_ActionAtDest;

      //! Current move region where the group is located
      UINT32                  m_iMoveRegionID;

      //! Previous move region where the group is located
      UINT32                  m_iPrevMoveRegionID;

      //! Value of the group content at start of movement
      REAL64                  m_fLastMoveValue;

   protected:
      //! Save NextStatus if combat occurs while changing status
      EMilitaryStatus::Enum   m_eSavedNextStatus;
   };
}

#endif // #ifndef _SP2_UNIT_GROUP_EX_H_
