/**************************************************************
*
* sp2_client.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  François Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_client.h"

#include "../../includes/common_lib//sp2_event_history_markers.h"

#include "../include/sp2_background_earth.h"
#include "../include/sp2_battle_overview_window.h"
#include "../include/sp2_fsm_main.h"
#include "../include/sp2_game_lobby_window.h"
#include "../include/sp2_game_scenario_window.h"
#include "../include/sp2_generic_message_window.h"
#include "../include/sp2_join_multiplayer_game_window.h"
#include "../include/sp2_host_multiplayer_game.h"
#include "../include/sp2_main_menu.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"
#include "../include/sp2_progress_meter.h"
#include "../include/sp2_objectives_window.h"
#include "../include/sp2_save_game_window.h"
#include "../include/sp2_load_game_window.h"
#include "../include/sp2_RGAPI_login.h"

#pragma warning (disable: 4407)

/*!
* Constructor
**/
GClient::GClient() :
m_DAL(),
m_DCL(&m_DAL),
m_DDL(),
m_eCurrentGameType(EGameTypes::None),
m_pSoundBox(NULL),
m_pStateMachine(NULL),
m_pAdaptiveMusicPlayer(NULL),
m_bLoadingGame(false),
m_bCheckForEngineConnection(true),
m_bLimitFPS(false),
m_bPlayCredits(false),
m_iSavedFSMEvent(0)
{
}

/*!
* ~Destructor
**/
GClient::~GClient()
{
}

/*!
* Initialize the Client
**/
GAME_MSG GClient::Initialize()
{
   SetWindowTextA(g_Joshua.Renderer()->Get_Engine().m_HWnd, "SP2");

   GDALInterface::Instance = &m_DAL;
   GDCLInterface::Instance = &m_DCL;

   g_Joshua.GameTimeSpeedSynchroEnable(true, 1);

   // Set initial time & speed to 0, 
   // making sure history values will be accumulated correctly
   g_Joshua.GameTime(0);
   g_Joshua.GameSpeed(0);

   // Build the main state machine
   m_pStateMachine = new FSM::GMain();

   // Start the main state machine
   m_pStateMachine->Start(FSM::EStates::Init);   

   m_sModVersion = g_Joshua.CurrentMod().m_sName + L" V" + g_Joshua.CurrentMod().m_sVersion + L" (HDM V10.5)";

   return GAME_OK;
}

GString GClient::ConsoleMainCommandsHandler(const GString & in_Command, const vector<GString> & in_Params)
{
   if (in_Command == "connect")
   {
      int port = in_Params[1].ToINT32();
      g_Joshua.Log(L"Trying to connect to " + in_Params[0] + " port " + GString(port ? port : c_iServerDefaultPort) + "...");
      this->DCL().Connect(in_Params[0], port ? port : c_iServerDefaultPort, L"");
      ChangeGameType(EGameTypes::JoinMultiPlayer);
      m_pStateMachine->SendEvent(FSM::EEvents::ConnectAtStartup);
      return L"";
   }
   else if(in_Command == "version")
   {
      return m_sModVersion;      
   }
   else if(in_Command == "kick")
   {
	   if(g_ClientDDL.GameLobby())
	   {
		   g_ClientDDL.GameLobby()->KickPlayer(in_Params[0]);
	   }
	return L"";
   }
   else if(in_Command == "ban")
   {
	   if(g_ClientDDL.GameLobby())
	   {
		   g_ClientDDL.GameLobby()->BanPlayer(in_Params[0]);
	   }
	return L"";
   }
#ifdef GOLEM_DEBUG
   else if (in_Command == "show")
   {
      return CREATE_GUI_OBJECT(in_Params[0], in_Params[0], g_ClientDDL.BackgroundEarth(), true, true) ? "Window shown" : "Unable to show this window";
   }
   else if (in_Command == "realistic")
   {
      if(DDL().BackgroundEarth() )
         DDL().BackgroundEarth()->SwitchView(EBackgroundView::Realistic, in_Params[0].ToREAL32() );

      return "Realistic background mode";
   }
   else if (in_Command == "strategic")
   {
      if(DDL().BackgroundEarth() )
         DDL().BackgroundEarth()->SwitchView(EBackgroundView::Strategic, in_Params[0].ToREAL32());

      return "Strategic background mode";
   }

#endif
   else
      return L"?? Unknown Command ??";
}

/*!
* Iterate the game client
* @return GAME_MSG: GAME_OK if no error
**/
GAME_MSG GClient::Iterate(void* param)
{
   // Before first iteration, we check if a connection
   // is going on so we skip the main menu directly to the
   // connecting state
   if(m_bCheckForEngineConnection && (CurrentFSMState() == FSM::EStates::Disconnected) )
   {
      if(g_Joshua.Connected() || g_Joshua.Connecting() )
      {
         ChangeGameType(EGameTypes::JoinMultiPlayer);
         m_pStateMachine->SendEvent(FSM::EEvents::ConnectAtStartup);
      }

      m_bCheckForEngineConnection = false;
   }

   m_pStateMachine->Iterate();
   
   g_Joshua.IterateSteam();//
      

   // When server is spawning or creating a game, make sure to give it more CPU
   if(m_bLimitFPS)
   {
      Sleep(30);
   }

   // Iterate sounds
   IterateSound();

   // When the state machine stops running, it means the game has quit
   if(!m_pStateMachine->IsStarted() )
   {
      g_Joshua.Quit();
   }
   else
   {
      DDL().Iterate();
   }

   return GAME_OK;
}

void GClient::QuitGame()
{
   m_pStateMachine->SendEvent(FSM::EEvents::QuitBtn);
}

/*!
* Shutdown the game engine
* @return GAME_MSG: GAME_OK if no error
**/
GAME_MSG GClient::Shutdown()
{
   GBattleScene::ClearAnimations();

   SAFE_DELETE(m_pAdaptiveMusicPlayer);

   // Delete sound box
   SAFE_DELETE(m_pSoundBox);

   // Delete state machine
   SAFE_DELETE(m_pStateMachine);

   m_DDL.Shutdown();
   m_DAL.Shutdown();

   return GAME_OK;
}

/*!
* Overloaded virtual function called by the engine when
* the client is effectively connected to the game server
**/
void GClient::OnConnectedToServer(INT32 in_iPlayerID)
{
   m_pStateMachine->SendEvent(FSM::EEvents::Connected);
}


void GClient::DisplayConnectionError(ConnectionLostReason::Enum in_eReason)
{
   static UINT32 s_iConnectErrors[] = 
   {
      102284,
      102285,
      102286,
      102287,
      102288,
      102289,
      102310,
      102290,
      102291,
      102292,
      102293,
      102294,
      102295,
      102346, // Ban from server
      102296,
      
   };

   // If we have a reason, show it, otherwise its just a normal disconnect.
   if( (in_eReason != ConnectionLostReason::TotalReason) && (in_eReason != ConnectionLostReason::DisconnectedByUser) )
   {
      if(!g_ClientDDL.GenericMessageDesktopWindow())
         g_ClientDDL.GenericMessageDesktopWindowSpawn();

      // Make it modal to the main menu
      if(g_ClientDDL.MainMenu())
         g_ClientDDL.MainMenu()->RegisteredModalWindow.push_back(g_ClientDDL.GenericMessageDesktopWindow());

      g_ClientDDL.GenericMessageDesktopWindow()->Show(g_ClientDAL.GetString(s_iConnectErrors[in_eReason] ), g_ClientDDL.MainMenu());
   }
}


void GClient::OnConnectionError(ConnectionLostReason::Enum in_eReason)
{
   if(in_eReason != ConnectionLostReason::DisconnectedByUser)
   {
      DisplayConnectionError(in_eReason);
      m_pStateMachine->SendEvent(FSM::EEvents::CancelConnect);
   }
}

void GClient::OnChatMessage(INT32 in_FromID, const GString & Message, bool in_bPrivate)
{
   if (g_ClientDDL.GameLobby())
   {
      if (in_FromID != Id())
         g_ClientDDL.GameLobby()->AddSentMessage(in_FromID, Message,in_bPrivate);
   }   
   __super::OnChatMessage(in_FromID, Message,in_bPrivate);
}

SP2::GChat * GClient::Chat() 
{
   return g_ClientDDL.GameLobby();
}


/*!
* Function that will receive the UDP Packets
**/
void GClient::OnUDPPacketReception(GOBuffer & in_Data, const sockaddr_in & in_From)
{
   INT16 l_iPacketTypeIdentifier;
   in_Data >> l_iPacketTypeIdentifier;

   switch(l_iPacketTypeIdentifier)
   {
   case c_iCombatPacketIdentifier:
      {
         //Send the packet to the DCL Method that handles Combat Information Packets
         GCombatInformationPacket l_Packet;
         in_Data >> l_Packet;
         g_ClientDCL.HandleCombatInformationUDPPacket(l_Packet);
      }
   }   
}

void GClient::OnGlobalHotkey(const GUI::GEventData & in_EventData)
{
   switch(in_EventData.Key())
   {
   case VK_F9:
      g_ClientDCL.ConsoleDisplayToggle();
      break;
   case VK_ESCAPE:
      m_pStateMachine->SendEvent(FSM::EEvents::CancelMovie);
      break;
   case VK_F10:
      if( (CurrentFSMState() != FSM::EStates::LoadData) && (CurrentFSMState() != FSM::EStates::Connecting))
      {
         if(!g_SP2Client->DDL().MainMenu() )
            g_SP2Client->DDL().MainMenuSpawn();

         if (!g_SP2Client->DDL().MainMenu()->Visible())
            g_SP2Client->DDL().MainMenu()->Show();
         else
            g_SP2Client->DDL().MainMenu()->Hide();
      }
      break;
   case VK_F2:
      if ( (CurrentGameType() != EGameTypes::SinglePlayer) && (CurrentGameType() != EGameTypes::Scenario) && (CurrentFSMState() == FSM::EStates::Playing) && g_ClientDDL.GameLobby())
      {
         if (g_ClientDDL.GameLobby()->Visible())
         {
            g_ClientDDL.GameLobby()->Hide();
            g_Joshua.GUIManager()->Focus(g_ClientDDL.BackgroundEarth());
         }
         else
         {
            g_ClientDDL.GameLobby()->HideMiniChatWindow();
            g_ClientDDL.GameLobby()->Show(EGameLobbyType::ChatRoom);
         }
      }
      break;
   case VK_F3:
      if( ( (CurrentGameType() == EGameTypes::SinglePlayer) || (CurrentGameType() == EGameTypes::Scenario) || (CurrentGameType() == EGameTypes::HostMultiPlayer) ) && (CurrentFSMState() == FSM::EStates::Playing) )
      {
         if( !g_ClientDDL.SaveGameWindow() )
            g_ClientDDL.SaveGameWindowSpawn();

         if( !g_ClientDDL.SaveGameWindow()->Visible() )
            g_ClientDDL.SaveGameWindow()->Show();
         else
            g_ClientDDL.SaveGameWindow()->Hide();
      }
      break;
   case VK_F4:
      if( ( (CurrentGameType() == EGameTypes::SinglePlayer) || (CurrentGameType() == EGameTypes::Scenario) ) && (CurrentFSMState() == FSM::EStates::Playing) )
      {
         if( !g_ClientDDL.LoadGameWindow() )
            g_ClientDDL.LoadGameWindowSpawn();

         if( !g_ClientDDL.LoadGameWindow()->Visible() )
         {
            if( (CurrentGameType() == EGameTypes::SinglePlayer) || (CurrentGameType() == EGameTypes::Scenario) )
               g_ClientDDL.LoadGameWindow()->Show(true);
            else
               g_ClientDDL.LoadGameWindow()->Show(false);
         }
         else
            g_ClientDDL.LoadGameWindow()->Hide();
      }
      break;
   case VK_F5:
      if ( (CurrentFSMState() == FSM::EStates::Playing) )
      {
         if (g_ClientDDL.ObjectivesWindow() == NULL)
         {
            g_ClientDDL.ObjectivesWindowSpawn();
         }
         if(g_ClientDDL.ObjectivesWindow()->Visible() == false)
         {
            g_ClientDDL.ObjectivesWindow()->Show();
            g_ClientDDL.ObjectivesWindow()->UpdateSelectedScenario();
         }
         else
         {
            g_ClientDDL.ObjectivesWindow()->Hide();
         }
      }
      break;   

   }
} 

void GClient::RestartGame()
{
   LoadGame(false);
   m_pStateMachine->SendEvent(FSM::EEvents::RestartGame);
}

void GClient::StopGame()
{
   LoadGame(false);
   m_pStateMachine->SendEvent(FSM::EEvents::StopGame);
}

void GClient::OnRGAPILoginCompleted(bool bSuccess)
{
   GDZLOG(EDZLogLevel::Entry, L"bSuccess = " + GString(bSuccess));

   GLoginRGAPIWindow * l_Win = g_ClientDDL.GetLoginRGAPIWindow();
   if(l_Win)
      l_Win->OnLoginCompleted(bSuccess);

   GDZLOG(EDZLogLevel::Exit, L"");
}

void GClient::OnRGAPIRegisterCompleted(bool bSuccess)
{
	GLoginRGAPIWindow * l_Win = g_ClientDDL.GetLoginRGAPIWindow();
	if(l_Win)
		l_Win->OnRegisterCompleted(bSuccess);
}



void GClient::OnNewServerListed(bool in_bIsLocal, const GListedServer & in_Server)
{
   GJoinMPGameWindow * l_Win = g_ClientDDL.JoinMPGameWindow();
   if (l_Win)
   {
      if (in_bIsLocal) //Add Lan server for later use
      {
         l_Win->m_ListsMutex.Lock();
         l_Win->m_LanServerList.insert(make_pair(in_Server.m_ServerAdress, in_Server));
         l_Win->m_ListsMutex.Unlock();
      }
      else //Add Internet server for later use
      {
         l_Win->m_ListsMutex.Lock();
         l_Win->m_InternetServerList.insert(make_pair(in_Server.m_ServerAdress, in_Server));
         l_Win->m_ListsMutex.Unlock();
      }

      l_Win->m_ReadyToRefresh = true;;
   }
}

GSP2Sound * GClient::SoundBox()
{
   return m_pSoundBox;
}

void GClient::OnServerDisconnected(SDK::ConnectionLostReason::Enum in_eReason)
{
   if(in_eReason != ConnectionLostReason::DisconnectedByUser)
   {
      DisplayConnectionError(in_eReason);
      m_pStateMachine->SendEvent(FSM::EEvents::Disconnection);
   }
}

/*!
* Plays the given sound if the soundbox exists, else ignores the event
* @param in_eSoundEventID: Sound ID
**/
void GClient::PlaySound(SP2::ESoundEventID::Enum in_eSoundEventID)
{
   if(m_pSoundBox)
      m_pSoundBox->PlaySound(in_eSoundEventID);
}

void GClient::PlaySoundWithDelay(SP2::ESoundEventID::Enum in_eSoundEventID,REAL32 in_fDelay, SP2::ESoundContext::Enum in_eContext)
{
   if(m_pSoundBox)
      m_pSoundBox->PlaySoundWithDelay(in_eSoundEventID,in_fDelay,in_eContext);
}

bool GClient::IsSoundPlaying(SP2::ESoundEventID::Enum in_eSoundEventID, SP2::ESoundContext::Enum in_eContext)
{
   if(m_pSoundBox)
   {

      CSFX_Sound_Itf* l_pSound = m_pSoundBox->GetSound(in_eSoundEventID);
      return m_pSoundBox->SoundContext(in_eContext)->IsSoundPlaying(l_pSound);
   }

   return false;
}

void GClient::PlaySoundWithGametimeDelay(SP2::ESoundEventID::Enum in_eSoundEventID,REAL64 in_fTimeToPlay)
{
   // Queue sound...
   m_DelayedSound.insert( make_pair<REAL64,SP2::ESoundEventID::Enum>(in_fTimeToPlay,in_eSoundEventID));
}

void GClient::IterateSound(void)
{
   multimap<REAL64,SP2::ESoundEventID::Enum>::iterator l_soundIterator = m_DelayedSound.begin();
   while(l_soundIterator != m_DelayedSound.end())
   {
      if(l_soundIterator->first < g_Joshua.GameTime())
      {
         // Play this sound
         PlaySound(l_soundIterator->second);
      }
      else
      {
         break;
      }
      l_soundIterator++;
   }

   // Removed played sound
   if(l_soundIterator != m_DelayedSound.begin())
      m_DelayedSound.erase(m_DelayedSound.begin(),l_soundIterator);

}




UINT32 GClient::CurrentFSMState()
{
   return m_pStateMachine->CurrentState();
}

EGameTypes::Enum GClient::CurrentGameType()
{
   return m_eCurrentGameType;
}

void GClient::RegisterElements()
{
   REGISTER_GAME_EVENT(SP2::Event::GNotifyClient,              &SP2::GGeneralEventHandler::HandleNotifyClient,              &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GGetRegionsCharacteristicResult, &SP2::GGeneralEventHandler::HandleGetRegionsCharacteristicResult,  &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GStartGame,                 &SP2::GGeneralEventHandler::HandleStartGame,                 &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GStrategicWarfareNotify,    &SP2::GGeneralEventHandler::HandleStrategicWarfareNotify,    &m_EventHandler);
	REGISTER_GAME_EVENT(SP2::Event::GRandomEvent,		         &SP2::GGeneralEventHandler::HandleRandomEvents,					&m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GSendData,                  &SP2::GGeneralEventHandler::HandleSendData,                  &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GReceiveCountryList,        &SP2::GGeneralEventHandler::HandleReceiveCountryList,        &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GReceiveCountryStaticData,  &SP2::GGeneralEventHandler::HandleReceiveCountryStaticData,  &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GErrorCountryStaticData,    &SP2::GGeneralEventHandler::HandleErrorCountryStaticData,    &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GUpdateData,                &SP2::GGeneralEventHandler::HandleUpdateData,                &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GUpdateTreaty,              &SP2::GGeneralEventHandler::HandleUpdateTreaty,              &m_EventHandler);
	REGISTER_GAME_EVENT(SP2::Event::GUpdateWar,						&SP2::GGeneralEventHandler::HandleUpdateWar,						&m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GUpdateUnitValue,           &SP2::GGeneralEventHandler::HandleUpdateUnitValue,           &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GSendAvailableCountries,    &SP2::GGeneralEventHandler::HandleReceiveAvailableCountries, &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GConquerCountry,            &SP2::GGeneralEventHandler::HandleConquerCountry,            &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GGetPlayersList,            &SP2::GGeneralEventHandler::HandleGetPlayersList,            &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GGetCountryRanks,           &SP2::GGeneralEventHandler::HandleCountryRanksResult,        &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GGetCountriesRanks,         &SP2::GGeneralEventHandler::HandleCountriesRanksResult,      &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GSendNews,						&SP2::GGeneralEventHandler::HandleNews,								&m_EventHandler);
	REGISTER_GAME_EVENT(SP2::Event::GSendPlan,						&SP2::GGeneralEventHandler::HandleAdvisorPlan,               &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GSynchronizeCountryData,    &SP2::GGeneralEventHandler::HandleSynchronizeCountryData,    &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GGameOver,                  &SP2::GGeneralEventHandler::HandleGameOver,                  &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GObjectiveCompleted,        &SP2::GGeneralEventHandler::HandleObjectiveCompleted,        &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GRelationHistoryFetch,      &SP2::GGeneralEventHandler::HandleRelationHistoryReception,  &m_EventHandler);
	REGISTER_GAME_EVENT(SP2::Event::GEventFetchCountriesRelations, &SP2::GGeneralEventHandler::HandleRelationsCountries,     &m_EventHandler);



   REGISTER_GAME_EVENT(SP2::Event::GChangeGameSpeed,           NULL, NULL);
	REGISTER_GAME_EVENT(SP2::Event::GChangeAIAggressiveness,    NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GSetGameObjectives,         &SP2::GGeneralEventHandler::HandleSetGameObjectives,  &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GStartGameRequest,          NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GSetPlayerInfo,             NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GGetRegionsCharacteristic,  NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GEconomicHealthUpdate,      &SP2::GEconomicEventHandler::HandleEconomicHealthWindowUpdate, &m_EconomicEventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GResourcesUpdate,           &SP2::GEconomicEventHandler::HandleResourcesWindowUpdate, &m_EconomicEventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GBudgetUpdate,              NULL, NULL);
	REGISTER_GAME_EVENT(SP2::Event::GEventUpdateWarState,			NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GPerformOffshoreBombardment,NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GRequestCountryList,        NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GRequestCountryStaticData,  NULL, NULL);


   REGISTER_GAME_EVENT(SP2::Event::GStrategicWarfareLaunch,    NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GRequestTroopsMove,         NULL, NULL);
	REGISTER_GAME_EVENT(SP2::Event::GOneClickInvade,	         NULL, NULL);

   REGISTER_GAME_EVENT(SP2::Event::GTreatyOffer,					&SP2::GTreatyEventHandler::HandleTreatyOffer, &m_TreatyEventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GTreatyJoin,						NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GTreatyLeave,					NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GTreatyPeaceSigned,			NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GTreatyDetailsTreatyAdd,    NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GRequestTreatyConditionsCountries, &SP2::GTreatyEventHandler::HandleConditionsCountries, &m_TreatyEventHandler);

   REGISTER_GAME_EVENT(SP2::Event::GConstitutionalFormUpdate,  &SP2::GPoliticEventHandler::HandleConstitutionalFormInfoRequest, &m_PoliticEventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GInternalLawsUpdate,        &SP2::GPoliticEventHandler::HandleInternalLawsUpdate, &m_PoliticEventHandler);
	REGISTER_GAME_EVENT(SP2::Event::GEventTreatyRefusalDetails, &SP2::GPoliticEventHandler::HandleTreatyRefusalDetails, &m_PoliticEventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GUnitResearchUpdate,        &SP2::GMilitaryEventHandler::HandleUnitResearchUpdateRequest, &m_MilitaryEventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GRefuseMilitaryAction,      &SP2::GMilitaryEventHandler::HandleRefuseMilitaryAction,      &m_MilitaryEventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GCombatOver,                &SP2::GMilitaryEventHandler::HandleCombatOver,                &m_MilitaryEventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GBombardmentNotification,   &SP2::GMilitaryEventHandler::HandleBombardmentNotification,   &m_MilitaryEventHandler);


   REGISTER_GAME_EVENT(SP2::Event::GRequestMilitaryStatusChange,NULL,NULL);
   REGISTER_GAME_EVENT(SP2::Event::GAddUnitDesign,              NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GBattleOverviewUnitActionUpdate, NULL, NULL);

   REGISTER_GAME_EVENT(SP2::Event::GHdmEventCountryInfo, &SP2::GGeneralEventHandler::HandleCountryInfo, &m_EventHandler);

   // Unit Production Event
   REGISTER_GAME_EVENT(SP2::Event::GBuildUnitOrder,NULL,NULL);
   REGISTER_GAME_EVENT(SP2::Event::GChangeUnitOrder,NULL,NULL);
   REGISTER_GAME_EVENT(SP2::Event::GRequestProductionQueue,NULL,NULL);
   REGISTER_GAME_EVENT(SP2::Event::GDeployUnits,NULL,NULL);
   REGISTER_GAME_EVENT(SP2::Event::GSplitUnits,NULL,NULL);
   REGISTER_GAME_EVENT(SP2::Event::GCancelDeployement,NULL,NULL);
   REGISTER_GAME_EVENT(SP2::Event::GUpdateUnitOrderStatus,&SP2::GUnitProductionEventHandler::HandleUnitProductionQueueUpdate,&m_UnitProductionEventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GRequestForeignBuildingCountryList,NULL,NULL);
   REGISTER_GAME_EVENT(SP2::Event::GUpdateForeignBuildingCountryList,&SP2::GUnitProductionEventHandler::HandleUpdateForeignBuildingCountryList,&m_UnitProductionEventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GRegisterAsBuildingCountry, NULL,NULL);

   // Unit Training Event
   REGISTER_GAME_EVENT(SP2::Event::GTrainUnitRequest,NULL,NULL);
   REGISTER_GAME_EVENT(SP2::Event::GDisbandUnitRequest,NULL,NULL);

   // Unit Market Event
   REGISTER_GAME_EVENT(SP2::Event::GRequestForSaleUnitList,NULL,NULL);
   REGISTER_GAME_EVENT(SP2::Event::GUpdateForSaleUnitList,&SP2::GUnitProductionEventHandler::HandleUpdateForSaleUnitList,&m_UnitProductionEventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GTransfertUnit,NULL,NULL);


   REGISTER_GAME_EVENT(SP2::Event::GPoliticUpdate,             &SP2::GPoliticEventHandler::HandlePoliticWindowUpdate, &m_PoliticEventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GEconomicUpdate,            &SP2::GEconomicEventHandler::HandleEconomicWindowUpdate, &m_EconomicEventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GMilitaryMainUpdate,        NULL, NULL);

   REGISTER_GAME_EVENT(SP2::Event::GIncreaseProduction, NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GUpdateAdvisorSettings, NULL, NULL);

   REGISTER_GAME_EVENT(SP2::Event::GHistoryMarkers,            &SP2::GGeneralEventHandler::HandleHistoryMarkers, &m_EventHandler);

   REGISTER_GAME_EVENT(SP2::Event::GEventCellCreation, NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GEventCellUpdateStatus, NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GEventCellNewMission, NULL, NULL);
   REGISTER_GAME_EVENT(SP2::Event::GSendCovertOpsMissionDone,  &SP2::GMilitaryEventHandler::HandleMissionOver,  &m_MilitaryEventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GGetCountryParties, &SP2::GGeneralEventHandler::HandleGetCountryParties, &m_EventHandler);

   REGISTER_GAME_EVENT(SP2::Event::GAcknowledge, &SP2::GGeneralEventHandler::HandleAcknowledge,  &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GProgress,    &SP2::GGeneralEventHandler::HandleProgress,     &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GResetGame,   &SP2::GGeneralEventHandler::HandleResetGame,    &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GNewGame,     NULL,    NULL);
   REGISTER_GAME_EVENT(SP2::Event::GNewComplete, &SP2::GGeneralEventHandler::HandleNewComplete,  &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GLoadGame,    NULL,    NULL);
   REGISTER_GAME_EVENT(SP2::Event::GLoadComplete,&SP2::GGeneralEventHandler::HandleLoadComplete, &m_EventHandler);
   REGISTER_GAME_EVENT(SP2::Event::GSaveGame,    NULL,    NULL);
   REGISTER_GAME_EVENT(SP2::Event::GSaveComplete,&SP2::GGeneralEventHandler::HandleSaveComplete, &m_EventHandler);

   REGISTER_GAME_EVENT(SP2::Event::GEventGroupSplit, NULL, NULL);

   //! register acknowldgement handler
   REGISTER_GAME_EVENT(SP2::Event::GAnswerAckGameEvent, &SP2::GRequestManager::HandleAnswerAcknowledgement, &(DCL().RequestManager()));
   //! register regions values event handler
   REGISTER_GAME_EVENT(SP2::Event::GEventGetRegionsValues, &SP2::GEconomicEventHandler::HandleRegionsValuesUpdate, &m_EconomicEventHandler);
   //! register technologies values event handler
   REGISTER_GAME_EVENT(SP2::Event::GEventGetTechnologiesValues, &SP2::GEconomicEventHandler::HandleTechnologiesValuesUpdate, &m_EconomicEventHandler);
   //! register trade proposal event handler
   REGISTER_GAME_EVENT(SP2::Event::GEventTradeProposal, &SP2::GEconomicEventHandler::HandleRegionsTradeProposal, &m_EconomicEventHandler);

   g_Joshua.RegisterConsoleCommand(L"connect", L"P",  
      L"connect IP[:PORT] - connect the client to the server located at IP. PORT is optional", 
      (CALLBACK_HANDLER_GS_crGS_cvrGS)&GClient::ConsoleMainCommandsHandler, this);
   g_Joshua.RegisterConsoleCommand("version",  L"",
      L"Gives the version # of the current build.",
      (CALLBACK_HANDLER_GS_crGS_cvrGS)&GClient::ConsoleMainCommandsHandler, this);

    g_Joshua.RegisterConsoleCommand("kick",  L"S",
      L"Kick a player",
      (CALLBACK_HANDLER_GS_crGS_cvrGS)&GClient::ConsoleMainCommandsHandler, this);

    g_Joshua.RegisterConsoleCommand("ban",  L"S",
      L"Ban a player",
      (CALLBACK_HANDLER_GS_crGS_cvrGS)&GClient::ConsoleMainCommandsHandler, this);

#ifdef GOLEM_DEBUG
   //Todo: remove this
   g_Joshua.RegisterConsoleCommand(L"show", L"S",  
      L"show window_name - (temp) shows a window, given its name", 
      (CALLBACK_HANDLER_GS_crGS_cvrGS)&GClient::ConsoleMainCommandsHandler, this);

   //Todo: remove this
   g_Joshua.RegisterConsoleCommand(L"realistic", L"F",  
      L"realistic real_value - (temp) switch background view to realistic mode", 
      (CALLBACK_HANDLER_GS_crGS_cvrGS)&GClient::ConsoleMainCommandsHandler, this);

   //Todo: remove this
   g_Joshua.RegisterConsoleCommand(L"strategic", L"F",  
      L"strategic value - (temp) switch background view to strategic mode", 
      (CALLBACK_HANDLER_GS_crGS_cvrGS)&GClient::ConsoleMainCommandsHandler, this);

#endif
}

void GClient::ChangeGameType(EGameTypes::Enum in_eNewGameType)
{
   m_eCurrentGameType = in_eNewGameType;
}


void GClient::ImmediatlyChangeMusicalTheme(AdaptiveMusic::ESP2MusicalTheme::Enum in_eTheme)
{
   if(m_pAdaptiveMusicPlayer->ThemeManager()->NowPlayingAtASpeakerCloseToYou() != in_eTheme)
   {
      AdaptiveMusic::GMusicCommand* l_pFadeOutAndChangeThemeCommand = new AdaptiveMusic::GMusicCommand;
      l_pFadeOutAndChangeThemeCommand->m_eCommand = AdaptiveMusic::ESP2MusicCommand::FadeOutAndChangeTheme;
      l_pFadeOutAndChangeThemeCommand->m_iTotalBlock = 8;
      l_pFadeOutAndChangeThemeCommand->m_iTotalBlockSoFar = 0;
      m_pAdaptiveMusicPlayer->ThemeManager()->RequestTheme(in_eTheme);
      m_pAdaptiveMusicPlayer->SetCommand(l_pFadeOutAndChangeThemeCommand);
   }
}

void GClient::LimitFPS(bool in_bLimit)
{
   m_bLimitFPS = in_bLimit;
}

void GClient::ScenarioSphere(UINT32 in_iSphereID, bool in_bIsHard)
{
   m_iScenarioSphere = in_iSphereID;
   m_bScenarioIsHard = in_bIsHard;
}

void GClient::ContinueCurrentGame(UINT32 in_iFSMEvent)
{
   m_iSavedFSMEvent = in_iFSMEvent;
}

void GClient::ContinueCurrentGame()
{
   gassert(m_iSavedFSMEvent != 0, "FSM event was not saved");

   m_bPlayCredits = false;
   m_pStateMachine->SendEvent(m_iSavedFSMEvent);
   m_iSavedFSMEvent = 0;
}
