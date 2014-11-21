/**************************************************************
*
* sp2_host_multiplayer_game.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_host_multiplayer_game.h"
#include "../include/sp2_generic_message_window.h"
#include "../include/sp2_join_multiplayer_game_window.h"

const GString GHostMPGameWindow::TypeName(L"Host_Multiplayer_Game");

GUI::GBaseObject* GHostMPGameWindow::New()
{
	return (GUI::GBaseObject*) new GHostMPGameWindow();
}


GHostMPGameWindow::GHostMPGameWindow() : GBaseWindow()
{
   m_pObjConquerWorldTog     = NULL;
   m_pObjWorldPeaceTog       = NULL;
   m_pObjBalanceResTog       = NULL;
   m_pObjDevelopNationTog    = NULL;
   m_pObjRaiseGDPTog         = NULL;
   m_pObjConquerWorldCbo     = NULL;
   m_pObjRaiseGDPCbo         = NULL;
   m_pObjWorldPeaceCbo       = NULL;
   m_pObjBalanceResCbo       = NULL;
   m_pObjDevelopNationCbo    = NULL;
   m_cboNuclearWarfare       = NULL;
   m_bNuclearWarfareEnabled  = true;
}

GHostMPGameWindow::~GHostMPGameWindow()
{
}

bool GHostMPGameWindow::OnCreate()
{
	__super::OnCreate();

   GFrame*    l_frmInner = (GFrame*)Child(L"frmInner");
   
   m_pObjStartGame    = (GUI::GButton*)l_frmInner->Child(L"btnStart");
   m_pObjCancelBtn    = (GUI::GButton*)l_frmInner->Child(L"btnCancel");
   
   m_edtServerName    = (GUI::GEditBox*)l_frmInner->Child(L"frmGameInfo")->Child(L"edtServerName");
   m_edtPassword      = (GUI::GEditBox*)l_frmInner->Child(L"frmGameInfo")->Child(L"edtPassword");
   m_edtPort          = (GUI::GEditBox*)l_frmInner->Child(L"frmGameSettings")->Child(L"edtPort");
   m_togPrivate       = (GUI::GToggle*)l_frmInner->Child(L"frmGameSettings")->Child(L"togPrivate");

   m_edtMaxPlayers    = (GUI::GEditBox*)l_frmInner->Child(L"frmGameSettings")->Child(L"edtMaxPlayers");
   m_edtTimeLimit     = (GUI::GEditBox*)l_frmInner->Child(L"frmGameSettings")->Child(L"edtTimeLimit");
   m_togDedicated     = (GUI::GToggle*)l_frmInner->Child(L"frmGameSettings")->Child(L"togDedicatedServer");

   m_cboGameSpeed      = (GUI::GComboBox*)l_frmInner->Child(L"frmGoals")->Child(L"cboGameSpeed");
   m_cboNuclearWarfare = (GUI::GComboBox*)l_frmInner->Child(L"frmGoals")->Child(L"cboGameSpeed1");
   
   m_pObjWorldPeaceTog       = (GUI::GToggle*)l_frmInner->Child(L"frmGoals")->Child(L"frmTog1")->Child(L"togGoal");
   m_pObjBalanceResTog       = (GUI::GToggle*)l_frmInner->Child(L"frmGoals")->Child(L"frmTog2")->Child(L"togGoal");
   m_pObjConquerWorldTog     = (GUI::GToggle*)l_frmInner->Child(L"frmGoals")->Child(L"frmTog3")->Child(L"togGoal");
   m_pObjDevelopNationTog    = (GUI::GToggle*)l_frmInner->Child(L"frmGoals")->Child(L"frmTog4")->Child(L"togGoal");
   m_pObjRaiseGDPTog         = (GUI::GToggle*)l_frmInner->Child(L"frmGoals")->Child(L"frmTog5")->Child(L"togGoal");

   m_pObjWorldPeaceCbo       = (GUI::GComboBox*)l_frmInner->Child(L"frmGoals")->Child(L"cboGoal1");
   m_pObjBalanceResCbo       = (GUI::GComboBox*)l_frmInner->Child(L"frmGoals")->Child(L"cboGoal2");
   m_pObjConquerWorldCbo     = (GUI::GComboBox*)l_frmInner->Child(L"frmGoals")->Child(L"cboGoal3");
   m_pObjDevelopNationCbo    = (GUI::GComboBox*)l_frmInner->Child(L"frmGoals")->Child(L"cboGoal4");
   m_pObjRaiseGDPCbo         = (GUI::GComboBox*)l_frmInner->Child(L"frmGoals")->Child(L"cboGoal5");

   //Hide unused buttons
   m_pObjLockTog->Hide();
   m_pObjApplyBtn->Hide();
   m_pObjExitBtn->Hide();
   
   m_pObjWorldPeaceCbo    ->Enabled(false);
   m_pObjBalanceResCbo    ->Enabled(false);
   m_pObjConquerWorldCbo  ->Enabled(false); 
   m_pObjDevelopNationCbo ->Enabled(false);
   m_pObjRaiseGDPCbo      ->Enabled(false);

   m_cboGameSpeed->Add_Item( g_ClientDAL.GetString(EStrId::GameSpeedSlowest) );
   m_cboGameSpeed->Add_Item( g_ClientDAL.GetString(EStrId::GameSpeedSlow) );
   m_cboGameSpeed->Add_Item( g_ClientDAL.GetString(EStrId::GameSpeedNormal) );
   m_cboGameSpeed->Add_Item( g_ClientDAL.GetString(EStrId::GameSpeedFast) );
   m_cboGameSpeed->Add_Item( g_ClientDAL.GetString(EStrId::GameSpeedFastest) );
   m_cboGameSpeed->Selected_Content( g_ClientDAL.GetString(EStrId::GameSpeedNormal) );

   m_cboNuclearWarfare->Add_Item( g_ClientDAL.GetString(EStrId::Permitted) );
   m_cboNuclearWarfare->Add_Item( g_ClientDAL.GetString(EStrId::NotPermitted) );
   m_cboNuclearWarfare->Selected_Content( g_ClientDAL.GetString(EStrId::Permitted) );  
   
   FeedComboGoals(m_pObjWorldPeaceCbo);
   FeedComboGoals(m_pObjBalanceResCbo);
   FeedComboGoals(m_pObjConquerWorldCbo);
   FeedComboGoals(m_pObjDevelopNationCbo);
   FeedComboGoals(m_pObjRaiseGDPCbo);
   
   m_edtServerName->Text(g_ClientDAL.GetString(c_iDefaultServerNameStrId));
   m_edtPort->Text(GString(c_iServerDefaultPort));
   m_edtMaxPlayers->Text(DEFAULT_MAX_PLAYERS);
   m_edtTimeLimit->Text(DEFAULT_GAME_TIME_LIMIT);

   m_pObjGameSpy = (GUI::GPictureBox*)Child(L"picGameSpy");
   m_pObjGameSpy->Hide(); //GameSpy is dead, hide the image

   //Hide Make private toggle
   m_togPrivate->Hide();
   GUI::GEditBox* lblPrivate = (GUI::GEditBox*)l_frmInner->Child(L"frmGameSettings")->Child(L"lblPrivate");
   if(lblPrivate)
      lblPrivate->Hide();

	return true;
}

bool GHostMPGameWindow::OnDestroy()
{
   if(g_ClientDDL.GetLoginRGAPIWindow())
   {
      g_ClientDDL.LoginRGAPIWindowKill();   
   }

   //RegisteredModalWindow.clear();

   return __super::OnDestroy();
}

GUI::EEventStatus::Enum GHostMPGameWindow::OnMouseDown(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   return __super::OnMouseDown(in_EventData, in_pCaller);
}


//! called when the button is released into our menu
GUI::EEventStatus::Enum GHostMPGameWindow::OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         //On cancel button
         if(in_pCaller == m_pObjCancelBtn)
         {
            g_ClientDCL.CancelHostMPGame();
            return GUI::EEventStatus::Handled;
         }
         
         if(in_pCaller == m_pObjStartGame)
         {
            m_sServerName     = m_edtServerName->Text();
            m_sPassword       = m_edtPassword->Text();
            m_iPort           = m_edtPort->Text().ToINT32();
            m_iMaxPlayers     = m_edtMaxPlayers->Text().ToINT32();
            m_iGameTimeLimit  = m_edtTimeLimit->Text().ToINT32();
            //m_bPrivate        = m_togPrivate->ToggleState();
            m_bDedicated      = m_togDedicated->ToggleState();            

            if (g_ClientDDL.m_bLAN)
               m_bPrivate = true;
            else
               m_bPrivate = false;

				g_ClientDCL.StartHostServerGame();
         }

         if(in_pCaller == m_pObjConquerWorldTog)
         {
            if(m_pObjConquerWorldTog->ToggleState())
            {
               m_pObjConquerWorldCbo->Enabled(true);
               AddGameObjective(EObjectiveType::ConquerTheWorld,0);
            }
            else
            {
               m_pObjConquerWorldCbo->Enabled(false);
               RemoveGameObjective(EObjectiveType::ConquerTheWorld);
            }
         }
         else if(in_pCaller == m_pObjRaiseGDPTog)
         {
            if(m_pObjRaiseGDPTog->ToggleState())
            {
               m_pObjRaiseGDPCbo->Enabled(true);
               AddGameObjective(EObjectiveType::RaiseGDPPerCapita,0,1.5f);
            }
            else
            {
               m_pObjRaiseGDPCbo->Enabled(false);
               RemoveGameObjective(EObjectiveType::RaiseGDPPerCapita);
            }
         }

         else if(in_pCaller == m_pObjWorldPeaceTog)
         {
            if(m_pObjWorldPeaceTog->ToggleState())
            {
               m_pObjWorldPeaceCbo->Enabled(true);
               AddGameObjective(EObjectiveType::AchieveWorldPeace,0);
            }
            else
            {
               m_pObjWorldPeaceCbo->Enabled(false);
               RemoveGameObjective(EObjectiveType::AchieveWorldPeace);
            }
         }

         else if(in_pCaller == m_pObjBalanceResTog)
         {
            if(m_pObjBalanceResTog->ToggleState())
            {
               m_pObjBalanceResCbo->Enabled(true);
               AddGameObjective(EObjectiveType::MeetResourcesNeeds,0);
            }
            else
            {
               m_pObjBalanceResCbo->Enabled(false);
               RemoveGameObjective(EObjectiveType::MeetResourcesNeeds);
            }
         }

         else if(in_pCaller == m_pObjDevelopNationTog)
         {
            if(m_pObjDevelopNationTog->ToggleState())
            {
               m_pObjDevelopNationCbo->Enabled(true);
               AddGameObjective(EObjectiveType::DevelopNation,0);
            }
            else
            {
               m_pObjDevelopNationCbo->Enabled(false);
               RemoveGameObjective(EObjectiveType::DevelopNation);
            }
         }
      }
   }

   return EEventStatus::Handled;
}


void GHostMPGameWindow::AddGameObjective(EObjectiveType::Enum in_eType,
                                                UINT32 in_fTimeLimitYears,
                                                REAL32 in_fValueToReach)
{
   GGameObjective l_Obj;
   l_Obj.m_eObjectiveType = in_eType;
   l_Obj.m_fValueToReach  = in_fValueToReach;
   g_ClientDCL.m_vGameObjectives.push_back(l_Obj);
   UpdateObjective(in_eType);
}

void GHostMPGameWindow::RemoveGameObjective(EObjectiveType::Enum in_eType)
{
   for(UINT32 i = 0 ; i < g_ClientDCL.m_vGameObjectives.size() ; i++)
   {
      if(g_ClientDCL.m_vGameObjectives[i].m_eObjectiveType == in_eType)
      {
         g_ClientDCL.m_vGameObjectives.erase(g_ClientDCL.m_vGameObjectives.begin()+i);
         return;
      }
   }
   gassert(0,"GStartSinglePlayerWindow::RemoveGameObjective incoherence, asked to remove a non present Objectivetype");
}

/*!
 * Update the game objective in the list of the game objectives with the data from the interface
 **/
void GHostMPGameWindow::UpdateObjective(EObjectiveType::Enum in_eType)
{
   //Get the time limit
   UINT32 l_iTimeLimit = 0;
   {
      GString l_sSelectedTime;
      switch(in_eType)
      {
         case EObjectiveType::RaiseGDPPerCapita:
         {
            l_sSelectedTime = m_pObjRaiseGDPCbo->Selected_Content();
            break;
         }
         case EObjectiveType::ConquerTheWorld:
         {
            l_sSelectedTime = m_pObjConquerWorldCbo->Selected_Content();
            break;
         }
         case EObjectiveType::AchieveWorldPeace:
         {
            l_sSelectedTime = m_pObjWorldPeaceCbo->Selected_Content();
            break;
         }
         case EObjectiveType::MeetResourcesNeeds:
         {
            l_sSelectedTime = m_pObjBalanceResCbo->Selected_Content();
            break;
         }
         case EObjectiveType::DevelopNation:
         {
            l_sSelectedTime = m_pObjDevelopNationCbo->Selected_Content();
            break;
         }
      }
      if(l_sSelectedTime == g_ClientDAL.GetString(EStrId::NoLimit))
      {
         l_iTimeLimit = 0;
      }
      else if(l_sSelectedTime == g_ClientDAL.GetString(EStrId::Goal1Year))
      {
         l_iTimeLimit = 1;
      }
      else if(l_sSelectedTime == g_ClientDAL.GetString(EStrId::Goal5Years))
      {
         l_iTimeLimit = 5;
      }
      else if(l_sSelectedTime == g_ClientDAL.GetString(EStrId::Goal10Years))
      {
         l_iTimeLimit = 10;
      }
   }
   //Update the objective with the new time limit
   for(UINT32 i = 0 ; i < g_ClientDCL.m_vGameObjectives.size() ; i++)
   {
      if(g_ClientDCL.m_vGameObjectives[i].m_eObjectiveType == in_eType)
      {
         if(l_iTimeLimit)
            g_ClientDCL.m_vGameObjectives[i].m_fTimeOutTime = g_Joshua.GameTime() + (l_iTimeLimit * 365.f);
         else
            g_ClientDCL.m_vGameObjectives[i].m_fTimeOutTime = 0.f;
         return;
      }
   }
   gassert(0,"GHostMPGameWindow::UpdateObjective incoherence, asked to update a non present Objectivetype");
   return;
}

GUI::EEventStatus::Enum GHostMPGameWindow::OnCustomEvent(UINT32 in_iEventID, const GUI::GEventData& in_EventData, GUI::GBaseObject * in_pCaller)
{
   switch (in_iEventID)
   {
      //If a combo box was selected
      case ECustomEvents::OnComboBoxSelectionEvent:
      {
         //Determine which combo box was selected and update the objective for that box
         GUI::GComboBox * l_pCombo = (GUI::GComboBox *)in_EventData.Data;
         gassert(l_pCombo, "GStartSinglePlayerWindow::OnCustomEvent Combo box pointer shouldnt be null on custom event");
         if(l_pCombo == m_pObjConquerWorldCbo)
         {
            UpdateObjective(EObjectiveType::ConquerTheWorld);
         }
         else if(l_pCombo == m_pObjRaiseGDPCbo)
         {
            UpdateObjective(EObjectiveType::RaiseGDPPerCapita);
         }
         else if(l_pCombo == m_pObjWorldPeaceCbo)
         {
            UpdateObjective(EObjectiveType::AchieveWorldPeace);
         }
         else if(l_pCombo == m_pObjBalanceResCbo)
         {
            UpdateObjective(EObjectiveType::MeetResourcesNeeds);
         }
         else if(l_pCombo == m_pObjDevelopNationCbo)
         {
            UpdateObjective(EObjectiveType::DevelopNation);
         }
         else if(l_pCombo == m_cboNuclearWarfare)
         {
            // enable disable nuclear warfare hence user choice
            if (m_cboNuclearWarfare->Selected_Content() == g_ClientDAL.GetString(EStrId::NotPermitted))
            {
               m_bNuclearWarfareEnabled = false;
            }
            else
            {
               m_bNuclearWarfareEnabled = true; 
            }
         }
      }
      default:
         break;
   }
   return EEventStatus::Handled;
}

void GHostMPGameWindow::FeedComboGoals(GUI::GComboBox* in_pCbo)
{
   in_pCbo->Add_Item( g_ClientDAL.GetString(EStrId::NoLimit) );
   in_pCbo->Add_Item( g_ClientDAL.GetString(EStrId::Goal1Year) );
   in_pCbo->Add_Item( g_ClientDAL.GetString(EStrId::Goal5Years) );
   in_pCbo->Add_Item( g_ClientDAL.GetString(EStrId::Goal10Years) );
   in_pCbo->Selected_Content( g_ClientDAL.GetString(EStrId::NoLimit) );
}