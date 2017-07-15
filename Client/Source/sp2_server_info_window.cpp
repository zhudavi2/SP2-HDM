/**************************************************************
*
* sp2_server_info_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_server_info_window.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GServerInfoWindow::TypeName(L"Server_Info_Window");

GUI::GBaseObject* GServerInfoWindow::New()
{
	return (GUI::GBaseObject*) new GServerInfoWindow();
}


GServerInfoWindow::GServerInfoWindow() : GBaseWindow()
{
   m_pCaller = NULL;
}

GServerInfoWindow::~GServerInfoWindow()
{
}

bool GServerInfoWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm = (GUI::GFrame*)Child(L"frmInner");

   m_pObjServerNameTxt  = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmInfo")->Child(L"txtName");
   m_pObjIpAddressTxt   = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmInfo")->Child(L"txtIpAddress");
   m_pObjPlayersTxt     = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmInfo")->Child(L"txtPlayers");

   m_pObjJoinGameBtn    = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnJoinGame");

   m_pObjModsTxt        = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmInfo")->Child(L"txtMods");
   m_pObjScenarioTxt    = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmInfo")->Child(L"txtScenario");

   m_pObjPasswordCbo    = (GUI::GToggle*)m_pObjInnerFrm->Child(L"frmInfo")->Child(L"togPassword");

   m_pObjPasswordCbo->Enabled(false);

   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   Position(((INT16)mode.m_Resolution.x / 2) - (Width() / 2),((INT16)mode.m_Resolution.y / 2) - (Height() / 2));

	return true;
}

void GServerInfoWindow::Show(GUI::GBaseObject* in_pCaller)
{
   __super::Show(); 
   m_pCaller = in_pCaller;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GServerInfoWindow::OnShow()
{
   __super::OnShow();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

GUI::EEventStatus::Enum GServerInfoWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);
   
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjJoinGameBtn)
         {
            GUI::GEventData l_Data;

            if(m_pCaller)
               m_pCaller->FireCustomEvent(c_OnServerInfoWindowJoin, l_Data, true, this);

            Hide();
            return GUI::EEventStatus::Handled;
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

void GServerInfoWindow::Update(const GListedServer & in_Server)
{
   m_pObjServerNameTxt->Text(in_Server.m_sServerName);
   m_pObjIpAddressTxt ->Text(GString(inet_ntoa(((SOCKADDR_IN*)&in_Server.m_ServerAdress)->sin_addr)));
   m_pObjPlayersTxt   ->Text(GString(in_Server.m_iNbPlayers) + L"/" + GString(in_Server.m_iMaxPlayers));
   m_pObjModsTxt      ->Text(in_Server.m_sModName);
   m_pObjScenarioTxt  ->Text(in_Server.m_sScenarioName);

   m_pObjPasswordCbo  ->ToggleState(in_Server.m_bHasPassword);
}