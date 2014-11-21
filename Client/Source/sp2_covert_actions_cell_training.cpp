/**************************************************************
*
* sp2_covert_actions_new_cell.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_covert_actions_cell_training.h"
#include "../include/sp2_covert_actions_window.h"
#include "../include/sp2_generic_message_window.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GCovertActionsCellTrainingWindow::TypeName(L"Covert_Actions_Cell_Training");

GUI::GBaseObject* GCovertActionsCellTrainingWindow::New()
{
	return (GUI::GBaseObject*) new GCovertActionsCellTrainingWindow();
}


GCovertActionsCellTrainingWindow::GCovertActionsCellTrainingWindow() : GBaseWindow()
{
   m_viID.clear();
   m_vTrain.clear();
}

GCovertActionsCellTrainingWindow::~GCovertActionsCellTrainingWindow()
{
}

bool GCovertActionsCellTrainingWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjApplyBtn->Hide();
   m_pObjLockTog->Hide();

   m_pObjInnerFrm       = (GUI::GFrame*)Child(L"frmInner");
   
   m_pObjNameLbl        = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmCell")->Child(L"txtName");
   m_pObjTrainingLbl    = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmCell")->Child(L"txtTraining");
   m_pObjSetupCostLbl   = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmCell")->Child(L"txtCost");
   m_pObjUpkeepLbl      = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmCell")->Child(L"txtUpkeep");
   m_pObjUpdateTimeLbl  = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmCell")->Child(L"txtTime");

   m_pObjConfirmBtn     = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnConfirm");
   m_pObjCancelBtn      = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnCancel");

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GCovertActionsCellTrainingWindow::OnShow()
{
   __super::OnShow();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

GUI::EEventStatus::Enum GCovertActionsCellTrainingWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);

   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjConfirmBtn)
         {
            for(UINT32 i = 0; i < m_viID.size(); i++)
            {
               SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventCellUpdateStatus);
               SP2::Event::GEventCellUpdateStatus* l_pEvent = (SP2::Event::GEventCellUpdateStatus*)l_Event.get();

               l_pEvent->m_iSource = g_SP2Client->Id();
               l_pEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
               l_pEvent->m_bTrain  = true;
               l_pEvent->m_iCellID = m_viID[i];

               g_ClientDCL.ClientAdvisor().RaiseEvent(l_Event);

               //Locally change the cell so we dont have to wait for server events
               GCovertActionCell*      l_pCell = &g_ClientDAL.m_PlayerCountryData.CovertActionCell(m_viID[i]);
               gassert(l_pCell,"Cell should exist");
               if(l_pCell)
               {
                  l_pCell->ChangeState(ECovertActionsCellState::Training);
                  g_ClientDDL.CovertActionsWindow()->Update();
               }
            }
            Hide();
         }
         else if(in_pCaller == m_pObjCancelBtn)
         {
            Hide();
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

void GCovertActionsCellTrainingWindow::CalculateCost()
{
   REAL64 l_fCost  = 0.f;
   UINT32 l_iMonth = 0;
   REAL64 l_fUpkp  = 0.f;

   for(UINT32 i = 0; i < m_vTrain.size(); i++)
   {
      switch(m_vTrain[i])
      {
      case ECovertActionCellTraining::Recruit:
         l_fCost  += SP2::c_fCellTrainingCostRegular;
         l_iMonth += SP2::c_iCellCreationTimeMonthRegular;
         l_fUpkp  += c_fCellUpkeepBase * c_fCellUpkeepModifierRegular;
         break;
      case ECovertActionCellTraining::Regular:
         l_fCost  += SP2::c_fCellTrainingCostVeteran;
         l_iMonth += SP2::c_iCellCreationTimeMonthVeteran;
         l_fUpkp  += c_fCellUpkeepBase * c_fCellUpkeepModifierVeteran;
         break;
      case ECovertActionCellTraining::Veteran:
         l_fCost  += SP2::c_fCellTrainingCostElite;
         l_iMonth += SP2::c_iCellCreationTimeMonthElite;
         l_fUpkp  += c_fCellUpkeepBase * c_fCellUpkeepModifierElite;
         break;
      }
   }

   m_pObjSetupCostLbl->Text( GString::FormatNumber(l_fCost, L" ", L"", L"$ ", L"", 3, 0, false) );
   m_pObjUpkeepLbl->Text( GString::FormatNumber(l_fUpkp, L" ", L"", L"$ ", L"", 3, 0, false) );

   if(m_vTrain.size() == 1)
      m_pObjUpdateTimeLbl->Text( GString(l_iMonth) + GString(L" ") + g_ClientDAL.GetString(EStrId::Month) );
   else
      m_pObjUpdateTimeLbl->Text( L"-");
}

void GCovertActionsCellTrainingWindow::Update(UINT32 in_iID, GString in_sName, SP2::ECovertActionCellTraining::Enum in_Level)
{
   m_vTrain.push_back(in_Level);
   m_viID.push_back(in_iID);
   if( m_pObjNameLbl->Text().length() )
      m_pObjNameLbl->Text( L"-" );
   else
      m_pObjNameLbl->Text( in_sName );

   if(m_vTrain.size() == 1)
   {
      switch(m_vTrain[0])
      {
      case ECovertActionCellTraining::Recruit:
         m_pObjTrainingLbl->Text( g_ClientDAL.ConvertCellTrainingToString(ECovertActionCellTraining::Regular) );
         break;
      case ECovertActionCellTraining::Regular:
         m_pObjTrainingLbl->Text( g_ClientDAL.ConvertCellTrainingToString(ECovertActionCellTraining::Veteran) );
         break;
      case ECovertActionCellTraining::Veteran:
         m_pObjTrainingLbl->Text( g_ClientDAL.ConvertCellTrainingToString(ECovertActionCellTraining::Elite) );
         break;
      }
   }
   else
   {
      m_pObjTrainingLbl->Text( L"-" );
   }

   CalculateCost();
}

void GCovertActionsCellTrainingWindow::CleanUp()
{
   m_viID.clear();
   m_vTrain.clear();
   m_pObjNameLbl->Text( L"" );
}
