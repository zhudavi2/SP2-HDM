/**************************************************************
*
* sp2_server.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Mathieu Tremblay & Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../../includes/common_lib//sp2_event_history_markers.h"

#pragma warning (disable:4407)

GServer::GServer() :
m_DAL(),
m_DCL(&m_DAL),
m_WorldBehavior()
{
   g_Joshua.FileManager()->File(L"\\data\\StringTable.english.gst",m_StrTableFile);
   m_StrTable.Open(m_StrTableFile);
   m_fPreviousSyncDataUpdate = -1;
   m_bGameStarted    = false; //!<When the game is really started, Iterate
   m_bGameLoaded     = false;
   m_fHalfSecondClock = -1;
   m_fFiveSecondsClock  = -1;
	m_iMilitaryRankCountryToIterate = 1;
   m_fNextCombatIterationTime = 0.0f;
   m_iNuclearMissilesSubmarineCountryToReload = 1;	

	m_fAIAggressiveness = 0.5f;
   m_bPendingLoad = false;
   m_bPendingNew = false;
   m_bUpdateStaticOnly = false;
}

GServer::~GServer()
{
   m_StrTable.Close();
}


//! Initialize the SP2 Server
/*!
 *  Initializes the server.  Called by GJoshua::Init_As_Server
 *    This function does:
 *       - Register game event handlers
 *       - Initialize the database
 *       - Create the country List
 *       - Create the Active player list (as AI players)
 *
 * @return SDK::GAME_MSG: A Status message, SDK::GAME_OK if no error, != SDK::GAME_OK otherwise (@see game_process_itf.h)
 **/
SDK::GAME_MSG GServer::Initialize()
{
   GString  l_sTemp;
   GString  sCombatIterationPeriod;
	GString  l_sAIAggressiveness;

   g_Joshua.GameTimeSpeedSynchroEnable(true, 1);

   // Initialize Mode
   // JMercier : This will be changed later by reading a xml file or a menu.
   ModName(L"Human Development Mod V8");

   GDALInterface::Instance = &m_DAL;
   GDCLInterface::Instance = &m_DCL;
   m_DCL.RegisterCreateCountryDataMethod(GCountryData::New);

   // load our sp2 specifics game options
   LoadGameOptions();

   // Game event registration
   {
      REGISTER_GAME_EVENT(SP2::Event::GNotifyClient,              NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GGetRegionsCharacteristicResult, NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GStartGame,                 NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GRandomEvent,					NULL, NULL);
      REGISTER_GAME_EVENT_AI(SP2::Event::GStrategicWarfareNotify,    
         NULL, 
         NULL,
         &GAIGeneralEventHandler::HandleNukeLaunchNotify,
         &m_AIGeneralEventHandler);
      REGISTER_GAME_EVENT_AI(SP2::Event::GWarDeclarationNotify,    
         NULL, 
         NULL,
         &GAIGeneralEventHandler::HandleWarDeclaration,
         &m_AIGeneralEventHandler);	
      REGISTER_GAME_EVENT_AI(SP2::Event::GTreatyOffer,    
         NULL, 
         NULL,
         &GAIGeneralEventHandler::HandleTreatyOffer,
         &m_AIGeneralEventHandler);
      REGISTER_GAME_EVENT_AI(SP2::Event::GTreatyJoin,    
         &GTreatyEventHandler::HandleTreatyJoin, 
         &m_TreatyGameEventHandler,
         &GAIGeneralEventHandler::HandleTreatyJoin,
         &m_AIGeneralEventHandler);
      REGISTER_GAME_EVENT_AI(SP2::Event::GTreatyLeave,    
         &GTreatyEventHandler::HandleTreatyLeave, 
         &m_TreatyGameEventHandler,
         &GAIGeneralEventHandler::HandleTreatyLeave,
         &m_AIGeneralEventHandler);
      REGISTER_GAME_EVENT_AI(SP2::Event::GTreatyPeaceSigned,    
         NULL, 
         NULL,
         &GAIGeneralEventHandler::HandleTreatyPeaceSigned,
         &m_AIGeneralEventHandler);	

      REGISTER_GAME_EVENT(SP2::Event::GSendData,                  NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GReceiveCountryList,        NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GReceiveCountryStaticData,  NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GErrorCountryStaticData,    NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GUpdateData,                NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GUpdateTreaty,              NULL, NULL);
	   REGISTER_GAME_EVENT(SP2::Event::GUpdateWar,						NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GUpdateUnitValue,           NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GRefuseMilitaryAction,      NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GConquerCountry,            NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GSendNews,		            NULL, NULL);	
      REGISTER_GAME_EVENT(SP2::Event::GHistoryMarkers,		      NULL, NULL);	
      REGISTER_GAME_EVENT(SP2::Event::GCombatOver,                NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GSynchronizeCountryData,    NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GSendCovertOpsMissionDone,  NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GBombardmentNotification,   NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GSendPlan,		            &SP2::GAdvisorEventHandler::HandleAdvisorAnswer, &m_AdvisorEventHandler);

      REGISTER_GAME_EVENT(SP2::Event::GStartGameRequest,          &SP2::GGeneralEventHandler::HandleStartGameRequest,          &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GSendAvailableCountries,    &SP2::GGeneralEventHandler::HandleRequestAvailableCountries, &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GGetRegionsCharacteristic,  &SP2::GGeneralEventHandler::HandleGetRegionCharacteristic,   &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GSetPlayerInfo,             &SP2::GGeneralEventHandler::HandleSetPlayerInfo,             &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GGetPlayersList,            &SP2::GGeneralEventHandler::HandleGetPlayersList,            &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GGetCountryRanks,           &SP2::GGeneralEventHandler::HandleGetCountryRanks,           &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GGetCountriesRanks,         &SP2::GGeneralEventHandler::HandleGetCountriesRanks,         &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GGameOver,                  &SP2::GGeneralEventHandler::HandleGameOverAnswer,            &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GObjectiveCompleted,        NULL,                                                       &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GSetGameObjectives,         &SP2::GGeneralEventHandler::HandleSetObjectives,             &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GChangeGameSpeed,           &SP2::GGeneralEventHandler::HandleChangeGameSpeed,           &m_EventHandler);
		REGISTER_GAME_EVENT(SP2::Event::GChangeAIAggressiveness,    &SP2::GGeneralEventHandler::HandleChangeAIAggressiveness,    &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GRelationHistoryFetch,      &SP2::GGeneralEventHandler::HandleRelationHistoryFetch,      &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GRequestCountryList,        &SP2::GGeneralEventHandler::HandleRequestCountryList,        &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GRequestCountryStaticData,  &SP2::GGeneralEventHandler::HandleRequestCountryStaticData,  &m_EventHandler);
		REGISTER_GAME_EVENT(SP2::Event::GEventFetchCountriesRelations, &SP2::GGeneralEventHandler::HandleFetchCountriesRelations,&m_EventHandler);
		
		REGISTER_GAME_EVENT(SP2::Event::GEventGetRegionsValues,		&SP2::GEconomicEventHandler::HandleGetRegionValues,				 &m_EconomicGameEventHandler);      
      REGISTER_GAME_EVENT(SP2::Event::GEventGetTechnologiesValues,&SP2::GEconomicEventHandler::HandleGetTechnologiesValues,     &m_EconomicGameEventHandler);
		REGISTER_GAME_EVENT(SP2::Event::GEventTradeProposal,        &SP2::GEconomicEventHandler::HandleTradeOffer,			       &m_EconomicGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GEconomicUpdate,            &SP2::GEconomicEventHandler::HandleEconomicWindowUpdate,      &m_EconomicGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GEconomicHealthUpdate,      &SP2::GEconomicEventHandler::HandleUpdateEconomicHealth,      &m_EconomicGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GResourcesUpdate,           &SP2::GEconomicEventHandler::HandleUpdateResources,           &m_EconomicGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GIncreaseProduction,        &SP2::GEconomicEventHandler::HandleIncreaseProduction,        &m_EconomicGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GBudgetUpdate,              &SP2::GEconomicEventHandler::HandleUpdateBudget,              &m_EconomicGameEventHandler);
	   REGISTER_GAME_EVENT(SP2::Event::GEventUpdateWarState,			&SP2::GMilitaryEventHandler::HandleUpdateWarState,				 &m_MilitaryGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GPerformOffshoreBombardment,&SP2::GMilitaryEventHandler::HandlePerformOffshoreBombardment,&m_MilitaryGameEventHandler);
      

      REGISTER_GAME_EVENT(SP2::Event::GStrategicWarfareLaunch,    &SP2::GMilitaryEventHandler::HandleStrategicWarfareLaunch,   &m_MilitaryGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GRequestTroopsMove,         &SP2::GMilitaryEventHandler::HandleMoveUnit,                 &m_MilitaryGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GRequestMilitaryStatusChange,&SP2::GMilitaryEventHandler::HandleStatusChange,            &m_MilitaryGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GAddUnitDesign,             &SP2::GMilitaryEventHandler::HandleAddUnitDesign,            &m_MilitaryGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GUnitResearchUpdate,        &SP2::GMilitaryEventHandler::HandleUnitResearchUpdateRequest,&m_MilitaryGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GBattleOverviewUnitActionUpdate, &SP2::GMilitaryEventHandler::HandleBattleOverviewUnitActionUpdate, &m_MilitaryGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GOneClickInvade,				&SP2::GMilitaryEventHandler::HandleOneClickInvade,				&m_MilitaryGameEventHandler);
      

      REGISTER_GAME_EVENT(SP2::Event::GPoliticUpdate,             &SP2::GPoliticEventHandler::HandlePoliticWindowUpdate,       &m_PoliticGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GInternalLawsUpdate,        &SP2::GPoliticEventHandler::HandleUpdateInternalLaws,        &m_PoliticGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GConstitutionalFormUpdate,  &SP2::GPoliticEventHandler::HandleUpdateConstitutionalForm,  &m_PoliticGameEventHandler);
		REGISTER_GAME_EVENT(SP2::Event::GEventTreatyRefusalDetails, NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GMilitaryMainUpdate,        &SP2::GMilitaryEventHandler::HandleMilitaryMainUpdateRequest,&m_MilitaryGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GUpdateAdvisorSettings,     &SP2::GGeneralEventHandler::HandleUpdateAdvisorSettings,     &m_EventHandler);

      // Register event for UnitProductor
      REGISTER_GAME_EVENT(SP2::Event::GBuildUnitOrder,            &SP2::GUnitProductionEventHandler::HandleBuildUnitOrder,     &m_UnitProductionGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GChangeUnitOrder,           &SP2::GUnitProductionEventHandler::HandleChangeOrder,        &m_UnitProductionGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GRequestProductionQueue,    &SP2::GUnitProductionEventHandler::HandleRequestProductionQueue,&m_UnitProductionGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GDeployUnits,               &SP2::GUnitProductionEventHandler::HandleDeployUnits,        &m_UnitProductionGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GSplitUnits,                &SP2::GUnitProductionEventHandler::HandleSplitUnits,         &m_UnitProductionGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GCancelDeployement,         &SP2::GUnitProductionEventHandler::HandleCancelDeployement,  &m_UnitProductionGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GUpdateUnitOrderStatus,     NULL,                                                       NULL);
      REGISTER_GAME_EVENT(SP2::Event::GUpdateForeignBuildingCountryList,NULL,                                                 NULL);
      REGISTER_GAME_EVENT(SP2::Event::GRequestForeignBuildingCountryList, &SP2::GUnitProductionEventHandler::HandleRequestBuildingCountryList, &m_UnitProductionGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GRegisterAsBuildingCountry, &SP2::GUnitProductionEventHandler::HandleGRegisterAsBuildingCountry, &m_UnitProductionGameEventHandler);

      // Register event for Unit Training
      REGISTER_GAME_EVENT(SP2::Event::GTrainUnitRequest, &SP2::GUnitProductionEventHandler::HandleTrainUnitRequest, &m_UnitProductionGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GDisbandUnitRequest, &SP2::GUnitProductionEventHandler::HandleDisbandUnitRequest, &m_UnitProductionGameEventHandler);

      // Register event for unit market
      REGISTER_GAME_EVENT(SP2::Event::GRequestForSaleUnitList,&SP2::GUnitProductionEventHandler::HandleRequestForSaleUnitList,&m_UnitProductionGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GUpdateForSaleUnitList,NULL,NULL);
      REGISTER_GAME_EVENT(SP2::Event::GTransfertUnit,&SP2::GUnitProductionEventHandler::HandleUnitTransfert,&m_UnitProductionGameEventHandler);


      REGISTER_GAME_EVENT(SP2::Event::GTreatyDetailsTreatyAdd,    &SP2::GPoliticEventHandler::HandleTreatyAdd,     &m_PoliticGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GRequestTreatyConditionsCountries, &SP2::GTreatyEventHandler::HandleConditionsCountries, &m_TreatyGameEventHandler);

      REGISTER_GAME_EVENT(SP2::Event::GEventCountryInfo,          &SP2::GGeneralEventHandler::HandleCountryInfo,    &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GEventCellCreation,         &SP2::GMilitaryEventHandler::HandleCellCreate,    &m_MilitaryGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GEventCellUpdateStatus,     &SP2::GMilitaryEventHandler::HandleCellUpdate,    &m_MilitaryGameEventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GEventCellNewMission,       &SP2::GMilitaryEventHandler::HandleNewMission,    &m_MilitaryGameEventHandler);

      REGISTER_GAME_EVENT(SP2::Event::GGetCountryParties,         &SP2::GGeneralEventHandler::HandleGetCountryParties, &m_EventHandler);

      //! register acknowldgement event
      REGISTER_GAME_EVENT(SP2::Event::GAnswerAckGameEvent, NULL, NULL);      

      REGISTER_GAME_EVENT(SP2::Event::GAcknowledge, NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GProgress,    NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GResetGame,   &SP2::GGeneralEventHandler::HandleResetGame,   &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GNewGame,     &SP2::GGeneralEventHandler::HandleNewGame,     &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GNewComplete, NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GLoadGame,    &SP2::GGeneralEventHandler::HandleLoadGame,    &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GLoadComplete,NULL, NULL);
      REGISTER_GAME_EVENT(SP2::Event::GSaveGame,    &SP2::GGeneralEventHandler::HandleSaveGame,    &m_EventHandler);
      REGISTER_GAME_EVENT(SP2::Event::GSaveComplete,NULL, NULL);

      REGISTER_GAME_EVENT(SP2::Event::GEventGroupSplit, &SP2::GUnitProductionEventHandler::HandleGroupSplit, &m_UnitProductionGameEventHandler);
   }

   m_DAL.DataManager()->RegisterNode(this);
   IF_RETURN(!m_DAL.Initialize(SP2::SERVER_CONFIG),SDK::GAME_INIT_ERROR);
   IF_RETURN(!m_DAL.LoadServerConfigGameplayConstants(),SDK::GAME_INIT_ERROR);

   m_DAL.MapEngineInit(true);

   // set the combat iteration period
   if (m_DAL.XMLValue(SP2::COMBAT_ITERATION_PERIOD,sCombatIterationPeriod))
   {
      m_fCombatIterationPeriod = sCombatIterationPeriod.ToREAL64();        
   }
   else
   {
      g_Joshua.Log( L"Could not set AI combat iteration period", MSGTYPE_FATAL_ERROR );
      return SDK::GAME_INIT_ERROR;
   }	 

   // Setup the AI
   if ( ! InitAI() )
   {
      g_Joshua.Log( L"Couldn't Initialize the AI", MSGTYPE_FATAL_ERROR );
      return SDK::GAME_INIT_ERROR;
   }

   // World Behavior Initialisation
   m_WorldBehavior.Reset();
   IF_RETURN_LOG(!m_WorldBehavior.LoadConfigFile(L"behavior.xml", L""),SDK::GAME_INIT_ERROR,"Couldnt load behavior file");

   {
        // Default values
        InitializeDefaultConfig();

        // Load the SP2-HDM_Config.xml
        LoadSP2HDMConfigXML();
   }

   // Initialize the DCL (must be done before loading military data, since it uses the unit mover)
   m_DCL.Initialize();

   //Initialize the SDK::Combat Engine
   if(!CombatEngineInit())
   {
      g_Joshua.Log(L"Couldnt Initialize the SDK::Combat engine",MSGTYPE_FATAL_ERROR);
      return SDK::GAME_INIT_ERROR;
   }

   //Register the Unit constructors to the unit manager
   g_Joshua.UnitManager().RegisterUnitCreationFunc(SP2::GUnit::New);
   //Register the Unit Group constructors to the unit manager
   g_Joshua.UnitManager().RegisterUnitGroupCreationFunc(SP2::GUnitGroupEx::New);
   //Register the Unit Design constructors to the unit manager
   g_Joshua.UnitManager().RegisterUnitDesignCreationFunc(SP2::GUnitDesign::New);
   //Register the arena information construction functoin
   g_Joshua.UnitManager().RegisterArenaInfoCreationFunc(SP2::GArenaInfo::New);
   //Register Arena creation function
   g_CombatManager.RegisterArenaCreationFunc(SP2::GArena::New);

   //Attach the observer to the unit manager
   g_Joshua.UnitManager().Attach(&m_MilitaryGameEventHandler, SDK::Combat::c_notificationBeforeUnitKilled);

   IF_RETURN_LOG(!m_DAL.LoadUnitTypes(), SDK::GAME_INIT_ERROR, "Unable to load unit designs");

   // Register server specific console commands
   {
      g_Joshua.RegisterConsoleCommand(
         L"name",
         L"S",
         L"Used to change the name of the server",   
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"version",
         L"",
         L"Gives the current version # of the game.",   
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"new",
         L"",
         L"Create a new game.",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"newfrom",
         L"S",
         L"Create a new game from the specified database.",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"load",
         L"S",
         L"Load a game from the specified file.",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"save",
         L"S",
         L"Save the game to the specified file.",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         "kick",  
         L"S",
         L"Kick a player",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler, this);

      g_Joshua.RegisterConsoleCommand(
         "ban",  
         L"S",
         L"ban a player",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler, this);

      g_Joshua.RegisterConsoleCommand(
         "clear_ban_list",  
         L"",
         L"Clear the list of banned people",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler, this);

      g_Joshua.RegisterConsoleCommand(
         "set_admin_country",
         L"I",
         L"Set server admin player by country ID",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler, this);

      g_Joshua.RegisterConsoleCommand(
         "set_admin_player",
         L"I",
         L"Set server admin player by player ID",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler, this);

      g_Joshua.RegisterConsoleCommand(
         "print_players",
         L"",
         L"List all current players",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler, this);

      g_Joshua.RegisterConsoleCommand(
          L"print_military_control",
          L"I",
          L"List all regions under military control of country, as well as political and historical claims",
          (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler, this);

      g_Joshua.RegisterConsoleCommand(
          L"print_foreign_presence",
          L"I",
          L"List all foreign military units on country's territory",
          (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler, this);

      g_Joshua.RegisterConsoleCommand(
          L"print_regions_in_area",
          L"IIII",
          L"List all regions within given low and high latitudes, and low and high longitudes",
          (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler, this);

      g_Joshua.RegisterConsoleCommand(
          L"declare_peace",
          L"",
          L"Force all countries to declare peace, ending all wars",
          (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler, this);

   }




#ifdef GOLEM_DEBUG
   //ALL THE STUFF BETWEEN THESE BRACKETS IS FOR DEBUGGING PURPOSES AND CAN BE REMOVED
   //IN THE RELEASE VERSION
   {
      g_Joshua.RegisterConsoleCommand(
         L"build",
         L"SSS",
         L"Used to build units, 3 Params needed: CountryID DesignID Qty",   
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"start_game",
         L"",
         L"Artificially start the game",   
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"fight",
         L"",
         L"Artificially setup a fight",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"status",
         L"II",
         L"Change status of a unit group",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"declare_war",
         L"II",
         L"Declare war between the 2 countries.",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
          L"declare_war_2v2",
          L"IIII",
          L"Declare war between the first 2 and last 2 countries. First and third countries are attacking and defending masters, respectively.",
          (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
          this);

		g_Joshua.RegisterConsoleCommand(
         L"print_wars",
         L"",
         L"Print the description of every wars currently going on.",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"sell_units",
         L"",
         L"Print, and sell, necessary units for every country",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"print_relations",
         L"I",
         L"Print relations of that country",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"embargo",
         L"II",
         L"First country make a trade embargo against second country.",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"occupy",
         L"II",
         L"First country will occupy second country.",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"military_removal",
         L"II",
         L"First country ask for second country to leave",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"annex",
         L"II",
         L"annex all possible regions of second country to first country",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"bombard",
         L"II",
         L"test bombardment, country1 tries to bombard country2",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"givemoney",
         L"II",
         L"give money to accelerate testing country, money amount",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"build_amds",
         L"I",
         L"force country to build AMDS",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"print_amds",
         L"I",
         L"print country's AMDS status",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"force_occupy",
         L"III",
         L"All second country's regions will become occupied by first country, test conquer",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"research_nuclear",
         L"I",
         L"force country to start nuclear research",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
         L"print_nuclear_research",
         L"I",
         L"print country's general nuclear research level, and range, precision, and damage research levels",
         (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
         this);

      g_Joshua.RegisterConsoleCommand(
          L"force_client",
          L"II",
          L"first country creates client treaty with second country",
          (CALLBACK_HANDLER_GS_crGS_cvrGS)&GServer::ConsoleServerCommandsHandler,
          this);
   } 
#endif

   //Init the military plans vector
   g_CombatPlanner.Init();

   // Set the game speed as paused by default
   g_Joshua.GameSpeed(0);

   m_DAL.ResetUpdateData();
   m_DAL.ResetUpdateUnitValue();

   // Init some memory
   g_ServerDCL.InitCacheMemory();

   // Initialize the default game objectives
   {
      m_WorldBehavior.Attach(&g_ServerDCL.GameObjectiveEvaluator(), c_iNotificationLeadingPoliticalPartyChangeAfterElections);
      m_DCL          .Attach(&g_ServerDCL.GameObjectiveEvaluator(), c_iNotificationOnSuccessfulCoupEtat);
      m_DCL          .Attach(&g_ServerDCL.GameObjectiveEvaluator(), c_iNotificationOnNukeLaunched);
      m_DCL          .Attach(&g_ServerDCL.GameObjectiveEvaluator(), c_iWorldPeaceStatusChanged);
   }


   // Automatically start a new game on a dedicated server
   if(!g_Joshua.Dependant() )
   {
      m_sNewGameDB = c_sDatabaseLocation;
      m_iNewsource = 0;

      NewGame();
   }

   return SDK::GAME_OK;
}
 
/*!
* Load our mod command line parameters
**/
void GServer::LoadGameOptions()
{
   // Parse the joshua command line for extra parameters
   if(wcsstr(g_Joshua.ModCommandLine().c_str(), L"+nonuke"))
      m_DAL.GameOptions().NuclearAllowed(false);
   else
      m_DAL.GameOptions().NuclearAllowed(true);
}

//! Iterate as the server
/*!
* Iterate function of the server, called every iteration by the GJoshua engine.
**/
SDK::GAME_MSG GServer::Iterate(void* param)
{
   bool l_bUpdateGameTime = false;
   m_fLastSpeed = g_Joshua.GameSpeed();
   while(!m_vPendingSaves.empty() )
   {
      g_Joshua.GameSpeed(0);
      SaveGame(m_vPendingSaves.front() );

      m_vPendingSaves.pop_front();
      l_bUpdateGameTime = true;
   }

   if(l_bUpdateGameTime)
   {
      g_Joshua.GameSpeed(m_fLastSpeed);
      g_Joshua.GameTime(g_Joshua.GameTime() );
   }

   if(m_bPendingNew)
   {
      NewGame();
      m_bPendingNew = false;
   }
   else if(m_bPendingLoad)
   {
      LoadGame();
      m_bPendingLoad = false;
   }

   if(!GameStarted() )
   {
      return SDK::GAME_OK;
   }

   // Update all unit values
   g_ServerDAL.UpdateCountryUnitAndMissilesValues();

   m_DCL.UnitMover().Iterate();	 

   g_CombatManager.Iterate(g_Joshua.GameTime() );
   if (g_Joshua.Clock() >= m_fNextCombatIterationTime)
   {		
		if(m_iMilitaryRankCountryToIterate == 1)
		{
			//redo the ranks
			m_OrderCountriesByMilitaryRank.clear();
			for(UINT32 i=1; i<= g_ServerDAL.NbCountry() ; i++)
			{
				if(!g_ServerDAL.CountryIsValid(i))
					continue;
				m_OrderCountriesByMilitaryRank.insert(make_pair<INT32,UINT32>(g_ServerDAL.CountryData(i)->MilitaryRank(),i));
				m_IteratorMilitaryRank = m_OrderCountriesByMilitaryRank.begin();
			}
		}
      m_fNextCombatIterationTime = g_Joshua.Clock() + m_fCombatIterationPeriod;
      g_CombatPlanner.Iterate( m_IteratorMilitaryRank->second );
		m_IteratorMilitaryRank++;
		m_iMilitaryRankCountryToIterate++;
		if(m_iMilitaryRankCountryToIterate > m_OrderCountriesByMilitaryRank.size() )
		{
			m_iMilitaryRankCountryToIterate = 1;

		}
   }

   // Re-Update all unit values
   g_ServerDAL.UpdateCountryUnitAndMissilesValues();

   m_DCL.IterateCountryControl();

   m_WorldBehavior.GlobalIterate(g_Joshua.GameTime() );

   // Update region, city & capital data, if needed
   m_DAL.SendUpdateData();

   // Update historical markers
   m_DAL.SendHistory();

   //Perform every 1.5 seconds
   REAL64 l_fActualClock = g_Joshua.Clock();
   if((l_fActualClock - m_fPreviousSyncDataUpdate) >= 1.5f)
   {
      SyncronizeMilitary(SDK::Event::ESpecialTargets::BroadcastHumanPlayers,l_fActualClock);
      m_DAL.SendUpdateUnitValue();

      //Synchronize the country data of each human player
      {
         SDK::GGameEventSPtr l_SyncEvt = CREATE_GAME_EVENT(SP2::Event::GSynchronizeCountryData);
         for(SDK::GPlayers::const_iterator l_HumanPlayersIt = g_Joshua.HumanPlayers().begin() ;
            l_HumanPlayersIt != g_Joshua.HumanPlayers().end();
            l_HumanPlayersIt++)
         {
            if(l_HumanPlayersIt->second->PlayerStatus() ==  SDK::PLAYER_STATUS_ACTIVE)
            {
               //Check if the country data of that human player must be synchronized, if so, send it
               GCountryData* l_pCountryData = g_ServerDAL.CountryData(l_HumanPlayersIt->second->ModID());
               gassert(l_pCountryData,"GServer::Iterate, Country Data does not exist for human player");
               if(l_pCountryData->Dirty())
               {                  
                  l_SyncEvt->m_iSource = SDK::Event::ESpecialTargets::Server;
                  l_SyncEvt->m_iTarget = l_HumanPlayersIt->second->Id();
                  SP2::Event::GSynchronizeCountryData* l_pEvent = (SP2::Event::GSynchronizeCountryData*)l_SyncEvt.get();
                  l_pEvent->CountryData(l_pCountryData);
                  g_Joshua.RaiseEvent(l_SyncEvt);
                  l_pCountryData->ClearModifications();
               }
            }
         }
      }//end of synchronize country data


      //Perform offshore bombardments (every 1.5 seconds)
      {
         m_DCL.UnitMover().PerformOffShoreBombardment();
      }

   }

   //Iterate every 0.5 secs
   if((g_Joshua.Clock() - m_fHalfSecondClock) >= 0.5f)
   {
		m_DAL.SyncronizeTreaties();
      m_DCL.GameObjectiveEvaluator().EvaluateObjectives();
      m_DCL.IterateCovertActionCells();
      m_DAL.UpdateRelationsHistory();		
		m_DAL.IterateRegionSynchronization();

      m_DCL.IterateUnitProduction();

      m_DCL.IterateNukes();      
      m_fHalfSecondClock= g_Joshua.Clock();

      //Cycle the countries to reload the nuclear submarines nuclear missiles
      {
         g_ServerDAL.CountryData(m_iNuclearMissilesSubmarineCountryToReload++)->ReloadSubmarinesNuclearMissiles();
         if(m_iNuclearMissilesSubmarineCountryToReload > g_ServerDAL.NbCountry())
         {
            m_iNuclearMissilesSubmarineCountryToReload = 1;
         }
      }
   }

   //Iterate every 5 seconds
   if((g_Joshua.Clock() - m_fFiveSecondsClock) >= 5.f)
   {
      m_fFiveSecondsClock = g_Joshua.Clock();
      m_DCL.IterateResearch(g_Joshua.GameTime() );
   }

   // Update all unit values
   g_ServerDAL.UpdateCountryUnitAndMissilesValues();

   // Iterate autosave
   if((m_fDedicatedServerAutosavePeriod > 0) && ((g_Joshua.Clock() - m_fTimeOfLastAutosave) >= (m_fDedicatedServerAutosavePeriod * 60.f)))
   {
       m_fTimeOfLastAutosave = g_Joshua.Clock();
       
       if(!m_bDedicatedServerAutosaveToJoshuaFolder)
           m_DCL.CreateSaveDirectory();

       GSaveRequest l_Request;
       const GString& l_sAutosaveName = L"Autosave";
       l_Request.m_iSourceID = 0;
       l_Request.m_sSaveName = l_sAutosaveName;
       l_Request.m_sSaveFile = (m_bDedicatedServerAutosaveToJoshuaFolder ? L"" : (g_Joshua.CurrentMod().m_sPath + c_sSaveGameLocation)) +
                               l_sAutosaveName + c_sSaveExtServer;
       m_vPendingSaves.push_back(l_Request);
   }

   return SDK::GAME_OK;
}


//! Shutdown the game server
/*!
* Shutdown the game server, the database, deletes the event handler, clears the country list
* etc.
* @return SDK::GAME_MSG: SDK::GAME_OK if no error, != SDK::GAME_OK otherwise
**/
SDK::GAME_MSG GServer::Shutdown()
{
   //Saves the AI training data into the entities xml file.
   GString     sFileName;
   GString     sTmpFileName;
   GFile       File;
   if ( ! m_DAL.XMLValue( ENTITIES_FILE, sFileName )  ||
      ! m_DAL.File( sFileName, File )               ||
      ! File.Extract( sTmpFileName )  )
   {
      g_Joshua.Log( L"Could not save entities into XML file", MSGTYPE_FATAL_ERROR );
      return SDK::GAME_FATAL_ERROR;
   }
   GManager::WriteEntities( sTmpFileName );
   File.Unextract();  // destroy the temporary file

   //Shutdown the DAL
   m_DAL.Shutdown();

   //Clear the country list
   m_Countries.clear();

   m_WorldBehavior.Reset();

   //Shutdown the SDK::Combat engine
   CombatEngineShutdown();

   //Detach
   m_WorldBehavior.Detach(&g_ServerDCL.GameObjectiveEvaluator());

   return SDK::GAME_OK;
}


void GServer::SendCountryList(INT32 in_PlayerDestinationId)
{

   //HACK - PROBLEM - (jmercier)
   //This does not send the list of available countries, but the list of unavailable countries.
   //Total opposite

   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GSendAvailableCountries);
   Event::GSendAvailableCountries* l_pAvailableCountryEvent = (Event::GSendAvailableCountries*)l_pEvent.get();
   l_pEvent->m_iTarget = in_PlayerDestinationId;
   l_pEvent->m_iSource = SDK::Event::ESpecialTargets::Server;

   vector<UINT16> l_vAvailableCountries;

   // Iterate over player and send choosen(taken) country
   for (SDK::GPlayers::const_iterator itr =  g_Joshua.HumanPlayers().begin(); itr != g_Joshua.HumanPlayers().end(); itr++)
   {
      // If the player is active(playing) or ready to play, thus country, is unavailable.
      SDK::GPlayer *l_pPlayer = itr->second;
      if(l_pPlayer->PlayerStatus() == SDK::PLAYER_STATUS_READY || 
         l_pPlayer->PlayerStatus() == SDK::PLAYER_STATUS_ACTIVE)
      {
         UINT16 l_iChosenCountry = (UINT16)l_pPlayer->ModID();
         l_vAvailableCountries.push_back(l_iChosenCountry);
      }
   }

   //Add the Inactive countries (most likely dead - annexed)
   for(UINT16 i = 1 ; i <= g_ServerDAL.NbCountry() ; i++)
   {
      if(!g_ServerDAL.CountryData(i)->Activated())
      {
#ifdef GOLEM_DEBUG
         vector<UINT16>::iterator l_AvCountryIt = find(l_vAvailableCountries.begin(),l_vAvailableCountries.end(),i);
         gassert(l_AvCountryIt == l_vAvailableCountries.end(),"Inactive country should not be controlled by a human player,GServer::SendCountryList ");
#endif

         l_vAvailableCountries.push_back(i);
      }
   }





   // Set country list in event
   l_pAvailableCountryEvent->AvailableCountries(l_vAvailableCountries);

   // Fire Event
   g_Joshua.RaiseEvent(l_pEvent);

}

void GServer::SendPlayersList(INT32 in_PlayerDestinationId)
{
   if (in_PlayerDestinationId == 0)
      return;

   SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GGetPlayersList);
   Event::GGetPlayersList & gpl = *(Event::GGetPlayersList *)l_Event.get();

   gpl.m_iSource = SDK::Event::ESpecialTargets::Server;
   gpl.m_iTarget = in_PlayerDestinationId;

   gpl.AdminPlayerID(g_Joshua.AdminPlayerID());

   if (in_PlayerDestinationId == SDK::Event::ESpecialTargets::BroadcastHumanPlayers)
      g_Joshua.Log(L"Broadcasting Players List");
   else
      g_Joshua.Log(L"Sending Players List to client");


   for (SDK::GPlayers::const_iterator itr =  g_Joshua.HumanPlayers().begin(); itr != g_Joshua.HumanPlayers().end(); itr++)
   {
      Event::GGetPlayersList::Player player;
      SDK::GPlayer *l_pPlayer = itr->second;

      player.ClientID         = l_pPlayer->Id();
      player.PlayerName       = l_pPlayer->Name();
      player.CountryID        = l_pPlayer->ModID();
      player.PlayerStatus     = l_pPlayer->PlayerStatus();

      gpl.PlayersList.push_back(player);
   }

   g_Joshua.RaiseEvent(l_Event);
}

//!Function called when a player has joined
/*!
* Called by the Joshua engine when a new player has just joined the
* game.  At the moment this function is called, the new player is already
* in the IDLE player list, has an Id assigned.
**/
void GServer::OnNewPlayerJoined(SDK::GPlayer* in_pPlayer)
{
   g_ServerDAL.AddHumanPlayer(in_pPlayer);
}

/*!
* Handle disconnections
**/
void GServer::OnPlayerDisconnect(SDK::GPlayer* in_pPlayer)
{
   //Accept all pending plans, because an AI would accept it
   if(in_pPlayer->ModID() != -1)
      g_ServerDAL.CountryData(in_pPlayer->ModID())->RemoveAdvisor();

   // a player as left
   m_PlayerIDs.erase(in_pPlayer->Id());
   InformPlayerLeft(in_pPlayer);

   // A new player list was generated, send it
   SendPlayersList();

   return;
}

SDK::GStringTable* GServer::StrTable()
{
   return &m_StrTable;
}

/*!
* Get a vector containing the IDs of the human players
* @return vector<INT32>: A constant reference to the vector of human player ids
**//*
const vector<UINT32>& GServer::HumanPlayers()
{
return m_vHumanPlayersID;
}
*/
/*!
* Initialize the AI (register actions, objectives and entities)
* @return true if successfully initialized, false otherwise
**/
bool GServer::InitAI()
{
   // register objectives
   RegisterObjectives();

   // register actions
   RegisterActions();

   GManager::RegisterPlanCreationFunc(SP2::GPlan::New);

   // load entities from xml file
   GString     sFileName;
   GString     sTmpFileName;
   GFile       File;
   if ( ! m_DAL.XMLValue( ENTITIES_FILE, sFileName )  ||
      ! m_DAL.File( sFileName, File )               ||
      ! File.Extract( sTmpFileName )                ||
      ! GManager::LoadEntities( sTmpFileName ) )
   {
      g_Joshua.Log( L"Could not load entities into EHE", MSGTYPE_FATAL_ERROR );
      return false;
   }
   File.Unextract();  // destroy the temporary file

   // set iteration period
   GString sIterationPeriod;
   if ( ! m_DAL.XMLValue( ITERATION_PERIOD, sIterationPeriod ) )
   {
      g_Joshua.Log( L"Could not set AI iteration period", MSGTYPE_FATAL_ERROR );
      return false;
   }
   g_Joshua.AIIterationPeriod( sIterationPeriod.ToREAL64() );

   GManager::RegisterStartIterationHandler((GEventHandler*)this,(EHE_ITERATION_FUNC_PTR)&SP2::GAI::ExecuteNewAIIteration);
   GManager::RegisterEndIterationHandler((GEventHandler*)this,(EHE_ITERATION_FUNC_PTR)&SP2::GAI::ExecuteFinishAIIteration);

   return true;
}

/*!
* Register objectives in EHE
**/
void GServer::RegisterObjectives()
{
   GManager::RegisterObjective( EHEObjectives::Ids::POPULATION_SUPPORT,
      EHEObjectives::Names::POPULATION_SUPPORT,
      false,
      EHEObjectives::GPopulationSupport() );

   GManager::RegisterObjective( EHEObjectives::Ids::HAVE_SPACE,
      EHEObjectives::Names::HAVE_SPACE,
      false,
      EHEObjectives::GHaveSpace() );

   GManager::RegisterObjective( EHEObjectives::Ids::GOVERNMENT_STABILITY,
      EHEObjectives::Names::GOVERNMENT_STABILITY,
      false,
      EHEObjectives::GGovernmentStability() );

   GManager::RegisterObjective( EHEObjectives::Ids::ECONOMY_TOO_LOW,
      EHEObjectives::Names::ECONOMY_TOO_LOW,
      false,
      EHEObjectives::GEconomyTooLow() );

   GManager::RegisterObjective( EHEObjectives::Ids::ECONOMY_TOO_HIGH,
      EHEObjectives::Names::ECONOMY_TOO_HIGH,
      false,
      EHEObjectives::GEconomyTooHigh() );

   GManager::RegisterObjective( EHEObjectives::Ids::HAVE_POSITIVE_BUDGET,
      EHEObjectives::Names::HAVE_POSITIVE_BUDGET,
      false,
      EHEObjectives::GHaveMoney() );

   GManager::RegisterObjective( EHEObjectives::Ids::MEET_RESOURCE_DEMAND,
      EHEObjectives::Names::MEET_RESOURCE_DEMAND,
      false,
      EHEObjectives::GIndustrialStrength() );   

   GManager::RegisterObjective( EHEObjectives::Ids::HIGH_SELF_RELATIONS,
      EHEObjectives::Names::HIGH_SELF_RELATIONS,
      false,
      EHEObjectives::GHighSelfRelations() );

   GManager::RegisterObjective( EHEObjectives::Ids::HAVE_MILITARY_ALLIES,
      EHEObjectives::Names::HAVE_MILITARY_ALLIES,
      false,
      EHEObjectives::GHaveMilitaryAllies() );

   GManager::RegisterObjective( EHEObjectives::Ids::HIGH_NATIONAL_SECURITY,
      EHEObjectives::Names::HIGH_NATIONAL_SECURITY,
      false,
      EHEObjectives::GHaveHighNationalSecurity() );

   GManager::RegisterObjective( EHEObjectives::Ids::HIGH_HUMAN_DEVELOPMENT,
      EHEObjectives::Names::HIGH_HUMAN_DEVELOPMENT,
      false,
      EHEObjectives::GHaveHighHumanDevelopment() );

   GManager::RegisterObjective( EHEObjectives::Ids::HIGH_INFRASTRUCTURE,
      EHEObjectives::Names::HIGH_INFRASTRUCTURE,
      false,
      EHEObjectives::GHaveHighInfrastructure() );

   GManager::RegisterObjective( EHEObjectives::Ids::HIGH_TELECOM_LEVEL,
      EHEObjectives::Names::HIGH_TELECOM_LEVEL,
      false,
      EHEObjectives::GHaveHighTelecomLevel() );

   GManager::RegisterObjective( EHEObjectives::Ids::HIGH_ECONOMIC_HEALTH,
      EHEObjectives::Names::HIGH_ECONOMIC_HEALTH,
      false,
      EHEObjectives::GHaveHighEconomicHealth() );

   GManager::RegisterObjective( EHEObjectives::Ids::HAVE_NO_DEBT,
      EHEObjectives::Names::HAVE_NO_DEBT,
      false,
      EHEObjectives::GHaveNoDebt() );

   GManager::RegisterObjective( EHEObjectives::Ids::STRONG_AIR_FORCES,
      EHEObjectives::Names::STRONG_AIR_FORCES,
      false,
      EHEObjectives::GStrongAir() );

   GManager::RegisterObjective( EHEObjectives::Ids::STRONG_GROUND_FORCES,
      EHEObjectives::Names::STRONG_GROUND_FORCES,
      false,
      EHEObjectives::GStrongGround() );

   GManager::RegisterObjective( EHEObjectives::Ids::STRONG_NAVAL_FORCES,
      EHEObjectives::Names::STRONG_NAVAL_FORCES,
      false,
      EHEObjectives::GStrongNaval() );

   GManager::RegisterObjective( EHEObjectives::Ids::STRONG_INFANTRY_FORCES,
      EHEObjectives::Names::STRONG_INFANTRY_FORCES,
      false,
      EHEObjectives::GStrongInfantry() );

   GManager::RegisterObjective( EHEObjectives::Ids::STRONG_NUCLEAR_FORCES,
      EHEObjectives::Names::STRONG_NUCLEAR_FORCES,
      false,
      EHEObjectives::GStrongNuclear() );

   GManager::RegisterObjective( EHEObjectives::Ids::HAVE_AMDS,
      EHEObjectives::Names::HAVE_AMDS,
      false,
      EHEObjectives::GHaveAMDS() );

   GManager::RegisterObjective( EHEObjectives::Ids::GOOD_RESEARCH_LEVEL,
      EHEObjectives::Names::GOOD_RESEARCH_LEVEL,
      false,
      EHEObjectives::GHaveGoodResearchLevel() );

   GManager::RegisterObjective( EHEObjectives::Ids::MILITARY_ACCESS,
      EHEObjectives::Names::MILITARY_ACCESS,
      false,
      EHEObjectives::GHaveMilitaryAccess() );

   GManager::RegisterObjective( EHEObjectives::Ids::ENEMIES_LOW_STABILITY,
      EHEObjectives::Names::ENEMIES_LOW_STABILITY,
      false,
      EHEObjectives::GEnemiesLowStability() );

   GManager::RegisterObjective( EHEObjectives::Ids::ENEMIES_NO_MONEY,
      EHEObjectives::Names::ENEMIES_NO_MONEY,
      false,
      EHEObjectives::GEnemiesNoMoney() );

   GManager::RegisterObjective( EHEObjectives::Ids::ENEMIES_NO_MILITARY_FORCES,
      EHEObjectives::Names::ENEMIES_NO_MILITARY_FORCES,
      false,
      EHEObjectives::GEnemiesNoMilitary() );

   GManager::RegisterObjective( EHEObjectives::Ids::LOW_CORRUPTION_LEVEL,
      EHEObjectives::Names::LOW_CORRUPTION_LEVEL,
      false,
      EHEObjectives::GLowCorruptionLevel() );


}

/*!
* Register actions in EHE
**/
void GServer::RegisterActions()
{
   GManager::RegisterAction( EHEActions::Ids::RAISE_TAXES,
      EHEActions::Names::RAISE_TAXES,
      EHEActions::GRaiseTaxes() );

   GManager::RegisterAction( EHEActions::Ids::LOWER_TAXES,
      EHEActions::Names::LOWER_TAXES,
      EHEActions::GLowerTaxes() );

   GManager::RegisterAction( EHEActions::Ids::RAISE_INTEREST_LEVEL,
      EHEActions::Names::RAISE_INTEREST_LEVEL,
      EHEActions::GRaiseInterestRate() );

   GManager::RegisterAction( EHEActions::Ids::LOWER_INTEREST_LEVEL,
      EHEActions::Names::LOWER_INTEREST_LEVEL,
      EHEActions::GLowerInterestRate() );

   GManager::RegisterAction( EHEActions::Ids::ENROLL_INFANTRY,
      EHEActions::Names::ENROLL_INFANTRY,
      EHEActions::GEnrollMen() );

   GManager::RegisterAction( EHEActions::Ids::INCREASE_PRODUCTION,
      EHEActions::Names::INCREASE_PRODUCTION,
      EHEActions::GIncreaseProduction() );

   GManager::RegisterAction( EHEActions::Ids::WAR_DECLARATION,
      EHEActions::Names::WAR_DECLARATION,
      EHEActions::GDeclareWar() );

   GManager::RegisterAction( EHEActions::Ids::TRAIN_SPECOPS,
      EHEActions::Names::TRAIN_SPECOPS,
      EHEActions::GTrainSpecops() );

   GManager::RegisterAction( EHEActions::Ids::DISBAND_NUKE,
      EHEActions::Names::DISBAND_NUKE,
      EHEActions::GDisbandNuke() );

   GManager::RegisterAction( EHEActions::Ids::LAUNCH_NUKE,
      EHEActions::Names::LAUNCH_NUKE,
      EHEActions::GLaunchNuke() );

   GManager::RegisterAction( EHEActions::Ids::NUCLEARIZE_COUNTRY,
      EHEActions::Names::NUCLEARIZE_COUNTRY,
      EHEActions::GNuclearizeCountry() );

   GManager::RegisterAction( EHEActions::Ids::RAISE_RESOURCE_TAXES,
      EHEActions::Names::RAISE_RESOURCE_TAXES,
      EHEActions::GRaiseResourceTaxes() );

   GManager::RegisterAction( EHEActions::Ids::LOWER_RESOURCE_TAXES,
      EHEActions::Names::LOWER_RESOURCE_TAXES,
      EHEActions::GLowerResourceTaxes() );

   GManager::RegisterAction( EHEActions::Ids::DECLARE_MARTIAL_LAW,
      EHEActions::Names::DECLARE_MARTIAL_LAW,
      EHEActions::GDeclareMartialLaw() );

   GManager::RegisterAction( EHEActions::Ids::CHANGE_TO_DEMOCRACY,
      EHEActions::Names::CHANGE_TO_DEMOCRACY,
      EHEActions::GChangeToDemocracy() );

   GManager::RegisterAction( EHEActions::Ids::CHANGE_TO_COMMUNISM,
      EHEActions::Names::CHANGE_TO_COMMUNISM,
      EHEActions::GChangeToCommunism() );

   GManager::RegisterAction( EHEActions::Ids::CHANGE_TO_DICTATORSHIP,
      EHEActions::Names::CHANGE_TO_DICTATORSHIP,
      EHEActions::GChangeToDictatorship() );

   GManager::RegisterAction( EHEActions::Ids::CHANGE_TO_MONARCHY,
      EHEActions::Names::CHANGE_TO_MONARCHY,
      EHEActions::GChangeToMonarchy() );

   GManager::RegisterAction( EHEActions::Ids::CHANGE_TO_THEOCRACY,
      EHEActions::Names::CHANGE_TO_THEOCRACY,
      EHEActions::GChangeToTheocracy() );

   GManager::RegisterAction( EHEActions::Ids::GIVE_BETTER_STATUS_REL_LANG,
      EHEActions::Names::GIVE_BETTER_STATUS_REL_LANG,
      EHEActions::GImproveStatusRelLang() );

   GManager::RegisterAction( EHEActions::Ids::GIVE_WORSE_STATUS_REL_LANG,
      EHEActions::Names::GIVE_WORSE_STATUS_REL_LANG,
      EHEActions::GDecreaseStatusRelLang() );

   GManager::RegisterAction( EHEActions::Ids::LEGAL_FREEDOM_SPEECH,
      EHEActions::Names::LEGAL_FREEDOM_SPEECH,
      EHEActions::GLegalFreedomOfSpeech() );

   GManager::RegisterAction( EHEActions::Ids::ILLEGAL_FREEDOM_SPEECH,
      EHEActions::Names::ILLEGAL_FREEDOM_SPEECH,
      EHEActions::GIllegalFreedomOfSpeech() );

   GManager::RegisterAction( EHEActions::Ids::LEGAL_FREEDOM_DEMONSTRATION,
      EHEActions::Names::LEGAL_FREEDOM_DEMONSTRATION,
      EHEActions::GLegalFreedomOfDemonstration() );

   GManager::RegisterAction( EHEActions::Ids::ILLEGAL_FREEDOM_DEMONSTRATION,
      EHEActions::Names::ILLEGAL_FREEDOM_DEMONSTRATION,
      EHEActions::GIllegalFreedomOfDemonstration() );

   GManager::RegisterAction( EHEActions::Ids::ILLIMITED_NB_CHILDREN,
      EHEActions::Names::ILLIMITED_NB_CHILDREN,
      EHEActions::GIllimitedNbChildren() );

   GManager::RegisterAction( EHEActions::Ids::LIMITED_NB_CHILDREN,
      EHEActions::Names::LIMITED_NB_CHILDREN,
      EHEActions::GLimitedNbChildren() );

   GManager::RegisterAction( EHEActions::Ids::LEGAL_ABORTION,
      EHEActions::Names::LEGAL_ABORTION,
      EHEActions::GLegalAbortion() );

   GManager::RegisterAction( EHEActions::Ids::ILLEGAL_ABORTION,
      EHEActions::Names::ILLEGAL_ABORTION,
      EHEActions::GIllegalAbortion() );

   GManager::RegisterAction( EHEActions::Ids::LEGAL_CONTRACEPTION,
      EHEActions::Names::LEGAL_CONTRACEPTION,
      EHEActions::GLegalContraception() );

   GManager::RegisterAction( EHEActions::Ids::ILLEGAL_CONTRACEPTION,
      EHEActions::Names::LIMITED_NB_CHILDREN,
      EHEActions::GIllegalContraception() );

   GManager::RegisterAction( EHEActions::Ids::ALLIANCE,
      EHEActions::Names::ALLIANCE,
      EHEActions::GAlliance() );

   GManager::RegisterAction( EHEActions::Ids::ANNEXATION,
      EHEActions::Names::ANNEXATION,
      EHEActions::GAnnexation() );

   GManager::RegisterAction( EHEActions::Ids::FREE_REGION,
      EHEActions::Names::FREE_REGION,
      EHEActions::GFreeRegion() );

   GManager::RegisterAction( EHEActions::Ids::MILITARY_TRESPASSING_RIGHT,
      EHEActions::Names::MILITARY_TRESPASSING_RIGHT,
      EHEActions::GMilitaryTrespassingRights() );

   GManager::RegisterAction( EHEActions::Ids::CULTURAL_EXCHANGES,
      EHEActions::Names::CULTURAL_EXCHANGES,
      EHEActions::GCulturalExchanges() );

   GManager::RegisterAction( EHEActions::Ids::NOBLE_CAUSE,
      EHEActions::Names::NOBLE_CAUSE,
      EHEActions::GNobleCause() );

   GManager::RegisterAction( EHEActions::Ids::RESEARCH_PARTNERSHIP,
      EHEActions::Names::RESEARCH_PARTNERSHIP,
      EHEActions::GResearchPartnership() );

   GManager::RegisterAction( EHEActions::Ids::HUMAN_DEVELOPMENT_COLLABORATION,
      EHEActions::Names::HUMAN_DEVELOPMENT_COLLABORATION,
      EHEActions::GHumanDevelopmentCollaboration() );

   GManager::RegisterAction( EHEActions::Ids::ECONOMIC_PARTNERSHIP,
      EHEActions::Names::ECONOMIC_PARTNERSHIP,
      EHEActions::GEconomicPartnership() );

   GManager::RegisterAction( EHEActions::Ids::COMMON_MARKET,
      EHEActions::Names::COMMON_MARKET,
      EHEActions::GCommonMarket() );

   GManager::RegisterAction( EHEActions::Ids::ASSUME_FOREIGN_DEBT_GIVE,
      EHEActions::Names::ASSUME_FOREIGN_DEBT_GIVE,
      EHEActions::GAssumeForeignDebtGive() );

   GManager::RegisterAction( EHEActions::Ids::ASSUME_FOREIGN_DEBT_RECEIVE,
      EHEActions::Names::ASSUME_FOREIGN_DEBT_RECEIVE,
      EHEActions::GAssumeForeignDebtReceive() );

   GManager::RegisterAction( EHEActions::Ids::ECONOMIC_AID_GIVE,
      EHEActions::Names::ECONOMIC_AID_GIVE,
      EHEActions::GEconomicAidGive() );

   GManager::RegisterAction( EHEActions::Ids::ECONOMIC_AID_RECEIVE,
      EHEActions::Names::ECONOMIC_AID_RECEIVE,
      EHEActions::GEconomicAidReceive() );

   GManager::RegisterAction( EHEActions::Ids::ECONOMIC_EMBARGO,
      EHEActions::Names::ECONOMIC_EMBARGO,
      EHEActions::GEconomicEmbargo() );

   GManager::RegisterAction( EHEActions::Ids::WEAPON_TRADE,
      EHEActions::Names::WEAPON_TRADE,
      EHEActions::GWeaponTrade() );

   GManager::RegisterAction( EHEActions::Ids::WEAPON_TRADE_EMBARGO,
      EHEActions::Names::WEAPON_TRADE_EMBARGO,
      EHEActions::GWeaponTradeEmbargo() );

   GManager::RegisterAction( EHEActions::Ids::RESEARCH_AMDS,
      EHEActions::Names::RESEARCH_AMDS,
      EHEActions::GResearchAMDS() );

   GManager::RegisterAction( EHEActions::Ids::TRAIN_INFANTRY,
      EHEActions::Names::TRAIN_INFANTRY,
      EHEActions::GTrainInfantry() );

   GManager::RegisterAction( EHEActions::Ids::DISBAND_INFANTRY,
      EHEActions::Names::DISBAND_INFANTRY,
      EHEActions::GDisbandInfantry() );

   GManager::RegisterAction( EHEActions::Ids::BUILD_AIR_UNITS,
      EHEActions::Names::BUILD_AIR_UNITS,
      EHEActions::GBuildAirForces() );

   GManager::RegisterAction( EHEActions::Ids::TRAIN_AIR_UNITS,
      EHEActions::Names::TRAIN_AIR_UNITS,
      EHEActions::GTrainAirForces() );

   GManager::RegisterAction( EHEActions::Ids::DISBAND_AIR_UNITS,
      EHEActions::Names::DISBAND_AIR_UNITS,
      EHEActions::GDisbandAirForces() );

   GManager::RegisterAction( EHEActions::Ids::BUILD_NAVAL_UNITS,
      EHEActions::Names::BUILD_NAVAL_UNITS,
      EHEActions::GBuildNavalForces() );

   GManager::RegisterAction( EHEActions::Ids::TRAIN_NAVAL_UNITS,
      EHEActions::Names::TRAIN_NAVAL_UNITS,
      EHEActions::GTrainNavalForces() );

   GManager::RegisterAction( EHEActions::Ids::DISBAND_NAVAL_UNITS,
      EHEActions::Names::DISBAND_NAVAL_UNITS,
      EHEActions::GDisbandAirForces() );

   GManager::RegisterAction( EHEActions::Ids::BUILD_GROUND_UNITS,
      EHEActions::Names::BUILD_GROUND_UNITS,
      EHEActions::GBuildGroundForces() );

   GManager::RegisterAction( EHEActions::Ids::TRAIN_GROUND_UNITS,
      EHEActions::Names::TRAIN_GROUND_UNITS,
      EHEActions::GTrainGroundForces() );

   GManager::RegisterAction( EHEActions::Ids::DISBAND_GROUND_UNITS,
      EHEActions::Names::DISBAND_GROUND_UNITS,
      EHEActions::GDisbandGroundForces() );

   GManager::RegisterAction( EHEActions::Ids::PRIVATE_CONTROL_RESOURCE,
      EHEActions::Names::PRIVATE_CONTROL_RESOURCE,
      EHEActions::GPrivateResourceControl() );

   GManager::RegisterAction( EHEActions::Ids::PUBLIC_CONTROL_RESOURCE,
      EHEActions::Names::PUBLIC_CONTROL_RESOURCE,
      EHEActions::GPublicResourceControl() );

   GManager::RegisterAction( EHEActions::Ids::ASSASSINATION,
      EHEActions::Names::ASSASSINATION,
      EHEActions::GAssassination() );

   GManager::RegisterAction( EHEActions::Ids::ESPIONAGE_MILITARY,
      EHEActions::Names::ESPIONAGE_MILITARY,
      EHEActions::GEspionageMilitary() );

   GManager::RegisterAction( EHEActions::Ids::ESPIONAGE_CIVILIAN,
      EHEActions::Names::ESPIONAGE_CIVILIAN,
      EHEActions::GEspionageCivilian() );

   GManager::RegisterAction( EHEActions::Ids::SABOTAGE_MILITARY,
      EHEActions::Names::SABOTAGE_MILITARY,
      EHEActions::GSabotageMilitary() );

   GManager::RegisterAction( EHEActions::Ids::SABOTAGE_CIVILIAN,
      EHEActions::Names::SABOTAGE_CIVILIAN,
      EHEActions::GSabotageCivilian() );

   GManager::RegisterAction( EHEActions::Ids::TERRORISM,
      EHEActions::Names::TERRORISM,
      EHEActions::GTerrorism() );

   GManager::RegisterAction( EHEActions::Ids::COUP_ETAT,
      EHEActions::Names::COUP_ETAT,
      EHEActions::GCoupEtat() );

   GManager::RegisterAction( EHEActions::Ids::CREATE_SPECOPS,
      EHEActions::Names::CREATE_SPECOPS,
      EHEActions::GCreateSpecOpsCells() );

   GManager::RegisterAction( EHEActions::Ids::DISBAND_SPECOPS,
      EHEActions::Names::DISBAND_SPECOPS,
      EHEActions::GDisbandSpecOpsCells() );

   GManager::RegisterAction( EHEActions::Ids::CLOSE_EMIGRATION,
      EHEActions::Names::CLOSE_EMIGRATION,
      EHEActions::GCloseEmigration() );

   GManager::RegisterAction( EHEActions::Ids::CLOSE_IMMIGRATION,
      EHEActions::Names::CLOSE_IMMIGRATION,
      EHEActions::GCloseImmigration() );

   GManager::RegisterAction( EHEActions::Ids::OPEN_EMIGRATION,
      EHEActions::Names::OPEN_EMIGRATION,
      EHEActions::GOpenEmigration() );

   GManager::RegisterAction( EHEActions::Ids::OPEN_IMMIGRATION,
      EHEActions::Names::OPEN_IMMIGRATION,
      EHEActions::GOpenImmigration() );

   GManager::RegisterAction( EHEActions::Ids::TRADE_IN_REGION,
      EHEActions::Names::TRADE_IN_REGION,
      EHEActions::GTradeInRegion() );

   GManager::RegisterAction( EHEActions::Ids::TRADE_IN_RESEARCH,
      EHEActions::Names::TRADE_IN_RESEARCH,
      EHEActions::GTradeInResearch() );

}

GString GServer::ConsoleServerCommandsHandler(const GString & in_sCommand, const vector<GString>& in_vArgs)
{
   if(in_sCommand == L"name")
   {
      g_Joshua.ServerName(in_vArgs[0] );
      return GString(L"Changed server name to: ") + in_vArgs[0];
   }
   else if(in_sCommand == L"version")
   {
      return g_Joshua.CurrentMod().m_sName + " V" + g_Joshua.CurrentMod().m_sVersion;         
   }
   else if(in_sCommand == L"new")
   {
      RequestNewGame(c_sDatabaseLocation);
   }
   else if(in_sCommand == L"newfrom")
   {
      RequestNewGame(in_vArgs[0] );
   }
   else if(in_sCommand == L"load")
   {
      RequestLoadGame(in_vArgs[0] );
   }
   else if(in_sCommand == L"save")
   {
      RequestSaveGame(in_vArgs[0], 0 );
   }
   else if(in_sCommand == "kick")
   {
      if(in_vArgs[0].ToINT32() > 0)
         RequestSomeKickingAction(in_vArgs[0].ToINT32());
      else
         RequestSomeKickingAction(in_vArgs[0]);
   }
   else if(in_sCommand == "ban")
   {
      if(in_vArgs[0].ToINT32() > 0)
         RequestSomeBanningAction(in_vArgs[0].ToINT32());
      else
         RequestSomeBanningAction(in_vArgs[0]);
   }
   else if(in_sCommand == "clear_ban_list")
   {
      // Clear list of banned people
      g_Joshua.ClearBanList();
   }
   else if(in_sCommand == "set_admin_country")
   {
       INT32 l_iNewAdminCountryID      = in_vArgs[0].ToINT32();
       SDK::GPlayer* l_pNewAdminPlayer = g_Joshua.ActivePlayerByModID(l_iNewAdminCountryID);
       ChangeAdminPlayer(l_pNewAdminPlayer);
   }
   else if(in_sCommand == "set_admin_player")
   {
       INT32 l_iNewAdminID             = in_vArgs[0].ToINT32();
       SDK::GPlayer* l_pNewAdminPlayer = g_Joshua.ActivePlayer(l_iNewAdminID);
       ChangeAdminPlayer(l_pNewAdminPlayer);
   }
   else if(in_sCommand == "print_players")
   {
       GString l_sPlayersList;
       for(SDK::GPlayers::const_iterator l_HumanPlayersIt = g_Joshua.HumanPlayers().begin();
           l_HumanPlayersIt != g_Joshua.HumanPlayers().end();
           ++l_HumanPlayersIt)
		{
			SDK::GPlayer* l_pPlayer = l_HumanPlayersIt->second;
            const ENTITY_ID l_iCountryID = l_pPlayer->ModID();
            if(l_pPlayer->PlayerStatus() == SDK::PLAYER_STATUS_ACTIVE && 
			   l_iCountryID > 0)
            {
                g_Joshua.Log(L"Player ID " + GString(l_pPlayer->Id()) + L": " +
                             L"Name " + l_pPlayer->Name() + L"; " +
                             m_DAL.CountryData(l_iCountryID)->NameAndIDForLog());
            }
		}
   }
   else if(in_sCommand == L"print_military_control")
   {
       const ENTITY_ID l_iCountryID = in_vArgs.front().ToINT32();
       if(0 < l_iCountryID && l_iCountryID <= m_DAL.NbCountry())
       {
           g_Joshua.Log(m_DAL.CountryData(l_iCountryID)->NameAndIDForLog() + L":");

           const set<UINT32> l_vMilitaryControl = m_DAL.CountryMilitaryControl(l_iCountryID);
           for(auto it = l_vMilitaryControl.cbegin(); it != l_vMilitaryControl.cend(); ++it)
           {
               const GVector2D<REAL32> l_Location = m_DCL.UnitMover().RegionLocation(*it);
               const GRegion* const l_pRegion = m_DAL.GetGRegion(*it);
               g_Joshua.Log(m_DAL.RegionNameAndIDForLog(*it) + L": " +
                            L"Location " + GString(l_Location.x) + L", " + GString(l_Location.y) + L", " +
                            L"political control " + m_DAL.CountryData(l_pRegion->OwnerId())->NameAndIDForLog() + L", " +
                            L"historical claim " + m_DAL.CountryData(l_pRegion->HistoricalClaim())->NameAndIDForLog());
           }
       }
   }
   else if(in_sCommand == L"print_foreign_presence")
   {
       const ENTITY_ID l_iCountryID = in_vArgs.front().ToINT32();
       if(0 < l_iCountryID && l_iCountryID <= m_DAL.NbCountry())
       {
           g_Joshua.Log(L"Foreign presence in " + m_DAL.CountryData(l_iCountryID)->NameAndIDForLog() + L":");

           const set<UINT32>& l_vRegions = m_DAL.CountryPoliticalControl(l_iCountryID);
           for(auto l_RegionIt = l_vRegions.cbegin(); l_RegionIt != l_vRegions.cend(); ++l_RegionIt)
           {
               const set<UINT32>& l_vUnitGroups = m_DCL.UnitMover().UnitGroupsInsideRegion(*l_RegionIt);
               for(auto l_UnitGroupIt = l_vUnitGroups.cbegin(); l_UnitGroupIt != l_vUnitGroups.cend(); ++l_UnitGroupIt)
               {
                   const SDK::Combat::GUnitGroup* const l_pGroup = g_Joshua.UnitManager().UnitGroup(*l_UnitGroupIt);
                   const ENTITY_ID l_iGroupOwner = l_pGroup->OwnerId();

                   if(l_iGroupOwner != l_iCountryID)
                       g_Joshua.Log(m_DAL.RegionNameAndIDForLog(*l_RegionIt) + L": " +
                                    m_DAL.CountryData(l_iGroupOwner)->NameAndIDForLog() + L", "
                                    L"location " + GString(l_pGroup->Position().x) + L", " + GString(l_pGroup->Position().y));
               }
           }
       }
   }
   else if(in_sCommand == L"print_regions_in_area")
   {
       const REAL32 l_fMinLat  = in_vArgs[0].ToREAL32();
       const REAL32 l_fMaxLat  = in_vArgs[1].ToREAL32();
       const REAL32 l_fMinLong = in_vArgs[2].ToREAL32();
       const REAL32 l_fMaxLong = in_vArgs[3].ToREAL32();

       // For GRectangle, lowest coordinates are at the top-left
       const GRectangle<REAL32> l_Rectangle(l_fMinLong, l_fMinLat, l_fMaxLong - l_fMinLong, l_fMaxLat - l_fMinLat);

       // 1-based vector
       const vector<GRegionControl> l_vRegionControl = m_DAL.RegionControlArray();
       for(auto it = l_vRegionControl.cbegin() + 1; it < l_vRegionControl.cend(); ++it)
       {
           const UINT32 l_iRegionID = distance(l_vRegionControl.cbegin(), it);
           const GVector2D<REAL32> l_Location = m_DCL.UnitMover().RegionLocation(l_iRegionID);
           if(l_Rectangle.Contains(l_Location))
           {
               const GRegion* const l_pRegion = m_DAL.GetGRegion(l_iRegionID);
               g_Joshua.Log(m_DAL.RegionNameAndIDForLog(l_iRegionID) + L": " +
                            L"Location " + GString(l_Location.x) + L", " + GString(l_Location.y) + L", " +
                            L"political control " + m_DAL.CountryData(l_pRegion->OwnerId())->NameAndIDForLog() + L", " +
                            L"military control " + m_DAL.CountryData(l_pRegion->OwnerMilitaryId())->NameAndIDForLog() + L", " +
                            L"historical claim " + m_DAL.CountryData(l_pRegion->HistoricalClaim())->NameAndIDForLog());
           }
       }
   }
   else if(in_sCommand == L"declare_peace")
   {
       const auto l_mWars = m_DAL.CurrentWars();
       for(auto it = l_mWars.cbegin(); it != l_mWars.cend(); ++it)
       {
           const GWar& l_War = it->second;
           m_DCL.ChangeOpinionOnWar(l_War.MasterAttacking(), it->first, true);
           m_DCL.ChangeOpinionOnWar(l_War.MasterDefending(), it->first, true);
       }
   }
#ifdef GOLEM_DEBUG
   else if(in_sCommand == L"build")
   {
      //Build units
      UINT32  l_iCountryID = (UINT32) in_vArgs[0].ToINT32();
      UINT32  l_iDesignID  = (UINT32) in_vArgs[1].ToINT32();
      UINT32  l_iQty       = (UINT32) in_vArgs[2].ToINT32();
      m_DCL.BuildUnits(l_iCountryID,l_iCountryID,l_iDesignID,l_iQty,0,EUnitProductionPriority::Low);
   }
   else if(in_sCommand == L"start_game")
   {
      if(!GameStarted())
      {
         return StartGame() ? L"starting..." : L"error : no game data loaded";
      }
      else
         return L"already started";
   }
   else if(in_sCommand == L"givemoney")
   {
      g_ServerDCL.GiveMoney(in_vArgs[0].ToINT32(), in_vArgs[1].ToREAL64());
   }
   else if(in_sCommand == L"bombard")
   {
      UINT32  l_iCountry1  = (UINT32) in_vArgs[0].ToINT32();
      UINT32  l_iCountry2  = (UINT32) in_vArgs[1].ToINT32();

      //Find the bombardment points of the target country
      vector<UINT32> l_vBombardableRegions = g_ServerDAL.BombardableRegions(l_iCountry2);

      //Find unit group that can bombard given a country
      vector<SDK::Combat::GUnitGroup*> l_vGroupThatCanBombard = g_ServerDAL.UnitGroupsThatCanBombard(l_iCountry1);


      //Pick the 1st region
      if(l_vBombardableRegions.size() && l_vGroupThatCanBombard.size())
      {
         g_ServerDCL.NavalUnitsBombardRegion( l_vGroupThatCanBombard,l_vBombardableRegions[0]);
      }
      

   }
   else if(in_sCommand == L"fight")
   {
      //Create an arena
      GVector2D<REAL32> l_Pos(0,0);
      SDK::Combat::GArena* l_pArena = g_CombatManager.CreateArena(g_Joshua.UnitManager().NewArenaInfoID(),l_Pos);
      SP2::GUnitGroup* l_pDefender = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(238);
      SP2::GUnitGroup* l_pAttacker = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(750);
		set<UINT32> l_Attackers;
		l_Attackers.insert(l_pDefender->OwnerId());
      m_DCL.DeclareNewWar(l_Attackers, l_pDefender->OwnerId(), l_pAttacker->OwnerId());
      l_pArena->AddGroup(l_pDefender);
      l_pArena->AddGroup(l_pAttacker);
   }
   else if(in_sCommand == L"status")
   {
      //Change status of the unit
      SDK::Combat::GUnitGroup* l_pUnit = g_Joshua.UnitManager().UnitGroup(in_vArgs[0].ToINT32() );
      EMilitaryStatus::Enum l_eNewStatus = (EMilitaryStatus::Enum) in_vArgs[1].ToINT32();
      m_DCL.UnitMover().ChangeUnitState(l_pUnit, l_eNewStatus);
   }
   else if(in_sCommand == L"declare_war")
   {		
      //Declare war between the 2 countries
		set<UINT32> l_Attackers;
		l_Attackers.insert(in_vArgs[0].ToINT32());
      m_DCL.DeclareNewWar(l_Attackers,in_vArgs[0].ToINT32(),in_vArgs[1].ToINT32());      
   }
   else if(in_sCommand == L"declare_war_2v2")
   {
       set<ENTITY_ID> l_vAttackers;
       const ENTITY_ID l_iMasterAttacker = in_vArgs[0].ToINT32();
       l_vAttackers.insert(l_iMasterAttacker);
       l_vAttackers.insert(in_vArgs[1].ToINT32());
       m_DCL.DeclareNewWar(l_vAttackers, l_iMasterAttacker, in_vArgs[2].ToINT32());

       //m_iNextWarID actually holds the ID of the last-successfully-declared war
       m_DCL.JoinAWar(in_vArgs[3].ToINT32(), m_DAL.m_iNextWarID, 2, false);
   }
	else if(in_sCommand == L"print_wars")
   {		
      	const hash_map<UINT32,GWar>& l_Wars = g_ServerDAL.CurrentWars();
			for(hash_map<UINT32,GWar>::const_iterator it = l_Wars.begin();
					it != l_Wars.end(); it++)
			{
				const GWar& l_CurWar = it->second;
				GString SideA = L"";
				GString SideB = L"";
				for(set<UINT32>::const_iterator it = l_CurWar.AttackingSide().begin();
					 it != l_CurWar.AttackingSide().end(); it++)
				{
					SideA += g_ServerDAL.GetString(g_ServerDAL.CountryData(*it)->NameID()) + L", ";
				}
				g_Joshua.Log(SideA);
				g_Joshua.Log(L" vs ");
				for(set<UINT32>::const_iterator it = l_CurWar.DefendingSide().begin();
					 it != l_CurWar.DefendingSide().end(); it++)
				{
					SideB += g_ServerDAL.GetString(g_ServerDAL.CountryData(*it)->NameID()) + L", ";
				}
				g_Joshua.Log(SideB);
				g_Joshua.Log(L"------------");
			}
   }
   else if(in_sCommand == L"occupy")
   {				
      set<UINT32> l_EmptyListOfCountries;
      m_DCL.OccupyCountry(in_vArgs[0].ToINT32(),in_vArgs[1].ToINT32(),l_EmptyListOfCountries);
   }
   else if(in_sCommand == L"military_removal")
   {		
      //First country ask for second country to leave
      set<UINT32> l_SideA;
      set<UINT32> l_SideB;
      set<UINT32> l_SidePressure;
      l_SideA.insert(in_vArgs[0].ToINT32());
      l_SideB.insert(in_vArgs[1].ToINT32());
      UINT32 l_iConditions[ETreatyConditions::ItemCount];
      Memory::Fill<UINT32>(l_iConditions,0,ETreatyConditions::ItemCount);
      GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::RequestMilitaryPresenceRemoval,g_ServerDAL.GetString(g_ServerDAL.CountryData(in_vArgs[0].ToINT32())->NameID()));
      UINT32 l_iTreatyID = m_DCL.CreateNewTreaty(in_vArgs[0].ToINT32(),l_SideA,l_SideB,l_SidePressure,ETreatyType::RequestMilitaryPresenceRemoval,true,l_sTreatyName,l_iConditions);
      m_DCL.JoinTreaty(in_vArgs[1].ToINT32(),l_iTreatyID,2);
   }
   else if(in_sCommand == L"embargo")
   {		
      //First country make a trade embargo against second country
      set<UINT32> l_SideA;
      set<UINT32> l_SideB;
      set<UINT32> l_SidePressure;
      l_SideA.insert(in_vArgs[0].ToINT32());
      l_SideB.insert(in_vArgs[1].ToINT32());
      UINT32 l_iConditions[ETreatyConditions::ItemCount];
      Memory::Fill<UINT32>(l_iConditions,0,ETreatyConditions::ItemCount);
      GString l_sTreatyName = g_ServerDAL.TreatyName(ETreatyType::EconomicEmbargo,g_ServerDAL.GetString(g_ServerDAL.CountryData(in_vArgs[0].ToINT32())->NameID()));
      m_DCL.CreateNewTreaty(in_vArgs[0].ToINT32(),l_SideA,l_SideB,l_SidePressure,ETreatyType::EconomicEmbargo,true,l_sTreatyName,l_iConditions);
   }   
   else if(in_sCommand == "annex")
   {
      UINT32 l_iConquering = in_vArgs[0].ToINT32();
      UINT32 l_iConquered = in_vArgs[1].ToINT32();
      const set<UINT32>& l_MilControl = g_ServerDAL.CountryMilitaryControl(l_iConquering);
      for(set<UINT32>::const_iterator i = l_MilControl.begin();i != l_MilControl.end();++ i)
      {
         if(m_DAL.RegionControl(*i).m_iPolitical == l_iConquered)
         {
            m_DCL.AnnexRegion(l_iConquering, *i);
         }
      }
   }
   else if(in_sCommand == "print_relations")
   {
      UINT32 l_iCountry = in_vArgs[0].ToINT32();
      g_Joshua.Log(g_ServerDAL.CountryData(l_iCountry)->NameAndIDForLog() + L": ");

      multimap<REAL32, ENTITY_ID> l_mRelationsMap;
      for(UINT32 i=1; i<= (UINT32)g_ServerDAL.NbCountry(); i++)
      {
         l_mRelationsMap.insert(make_pair<REAL32, ENTITY_ID>(g_ServerDAL.RelationBetweenCountries(l_iCountry,i), i));
      }

      for(multimap<REAL32, ENTITY_ID>::const_reverse_iterator l_Rit = l_mRelationsMap.crbegin();
          l_Rit != l_mRelationsMap.crend();
          ++l_Rit)
      {
         if(l_Rit->second != l_iCountry)
            g_Joshua.Log(g_ServerDAL.CountryData(l_Rit->second)->NameAndIDForLog() + L": " +
                         GString::FormatNumber(l_Rit->first, 1));
      }
   }
   else if(in_sCommand == "sell_units")
   {
      g_Joshua.Log(L"*** Selling units ***");
      for(UINT32 i=1; i<= (UINT32)g_ServerDAL.NbCountry(); i++)
      {
         REAL64 l_fBudgetRevenues = g_ServerDAL.CountryData(i)->BudgetRevenues();
         REAL64 l_fBudgetExpenses = g_ServerDAL.CountryData(i)->BudgetExpenses();

         multimap<REAL32,UINT32> l_UnitsPossibleToSale;
         //Make a list of units that may be sold
         const set<UINT32>& l_vUnitGroupsID = g_Joshua.UnitManager().CountryUnitGroups(i);
         for(set<UINT32>::const_iterator l_It = l_vUnitGroupsID.begin();
            l_It != l_vUnitGroupsID.end();
            l_It++)
         {
            SDK::Combat::GUnitGroup* l_pUnitGroup = g_Joshua.UnitManager().UnitGroup((*l_It));
            for(vector<SDK::Combat::GUnit*>::const_iterator l_It2 = l_pUnitGroup->Units().begin();
               l_It2 != l_pUnitGroup->Units().end();
               l_It2++)
            {
               SDK::Combat::GUnit* l_pUnitThatMayBeDisbanded = *l_It2;
               if( ((SP2::GUnitDesign*)l_pUnitThatMayBeDisbanded->Design())->Type()->Category() == EUnitCategory::Infantry ||
                  g_ServerDCL.IsUnitForSale( l_pUnitThatMayBeDisbanded->Id() ) ||
                  g_ServerDCL.UnitMover().IsUnitInTraining( l_pUnitThatMayBeDisbanded->Id() ))
                  continue;
               l_UnitsPossibleToSale.insert( make_pair<REAL32,UINT32>(((SP2::GUnitDesign*)l_pUnitThatMayBeDisbanded->Design())->Cost(),l_pUnitThatMayBeDisbanded->Id())); 
            }
         }	

         if(l_fBudgetExpenses <= l_fBudgetRevenues)
            continue;

         //Now, disband that kind of units until we have a positive budget. We have to disband at least 1 group
         for(multimap<REAL32,UINT32>::iterator it = l_UnitsPossibleToSale.begin();
            it != l_UnitsPossibleToSale.end(); it++)
         {
            //We must calculate the cost of that unit
            REAL32 l_fUpkeepCostOfUnit = g_ServerDCL.UpkeepFeeForSingleUnit((SP2::GUnit*)g_Joshua.UnitManager().Unit(it->second));		
            l_fBudgetExpenses -= l_fUpkeepCostOfUnit;

            g_Joshua.Log(GString(it->second));
            g_ServerDCL.SellUnit(it->second);

            if(l_fBudgetExpenses <= l_fBudgetRevenues)
               break;
         }
      }	
      g_Joshua.Log(L"*** End Selling units ***");
   }
   else if(in_sCommand == L"build_amds")
   {
      ENTITY_ID l_iCountry = in_vArgs[0].ToINT32();
      m_DCL.StartAMDSResearch(l_iCountry);
   }
   else if(in_sCommand == L"print_amds")
   {
      ENTITY_ID l_iCountry = in_vArgs[0].ToINT32();
      const GCountryData* l_pCountryData = m_DAL.CountryData(l_iCountry);
      return l_pCountryData->NameAndIDForLog() + L": " +
             L"AMDS level " + GString::FormatNumber(l_pCountryData->AMDSLevel(), 3);
   }
   else if(in_sCommand == L"force_occupy")
   {
      const ENTITY_ID l_iOccupier = in_vArgs[0].ToINT32();
      const ENTITY_ID l_iTarget   = in_vArgs[1].ToINT32();
      const bool l_bTestConquer   = in_vArgs[2].ToINT32() > 0;

      const set<UINT32>& l_vPoliticalRegions = m_DAL.CountryPoliticalControl(l_iTarget);
      for(set<UINT32>::const_iterator l_It = l_vPoliticalRegions.begin();
          l_It != l_vPoliticalRegions.end();
          ++l_It)
      {
          if(g_ServerDAL.RegionControlArray()[*l_It].m_iMilitary != l_iOccupier)
              m_DCL.ChangeRegionMilitaryControl(*l_It, l_iOccupier, l_bTestConquer);
      }
   }
   else if(in_sCommand == L"research_nuclear")
   {
      const ENTITY_ID l_iCountry = in_vArgs[0].ToINT32();
      m_DCL.StartNuclearResearch(l_iCountry);
   }
   else if(in_sCommand == L"print_nuclear_research")
   {
      const ENTITY_ID l_iCountry = in_vArgs[0].ToINT32();
      const GCountryData* const l_pCountryData = m_DAL.CountryData(l_iCountry);
      GString l_sNuclearResearchString(l_pCountryData->NameAndIDForLog() + L": " + GString(l_pCountryData->NuclearReady()));
      for(INT32 i = EUnitDesignCharacteristics::MissileRange; i <= EUnitDesignCharacteristics::MissileDamage; i++)
      {
          const EUnitDesignCharacteristics::Enum l_eResearchCategory = static_cast<EUnitDesignCharacteristics::Enum>(i);
          l_sNuclearResearchString += L" " + GString(l_pCountryData->ResearchInfo()->m_fMaxValues[EUnitCategory::Nuclear][l_eResearchCategory]);
      }
      return l_sNuclearResearchString;
   }
   else if(in_sCommand == L"force_client")
   {
       set<ENTITY_ID> l_vSideA;
       const ENTITY_ID l_iMaster = in_vArgs[0].ToINT32();
       l_vSideA.insert(l_iMaster);

       set<ENTITY_ID> l_vSideB;
       const ENTITY_ID l_iClient = in_vArgs[1].ToINT32();
       l_vSideB.insert(l_iClient);

       //Needs non-const GString reference for some reason
       GString l_sName(GDataControlLayer::c_sClientStateTreatyPrefix + L" - " + m_DAL.CountryData(l_iMaster)->NameAndIDForLog() + L"-" + m_DAL.CountryData(l_iClient)->NameAndIDForLog());

       const vector<UINT32> l_vConditions(ETreatyConditions::ItemCount, 0);

       m_DCL.CreateNewTreaty(l_iMaster, l_vSideA, l_vSideB, set<ENTITY_ID>(), ETreatyType::MilitaryAccess, true, l_sName, l_vConditions.data());
   }
#endif //#define GOLEM_DEBUG
   return L"";
}

bool GServer::ChangeAdminPlayer(SDK::GPlayer* in_pPlayer)
{
    bool l_bAdminPlayerChanged = false;

    if(in_pPlayer != NULL && g_Joshua.AdminPlayerID() != in_pPlayer->Id())
    {
        g_Joshua.AdminPlayerID(in_pPlayer->Id());
        SendPlayersList();
        l_bAdminPlayerChanged = true;

        g_Joshua.Log(L"Admin player changed to " + in_pPlayer->Name() + L", " +
                     m_DAL.CountryData(in_pPlayer->ModID())->Name());
    }

    return l_bAdminPlayerChanged;
}

/*!
* Initialize the combat engine, register types etc.
* @return true if successfully initialized, false otherwise
**/
bool GServer::CombatEngineInit()
{
   //Register the handler to the SDK::Combat over event
   g_CombatManager.RegisterOnCombatOverHandler((CALLBACK_HANDLER_void_int_pvoid)&SP2::GMilitaryEventHandler::HandleCombatOver, &m_MilitaryGameEventHandler);

   return true;
}

/*!
* Shutdown the SDK::Combat engine
* @return true if successfully shutted down, false otherwise
**/
bool GServer::CombatEngineShutdown()
{
   return true;
}

void GServer::TryStartGame(void)
{
   // Get Player list
   const SDK::GPlayers & l_players = g_Joshua.HumanPlayers();
   SDK::GPlayers::const_iterator l_itrPlayer = l_players.begin();

   bool l_bAllPlayerReady = true;

   while(l_bAllPlayerReady && l_itrPlayer != l_players.end())
   {
      l_bAllPlayerReady = (l_itrPlayer->second->PlayerStatus() == SDK::PLAYER_STATUS_READY);
      l_itrPlayer++;
   }

   // If every player are ready, start the game
   if(l_bAllPlayerReady)
   {
      //1st synchronisation of the country data of each human player, make sure everything is sent
      {
         SDK::GGameEventSPtr l_SyncEvt = CREATE_GAME_EVENT(SP2::Event::GSynchronizeCountryData);
         for(SDK::GPlayers::const_iterator l_HumanPlayersIt = g_Joshua.HumanPlayers().begin() ;
            l_HumanPlayersIt != g_Joshua.HumanPlayers().end();
            l_HumanPlayersIt++)
         {
            if(l_HumanPlayersIt->second->PlayerStatus() ==  SDK::PLAYER_STATUS_READY)
            {
               //Check if the country data of that human player must be synchronized, if so, send it
               GCountryData* l_pCountryData = g_ServerDAL.CountryData(l_HumanPlayersIt->second->ModID());
               gassert(l_pCountryData,"GServer::Iterate, Country Data does not exist for human player");          
               //Make sure everything is sent so set everything dirty
               {
                  l_pCountryData->m_bBudgetDataDirty           = true;
                  l_pCountryData->m_bCovertActionCellsDirty    = true;
                  l_pCountryData->m_bEconomicDataDirty         = true;
                  l_pCountryData->m_bNuclearMissilesDataDirty  = true;
                  l_pCountryData->m_bResourceDataDirty         = true;
                  l_pCountryData->m_bOtherDataDirty            = true;
               }
               l_SyncEvt->m_iSource = SDK::Event::ESpecialTargets::Server;
               l_SyncEvt->m_iTarget = l_HumanPlayersIt->second->Id();
               SP2::Event::GSynchronizeCountryData* l_pEvent = (SP2::Event::GSynchronizeCountryData*)l_SyncEvt.get();
               l_pEvent->CountryData(l_pCountryData);
               g_Joshua.RaiseEvent(l_SyncEvt);
               l_pCountryData->ClearModifications();
            }
         }
      }//end of synchronize country data

      StartGame();
   }
}

void GServer::ActivateReadyHumanPlayers(void)
{

   const SDK::GPlayers & l_players = g_Joshua.HumanPlayers();
   SDK::GPlayers::const_iterator l_itrPlayer = l_players.begin();

   while(l_itrPlayer != l_players.end())
   {
      SDK::GPlayer *l_pPlayer = l_itrPlayer->second;

      // Activate the player that are ready
      if(l_pPlayer->PlayerStatus() == SDK::PLAYER_STATUS_READY)
      {
         // Add this guy to the active list
         g_Joshua.ActivatePlayer(l_pPlayer);

         // Send him initial data
         SendInitialDataToPlayer(l_pPlayer);
      }

      // Iterate
      l_itrPlayer++;
   }
}

void GServer::ActivateAIPlayers(void)
{


   // Iterate over countries and activate an AI for those without players...
   GCountries l_Countries = Countries();
   GCountries::iterator l_itrCountry = l_Countries.begin();

   while(l_itrCountry != l_Countries.end())
   {
      // Try to get the player for this country
      SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayerByModID(l_itrCountry->Id());

      // If the country doesnt have a player create an ai for it
      if(!l_pPlayer)
      {
         //Create a player and a context, assign it a mod id, the Mod Id equals the Country Id
         l_pPlayer =            new SDK::GPlayer() ;
         l_pPlayer->Id         (g_Joshua.NextID() );
         l_pPlayer->ModID      (l_itrCountry->Id());

         //Create an AI Client and associate it with the player
         SDK::GClientAI* l_pAIClient = new SDK::GClientAI();
         l_pAIClient->Id(l_pPlayer->Id());
         l_pPlayer->Client(l_pAIClient,true);

         // Activate thise player
         g_Joshua.ActivatePlayer(l_pPlayer);
      }
      l_itrCountry++;
   }
}

void GServer::SendInitialDataToPlayer(SDK::GPlayer* in_pPlayer)
{
   //! \todo Set an advisor for that player based on the settings asked by the player
   gassert(in_pPlayer->ModID(),"GServer::SendInitialDataToPlayer invalid player, has no mod id");
   g_ServerDAL.CountryData( in_pPlayer->ModID() )->AddAdvisor(
      EAdvisorStatus::Manual,EAdvisorStatus::Manual,EAdvisorStatus::Manual);

   // inform other players that a new one joined
   InformPlayerJoined(in_pPlayer);

   // Send data through a SendData event
   SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GSendData);
   SP2::Event::GSendData* l_pSendData = (SP2::Event::GSendData*)l_Event.get();
   g_ServerDAL.FillInitialData(l_pSendData,in_pPlayer->ModID());
   l_Event->m_iSource    = SDK::Event::ESpecialTargets::Server;
   l_Event->m_iTarget    = in_pPlayer->Id();
   g_Joshua.RaiseEvent(l_Event);

   //Send the initial game objectives
   SDK::GGameEventSPtr l_EvtObj = CREATE_GAME_EVENT(SP2::Event::GSetGameObjectives);
   SP2::Event::GSetGameObjectives* l_pSetGameObj = (SP2::Event::GSetGameObjectives*)l_EvtObj.get();
   l_pSetGameObj->m_iSource = SDK::Event::ESpecialTargets::Server;
   l_pSetGameObj->m_iTarget = in_pPlayer->Id();
   l_pSetGameObj->m_vObjectives = g_ServerDAL.GameObjectives();
   g_Joshua.RaiseEvent(l_EvtObj);
}

void GServer::SendGameStartEvent(INT32 in_pPlayerID)
{
   g_Joshua.Log("   Sending start game event");

   //Broadcast a Start Game Game event to all player
   SDK::GGameEventSPtr l_eventGameStart = CREATE_GAME_EVENT(SP2::Event::GStartGame);
   l_eventGameStart->m_iSource   = SDK::Event::ESpecialTargets::Server;
   l_eventGameStart->m_iTarget   = in_pPlayerID;

   SP2::Event::GStartGame* l_pSG = (SP2::Event::GStartGame*)l_eventGameStart.get();
   l_pSG->StartDate  = g_ServerDAL.ZeroDate().Serialize();
   g_Joshua.RaiseEvent(l_eventGameStart);
}

bool GServer::GameStarted()
{
   return m_bGameStarted;
}

void GServer::GameStarted(bool in_bGameStarted)
{
   if(in_bGameStarted)
   {
      if(GameLoaded() )
      {
         // Start the AI iteration
         g_Joshua.MustIterateAI(true);
         m_bGameStarted = true;
      }
   }
   else
   {
      // Stop AI from iterating
      g_Joshua.MustIterateAI(false);
      g_Joshua.DeleteAllPlayers();
      m_bGameStarted = false;
   }
}

/*!
* Function that will receive the UDP Packets
**/
void GServer::OnUDPPacketReception(int in_iPacketSize,void* in_pPointer)
{
   return;
}

void GServer::SendCombatInfo(UINT32 in_iPlayerID)
{
   //Send all the Combat::Information
   {
      SDK::GGameEventSPtr l_CInfo = CREATE_GAME_EVENT(SDK::Event::GSyncCombatDataFirst);
      l_CInfo->m_iSource                        = SDK::Event::ESpecialTargets::Server;
      l_CInfo->m_iTarget                        = in_iPlayerID;
      SDK::Event::GSyncCombatDataFirst* l_pSync = (SDK::Event::GSyncCombatDataFirst*)l_CInfo.get();

      l_pSync->m_iNbCountries = m_DAL.NbCountry();

      //Added modified designs
      {
         hash_map<UINT32,SDK::Combat::Design::GUnit*>::const_iterator l_It = g_Joshua.UnitManager().RegisteredUnitDesigns().begin();
         while(l_It != g_Joshua.UnitManager().RegisteredUnitDesigns().end())
         {
            l_pSync->m_vAddedModifiedDesigns.insert(make_pair(l_It->second->Id(), l_It->second));
            l_It++;
         }
      }


      {//Send all the units
         hash_map<UINT32,SDK::Combat::GUnit*>::const_iterator l_It = g_Joshua.UnitManager().Units().begin();
         while(l_It != g_Joshua.UnitManager().Units().end())
         {
            l_pSync->m_vAddedModifiedUnits.insert(make_pair(l_It->second->Id(), l_It->second));
            l_It++;
         }
      }

      {//Send all the unit groups
         hash_map<UINT32,SDK::Combat::GUnitGroup*>::const_iterator l_It = g_Joshua.UnitManager().UnitGroups().begin();
         while(l_It != g_Joshua.UnitManager().UnitGroups().end())
         {
            l_pSync->m_vAddedModifiedUnitGroups.insert(make_pair(l_It->second->Id(), l_It->second));
            l_It++;
         }
      }

      {//Send all the arenas info
         hash_map<UINT32,SDK::Combat::GArenaInfo*>::const_iterator l_It = g_Joshua.UnitManager().ArenaInfos().begin();
         while(l_It != g_Joshua.UnitManager().ArenaInfos().end())
         {
            l_pSync->m_vAddedModifiedArenas.insert(make_pair(l_It->second->m_iId, l_It->second));
            l_It++;
         }
      }
      g_Joshua.RaiseEvent(l_CInfo);
   }

   g_Joshua.Log(L"... done (sending combat information)");
}


void GServer::SyncronizeMilitary(INT32 in_iTarget,REAL64 in_fActualClock)
{
   SDK::GGameEventSPtr l_CInfo           = CREATE_GAME_EVENT(SDK::Event::GSyncCombatData);
   l_CInfo->m_iSource                    = SDK::Event::ESpecialTargets::Server;
   l_CInfo->m_iTarget                    = in_iTarget;
   SDK::Event::GSyncCombatData* l_pSync  = (SDK::Event::GSyncCombatData*)l_CInfo.get();
   l_pSync->m_vAddedModifiedUnits        = g_Joshua.UnitManager().ModifiedUnits();
   l_pSync->m_vAddedModifiedDesigns      = g_Joshua.UnitManager().ModifiedUnitDesigns();
   l_pSync->m_vDeletedUnits              = g_Joshua.UnitManager().RemovedUnits();
   l_pSync->m_vAddedModifiedUnitGroups   = g_Joshua.UnitManager().ModifiedUnitGroups();
   l_pSync->m_vDeletedUnitGroups         = g_Joshua.UnitManager().RemovedUnitGroups();
   l_pSync->m_vAddedModifiedArenas       = g_Joshua.UnitManager().ModifiedArenaInfos();
   l_pSync->m_vDeletedArenas             = g_Joshua.UnitManager().RemovedArenaInfos();
   g_Joshua.RaiseEvent(l_CInfo); 


   m_fPreviousSyncDataUpdate = in_fActualClock;
   g_Joshua.UnitManager().ClearModifications();
}

// When saving a game, the DB used it create it is also saved, 
// so that at load time, if DB is not the same, 
// the static part of the game is cleaned 
// and a new game is created with only static data read from the DB.
//
// I.E. In th following scenario
// New game (Data1.DB)
// Save game (Save1.sav)
// New game (Data2.DB)
// Load game (Save1.sav)
//
// The game loaded uses Data1.DB, but Data2.DB is the current DB for the game. 
// Since only variable data is saved, static data (cities, regions & country list) 
// via a new game with a condition: do not create game variable data 
// (units, nukes, ressources, population)
//
// Finally, when loading the clean mechanism only 
// cleans the variable portion of objects, 
// and when creating a new game all data is cleaned (static & variable).

bool GServer::OnSave(GIBuffer& io_Buffer)
{
   // Save egnine info
   io_Buffer << g_Joshua.Clock()
             << m_fLastSpeed
             << g_Joshua.GameTime()
             << g_Joshua.ServerName()
             << g_Joshua.MaxPlayers()
             << g_Joshua.Password()
             << g_Joshua.Private()
             << g_Joshua.MinPing();

   // Save server data
   io_Buffer << m_sCurrentDB
             << m_fPreviousSyncDataUpdate
             << m_fHalfSecondClock
             << m_fFiveSecondsClock
             << m_fNextCombatIterationTime
             << m_iNuclearMissilesSubmarineCountryToReload;

   return true;
}

bool GServer::OnLoad(GOBuffer& io_Buffer)
{
   g_Joshua.GameSpeed(0);

   GString l_SavedDB, l_sServerName, l_sPassword;
   REAL64 l_fLoadClock = g_Joshua.Clock();
   REAL64 l_fSaveTimeClock, l_fGameTime;
   INT32 l_iMaxPlayers, l_iMinPing;
   bool l_bPrivate;

   // Load egnine info
   io_Buffer >> l_fSaveTimeClock
             >> m_fNextGameSpeed
             >> l_fGameTime
             >> l_sServerName
             >> l_iMaxPlayers
             >> l_sPassword
             >> l_bPrivate
             >> l_iMinPing;

   g_Joshua.GameTime(l_fGameTime);
   g_Joshua.ServerName(l_sServerName);
   g_Joshua.MaxPlayers(l_iMaxPlayers);
   g_Joshua.Password(l_sPassword);
   g_Joshua.Private(l_bPrivate);
   g_Joshua.MinPing(l_iMinPing);

   // Load AI
   InitAI();

   // Load server data
   io_Buffer >> l_SavedDB
             >> m_fPreviousSyncDataUpdate
             >> m_fHalfSecondClock
             >> m_fFiveSecondsClock
             >> m_fNextCombatIterationTime
             >> m_iNuclearMissilesSubmarineCountryToReload;

   m_fLoadClockAdjust = l_fLoadClock - l_fSaveTimeClock;

   // Make sure times are relative to current program time
   if(m_fPreviousSyncDataUpdate >= 0)
      m_fPreviousSyncDataUpdate += m_fLoadClockAdjust;

   if(m_fHalfSecondClock >= 0)
      m_fHalfSecondClock += m_fLoadClockAdjust;

   if(m_fFiveSecondsClock >= 0)
      m_fFiveSecondsClock += m_fLoadClockAdjust;

   if(m_fNextCombatIterationTime >= 0)
      m_fNextCombatIterationTime += m_fLoadClockAdjust;

   // Change DB static content only if needed
   if(m_sCurrentDB != l_SavedDB)
   {
      m_bCleanVariableOnly = false;
      m_DAL.DataManager()->CleanGame();

      m_bUpdateStaticOnly = true;
      GDatabase* l_pDB = m_DAL.LoadDatabase(l_SavedDB);
      if(m_DAL.DataManager()->NewGame(l_pDB) == SDK::EGameDataError::NoError)
      {
         m_sCurrentDB = l_SavedDB;
      }
      else
      {
         return false;
      }
   }
   else
   {
      m_bCleanVariableOnly = true;
      m_DAL.DataManager()->CleanGame();
   }

	m_DAL.LoadRelationTable();

   return true;
}

bool GServer::OnNew(GDatabase* in_pDatabase)
{
   if(!UpdateStaticOnly() )
   {
      g_Joshua.GameSpeed(0);
      g_Joshua.GameTime(0);
   }

   // Load AI
   InitAI();

   g_ServerDAL.ZeroDate(g_ServerDAL.ConfigZeroDate() );

   m_fNextGameSpeed = c_fSP2NormalTimeSpeed;

   m_fPreviousSyncDataUpdate = -1;
   m_fHalfSecondClock = -1;
   m_fFiveSecondsClock  = -1;
   m_fNextCombatIterationTime = 0.0f;
   m_iNuclearMissilesSubmarineCountryToReload = 1;

   //Create the country list
   GTable l_Table;
   m_DAL.DBGetCountryList(l_Table);

   m_DAL.NbCountry( (INT16) l_Table.RowCount());

   //Create the player list and country listing
   g_Joshua.Log("Creating the player list and country listing");
   m_Countries.resize(l_Table.RowCount());
   for(UINT32 i = 0; i < l_Table.RowCount(); i++)
   {
      INT32       l_ID              = 0;
      INT32       l_CountryNameID   = 0;
      GSString    l_CountryCode        ;
      GString     l_sCountryName       ;
      GSString    l_sCountryFlag       ;

      //Fetch data from the table
      l_Table.Row(i)->Cell(0)->GetData(l_ID);
      l_Table.Row(i)->Cell(1)->GetData(l_CountryNameID);
      l_Table.Row(i)->Cell(2)->GetData(l_CountryCode);            
      l_Table.Row(i)->Cell(3)->GetData(l_sCountryFlag);            
      m_StrTable.Get_String(l_CountryNameID, l_sCountryName);

      //Fill the country
      GCountry l_CountryTemp;
      l_CountryTemp.Id( (INT16) l_ID);
      l_CountryTemp.Name(l_sCountryName);
      // only the first 3 chars will make the code !
      l_CountryTemp.Code(l_CountryCode.substr(0, 3));
      l_CountryTemp.Flag(l_sCountryFlag);
      m_Countries[i] = l_CountryTemp; //Add the country to the list
   }

   return true;
}

void GServer::OnClean()
{
   if(!CleanVariableOnly() )
   {
      m_Countries.clear();
   }

	//Restart 
	m_iMilitaryRankCountryToIterate = 1;

   // De-activate human players, destroy AI players
   g_Joshua.DeleteAllPlayers();
}

bool GServer::GameLoaded() const
{
   return m_bGameLoaded;
}

void GServer::GameLoaded(bool in_bGameStarted)
{
   m_bGameLoaded = in_bGameStarted;
   if(m_bGameLoaded)
   {
      GameStarted(false);
   }
}

void GServer::RequestNewGame(const GString& in_sNewGameDB)
{
   m_bPendingNew = true;
   m_sNewGameDB = in_sNewGameDB;
   m_iNewsource = 0;
}

void GServer::RequestNewGame(const GString& in_sNewGameDB, UINT32 in_iSource)
{
   m_bPendingNew = true;
   m_sNewGameDB = in_sNewGameDB;
   m_iNewsource = in_iSource;
}

void GServer::RequestLoadGame(const GString& in_sLoadGameFile)
{
   m_bPendingLoad = true;
   m_iLoadsource = 0;
   m_sLoadFileName = in_sLoadGameFile;
}

void GServer::RequestLoadGame(const GString& in_sLoadName, UINT32 in_iSource)
{
   m_bPendingLoad = true;
   m_sLoadName = in_sLoadName;
   m_iLoadsource = in_iSource;
   m_sLoadFileName = g_Joshua.CurrentMod().m_sPath + c_sSaveGameLocation + in_sLoadName + c_sSaveExtServer;
}


void GServer::RequestSomeKickingAction(const GString& in_sPlayerNameToKick)
{
   // Find a player with corresponding name and kick him/her
   SDK::GPlayers::const_iterator l_player = g_Joshua.HumanPlayers().begin();
   while(l_player!=g_Joshua.HumanPlayers().end())
   {
      if(l_player->second->Name() == in_sPlayerNameToKick)
      {
         UINT32 l_playerTokick = l_player->first;
         RequestSomeKickingAction(l_playerTokick);
         break;
      }
      ++l_player;
   }
}

void GServer::RequestSomeKickingAction(UINT32 in_iPlayerID)
{
   g_Joshua.SendDisconnectionReasonToPlayer(in_iPlayerID,SDK::ConnectionLostReason::KickedFromServer);
   // Get out of here
   g_Joshua.RemoveHumanPlayer(in_iPlayerID);
}


void GServer::RequestSomeBanningAction(const GString& in_sPlayerNameToBan)
{
   // Find a player with corresponding name and kick him/her
   SDK::GPlayers::const_iterator l_player = g_Joshua.HumanPlayers().begin();
   while(l_player!=g_Joshua.HumanPlayers().end())
   {
      if(l_player->second->Name() == in_sPlayerNameToBan)
      {
         UINT32 l_playerTokick = l_player->first;
         RequestSomeBanningAction(l_playerTokick);
         break;
      }
      ++l_player;
   }
}

void GServer::RequestSomeBanningAction(UINT32 in_iPlayerID)
{

   // Ban this player
   g_Joshua.Banned(in_iPlayerID);

   // Send disconect reason
   g_Joshua.SendDisconnectionReasonToPlayer(in_iPlayerID,SDK::ConnectionLostReason::BanFromServer);
   
   // Get out of here
   g_Joshua.RemoveHumanPlayer(in_iPlayerID);
}


void GServer::RequestSaveGame(const GString& in_sSaveGameFile)
{
   GSaveRequest l_Request;
   l_Request.m_iSourceID = 0;
   l_Request.m_sSaveName = in_sSaveGameFile;
   l_Request.m_sSaveFile = in_sSaveGameFile;

   m_vPendingSaves.push_back(l_Request);
}

void GServer::RequestSaveGame(const GString& in_sSaveName, UINT32 in_iSource)
{
   m_DCL.CreateSaveDirectory();

   GSaveRequest l_Request;
   l_Request.m_iSourceID = in_iSource;
   l_Request.m_sSaveName = in_sSaveName;
   l_Request.m_sSaveFile = g_Joshua.CurrentMod().m_sPath + c_sSaveGameLocation + in_sSaveName + c_sSaveExtServer;

   m_vPendingSaves.push_back(l_Request);
}


bool GServer::StartGame()
{
   //Raise the StartGameRequest Game Event
   if(GameLoaded() )
   {
      SDK::GGameEventSPtr l_evtGameStartRequest = CREATE_GAME_EVENT(SP2::Event::GStartGameRequest);
      l_evtGameStartRequest->m_iSource = SDK::Event::ESpecialTargets::Server;
      l_evtGameStartRequest->m_iTarget = SDK::Event::ESpecialTargets::Server;
      g_Joshua.RaiseEvent(l_evtGameStartRequest);

      return true;
   }

   return false;
}

UINT32 GServer::IsCountryChoosen(UINT16 in_iCountryID)
{
   // Iterate over human player and make sure no one has this one...
   SDK::GPlayers::const_iterator l_PlayerIt = g_Joshua.HumanPlayers().begin();

   while(l_PlayerIt != g_Joshua.HumanPlayers().end())
   {
      if(l_PlayerIt->second->ModID() == in_iCountryID)
         return l_PlayerIt->first;
      l_PlayerIt++; 
   }

   return 0;
}

void GServer::SaveGame(const GSaveRequest& in_SaveInfo)
{
   SDK::EGameDataError::Enum l_eResult = (SDK::EGameDataError::Enum) c_iGameSaveLoadErrorNotAdmin;
   // Only server itself (via console) and admin player can save the game
   if( (in_SaveInfo.m_iSourceID == 0) || 
       (in_SaveInfo.m_iSourceID == (UINT32) g_Joshua.AdminPlayerID() ) )
   {
      REAL64 l_fCurTime = g_Joshua.TimeCounter().GetPrecisionTime();
      g_Joshua.Log(GString(L"Saving game...") );
      l_eResult = m_DAL.DataManager()->SaveGame(in_SaveInfo.m_sSaveFile, c_iSP2GameType, c_iSP2DataVersion);
      if(l_eResult == SDK::EGameDataError::NoError)
      {
         g_Joshua.Log("...done");
         g_Joshua.Log(GString(L"Saving game time : ") + GString(g_Joshua.TimeCounter().GetPrecisionTime() - l_fCurTime) );
      }
      else
      {
         g_Joshua.Log("Error while saving game");
      }
   }

   // Just to show the client that the game saves...
   Sleep(1000);

   if(in_SaveInfo.m_iSourceID)
   {
      SDK::GGameEventSPtr l_SaveCompleteEvt = CREATE_GAME_EVENT(SP2::Event::GSaveComplete);
      l_SaveCompleteEvt->m_iSource = SDK::Event::ESpecialTargets::Server;
      l_SaveCompleteEvt->m_iTarget = in_SaveInfo.m_iSourceID;
      SP2::Event::GSaveComplete* l_pSaveComplete = (SP2::Event::GSaveComplete*) l_SaveCompleteEvt.get();
      l_pSaveComplete->m_sName = in_SaveInfo.m_sSaveName;
      l_pSaveComplete->m_eResult = l_eResult;

      g_Joshua.RaiseEvent(l_SaveCompleteEvt);
   }
}

void GServer::LoadGame()
{
   SDK::EGameDataError::Enum l_eResult = (SDK::EGameDataError::Enum) c_iGameSaveLoadErrorNotAdmin;
   // Only server itself (via console) and admin player can load the game
   if( (m_iLoadsource == 0) || 
       (m_iLoadsource == (UINT32) g_Joshua.AdminPlayerID() ) )
   {
      g_Joshua.Log(GString(L"Loading game...") );
      l_eResult = m_DAL.DataManager()->LoadGame(m_sLoadFileName, c_iSP2GameType, c_iSP2DataVersion);
      if(l_eResult == SDK::EGameDataError::NoError)
      {
         g_Joshua.Log("...done");
         GameLoaded(true);
      }
      else
      {
         g_Joshua.Log("Error while loading game");
      }

      g_Joshua.UnitManager().ClearModifications();
		GCountryDataItf::m_fHumanDevelopmentAvg = g_ServerDCL.AverageHumanDevelopment();
		

      if(m_iLoadsource)
      {
         SDK::GGameEventSPtr l_LoadCompleteEvt = CREATE_GAME_EVENT(SP2::Event::GLoadComplete);
         l_LoadCompleteEvt->m_iSource = SDK::Event::ESpecialTargets::Server;
         l_LoadCompleteEvt->m_iTarget = m_iLoadsource;
         SP2::Event::GLoadComplete* l_pLoadComplete = (SP2::Event::GLoadComplete*) l_LoadCompleteEvt.get();
         l_pLoadComplete->m_sName = m_sLoadName;
         l_pLoadComplete->m_eResult = l_eResult;

         g_Joshua.RaiseEvent(l_LoadCompleteEvt);
      }
   }

   m_sLoadFileName = "";
}

void GServer::NewGame()
{
   SDK::EGameDataError::Enum l_eResult = (SDK::EGameDataError::Enum) c_iGameSaveLoadErrorNotAdmin;
   // Only server itself (via console) and admin player can create a new game
   if( (m_iNewsource == 0) || 
       (m_iNewsource == (UINT32) g_Joshua.AdminPlayerID() ) )
   {
      // Create new game using DB
      m_bCleanVariableOnly = false;
      m_DAL.DataManager()->CleanGame();

      m_bUpdateStaticOnly = false;
      g_Joshua.Log(GString(L"Creating new game...") );
      GDatabase* l_pDB = m_DAL.LoadDatabase(m_sNewGameDB);
      if(l_pDB)
      {
         l_eResult = m_DAL.DataManager()->NewGame(l_pDB);
         if(l_eResult == SDK::EGameDataError::NoError)
         {
            g_Joshua.Log("...done");
            m_sCurrentDB = m_sNewGameDB;
            GameLoaded(true);
         }
         else
         {
            g_Joshua.Log("Error while creating new game");
         }
      }
      else
      {
         g_Joshua.Log("DB failed to initialize");
         l_eResult = SDK::EGameDataError::NewGameError;
      }

      g_Joshua.UnitManager().ClearModifications();

      if(m_iNewsource)
      {
         SDK::GGameEventSPtr l_NewCompleteEvt = CREATE_GAME_EVENT(SP2::Event::GNewComplete);
         l_NewCompleteEvt->m_iSource = SDK::Event::ESpecialTargets::Server;
         l_NewCompleteEvt->m_iTarget = m_iNewsource;
         SP2::Event::GNewComplete* l_pNewComplete = (SP2::Event::GNewComplete*) l_NewCompleteEvt.get();
         l_pNewComplete->m_sDBFileName = m_sNewGameDB;
         l_pNewComplete->m_eResult = l_eResult;

         g_Joshua.RaiseEvent(l_NewCompleteEvt);
      }
   }

   m_sNewGameDB = "";
}

/*!
* Inform game lobby of player arrival
**/
void GServer::InformPlayerJoined(SDK::GPlayer* in_pPlayer)
{
   const UINT32 c_StrIdPlayerJoined = 102304;
   g_SP2Server->SendChatMessage(in_pPlayer->Id(), SDK::Event::ESpecialTargets::BroadcastHumanPlayers, g_ServerDAL.GetString(c_StrIdPlayerJoined));
}

/*!
* Inform game lobby of player departure
**/
void GServer::InformPlayerLeft(SDK::GPlayer* in_pPlayer)
{
   const UINT32 c_StrIdPlayerLeft = 102305;
   g_SP2Server->SendChatMessage(in_pPlayer->Id(), SDK::Event::ESpecialTargets::BroadcastHumanPlayers, g_ServerDAL.GetString(c_StrIdPlayerLeft));
}


/*!
* Return the AI Aggressiveness, 1 being the most aggressive
**/
REAL32 GServer::AIAggressiveness()
{
	return m_fAIAggressiveness;
}
void GServer::AIAggressiveness(REAL32 in_fAIAggressiveness)
{
	m_fAIAggressiveness = in_fAIAggressiveness;
		
}

void GServer::SendChatMessage(const INT32 in_iSource, const INT32 in_iTarget, const GString& in_sMessage) const
{
    SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SDK::Event::GChatEvent);

    // reinterpret_cast, CREATE_GAME_EVENT doesn't "actually" create a GChatEvent object for some reason
    reinterpret_cast<SDK::Event::GChatEvent*>(l_pEvent.get())->Message(in_sMessage);

    l_pEvent->m_iSource = in_iSource;
    l_pEvent->m_iTarget = in_iTarget;

    g_Joshua.RaiseEvent(l_pEvent);
}

/*!
* Load default server options.
*/
void GServer::InitializeDefaultConfig()
{
    // Default values
    m_bAllowDefenderAttackAttackerTerritory  = true;
    m_bAllowHumanClientStates                = false;
    m_bAllowAIAssumeDebt                     = true;
    m_fAnnexationRelationLossPercent         = 1.f;
    m_fCombatThresholdSquare                 = 0.7f*0.7f;
    m_iCountryNameChangeMode                 = ECountryNameChangeMode::PlayerNameViaCovert;
    m_bDedicatedServerAutosaveToJoshuaFolder = false;
    m_fDedicatedServerAutosavePeriod         = 0.f;
    m_bDisableNuclearOnOccupy                = false;
    m_bDisbandAMDSOnOccupy                   = false;
    m_fTimeOfLastAutosave                    = 0;
    m_fGlobalTaxLimit                        = 1.f;

    m_GlobalTaxSpecials[EGlobalTaxSpecialType::ExportAll]    = 100;
    m_GlobalTaxSpecials[EGlobalTaxSpecialType::MeetDomestic] = 99;
    m_GlobalTaxSpecials[EGlobalTaxSpecialType::ImportAll]    = 98;
    m_GlobalTaxSpecials[EGlobalTaxSpecialType::PrivatizeAll] = 97;

    for(INT32 i=0; i<EGovernmentType::ItemCount; i++)
        m_IncomeTaxLimits[static_cast<EGovernmentType::Enum>(i)] = PersonalTaxes_UpperCap;

    m_bLogBankruptcies              = false;
    m_iMaximumCellsInForeignCountry = 0;

    // Training levels don't exist for nuclear units, so store their upkeep separately from the main map
    for(INT32 i = EUnitCategory::Infantry; i < EUnitCategory::Nuclear; i++)
    {
        EUnitCategory::Enum l_eCategory = static_cast<EUnitCategory::Enum>(i);

        m_mMilitaryUpkeepPercentages[l_eCategory] = map<ETrainingLevel::Enum, REAL32>();
        for(INT32 j = ETrainingLevel::Recruit; j < ETrainingLevel::ItemCount; j++)
            m_mMilitaryUpkeepPercentages[l_eCategory][static_cast<ETrainingLevel::Enum>(j)] = 1.f;

        gassert(m_mMilitaryUpkeepPercentages[l_eCategory].size() == ETrainingLevel::ItemCount, L"Unexpected number of training levels");
    }

    gassert(m_mMilitaryUpkeepPercentages.size() == EUnitCategory::Nuclear, L"Unexpected number of unit categories");

    m_bNavalRuleEnabled                   = true;
    m_fNuclearMissileRangePercentage      = 1.f;
    m_fNuclearUpkeepPercentage            = 1.f;
    m_fOccupiedRegionPercentageForNuclear = 0.f;
    m_fResourceTaxLimit                   = 1.f;
    m_bShowHDIComponents                  = false;
    m_fStabilityAnarchyLowerLimit         = c_fStabilityAnarchyLowerLimit;
    m_fStabilityAnarchyUpperLimit         = c_fStabilityAnarchyHigherLimit;
    m_fTributePercent                     = 0.08f;
}


/*!
* Load server options from SP2-HDM_Config.xml.
*/
void GServer::LoadSP2HDMConfigXML()
{
    const GString  c_sConfigXMLFile("SP2-HDM_Config.xml");

    GFile          l_XMLFile;
	
	//Request the file to the file manager
	if(m_DAL.File(c_sConfigXMLFile,l_XMLFile))
	{
        GString l_sTempName;

		//Extract the file to a temp file on the disk so it can be parsed (xml)
	    if(l_XMLFile.Extract(l_sTempName))
        {
            GXMLParser l_XmlParser;
            GTree<GXMLNode>* l_XMLData = l_XmlParser.Parse(l_sTempName);
	        l_XMLFile.Unextract(); //free the temp file

            if(l_XMLData != NULL)
	        {
                // parse the file
	            for(UINT32 i=0; i<l_XMLData->Root()->NbChilds(); i++)
	            {
		            //Look for OBJECT nodes
		            const GTreeNode<GXMLNode>* objectNode = l_XMLData->Root()->Child(i);

		            const GString l_sElementName  = objectNode->Data().m_sName;
                    const GString l_sElementValue = objectNode->Data().m_value;

                    if(l_sElementName == L"allowAIAssumeDebt")
                    {
                        m_bAllowAIAssumeDebt = (l_sElementValue.ToINT32() != 0);
                        g_Joshua.Log(L"allowAIAssumeDebt: " + GString(m_bAllowAIAssumeDebt));
                    }
                    else if(l_sElementName == L"allowHumanClientStates")
                    {
                        m_bAllowHumanClientStates = (l_sElementValue.ToINT32() != 0);
                        g_Joshua.Log(L"allowHumanClientStates: " + GString(m_bAllowHumanClientStates));
                    }
                    else if(l_sElementName == L"allowDefenderAttackAttackerTerritory")
                    {
                        m_bAllowDefenderAttackAttackerTerritory = (l_sElementValue.ToINT32() != 0);
                        g_Joshua.Log(L"allowDefenderAttackAttackerTerritory: " + GString(m_bAllowDefenderAttackAttackerTerritory));
                    }
                    else if(l_sElementName == L"annexationRelationLossPercent")
                    {
                        m_fAnnexationRelationLossPercent = l_sElementValue.ToREAL32() / 100.f;
                        g_Joshua.Log(L"annexationRelationLossPercent: " + GString(m_fAnnexationRelationLossPercent));
                    }
                    else if(l_sElementName == L"autoCovertMissions")
                    {
                        m_bAutoCovertMissions = (l_sElementValue.ToINT32() != 0);
                        g_Joshua.Log(L"autoCovertMissions: " + GString(m_bAutoCovertMissions));
                    }
                    else if(l_sElementName == L"combatRangeDegrees")
                    {
                        const REAL32 l_fCombatRangeDegrees = l_sElementValue.ToREAL32();
                        m_fCombatThresholdSquare = l_fCombatRangeDegrees*l_fCombatRangeDegrees;
                        g_Joshua.Log(L"combatRangeDegrees: " + GString::FormatNumber(l_fCombatRangeDegrees, 2));
                    }
                    else if(l_sElementName == L"countryNameChangeMode")
                    {
                        m_iCountryNameChangeMode = l_sElementValue.ToINT32();
                        g_Joshua.Log(L"countryNameChangeMode: " + GString(m_iCountryNameChangeMode));
                    }
                    else if(l_sElementName == L"dedicatedServerAutosavePeriod")
                    {
                        m_fDedicatedServerAutosavePeriod = l_sElementValue.ToREAL32();
                        g_Joshua.Log(L"dedicatedServerAutosavePeriod: " + GString::FormatNumber(m_fDedicatedServerAutosavePeriod, 2));
                    }
                    else if(l_sElementName == L"dedicatedServerAutosaveToJoshuaFolder")
                    {
                        m_bDedicatedServerAutosaveToJoshuaFolder = (l_sElementValue.ToINT32() != 0);
                        g_Joshua.Log(L"dedicatedServerAutosaveToJoshuaFolder: " + GString(m_bDedicatedServerAutosaveToJoshuaFolder));
                    }
                    else if(l_sElementName == L"disableNuclearOnOccupy")
                    {
                        m_bDisableNuclearOnOccupy = (l_sElementValue.ToINT32() != 0);
                        g_Joshua.Log(L"disableNuclearOnOccupy: " + GString(m_bDisableNuclearOnOccupy));
                    }
                    else if(l_sElementName == L"disbandAMDSOnOccupy")
                    {
                        m_bDisbandAMDSOnOccupy = (l_sElementValue.ToINT32() != 0);
                        g_Joshua.Log(L"disbandAMDSOnOccupy: " + GString(m_bDisbandAMDSOnOccupy));
                    }
                    else if(l_sElementName == L"globalTaxLimit")
		            {
                        m_fGlobalTaxLimit = l_sElementValue.ToREAL32() / 100.f;
                        g_Joshua.Log(L"globalTaxLimit: " + GString::FormatNumber(m_fGlobalTaxLimit, 3));
		            }
                    else if(l_sElementName == L"globalTaxSpecials")
		            {
                        for(UINT32 j=0; j<objectNode->NbChilds(); j++)
	                    {
                            const GTreeNode<GXMLNode>* l_GovernmentNode = objectNode->Child(j);

		                    const GString l_sName = l_GovernmentNode->Data().m_sName;
                            EGlobalTaxSpecialType::Enum l_eGlobalTaxSpecial = EGlobalTaxSpecialType::ItemCount;
                            if(l_sName == L"exportAll")
                                l_eGlobalTaxSpecial = EGlobalTaxSpecialType::ExportAll;
                            else if(l_sName == L"meetDomestic")
                                l_eGlobalTaxSpecial = EGlobalTaxSpecialType::MeetDomestic;
                            else if(l_sName == L"importAll")
                                l_eGlobalTaxSpecial = EGlobalTaxSpecialType::ImportAll;
                            else if(l_sName == L"privatizeAll")
                                l_eGlobalTaxSpecial = EGlobalTaxSpecialType::PrivatizeAll;

                            m_GlobalTaxSpecials[l_eGlobalTaxSpecial] = l_GovernmentNode->Data().m_value.ToINT32();
                            g_Joshua.Log(L"globalTaxSpecials[" + l_sName + L"]: " +
                                         GString(m_GlobalTaxSpecials[l_eGlobalTaxSpecial]));
                        }
		            }
                    else if(l_sElementName == L"incomeTaxLimits")
                    {
                        for(UINT32 j=0; j<objectNode->NbChilds(); j++)
	                    {
                            const GTreeNode<GXMLNode>* l_GovernmentNode = objectNode->Child(j);

		                    const GString l_sName = l_GovernmentNode->Data().m_sName;
                            EGovernmentType::Enum l_eGovernmentType = EGovernmentType::ItemCount;
                            if(l_sName == L"com")
                                l_eGovernmentType = EGovernmentType::Communist;
                            else if(l_sName == L"mil")
                                l_eGovernmentType = EGovernmentType::MilitaryDictatorship;
                            else if(l_sName == L"mon")
                                l_eGovernmentType = EGovernmentType::Monarchy;
                            else if(l_sName == L"mpd")
                                l_eGovernmentType = EGovernmentType::MultiPartyDemocracy;
                            else if(l_sName == L"spd")
                                l_eGovernmentType = EGovernmentType::SinglePartyDemocracy;
                            else if(l_sName == L"the")
                                l_eGovernmentType = EGovernmentType::Theocracy;

                            m_IncomeTaxLimits[l_eGovernmentType] = l_GovernmentNode->Data().m_value.ToREAL64() / 100.0;
                            g_Joshua.Log(L"incomeTaxLimit[" + l_sName + L"]: " +
                                         GString::FormatNumber(m_IncomeTaxLimits[l_eGovernmentType], 3));
                        }
                    }
                    else if(l_sElementName == L"logBankruptcies")
                    {
                        m_bLogBankruptcies = (l_sElementValue.ToINT32() != 0);
                        g_Joshua.Log(L"logBankruptcies: " + GString(m_bLogBankruptcies));
                    }
                    else if(l_sElementName == L"maximumCellsInForeignCountry")
                    {
                        m_iMaximumCellsInForeignCountry = l_sElementValue.ToINT32();
                        g_Joshua.Log(L"maximumCellsInForeignCountry: " + GString(m_iMaximumCellsInForeignCountry));
                    }
                    else if(l_sElementName == L"message")
                    {
                        m_sMessage = l_sElementValue;
                        g_Joshua.Log(L"message: " + m_sMessage);
                    }
                    else if(l_sElementName == L"militaryUpkeepPercentages")
                    {
                        // Upkeep for all unit categories, including nuclear, are together in the XML
                        // But nuclear upkeep will be stored separately from the others in the GServer object
                        for(UINT32 j=0; j<EUnitCategory::ItemCount; j++)
	                    {
                            const GTreeNode<GXMLNode>* const l_CategoryNode = objectNode->Child(j);

		                    const GString l_sCategoryName = l_CategoryNode->Data().m_sName;
                            EUnitCategory::Enum l_eUnitCategory = EUnitCategory::ItemCount;
                            if(l_sCategoryName == L"inf")
                                l_eUnitCategory = EUnitCategory::Infantry;
                            else if(l_sCategoryName == L"gro")
                                l_eUnitCategory = EUnitCategory::Ground;
                            else if(l_sCategoryName == L"air")
                                l_eUnitCategory = EUnitCategory::Air;
                            else if(l_sCategoryName == L"nav")
                                l_eUnitCategory = EUnitCategory::Naval;
                            else if(l_sCategoryName == L"nuc")
                                l_eUnitCategory = EUnitCategory::Nuclear;

                            const UINT32 l_iNbCNChildren = l_CategoryNode->NbChilds();

                            switch(l_eUnitCategory)
                            {
                            case EUnitCategory::Nuclear:
                                gassert(l_iNbCNChildren == 0, L"Nuclear node has  " + GString(l_iNbCNChildren) + L" children, expected 0");
                                m_fNuclearUpkeepPercentage = l_CategoryNode->Data().m_value.ToREAL32() / 100.f;
                                g_Joshua.Log(L"militaryUpkeepPercentages[" + l_sCategoryName + L"]: " + GString::FormatNumber(m_fNuclearUpkeepPercentage, 3));
                                break;

                            default:
                                for(UINT32 k=0; k<ETrainingLevel::ItemCount; k++)
                                {
                                    const GTreeNode<GXMLNode>* const l_TrainingNode = l_CategoryNode->Child(k);

                                    const GString l_sTrainingName = l_TrainingNode->Data().m_sName;
                                    ETrainingLevel::Enum l_eTrainingLevel = ETrainingLevel::ItemCount;
                                    if(l_sTrainingName == L"rec")
                                        l_eTrainingLevel = ETrainingLevel::Recruit;
                                    else if(l_sTrainingName == L"reg")
                                        l_eTrainingLevel = ETrainingLevel::Regular;
                                    else if(l_sTrainingName == L"vet")
                                        l_eTrainingLevel = ETrainingLevel::Veteran;
                                    else if(l_sTrainingName == L"eli")
                                        l_eTrainingLevel = ETrainingLevel::Elite;

                                    m_mMilitaryUpkeepPercentages[l_eUnitCategory][l_eTrainingLevel] = l_TrainingNode->Data().m_value.ToREAL32() / 100.f;
                                    g_Joshua.Log(L"militaryUpkeepPercentages[" + l_sCategoryName + L"][" + l_sTrainingName + L"]: " +
                                                 GString::FormatNumber(m_mMilitaryUpkeepPercentages[l_eUnitCategory][l_eTrainingLevel], 3));
                                }
                                break;
                            }
                        }
                    }
		            else if(l_sElementName == L"navalRuleEnabled")
		            {
                        m_bNavalRuleEnabled = (l_sElementValue.ToINT32() != 0);
                        g_Joshua.Log(L"navalRuleEnabled: " + GString(m_bNavalRuleEnabled));
		            }
                    else if(l_sElementName == L"nuclearMissileRangePercentage")
                    {
                        m_fNuclearMissileRangePercentage = l_sElementValue.ToREAL32() / 100.f;
                        g_Joshua.Log(L"nuclearMissileRangePercentage: " + GString::FormatNumber(m_fNuclearMissileRangePercentage, 2));
                    }
                    else if(l_sElementName == L"occupiedRegionPercentageForNuclear")
                    {
                        m_fOccupiedRegionPercentageForNuclear = l_sElementValue.ToREAL32() / 100.f;
                        g_Joshua.Log(L"occupiedRegionPercentageForNuclear: " + GString::FormatNumber(m_fOccupiedRegionPercentageForNuclear, 2));
                    }
                    else if(l_sElementName == L"productionLossOnAnnex")
                    {
                        m_fProductionLossOnAnnex = l_sElementValue.ToREAL32() / 100.f;
                        g_Joshua.Log(L"productionLossOnAnnex: " + GString::FormatNumber(m_fProductionLossOnAnnex, 3));
                    }
                    else if(l_sElementName == L"resourceTaxLimit")
                    {
                        m_fResourceTaxLimit = l_sElementValue.ToREAL32() / 100.f;
                        g_Joshua.Log(L"resourceTaxLimit: " + GString::FormatNumber(m_fResourceTaxLimit, 3));
                    }
                    else if(l_sElementName == L"showHDIComponents")
                    {
                        m_bShowHDIComponents = (l_sElementValue.ToINT32() != 0);
                        g_Joshua.Log(L"showHDIComponents: " + GString(m_bShowHDIComponents));
                    }
                    else if(l_sElementName == L"stabilityAnarchyLowerLimit")
                    {
                        m_fStabilityAnarchyLowerLimit = l_sElementValue.ToREAL32() / 100.f;
                        g_Joshua.Log(L"stabilityAnarchyLowerLimit: " + GString::FormatNumber(m_fStabilityAnarchyLowerLimit, 3));
                    }
                    else if(l_sElementName == L"stabilityAnarchyUpperLimit")
                    {
                        m_fStabilityAnarchyUpperLimit = l_sElementValue.ToREAL32() / 100.f;
                        g_Joshua.Log(L"stabilityAnarchyUpperLimit: " + GString::FormatNumber(m_fStabilityAnarchyUpperLimit, 3));
                    }
                    else if(l_sElementName == L"tributePercent")
                    {
                        m_fTributePercent = l_sElementValue.ToREAL32() / 100.f;
                        g_Joshua.Log(L"tributePercent: " + GString::FormatNumber(m_fTributePercent, 2));
                    }
	            }
            }
            else
            {
		        g_Joshua.Log(SP2::ERROR_PARSING_XML_FILE + l_XMLFile.Name(),MSGTYPE_WARNING);
	        }
        }
        else
	    {
		    g_Joshua.Log(SP2::ERROR_EXTRACT_FILE + l_XMLFile.Name(),MSGTYPE_WARNING);
	    }
	}
    else
    {
        g_Joshua.Log(SP2::ERROR_CANT_FIND_FILE + c_sConfigXMLFile,MSGTYPE_WARNING);
    }
}
