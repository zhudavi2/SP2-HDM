/**************************************************************
*
* sp2_submenu_single.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_submenu_single.h"
#include "../include/sp2_load_game_window.h"
#include "../include/sp2_game_scenario_window.h"
#include "../include/sp2_main_menu.h"
#include "../include/sp2_fsm_main.h"

const GString GSubmenuSingle::TypeName(L"Submenu_Single");

GUI::GBaseObject* GSubmenuSingle::New()
{
	return (GUI::GBaseObject*) new GSubmenuSingle();
}

GSubmenuSingle::GSubmenuSingle() : SP2::GBaseWindow()
{
}

GSubmenuSingle::~GSubmenuSingle()
{
}

GUI::EEventStatus::Enum GSubmenuSingle::OnMouseClick(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);

	if(!in_pCaller)
      return GUI::EEventStatus::Handled;

   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(in_pCaller == m_pObjTutorial1)
      {
         g_ClientDCL.StartTutorial(ETutorials::InterfaceTutorial);
      }
      else if(in_pCaller == m_pObjTutorial2)
      {
         g_ClientDCL.StartTutorial(ETutorials::MilitaryTutorial);
      }
      else if(in_pCaller == m_pObjScenario)
      {
         g_ClientDCL.StartCampaingSinglePlayerGame();
      }
      else if(in_pCaller == m_pObjCustomGame)
      {
         if(g_SP2Client->CurrentFSMState() == FSM::EStates::Playing)
         {
            if(g_SP2Client->CurrentGameType() == EGameTypes::SinglePlayer)
            {
               g_SP2Client->RestartGame();
            }
            else
            {
               g_SP2Client->StopGame();
            }
         }
         else
         {
            g_ClientDCL.StartSingleplayerGame();         
         }
      }
      else if(in_pCaller == m_pObjLoadGame)
      {
         if(!g_ClientDDL.LoadGameWindow())
            g_ClientDDL.LoadGameWindowSpawn();

         g_ClientDDL.LoadGameWindow()->Show(true);
      }
      else if(in_pCaller == m_pObjReturnMain)
      {
         g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::SubMenuBackBtn);
      }
   }

	return GUI::EEventStatus::Handled;
}

bool GSubmenuSingle::OnCreate()
{
	__super::OnCreate();
   m_pObjLockTog->Hide();
   m_pObjApplyBtn->Hide();
   m_pObjExitBtn->Hide();   
   
   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   INT32 x = (mode.m_Resolution.x - Width()) / 2;
   INT32 y = (mode.m_Resolution.y - Height()) / 2;

   Position(x,y);

   m_pObjInnerFrm    = (GUI::GFrame*)Child(L"frmInner");
   
   m_pObjTutorial1   = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnTutorial1");
   m_pObjTutorial2   = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnTutorial2");
   m_pObjScenario    = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnScenario");
   m_pObjCustomGame  = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnCustom");
   m_pObjLoadGame    = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnLoad");
   m_pObjReturnMain  = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnReturn");

   m_pObjSep         = (GUI::GPictureBox*)m_pObjInnerFrm->Child(L"picSep");
   m_pObjSep1        = (GUI::GPictureBox*)m_pObjInnerFrm->Child(L"picSep1");

   ((GFX::GWindow*)m_pObjSep->Model()->Get_Primitive())->BackgroundType(GFX::EBgndType::Mosaic);
   ((GFX::GWindow*)m_pObjSep1->Model()->Get_Primitive())->BackgroundType(GFX::EBgndType::Mosaic);

	return true;
}


//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GSubmenuSingle::OnShow()
{
   __super::OnShow();

   BringToFront(); 
}

void GSubmenuSingle::OnHide()
{
   __super::OnHide();
}