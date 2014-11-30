/**************************************************************
*
* sp2_covert_actions_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_covert_actions_window.h"
#include "../include/sp2_covert_actions_new_cell.h"
#include "../include/sp2_covert_actions_new_mission.h"
#include "../include/sp2_covert_actions_mission_outcome.h"
#include "../include/sp2_covert_actions_cell_training.h"

#include "../include/sp2_frame_cursor_meter.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

#include "../include/sp2_combo_listboxless.h"

const GString GCovertActionsWindow::TypeName(L"Covert_Actions_Window");

GUI::GBaseObject* GCovertActionsWindow::New()
{
	return (GUI::GBaseObject*) new GCovertActionsWindow();
}


GCovertActionsWindow::GCovertActionsWindow() : GBaseWindow()
{
   m_fWhen                = 0.f;
   m_bClearBeforeUpdate   = false;
   m_bIgnoreNextEvent     = false;
}

GCovertActionsWindow::~GCovertActionsWindow()
{
}

bool GCovertActionsWindow::OnCreate()
{
	__super::OnCreate();   

   // main window frame
   m_pObjInnerFrm           = (GUI::GFrame*)Child(L"frmInner");

   if(m_pObjApplyBtn)
      m_pObjApplyBtn->Hide();

   // cell frame controls
   GUI::GFrame* l_pFrmCell  = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmCell");
   m_pObjCellLst            = (GUI::GListBox*)l_pFrmCell->Child(L"lstCell");
   m_pObjCellLst->SelectionMode(EListBoxSelectionMode::MultiExtended);
   m_pObjNewCellBtn         = (GUI::GButton*)l_pFrmCell->Child(L"btnNewCell");
   m_pObjNewMissionBtn      = (GUI::GButton*)l_pFrmCell->Child(L"btnNewMission");
   m_pObjTrainCellBtn       = (GUI::GButton*)l_pFrmCell->Child(L"btnTrainCell");
   m_pObjDisbandCellBtn     = (GUI::GButton*)l_pFrmCell->Child(L"btnDisbandCell");
   m_pObjCancelMissionBtn   = (GUI::GButton*)l_pFrmCell->Child(L"btnCancelMission");

   // mission frame controls
   GUI::GFrame* l_pFrmMiss  = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmMission");
   m_pObjMissionTypeLbl     = (GUI::GLabel*)l_pFrmMiss->Child(L"txtMission");
   m_pObjSectorLbl          = (GUI::GLabel*)l_pFrmMiss->Child(L"txtSector");
   m_pObjTypeLbl            = (GUI::GLabel*)l_pFrmMiss->Child(L"txtResourceUnit");
   m_pObjComplexityLbl      = (GUI::GLabel*)l_pFrmMiss->Child(L"txtComplexity");
   m_pObjFramedCountryLbl   = (GUI::GLabel*)l_pFrmMiss->Child(L"txtFramedCountry");

   // upkeep frame controls
   GUI::GFrame* l_pFrmUp    = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmUpkeep");
   m_pObjTotalLbl           = (GUI::GLabel*)l_pFrmUp->Child(L"txtUpkeep");

   m_pObjProtectionLbl      = (GUI::GLabel*)l_pFrmUp->Child(L"txtProtection");

   m_pObjCancelMissionBtn->Enabled(false);
   m_pObjNewMissionBtn->Enabled(false);
   m_pObjTrainCellBtn->Enabled(false);
   m_pObjDisbandCellBtn->Enabled(false);

	return true;
}


void GCovertActionsWindow::ClearBeforeUpdate(bool in_bClearBeforeUpdate)
{
   m_bClearBeforeUpdate = in_bClearBeforeUpdate;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GCovertActionsWindow::OnShow()
{
   __super::OnShow();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();

   Update();
   
   g_ClientDAL.Attach(this, (DesignPattern::GSubjectNotification)ENotifyWindowMsg::CovertActions);
}

void GCovertActionsWindow::OnHide()
{
   g_ClientDAL.Detach(this, (DesignPattern::GSubjectNotification)ENotifyWindowMsg::CovertActions);

   __super::OnHide();
}

GUI::EEventStatus::Enum GCovertActionsWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);

   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjNewCellBtn)
         {
            if(!g_ClientDDL.CovertActionsNewCellWindow())
            {
               g_ClientDDL.CovertActionsNewCellWindowSpawn();
               RegisteredModalWindow.push_back( g_ClientDDL.CovertActionsNewCellWindow() );
            }

            g_ClientDDL.CovertActionsNewCellWindow()->Show();
         }
         else if(in_pCaller == m_pObjNewMissionBtn)
         {
            vector<GCovertActionCell> l_vCells = g_ClientDAL.m_PlayerCountryData.CovertActionCells();

            if(m_viLastSelectedRows.size() == 0)
               return GUI::EEventStatus::Handled;

            if(m_pObjNewMissionBtn->Text() == g_ClientDAL.GetString(EStrId::NewMission))
            {
               if(!g_ClientDDL.CovertActionsNewMissionWindow())
               {
                  g_ClientDDL.CovertActionsNewMissionWindowSpawn();
                  RegisteredModalWindow.push_back( g_ClientDDL.CovertActionsNewMissionWindow() );
               }

               g_ClientDDL.CovertActionsNewMissionWindow()->CellCleanUp();

               for(UINT32 i = 0; i < m_viLastSelectedRows.size(); i++)
               {
                  hash_map<UINT32, UINT32>::iterator l_It = m_ShownCell.find(m_viLastSelectedRows[i]);

                  if(l_It != m_ShownCell.end())
                     g_ClientDDL.CovertActionsNewMissionWindow()->AddCell(l_It->second);
               }
               g_SP2Client->ImmediatlyChangeMusicalTheme(AdaptiveMusic::ESP2MusicalTheme::Sneaky);
               g_ClientDDL.CovertActionsNewMissionWindow()->Show();
            }
            else
            {
               for(UINT32 i = 0; i < m_viLastSelectedRows.size(); i++)
               {
                  hash_map<UINT32, UINT32>::iterator l_It = m_ShownCell.find(m_viLastSelectedRows[i]);

                  if(l_It != m_ShownCell.end())
                  {
                     GCovertActionCell*      l_pCell = &g_ClientDAL.m_PlayerCountryData.CovertActionCell(l_It->second);
			            gassert(l_pCell,"Cell should exist");
                     SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventCellUpdateStatus);
                     SP2::Event::GEventCellUpdateStatus* l_pUpdate = (SP2::Event::GEventCellUpdateStatus*)l_Event.get();
                     l_pUpdate->m_iSource = g_SP2Client->Id();
                     l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;
                     l_pUpdate->m_bStartMission = true;
                     l_pUpdate->m_iCellID = l_It->second;
                     g_Joshua.RaiseEvent(l_Event);

                     m_pObjNewMissionBtn->Enabled(false);
                     m_JustDidMission[l_It->second] = true;
                     g_SP2Client->ImmediatlyChangeMusicalTheme(AdaptiveMusic::ESP2MusicalTheme::Sneaky);
                     m_fWhen = g_Joshua.GameTime();

			            if(l_pCell)
			            {
	                     //Locally modify the cell
               	      l_pCell->ChangeState(ECovertActionsCellState::Active);
               	      Update();
                     }
                  }
               }
            }
         }
         else if(in_pCaller == m_pObjTrainCellBtn)
         {
            if(m_viLastSelectedRows.size() == 0)
               return GUI::EEventStatus::Handled;

            if(!g_ClientDDL.CovertActionsCellTrainingWindow())
            {               
               g_ClientDDL.CovertActionsCellTrainingWindowSpawn();
               RegisteredModalWindow.push_back( g_ClientDDL.CovertActionsCellTrainingWindow() );
            }

            g_ClientDDL.CovertActionsCellTrainingWindow()->CleanUp();

            for(UINT32 i = 0; i < m_viLastSelectedRows.size(); i++)
            {
               vector<GCovertActionCell> l_vCells = g_ClientDAL.m_PlayerCountryData.CovertActionCells();
               hash_map<UINT32, UINT32>::iterator l_It = m_ShownCell.find(m_viLastSelectedRows[i]);

               if(l_It != m_ShownCell.end())
               {
                  GCovertActionCell* l_pCell = &g_ClientDAL.m_PlayerCountryData.CovertActionCell(l_It->second);

                  g_ClientDDL.CovertActionsCellTrainingWindow()->Update(l_pCell->ID(), l_pCell->Name(), l_pCell->ExperienceLevelType() );
               }
            }

            g_ClientDDL.CovertActionsCellTrainingWindow()->Show();
         }
         else if(in_pCaller == m_pObjDisbandCellBtn)
         {
            if(m_viLastSelectedRows.size() == 0)
               return GUI::EEventStatus::Handled;
            else
            {
               vector<INT32>::iterator l_It = m_viLastSelectedRows.begin();
               while(l_It != m_viLastSelectedRows.end())
               {
                  // Reduce de indexes of all that are above current
                  for(UINT32 i = 0; i < m_viLastSelectedRows.size(); i++)
                  {
                     if(m_viLastSelectedRows[i] > (*l_It))
                        m_viLastSelectedRows[i]--;
                  }

                  vector<GCovertActionCell> l_vCells = g_ClientDAL.m_PlayerCountryData.CovertActionCells();
                  hash_map<UINT32, UINT32>::iterator l_ItCell = m_ShownCell.find(*l_It);

                  if(l_ItCell != m_ShownCell.end())
                  {
                     SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventCellUpdateStatus);
                     SP2::Event::GEventCellUpdateStatus* l_pUpdate = (SP2::Event::GEventCellUpdateStatus*)l_Event.get();
                     l_pUpdate->m_iSource        = g_SP2Client->Id();
                     l_pUpdate->m_iTarget        = SDK::Event::ESpecialTargets::Server;
                     l_pUpdate->m_bEraseCell     = true;
                     l_pUpdate->m_iCellID        = l_ItCell->second;
                     g_Joshua.RaiseEvent(l_Event);

                     m_fWhen = g_Joshua.GameTime();

                     m_ShownCell.erase(l_ItCell);
                     RebuildShownCells(*l_It);
    
                     m_pObjCellLst->RemoveRow(*l_It);
                     l_It = m_viLastSelectedRows.erase(l_It);
                  }
                  else
                  {
                     l_It++;
                  }
               }
               m_pObjCellLst->Update();
               m_viLastSelectedRows.clear();

               m_pObjCancelMissionBtn->Enabled(false);
               m_pObjNewMissionBtn->Enabled(false);
               m_pObjTrainCellBtn->Enabled(false);
               m_pObjDisbandCellBtn->Enabled(false);
            }
         }
         else if(in_pCaller == m_pObjCancelMissionBtn)
         {
            if(m_viLastSelectedRows.size() == 0)
               return GUI::EEventStatus::Handled;
            else
            {
               for(UINT32 i = 0; i < m_viLastSelectedRows.size(); i++)
               {
                  hash_map<UINT32, UINT32>::iterator l_It = m_ShownCell.find(m_viLastSelectedRows[i]);

                  if(l_It != m_ShownCell.end())
                  {
                     GCovertActionCell*      l_pCell = &g_ClientDAL.m_PlayerCountryData.CovertActionCell(l_It->second);
                     gassert(l_pCell,"Cell should exist");

                     SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventCellUpdateStatus);
                     SP2::Event::GEventCellUpdateStatus* l_pUpdate = (SP2::Event::GEventCellUpdateStatus*)l_Event.get();
                     l_pUpdate->m_iSource = g_SP2Client->Id();
                     l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;
                     l_pUpdate->m_bCancelMission = true;
                     l_pUpdate->m_iCellID = l_It->second;
                     g_Joshua.RaiseEvent(l_Event);
                     m_fWhen = g_Joshua.GameTime();

			            if(l_pCell)
			            {
               	      //Locally modify the cell
               	      l_pCell->CancelAction();
               	      Update();
                     }
                  }
               }
            }
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

EEventStatus::Enum GCovertActionsWindow::OnCustomEvent(UINT32 in_iType, const SDK::GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
   if(in_iType == ECustomEvents::OnComboBoxSelectionEvent)
   {
      GUI::GComboBox* l_pCombo = (GUI::GComboBox*)in_EventData.Data;
      GUI::GListBox::GRow*          l_pRow  = NULL;
      bool l_bSelectNew = true;
      
      //Fetch the ID of the selected cell
      GString l_sCellID = l_pCombo->Property("cellid");
      INT32  l_iCellID  = -1;
      if(l_sCellID != L"")
      {
         l_iCellID = l_sCellID.ToINT32();
      }
      else
      {
         //Not a handled case
         return EEventStatus::NotHandled;
      }

      // Check if the cell associated with cbo box is already selected
      for(UINT32 i = 0; i < m_viLastSelectedRows.size(); i++)
      {
         hash_map<UINT32, UINT32>::iterator l_CellIt = m_ShownCell.find(m_viLastSelectedRows[i]);
         if(l_CellIt->second == (UINT32)l_iCellID)
         {
            l_bSelectNew = false;
            l_pRow = m_pObjCellLst->Get_Row_At(l_CellIt->first);
            break;
         }
      }

      if(l_bSelectNew)
      {
         for(UINT32 i = 0; i < m_ShownCell.size(); i++)
         {
            hash_map<UINT32, UINT32>::iterator l_CellIt = m_ShownCell.find(i);
            if(l_CellIt->second == (UINT32)l_iCellID)
            {
               m_pObjCellLst->ClearSelection();

               DeselectListRows(m_viLastSelectedRows);
               m_viLastSelectedRows.clear();
               m_viLastSelectedRows.push_back(l_CellIt->first);
               SelectListRows(m_viLastSelectedRows);

               m_pObjCellLst->Select_Items(m_viLastSelectedRows, true);
               l_pRow = m_pObjCellLst->Get_Row_At(m_viLastSelectedRows[0]);
               break;
            }
         }
      }

      for(UINT32 i = 0; i < m_viLastSelectedRows.size(); i++)
      {
         //Find the covert action cell
         GCovertActionCell*            l_pCell = NULL;

         //find the cell and the row
         hash_map<UINT32, UINT32>::iterator l_CellIt = m_ShownCell.find(m_viLastSelectedRows[i]);
         if(l_CellIt != m_ShownCell.end())
            l_pCell = &g_ClientDAL.m_PlayerCountryData.CovertActionCell(l_CellIt->second);
         gassert(l_pCell && l_pRow,"There should be a row or a cell present");

	      if(!l_pCell || !l_pRow)
	      {
	  	      return EEventStatus::Handled;
	      }

         GComboListBoxLess* l_pCtryCbo     = (GComboListBoxLess*)l_pRow->Object()->Child(L"cboCountry");
         GUI::GComboBox*    l_pStatusCbo   = (GUI::GComboBox*)l_pRow->Object()->Child(L"cboStatus");

         if(l_pCombo == l_pCtryCbo)
         {         
            GUI::GComboBox*    l_pStatusCbo = (GUI::GComboBox*)l_pRow->Object()->Child(L"cboStatus");

            UINT32 l_iSelID = (UINT32)g_ClientDAL.Country(l_pCtryCbo->Selected_Content()).Id();

            if(l_iSelID != l_pCell->AssignedCountry())
            {
               SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventCellUpdateStatus);
               SP2::Event::GEventCellUpdateStatus* l_pUpdate = (SP2::Event::GEventCellUpdateStatus*)l_Event.get();
               l_pUpdate->m_iSource      = g_SP2Client->Id();
               l_pUpdate->m_iTarget      = SDK::Event::ESpecialTargets::Server;
               l_pUpdate->m_iToCountryID = l_iSelID;
               l_pUpdate->m_iCellID      = l_CellIt->second;            
               g_Joshua.RaiseEvent(l_Event);
               m_fWhen = g_Joshua.GameTime();
               l_pStatusCbo->Enabled(false);
               l_pCtryCbo  ->Enabled(false);  

               //Set the assigned country of the cell localy
               l_pCell->NextAssignedCountry(l_iSelID); 
               l_pCell->ChangeState(ECovertActionsCellState::InTransit);
//               Update();

            }
         }
         else if(l_pCombo == l_pStatusCbo)
         {
            ECovertActionsCellState::Enum l_eState = g_ClientDAL.ConvertCellStateToEnum( l_pStatusCbo->Selected_Content() );

            if(l_eState != l_pCell->ActualState())
            {
               SDK::GGameEventSPtr l_Event                   = CREATE_GAME_EVENT(SP2::Event::GEventCellUpdateStatus);
               SP2::Event::GEventCellUpdateStatus* l_pUpdate = (SP2::Event::GEventCellUpdateStatus*)l_Event.get();
               l_pUpdate->m_iSource                          = g_SP2Client->Id();
               l_pUpdate->m_iTarget                          = SDK::Event::ESpecialTargets::Server;            
               if(l_eState == ECovertActionsCellState::Active)
                  l_pUpdate->m_bToActive = true;
               else if(l_eState == ECovertActionsCellState::Dormant)
                  l_pUpdate->m_bToDormant = true;
               else
                  gassert(0, "Should never happen");            
               l_pUpdate->m_iCellID                         = l_CellIt->second;
               g_Joshua.RaiseEvent(l_Event);
               m_fWhen = g_Joshua.GameTime();
               l_pStatusCbo->Enabled(false);
               l_pCtryCbo  ->Enabled(false);

               //Locally change the cell state
               if(l_pUpdate->m_bToActive)
                  l_pCell->ChangeState(ECovertActionsCellState::GettingReady); 
               else if(l_pUpdate->m_bToDormant)
                  l_pCell->ChangeState(ECovertActionsCellState::Dormant);
            }
         }
      }
      Update();
   }
   else if(in_iType == ECustomEvents::OnListItemSelected)
   {
      vector<INT32> l_viSel = m_pObjCellLst->Get_Selected_Item();

      DeselectListRows(m_viLastSelectedRows);
      m_viLastSelectedRows = l_viSel;
      SelectListRows(m_viLastSelectedRows);
   }
   else if(in_iType == ECustomEvents::OnComboBoxBeforeOpen)
   {
      if(((GUI::GComboBox*)in_EventData.Data)->Name() == L"cboCountry")
         g_ClientDDL.ComboboxSetCountriesAndFlag(g_ClientDAL.Countries(), (GUI::GComboBox*)in_EventData.Data, true);

      return GUI::EEventStatus::Handled;
   }

   return GUI::EEventStatus::Handled;
}

void GCovertActionsWindow::RebuildShownCells(UINT32 in_iID)
{
   vector<GCovertActionCell> l_vCells = g_ClientDAL.m_PlayerCountryData.CovertActionCells();
   hash_map<UINT32, UINT32> l_ShownCellCopy = m_ShownCell;

   m_ShownCell.clear();

   //For each cells
   for(hash_map<UINT32, UINT32>::iterator l_It = l_ShownCellCopy.begin(); l_It != l_ShownCellCopy.end(); l_It++)
   {
      if(l_It->first > in_iID)
         m_ShownCell.insert( make_pair(l_It->first - 1, l_It->second ) );
      else
         m_ShownCell.insert( make_pair(l_It->first, l_It->second ) );
   }
}

void GCovertActionsWindow::DeselectListRows(const vector<INT32> &in_Rows)
{
   for(UINT32 i = 0; i < in_Rows.size(); i++)
   {
      GUI::GListBox::GRow* l_pRow = m_pObjCellLst->Get_Row_At(in_Rows[i]);

      if(l_pRow)
      {
         GUI::GLabel* l_pNameLbl = (GUI::GLabel*)l_pRow->Object()->Child(L"txtName");
         l_pNameLbl->PictureNormal(GString(L"000587"));
      }
   }
}

void GCovertActionsWindow::SelectListRows(const vector<INT32> &in_Rows)
{
   if(in_Rows.size())
   {
      vector<GCovertActionCell> l_vCells = g_ClientDAL.m_PlayerCountryData.CovertActionCells();
      GString l_sFramedCountry = L"";
      GString l_sSector = L"";
      GString l_sObjType= L"";
      GString l_sNewMission = L"";
      GString l_sMissionType = L"";
      GString l_sComplexity = L"";
      bool l_bTrainBtnEnabled = true;
      bool l_bNewMissionBtnEnabled = false;
      bool l_bCancelMissionBtnEnabled = true;
      bool l_bDisableMissionBtn = false;

      for(UINT32 i = 0; i < in_Rows.size(); i++)
      {
         GUI::GListBox::GRow* l_pRow = m_pObjCellLst->Get_Row_At(in_Rows[i]);
         if(l_pRow)
         {
            GUI::GLabel* l_pNameLbl = (GUI::GLabel*)l_pRow->Object()->Child(L"txtName");
            l_pNameLbl->PictureNormal(GString(L"000647"));
         }

         hash_map<UINT32, UINT32>::iterator l_It = m_ShownCell.find(m_viLastSelectedRows[i]);

         if(l_It != m_ShownCell.end())
         {
            GCovertActionCell* l_pCell = &g_ClientDAL.m_PlayerCountryData.CovertActionCell(l_It->second);
            if(l_pCell->ActualState() == ECovertActionsCellState::Active || l_pCell->ActualState() == ECovertActionsCellState::Dormant)
            {
               if(l_pCell->ExperienceLevel() == ECovertActionCellTraining::Elite)
                  l_bTrainBtnEnabled = false;
               if(!m_JustDidMission[l_pCell->ID()])
                  l_bNewMissionBtnEnabled = true;

               l_bCancelMissionBtnEnabled = false;
            }

            if(l_pCell->ActualState() == ECovertActionsCellState::PreparingMission || l_pCell->ActualState() == ECovertActionsCellState::ReadyToExecute)
            {
               l_sMissionType = g_ClientDAL.ConvertMissionTypeToString( l_pCell->MissionType() );
               l_sComplexity = g_ClientDAL.ConvertMissionComplexityToString( l_pCell->MissionComplexity() );
               
               if(l_pCell->CountryToFrame() > 0)
                  l_sFramedCountry = g_ClientDAL.Country( l_pCell->CountryToFrame() ).Name();
               else
                  l_sFramedCountry = g_ClientDAL.GetString(EStrId::None);

               if(l_pCell->MissionType() == SP2::ECovertActionsMissionType::Espionage || l_pCell->MissionType() == SP2::ECovertActionsMissionType::Sabotage)
               {
                  l_sSector = g_ClientDAL.ConvertTargetSectorToString( l_pCell->TargetSector() );

                  if(l_pCell->TargetSector() == SP2::ECovertActionsTargetSector::Civilian)
                  {
                     l_sObjType = g_ClientDAL.ConvertResourcesToString( l_pCell->ResourceType() );
                  }
                  else if(l_pCell->TargetSector() == SP2::ECovertActionsTargetSector::Military)
                  {
                     l_sObjType = g_ClientDAL.ConvertUnitCategoryToString( l_pCell->UnitCategory() );
                  }
               }
            
               if(l_sNewMission.length() != 0)
                  l_bDisableMissionBtn = true;

               l_sNewMission = g_ClientDAL.GetString(EStrId::ExecuteMission);

               if(l_pCell->ActualState() == ECovertActionsCellState::ReadyToExecute)
               {
                  l_bTrainBtnEnabled = false;
                  if(!m_JustDidMission[l_pCell->ID()])
                     l_bNewMissionBtnEnabled = true;
               }
               else
               {
                  l_bNewMissionBtnEnabled = false;
               }
            }
            else if(l_pCell->ActualState() == ECovertActionsCellState::InTransit ||
                     l_pCell->ActualState() == ECovertActionsCellState::Training ||
                     l_pCell->ActualState() == ECovertActionsCellState::GettingReady)
            {
               l_bTrainBtnEnabled = false;
            }
            else
            {
               if(l_sNewMission != g_ClientDAL.GetString(EStrId::NewMission) &&
                  l_sNewMission.length() > 0)
                  l_bDisableMissionBtn = true;
               l_sNewMission = g_ClientDAL.GetString(EStrId::NewMission);
               m_JustDidMission[in_Rows[i]] = false;
            }
         }
      }

      if(l_bDisableMissionBtn)
         l_bNewMissionBtnEnabled = false;
     
      m_pObjTrainCellBtn->Enabled(l_bTrainBtnEnabled);
      m_pObjNewMissionBtn->Enabled(l_bNewMissionBtnEnabled);
      m_pObjCancelMissionBtn->Enabled(l_bCancelMissionBtnEnabled);
      m_pObjDisbandCellBtn->Enabled(true);
      m_pObjMissionTypeLbl->Text(l_sMissionType);
      m_pObjSectorLbl->Text(l_sSector);
      m_pObjComplexityLbl->Text(l_sComplexity);
      m_pObjFramedCountryLbl->Text(l_sFramedCountry);
      m_pObjTypeLbl->Text(l_sObjType);
      m_pObjNewMissionBtn->Text( l_sNewMission );
   }
   else
   {
      m_pObjTrainCellBtn->Enabled(false);
      m_pObjNewMissionBtn->Enabled(false);
      m_pObjCancelMissionBtn->Enabled(false);
      m_pObjDisbandCellBtn->Enabled(false);
   }
}

void GCovertActionsWindow::SelectCellRow(UINT32 in_ID)
{
   m_viLastSelectedRows.push_back(in_ID);

   GUI::GListBox::GRow* l_pRow = m_pObjCellLst->Get_Row_At(in_ID);

   if(l_pRow)
   {
      GUI::GLabel* l_pNameLbl = (GUI::GLabel*)l_pRow->Object()->Child(L"txtName");
      l_pNameLbl->PictureNormal(GString(L"000647"));
   }
}

bool GCovertActionsWindow::OnDestroy()
{
   if(g_ClientDDL.CovertActionsNewCellWindow())
      g_ClientDDL.CovertActionsNewCellWindowKill();
   
   if(g_ClientDDL.CovertActionsNewMissionWindow())
      g_ClientDDL.CovertActionsNewMissionWindowKill();
   
   if(g_ClientDDL.CovertActionsCellTrainingWindow())
      g_ClientDDL.CovertActionsCellTrainingWindowKill();

   RegisteredModalWindow.clear();

   return __super::OnDestroy();
}

void GCovertActionsWindow::OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   if((SP2::ENotifyWindowMsg::Enum)in_Notification == SP2::ENotifyWindowMsg::CovertActions)
   {
      if((UINT32)in_pAdditionalMsg == 0xFFFFFFFF)
         UpdateNationalSecurity();
      else if(((m_fWhen + g_Joshua.Latency()) <= (g_ClientDAL.m_PlayerCountryData.m_fWhenSent + (g_Joshua.Latency() / 2))))
         Update();
   }
}

void GCovertActionsWindow::UpdateNationalSecurity()
{
   m_pObjProtectionLbl->Text( GString::FormatNumber(g_ClientDAL.m_PlayerCountryData.NationalSecurity() * 100.f, L"", L".", L"", L"", 0, 1) );
}

bool GCovertActionsWindow::Update()
{
   //If we must clear the list before the update (cell disbanded or captured)
   if(m_bClearBeforeUpdate)
   {
      // clear global objects lists and maps
      m_pObjCellLst->Clear();
      m_ShownCell.clear();
      m_viLastSelectedRows.clear();
      m_bClearBeforeUpdate   = false;
   }

   vector<UINT32> l_viCurrentCells;

   //Get the cells
   const vector<GCovertActionCell> & l_vCells = g_ClientDAL.m_PlayerCountryData.CovertActionCells();
   REAL64 l_fTotalUpkeep = 0.f;

   UpdateNationalSecurity();

   // retrieve a reference to the terrorists cells of our country
   GUI::GListBox *l_pOpenedCountryListBox     = NULL;
   bool   m_bMustInit                         = false;
   GUI::GListBox::GRow* l_pRow                = NULL;
  
   //For each cells
   for(UINT32 i=0; i<l_vCells.size(); i++)
   {
      //Check if we must insert rows before filling the cell
      if(m_pObjCellLst->Get_Nb_Rows() > 0)
      {
		 UINT32 j;
         for(j = 0; j < m_ShownCell.size(); j++)
         {
            hash_map<UINT32, UINT32>::iterator l_It = m_ShownCell.find(j);
            if(l_It->second == l_vCells[i].ID())
            {
               l_pRow = m_pObjCellLst->Get_Row_At(l_It->first);
               
               m_pObjTrainCellBtn->Enabled(false);
               m_pObjNewMissionBtn->Enabled(false);
               break;
            }
         }

         if(j == m_ShownCell.size())
         {
            l_pRow = m_pObjCellLst->Insert_New_Row(l_vCells[i].ID());
            m_ShownCell.insert( make_pair(l_pRow->m_iId, l_vCells[i].ID() ) );

            l_viCurrentCells.push_back(l_vCells[i].ID());
            
            hash_map<UINT32, bool>::iterator l_It = m_JustDidMission.find( l_vCells[i].ID() );
            if(l_It == m_JustDidMission.end())
               m_JustDidMission[l_vCells[i].ID()] = false;
               
            m_bMustInit = true;
         }
      }
      else
      {
         l_pRow = m_pObjCellLst->Insert_New_Row(l_vCells[i].ID());
         m_ShownCell.insert( make_pair(l_pRow->m_iId, l_vCells[i].ID() ) );
         
         l_viCurrentCells.push_back(l_vCells[i].ID());

         hash_map<UINT32, bool>::iterator l_It = m_JustDidMission.find( l_vCells[i].ID() );
         if(l_It == m_JustDidMission.end())
            m_JustDidMission[l_vCells[i].ID()] = false;

         m_bMustInit = true;
      }
      SelectListRows(m_viLastSelectedRows);

      // retrieve the objects ptr in our row
      GUI::GLabel*       l_pNameLbl     = (GUI::GLabel*)l_pRow->Object()->Child(L"txtName");
      GComboListBoxLess* l_pCtryCbo     = (GComboListBoxLess*)l_pRow->Object()->Child(L"cboCountry");
      GUI::GComboBox*    l_pStatusCbo   = (GUI::GComboBox*)l_pRow->Object()->Child(L"cboStatus");
      SP2::GGradientBar* l_pProgressMtr = (SP2::GGradientBar*)l_pRow->Object()->Child(L"frmProgress");
      GUI::GLabel*       l_pUpkeepLbl   = (GUI::GLabel*)l_pRow->Object()->Child(L"txtUpkeep");
      GUI::GPictureBox*  l_pGradePic    = (GUI::GPictureBox*)l_pRow->Object()->Child(L"picGrade");

      // assign the countries listbox to the combolistboxless
      l_pCtryCbo->SetListBox(g_ClientDDL.CountriesListBox()); 

      // setup cellid props 
      l_pCtryCbo->Property("cellid", GString(l_vCells[i].ID()));
      l_pStatusCbo->Property("cellid", GString(l_vCells[i].ID()));

      // did we add a row ?
      if(m_bMustInit)
      {
         m_bMustInit = false;
         // yes ? init progress bar and cbos objects
         l_pProgressMtr->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));
         l_pProgressMtr->RightBound(73);
         l_pProgressMtr->Value(l_vCells[i].Progression(), 0, false, false);
         l_pStatusCbo->Add_Item(g_ClientDAL.GetString(EStrId::CellActive));
         l_pStatusCbo->Add_Item(g_ClientDAL.GetString(EStrId::CellDormant));      
      }
  
      // retrieve terrorist cell status
      GString l_sStatus = g_ClientDAL.ConvertCellStateToString((ECovertActionsCellState::Enum)l_vCells[i].ActualState());
      l_pStatusCbo->Selected_Content(l_sStatus);
      // set up labels and cbos displayed text
      l_pNameLbl->Text(l_vCells[i].Name());
      l_pCtryCbo->Selected_Content(g_ClientDAL.Country(l_vCells[i].AssignedCountry()).Name());
      l_pUpkeepLbl->Text(GString::FormatNumber((REAL64)l_vCells[i].UpkeepFee(), L" ", L".", L"$ ", L"", 3, 0, false));
      l_pGradePic->BringToFront();
      // total of the upkeep for every cells
      l_fTotalUpkeep += (REAL64)l_vCells[i].UpkeepFee();

      // country cbo was opened ?
      if(l_pCtryCbo->GetListBox() && l_pCtryCbo->GetListBox()->Visible())
         l_pOpenedCountryListBox = l_pCtryCbo->GetListBox();

      switch(l_vCells[i].ExperienceLevelType())
      {
      case ECovertActionCellTraining::Recruit:
         l_pGradePic->SendToBack();
         break;
      case ECovertActionCellTraining::Regular:
         l_pGradePic->PictureNormal(GString(L"000825"));
         break;
      case ECovertActionCellTraining::Veteran:
         l_pGradePic->PictureNormal(GString(L"000826"));
         break;
      case ECovertActionCellTraining::Elite:
         l_pGradePic->PictureNormal(GString(L"000827"));
         break;
      }
      
      // enable buttons and labels switch cells status
      if( (l_sStatus == g_ClientDAL.GetString(EStrId::CellActive)) || (l_sStatus == g_ClientDAL.GetString(EStrId::CellDormant)) )
      {
         l_pCtryCbo->Enabled(true);         
         l_pStatusCbo->Enabled(true);
      }
      else 
      {
         if(l_sStatus == g_ClientDAL.GetString(EStrId::CellInCreation))
         {
            l_pUpkeepLbl->Text( GString::FormatNumber( 0.f, L" ", L".", L"$ ", L"", 3, 0, false ) );
            l_fTotalUpkeep -= (REAL64)l_vCells[i].UpkeepFee();
         }
         else if(l_sStatus == g_ClientDAL.GetString(EStrId::CellInTransit))
         {
            l_pCtryCbo->Selected_Content( g_ClientDAL.Country(l_vCells[i].NextAssignedCountry()).Name() );
         }
         
         l_pStatusCbo->Enabled(false);
         l_pCtryCbo->Enabled(false);
      }
   }  

   hash_map<UINT32, bool>::iterator l_It;
   vector<UINT32>::iterator l_It2;

   //Clear elements that are no longer used
   for(UINT32 i=0; i<m_JustDidMission.size(); i++)
   {
      l_It = m_JustDidMission.begin();
      
      for(UINT32 j=0; j<i; j++)
         l_It++;

      l_It2 = find( l_viCurrentCells.begin(), l_viCurrentCells.end(), l_It->first );
      if( l_It2 == l_viCurrentCells.end() )
      {
         m_JustDidMission.erase( l_It );
         --i;
      }
   }

   // set up total upkeep label text
   m_pObjTotalLbl->Text(GString::FormatNumber((REAL64)l_fTotalUpkeep, L" ", L".", L"$ ", L"", 3, 0, false));

   // display the results
   m_pObjCellLst->Update();
   if(l_pOpenedCountryListBox)
      l_pOpenedCountryListBox->Show();

   return true;
}

void GCovertActionsWindow::Iterate()
{
   for(hash_map<UINT32, UINT32>::iterator l_It = m_ShownCell.begin(); l_It != m_ShownCell.end(); l_It++)
   {
      const vector<GCovertActionCell> & l_vCells = g_ClientDAL.m_PlayerCountryData.CovertActionCells();
      for(UINT32 i=0; i<l_vCells.size(); i++)
      {
         if(l_vCells[i].ID() == l_It->second)
         {
            GUI::GListBox::GRow* l_pRow = m_pObjCellLst->Get_Row_At(l_It->first);
            if(l_pRow)
            {
               SP2::GGradientBar* l_pProgressMtr = (SP2::GGradientBar*)l_pRow->Object()->Child(L"frmProgress");

               REAL32 l_fProgression = l_vCells[i].Progression();

               l_pProgressMtr->Value(l_fProgression, 0, false, false);
            }
         }
      }
   }
}
