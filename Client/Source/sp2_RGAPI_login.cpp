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

#include "../include/sp2_RGAPI_Login.h"
#include "../include/sp2_connecting_window.h"
#include "../include/sp2_generic_message_window.h"
#include "../include/sp2_join_multiplayer_game_window.h"

const GString GLoginRGAPIWindow::TypeName(L"frmBaseWindow");

GUI::GBaseObject* GLoginRGAPIWindow::New()
{
	return (GUI::GBaseObject*) new GLoginRGAPIWindow();
}


GLoginRGAPIWindow::GLoginRGAPIWindow() : GBaseWindow()
{
}

GLoginRGAPIWindow::~GLoginRGAPIWindow()
{
}

void GLoginRGAPIWindow::Show(GUI::GBaseObject* in_pCaller)
{
   m_pCaller = in_pCaller;

   m_edtUserName->Text("");
   m_edtPassword->Text("");
   m_bIsLoginWindow = true;

   m_pCaller->Hide();
   __super::Show();
}

bool GLoginRGAPIWindow::OnCreate()
{
	__super::OnCreate();
	
	this->Height(220);
	m_pObjGradientPic->Height(214);


	l_mFrame   = (GUI::GFrame*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"g_def_frame", L"frameLogin", this, true, true);
	l_mFrame->PictureNormal(GString(L"000647"));
	l_mFrame->Anchors(GAnchorPoints("TopLeft"));
	l_mFrame->Borders( GBorders(1, 1, 1, 1) );
	l_mFrame->Position(18, 29);
	l_mFrame->Size(305, 120);
		
	m_lblTitle = (GUI::GLabel*)m_pObjFrameTitle->Child(L"lblTitle");
	if(m_lblTitle)
		m_lblTitle->Text(g_ClientDAL.GetString(102372));

	m_txtUserName    = (GUI::GEditBox*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"lblNormal", L"txtUsernameRGAPI", l_mFrame, true, true);
	m_txtUserName->Anchors(GAnchorPoints("TopLeft"));
	m_txtUserName->Position(30, 20);
	m_txtUserName->Text(g_ClientDAL.GetString(102373));

	m_edtUserName    = (GUI::GEditBox*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"edtNormal", L"edtUsernameRGAPI", l_mFrame, true, true);	
	m_edtUserName->Anchors(GAnchorPoints("TopLeft"));
	m_edtUserName->Position(30, 35);
	m_edtUserName->Width(130);
	m_edtUserName->SetMaxLength(25);
	
	m_txtPassword    = (GUI::GEditBox*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"lblNormal", L"txtPasswordRGAPI", l_mFrame, true, true);
	m_txtPassword->Anchors(GAnchorPoints("TopLeft"));
	m_txtPassword->Position(30, 65);
	m_txtPassword->Text(g_ClientDAL.GetString(102362));

	m_edtPassword    = (GUI::GEditBox*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"edtNormal", L"edtPasswordRGAPI", l_mFrame, true, true);
	m_edtPassword->Anchors(GAnchorPoints("TopLeft"));
	m_edtPassword->Position(30, 80);
	m_edtPassword->Width(130);
	m_edtPassword->SetMaxLength(25);
	m_edtPassword->SetPasswordChar('*');   
	
	m_pObjConnectBtn    = (GUI::GButton*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"btnNormal", L"btnConnectRGAPI", this, true, true);
	m_pObjConnectBtn->Anchors(GAnchorPoints("TopLeft"));
	m_pObjConnectBtn->Position(40, 160);
	m_pObjConnectBtn->Width(110);
	m_pObjConnectBtn->Text(g_ClientDAL.GetString(102363));

	m_pObjCancelLoginBtn    = (GUI::GButton*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"btnNormal", L"btnCancelRGAPI", this, true, true);
	m_pObjCancelLoginBtn->Anchors(GAnchorPoints("TopLeft"));
	m_pObjCancelLoginBtn->Position(200, 160);
	m_pObjCancelLoginBtn->Width(110);
	m_pObjCancelLoginBtn->Text(g_ClientDAL.GetString(102364));

	m_pObjRegisterBtn    = (GUI::GButton*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"btnNormal", L"btnRegisterRGAPI", this, true, true);
	m_pObjRegisterBtn->Anchors(GAnchorPoints("TopLeft"));
	m_pObjRegisterBtn->Position(40, 185);
	m_pObjRegisterBtn->Width(270);
    m_pObjRegisterBtn->Text(g_ClientDAL.GetString(102365));

    //Register window
    m_txtPassword2    = (GUI::GEditBox*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"lblNormal", L"txtPassword2RGAPI", l_mFrame, false, true);
    m_txtPassword2->Anchors(GAnchorPoints("TopLeft"));
    m_txtPassword2->Position(30, 110);
    m_txtPassword2->Text(g_ClientDAL.GetString(102366));

    m_edtPassword2    = (GUI::GEditBox*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"edtNormal", L"edtPassword2RGAPI", l_mFrame, false, true);
    m_edtPassword2->Anchors(GAnchorPoints("TopLeft"));
    m_edtPassword2->Position(30, 125);
	m_edtPassword2->Width(130);
	m_edtPassword2->SetMaxLength(25);
    m_edtPassword2->SetPasswordChar('*');
    
    m_txtEmail    = (GUI::GEditBox*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"lblNormal", L"txtEmailRGAPI", l_mFrame, false, true);
    m_txtEmail->Anchors(GAnchorPoints("TopLeft"));
    m_txtEmail->Position(30, 155);
    m_txtEmail->Text(g_ClientDAL.GetString(102367));
    
    m_edtEmail    = (GUI::GEditBox*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"edtNormal", L"edtEmailRGAPI", l_mFrame, false, true);
    m_edtEmail->Anchors(GAnchorPoints("TopLeft"));
    m_edtEmail->Position(30, 170);
	m_edtEmail->Width(190);
    
    m_pObjCreateBtn = (GUI::GButton*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"btnNormal", L"btnCreateRGAPI", this, false, true);
    m_pObjCreateBtn->Anchors(GAnchorPoints("TopLeft"));
    m_pObjCreateBtn->Position(40, 250);
	m_pObjCreateBtn->Width(110);
    m_pObjCreateBtn->Text(g_ClientDAL.GetString(102368));

    m_pObjCancelRegisterBtn = (GUI::GButton*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"btnNormal", L"btnCancelRegisterRGAPI", this, false, true);
    m_pObjCancelRegisterBtn->Anchors(GAnchorPoints("TopLeft"));
    m_pObjCancelRegisterBtn->Position(200, 250);
	m_pObjCancelRegisterBtn->Width(110);
    m_pObjCancelRegisterBtn->Text(g_ClientDAL.GetString(102364));

   //Hide unused buttons
   m_pObjLockTog->Hide();
   m_pObjApplyBtn->Hide();
   m_pObjExitBtn->Hide();

	return true;
}

bool GLoginRGAPIWindow::OnDestroy()
{
   return __super::OnDestroy();
}

GUI::EEventStatus::Enum GLoginRGAPIWindow::OnMouseDown(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   return __super::OnMouseDown(in_EventData, in_pCaller);
}


//! called when the button is released into our menu
GUI::EEventStatus::Enum GLoginRGAPIWindow::OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         //On cancel button
         if(in_pCaller == m_pObjCancelLoginBtn)
         {
			   //Show the join or host window
			   m_pCaller->Show();
            Hide();
            
            return GUI::EEventStatus::Handled;
         }
         if(in_pCaller == m_pObjCancelRegisterBtn)
         {
            m_edtUserName->Text("");
            ShowLoginWindow();
            return GUI::EEventStatus::Handled;
         }
         
         if(in_pCaller == m_pObjConnectBtn)
         {
            m_sUserName     = m_edtUserName->Text();
            m_sPassword       = m_edtPassword->Text();

            g_ClientDDL.ConnectingWindowSpawn();
            g_ClientDDL.ConnectingWindow()->SetDisplayText(g_ClientDAL.GetString(102370) );
            g_ClientDDL.ConnectingWindow()->SetCancelEvent(FSM::EEvents::CancelConnect);
            //Hide();

            g_Joshua.Client()->RGAPILogin(m_sUserName, m_sPassword);
            return GUI::EEventStatus::Handled;
         }

		    if(in_pCaller ==  m_pObjRegisterBtn)
		    {
             ShowRegisterWindow();
             return GUI::EEventStatus::Handled;
          }       
         if(in_pCaller == m_pObjCreateBtn)
         {
            m_sUserName     = m_edtUserName->Text();
            if(m_edtPassword->Text() == m_edtPassword2->Text())
            {
               g_ClientDDL.ConnectingWindowSpawn();
               g_ClientDDL.ConnectingWindow()->SetDisplayText(g_ClientDAL.GetString(102370) );
               g_ClientDDL.ConnectingWindow()->SetCancelEvent(FSM::EEvents::CancelConnect);
               //Hide();
               g_Joshua.Client()->RAGPIRegisterAccount(m_edtUserName->Text(), m_edtPassword->Text(), m_edtEmail->Text());
            }
            else
            {
               m_edtPassword->Text("");
               m_edtPassword2->Text("");
               
               g_ClientDDL.GenericMessageWindowSpawn();
               g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(102376), this );
            }
            return GUI::EEventStatus::Handled;
         }
      }
   }

   return EEventStatus::Handled;
}

void GLoginRGAPIWindow::OnLoginCompleted(bool bSuccess)
{
   g_ClientDDL.ConnectingWindowKill();   
   if(!bSuccess)
   {
      m_edtUserName->Text("");
      m_edtPassword->Text("");
      m_edtPassword2->Text("");
      m_edtEmail->Text("");

      //Show error msg
      g_ClientDDL.GenericMessageWindowSpawn();
      g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(102374), this );
   }
   else
   {
      Hide();
      g_ClientDDL.m_bLAN = false;
      g_ClientDDL.JoinMPGameWindowSpawn();      
      g_ClientDDL.JoinMPGameWindow()->Show();
      g_ClientDCL.JoinMultiplayerGame();
   }   
}

void GLoginRGAPIWindow::OnRegisterCompleted(bool bSuccess)
{
   g_ClientDDL.ConnectingWindowKill();
	if(bSuccess)
	{
		ShowLoginWindow();
	}
	else
   {  
      m_edtUserName->Text("");
      m_edtPassword->Text("");
      m_edtPassword2->Text("");
      m_edtEmail->Text("");

      g_ClientDDL.GenericMessageWindowSpawn();
      g_ClientDDL.GenericMessageWindow()->Show( g_ClientDAL.GetString(102375), this );
	}
}

void GLoginRGAPIWindow::ShowRegisterWindow()
{
   this->Height(285);   
   m_pObjGradientPic->Height(279);

   l_mFrame->Size(305, 210);

   m_lblTitle->Text(g_ClientDAL.GetString(102369));

   m_edtUserName->Text("");
   m_edtPassword->Text("");
   m_edtPassword2->Text("");
   m_edtEmail->Text("");

   m_txtPassword2->Show();
   m_edtPassword2->Show();

   m_txtEmail->Show();
   m_edtEmail->Show();

   m_pObjConnectBtn->Hide();   
   m_pObjRegisterBtn->Hide();
   m_pObjCancelLoginBtn->Hide();

   m_pObjCreateBtn->Show();
   m_pObjCancelRegisterBtn->Show();

   m_bIsLoginWindow = false;
}
void GLoginRGAPIWindow::ShowLoginWindow()
{
   this->Height(220);
   m_pObjGradientPic->Height(214);   

   l_mFrame->Size(305, 120);

   m_lblTitle->Text(g_ClientDAL.GetString(102372));

   //do not erase the username
   m_edtPassword->Text("");
   m_edtPassword2->Text("");
   m_edtEmail->Text("");

   m_txtPassword2->Hide();
   m_edtPassword2->Hide();
   
   m_txtEmail->Hide();
   m_edtEmail->Hide();

   m_pObjConnectBtn->Show();   
   m_pObjRegisterBtn->Show();
   m_pObjCancelLoginBtn->Show();

   m_pObjCreateBtn->Hide();
   m_pObjCancelRegisterBtn->Hide();

   m_bIsLoginWindow = true;
}
