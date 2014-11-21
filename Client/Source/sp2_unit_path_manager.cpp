/**************************************************************
*
* sp2_unit_path_manager.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

GUnitPathManager::GUnitPathManager()
{
   // Attach to unit manager to get notify when groups are deleted or moved
   g_Joshua.UnitManager().Attach(this, Combat::c_notificationUnitGroupRemoved);
   g_Joshua.UnitManager().Attach(this, Combat::c_notificationUnitGroupModified);
}


GUnitPathManager::~GUnitPathManager()
{
   if(&g_Joshua.UnitManager())
      g_Joshua.UnitManager().Detach(this);
}

void GUnitPathManager::Iterate(REAL64 in_fTime)
{
   const stdext::hash_map<UINT32, SDK::Combat::GUnitGroup*>& l_Groups = g_Joshua.UnitManager().UnitGroups();
   stdext::hash_map<UINT32, SDK::Combat::GUnitGroup*>::const_iterator l_GroupIt;

   set<UINT32>::iterator l_It = m_GroupsWithPath.begin();
   while(l_It != m_GroupsWithPath.end() )
   {
      // Find the group ptr
      l_GroupIt = l_Groups.find(*l_It);

      // The ptr should always exist
      gassert(l_GroupIt != l_Groups.end(), "Problem synchronizing group paths");

      // Update the current group path
      if( ( (SP2::GUnitGroup*) l_GroupIt->second)->UpdatePath(in_fTime) )
      {
         // Remove the path if complete
         m_GroupsWithPath.erase(l_It++);
         ( (SP2::GUnitGroup*) l_GroupIt->second)->Path().Reset();
      }
      else
      {
         ++ l_It;
      }
   }
}

void GUnitPathManager::OnSubjectChange(DesignPattern::GSubject&  in_Subject,const DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg  )
{
   if(&in_Subject == &g_Joshua.UnitManager())
   {
      if(in_Notification == Combat::c_notificationUnitGroupRemoved)
      {
         SynchronizeDeletedGroup();
      }
      else if(in_Notification == Combat::c_notificationUnitGroupModified)
      {
         SynchronizePath();
      }
   }
}

void GUnitPathManager::SynchronizePath(void)
{
   // Add modified groups with a path,
   // remove modified groups with no paths,
   // locate arena info of modified groups
   hash_map<UINT32, SDK::Combat::GUnitGroup*>::const_iterator l_It = g_Joshua.UnitManager().ModifiedUnitGroups().begin();
   set<UINT32>::iterator l_PathIt;
   while(l_It != g_Joshua.UnitManager().ModifiedUnitGroups().end() )
   {
      SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*) (l_It->second);
      l_PathIt = m_GroupsWithPath.find(l_pGroup->Id() );

      if(l_pGroup->m_iArenaInfoId)
      {
         l_pGroup->m_pArenaInfo = g_Joshua.UnitManager().ArenaInfo(l_pGroup->m_iArenaInfoId);
      }

      // Different actions if group with a path was already present
      if(l_PathIt != m_GroupsWithPath.end() )
      {
         // Verify if path is still present
         if(l_pGroup->Path().Points().size() == 0)
         {
            // No more path on this group, remove it from our list
            m_GroupsWithPath.erase(l_PathIt);
         }
      }
      else
      {
         // Verify if path was added to the group
         if(l_pGroup->Path().Points().size() > 0)
         {
            // A path is present, add it to our list
            m_GroupsWithPath.insert(l_pGroup->Id() );
         }
      }

      ++ l_It;
   }
}
void GUnitPathManager::SynchronizeDeletedGroup(void)
{
   // Remove deleted groups
   set<UINT32>::const_iterator l_RemovedIt = g_Joshua.UnitManager().RemovedUnitGroups().begin();
   while(l_RemovedIt != g_Joshua.UnitManager().RemovedUnitGroups().end() )
   {
      m_GroupsWithPath.erase(*l_RemovedIt);
      ++ l_RemovedIt;
   }
}

void GUnitPathManager::Cleanup()
{
   m_GroupsWithPath.clear();
}