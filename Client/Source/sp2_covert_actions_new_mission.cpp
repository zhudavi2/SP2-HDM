/**************************************************************
*
* sp2_covert_actions_new_mission.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_covert_actions_new_mission.h"
#include "../include/sp2_covert_actions_window.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GCovertActionsNewMissionWindow::TypeName(L"Covert_Actions_New_Mission");

GUI::GBaseObject* GCovertActionsNewMissionWindow::New()
{
	return (GUI::GBaseObject*) new GCovertActionsNewMissionWindow();
}


GCovertActionsNewMissionWindow::GCovertActionsNewMissionWindow() : GBaseWindow()
{
}

GCovertActionsNewMissionWindow::~GCovertActionsNewMissionWindow()
{
}

bool GCovertActionsNewMissionWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm       = (GUI::GFrame*)Child(L"frmInner");

   m_pObjTypeCbo        = (GUI::GComboBox*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"cboMissionType");
   m_pObjSectorCbo      = (GUI::GComboBox*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"cboSector");
   m_pObjComplexityCbo  = (GUI::GComboBox*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"cboComplexity");
   m_pObjResourceCbo    = (GUI::GComboBox*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"cboResourceUnit");
   m_pObjTargetLbl      = (GUI::GLabel*)   m_pObjInnerFrm->Child(L"frmMission")->Child(L"txtCountry");
   m_pObjFramedCbo      = (GUI::GComboBox*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"cboCountry");

   m_pObjResourceLbl    = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"lblResourceUnit");
   m_pObjSectorLbl      = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmMission")->Child(L"lblTargetSector");


   m_pObjConfirm        = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnConfirm");
   m_pObjCancel         = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnCancel");

   m_pObjTypeCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionTypeEspionage) );
   m_pObjTypeCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionTypeSabotage) );
   m_pObjTypeCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionTypeAssassination) );
   m_pObjTypeCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionTypeTerrorism) );
   m_pRowCoupDetat = m_pObjTypeCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionTypeCoupEtat) );

   m_pObjSectorCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionTargetCivilian) );
   m_pObjSectorCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionTargetMilitary) );

   if(m_pObjApplyBtn)
      m_pObjApplyBtn->Hide();

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GCovertActionsNewMissionWindow::OnShow()
{
   __super::OnShow();

   m_pObjTypeCbo->Selected_Content( g_ClientDAL.GetString(EStrId::MissionTypeEspionage) );
   m_pObjSectorCbo->Selected_Content( g_ClientDAL.GetString(EStrId::MissionTargetMilitary) );
   m_pObjComplexityCbo->Selected_Content( g_ClientDAL.GetString(EStrId::MissionComplexityMedium) );
  
   UpdateComboBox();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

void GCovertActionsNewMissionWindow::CellCleanUp()
{
   m_viCellsID.clear();
}

void GCovertActionsNewMissionWindow::AddCell(UINT32 in_iCellID)
{
   m_viCellsID.push_back(in_iCellID);
}

void GCovertActionsNewMissionWindow::Show()
{
   __super::Show();

   UpdateCountryToFrameCbo();
}

void GCovertActionsNewMissionWindow::UpdateCountryToFrameCbo()
{
   vector<GCovertActionCell> l_vCells = g_ClientDAL.m_PlayerCountryData.CovertActionCells();
   vector<SP2::GCountry> l_Country = g_ClientDAL.Countries();
   UINT32 j;

   if(m_pRowCoupDetat)
   {
      m_pRowCoupDetat->m_bHidden = false;
      m_pObjTypeCbo->DropSize( 72 );
      for(UINT32 i = 0; i < m_viCellsID.size(); i++)
      {
         for(j = 0; j < l_vCells.size(); j++)
         {
            if(l_vCells[j].ID() == m_viCellsID[i])
            {
               if( l_vCells[j].AssignedCountry() == (UINT32)g_ClientDAL.ControlledCountryID() )
               {
                  // Cant do a coup detat on yourself, else you could lose the game =/
                  m_pRowCoupDetat->m_bHidden = true;
                  m_pObjTypeCbo->DropSize( 58 );
               }
            }
         }
      }
      gassert(j < l_vCells.size(),"Cell should be in the list");
      m_pObjTypeCbo->Update();
   }

   if(m_viCellsID.size() > 1)
   {
      m_pObjTargetLbl->Text(L"-");
      m_pObjFramedCbo->Enabled(false);
   }
   else
   {
      m_pObjFramedCbo->Enabled(true);
      for(j = 0; j < l_vCells.size(); j++)
      {
         if(l_vCells[j].ID() == m_viCellsID[0])
         {
            m_pObjTargetLbl->Text( g_ClientDAL.Country( l_vCells[j].AssignedCountry() ).Name() );
            break;
         }
      }

      if(j == l_vCells.size())
         return;

      bool l_bCanBreak = false;
      for(UINT32 k = 0; k < l_Country.size(); k++)
      {
         if(l_Country[j].Id() == (INT16)l_vCells[j].AssignedCountry())
         {
            l_Country.erase(l_Country.begin() + k);
            if(l_bCanBreak)
               break;
            else
               l_bCanBreak = true;
         }
      }
   }

   m_pObjFramedCbo->Clear();

   m_pObjFramedCbo->Add_Item(g_ClientDAL.GetString(EStrId::None));
   g_ClientDDL.ComboboxSetCountriesAndFlag(l_Country, m_pObjFramedCbo);
   m_pObjFramedCbo->Selected_Content( g_ClientDAL.GetString(EStrId::None) );
}

GUI::EEventStatus::Enum GCovertActionsNewMissionWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);
   
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjCancel)
            Hide();
         else if(in_pCaller == m_pObjConfirm)
         {
            for(UINT32 i = 0; i < m_viCellsID.size(); i++)
            {
               SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventCellNewMission);
               SP2::Event::GEventCellNewMission* l_pUpdate = (SP2::Event::GEventCellNewMission*)l_Event.get();
               
               l_pUpdate->m_iSource = g_SP2Client->Id();
               l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;

               l_pUpdate->m_iCellID         = m_viCellsID[i];
               l_pUpdate->m_eComplexity     = g_ClientDAL.ConvertMissionComplexityToEnum( m_pObjComplexityCbo->Selected_Content() );
               l_pUpdate->m_eSector         = g_ClientDAL.ConvertTargetSectorToEnum( m_pObjSectorCbo->Selected_Content() );
               
               if( m_pObjFramedCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::None) )
                  l_pUpdate->m_iFramedCountry  = 0;
               else
                  l_pUpdate->m_iFramedCountry  = g_ClientDAL.Country( m_pObjFramedCbo->Selected_Content() ).Id();

               l_pUpdate->m_eType           = g_ClientDAL.ConvertMissionTypeToEnum( m_pObjTypeCbo->Selected_Content() );

               if(m_pObjSectorCbo->Selected_Content() == g_ClientDAL.ConvertTargetSectorToString(SP2::ECovertActionsTargetSector::Military) )
               {
                  l_pUpdate->m_eUnitCategory = g_ClientDAL.ConvertUnitCategoryToEnum( m_pObjResourceCbo->Selected_Content() );
               }
               else if(m_pObjSectorCbo->Selected_Content() == g_ClientDAL.ConvertTargetSectorToString(SP2::ECovertActionsTargetSector::Civilian))
               {
                  l_pUpdate->m_eResourceType = g_ClientDAL.ConvertResourcesToEnum( m_pObjResourceCbo->Selected_Content() );
               }

               g_Joshua.RaiseEvent(l_Event);


               //Locally modify the cell 
               GCovertActionCell*      l_pCell = &g_ClientDAL.m_PlayerCountryData.CovertActionCell(m_viCellsID[i]);
               gassert(l_pCell,"Cell should exist");
               if(l_pCell)
               {
                  l_pCell->TargetSector( l_pUpdate->m_eSector );
                  l_pCell->MissionComplexity( l_pUpdate->m_eComplexity );
                  l_pCell->CountryToFrame( l_pUpdate->m_iFramedCountry );
                  
                  l_pCell->MissionType( l_pUpdate->m_eType );
                  
                  if( l_pUpdate->m_eSector == SP2::ECovertActionsTargetSector::Civilian )
                     l_pCell->ResourceType(l_pUpdate->m_eResourceType);
                  else if( l_pUpdate->m_eSector == SP2::ECovertActionsTargetSector::Military )
                     l_pCell->UnitCategory(l_pUpdate->m_eUnitCategory);

                  if(l_pCell->ActualState() == ECovertActionsCellState::Dormant)
                  {
                     l_pCell->ChangeState(ECovertActionsCellState::GettingReady);
                     l_pCell->SubsequentStateAdd(ECovertActionsCellState::PreparingMission);
                  }
                  else if(l_pCell->ActualState() == ECovertActionsCellState::Active)
                  {
                     l_pCell->ChangeState(ECovertActionsCellState::PreparingMission);
                  }
               
               }
            }

            g_ClientDDL.CovertActionsWindow()->Update();
            Hide();
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

EEventStatus::Enum GCovertActionsNewMissionWindow::OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_Data, SDK::GUI::GBaseObject* in_pCaller)
{
   if(in_iEventID == ECustomEvents::OnComboBoxSelectionEvent)
   {
      if(((GUI::GComboBox*)in_Data.Data) == m_pObjTypeCbo || ((GUI::GComboBox*)in_Data.Data) == m_pObjSectorCbo)
         UpdateComboBox();
   }

   return GUI::EEventStatus::Handled;
}

void GCovertActionsNewMissionWindow::UpdateComboBox()
{
   if(m_pObjTypeCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::MissionTypeEspionage) || 
      m_pObjTypeCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::MissionTypeSabotage) )
   {      
      // reenable the sector cbo if it was previously disabled
      m_pObjSectorCbo->Enabled(true);
      if(m_pObjSectorCbo->Selected_Content() == (L""))
         m_pObjSectorCbo->Selected_Content(g_ClientDAL.GetString(EStrId::MissionTargetCivilian));

      m_pObjResourceCbo->Enabled(true);
      m_pObjResourceLbl->Enabled(true);
      m_pObjSectorLbl->Enabled(true);
      // default options with the sector cbo selection
      if(m_pObjSectorCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::MissionTargetCivilian) )
      {
         m_pObjResourceCbo->Clear();
         
         for(INT32 i=100359; i<=100385; i++)
         {
            m_pObjResourceCbo->Add_Item( g_ClientDAL.GetString(i) );
         }
         m_pObjResourceCbo->DropSize( 100 );
         m_pObjResourceCbo->Selected_Content(0);
      }
      else
      {
         m_pObjResourceCbo->Clear();
         m_pObjResourceCbo->Add_Item( g_ClientDAL.GetString(EStrId::UnitTypeGround) );
         m_pObjResourceCbo->Add_Item( g_ClientDAL.GetString(EStrId::UnitTypeAir) );
         m_pObjResourceCbo->Add_Item( g_ClientDAL.GetString(EStrId::UnitTypeNaval) );
         m_pObjResourceCbo->Add_Item( g_ClientDAL.GetString(EStrId::UnitTypeStrategic) );
         m_pObjResourceCbo->DropSize( 58 );
         m_pObjResourceCbo->Selected_Content( g_ClientDAL.GetString(EStrId::UnitTypeGround) );
      }
   }
   else
   {
      // no target sector when these specifics objectives are selected
      if(m_pObjTypeCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::MissionTypeAssassination) || 
         m_pObjTypeCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::MissionTypeTerrorism)     ||
         m_pObjTypeCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::MissionTypeCoupEtat))
      {
         m_pObjSectorCbo->Selected_Content(L"");
         m_pObjSectorCbo->Enabled(false);
         m_pObjSectorLbl->Enabled(false);
      }
      else
      {
         m_pObjSectorCbo->Selected_Content(g_ClientDAL.GetString(EStrId::MissionTargetCivilian));
         m_pObjSectorCbo->Enabled(true);
         m_pObjSectorLbl->Enabled(true);
      }

      m_pObjResourceCbo->Selected_Content(L"");
      m_pObjResourceCbo->Enabled(false);
      m_pObjResourceLbl->Enabled(false);
   }

   m_pObjComplexityCbo->Clear();

   if(m_pObjTypeCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::MissionTypeSabotage))
   {
      if(m_pObjSectorCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::MissionTargetMilitary))
      {
         m_pObjComplexityCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionComplexityMedium) );
         m_pObjComplexityCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionComplexityHigh) );
         m_pObjComplexityCbo->Selected_Content( g_ClientDAL.GetString(EStrId::MissionComplexityMedium) );
         m_pObjComplexityCbo->DropSize( 30 );
      }
      else
      {
         m_pObjComplexityCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionComplexityLow) );
         m_pObjComplexityCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionComplexityMedium) );
         m_pObjComplexityCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionComplexityHigh) );
         m_pObjComplexityCbo->Selected_Content( g_ClientDAL.GetString(EStrId::MissionComplexityMedium) );
         m_pObjComplexityCbo->DropSize( 44 );
      }
   }   
   else if(m_pObjTypeCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::MissionTypeCoupEtat))
   {
      m_pObjComplexityCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionComplexityHigh) );
      m_pObjComplexityCbo->Selected_Content( g_ClientDAL.GetString(EStrId::MissionComplexityHigh) );
      m_pObjComplexityCbo->DropSize( 16 );
   }
   else
   {
      m_pObjComplexityCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionComplexityLow) );
      m_pObjComplexityCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionComplexityMedium) );
      m_pObjComplexityCbo->Add_Item( g_ClientDAL.GetString(EStrId::MissionComplexityHigh) );
      m_pObjComplexityCbo->Selected_Content( g_ClientDAL.GetString(EStrId::MissionComplexityMedium) );
      m_pObjComplexityCbo->DropSize( 44 );
   }
}

