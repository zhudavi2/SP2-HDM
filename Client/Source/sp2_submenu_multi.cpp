/**************************************************************
*
* sp2_submenu_Multi.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_submenu_multi.h"
#include "../include/sp2_load_game_window.h"
#include "../include/sp2_main_menu.h"
#include "../include/sp2_fsm_main.h"
#include "../include/sp2_RGAPI_login.h"
#include "../include/sp2_join_multiplayer_game_window.h"

const GString GSubmenuMulti::TypeName(L"Submenu_Multi");

GUI::GBaseObject* GSubmenuMulti::New()
{
	return (GUI::GBaseObject*) new GSubmenuMulti();
}

GSubmenuMulti::GSubmenuMulti() : SP2::GBaseWindow()
{
}

GSubmenuMulti::~GSubmenuMulti()
{
}

GUI::EEventStatus::Enum GSubmenuMulti::OnMouseClick(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);

	if(!in_pCaller)
      return GUI::EEventStatus::Handled;

   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(in_pCaller == m_pObjInternet)
      {
         g_ClientDDL.LoginRGAPIWindowSpawn();            
         g_ClientDDL.GetLoginRGAPIWindow()->Show(this);
      }
      else if(in_pCaller == m_pObjLAN)
      {
         g_ClientDDL.m_bLAN = true;
         g_ClientDDL.JoinMPGameWindowSpawn();
         g_ClientDDL.JoinMPGameWindow()->Show();
         g_ClientDCL.JoinMultiplayerGame();
      }
      else if(in_pCaller == m_pObjReturnMain)
      {
         g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::SubMenuBackBtn);
      }
   }

	return GUI::EEventStatus::Handled;
}

bool GSubmenuMulti::OnCreate()
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
   
   m_pObjInternet = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnHost");
   m_pObjInternet->Text(g_ClientDAL.GetString(100145));//Internet
   m_pObjLAN      = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnJoin");
   m_pObjLAN->Text(g_ClientDAL.GetString(100146)); //LAN
   m_pObjLoad        = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnLoad");
   m_pObjLoad->Hide();
   m_pObjReturnMain  = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnReturn");
   m_pObjSep         = (GUI::GPictureBox*)m_pObjInnerFrm->Child(L"picSep");

   ((GFX::GWindow*)m_pObjSep->Model()->Get_Primitive())->BackgroundType(GFX::EBgndType::Mosaic);

   //Reduce the height of the window
   int h = -28;
   Height(Height() + h);
   m_pObjInnerFrm->Height(m_pObjInnerFrm->Height() + h);
   GUI::GBaseObject* picWhite  = (GUI::GBaseObject*)Child(L"picWhiteGradient");
   if(picWhite)
      picWhite->Height(picWhite->Height() + h);
   m_pObjGradientPic->Height(m_pObjGradientPic->Height() + h);

   GUI::GBaseObject* picSep = (GUI::GBaseObject*)m_pObjInnerFrm->Child(L"picSep");;
   if(picSep)
      picSep->Top(picSep->Top() + h);
   
   m_pObjReturnMain->Top(m_pObjReturnMain->Top() + h);

	return true;
}


//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GSubmenuMulti::OnShow()
{
   __super::OnShow();

   BringToFront(); 
}

void GSubmenuMulti::OnHide()
{
   __super::OnHide();
}