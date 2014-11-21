/**************************************************************
*
* sp2_unit_group.h
*
* Description
* ===========
*  Describes the interface of class GUnitGroup
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_SP2_UNIT_GROUP_H_
#define _GOLEM_SP2_UNIT_GROUP_H_

namespace SP2
{
   /*!
    * Representation of a SP2 unit group
    **/
   class GUnitGroup : public ::SDK::Combat::GUnitGroup
   {
      friend class GUnitPathManager;
      friend class GArena;

   public:
      //! Construction function
      static ::SDK::Combat::GUnitGroup* New();

      //! Get the status of the unit group (only valid server side after an iteration of the unit mover)
      EMilitaryStatus::Enum Status() const;

      //! Get the status at the specified time (called client side for accurate info)
      EMilitaryStatus::Enum Status(REAL32 in_fTime) const;

      //! Change the status of the unit group instantly
      void ChangeStatus(EMilitaryStatus::Enum in_eNewStatus);

      //! Change the status of the unit group over time
      void ChangeStatus(EMilitaryStatus::Enum in_eNewStatus, REAL32 in_fTime, REAL32 in_fChangeLength);

      //! Get information about the current status change
      EMilitaryStatus::Enum NextStatus() const;

      //! Get information about when the current status starts to change
      REAL32 StatusChangeStartTime() const;

      //! Get information about when the current status change will be complete
      REAL32 StatusChangeEndTime() const;

      //! Get the status change progress
      REAL32 StatusChangeProgress(REAL32 in_fTime) const;

      //! Get the combat status of the unit (only valid when status is Attacking)
      EMilitaryStatus::Enum CombatStatus() const;

      //! Get the current battle arena in which the group is ingaged, read only (NULL if not in combat)
      const SDK::Combat::GArenaInfo*   ArenaInfo() const { return m_pArenaInfo; }

      //! Get the current battle arena in which the group is ingaged (NULL if not in combat)
      SDK::Combat::GArenaInfo*   ArenaInfo() { return m_pArenaInfo; }

      //! Gets the value in $ of the unit group
      REAL64 Value() const;

      //! Specifies if a group contains the required elements to be able to perform offshore bombardments
      bool CanPerformOffShoreBombardment() const;

      //! Content change
      void      ContentChanged();

      //! Return the current path of the group
      GUnitPath& Path() { return m_Path; }

      //! Return the current path of the group (read only)
      const GUnitPath& Path() const { return m_Path; }

      //! Update the current group path
      bool UpdatePath(REAL64 in_fTime);

      //! Gets if the group is a naval unit or not
      bool IsNaval() const;

      //! Return true if this unit can be see
      bool IsVisible(UINT8 in_iGroundResearchStealthValue, UINT8 in_iAirResearchStealthValue, UINT8 in_iNavalResearchStealthValue) const;


      //! Calculate and return the stealth value of the group
      void CalculateStealthValue(void);

      UINT16 RegionToBombard() const;
      void   RegionToBombard(UINT16 in_iRegion);

   protected:
      //! Contructor
      GUnitGroup();

      //! Destructor
      ~GUnitGroup();

      // Serialization overload
      virtual bool Serialize(GIBuffer&) const;
      virtual bool Unserialize(GOBuffer&);

      EMilitaryStatus::Enum      m_eStatus;
      EMilitaryStatus::Enum      m_eNextStatus;
      REAL32                     m_fTimeAtPrevStatus;
      REAL32                     m_fTimeAtNextStatus;

      UINT16                     m_iRegionToBombard;
      mutable bool               m_bUnitGroupIsDirtyForNaval;
      mutable bool               m_bIsNaval;

      EMilitaryStatus::Enum      m_eCombatStatus;

      union
      {
         SDK::Combat::GArenaInfo*   m_pArenaInfo;
         UINT32                     m_iArenaInfoId;
      };

      GUnitPath                  m_Path;
   };
}

#endif //_GOLEM_SP2_UNIT_GROUP_H_
