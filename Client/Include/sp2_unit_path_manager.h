/**************************************************************
*
* sp2_unit_path_manager.h
*
* Description
* ===========
*  Describes the interface of class SP2::GUnitPathManager
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_SP2_UNIT_PATH_MANAGER_H_
#define _GOLEM_SP2_UNIT_PATH_MANAGER_H_

namespace SP2
{
   /*!
    * Holds all unit group paths for fast iteration
    **/
   class GUnitPathManager : public DesignPattern::GObserver
   {

   public:
      GUnitPathManager();
      ~GUnitPathManager();

      void Iterate(REAL64 in_fTime);
      void Cleanup();

      //! Get a Notification when the UnitManager change 
      void OnSubjectChange(DesignPattern::GSubject&  in_Subject,const DesignPattern::GSubjectNotification& in_Notification ,void* in_pAdditionalMsg );

      //! Synchronize deleted group from unit manager with the list of groups with path.
      void SynchronizeDeletedGroup(void);

      //! Syncrhonize modified group  from unit manager with the list of groups with path.
      void SynchronizePath(void);


   protected:
      set<UINT32>   m_GroupsWithPath;
   };
}

#endif // #ifndef _GOLEM_SP2_UNIT_PATH_MANAGER_H_
