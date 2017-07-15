/**************************************************************
*
* sp2_military_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_military_window.h"

#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"
#include "../include/sp2_design_window.h"
#include "../include/sp2_unit_list_window.h"
#include "../include/sp2_unit_list_window_new.h"
#include "../include/sp2_unit_production_window.h"
#include "../include/sp2_unit_research_window.h"
#include "../include/sp2_used_unit_window.h"
#include "../include/sp2_covert_actions_window.h"
#include "../include/sp2_war_list_window.h"
#include "../include/sp2_unit_ground_deployment_window.h"

const GString GMilitaryWindow::TypeName(L"Military_Window");

GUI::GBaseObject* GMilitaryWindow::New()
{
	return (GUI::GBaseObject*) new GMilitaryWindow();
}


GMilitaryWindow::GMilitaryWindow() : GBaseWindow()
{
   m_pData       = &(g_ClientDAL.DataMilitaryWindow);

   m_pObjInnerFrm = NULL;
   m_pObjConventionalFrame = NULL;
   m_pObjStrategicFrame = NULL;
   m_pObjStrategicWarfareBtn = NULL;
   m_pObjDesignBtn = NULL;
   m_pObjBuildBtn = NULL;
   m_pObjBuyBtn = NULL;
   m_pObjResearchBtn = NULL;
   m_pObjStartNuclearResearchBtn = NULL;
   m_pObjAmdsLbl = NULL;
   m_pObjAmdsResearchBtn = NULL;
   m_pObjNuclearBuildBtn = NULL;
   m_pObjCovertActionsBtn = NULL;
   m_pObjNuclearProgressMeter = NULL;
	m_pObjAMDSProgressMeter = NULL;
   m_pObjWarListBtn = NULL;
   m_pObjDeployBtn = NULL;
}

GMilitaryWindow::~GMilitaryWindow()
{
}

bool GMilitaryWindow::OnCreate()
{
	__super::OnCreate();
   
   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   if(m_pData->Left < 0)
      Position(3,(INT16)mode.m_Resolution.y - 99 - Height());

   m_pObjInnerFrm                  = (GUI::GFrame*)Child(L"frmInner");
   m_pObjConventionalFrame         = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmConventionalWarfare");
   m_pObjConventionalForcesMoreBtn = (GUI::GButton*)m_pObjConventionalFrame->Child(L"btnConventionalForcesMore");
   m_pObjStrategicWarfareBtn       = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnStrategicWarfare");
   m_pObjDesignBtn                 = (GUI::GButton*)m_pObjConventionalFrame->Child(L"btnDesign");
   m_pObjBuildBtn                  = (GUI::GButton*)m_pObjConventionalFrame->Child(L"btnBuild");
   m_pObjBuyBtn                    = (GUI::GButton*)m_pObjConventionalFrame->Child(L"btnBuy");
   m_pObjResearchBtn               = (GUI::GButton*)m_pObjConventionalFrame->Child(L"btnResearch");
   m_pObjDeployBtn                 = (GUI::GButton*)m_pObjConventionalFrame->Child(L"btnDeploy");
   m_pObjStrategicFrame            = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmStrategicWeapons");
   m_pObjNuclearProgressMeter      = (SP2::GProgressMeterYellowValue*)m_pObjStrategicFrame->Child(L"frmNuclearProgress");
	m_pObjAMDSProgressMeter			  = (SP2::GProgressMeterYellowValue*)m_pObjStrategicFrame->Child(L"frmAmdsProgress");
   m_pObjStartNuclearResearchBtn   = (GUI::GButton*)m_pObjStrategicFrame->Child(L"btnNuclearProgram");
   m_pObjNuclearBuildBtn           = (GUI::GButton*)m_pObjStrategicFrame->Child(L"btnBuild");
   m_pObjAmdsLbl                   = (GUI::GLabel*)m_pObjStrategicFrame->Child(L"txtMissileDefenseStatus");
   m_pObjAmdsResearchBtn           = (GUI::GButton*)m_pObjStrategicFrame->Child(L"btnResearch");
   m_pObjCovertActionsBtn          = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnCovertActions");
   m_pObjWarListBtn                = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnWarList");

   m_pObjNuclearProgressMeter->Value(0,0);
	m_pObjAMDSProgressMeter->Value(0,0);
   
   m_pObjApplyBtn->Hide();
   m_pObjLockTog->Hide();

   m_pObjStrategicWarfareBtn->BringToFront();
   m_pObjConventionalForcesMoreBtn->BringToFront();

   if(Visible())
      Update();

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GMilitaryWindow::OnShow()
{
   __super::OnShow();

   g_ClientDAL.Attach(this, ENotifyWindowMsg::Military);
   
   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();

   if(m_pObjInnerFrm)
      Update();
}

void GMilitaryWindow::OnHide()
{
   g_ClientDAL.Detach(this, ENotifyWindowMsg::Military);

   __super::OnHide();
}

GUI::EEventStatus::Enum GMilitaryWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {
            g_ClientDDL.MilitaryWindow()->Hide();
            g_ClientDDL.KillWindowsMilitaryLv2();
            g_ClientDDL.MainBar()->m_pObjMilitary->ToggleState(false);
         }
         else if(in_pCaller == m_pObjStrategicWarfareBtn)
         {
            g_ClientDDL.SwapToStrategicWarfare();
         }
         else if(in_pCaller == m_pObjConventionalForcesMoreBtn)
         {
            if(!g_ClientDDL.UnitListWindowNew()) 
               g_ClientDDL.UnitListWindowNewSpawn();
                  
            g_ClientDDL.UnitListWindowNew()->UpdateUnitList(g_ClientDAL.ControlledCountryID(), -1);

            if(!g_ClientDDL.UnitListWindowNew()->Visible())
               g_ClientDDL.UnitListWindowNew()->Show();
            else
               g_ClientDDL.UnitListWindowNew()->BringToFront();
         }
         else if(in_pCaller == m_pObjDesignBtn)
         {
            if(!g_ClientDDL.DesignWindow())
               g_ClientDDL.DesignWindowSpawn();

            if(!g_ClientDDL.DesignWindow()->Visible())
               g_ClientDDL.DesignWindow()->Show();
            else
               g_ClientDDL.DesignWindow()->BringToFront();
         }
         else if(in_pCaller == m_pObjBuildBtn)
         {
            if(!g_ClientDDL.UnitProductionWindow())
               g_ClientDDL.UnitProductionWindowSpawn();

            if(!g_ClientDDL.UnitProductionWindow()->Visible())
            {
               g_ClientDDL.UnitProductionWindow()->SelectedCategory(SP2::EUnitCategory::Ground);
               g_ClientDDL.UnitProductionWindow()->Show();
            }
            else
               g_ClientDDL.UnitProductionWindow()->BringToFront();
         }
         else if(in_pCaller == m_pObjNuclearBuildBtn)
         {
            if(!g_ClientDDL.UnitProductionWindow())
               g_ClientDDL.UnitProductionWindowSpawn();

            if(!g_ClientDDL.UnitProductionWindow()->Visible())
            {
               g_ClientDDL.UnitProductionWindow()->SelectedCategory(SP2::EUnitCategory::Nuclear);
               g_ClientDDL.UnitProductionWindow()->Show();
            }
            else
               g_ClientDDL.UnitProductionWindow()->BringToFront();
         }
         else if(in_pCaller == m_pObjBuyBtn)
         {
            if(!g_ClientDDL.UsedUnitWindow())
               g_ClientDDL.UsedUnitWindowSpawn();

            if(!g_ClientDDL.UsedUnitWindow()->Visible())
               g_ClientDDL.UsedUnitWindow()->Show();
            else
               g_ClientDDL.UsedUnitWindow()->BringToFront();

         }
         else if(in_pCaller == m_pObjResearchBtn)
         {
            if(!g_ClientDDL.UnitResearchWindow())
               g_ClientDDL.UnitResearchWindowSpawn();

            if(!g_ClientDDL.UnitResearchWindow()->Visible())
               g_ClientDDL.UnitResearchWindow()->Show();
            else
               g_ClientDDL.UnitResearchWindow()->BringToFront();
         }
         else if(in_pCaller == m_pObjAmdsResearchBtn)
         {
            GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GMilitaryMainUpdate);
            SP2::Event::GMilitaryMainUpdate* l_pUpdateEvent = (SP2::Event::GMilitaryMainUpdate*)l_pEvent.get();

            l_pUpdateEvent->m_iSource = g_SP2Client->Id();
            l_pUpdateEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
            l_pUpdateEvent->CountryId = g_ClientDAL.ControlledCountryID();
            l_pUpdateEvent->StartAmdsResearch = true;
            
            g_ClientDCL.ClientAdvisor().RaiseEvent(l_pEvent);
         }
         else if(in_pCaller == m_pObjStartNuclearResearchBtn)
         {
            GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GMilitaryMainUpdate);
            SP2::Event::GMilitaryMainUpdate* l_pUpdateEvent = (SP2::Event::GMilitaryMainUpdate*)l_pEvent.get();

            l_pUpdateEvent->m_iSource = g_SP2Client->Id();
            l_pUpdateEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
            l_pUpdateEvent->CountryId = g_ClientDAL.ControlledCountryID();
            l_pUpdateEvent->StartNuclearResearch = true;
            
            g_ClientDCL.ClientAdvisor().RaiseEvent(l_pEvent);
         }
         else if(in_pCaller == m_pObjCovertActionsBtn)
         {
            if(!g_ClientDDL.CovertActionsWindow())
               g_ClientDDL.CovertActionsWindowSpawn();
            
            if(!g_ClientDDL.CovertActionsWindow()->Visible())
               g_ClientDDL.CovertActionsWindow()->Show();
            else
               g_ClientDDL.CovertActionsWindow()->BringToFront();
        }
         else if(in_pCaller == m_pObjWarListBtn)
         {
            if(!g_ClientDDL.WarListWindow())
               g_ClientDDL.WarListWindowSpawn();

            if(!g_ClientDDL.WarListWindow()->Visible())
               g_ClientDDL.WarListWindow()->Show();
            else
               g_ClientDDL.WarListWindow()->BringToFront();
         }
         else if(in_pCaller == m_pObjDeployBtn)
         {
            if(!g_ClientDDL.UnitGroundDeploymentWindow())
               g_ClientDDL.UnitGroundDeploymentWindowSpawn();

            if(!g_ClientDDL.UnitGroundDeploymentWindow()->Visible())
               g_ClientDDL.UnitGroundDeploymentWindow()->Show();
            else
               g_ClientDDL.UnitGroundDeploymentWindow()->BringToFront();
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

void GMilitaryWindow::OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   if((SP2::ENotifyWindowMsg::Enum)in_Notification == SP2::ENotifyWindowMsg::Military)
      Update();
}

bool GMilitaryWindow::Update()
{	
   // Enable strategic warfares ?
	if(g_ClientDAL.m_PlayerCountryData.NuclearReady() < 1.f || !g_ClientDAL.GameOptions().NuclearAllowed())
		m_pObjStrategicWarfareBtn->Enabled(false);
	else
		m_pObjStrategicWarfareBtn->Enabled(true);

   // Enable Nuclear inforamtions ?
   if(g_ClientDCL.SelectedCountryID() != g_ClientDAL.ControlledCountryID() || !g_ClientDAL.GameOptions().NuclearAllowed())
   {
      DisableNuclear();
   }
   else
   {
      REAL64 m_fNuclarReady             = g_ClientDAL.m_PlayerCountryData.NuclearReady();
      REAL64 m_fAMDSLevel               = g_ClientDAL.m_PlayerCountryData.AMDSLevel();

      // Show the buttons by default
      m_pObjNuclearBuildBtn        ->Show();
      m_pObjStartNuclearResearchBtn->Show();
      m_pObjNuclearProgressMeter   ->Show();
      m_pObjAmdsResearchBtn        ->Show();
      m_pObjAMDSProgressMeter      ->Show();

      if(m_fNuclarReady < 0)
      {         
			m_pObjNuclearProgressMeter->Hide();
         m_pObjNuclearProgressMeter->Value(0,0);
         m_pObjNuclearProgressMeter->TextValueObj()->Text(g_ClientDAL.GetString(100106));
         m_pObjStartNuclearResearchBtn->Enabled(true);
         m_pObjNuclearBuildBtn->Enabled(false);
			m_pObjNuclearBuildBtn->Hide();

      }
      else
      {
         if(m_fNuclarReady == 1)
         {
				m_pObjNuclearBuildBtn->Show();
				m_pObjNuclearProgressMeter->Hide();
            m_pObjNuclearProgressMeter->Value(1,0);
            m_pObjNuclearProgressMeter->TextValueObj()->Text(L"100%");

            m_pObjStartNuclearResearchBtn->Enabled(false);            
            m_pObjStartNuclearResearchBtn->Hide();
            m_pObjNuclearBuildBtn->Show();
            m_pObjNuclearBuildBtn->Enabled(true);
         }
         else
         {
				m_pObjNuclearProgressMeter->Show();
            m_pObjNuclearProgressMeter->Value((REAL32)m_fNuclarReady,0);
            m_pObjNuclearProgressMeter->TextValueObj()->Text(GString::FormatNumber(m_fNuclarReady * 100,L"",L"",L"",L"%",0,0));         

            //RESEARCHING ...
				m_pObjStartNuclearResearchBtn->Hide();
				m_pObjNuclearBuildBtn->Hide();
            m_pObjStartNuclearResearchBtn->Text(g_ClientDAL.GetString(100299));
            m_pObjStartNuclearResearchBtn->Enabled(false);
            m_pObjNuclearBuildBtn->Enabled(false);
         }
      }

      if(m_fAMDSLevel >= 1)
      {
			m_pObjAMDSProgressMeter->Hide();
			m_pObjAmdsResearchBtn->Hide();
         m_pObjAmdsResearchBtn->Enabled(false);
         m_pObjAmdsResearchBtn->Hide();

         m_pObjAmdsLbl->Text(g_ClientDAL.GetString(100302));
         m_pObjAmdsLbl->FontColorNormal(GColorRGBInt(117,219,94,255));
      }
      else
      {
         if(m_fAMDSLevel <= 0)
         {
				m_pObjAMDSProgressMeter->Hide();
            m_pObjAmdsResearchBtn->Enabled(true);
            m_pObjAmdsResearchBtn->Show();
            m_pObjAmdsResearchBtn->Text(g_ClientDAL.GetString(100030));

            m_pObjAmdsLbl->Text(g_ClientDAL.GetString(100106));
            m_pObjAmdsLbl->FontColorNormal(GColorRGBInt(215,90,90,255));
         }
         else
         {
				m_pObjAMDSProgressMeter->Show();
				m_pObjAMDSProgressMeter->Value((REAL32)m_fAMDSLevel,0);
				m_pObjAMDSProgressMeter->TextValueObj()->Text(GString::FormatNumber(m_fAMDSLevel * 100,L"",L"",L"",L"%",0,0));         
            m_pObjAmdsResearchBtn->Enabled(false);
            m_pObjAmdsResearchBtn->Hide();
            m_pObjAmdsResearchBtn->Text(g_ClientDAL.GetString(100299));

            m_pObjAmdsLbl->Text(g_ClientDAL.GetString(100299));
            m_pObjAmdsLbl->FontColorNormal(GColorRGBInt(226,224,92,255));
         }
      }       
   }

   UpdateMilitaryUnits();
   return true;
}

/*!
* disable the nuclear features for this window
**/
void GMilitaryWindow::DisableNuclear()
{
   Clear();
   m_pObjNuclearBuildBtn        ->Hide();
   m_pObjStartNuclearResearchBtn->Hide();
   m_pObjNuclearProgressMeter   ->Hide();
   m_pObjAmdsResearchBtn        ->Hide();
   m_pObjAMDSProgressMeter      ->Hide();
}

void GMilitaryWindow::UpdateMilitaryUnits()
{
   INT32  l_iCountryId = g_ClientDCL.SelectedCountryID();
   UINT32 l_iCountryQty[SP2::EUnitCategory::ItemCount];
   UINT32 l_iCountryRank[SP2::EUnitCategory::ItemCount];
   REAL32 l_fUnitValues[SP2::EUnitCategory::ItemCount];

   // loop into every existing category
   for(UINT32 i = 0; i < SP2::EUnitCategory::ItemCount; i++)
   {
      // current country units number for this category
      g_ClientDAL.UnitQtyByCountryAndCategory(l_iCountryId, (SP2::EUnitCategory::Enum)i, l_iCountryQty[i] );
      // our units values for this category
      l_fUnitValues[i] = g_ClientDAL.TotalUnitsValue(l_iCountryId, (SP2::EUnitCategory::Enum)i);
      // default country rank      
      l_iCountryRank[i] = (l_fUnitValues[i] == 0 ? 0 : 1);
   }

   for(INT32 i=1; i<=g_ClientDAL.NbCountry(); i++)
   {
      if(i != l_iCountryId)
      {
         for(UINT32 j = 0; (j < SP2::EUnitCategory::ItemCount) && l_iCountryRank; j++)
         {
            if(g_ClientDAL.TotalUnitsValue(i, (SP2::EUnitCategory::Enum)j) > l_fUnitValues[j] )
            {
               l_iCountryRank[j]++;
            }
         }
      }
   }

   for(UINT32 i = 0; i < SP2::EUnitCategory::ItemCount; i++)
   {
      if(l_iCountryRank[i] == 0)
      {
         l_iCountryRank[i] = g_ClientDAL.NbCountry();
      }
   }

   // Update it visually
   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblSoldiersNb"))->Text(GString(l_iCountryQty[SP2::EUnitCategory::Infantry]));
   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblSoldiersRank"))->Text(GString(l_iCountryRank[SP2::EUnitCategory::Infantry]));

   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblLandNb"))->Text(GString(l_iCountryQty[SP2::EUnitCategory::Ground]));
   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblLandRank"))->Text(GString(l_iCountryRank[SP2::EUnitCategory::Ground]));

   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblAirNb"))->Text(GString(l_iCountryQty[SP2::EUnitCategory::Air]));
   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblAirRank"))->Text(GString(l_iCountryRank[SP2::EUnitCategory::Air]));

   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblNavalNb"))->Text(GString(l_iCountryQty[SP2::EUnitCategory::Naval]));
   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblNavalRank"))->Text(GString(l_iCountryRank[SP2::EUnitCategory::Naval]));
}

void GMilitaryWindow::Clear()
{
   m_pObjStartNuclearResearchBtn->Enabled(false);            
   m_pObjStartNuclearResearchBtn->Hide();

   m_pObjAmdsResearchBtn->Enabled(false);
   m_pObjAmdsResearchBtn->Hide();

   m_pObjAmdsLbl->Text(g_ClientDAL.GetString(100106));
   m_pObjAmdsLbl->FontColorNormal(GColorRGBInt(215,90,90,255));

   m_pObjNuclearProgressMeter->TextValueObj()->Text(g_ClientDAL.GetString(100106));
   m_pObjNuclearProgressMeter->Value(0,0);

   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblSoldiersNb"))->Text(L"");
   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblSoldiersRank"))->Text(L"");

   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblLandNb"))->Text(L"");
   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblLandRank"))->Text(L"");

   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblAirNb"))->Text(L"");
   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblAirRank"))->Text(L"");

   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblNavalNb"))->Text(L"");
   ((GUI::GLabel*)m_pObjConventionalFrame->Child(L"lblNavalRank"))->Text(L"");
}