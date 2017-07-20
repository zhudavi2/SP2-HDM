/**************************************************************
*
* sp2_unit_group_ex.cpp
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

//#define __LOG_GROUP_MODIFICATIONS__


#ifdef __LOG_GROUP_MODIFICATIONS__
   #define LOG_MODIF(text) (g_Joshua.Log(text) );
#else //#ifdef __LOG_GROUP_MODIFICATIONS__
   #define LOG_MODIF(text) ;
#endif //#ifdef __LOG_GROUP_MODIFICATIONS__

SP2::GUnitGroupEx::GUnitGroupEx()
{
   m_iMoveRegionID     = 0;
   m_iPrevMoveRegionID = 0;
   m_iRegionID         = 0;
   m_ActionAtDest      = EMilitaryAction::Default;
   m_fLastMoveValue    = 0;
   m_eSavedNextStatus  = EMilitaryStatus::Parked;
}

void SP2::GUnitGroupEx::EnterCombat(SDK::Combat::GArena* in_pCombat)
{
   LOG_MODIF( GString(L"EnterCombat : ") + GString(Id() ) + GString(L" -> ") + GString(in_pCombat->Info()->m_iId) );

#ifdef GOLEM_DEBUG
   GDZLOG(EDZLogLevel::Info2, GString(L"Unit group enters combat ") + GString(Id() ));
#endif
   if(m_eStatus != EMilitaryStatus::Attacking)
   {
      m_eCombatStatus    = m_eStatus;
      m_eSavedNextStatus = m_eNextStatus;
      ChangeStatus(EMilitaryStatus::Attacking);
   }

   //If we were in the bombardment list, remove it from there
   {
      //If we were in the bombardment list
      bool l_bPresentInBombardmentList = false;
      {
         const hash_set<UINT32>& l_BombardingGroups = g_ServerDCL.UnitMover().BombardingGroupList();
         hash_set<UINT32>::const_iterator l_It = l_BombardingGroups.find(Id());
         if(l_It != l_BombardingGroups.end())
            l_bPresentInBombardmentList = true;
         else
            l_bPresentInBombardmentList = false;
      }      
      if(l_bPresentInBombardmentList)
      {
         g_ServerDCL.UnitMover().RemoveBombardingGroup(Id());

         //Make sure the flag that says the unit was supposed to bombard is there
         gassert(m_ActionAtDest & EMilitaryAction::BombardRegion,"Unit should be flagged as bombaring");
      }
   }

   m_pArenaInfo = in_pCombat->Info();
   g_Joshua.UnitManager().ModifyUnitGroup(this);
}

void SP2::GUnitGroupEx::LeaveCombat()
{
   GDZLOG(EDZLogLevel::Info2, GString(L"Unit group leaves combat ") + GString(Id() ) + L" status " + GString( (UINT32) m_eStatus) + L" combat status " + GString( (UINT32) m_eCombatStatus));

   if(m_eStatus == EMilitaryStatus::Attacking)
   {
      ChangeStatus(m_eCombatStatus);
      gassert(m_eStatus == m_eCombatStatus, L"Current status " + GString(m_eStatus) + L" wasn't properly updated to saved status " + GString(m_eCombatStatus) + L" before combat");
      
      if(g_ServerDCL.UnitMover().IsUnitGroupChangingStatus(Id()))
      {
          if(m_eStatus != m_eSavedNextStatus)
          {
              GDZLOG(EDZLogLevel::Info1, L"Unit group " + GString(Id()) + L", status " + GString(m_eStatus) + L", will have its NextStatus changed from " + GString(m_eNextStatus) + L" to " + GString(m_eSavedNextStatus) + L" to match pre-combat state");

              m_eNextStatus = m_eSavedNextStatus;
          }
      }
      else
          m_eNextStatus = m_eStatus;
   }

   //If unit was supposed to be bombarding (and can still bombard), resume bombardments
   if(m_ActionAtDest & EMilitaryAction::BombardRegion)
   {
      if(CanPerformOffShoreBombardment())
      {
         g_ServerDCL.UnitMover().AddBombardingGroup(Id());
      }
      else
      {
         m_ActionAtDest &= ~EMilitaryAction::BombardRegion;
      }
   }

   m_pArenaInfo = NULL;
   g_Joshua.UnitManager().ModifyUnitGroup(this);
}
