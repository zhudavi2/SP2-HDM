/**************************************************************
*
* sp2_game_scenario_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/

// Includes
#include "golem_pch.hpp"
#include "../include/sp2_game_scenario_window.h"
#include "../include/sp2_frame_scenario.h"
#include "../include/sp2_main_menu.h"
#include "../../includes/common_lib//sp2_game_scenario.h"

// Constants
const GString GGameScenarioWindow::TypeName(L"Scenarios_Window");
const UINT32   c_iRegularScenarioCount = 9;


// Constructors and Destructors
GUI::GBaseObject* GGameScenarioWindow::New()
{
   return new GGameScenarioWindow();
}

GGameScenarioWindow::GGameScenarioWindow()
   : SP2::GBaseWindow()
{
}

GGameScenarioWindow::~GGameScenarioWindow()
{
}

bool GGameScenarioWindow::OnCreate()
{
   __super::OnCreate();

   m_pObjApplyBtn->Hide();
   m_pObjLockTog->Hide();

   // Collect GUI item
   m_pObjPoliticEasy          = (GUI::GToggle* )Child("frmInner")->Child("frmGolem")->Child("togPoliticBeginner");
   m_pObjPoliticNormal        = (GUI::GToggle* )Child("frmInner")->Child("frmGolem")->Child("togPoliticIntermediate");
   m_pObjPoliticHard          = (GUI::GToggle* )Child("frmInner")->Child("frmGolem")->Child("togPoliticExpert");
   m_pObjEconomicEasy         = (GUI::GToggle* )Child("frmInner")->Child("frmGolem")->Child("togEconomicBeginner");
   m_pObjEconomicNormal       = (GUI::GToggle* )Child("frmInner")->Child("frmGolem")->Child("togEconomicIntermediate");
   m_pObjEconomicHard         = (GUI::GToggle* )Child("frmInner")->Child("frmGolem")->Child("togEconomicExpert");
   m_pObjMilitaryEasy         = (GUI::GToggle* )Child("frmInner")->Child("frmGolem")->Child("togMilitaryBeginner");
   m_pObjMilitaryNormal       = (GUI::GToggle* )Child("frmInner")->Child("frmGolem")->Child("togMilitaryIntermediate");
   m_pObjMilitaryHard         = (GUI::GToggle* )Child("frmInner")->Child("frmGolem")->Child("togMilitaryExpert");

   m_pObjCustomScenarioList   = (GUI::GListBox*)Child("frmInner")->Child(L"frmCustom")->Child(L"lstCustom");

   m_pObjStartBtn             = (GUI::GButton*)Child("frmInner")->Child("btnStart");
   m_pObjCancelBtn            = (GUI::GButton*)Child("frmInner")->Child("btnCancel");

   m_pScenarioFrm             = (SP2::GFrameScenario*)Child(L"frmInner")->Child(L"frmScenario");

   // If the number of country is greater than zero, it means we already have received all the country information.
   if(g_ClientDAL.Countries().size() > 0)
      OnEndReceivedCountryData();

   return true;
}

void GGameScenarioWindow::OnEndReceivedCountryData(void)
{
   if(g_ClientDAL.Scenarios().size() == 0)
   {
      // Read Scenario files
      g_ClientDAL.ReadScenarios("Data\\Scenarios\\Campaing Scenarios.xml");

      // Read custo, scenario files
      
      GFile l_file;
      if(!g_Joshua.FileManager()->FileOnDisk("Data\\Scenarios\\Custom Scenarios.xml",l_file))
      {
         g_ClientDAL.ReadScenarios("Data\\Scenarios\\Custom Scenarios.xml");
         g_Joshua.Log("No custom scenarios files");
         return;
      } 

      

      // Fill out Scenario List
      FillOutScenarios();

      // Select the first one
      g_SP2Client->ScenarioSphere(0, false);
      g_ClientDAL.Scenario(g_ClientDAL.Scenarios()[0]);
      m_pScenarioFrm->Scenario( g_ClientDAL.Scenarios()[0] );

      m_pObjPoliticEasy->ToggleState(true);

      // Update Selected scenario
      UpdateSelectedScenario();
   }

   m_pObjPoliticEasy->Enabled(true);
   m_pObjPoliticNormal->Enabled(true);
   m_pObjPoliticHard->Enabled(true); 
   m_pObjEconomicEasy->Enabled(true);
   m_pObjEconomicNormal->Enabled(true);
   m_pObjEconomicHard->Enabled(true);
   m_pObjMilitaryEasy->Enabled(true);
   m_pObjMilitaryNormal->Enabled(true);
   m_pObjMilitaryHard->Enabled(true);
   m_pObjCustomScenarioList->Enabled(true);


   // Crop Items that are not needed
   m_pScenarioFrm->m_pObjScenarioCountry->Enabled(true);
   m_pObjCancelBtn->Show();
   m_pObjStartBtn->Show();
}

void  GGameScenarioWindow::FillOutScenarios(void)
{
   // Clear scenario list
   m_pObjCustomScenarioList->Clear();

   // The first 9(c_iRegularScenarioCount) scenario in the scenario list are for the campaign
   for(UINT i=0;i<g_ClientDAL.Scenarios().size() - c_iRegularScenarioCount;i++)
   {
      GGameScenario* l_pScenario = g_ClientDAL.Scenarios()[i+c_iRegularScenarioCount];
      // Add custom scenario to the list
      GUI::GListBox::GRow* l_pRow =  m_pObjCustomScenarioList->Insert_New_Row();
      ((GLabel*)l_pRow->Object())->Text(g_ClientDAL.StringReplaceNewLineChar(g_ClientDAL.GetString(l_pScenario->Name().ToINT32())));
   }
   m_pObjCustomScenarioList->Update();
}

void GGameScenarioWindow::ClearSelectedScenario()
{
   m_pScenarioFrm->Scenario(GSmartPtr<GGameScenario>(NULL) );
}

void GGameScenarioWindow::UpdateSelectedScenario()
{
   m_pScenarioFrm->UpdateSelectedScenario();
}

GUI::EEventStatus::Enum GGameScenarioWindow::OnCustomEvent(UINT32 in_EventID, const GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   __super::OnCustomEvent(in_EventID,in_EventData, in_pCaller);  

   switch(in_EventID)
   {
   case ECustomEvents::OnListItemSelected:
      {
         GListBox* l_pSelectedList = (GListBox*)in_pCaller;

         if(in_pCaller == m_pObjCustomScenarioList)
         {
            UINT32 l_iRowID =  l_pSelectedList->Get_Selected_Item()[0];
            if((l_iRowID + c_iRegularScenarioCount)  < g_ClientDAL.Scenarios().size())
            {
               m_pScenarioFrm->Scenario( g_ClientDAL.Scenarios()[l_iRowID + c_iRegularScenarioCount] );
               g_ClientDAL.Scenario(g_ClientDAL.Scenarios()[l_iRowID + c_iRegularScenarioCount]);
            }
            else
            {
               m_pScenarioFrm->Scenario( GSmartPtr<GGameScenario>(NULL) );
            }
            UpdateSelectedScenario();
         }
      }
      break;
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GGameScenarioWindow::OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);  

   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(in_pCaller == m_pObjCancelBtn || in_pCaller == m_pObjExitBtn)
      {
         Hide();
         g_ClientDCL.CancelConnect();
      }
      else
      if(in_pCaller == m_pObjStartBtn)
      {
         if(m_pScenarioFrm->Scenario() != NULL)
         {
            // Get Selected Country
            UINT32 l_iCountryID = m_pScenarioFrm->SelectedCountryID();
            if( l_iCountryID < 0 )
               return EEventStatus::Handled;

            g_ClientDCL.SelectScenario(m_pScenarioFrm->Scenario(), 
                                       l_iCountryID, 
                                       g_ClientDAL.m_DataOptionsWindow.m_sPlayerName);

            m_pObjStartBtn->Enabled(false);
         }
         Hide();
      }
      else
      {
         if(in_pCaller ==  m_pObjEconomicEasy)
         {
            g_SP2Client->ScenarioSphere(2, false);
            g_ClientDAL.Scenario(g_ClientDAL.Scenarios()[3]);
            m_pScenarioFrm->Scenario( g_ClientDAL.Scenarios()[3] );
            UpdateSelectedScenario();
         }
         else
         if(in_pCaller ==  m_pObjEconomicNormal)
         {
            g_SP2Client->ScenarioSphere(2, false);
            g_ClientDAL.Scenario(g_ClientDAL.Scenarios()[4]);
            m_pScenarioFrm->Scenario( g_ClientDAL.Scenarios()[4] );
            UpdateSelectedScenario();
         }
         else
         if(in_pCaller ==  m_pObjEconomicHard)
         {
            g_SP2Client->ScenarioSphere(2, true);
            g_ClientDAL.Scenario(g_ClientDAL.Scenarios()[5]);
            m_pScenarioFrm->Scenario( g_ClientDAL.Scenarios()[5] );
            UpdateSelectedScenario();
         }
         else
         if(in_pCaller ==  m_pObjPoliticEasy)
         {
            g_SP2Client->ScenarioSphere(0, false);
            g_ClientDAL.Scenario(g_ClientDAL.Scenarios()[0]);
            m_pScenarioFrm->Scenario( g_ClientDAL.Scenarios()[0] );
            UpdateSelectedScenario();
         }
         else
         if(in_pCaller ==  m_pObjPoliticNormal)
         {
            g_SP2Client->ScenarioSphere(0, false);
            g_ClientDAL.Scenario(g_ClientDAL.Scenarios()[1]);
            m_pScenarioFrm->Scenario( g_ClientDAL.Scenarios()[1] );
            UpdateSelectedScenario();
         }
         else
         if(in_pCaller ==  m_pObjPoliticHard)
         {
            g_SP2Client->ScenarioSphere(0, true);
            g_ClientDAL.Scenario(g_ClientDAL.Scenarios()[2]);
            m_pScenarioFrm->Scenario( g_ClientDAL.Scenarios()[2] );
            UpdateSelectedScenario();
         }
         else
         if(in_pCaller ==  m_pObjMilitaryEasy)
         {
            g_SP2Client->ScenarioSphere(1, false);
            g_ClientDAL.Scenario(g_ClientDAL.Scenarios()[6]);
            m_pScenarioFrm->Scenario( g_ClientDAL.Scenarios()[6] );
            UpdateSelectedScenario();
         }
         else
         if(in_pCaller ==  m_pObjMilitaryNormal)
         {
            g_SP2Client->ScenarioSphere(1, false);
            g_ClientDAL.Scenario(g_ClientDAL.Scenarios()[7]);
            m_pScenarioFrm->Scenario( g_ClientDAL.Scenarios()[7] );
            UpdateSelectedScenario();
         }
         else
         if(in_pCaller ==  m_pObjMilitaryHard)
         {
            g_SP2Client->ScenarioSphere(1, true);
            g_ClientDAL.Scenario(g_ClientDAL.Scenarios()[8]);
            m_pScenarioFrm->Scenario( g_ClientDAL.Scenarios()[8] );
            UpdateSelectedScenario();
         }
      }
   }

   return GUI::EEventStatus::Handled;
}
