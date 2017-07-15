/**************************************************************
*
* sp2_main_menu.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_main_menu.h"

#include "../include/sp2_credits_window.h"
#include "../include/sp2_fsm_main.h"
#include "../include/sp2_game_scenario_window.h"
#include "../include/sp2_game_options_window.h"
#include "../include/sp2_submenu_single.h"
#include "../include/sp2_submenu_multi.h"
#include "../include/sp2_game_lobby_window.h"
#include "../include/sp2_generic_message_window.h"

// The next include seems weird, but its to get the sprite from the battle Unit (I know i should have split the header file : No time. sorry)
#include "../include/sp2_battle_overview_unit.h"



const GString GMainMenu::TypeName(L"Main_Menu");

/*!
 * MAIN MENU construction function
 **/
GUI::GBaseObject* GMainMenu::New()
{
	return (GUI::GBaseObject*) new GMainMenu();
}


GMainMenu::GMainMenu() : SP2::GBaseWindow() , m_pTitleLogo(NULL)
{
}

GMainMenu::~GMainMenu()
{
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Remove_Track(m_pLogoFadeOutTrack);
   RegisteredModalWindow.clear();
   SAFE_RELEASE(m_pLogoFadeOutTrack);
   SAFE_DELETE(m_pTitleLogo);
}

GUI::EEventStatus::Enum GMainMenu::OnCustomEvent(UINT32 in_iEventID,const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   __super::OnCustomEvent(in_iEventID,in_EventData, in_pCaller);
   if(in_iEventID == c_OnGenericMessageWindowAnswerEvent)
   {
      // Unregister window
      vector<GUI::GBaseObject*>::iterator l_It = find(RegisteredModalWindow.begin(), RegisteredModalWindow.end(), g_ClientDDL.GenericMessageDesktopWindow());
      gassert(l_It != RegisteredModalWindow.end(),"We attached ourselve in the show...it has to be there");
      RegisteredModalWindow.erase(l_It);

      // Kill it
      g_ClientDDL.GenericMessageDesktopWindowKill();
   }

   return GUI::EEventStatus::Handled;
}

//! called when the button is released into our menu
GUI::EEventStatus::Enum GMainMenu::OnMouseClick(const GEventData & in_EventData,GUI::GBaseObject* in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);

	if(!in_pCaller)
      return GUI::EEventStatus::Handled;

   bool l_bMustClose = false;

   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
	   //Single player or Return to game
      if(in_pCaller == m_pObjSingleBtn)
      {
         if (g_SP2Client->CurrentFSMState() != FSM::EStates::Disconnected)
         {
            //Simply hide menu
            l_bMustClose = true;
         }
         else
         {
            g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::SPMenuBtn);
         }
      }

      //Multiplayer or Restart
      if(in_pCaller == m_pObjMultiBtn)
      {
         if (g_SP2Client->CurrentFSMState() != FSM::EStates::Disconnected)
         {
            g_SP2Client->RestartGame();
         }
         else
         {
            g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::MPMenuBtn);
         }
      }

      //Stop or disconnect
      if(in_pCaller == m_pObjStopBtn)
      {
         if (g_SP2Client->CurrentFSMState() != FSM::EStates::Disconnected)
         {
            g_SP2Client->StopGame();
         }
         else
         {
            g_SP2Client->m_pStateMachine->SendEvent(FSM::EEvents::MPMenuBtn);
         }
      }

      //Quit
      if(in_pCaller == m_pObjQuitBtn)
	   {
		   //Actually force the quit game
         g_SP2Client->QuitGame();		
         l_bMustClose = true;
	   }

      //Credits button
      if(in_pCaller == m_pObjCreditsBtn)
      {
         if(!g_ClientDDL.CreditsWindow())
            g_ClientDDL.CreditsWindowSpawn();

         g_ClientDDL.CreditsWindow()->Show();
      }

      //Options button
      if(in_pCaller == m_pObjOptionsBtn)
      {
         if(!g_ClientDDL.GameOptionsWindow())
            g_ClientDDL.GameOptionsWindowSpawn();

         g_ClientDDL.GameOptionsWindow()->Show();
      }
   }

   if(l_bMustClose)
      this->Hide();

	return GUI::EEventStatus::Handled;
}

bool GMainMenu::OnCreate()
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

   BringToFront();


   //Display the version number (small lower right corner)
   {
      m_pVersionLbl = CREATE_GUI_OBJECT_STATIC(GUI::GLabel,this,true);
      m_pVersionLbl->Text(g_Joshua.CurrentMod().m_sName +" V" + g_Joshua.CurrentMod().m_sVersion);
      //m_pVersionLbl->Text("SDK Version");
      m_pVersionLbl->FontColorNormal(GColorRGBInt(255,255,255,255));
      m_pVersionLbl->ColorNormal(GColorRGBInt(0,0,0,0));
      m_pVersionLbl->Font(c_sDefaultFont,c_iDefaultFontSize);
      m_pVersionLbl->TextAlignment(L"MiddleLeft");
      m_pVersionLbl->Size(m_pVersionLbl->Text2D()->Width()+4,m_pVersionLbl->Text2D()->Height()+2);
      m_pVersionLbl->Position(Width()-m_pVersionLbl->Width(),Height()-m_pVersionLbl->Height());
   }

   m_pObjInnerFrm = (GUI::GFrame*)Child(L"frmInner");
   
   m_pObjSingleBtn   = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnSingle");
   m_pObjMultiBtn    = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnMulti");
   m_pObjStopBtn     = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnStop");
   m_pObjOptionsBtn  = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnOptions");
   m_pObjCreditsBtn  = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnCredits");
   m_pObjQuitBtn     = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnQuit");
   m_pObjSep         = (GUI::GPictureBox*)m_pObjInnerFrm->Child(L"picSep");
   m_pObjColorGradientPic = (GUI::GPictureBox*)Child(L"picWhiteGradient");

   ((GFX::GWindow*)m_pObjSep->Model()->Get_Primitive())->BackgroundType(GFX::EBgndType::Mosaic);

	return true;
}


//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GMainMenu::OnShow()
{
   __super::OnShow();

   // Show Title 
   if(m_pTitleLogo == NULL)
   {
      m_pTitleLogo = new GSprite;
      Parent()->Model()->Add_Child(m_pTitleLogo->Model());
      m_pTitleLogo->LoadTexture("Data\\Textures\\Splashscreen\\logo.tga");
      CGFX_Renderer_Init_Mode l_Mode;
      g_Joshua.Renderer()->Get_Current_Mode(&l_Mode);
      REAL32 l_fSize = (REAL32) l_Mode.m_Resolution.x;
      m_pTitleLogo->SetPosition((l_fSize- m_pTitleLogo->Width()) /2.0f,40);

      // Build Fade out logo track
      m_pLogoFadeOutTrack = g_Joshua.Renderer()->Create_Track();
      CGFX_Action_Itf* l_pAction                = g_Joshua.Renderer()->Create_Action();
      CGFX_Animation_Linear_Itf *l_pAnimation   = g_Joshua.Renderer()->Create_Animation_Linear();
      CGFX_Key_Linear   l_key;
      l_key.m_Time = 0 ;
      l_key.m_Value = 1 ;
      l_pAnimation->Add_Key(l_key);
   
      l_key.m_Time = 1 ;
      l_key.m_Value = 0 ;
      l_pAnimation->Add_Key(l_key);

      l_pAnimation->Setup();

      // Create Animation Multiple and connects all the parameters
      l_pAnimation->Connect(m_pTitleLogo->m_pMaterial->PixelShader(0),GFX::PARAM_COLOR_FACTOR_A);
   
      // Add this animation to this action
      l_pAction->Add_Animation(l_pAnimation);

      // Add this action to the track
      l_pAction->Length(2.0f);
      m_pLogoFadeOutTrack->Set_Action(l_pAction);

      SAFE_RELEASE(l_pAnimation);
      SAFE_RELEASE(l_pAction);
   }
   else
   {
      m_pTitleLogo->Model()->Visibility() = true;
   }

   Parent()->Model()->Bring_To_Front(m_pTitleLogo->Model() );

   // Hide wait label if its displayed.
   if(g_ClientDDL.GameLobby())
      g_ClientDDL.GameLobby()->HideWaitLabel();

   // Play something
   g_SP2Client->AdaptiveMusicPlayer()->ThemeManager()->RequestTheme(AdaptiveMusic::ESP2MusicalTheme::Main);

   if (g_SP2Client->CurrentFSMState() != FSM::EStates::Disconnected)
   {
      switch(g_SP2Client->CurrentGameType() )
      {
      case EGameTypes::SinglePlayer:
      case EGameTypes::Scenario:
         m_pObjSingleBtn->Text( g_ClientDAL.GetString(102219) ); // Return to game
         m_pObjMultiBtn->Show();
         m_pObjMultiBtn->Text( g_ClientDAL.GetString(102220) ); // Restart game
         m_pObjStopBtn->Show();
         m_pObjStopBtn->Text( g_ClientDAL.GetString(101599) ); // Stop game

         m_pObjStopBtn->Top(66);
         m_pObjSep->Top(94);
         m_pObjOptionsBtn->Top(106);
         m_pObjCreditsBtn->Top(133);
         m_pObjQuitBtn->Top(160);

         Height(223);
         m_pObjGradientPic->Height( Height() - m_pObjGradientPic->Top() - 4 );
         m_pObjColorGradientPic->Height( Height() - m_pObjColorGradientPic->Top() - 4 );
         m_pObjInnerFrm->Height( Height() - m_pObjInnerFrm->Top() - 4 );
         m_pVersionLbl->Top( Height() - m_pVersionLbl->Height() - 2 );
         break;
      case EGameTypes::Tutorial:
         m_pObjSingleBtn->Text( g_ClientDAL.GetString(102221) ); // Return to tut
         m_pObjMultiBtn->Hide();
         m_pObjStopBtn->Show();
         m_pObjStopBtn->Text( g_ClientDAL.GetString(102222) ); // Stop tut
         
         m_pObjStopBtn->Top(39);
         m_pObjSep->Top(67);
         m_pObjOptionsBtn->Top(79);
         m_pObjCreditsBtn->Top(106);
         m_pObjQuitBtn->Top(133);

         Height(196);
         m_pObjGradientPic->Height( Height() - m_pObjGradientPic->Top() - 4 );
         m_pObjColorGradientPic->Height( Height() - m_pObjColorGradientPic->Top() - 4 );
         m_pObjInnerFrm->Height( Height() - m_pObjInnerFrm->Top() - 4 );
         m_pVersionLbl->Top( Height() - m_pVersionLbl->Height() - 2 );
         break;
      case EGameTypes::HostMultiPlayer:
      case EGameTypes::JoinMultiPlayer:
         m_pObjSingleBtn->Text( g_ClientDAL.GetString(102219) ); // Return to game
         m_pObjMultiBtn->Hide();
         m_pObjStopBtn->Show();
         m_pObjStopBtn->Text( g_ClientDAL.GetString(101598) ); // Disconnect

         m_pObjStopBtn->Top(39);
         m_pObjSep->Top(67);
         m_pObjOptionsBtn->Top(79);
         m_pObjCreditsBtn->Top(106);
         m_pObjQuitBtn->Top(133);

         Height(196);
         m_pObjGradientPic->Height( Height() - m_pObjGradientPic->Top() - 4 );
         m_pObjColorGradientPic->Height( Height() - m_pObjColorGradientPic->Top() - 4 );
         m_pObjInnerFrm->Height( Height() - m_pObjInnerFrm->Top() - 4 );
         m_pVersionLbl->Top( Height() - m_pVersionLbl->Height() - 2 );
         break;
      default:
         gassert(0, "Invalid game type");
         break;
      }
   }
   else
   {
      m_pObjSingleBtn->Text( g_ClientDAL.GetString(100004) ); 

      m_pObjMultiBtn->Show();
      m_pObjMultiBtn->Text( g_ClientDAL.GetString(100005) );

      m_pObjStopBtn->Hide();

      m_pObjSep->Top(67);
      m_pObjOptionsBtn->Top(79);
      m_pObjCreditsBtn->Top(106);
      m_pObjQuitBtn->Top(133);

      Height(196);
      m_pObjGradientPic->Height( Height() - m_pObjGradientPic->Top() - 4 );
      m_pObjColorGradientPic->Height( Height() - m_pObjColorGradientPic->Top() - 4 );
      m_pObjInnerFrm->Height( Height() - m_pObjInnerFrm->Top() - 4 );
      
      m_pVersionLbl->Top( Height() - m_pVersionLbl->Height() - 2 );
   }

   BringToFront(); 

   // Me too I want to be brought to front.
   if(g_ClientDDL.GenericMessageDesktopWindow())
      g_ClientDDL.GenericMessageDesktopWindow()->BringToFront();
}

void GMainMenu::OnHide()
{
   __super::OnHide();
   if(m_pTitleLogo != NULL)
   {
      m_pTitleLogo->Model()->Visibility() = false;
   }
}

bool GMainMenu::OnDestroy()
{
   g_Joshua.GUIManager()->ReleaseObject(m_pVersionLbl);
   m_pVersionLbl = NULL;
   return __super::OnDestroy();
}