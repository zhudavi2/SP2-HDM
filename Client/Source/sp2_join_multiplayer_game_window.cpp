/**************************************************************
*
* sp2_join_multiplayer_game_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_join_multiplayer_game_window.h"
#include "../include/sp2_server_info_window.h"
#include "../include/sp2_generic_input_window.h"
#include "../include/sp2_generic_question_window.h"
#include "../include/sp2_generic_message_window.h"
#include "../include/sp2_load_game_window.h"

const GString GJoinMPGameWindow::TypeName(L"Join_Multiplayer_Game");

bool operator < (const sockaddr & lsh, const sockaddr & rsh)
{
   for (int i = 0; i < sizeof(sockaddr); i += sizeof(int))
   {
      int diff = *((int*)((char*)&lsh + i)) - *((int*)((char*)&rsh + i));      
      if (diff < 0) return true;
      if (diff > 0) return false;
   }
   return false;
}

size_t stdext::hash_value(const sockaddr & _Keyval)
{	
   int sum = 0;
   for (int i = 0; i < sizeof(sockaddr); i += sizeof(int))
      sum += *((int*)((char*)&_Keyval + i));

   return ((size_t)sum ^ _HASH_SEED);
}

/*!
 * MAIN MENU construction function
 **/
GUI::GBaseObject* GJoinMPGameWindow::New()
{
	return (GUI::GBaseObject*) new GJoinMPGameWindow();
}


GJoinMPGameWindow::GJoinMPGameWindow() : SP2::GBaseWindow()
{
   m_iReceivingThreadId = 0;
   m_iSocketId = 0;
   m_ReadyToRefresh = false;
   m_iNetLastSelectedId = -1;
   m_iLanLastSelectedId = -1;
}

GJoinMPGameWindow::~GJoinMPGameWindow()
{
}

//! called when the button is released into our menu
GUI::EEventStatus::Enum GJoinMPGameWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         //  Cancel button
         //-----------------
         if(in_pCaller == m_pObjCancelBtn)
         {
            g_Joshua.Client()->RGAPIDisconnect();
            g_ClientDCL.CancelJoinMPGame();
            return GUI::EEventStatus::Handled;
         }
         //  Update button
         //-----------------
         else if(in_pCaller == m_pObjUpdateBtn)
         {
            //Tab 1 = Internet, Tab 2 = Lan (enum 1 = internet, 2 = lan)
            Update((EConnectionType::Enum)m_pObjTabGames->ActiveTab());
         }
         //  Join Game button
         //-----------------
         else if(in_pCaller == m_pObjJoinBtn)
         {
            JoinSelectedServer();
            return GUI::EEventStatus::Handled;
         }
         //  Internet Game Tab
         //-----------------------      
         else if(in_pCaller == m_pObjTabGames->m_vTabButtons[EConnectionType::Internet-1])
         {
            m_ListsMutex.Lock();
            Update(EConnectionType::Internet);
            m_ListsMutex.Unlock();
            return EEventStatus::Handled;
         }
         //  Lan Game Tab
         //-----------------------      
         else if(in_pCaller == m_pObjTabGames->m_vTabButtons[EConnectionType::Lan-1])
         {
            m_ListsMutex.Lock();
            Update(EConnectionType::Lan);
            m_ListsMutex.Unlock();
            return EEventStatus::Handled;
         }
         else if(in_pCaller == m_pObjServerInfoBtn)
         {
            if(!g_ClientDDL.ServerInfoWindow())
               g_ClientDDL.ServerInfoWindowSpawn();
            
            GListedServer l_sServer = GetSelectedServer();
            
            g_ClientDDL.ServerInfoWindow()->Update(l_sServer);
            g_ClientDDL.ServerInfoWindow()->Show(this);
         }
         else if(in_pCaller == m_pObjHostBtn)
         {
            g_ClientDCL.StartMultiplayerGame();
         }
         else if(in_pCaller == m_pObjLoadBtn)
         {
            if(!g_ClientDDL.LoadGameWindow())
               g_ClientDDL.LoadGameWindowSpawn();

            g_ClientDDL.LoadGameWindow()->Show(false);
         }
      }
   }//End if in_pCaller

   return __super::OnMouseDown(in_EventData, in_pCaller);
}

void GJoinMPGameWindow::Show()
{
   GDZLOG(EDZLogLevel::Entry, L"");

   __super::Show();

   if(g_ClientDDL.m_bLAN)
   {
      GEventData l_EventData;
      l_EventData.Mouse.Actor.Bits.Left = true;
      l_EventData.Mouse.Position = g_Joshua.GUIManager()->MousePosition();
      m_pObjTabGames->OnMouseDown(l_EventData, m_pObjTabGames->m_vTabButtons[1]);

      m_pObjTabGames->m_vTabButtons[1]->ToggleState(true);
      //Show the active tab
      m_pObjTabGames->m_vTabPage[1]->Show();
      m_pObjTabGames->m_vTabPage[1]->BringToFront();

      m_pObjTabGames->m_vTabButtons[0]->Hide(); //Internet
      m_pObjTabGames->m_vTabButtons[1]->Show(); //LAN      
      m_pObjLoginAs->Hide();
	  m_pObjUpdateBtn->Show();

      m_ListsMutex.Lock();
      Update(EConnectionType::Lan);
      m_ListsMutex.Unlock();
   }
   else
   {
      GEventData l_EventData;
      l_EventData.Mouse.Actor.Bits.Left = true;
      l_EventData.Mouse.Position = g_Joshua.GUIManager()->MousePosition();
      m_pObjTabGames->OnMouseDown(l_EventData, m_pObjTabGames->m_vTabButtons[0]);

      m_pObjTabGames->m_vTabButtons[0]->ToggleState(true);
      //Show the active tab
      m_pObjTabGames->m_vTabPage[0]->Show();
      m_pObjTabGames->m_vTabPage[0]->BringToFront();

      m_pObjTabGames->m_vTabButtons[0]->Show(); //Internet
      m_pObjTabGames->m_vTabButtons[1]->Hide(); //LAN      
      m_pObjLoginAs->Show();
	  m_pObjUpdateBtn->Hide();

      m_ListsMutex.Lock();
      Update(EConnectionType::Internet);
      m_ListsMutex.Unlock();
   }

   GDZLOG(EDZLogLevel::Exit, L"");
}

bool GJoinMPGameWindow::OnCreate()
{
   //Start receiving thread to receive games from server that you request
   SListMultiplayerGamesArgs* l_Args = new SListMultiplayerGamesArgs;
   l_Args->m_iGameType = 1;
   l_Args->m_pWindow = this;

	__super::OnCreate();

   m_pObjInnerFrm  = (GFrame*)Child(L"frmInner");
   m_pObjTabGames  = (GTabbed*)m_pObjInnerFrm->Child(L"tabGames");   

   m_pObjTabGames->m_vTabPage[0]->PictureNormal(L"000647");
   m_pObjTabGames->m_vTabPage[0]->Borders( GBorders(1,1,1,1) );
   m_pObjTabGames->m_vTabPage[0]->TooltipText( g_ClientDAL.GetString(102226) );
   m_pObjTabGames->m_vTabButtons[0]->TooltipText( g_ClientDAL.GetString(102226) );
   m_pObjNetLst = (GUI::GListBox*)m_pObjTabGames->m_vTabPage[0]->Child(L"lstServer");

   m_pObjTabGames->m_vTabPage[1]->PictureNormal(L"000647");
   m_pObjTabGames->m_vTabPage[1]->Borders( GBorders(1,1,1,1) );
   m_pObjTabGames->m_vTabPage[1]->TooltipText( g_ClientDAL.GetString(102227) );
   m_pObjTabGames->m_vTabButtons[1]->TooltipText( g_ClientDAL.GetString(102227) );
   m_pObjLanLst = (GUI::GListBox*)m_pObjTabGames->m_vTabPage[1]->Child(L"lstServer");

   m_pObjGameSpy = (GUI::GPictureBox*)Child(L"picGameSpy");
   //m_pObjGameSpy->BringToFront();
   m_pObjGameSpy->Hide(); //GameSpy is dead, hide the image

   m_pObjServerInfoBtn  = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnServerInfo");
   m_pObjJoinBtn        = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnJoinGame");
   m_pObjCancelBtn      = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnCancel");
   m_pObjUpdateBtn      = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnUpdate");

   //Hide unused buttons
   m_pObjLockTog->Hide();
   m_pObjApplyBtn->Hide();
   m_pObjExitBtn->Hide();

   m_pObjServerInfoBtn->Enabled(false);
   m_pObjJoinBtn->Enabled(false);
   
   //Add host and load btn
   int h = 38;
   int hBtn = 22;
   
   m_pObjHostBtn = (GUI::GButton*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"btnNormal", L"btnHostInJoinMenu", m_pObjInnerFrm, true, true);
   m_pObjHostBtn->Top(m_pObjJoinBtn->Top());
   m_pObjHostBtn->Left(m_pObjJoinBtn->Left() - 40);
   m_pObjHostBtn->Width(m_pObjJoinBtn->Width() + 40);
   m_pObjHostBtn->Height(m_pObjJoinBtn->Height());
   m_pObjHostBtn->Text(g_ClientDAL.GetString(100125));

   m_pObjLoadBtn = (GUI::GButton*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"btnNormal", L"btnLoadInJoinMenu", m_pObjInnerFrm, true, true);   
   m_pObjLoadBtn->Top(m_pObjCancelBtn->Top());
   m_pObjLoadBtn->Left(m_pObjCancelBtn->Left());
   m_pObjLoadBtn->Width(m_pObjCancelBtn->Width());
   m_pObjLoadBtn->Height(m_pObjCancelBtn->Height());
   m_pObjLoadBtn->Text(g_ClientDAL.GetString(101653));

   Height(Height() + h);
   m_pObjInnerFrm->Height(m_pObjInnerFrm->Height() + h);
   GUI::GBaseObject* picWhite  = (GUI::GBaseObject*)Child(L"picWhiteBar");
   picWhite->Height(picWhite->Height() + h);
   m_pObjGradientPic->Height(m_pObjGradientPic->Height() + h);

   m_pObjServerInfoBtn->Top(m_pObjServerInfoBtn->Top() + hBtn);
   m_pObjJoinBtn->Top(m_pObjJoinBtn->Top() + hBtn);
   m_pObjJoinBtn->Left(m_pObjJoinBtn->Left() - 40);
   m_pObjJoinBtn->Width(m_pObjJoinBtn->Width() + 40);   
   m_pObjCancelBtn->Top(m_pObjCancelBtn->Top() + hBtn);
   m_pObjUpdateBtn->Top(m_pObjUpdateBtn->Top() + hBtn);
  
   m_pObjLoginAs = (GUI::GLabel*)g_Joshua.GUIManager()->GetNewGUIObjectInternal(L"lblNormal", L"lblLoginAs", m_pObjInnerFrm, true, true);
   m_pObjLoginAs->Width(400);
   m_pObjLoginAs->Top(m_pObjInnerFrm->Height() - 18);
   m_pObjLoginAs->Left(m_pObjInnerFrm->Width() - 408);
   m_pObjLoginAs->Text(g_ClientDAL.GetString(102372) + ": " + g_Joshua.Client()->PlayerName()) ; //Multiplayer login
   m_pObjLoginAs->TextAlignment("MiddleRight", true);

   GetRGAPIServerListAsync();
	return true;
}

bool GJoinMPGameWindow::OnDestroy()
{
   //If we are still waiting for games close the socket then the thread
   if(m_iSocketId > 0)
   {
      closesocket(m_iSocketId);
      m_iSocketId = 0;
   }

   if(m_iReceivingThreadId > 0)
   {
      Hector::Util::StopThread(m_iReceivingThreadId);
      m_iReceivingThreadId = 0;
   }

   if(g_ClientDDL.GetLoginRGAPIWindow())
   {
      g_ClientDDL.LoginRGAPIWindowKill();   
   }

   RegisteredModalWindow.clear();

   __super::OnDestroy();

   return true;
}


hash_map<sockaddr, GListedServer> * GJoinMPGameWindow::s_ServerList;
ESortField::Enum GJoinMPGameWindow::s_SortServersBy = ESortField::ServerName;
bool GJoinMPGameWindow::s_SortInverse = false;
bool GJoinMPGameWindow::ServerBinaryPredicate(const sockaddr & lsh, const sockaddr & rsh)
{
   switch (s_SortServersBy)
   {
   case ESortField::ServerName:
      return s_SortInverse ^ (s_ServerList->operator [](lsh).m_sServerName.compare(s_ServerList->operator [](rsh).m_sServerName) < 0);
   case ESortField::PlayersCount:
      return s_SortInverse ^ (s_ServerList->operator [](lsh).m_iNbPlayers < s_ServerList->operator [](rsh).m_iNbPlayers);
   }
   return false;
}

void GJoinMPGameWindow::SortServersBy(ESortField::Enum l_SortBy, bool l_Inverse)
{
   s_SortServersBy = l_SortBy;
   s_SortInverse = l_Inverse;
   Refresh();
}

void GJoinMPGameWindow::Refresh()
{
   INT16 l_iActiveTab = m_pObjTabGames->ActiveTab();   
   vector<sockaddr> * l_ShownServerList = NULL;
   GUI::GListBox* l_pLstGames = (GUI::GListBox*)((GUI::GTabPage*)m_pObjTabGames->m_vTabPage[l_iActiveTab - 1])->Child(L"lstServer");

   switch (m_pObjTabGames->ActiveTab())
   {
   case EConnectionType::Internet:
      s_ServerList      = &m_InternetServerList;
      l_ShownServerList = &m_ShownInternetServerList;
      break;
   case EConnectionType::Lan:
      s_ServerList      = &m_LanServerList;
      l_ShownServerList = &m_ShownLanServerList;
      break;
   //case EConnectionType::Favorites:
   //   s_ServerList = &m_FavoriteServerList;
   //   l_ShownServerList = &m_ShownFavoriteServerList;
   //   break;
   default:
      gassert(0,"JoinMPGameWindow::Refresh() unhandled switch case");
   }
   l_ShownServerList->clear();
   l_pLstGames->Clear();

   m_ListsMutex.Lock();
   for (hash_map<sockaddr, GListedServer>::iterator itr = s_ServerList->begin(); itr != s_ServerList->end(); itr++)
      l_ShownServerList->push_back(itr->first);
   m_ListsMutex.Unlock();

   sort(l_ShownServerList->begin(), l_ShownServerList->end(), ServerBinaryPredicate);

   for (vector<sockaddr>::iterator itr = l_ShownServerList->begin(); itr != l_ShownServerList->end(); itr++)
   {
      GListedServer & l_Server = s_ServerList->operator[](*itr);
      GUI::GListBox::GRow* l_pRow = l_pLstGames->Insert_New_Row();
      
      ((GLabel*)l_pRow->Object()->Child(L"txtServerName"))->Text(l_Server.m_sServerName);
      ((GLabel*)l_pRow->Object()->Child(L"txtPlayers"))->Text(GString(l_Server.m_iNbPlayers) + L"/" + GString(l_Server.m_iMaxPlayers));
      ((GLabel*)l_pRow->Object()->Child(L"txtMods"))->Text(l_Server.m_sModName);
      ((GLabel*)l_pRow->Object()->Child(L"txtScenarios"))->Text(l_Server.m_sScenarioName);
      if(l_Server.m_bOfficial)
         ((GPictureBox*)l_pRow->Object()->Child(L"picLegal"))->PictureNormal(L"000242");
      else
         ((GPictureBox*)l_pRow->Object()->Child(L"picLegal"))->PictureNormal(L"000244");
      
      ((GPictureBox*)l_pRow->Object()->Child(L"picLegal"))->PictureBackground( ((GPictureBox*)l_pRow->Object()->Child(L"picLegal"))->PictureNormal() );
      l_pRow->Object()->Child(L"picLegal")->BringToFront();
      if(l_Server.m_bHasPassword)
      {
         GPictureBox* l_Pic = CREATE_GUI_OBJECT_NAMED_STATIC(GPictureBox, L"Locked",l_pRow->Object(), true);
         l_Pic->PictureNormal(L"276");
         l_Pic->Position(171,3);
         l_Pic->Size(5,7);
         l_Pic->BringToFront();
      }
   }
   l_pLstGames->Update();
}

void GJoinMPGameWindow::SelectRow(GUI::GListBox::GRow* in_pRow)
{
   if(!in_pRow)
      return;

   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtServerName"))->PictureNormal(L"000647");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtPlayers"))->PictureNormal(L"000647");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtMods"))->PictureNormal(L"000647");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtScenarios"))->PictureNormal(L"000647");

   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtServerName"))->PictureBackground( ((GUI::GLabel*)in_pRow->Object()->Child(L"txtServerName"))->PictureNormal() );
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtPlayers"))->PictureBackground( ((GUI::GLabel*)in_pRow->Object()->Child(L"txtPlayers"))->PictureNormal() );
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtMods"))->PictureBackground( ((GUI::GLabel*)in_pRow->Object()->Child(L"txtMods"))->PictureNormal() );
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtScenarios"))->PictureBackground( ((GUI::GLabel*)in_pRow->Object()->Child(L"txtScenarios"))->PictureNormal() );

   m_pObjJoinBtn->Enabled(true);
   m_pObjServerInfoBtn->Enabled(true);
}

/*!
 * Remove our last selected row
 **/
void GJoinMPGameWindow::RemoveSelection(EConnectionType::Enum in_eConnectType)
{
   switch(in_eConnectType)
   {
   // remove the internet selection
   case EConnectionType::Internet:
   {
      if( m_iNetLastSelectedId >= 0 )
      {
         UnselectRow( m_pObjNetLst->Get_Row_At(m_iNetLastSelectedId) );
         m_iNetLastSelectedId = -1;
      }
   }
   break;
   // remove the lan selection
   case EConnectionType::Lan:
   {
      if( m_iLanLastSelectedId >= 0 )
      {
         UnselectRow( m_pObjLanLst->Get_Row_At(m_iLanLastSelectedId) );
         m_iLanLastSelectedId = -1;
      }
   }
   break;
   }
   // Disable buttons
   m_pObjJoinBtn      ->Enabled(false);
   m_pObjServerInfoBtn->Enabled(false);
}

/*!
 * Set up row background picture
 **/
void GJoinMPGameWindow::UnselectRow(GUI::GListBox::GRow* in_pRow)
{
   if(!in_pRow)
      return;

   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtServerName"))->PictureNormal(L"000587");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtPlayers"))->PictureNormal(L"000587");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtMods"))->PictureNormal(L"000587");
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtScenarios"))->PictureNormal(L"000587");

   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtServerName"))->PictureBackground( ((GUI::GLabel*)in_pRow->Object()->Child(L"txtServerName"))->PictureNormal() );
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtPlayers"))->PictureBackground( ((GUI::GLabel*)in_pRow->Object()->Child(L"txtPlayers"))->PictureNormal() );
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtMods"))->PictureBackground( ((GUI::GLabel*)in_pRow->Object()->Child(L"txtMods"))->PictureNormal() );
   ((GUI::GLabel*)in_pRow->Object()->Child(L"txtScenarios"))->PictureBackground( ((GUI::GLabel*)in_pRow->Object()->Child(L"txtScenarios"))->PictureNormal() );
}

void GJoinMPGameWindow::Update(EConnectionType::Enum in_eType)
{
   switch(in_eType)
   {
      //Internet games
      case EConnectionType::Internet:
         // ! Remove our last selection
         RemoveSelection(EConnectionType::Internet);
         m_InternetServerList.clear();
         m_ShownInternetServerList.clear();     
         ((GUI::GListBox*)m_pObjTabGames->m_vTabPage[0]->Child(L"lstServer"))->Clear();
		 GetRGAPIServerListAsync();
         break;
      //Lan games
      case EConnectionType::Lan:
         // ! Remove our last selection
         RemoveSelection(EConnectionType::Lan);
         m_LanServerList.clear();
         m_ShownLanServerList.clear();     
         ((GUI::GListBox*)m_pObjTabGames->m_vTabPage[1]->Child(L"lstServer"))->Clear();
         g_Joshua.Client()->GetLocalServerListAsync();
         break;
      //case EConnectionType::Favorites:
         //Nothing yet...
      //   break;
      default:
         gassert(0,"JoinMPGameWindow, unhandled switch case ::Update");
   }   
}

EEventStatus::Enum GJoinMPGameWindow::OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_iEventID == ECustomEvents::OnListItemSelected)
   {
      // ! Remove our last selection
      RemoveSelection(EConnectionType::Internet);
      RemoveSelection(EConnectionType::Lan);
      // internet tab selected
      if( m_pObjTabGames->ActiveTab() == EConnectionType::Internet )
      {       
         GUI::GListBox::GRow* l_pRow = m_pObjNetLst->Get_Row_At( m_pObjNetLst->Get_Selected_Item()[0] );
         SelectRow(l_pRow);
         m_iNetLastSelectedId = m_pObjNetLst->Get_Selected_Item()[0];
      }
      // lan tab selected
      else if( m_pObjTabGames->ActiveTab() == EConnectionType::Lan )
      {                     
         GUI::GListBox::GRow* l_pRow = m_pObjLanLst->Get_Row_At( m_pObjLanLst->Get_Selected_Item()[0] );
         SelectRow(l_pRow);
         m_iLanLastSelectedId = m_pObjLanLst->Get_Selected_Item()[0];
      }
      //else
      //{
      //   if(m_iFavLastSelectedId >= 0)
      //      UnselectRow( m_pObjFavLst->Get_Row_At( m_iFavLastSelectedId ) );

      //   GUI::GListBox::GRow* l_pRow = m_pObjFavLst->Get_Row_At( m_pObjFavLst->Get_Selected_Item()[0] );
      //   SelectRow(l_pRow);
      //   m_iFavLastSelectedId = m_pObjFavLst->Get_Selected_Item()[0];
      //}
   }
   else if(in_iEventID == c_OnGenericInputWindowCancel)
   {

   }
   else if(in_iEventID == c_OnGenericInputWindowOk)
   {
      GString l_sPassword = *(GString*)in_EventData.Data;
      JoinSelectedServer(l_sPassword, true);
   }
   else if(in_iEventID == c_OnServerInfoWindowJoin)
   {
      JoinSelectedServer();
   }

   return GUI::EEventStatus::Handled;
}

/*!
 * Connects to the actually selected server in the list
 **/
bool GJoinMPGameWindow::JoinSelectedServer(GString in_sPassword, bool in_bConfirmed)
{
   GDZLOG(EDZLogLevel::Entry, L"in_sPassword = " + GString(in_sPassword) + L", in_bConfirmed = " + GString(in_bConfirmed));

   GListedServer l_sServer = GetSelectedServer();
   
   sockaddr_in* l_AdressPtr = (sockaddr_in*)&l_sServer.m_ServerAdress;
   GString l_sServerAdress = GString(inet_ntoa(l_AdressPtr->sin_addr));

   g_SP2Client->SetUseHdmEventsWithModName(l_sServer.m_sModName);

   if(l_sServer.m_bOfficial || in_bConfirmed)
   {   
      //If the server is public connect to it else popup the password window
      if(!l_sServer.m_bHasPassword)
      {
         g_ClientDCL.Connect(l_sServerAdress,l_sServer.m_iServerPort,L"",l_sServer.m_ips);
      }
      else
      {
         if(in_sPassword == L"")
         {
            if(!g_ClientDDL.GenericInputWindow())
               g_ClientDDL.GenericInputWindowSpawn();

            //String password
            g_ClientDDL.GenericInputWindow()->Show( g_ClientDAL.GetString(100127), this );
         }
         else
         {
            g_ClientDCL.Connect(l_sServerAdress,l_sServer.m_iServerPort,in_sPassword);
         }
      }
   }
   else
   {
      SP2::GGenericQuestion l_Question;
      l_Question.m_pParentWindow       = this;
      l_Question.m_pOberserToNotify    = this;
      l_Question.m_sMessage            = g_ClientDAL.GetString(102316);
      l_Question.m_sOkCaption          = g_ClientDAL.GetString(100348);
      l_Question.m_sCancelCaption      = g_ClientDAL.GetString(100349);
      g_ClientDDL.GenericQuestion(l_Question);
   }

   GDZLOG(EDZLogLevel::Exit, L"Returning true");
   return true;
}

void GJoinMPGameWindow::OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   if(in_Notification != c_OnGenericQuestionWindowAnswerCancel)
   {
      JoinSelectedServer(L"", true);
   }
}

GListedServer GJoinMPGameWindow::GetSelectedServer()
{
   GListedServer l_sServer;

   //Check active tab and get the selected server of that tab
   INT32 l_iActiveTab = m_pObjTabGames->ActiveTab();
   const vector<INT32>& l_viSelected = ((GUI::GListBox*)m_pObjTabGames->m_vTabPage[l_iActiveTab - 1]->Child(L"lstServer"))->Get_Selected_Item();
   
   //If no server was selected simply do nothing
   if(l_viSelected.size() <= 0)
      return l_sServer;
   
   //Retreive server adress and convert it to an usable format
   GString l_sServerAdress;
   if(l_viSelected[0] == -1)
   {
#ifdef GOLEM_DEBUG
      g_Joshua.Log("Problem in selection of the server");
#endif
      return l_sServer;
   }
   

   m_ListsMutex.Lock();
   switch ((EConnectionType::Enum)m_pObjTabGames->ActiveTab())
   {
      case EConnectionType::Internet:
         l_sServer = m_InternetServerList[m_ShownInternetServerList[l_viSelected[0]]];
         break;
      case EConnectionType::Lan:
         l_sServer = m_LanServerList[m_ShownLanServerList[l_viSelected[0]]];
         break;
      //case EConnectionType::Favorites:
       //  l_sServer = m_FavoriteServerList[m_ShownFavoriteServerList[l_viSelected[0]]];
        // break;
   }
   m_ListsMutex.Unlock();

   return l_sServer;
}

void GJoinMPGameWindow::GetRGAPIServerListAsync()
{
   if(g_Joshua.Client()->RGAPIPlayerIsLoggedIn())
   {
		g_Joshua.Client()->GetRGAPIServerListAsync();
   }
}