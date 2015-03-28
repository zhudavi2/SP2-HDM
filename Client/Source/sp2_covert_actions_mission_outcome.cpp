/**************************************************************
*
* sp2_covert_actions_mission_outcome.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_covert_actions_mission_outcome.h"
#include "../include/sp2_covert_actions_window.h"
#include "../include/sp2_covert_actions_new_mission.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GCovertActionsMissionOutcomeWindow::TypeName(L"Covert_Actions_Mission_Outcome");

GUI::GBaseObject* GCovertActionsMissionOutcomeWindow::New()
{
	return (GUI::GBaseObject*) new GCovertActionsMissionOutcomeWindow();
}


GCovertActionsMissionOutcomeWindow::GCovertActionsMissionOutcomeWindow() : GBaseWindow()
{
}

GCovertActionsMissionOutcomeWindow::~GCovertActionsMissionOutcomeWindow()
{
}

bool GCovertActionsMissionOutcomeWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm           = (GUI::GFrame*)Child(L"frmInner");
   
   m_pObjCellNameLbl        = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"txtCellName");
   m_pObjMissionTypeLbl     = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"txtMissionType");
   m_pObjTargetSectorLbl    = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"txtSector");
   m_pObjComplexityLbl      = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"txtComplexity");
   m_pObjResourceUnitLbl    = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"txtResourceUnit");
   m_pObjTargetCountryLbl   = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"txtTargetCountry");
   m_pObjFramedCountryLbl   = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"txtFramedCountry");

   m_pObjResultLbl          = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"lblResult");
   m_pObjDetailedResultsLbl = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"txtResult");

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GCovertActionsMissionOutcomeWindow::OnShow()
{
   __super::OnShow();

   BringToFront();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

GUI::EEventStatus::Enum GCovertActionsMissionOutcomeWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {
            Hide();
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

void GCovertActionsMissionOutcomeWindow::Update(UINT32 in_iCellID, bool in_bSuccess, bool in_bCaptured, bool in_bFoundOut)
{
   vector<GCovertActionCell> l_vCells = g_ClientDAL.m_PlayerCountryData.CovertActionCells();
   for(UINT32 i=0; i<l_vCells.size(); i++)
   {
      if(l_vCells[i].ID() == in_iCellID)
      {
         m_pObjCellNameLbl->Text( l_vCells[i].Name() );
         m_pObjMissionTypeLbl->Text( g_ClientDAL.ConvertMissionTypeToString(l_vCells[i].MissionType()) );
         m_pObjComplexityLbl->Text( g_ClientDAL.ConvertMissionComplexityToString(l_vCells[i].MissionComplexity()) );

         if(l_vCells[i].MissionType() == SP2::ECovertActionsMissionType::Espionage || l_vCells[i].MissionType() == SP2::ECovertActionsMissionType::Sabotage)
         {
            m_pObjTargetSectorLbl->Text( g_ClientDAL.ConvertTargetSectorToString(l_vCells[i].TargetSector()) );
            
            if(l_vCells[i].TargetSector() == SP2::ECovertActionsTargetSector::Military)
            {
               m_pObjResourceUnitLbl->Text( g_ClientDAL.ConvertUnitCategoryToString( l_vCells[i].UnitCategory() ) );
            }
            else
            {
               m_pObjResourceUnitLbl->Text( g_ClientDAL.ConvertResourcesToString( l_vCells[i].ResourceType() ) );
            }
         }
         else
         {
            m_pObjTargetSectorLbl->Text( L"" );
            m_pObjResourceUnitLbl->Text( L"" );
         }
         
         m_pObjTargetCountryLbl->Text( g_ClientDAL.Country( l_vCells[i].AssignedCountry() ).Name() ); 
         if(l_vCells[i].CountryToFrame() != 0)
            m_pObjFramedCountryLbl->Text( g_ClientDAL.Country( l_vCells[i].CountryToFrame()  ).Name() );
         else
            m_pObjFramedCountryLbl->Text( g_ClientDAL.GetString(EStrId::None)); 

         if(l_vCells[i].MissionType() == SP2::ECovertActionsMissionType::CoupEtat && in_bSuccess)
         {
            g_Joshua.PassSteamAchievement(2); //CoupEtat
         }

         break;
      }
   }

   if(in_bSuccess)
   {
      m_pObjResultLbl->Text( g_ClientDAL.GetString(EStrId::Success) );
      m_pObjDetailedResultsLbl->Text("");

      if(in_bFoundOut)
      {
          m_pObjDetailedResultsLbl->Text("Success, but target found out that you did it.");
      }
   }
   else
   {
      // Set up the failed text with captured flag
      if(in_bCaptured)
         m_pObjDetailedResultsLbl->Text(g_ClientDAL.GetString(EStrId::CovertActionFailedCaptured));
      else
         m_pObjDetailedResultsLbl->Text(g_ClientDAL.GetString(EStrId::CovertActionFailedNotCaptured));
      // Result text
      m_pObjResultLbl->Text(g_ClientDAL.GetString(EStrId::Failed));
   }
}