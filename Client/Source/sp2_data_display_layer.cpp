/**************************************************************
*
* sp2_data_display_layer.cpp
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

//Basic Objects
#include "../include/sp2_editbox_numeric.h"
#include "../include/sp2_combo_listboxless.h"
#include "../include/sp2_text_overlay.h"

//Economic Windows
#include "../include/sp2_economic_window.h"
#include "../include/sp2_economic_health_window.h"
#include "../include/sp2_budget_window.h"
#include "../include/sp2_resources_window.h"
#include "../include/sp2_increase_production_window.h"

//Politic Windows
#include "../include/sp2_politic_window.h"
#include "../include/sp2_constitutional_form_window.h"
#include "../include/sp2_sphere_control_window.h"

//Military Windows
#include "../include/sp2_military_window.h"
#include "../include/sp2_strategic_warfare.h"
#include "../include/sp2_unit_view_scene.h"
#include "../include/sp2_unit_view_parts.h"
#include "../include/sp2_create_design_window.h"
#include "../include/sp2_design_window.h"
#include "../include/sp2_battle_info_window.h"
#include "../include/sp2_battle_overview_window.h"
#include "../include/sp2_battle_summary_window.h"
#include "../include/sp2_battle_overview_small_unit_info_window.h"
#include "../include/sp2_unit_attack_confirm_window.h"
#include "../include/sp2_unit_info_small_window.h"
#include "../include/sp2_unit_research_window.h"
#include "../include/sp2_unit_list_window.h"
#include "../include/sp2_unit_list_window_new.h"
#include "../include/sp2_unit_movement_denial_window.h"
#include "../include/sp2_unit_production_window.h"
#include "../include/sp2_unit_ground_deployment_window.h"
#include "../include/sp2_unit_split_window.h"
#include "../include/sp2_group_split_window.h"
#include "../include/sp2_war_list_window.h"

//Strategic Warfare Windows
//#include "../include/sp2_strategic_alert_window.h"
#include "../include/sp2_strategic_launch_window.h"
#include "../include/sp2_strategic_missile_list_window.h"
//#include "../include/sp2_strategic_news_window.h"
#include "../include/sp2_strategic_target_country_window.h"
#include "../include/sp2_strategic_target_multiple_countries_window.h"
#include "../include/sp2_strategic_select_target_window.h"

//System Windows / stuff
#include "../include/sp2_frame_scenario.h"
#include "../include/sp2_objectives_window.h"
#include "../include/sp2_connecting_window.h"
#include "../include/sp2_generic_message_window.h"
#include "../include/sp2_generic_question_window.h"
#include "../include/sp2_generic_input_window.h"
#include "../include/sp2_history_window.h"
#include "../include/sp2_history_marker_window.h"
#include "../include/sp2_history_marker_info_window.h"
#include "../include/sp2_advisor_window.h"
#include "../include/sp2_advisor_settings_window.h"
#include "../include/sp2_advisor_advise_window.h"
#include "../include/sp2_region_control_change_window.h"
//#include "../include/sp2_action_list_window.h"
#include "../include/sp2_mail_window.h"
#include "../include/sp2_event_log_window.h"
#include "../include/sp2_progress_meter.h"
#include "../include/sp2_context_menu.h"
#include "../include/sp2_context_menu_item.h"
#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_host_multiplayer_game.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_system_bar_window.h"
#include "../include/sp2_main_menu.h"
#include "../include/sp2_submenu_single.h"
#include "../include/sp2_submenu_multi.h"
#include "../include/sp2_mini_map.h"
#include "../include/sp2_error_window.h"
#include "../include/sp2_internal_laws_window.h"
#include "../include/sp2_join_multiplayer_game_window.h"
#include "../include/sp2_chat.h"
#include "../include/sp2_start_single_player_window.h"
#include "../include/sp2_game_lobby_window.h"
#include "../include/sp2_test_window.h"
#include "../include/sp2_background_earth.h"
#include "../include/sp2_detailed_earth_area.h"
#include "../include/sp2_frame_updatable_value.h"
#include "../include/sp2_save_confirmation_window.h"
#include "../include/sp2_tooltip_window.h"
#include "../include/sp2_treaties_window.h"
#include "../include/sp2_treaties_detailed_window.h"
#include "../include/sp2_add_remove_window.h"
#include "../include/sp2_tutorial_window.h"
#include "../include/sp2_tutorials.h"
#include "../include/sp2_meter_double.h"
#include "../include/sp2_used_unit_window.h"
#include "../include/sp2_relations_window.h"
#include "../include/sp2_country_information_window.h"
#include "../include/sp2_covert_actions_window.h"
#include "../include/sp2_covert_actions_new_cell.h"
#include "../include/sp2_covert_actions_new_mission.h"
#include "../include/sp2_covert_actions_mission_outcome.h"
#include "../include/sp2_covert_actions_mission_outcome_attacked.h"
#include "../include/sp2_covert_actions_cell_training.h"
#include "../include/sp2_thematic_map_legend_window.h"
#include "../include/sp2_game_scenario_window.h"
#include "../include/sp2_end_game_window.h"
#include "../include/sp2_credits_window.h"
#include "../include/sp2_trade_window.h"
#include "../include/sp2_trade_detailed_window.h"
#include "../include/sp2_country_profile.h"
#include "../include/sp2_party_selection_window.h"
#include "../include/sp2_game_options_window.h"
#include "../include/sp2_load_game_window.h"
#include "../include/sp2_save_game_window.h"
#include "../include/sp2_server_info_window.h"
#include "../include/sp2_RGAPI_login.h"

// Sounds
#include "../include/sp2_sound.h"

CWSTR c_sDefaultCursorFilename = L"/data/cursors/Arrow.cur";

GDataDisplayLayer::GDataDisplayLayer()
{
   m_bLAN = true;
   m_pAdvisorWindow                 = NULL;
   m_pAdvisorSettingsWindow         = NULL;
//   m_pActionListWindow              = NULL;
   m_pBackgroundEarth               = NULL;
   m_pConnectingWindow              = NULL;
   m_pEventLogWindow                = NULL;
   m_pInternalLawsWindow            = NULL;
   m_pMailWindow                    = NULL;
   m_pMainBar                       = NULL;
   m_pMiniMap                       = NULL;
   m_pProgressMeter                 = NULL;
   m_pConsoleWindow                 = NULL;
   m_pConsoleWindowShowAction       = NULL;
   m_pConsoleWindowHideAction       = NULL;
   m_pMainMenu                      = NULL;
   m_pResourcesWindow               = NULL;
   m_pJoinMPGameWindow              = NULL;
   m_pPoliticWindow                 = NULL;
   m_pBombardmentResultWindow       = NULL;
   m_pEconomicWindow                = NULL;
   m_pEconomicHealthWindow          = NULL;
   m_pMilitaryWindow                = NULL;
   m_pAdvisorAdviseWindow           = NULL;
   m_pSplashScreen                  = NULL;
   m_pConstitutionalFormWindow      = NULL;
   m_pStrategicWarfareWindow        = NULL;
   m_pBudgetWindow                  = NULL;
   m_pHostMultiplayerGameWindow     = NULL;
   m_pLoginRGAPIWindow				= NULL;
   m_pGameLobbyWindow               = NULL;
   m_pStartSinglePlayerWindow       = NULL;
   m_pCreateDesignWindow            = NULL;
   m_pDesignWindow                  = NULL;
   m_pUnitInfoSmallWindow           = NULL;
   m_pBattleInfoWindow              = NULL;
   m_pBattleOverviewWindow          = NULL;
   m_pBattleSummaryWindow           = NULL;
   m_pUnitListWindow                = NULL;
   m_pUnitListWindowNew             = NULL;
   m_pUnitProductionWindow          = NULL;
   m_pUsedUnitWindow                = NULL;
   m_pUnitResearchWindow            = NULL;
   //m_pStrategicAlertWindow          = NULL;
   m_pStrategicLaunchWindow         = NULL;
   m_pStrategicMissileListWindow    = NULL;
   //m_pStrategicNewsWindow           = NULL;
   m_pStrategicTargetCountryWindow  = NULL;
   m_pStrategicTargetMultipleCountriesWindow = NULL;
   m_pStrategicSelectTargetWindow   = NULL;
   m_pSphereControlWindow			   = NULL;
   m_pSaveConfirmationWindow        = NULL;
   m_pTooltipWindow                 = NULL;
   m_pTreatiesWindow                = NULL;
   m_pTreatyDetailsWindow           = NULL;
   m_pGenericMessageWindow          = NULL;
   m_pGenericMessageDesktopWindow   = NULL;
   m_pHistoryWindow                 = NULL;
   m_pUnitGroundDeploymentWindow    = NULL;
   m_pAddRemoveWindow               = NULL;
   m_pUnitSplitWindow               = NULL;
   m_pGroupSplitWindow              = NULL;
   m_pIncreaseProductionWindow      = NULL;
   m_pCountryInformationWindow      = NULL;
   m_pRelationsWindow               = NULL;
   m_pCovertActionsWindow           = NULL;
   m_pCovertActionsCellTrainingWindow = NULL;
   m_pCovertActionsMissionOutcomeWindow = NULL;
   m_pCovertActionsNewCellWindow    = NULL;
   m_pCovertActionsNewMissionWindow = NULL;
   m_pCovertActionsMissionOutcomeAttackedWindow = NULL;
   m_pThematicMapLegendWindow       = NULL;
   m_pCountryProfileWindow          = NULL;
   m_pPartySelectionWindow          = NULL;
   m_pTextOverlay                   = NULL;
   m_pGameOptionsWindow             = NULL;
   m_pLoadGameWindow                = NULL;
   m_pSaveGameWindow                = NULL;
   m_pTestWindow                    = NULL;
   m_pActionRectangle               = NULL;
   m_pModelRectangle                = NULL;
   m_pGameScenarioWindow            = NULL;
   m_pEndGameWindow                 = NULL;
   m_pCreditsWindow                 = NULL;
   m_pTradeDetailedWindow           = NULL;
   m_pTradeWindow                   = NULL;
   m_pContextMenu                   = NULL;
   m_pWarListWindow                 = NULL;
   m_pObjCountryLst                 = NULL;
   m_pObjectivesWindow              = NULL;
   m_pServerInfoWindow              = NULL;
   m_pGenericInputWindow            = NULL;
   m_pSystemBarWindow               = NULL;
   m_pSubmenuSingle                 = NULL;
   m_pSubmenuMulti                  = NULL;
   m_pRegionControlChangeWindow     = NULL;

   g_Joshua.FileManager()->File(c_sDefaultCursorFilename, m_CursorDefault);

   m_pLogger = new GLogger(L"window.log");
   m_iUnitMovementDenialNextID = 0;

   m_pLaunchSequence = NULL;
}

GDataDisplayLayer::~GDataDisplayLayer()
{
   SAFE_DELETE(m_pLogger);
}

void GDataDisplayLayer::ResetMousePointer()
{
   g_Joshua.GUIManager()->SetMousePointer(m_CursorDefault);
   if(m_pBackgroundEarth)
      m_pBackgroundEarth->m_DefaultMouseCursor = g_Joshua.GUIManager()->MousePointer();
}


/*!
 * Initializes the DDL, registers constructors for all SP2 Object types etc. 
 * @return true if successfully initialized, false otherwise
 **/
bool GDataDisplayLayer::Initialize()
{
   //
   // GUI Manager initialisation
   //		Register constructors for the types we want
   //		Process the dialog object resource file
   //REGISTER_GUI_OBJECT(GActionListWindow);
   REGISTER_GUI_OBJECT(GMailWindow);
   REGISTER_GUI_OBJECT(GEventLogWindow);
   REGISTER_GUI_OBJECT(GAdvisorWindow);
   REGISTER_GUI_OBJECT(GAdvisorSettingsWindow);
   REGISTER_GUI_OBJECT(GAdvisorAdviseWindow);
   REGISTER_GUI_OBJECT(GBackgroundEarth);
   REGISTER_GUI_OBJECT(GBaseWindow); 
   REGISTER_GUI_OBJECT(GBattleOverviewMeter);
   REGISTER_GUI_OBJECT(GBattleOverviewSmallUnitInfoWindow);
   REGISTER_GUI_OBJECT(GBattleInfoWindow);
   REGISTER_GUI_OBJECT(GBattleOverviewWindow);
   REGISTER_GUI_OBJECT(GBudgetWindow);
   REGISTER_GUI_OBJECT(GConnectingWindow);
   REGISTER_GUI_OBJECT(GConstitutionalFormWindow);
   REGISTER_GUI_OBJECT(GContextMenu);
   REGISTER_GUI_OBJECT(GContextMenuItem);
   REGISTER_GUI_OBJECT(GCursorMeter);
   REGISTER_GUI_OBJECT(GEconomicWindow);
   REGISTER_GUI_OBJECT(GEconomicHealthWindow);
   REGISTER_GUI_OBJECT(GErrorWindow);
   REGISTER_GUI_OBJECT(GFinancingMeter);
   REGISTER_GUI_OBJECT(GGameLobbyWindow); 
   REGISTER_GUI_OBJECT(GStartSinglePlayerWindow);
   REGISTER_GUI_OBJECT(GGradientBar);
   REGISTER_GUI_OBJECT(GHostMPGameWindow);
   REGISTER_GUI_OBJECT(GIdeologyMeter);
   REGISTER_GUI_OBJECT(GInternalLawsWindow);
   REGISTER_GUI_OBJECT(GJoinMPGameWindow);
   REGISTER_GUI_OBJECT(GMainBar);
   REGISTER_GUI_OBJECT(GMainMenu);
   REGISTER_GUI_OBJECT(GMilitaryWindow);
   REGISTER_GUI_OBJECT(GMiniMap);
   REGISTER_GUI_OBJECT(GPoliticWindow);
   REGISTER_GUI_OBJECT(GProgressMeter);
   REGISTER_GUI_OBJECT(GProgressMeterYellowValue);
   REGISTER_GUI_OBJECT(GProgressMeterYellowValueSmall);
   REGISTER_GUI_OBJECT(GResourcesWindow);
   REGISTER_GUI_OBJECT(GSaveConfirmationWindow);
   REGISTER_GUI_OBJECT(GSphereControlWindow);
   //REGISTER_GUI_OBJECT(GStrategicAlertWindow);
   REGISTER_GUI_OBJECT(GStrategicLaunchWindow);
   REGISTER_GUI_OBJECT(GStrategicMissileListWindow);
   //REGISTER_GUI_OBJECT(GStrategicNewsWindow);
   REGISTER_GUI_OBJECT(GStrategicSelectTargetWindow);
   REGISTER_GUI_OBJECT(GStrategicTargetCountryWindow);
   REGISTER_GUI_OBJECT(GStrategicTargetMultipleCountriesWindow);   
   REGISTER_GUI_OBJECT(GStrategicWarfareWindow);
   REGISTER_GUI_OBJECT(GTechnologyMeterBlue);
   REGISTER_GUI_OBJECT(GTestWindow);
   REGISTER_GUI_OBJECT(GUnitAttackConfirmWindow);
   REGISTER_GUI_OBJECT(GCreateDesignWindow);
   REGISTER_GUI_OBJECT(GDesignWindow);
   REGISTER_GUI_OBJECT(GUnitInfoSmallWindow);
   REGISTER_GUI_OBJECT(GUnitListWindow);
   REGISTER_GUI_OBJECT(GUnitListWindowNew);
   REGISTER_GUI_OBJECT(GUnitMovementDenialWindow);
   REGISTER_GUI_OBJECT(GUnitProductionWindow);
   REGISTER_GUI_OBJECT(GUsedUnitWindow);
   REGISTER_GUI_OBJECT(GUnitResearchWindow);
   REGISTER_GUI_OBJECT(GUpdatableValue);
   REGISTER_GUI_OBJECT(GTooltipWindow);
   REGISTER_GUI_OBJECT(GTreatiesWindow);
   REGISTER_GUI_OBJECT(GTreatyDetailsWindow);
   REGISTER_GUI_OBJECT(GGenericMessageWindow);
   REGISTER_GUI_OBJECT(GGenericQuestionWindow);
   REGISTER_GUI_OBJECT(GHistoryWindow);
   REGISTER_GUI_OBJECT(GHistoryMarkerWindow);
   REGISTER_GUI_OBJECT(GHistoryMarkerInfoWindow);
   REGISTER_GUI_OBJECT(GBattleSummaryWindow);
   REGISTER_GUI_OBJECT(GTutorialWindow);
   REGISTER_GUI_OBJECT(GUnitGroundDeploymentWindow);
   REGISTER_GUI_OBJECT(GAddRemoveWindow);
   REGISTER_GUI_OBJECT(GSplitMeter);
   REGISTER_GUI_OBJECT(GUnitSplitWindow);
   REGISTER_GUI_OBJECT(GEditBoxNumeric);
   REGISTER_GUI_OBJECT(GComboListBoxLess);
   REGISTER_GUI_OBJECT(GIncreaseProductionWindow);
   REGISTER_GUI_OBJECT(GMeterDouble);
   REGISTER_GUI_OBJECT(GCountryInformationWindow);
   REGISTER_GUI_OBJECT(GRelationsWindow);
   REGISTER_GUI_OBJECT(GCovertActionsWindow);          
   REGISTER_GUI_OBJECT(GCovertActionsCellTrainingWindow); 
   REGISTER_GUI_OBJECT(GCovertActionsMissionOutcomeWindow); 
   REGISTER_GUI_OBJECT(GCovertActionsMissionOutcomeAttackedWindow);
   REGISTER_GUI_OBJECT(GCovertActionsNewCellWindow);    
   REGISTER_GUI_OBJECT(GCovertActionsNewMissionWindow);
   REGISTER_GUI_OBJECT(GThematicMapLegendWindow);
   REGISTER_GUI_OBJECT(GGameScenarioWindow);
   REGISTER_GUI_OBJECT(GEndGameWindow);
   REGISTER_GUI_OBJECT(GCreditsWindow);
   REGISTER_GUI_OBJECT(GTradeWindow);
   REGISTER_GUI_OBJECT(GTradeDetailedWindow);
   REGISTER_GUI_OBJECT(GCountryProfileWindow);
   REGISTER_GUI_OBJECT(GPartySelectionWindow);
   REGISTER_GUI_OBJECT(GWarListWindow);
   REGISTER_GUI_OBJECT(GTextOverlay);
   REGISTER_GUI_OBJECT(GGameOptionsWindow);
   REGISTER_GUI_OBJECT(GLoadGameWindow);
   REGISTER_GUI_OBJECT(GSaveGameWindow);
   REGISTER_GUI_OBJECT(GGroupSplitWindow);
   REGISTER_GUI_OBJECT(GFrameScenario);
   REGISTER_GUI_OBJECT(GObjectivesWindow);
   REGISTER_GUI_OBJECT(GServerInfoWindow);
   REGISTER_GUI_OBJECT(GGenericInputWindow);
   REGISTER_GUI_OBJECT(GSystemBarWindow);
   REGISTER_GUI_OBJECT(GSubmenuSingle);
   REGISTER_GUI_OBJECT(GSubmenuMulti);
   REGISTER_GUI_OBJECT(GRegionControlChangeWindow);
   REGISTER_GUI_OBJECT(GLoginRGAPIWindow);

   CountriesListBoxSpawn();
   TooltipWindowSpawn();
   g_Joshua.GUIManager()->TooltipInitialize(m_pTooltipWindow, m_pTooltipWindow->Label());

   StartAnimationSystem();

   // Load launch button animation
   GFile l_File;
   if(g_Joshua.FileManager()->File(L"/data/textures/launch/Launch_000.ifl", l_File) )
   {
      m_pLaunchSequence = g_Joshua.Renderer()->TextureManager()->CreateTextureSequence(l_File, true);
      m_pLaunchSequence->Controller()->AnimateByFrame(true);
   }
   else
   {
      m_pLaunchSequence = NULL;
      g_Joshua.Log("Unable to find launch button sequence");
   }

   return true;
}



/*!
 * Shuts down the DDL
 **/
bool GDataDisplayLayer::Shutdown()
{
   AdvisorWindowKill();
   AdvisorSettingsWindowKill();
   ConsoleWindowKill();
   JoinMPGameWindowKill();
   ResourcesWindowKill();
   MainBarKill();
   TextOverlayKill();
   AdvisorAdviseWindowKill();
   ProgressMeterKill();
   SplashScreenKill();
   StrategicWarfareWindowKill();
   HostMPGameWindowKill();
   LoginRGAPIWindowKill();
   TestWindowKill();
   BattleInfoWindowKill();
   BattleOverviewWindowKill();
   BattleSummaryWindowKill();
   SaveConfirmationWindowKill();
   TooltipWindowKill();
   TreatiesWindowKill();
   TreatyDetailsWindowKill();
   AddRemoveWindowKill();
   StartSinglePlayerWindowKill();
   UnitGroundDeploymentWindowKill();
   UnitSplitWindowKill();
   RelationsWindowKill();
   CountryInformationWindowKill();
   CovertActionsWindowKill();          
   CovertActionsCellTrainingWindowKill(); 
   CovertActionsMissionOutcomeWindowKill();
   CovertActionsMissionOutcomeAttackedWindowKill();
   CovertActionsNewCellWindowKill();    
   CovertActionsNewMissionWindowKill();
   CreditsWindowKill();
   TradeWindowKill();
   TradeDetailedWindowKill();
   ContextMenuKill();
   CountryProfileWindowKill();
   PartySelectionWindowKill();
   WarListWindowKill();
   TextOverlayKill();
   GameOptionsWindowKill();
   LoadGameWindowKill();
   SaveGameWindowKill();
   GroupSplitWindowKill();
   ObjectivesWindowKill();
   ServerInfoWindowKill();
   GenericMessageWindowKill();
   GenericInputWindowKill();
   SystemBarWindowKill();
   GameLobbyKill();
   UnitInfoSmallWindowKill();
   RegionControlChangeWindowKill();   

   KillStrategicWarfare();
   KillWindowsMilitaryLv2(true);
   KillWindowsEconomicLv2(true);
   KillWindowsPoliticLv2(true);
   KillWindowsLvl();
   ThematicMapLegendKill();

   MiniMapKill();
   SphereControlWindowKill();
   HistoryWindowKill();

   BackgroundEarthKill();

   CountriesListBoxKill();
   SmallFlagsTexturesDestroy();

   SubmenuMultiKill();
   SubmenuSingleKill();
   MainMenuKill();


   while(m_vSpawnedWindows.size() )
   {
      g_Joshua.Log(GString("Non killed window : ") + m_vSpawnedWindows[0]->Name() );
      m_vSpawnedWindows.erase(m_vSpawnedWindows.begin() );
   }

   SAFE_RELEASE(m_pModelRectangle);
   SAFE_RELEASE(m_pActionRectangle);

   SAFE_RELEASE(m_pLaunchSequence);

   return true;
}

void GDataDisplayLayer::Iterate()
{
   // Gove focus to background earth when no other window has focus, so key events will still be catched
   if( (g_Joshua.GUIManager()->Focus() == NULL) || (g_Joshua.GUIManager()->Focus()->Parent() == NULL) )
   {
      if(BackgroundEarth() )
      {
         g_Joshua.GUIManager()->Focus(BackgroundEarth() );
      }
   }

   if(MainBar())
      MainBar()->Iterate();

   if(BackgroundEarth() )
   {
      BackgroundEarth()->Animate();
   }

   if(GameLobby())
   {
      GameLobby()->Animate();
   }

   if (JoinMPGameWindow() && JoinMPGameWindow()->m_ReadyToRefresh)
   {
      JoinMPGameWindow()->Refresh();
      JoinMPGameWindow()->m_ReadyToRefresh = false;
   }

   if(CovertActionsWindow() && CovertActionsWindow()->Visible())
      CovertActionsWindow()->Iterate();

   if(UnitListWindowNew() && UnitListWindowNew()->Visible())
      UnitListWindowNew()->Iterate();

   GTutorials::Instance().TutorialIterate();

   IterateTimedShowWindow();
   IterateUpdateCursor();
   //IterateWindowCanBeKilled();

   if(StrategicLaunchWindow() )
   {
      StrategicLaunchWindow()->Iterate();
   }

   if(UnitGroundDeploymentWindow() )
   {
      UnitGroundDeploymentWindow()->IterateTrainingDisplay();
   }
}

void GDataDisplayLayer::IterateWindowCanBeKilled()
{
   if( m_pEconomicHealthWindow && !m_pEconomicHealthWindow->Visible() )
      EconomicHealthWindowKill();

   if( m_pResourcesWindow && !m_pResourcesWindow->Visible() )
      ResourcesWindowKill();

   if( m_pBudgetWindow && !m_pBudgetWindow->Visible() )
      BudgetWindowKill();
   
   if( m_pInternalLawsWindow && !m_pInternalLawsWindow->Visible() )
      InternalLawsWindowKill();

   if( m_pConstitutionalFormWindow && !m_pConstitutionalFormWindow->Visible() )
      ConstitutionalFormWindowKill();

   if( m_pTreatiesWindow && !m_pTreatiesWindow->Visible() )
      TreatiesWindowKill();

   if( m_pTreatyDetailsWindow && !m_pTreatyDetailsWindow->Visible() )
      TreatyDetailsWindowKill();

   if( m_pAddRemoveWindow && !m_pAddRemoveWindow->Visible() )
      AddRemoveWindowKill();

   if( m_pUnitListWindow && !m_pUnitListWindow->Visible() )
      UnitListWindowKill();

   if( m_pUnitListWindowNew && !m_pUnitListWindowNew->Visible() )
      UnitListWindowNewKill();

   if( m_pDesignWindow && !m_pDesignWindow->Visible() )
      DesignWindowKill();

   if( m_pUnitProductionWindow && !m_pUnitProductionWindow->Visible() )
      UnitProductionWindowKill();
   
   if( m_pUnitResearchWindow && !m_pUnitResearchWindow->Visible() )
      UnitResearchWindowKill();
   
   if( m_pUnitGroundDeploymentWindow && !m_pUnitGroundDeploymentWindow->Visible() )
      UnitGroundDeploymentWindowKill();
   
   if( m_pCovertActionsWindow && !m_pCovertActionsWindow->Visible() )
      CovertActionsWindowKill();

   if( m_pWarListWindow && !m_pWarListWindow->Visible() )
      WarListWindowKill();

   if( m_pGroupSplitWindow && !m_pGroupSplitWindow->Visible() )
      GroupSplitWindowKill();

   if( m_pTradeDetailedWindow && !m_pTradeDetailedWindow->Visible() )
      TradeDetailedWindowKill();
}

/*!
 * This function is used to update GUpdatableValue when a button is pressed and held
 **/
void GDataDisplayLayer::IterateUpdateCursor()
{
   if(!m_vUpdateValueList.size())
      return;

   //if there is any value to update do them
   for(INT32 i=0; i<(INT32)m_vUpdateValueList.size(); i++)
   {
      //if it's the time to update
      if(g_Joshua.TimeCounter().GetPrecisionTime() > m_vUpdateValueList[i].m_fWhen)
      {
         m_vUpdateValueList[i].m_pObject->UpdateTimerProc(m_vUpdateValueList[i].m_bIncrement);
         if(m_vUpdateValueList[i].m_pObject->m_iCount > 5)
         {
            m_vUpdateValueList[i].m_fWhen = g_Joshua.TimeCounter().GetPrecisionTime() + 0.2f;

            if(m_vUpdateValueList[i].m_pObject->m_iCount > 12)
            {
               m_vUpdateValueList[i].m_fWhen = g_Joshua.TimeCounter().GetPrecisionTime() + 0.15f;
            }
            
            if(m_vUpdateValueList[i].m_pObject->m_iCount > 22)
            {
               m_vUpdateValueList[i].m_fWhen = g_Joshua.TimeCounter().GetPrecisionTime() + 0.1f;
            }

            if(m_vUpdateValueList[i].m_pObject->m_iCount > 35)
            {
               m_vUpdateValueList[i].m_fWhen = g_Joshua.TimeCounter().GetPrecisionTime() + 0.05f;
            }

            if(m_vUpdateValueList[i].m_pObject->m_iCount > 50)
            {
               m_vUpdateValueList[i].m_fWhen = g_Joshua.TimeCounter().GetPrecisionTime() + 0.01f;
            }

            if(m_vUpdateValueList[i].m_pObject->m_iCount > 75)
            {
               m_vUpdateValueList[i].m_fWhen = g_Joshua.TimeCounter().GetPrecisionTime() + 0.005f;
            }
         }
         else
         {
            m_vUpdateValueList[i].m_fWhen = g_Joshua.TimeCounter().GetPrecisionTime() + 0.25f;
         }
      }
   }
}

void GDataDisplayLayer::AddToIterateUpdateCursor(const GUpdateValueData & in_UpdateValueData)
{
   for(INT32 i=0; i<(INT32)m_vUpdateValueList.size(); i++)
   {
      if(m_vUpdateValueList[i].m_pObject == in_UpdateValueData.m_pObject)
         return;
   }
   
   m_vUpdateValueList.push_back(in_UpdateValueData);
}

void GDataDisplayLayer::RemoveFromIterateUpdateCursor(SP2::GUpdatableValue* in_pObject)
{
   vector<GUpdateValueData>::iterator l_It = m_vUpdateValueList.begin();

   while(l_It != m_vUpdateValueList.end())
   {
      if(l_It->m_pObject == in_pObject)
      {
         m_vUpdateValueList.erase(l_It);
         break;
      }
      
      l_It++;
   }
}

void GDataDisplayLayer::IterateTimedShowWindow()
{
   if(m_TimedDisplayObjects.size() > 0)
   {
      REAL32 l_fAnimTime = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Animation_Time();
      hash_map<GUI::GBaseObject*, GTimedDisplayObjectData> l_NewTimedDisplayObjectsList;

      for(hash_map<GUI::GBaseObject*, GTimedDisplayObjectData>::iterator l_It = m_TimedDisplayObjects.begin();
          l_It != m_TimedDisplayObjects.end(); 
          l_It++)
      {
         gassert(l_It->second.m_pObject == l_It->first,"GDataDisplayLayer::IterateTimedShowWindow Incoherence, values should be the same");
         
         //Set more simple variables with what the iterator is pointing to
         GUI::GBaseObject*          l_pObj                   = l_It->first;
         GTimedDisplayObjectData&   l_TimedDisplayObjectData = l_It->second;
         if(l_fAnimTime > l_It->second.m_fWhen)
         {
            if(l_pObj)
            {
               if(l_TimedDisplayObjectData.m_bVisible)
               {
                  if(!l_pObj->Visible())
                     l_pObj->Show();
               }
               else
               {
                  if(l_pObj->Visible())
                     l_pObj->Hide();
               }
            }
         }
         else
         {
            l_NewTimedDisplayObjectsList[l_It->first] = l_It->second;
         }
      }

      m_TimedDisplayObjects = l_NewTimedDisplayObjectsList;
   }
}

GBackgroundEarth* GDataDisplayLayer::BackgroundEarth()
{
   return m_pBackgroundEarth;
}

/*!
 * Create and Display the Background earth object
 * @return  true if successful, false otherwise
 **/
bool GDataDisplayLayer::BackgroundEarthSpawn()
{
   ObjectSpawn((GUI::GBaseObject**)&m_pBackgroundEarth,GBackgroundEarth::TypeName, true, true, NULL) ;

   if(m_pBackgroundEarth)
   {
      m_pBackgroundEarth->Initialize();
      return true;
   }
   else
      return false;
}


/*!
 * Kill the background earth
 **/
bool GDataDisplayLayer::BackgroundEarthKill()
{
   ObjectKill((GUI::GBaseObject**)&m_pBackgroundEarth);

   //By Killing the background earth, you kill all its children, set the pointer
   //to null so we wont use invalid pointers
   m_pAdvisorWindow                 = NULL;
   m_pAdvisorSettingsWindow         = NULL;
   m_pMiniMap                       = NULL;

   m_pConsoleWindow                 = NULL;
   m_pConsoleWindowShowAction       = NULL;
   m_pConsoleWindowHideAction       = NULL;
   m_pResourcesWindow               = NULL;
   m_pJoinMPGameWindow              = NULL;
   m_pMainBar                       = NULL;
   m_pPoliticWindow                 = NULL;
   m_pBombardmentResultWindow       = NULL;
   m_pEconomicWindow                = NULL;
   m_pEconomicHealthWindow          = NULL;
   m_pMilitaryWindow                = NULL;
   m_pAdvisorAdviseWindow           = NULL;
   m_pConstitutionalFormWindow      = NULL;
   m_pStrategicWarfareWindow        = NULL;
   m_pBudgetWindow                  = NULL;
   m_pHostMultiplayerGameWindow     = NULL;
   m_pInternalLawsWindow            = NULL;
//   m_pActionListWindow              = NULL;
   m_pMailWindow                    = NULL;
   m_pEventLogWindow                = NULL;
   m_pTestWindow                    = NULL;
   m_pBattleInfoWindow              = NULL;
   m_pBattleOverviewWindow          = NULL;
   m_pBattleSummaryWindow           = NULL;
   m_pUnitListWindow                = NULL;
   m_pUnitListWindowNew             = NULL;
   m_pUnitProductionWindow          = NULL;
   m_pUnitResearchWindow            = NULL;
   m_pUnitInfoSmallWindow           = NULL;
   //m_pStrategicAlertWindow          = NULL;
   m_pStrategicLaunchWindow         = NULL;
   m_pStrategicMissileListWindow    = NULL;
   //m_pStrategicNewsWindow           = NULL;
   m_pStrategicTargetCountryWindow  = NULL;
   m_pStrategicTargetMultipleCountriesWindow = NULL;
   m_pStrategicSelectTargetWindow   = NULL;
   m_pSphereControlWindow			   = NULL;
   m_pSaveConfirmationWindow        = NULL;
   m_pTooltipWindow                 = NULL;
   m_pTreatiesWindow                = NULL;
   m_pTreatyDetailsWindow           = NULL;
   m_pGenericMessageWindow          = NULL;
   m_pGenericMessageDesktopWindow   = NULL;
   m_pHistoryWindow                 = NULL;
   m_pGameLobbyWindow               = NULL;
   m_pStartSinglePlayerWindow       = NULL;
   m_pCreateDesignWindow            = NULL;
   m_pDesignWindow                  = NULL;
   m_pUnitGroundDeploymentWindow    = NULL;
   m_pAddRemoveWindow               = NULL;
   m_pUnitSplitWindow               = NULL;
   m_pGroupSplitWindow              = NULL;
   m_pIncreaseProductionWindow      = NULL;
   m_pCountryInformationWindow      = NULL;
   m_pGameScenarioWindow            = NULL;
   m_pEndGameWindow                 = NULL;
   m_pRelationsWindow               = NULL;
   m_pCovertActionsWindow           = NULL;
   m_pCovertActionsCellTrainingWindow = NULL;
   m_pCovertActionsMissionOutcomeWindow = NULL;
   m_pCovertActionsMissionOutcomeAttackedWindow = NULL;
   m_pCovertActionsNewCellWindow    = NULL;
   m_pCovertActionsNewMissionWindow = NULL;
   m_pThematicMapLegendWindow       = NULL;
   m_pTradeWindow                   = NULL;
   m_pTradeDetailedWindow           = NULL;
   m_pCountryProfileWindow          = NULL;
   m_pPartySelectionWindow          = NULL;
   m_pWarListWindow                 = NULL;
   m_pObjectivesWindow              = NULL;
   m_pSystemBarWindow               = NULL;
   m_pRegionControlChangeWindow     = NULL;

   m_UnitAttackConfirmWindows.clear();

   return true;
}



/*!
 * Starts the animation system
 * @return  true if successful, false otherwise
 **/
bool GDataDisplayLayer::StartAnimationSystem()
{
   //Play the animation system
   if(!g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Play_Animation())
   {
      return false;
   } 
   return true;
}

bool GDataDisplayLayer::StrategicWarfareWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pStrategicWarfareWindow,GStrategicWarfareWindow::TypeName, true, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::StrategicWarfareWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pStrategicWarfareWindow);
}

GStrategicWarfareWindow* GDataDisplayLayer::StrategicWarfareWindow()
{
   return m_pStrategicWarfareWindow;
}

GUI::GConsole* GDataDisplayLayer::ConsoleWindow()
{
   return m_pConsoleWindow;
}

bool GDataDisplayLayer::ConsoleWindowSpawn()
{
   //If already exists | displayed
   if(m_pConsoleWindow)
   {
      if(!m_pConsoleWindow->Visible())
         m_pConsoleWindow->Show();
      return true;
   }

   // Create the graphical console   
   m_pConsoleWindow = CREATE_GUI_OBJECT_HIDDEN(GUI::GConsole, NULL);
   if(!m_pConsoleWindow)
      return false;

   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   m_pConsoleWindow->Size(mode.m_Resolution.x, mode.m_Resolution.y / 2);
   m_pConsoleWindow->Position(0, 0);
   m_pConsoleWindow->ColorNormal( GColorRGBInt(64,64,64,128) );
   m_pConsoleWindow->SetTextColor( GColorRGBInt(200,200,200,255) );
   m_pConsoleWindow->SetAsEngineConsole();
 
   // Create vertical animation when showing console
   m_pConsoleWindowShowAction = g_Joshua.Renderer()->Create_Action();
   CGFX_Animation_Linear_Itf* l_ShowAnim = g_Joshua.Renderer()->Create_Animation_Linear();
   CGFX_Key_Linear l_ShowKeys[2] = 
   {
      {0, 0},
      {0.5f, (REAL32) mode.m_Resolution.y / 2},
   };
   l_ShowAnim->Setup_Keys(l_ShowKeys, 2);
   l_ShowAnim->Connect(m_pConsoleWindow->Model()->Get_Transform(), GFX::PARAM_SIZE_Y);
   m_pConsoleWindowShowAction->Add_Animation(l_ShowAnim);

   // Make sure visibility is enabled
   CGFX_Animation_Static_Itf* l_ShowAnimVis = g_Joshua.Renderer()->Create_Animation_Static();
   l_ShowAnimVis->Set_Value(true);
   l_ShowAnimVis->Connect(m_pConsoleWindow->Model(), GFX::PARAM_VISIBILITY);
   m_pConsoleWindowShowAction->Add_Animation(l_ShowAnimVis);

   m_pConsoleWindowShowAction->Length(0.5f);
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Add_Action(m_pConsoleWindowShowAction);
   SAFE_RELEASE(l_ShowAnim);
   SAFE_RELEASE(l_ShowAnimVis);
   
   // Create vertical animation when hiding console
   m_pConsoleWindowHideAction = g_Joshua.Renderer()->Create_Action();
   CGFX_Animation_Linear_Itf* l_HideAnim = g_Joshua.Renderer()->Create_Animation_Linear();
   CGFX_Key_Linear l_HideKeys[2] = 
   {
      {0, (REAL32) mode.m_Resolution.y / 2.f},
      {0.5f, 0},
   };
   l_HideAnim->Setup_Keys(l_HideKeys, 2);
   l_HideAnim->Connect(m_pConsoleWindow->Model()->Get_Transform(), GFX::PARAM_SIZE_Y);
   m_pConsoleWindowHideAction->Add_Animation(l_HideAnim);

   // Make sure visibility is disabled after console has finished hiding
   CGFX_Animation_Constant_Itf* l_HideAnimVis = g_Joshua.Renderer()->Create_Animation_Constant();
   CGFX_Key_Constant l_HideKeysVis[2] = 
   {
      {0, true},
      {0.5f, false},
   };
   l_HideAnimVis->Setup_Keys(l_HideKeysVis, 2);
   l_HideAnimVis->Connect(m_pConsoleWindow->Model(), GFX::PARAM_VISIBILITY);
   m_pConsoleWindowHideAction->Add_Animation(l_HideAnimVis);

   m_pConsoleWindowHideAction->Length(0.5f);
   UINT32 l_ID = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Add_Action(m_pConsoleWindowHideAction);
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Play_Action(l_ID);
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Set_Action_Time(l_ID, 0.5f);
   SAFE_RELEASE(l_HideAnim);
   SAFE_RELEASE(l_HideAnimVis);

   return true;
}

bool GDataDisplayLayer::ConsoleWindowShow()
{
   m_pConsoleWindow->Show();
   m_pConsoleWindow->BringToFront();

   g_Joshua.GUIManager()->Focus(m_pConsoleWindow);

   // Stop the console Hide animation
   UINT32 l_HideID = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Index(m_pConsoleWindowHideAction);
   REAL32 l_HideTime = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Time(l_HideID);
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Stop_Action(l_HideID);

   // Start the console show animation where the Hide animation ended
   UINT32 l_ShowID = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Index(m_pConsoleWindowShowAction);
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Play_Action(l_ShowID);
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Set_Action_Time(l_ShowID, max(m_pConsoleWindowHideAction->Length() - l_HideTime, 0) );

   return true;
}

bool GDataDisplayLayer::ConsoleWindowHide()
{
   m_pConsoleWindow->Hide();

   // Stop the console show animation
   UINT32 l_ShowID = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Index(m_pConsoleWindowShowAction);
   REAL32 l_ShowTime = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Time(l_ShowID);
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Stop_Action(l_ShowID);

   // Start the console Hide animation where the show animation ended
   UINT32 l_HideID = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Index(m_pConsoleWindowHideAction);
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Play_Action(l_HideID);
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Set_Action_Time(l_HideID, max(m_pConsoleWindowShowAction->Length() - l_ShowTime, 0) );

   return true;
}

bool GDataDisplayLayer::ConsoleWindowKill()
{
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Remove_Action(m_pConsoleWindowShowAction);
   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Remove_Action(m_pConsoleWindowHideAction);

   SAFE_RELEASE(m_pConsoleWindowShowAction);
   SAFE_RELEASE(m_pConsoleWindowHideAction);

   if(!m_pConsoleWindow)
      return true;

   g_Joshua.GUIManager()->ReleaseObjectAsync(m_pConsoleWindow);

   m_pConsoleWindow = NULL;

   return true;
}

GTextOverlay* GDataDisplayLayer::TextOverlay()
{
   return m_pTextOverlay;
}

bool GDataDisplayLayer::TextOverlaySpawn()
{
   if(ObjectSpawn((GUI::GBaseObject**)&m_pTextOverlay,GTextOverlay::TypeName, false, false, NULL) )
      return true;
   
   return false;
}

bool GDataDisplayLayer::TextOverlayKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pTextOverlay);
}

GBattleOverviewWindow* GDataDisplayLayer::BattleOverviewWindow()
{
   return m_pBattleOverviewWindow;
}

bool GDataDisplayLayer::BattleOverviewWindowSpawn()
{
   if(ObjectSpawn((GUI::GBaseObject**)&m_pBattleOverviewWindow,GBattleOverviewWindow::TypeName, true, true, m_pBackgroundEarth) )
      return true;
   
   return false;
}

bool GDataDisplayLayer::BattleOverviewWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pBattleOverviewWindow);
}

GGameOptionsWindow* GDataDisplayLayer::GameOptionsWindow()
{
   return m_pGameOptionsWindow;
}

bool GDataDisplayLayer::GameOptionsWindowSpawn()
{
   if(ObjectSpawn((GUI::GBaseObject**)&m_pGameOptionsWindow,GGameOptionsWindow::TypeName, false, true) )
      return true;
   
   return false;
}

bool GDataDisplayLayer::GameOptionsWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pGameOptionsWindow);
}

GRegionControlChangeWindow* GDataDisplayLayer::RegionControlChangeWindow()
{
   return m_pRegionControlChangeWindow;
}

bool GDataDisplayLayer::RegionControlChangeWindowSpawn()
{
   if(ObjectSpawn((GUI::GBaseObject**)&m_pRegionControlChangeWindow,GRegionControlChangeWindow::TypeName, false, true) )
      return true;
   
   return false;
}

bool GDataDisplayLayer::RegionControlChangeWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pRegionControlChangeWindow);
}

GBattleSummaryWindow* GDataDisplayLayer::BattleSummaryWindow()
{
   return m_pBattleSummaryWindow;
}

bool GDataDisplayLayer::BattleSummaryWindowSpawn(GUI::GBaseObject* in_pParent)
{
   gassert(in_pParent,"Null pointer");
   if(ObjectSpawn((GUI::GBaseObject**)&m_pBattleSummaryWindow,GBattleSummaryWindow::TypeName, true, true, in_pParent) )
      return true;
   
   return false;
}

bool GDataDisplayLayer::BattleSummaryWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pBattleSummaryWindow);
}


GBattleInfoWindow* GDataDisplayLayer::BattleInfoWindow()
{
   return m_pBattleInfoWindow;
}

bool GDataDisplayLayer::BattleInfoWindowSpawn()
{
   if(ObjectSpawn((GUI::GBaseObject**)&m_pBattleInfoWindow,GBattleInfoWindow::TypeName, true, true, m_pBackgroundEarth) )
      return true;
   
   return false;
}

bool GDataDisplayLayer::BattleInfoWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pBattleInfoWindow);
}

GBudgetWindow* GDataDisplayLayer::BudgetWindow()
{
   return m_pBudgetWindow;
}

bool GDataDisplayLayer::BudgetWindowSpawn()
{
   if(ObjectSpawn((GUI::GBaseObject**)&m_pBudgetWindow,GBudgetWindow::TypeName, g_ClientDAL.DataBudgetWindow.PreviouslyShown != 0, true, m_pBackgroundEarth) )
      return true;

   return false;
}

bool GDataDisplayLayer::BudgetWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pBudgetWindow);
}

GCountryProfileWindow* GDataDisplayLayer::CountryProfileWindow()
{
   return m_pCountryProfileWindow;
}

bool GDataDisplayLayer::CountryProfileWindowSpawn()
{
   if(ObjectSpawn((GUI::GBaseObject**)&m_pCountryProfileWindow,GCountryProfileWindow::TypeName, false, true, m_pBackgroundEarth) )
      return true;

   return false;
}

bool GDataDisplayLayer::CountryProfileWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pCountryProfileWindow);
}

GWarListWindow* GDataDisplayLayer::WarListWindow()
{
   return m_pWarListWindow;
}

bool GDataDisplayLayer::WarListWindowSpawn()
{
   if(ObjectSpawn((GUI::GBaseObject**)&m_pWarListWindow,GWarListWindow::TypeName, false, true, m_pBackgroundEarth) )
      return true;

   return false;
}

bool GDataDisplayLayer::WarListWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pWarListWindow);
}

GSaveConfirmationWindow* GDataDisplayLayer::SaveConfirmationWindow()
{
   return m_pSaveConfirmationWindow;
}

bool GDataDisplayLayer::SaveConfirmationWindowSpawn()
{
   if(ObjectSpawn((GUI::GBaseObject**)&m_pSaveConfirmationWindow,GSaveConfirmationWindow::TypeName, false, true, m_pBackgroundEarth) )
      return true;

   return false;
}

bool GDataDisplayLayer::SaveConfirmationWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pSaveConfirmationWindow);
}

GConstitutionalFormWindow* GDataDisplayLayer::ConstitutionalFormWindow()
{
   return m_pConstitutionalFormWindow;
}

bool GDataDisplayLayer::ConstitutionalFormWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pConstitutionalFormWindow,GConstitutionalFormWindow::TypeName, g_ClientDAL.DataConstitutionalForm.PreviouslyShown != 0, true, m_pBackgroundEarth))
      return true;

   return false;
}

bool GDataDisplayLayer::ConstitutionalFormWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pConstitutionalFormWindow);
}

//GActionListWindow* GDataDisplayLayer::ActionListWindow()
//{
//   return m_pActionListWindow;
//}

//bool GDataDisplayLayer::ActionListWindowSpawn()
//{
//   if (ObjectSpawn((GUI::GBaseObject**)&m_pActionListWindow,GActionListWindow::TypeName, false, true, m_pBackgroundEarth))
//      return true;

//   return false;
//}

//bool GDataDisplayLayer::ActionListWindowKill()
//{
//   return ObjectKill((GUI::GBaseObject**)&m_pActionListWindow);
//}

GMailWindow* GDataDisplayLayer::MailWindow()
{
   return m_pMailWindow;
}

bool GDataDisplayLayer::MailWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pMailWindow,GMailWindow::TypeName, false, true, m_pBackgroundEarth))
      return true;

   return false;
}

bool GDataDisplayLayer::MailWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pMailWindow);
}

GEventLogWindow* GDataDisplayLayer::EventLogWindow()
{
   return m_pEventLogWindow;
}

bool GDataDisplayLayer::EventLogWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pEventLogWindow, GEventLogWindow::TypeName, false, true, m_pBackgroundEarth))
      return true;

   return false;
}

bool GDataDisplayLayer::EventLogWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pEventLogWindow);
}

GAddRemoveWindow* GDataDisplayLayer::AddRemoveWindow()
{
   return m_pAddRemoveWindow;
}

bool GDataDisplayLayer::AddRemoveWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pAddRemoveWindow,GAddRemoveWindow::TypeName, false, true, m_pBackgroundEarth))
      return true;

   return false;
}

bool GDataDisplayLayer::AddRemoveWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pAddRemoveWindow);
}

GMainMenu* GDataDisplayLayer::MainMenu()
{
   return m_pMainMenu;
}

bool GDataDisplayLayer::MainMenuSpawn()
{
   ObjectSpawn((GUI::GBaseObject**)&m_pMainMenu,GMainMenu::TypeName, false, true);
   return true;
}

bool GDataDisplayLayer::MainMenuKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pMainMenu);
}

GSubmenuMulti* GDataDisplayLayer::SubmenuMulti()
{
   return m_pSubmenuMulti;
}

bool GDataDisplayLayer::SubmenuMultiSpawn()
{
   ObjectSpawn((GUI::GBaseObject**)&m_pSubmenuMulti,GSubmenuMulti::TypeName, false, true);
   return true;
}

bool GDataDisplayLayer::SubmenuMultiKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pSubmenuMulti);
}

GSubmenuSingle* GDataDisplayLayer::SubmenuSingle()
{
   return m_pSubmenuSingle;
}

bool GDataDisplayLayer::SubmenuSingleSpawn()
{
   ObjectSpawn((GUI::GBaseObject**)&m_pSubmenuSingle,GSubmenuSingle::TypeName, false, true);
   return true;
}

bool GDataDisplayLayer::SubmenuSingleKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pSubmenuSingle);
}

GLoadGameWindow* GDataDisplayLayer::LoadGameWindow()
{
   return m_pLoadGameWindow;
}

bool GDataDisplayLayer::LoadGameWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pLoadGameWindow,GLoadGameWindow::TypeName, false, true, NULL))
      return true;

   return false;
}

bool GDataDisplayLayer::LoadGameWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pAddRemoveWindow);
}

GSaveGameWindow* GDataDisplayLayer::SaveGameWindow()
{
   return m_pSaveGameWindow;
}

bool GDataDisplayLayer::SaveGameWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pSaveGameWindow,GSaveGameWindow::TypeName, false, true, NULL))
      return true;

   return false;
}

bool GDataDisplayLayer::SaveGameWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pAddRemoveWindow);
}

GAdvisorAdviseWindow* GDataDisplayLayer::AdvisorAdviseWindow()
{
   return m_pAdvisorAdviseWindow;
}

bool GDataDisplayLayer::AdvisorAdviseWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pAdvisorAdviseWindow,GAdvisorAdviseWindow::TypeName, true, true, m_pBackgroundEarth))
      return true;

   return false;
}

bool GDataDisplayLayer::AdvisorAdviseWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pAdvisorAdviseWindow);
}

void GDataDisplayLayer::AdvisorAdviseWindowSet(GString in_sText, GUI::GBaseObject* in_pObject)
{
   if(m_pAdvisorAdviseWindow)
      m_pAdvisorAdviseWindow->Set(in_sText, in_pObject);
}

GSystemBarWindow* GDataDisplayLayer::SystemBarWindow()
{
   return m_pSystemBarWindow;
}

bool GDataDisplayLayer::SystemBarWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pSystemBarWindow,GSystemBarWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::SystemBarWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pSystemBarWindow);
}

GMainBar* GDataDisplayLayer::MainBar()
{
   return m_pMainBar;
}

bool GDataDisplayLayer::MainBarSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pMainBar,GMainBar::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::MainBarKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pMainBar);
}

GPartySelectionWindow* GDataDisplayLayer::PartySelectionWindow()
{
   return m_pPartySelectionWindow;
}

bool GDataDisplayLayer::PartySelectionWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pPartySelectionWindow,GPartySelectionWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::PartySelectionWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pPartySelectionWindow);
}

GCovertActionsWindow* GDataDisplayLayer::CovertActionsWindow()
{
   return m_pCovertActionsWindow;
}

bool GDataDisplayLayer::CovertActionsWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pCovertActionsWindow,GCovertActionsWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::CovertActionsWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pCovertActionsWindow);
}

GCovertActionsCellTrainingWindow* GDataDisplayLayer::CovertActionsCellTrainingWindow()
{
   return m_pCovertActionsCellTrainingWindow;
}

bool GDataDisplayLayer::CovertActionsCellTrainingWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pCovertActionsCellTrainingWindow,GCovertActionsCellTrainingWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::CovertActionsCellTrainingWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pCovertActionsCellTrainingWindow);
}

GCovertActionsMissionOutcomeWindow* GDataDisplayLayer::CovertActionsMissionOutcomeWindow()
{
   return m_pCovertActionsMissionOutcomeWindow;
}

bool GDataDisplayLayer::CovertActionsMissionOutcomeWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pCovertActionsMissionOutcomeWindow,GCovertActionsMissionOutcomeWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::CovertActionsMissionOutcomeWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pCovertActionsMissionOutcomeWindow);
}

GCovertActionsMissionOutcomeAttackedWindow* GDataDisplayLayer::CovertActionsMissionOutcomeAttackedWindow()
{
   return m_pCovertActionsMissionOutcomeAttackedWindow;
}

bool GDataDisplayLayer::CovertActionsMissionOutcomeAttackedWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pCovertActionsMissionOutcomeAttackedWindow,GCovertActionsMissionOutcomeAttackedWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::CovertActionsMissionOutcomeAttackedWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pCovertActionsMissionOutcomeAttackedWindow);
}

GCovertActionsNewCellWindow* GDataDisplayLayer::CovertActionsNewCellWindow()
{
   return m_pCovertActionsNewCellWindow;
}

bool GDataDisplayLayer::CovertActionsNewCellWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pCovertActionsNewCellWindow,GCovertActionsNewCellWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::CovertActionsNewCellWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pCovertActionsNewCellWindow);
}

GCovertActionsNewMissionWindow* GDataDisplayLayer::CovertActionsNewMissionWindow()
{
   return m_pCovertActionsNewMissionWindow;
}

bool GDataDisplayLayer::CovertActionsNewMissionWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pCovertActionsNewMissionWindow,GCovertActionsNewMissionWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::CovertActionsNewMissionWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pCovertActionsNewMissionWindow);
}

bool GDataDisplayLayer::MainBarUpdateRank()
{
   if(g_ClientDAL.GenericDataHolder().m_iRankedCountryId!= (INT16)g_ClientDCL.SelectedCountryID())
      return false;

   MainBarScaleRank((GFrame*)m_pMainBar->Child(L"frmMeterEconomic") ,g_ClientDAL.NbCountry() - g_ClientDAL.GenericDataHolder().m_iEconomicRank);
   MainBarScaleRank((GFrame*)m_pMainBar->Child(L"frmMeterPolitic")  ,g_ClientDAL.NbCountry() - g_ClientDAL.GenericDataHolder().m_iPoliticRank);
   MainBarScaleRank((GFrame*)m_pMainBar->Child(L"frmMeterMilitary") ,g_ClientDAL.GenericDataHolder().m_iMilitaryRank);

   return true;
}

void GDataDisplayLayer::MainBarScaleRank(GFrame* in_pFrame, INT16 in_iRank)
{
   in_pFrame->Child(L"frm1")->Hide();
   in_pFrame->Child(L"frm2")->Hide();
   in_pFrame->Child(L"frm3")->Hide();
   in_pFrame->Child(L"frm4")->Hide();
   in_pFrame->Child(L"frm5")->Hide();
   in_pFrame->Child(L"frm6")->Hide();
   in_pFrame->Child(L"frm7")->Hide();
   in_pFrame->Child(L"frm8")->Hide();
   in_pFrame->Child(L"frm9")->Hide();

   if(in_iRank > 173)
      in_pFrame->Child(L"frm9")->Show();
   if(in_iRank > 153)
      in_pFrame->Child(L"frm8")->Show();
   if(in_iRank > 133)
      in_pFrame->Child(L"frm7")->Show();
   if(in_iRank > 113)
      in_pFrame->Child(L"frm6")->Show();
   if(in_iRank > 93)
      in_pFrame->Child(L"frm5")->Show();
   if(in_iRank > 73)
      in_pFrame->Child(L"frm4")->Show();
   if(in_iRank > 53)
      in_pFrame->Child(L"frm3")->Show();
   if(in_iRank > 33)
      in_pFrame->Child(L"frm2")->Show();
   if(in_iRank > 13)
      in_pFrame->Child(L"frm1")->Show();
}

GPoliticWindow* GDataDisplayLayer::PoliticWindow()
{
   return m_pPoliticWindow;
}

bool GDataDisplayLayer::PoliticWindowSpawn()
{
   if (!ObjectSpawn((GUI::GBaseObject**)&m_pPoliticWindow,GPoliticWindow::TypeName, false, true, m_pBackgroundEarth))
      return false;

   if(g_ClientDAL.GenericDataHolder().m_iRankedCountryId == (INT16)g_ClientDCL.SelectedCountryID())
   {
      //PoliticWindowUpdateRank(g_ClientDAL.GenericDataHolder().m_iPoliticRank);
   }

   
   return true;
}

bool GDataDisplayLayer::PoliticWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pPoliticWindow);
}

GTradeWindow* GDataDisplayLayer::TradeWindow()
{
   return m_pTradeWindow;
}

bool GDataDisplayLayer::TradeWindowSpawn()
{
   if (!ObjectSpawn((GUI::GBaseObject**)&m_pTradeWindow,GTradeWindow::TypeName, false, true, m_pBackgroundEarth))
      return true;

   return false;
}

bool GDataDisplayLayer::TradeWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pTradeWindow);
}

GTradeDetailedWindow* GDataDisplayLayer::TradeDetailedWindow()
{
   return m_pTradeDetailedWindow;
}

bool GDataDisplayLayer::TradeDetailedWindowSpawn()
{
   if (!ObjectSpawn((GUI::GBaseObject**)&m_pTradeDetailedWindow,GTradeDetailedWindow::TypeName, false, true, m_pBackgroundEarth))
      return true;

   return false;
}

bool GDataDisplayLayer::TradeDetailedWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pTradeDetailedWindow);
}

GEconomicWindow* GDataDisplayLayer::EconomicWindow()
{
   return m_pEconomicWindow;
}

bool GDataDisplayLayer::EconomicWindowSpawn()
{
   if (!ObjectSpawn((GUI::GBaseObject**)&m_pEconomicWindow,GEconomicWindow::TypeName,false, true, m_pBackgroundEarth))
      return false;

   if(g_ClientDAL.GenericDataHolder().m_iRankedCountryId == (INT16)g_ClientDCL.SelectedCountryID())
   {
//      EconomicWindowUpdateRank(g_ClientDAL.GenericDataHolder().m_iEconomicRank);
   }

   
   return true;
}

bool GDataDisplayLayer::EconomicWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pEconomicWindow);
}

GEconomicHealthWindow* GDataDisplayLayer::EconomicHealthWindow()
{
   return m_pEconomicHealthWindow;
}

bool GDataDisplayLayer::EconomicHealthWindowSpawn()
{
   if (!ObjectSpawn((GUI::GBaseObject**)&m_pEconomicHealthWindow,GEconomicHealthWindow::TypeName, g_ClientDAL.DataEconomicHealthWindow.PreviouslyShown != 0, true, m_pBackgroundEarth))
      return false;

   //if(g_ClientDAL.GenericDataHolder().m_iRankedCountryId == (INT16)g_ClientDCL.SelectedCountryID())
   //{
   //   EconomicHealthWindowUpdate();
   //}

   
   return true;
}

bool GDataDisplayLayer::EconomicHealthWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pEconomicHealthWindow);
}

GGenericMessageWindow* GDataDisplayLayer::GenericMessageWindow()
{
   return m_pGenericMessageWindow;
}

bool GDataDisplayLayer::GenericMessageWindowSpawn()
{
   if (!ObjectSpawn((GUI::GBaseObject**)&m_pGenericMessageWindow,GGenericMessageWindow::TypeName, false, true, m_pBackgroundEarth))
      return true;

   return false;
}

bool GDataDisplayLayer::GenericMessageWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pGenericMessageWindow);
}

GGenericMessageWindow* GDataDisplayLayer::GenericMessageDesktopWindow()
{
   return m_pGenericMessageDesktopWindow;
}

bool GDataDisplayLayer::GenericMessageDesktopWindowSpawn()
{
   if (!ObjectSpawn((GUI::GBaseObject**)&m_pGenericMessageDesktopWindow,GGenericMessageWindow::TypeName, false, true, NULL))
      return true;

   return false;
}

bool GDataDisplayLayer::GenericMessageDesktopWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pGenericMessageDesktopWindow);
}

GGenericInputWindow* GDataDisplayLayer::GenericInputWindow()
{
   return m_pGenericInputWindow;
}

bool GDataDisplayLayer::GenericInputWindowSpawn()
{
   if (!ObjectSpawn((GUI::GBaseObject**)&m_pGenericInputWindow,GGenericInputWindow::TypeName, false, true, NULL))
      return true;

   return false;
}

bool GDataDisplayLayer::GenericInputWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pGenericInputWindow);
}

GHistoryWindow* GDataDisplayLayer::HistoryWindow()
{
   return m_pHistoryWindow;
}

bool GDataDisplayLayer::HistoryWindowSpawn()
{
   if (!ObjectSpawn((GUI::GBaseObject**)&m_pHistoryWindow,GHistoryWindow::TypeName, false, true, m_pBackgroundEarth))
      return true;

   return false;
}

bool GDataDisplayLayer::HistoryWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pHistoryWindow);
}

GMilitaryWindow* GDataDisplayLayer::MilitaryWindow()
{
   return m_pMilitaryWindow;
}

bool GDataDisplayLayer::MilitaryWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pMilitaryWindow,GMilitaryWindow::TypeName, false , true, m_pBackgroundEarth))
      return true;

   return false;
}

bool GDataDisplayLayer::MilitaryWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pMilitaryWindow);
}


GHostMPGameWindow* GDataDisplayLayer::HostMPGameWindow()
{
   return m_pHostMultiplayerGameWindow;
}

bool GDataDisplayLayer::HostMPGameWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pHostMultiplayerGameWindow,GHostMPGameWindow::TypeName, true, true, m_pBackgroundEarth))
      return true;

   return false;
}

bool GDataDisplayLayer::HostMPGameWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pHostMultiplayerGameWindow);
}

bool GDataDisplayLayer::LoginRGAPIWindowSpawn()
{
	if(!m_pLoginRGAPIWindow)
	{
      //SP2 Steam, HACK, frmBaseWindow est utiliser pour créer la fenetre login RGAPI et register RGAPI car il manque des images pour utiliser gl editor
		m_pLoginRGAPIWindow = (GLoginRGAPIWindow *)g_Joshua.GUIManager()->GetNewGUIObjectInternal(GLoginRGAPIWindow::TypeName, L"frmRGAPIlogin", m_pBackgroundEarth, true, true);
		return true;
	}

   //if (ObjectSpawn((GUI::GBaseObject**)&m_pLoginRGAPIWindow,GLoginRGAPIWindow::TypeName, true, true, m_pBackgroundEarth))
    //  return true;

   return false;
}

bool GDataDisplayLayer::LoginRGAPIWindowKill()
{
	return ObjectKill((GUI::GBaseObject**)&m_pLoginRGAPIWindow);
}

GLoginRGAPIWindow* GDataDisplayLayer::GetLoginRGAPIWindow()
{
   return m_pLoginRGAPIWindow;
}

GJoinMPGameWindow* GDataDisplayLayer::JoinMPGameWindow()
{
   return m_pJoinMPGameWindow;
}

bool GDataDisplayLayer::JoinMPGameWindowSpawn()
{
   GDZLOG(EDZLogLevel::Entry, L"");

   if(!ObjectSpawn((GUI::GBaseObject**)&m_pJoinMPGameWindow, GJoinMPGameWindow::TypeName, true, true, m_pBackgroundEarth))
   {
       GDZLOG(EDZLogLevel::Exit, L"Returning false");
       return false;
   }

   GDZLOG(EDZLogLevel::Exit, L"Returning true");
   return true;  
}

bool GDataDisplayLayer::JoinMPGameWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pJoinMPGameWindow);
}

/*
bool GDataDisplayLayer::SelectedCountryWindowSpawn()
{
   //If already exists | displayed
   if(m_pSelectedCountryWindow)
   {
      if(!m_pSelectedCountryWindow->Visible())
         m_pSelectedCountryWindow->Show();
      return true;
   }


   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);


   m_pSelectedCountryWindow = (GSelectedCountryWindow*)g_Joshua.GUIManager()->GetNewGUIObjectAsChild(SP2::SELECTED_COUNTRY_WINDOW_OBJECT,SP2::SELECTED_COUNTRY_WINDOW_OBJECT,m_pBackgroundEarth,true);
   if (!m_pSelectedCountryWindow)
      return false;

   m_pSelectedCountryWindow->Position(15,(INT16)(mode.m_Resolution.y - 50 ) );
   return true;
}

bool GDataDisplayLayer::SelectedCountryWindowKill()
{
   if(!m_pSelectedCountryWindow)
      return true;

   g_Joshua.GUIManager()->ReleaseObjectAsync(m_pSelectedCountryWindow);

   m_pSelectedCountryWindow = NULL;

   return true;
}
*/
void GDataDisplayLayer::SelectedCountryWindowDisplayCountry(GString in_sFlagFile, GString in_sCountryName)
{
   //Set back the normal color
   ((GButton*)m_pMainBar->Child(L"btnFlag"))->ColorNormal(L"RGBA: 255, 255, 255",L"255");
   
   //Display the flag
   ((GButton*)m_pMainBar->Child(L"btnFlag"))->PictureNormal(in_sFlagFile);

   //Display the name of the selected country in the combo box
   ((GUI::GComboBox*)m_pMainBar->Child("cboCountry"))->Selected_Content(in_sCountryName);
}


GAdvisorWindow* GDataDisplayLayer::AdvisorWindow()
{
   return m_pAdvisorWindow;
}

bool GDataDisplayLayer::AdvisorWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pAdvisorWindow, GAdvisorWindow::TypeName, false, true, m_pBackgroundEarth))
      return true;

   return false;
}

bool GDataDisplayLayer::AdvisorWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pAdvisorWindow);
}

GAdvisorSettingsWindow* GDataDisplayLayer::AdvisorSettingsWindow()
{
   return m_pAdvisorSettingsWindow;
}

bool GDataDisplayLayer::AdvisorSettingsWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pAdvisorSettingsWindow, GAdvisorSettingsWindow::TypeName, false, true, m_pBackgroundEarth))
      return true;

   return false;
}

bool GDataDisplayLayer::AdvisorSettingsWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pAdvisorSettingsWindow);
}

GMiniMap* GDataDisplayLayer::MiniMap()
{
   return m_pMiniMap;
}

bool GDataDisplayLayer::MiniMapSpawn()
{
   CGFX_Renderer_Init_Mode mode;
   g_Joshua.Renderer()->Get_Current_Mode(&mode);

   if (!ObjectSpawn((GUI::GBaseObject**)&m_pMiniMap,GMiniMap::TypeName, false, true, m_pBackgroundEarth))
      return false;

   // Position minimap in lower rigth corner
   INT32 l_Width = m_pMiniMap->Property(DIALOG_WIDTH_NODE).ToINT32();
   INT32 l_Height = m_pMiniMap->Property(DIALOG_HEIGHT_NODE).ToINT32();
   m_pMiniMap->Position((INT16)(mode.m_Resolution.x - l_Width),(INT16)(mode.m_Resolution.y - l_Height));

   SEarthState & l_State = g_ClientDCL.m_EarthState;
   l_State.m_Zoom = 0;
   l_State.m_Latitude = 0;
   l_State.m_Longitude = 0;
   
   g_ClientDCL.UpdateWorldPosition(l_State);

   return true;
}

bool GDataDisplayLayer::MiniMapKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pMiniMap);
}

GResourcesWindow* GDataDisplayLayer::ResourcesWindow()
{
   return m_pResourcesWindow;
}

bool GDataDisplayLayer::ResourcesWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pResourcesWindow,GResourcesWindow::TypeName, g_ClientDAL.DataResourcesWindow.PreviouslyShown != 0, true, m_pBackgroundEarth))
      return true;

   return false;
}

bool GDataDisplayLayer::ResourcesWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pResourcesWindow);
}

GIncreaseProductionWindow* GDataDisplayLayer::IncreaseProductionWindow()
{
   return m_pIncreaseProductionWindow;
}

bool GDataDisplayLayer::IncreaseProductionWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pIncreaseProductionWindow,GIncreaseProductionWindow::TypeName, false, true, m_pBackgroundEarth))
      return true;

   return false;
}

bool GDataDisplayLayer::IncreaseProductionWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pIncreaseProductionWindow);
}

//Progress meter functions
GProgressMeter* GDataDisplayLayer::ProgressMeter()
{
   return m_pProgressMeter;
}
bool GDataDisplayLayer::ProgressMeterSpawn()
{
   //If already exists | displayed
   if(m_pProgressMeter)
   {
      if(!m_pProgressMeter->Visible())
         m_pProgressMeter->Show();
      return true;
   }

   m_pProgressMeter = CREATE_GUI_OBJECT_VISIBLE(GProgressMeter, NULL);
   if (!m_pProgressMeter)
      return false;
   return true;
}

bool GDataDisplayLayer::ProgressMeterKill()
{
   if(!m_pProgressMeter)
      return true;

   m_pProgressMeter->Hide();

   g_Joshua.GUIManager()->ReleaseObjectAsync(m_pProgressMeter);
   m_pProgressMeter = NULL;

   return true;
}

bool GDataDisplayLayer::ObjectSpawn(GUI::GBaseObject** in_pObject,
                                    const GString& in_sType,
                                    bool in_bVisible, 
                                    bool in_bFocusable,
                                    GUI::GBaseObject* in_pParent)
{
   GDZLOG(EDZLogLevel::Entry, L"in_pObject = " + GDZDebug::FormatPtr(in_pObject) + L", " +
          L"in_sType = " + in_sType + L", " +
          L"in_bVisible = " + GString(in_bVisible) + L", " +
          L"in_bFocusable = " + GString(in_bFocusable) + L", " +
          L"in_pParent = " + GDZDebug::FormatPtr(in_pParent));
    
   //If already exists | displayed   
   if(*in_pObject)
   {
      if(in_bVisible && !(*in_pObject)->Visible())
         (*in_pObject)->Show();

      GDZLOG(EDZLogLevel::Exit, L"Returning true");
      return true;
   }

   *in_pObject = CREATE_GUI_OBJECT(in_sType, in_sType, in_pParent, in_bVisible, in_bFocusable);
   m_vSpawnedWindows.push_back(*in_pObject);

   if (!*in_pObject)
   {
#ifdef GOLEM_DEBUG
      g_Joshua.Log(L"Couldn't spawn object : " + in_sType,MSGTYPE_ERROR);
#endif
      GDZLOG(EDZLogLevel::Exit, L"Returning false");
      return false;
   }
   else
   {
#ifdef GOLEM_DEBUG
      g_Joshua.Log(GString("Spawning Object : ") + (*in_pObject)->Name() + GString(" at : ") + GString(m_vSpawnedWindows.size() - 1) );
#endif
   }

   GDZLOG(EDZLogLevel::Exit, L"Returning true");
   return true;

}
bool GDataDisplayLayer::ObjectKill(GUI::GBaseObject** in_pObject)
{
    if(!(*in_pObject))
      return true;

   g_Joshua.GUIManager()->ReleaseObjectAsync(*in_pObject);
   vector<GUI::GBaseObject*>::iterator l_It = find(m_vSpawnedWindows.begin(), m_vSpawnedWindows.end(), *in_pObject);
   
   if (l_It != m_vSpawnedWindows.end())
   {
#ifdef GOLEM_DEBUG
      g_Joshua.Log(GString("Killing Object : ") + (*in_pObject)->Name() + GString(" at : ") + GString(l_It - m_vSpawnedWindows.begin() ) );
#endif
      m_vSpawnedWindows.erase(l_It);
   }

   *in_pObject = NULL;

   return true;  
}

GFrame* GDataDisplayLayer::SplashScreen()
{
   return m_pSplashScreen;
}

bool GDataDisplayLayer::SplashScreenSpawn()
{
   bool l_bResult = ObjectSpawn((GUI::GBaseObject**)&m_pSplashScreen, L"Splash_Screen", true, false);
   m_pSplashScreen->PictureNormal(GString("Data\\Textures\\SplashScreen\\") + GString((UINT32)(timeGetTime()%7)) + GString(".dds") );
   ((GFX::GWindow*)m_pSplashScreen->Model()->Get_Primitive())->ImageMinFilter(GFX::FILTER_LINEAR);
   ((GFX::GWindow*)m_pSplashScreen->Model()->Get_Primitive())->ImageMagFilter(GFX::FILTER_LINEAR);
   return l_bResult;
}

bool GDataDisplayLayer::SplashScreenKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pSplashScreen);
}

GContextMenu* GDataDisplayLayer::ContextMenuSpawn()
{
   if(m_pContextMenu)
      m_pContextMenu->Kill();
   m_pContextMenu = CREATE_GUI_OBJECT_VISIBLE(GContextMenu, NULL);
   return m_pContextMenu;   
}

GContextMenu* GDataDisplayLayer::ContextMenu()
{
   return m_pContextMenu;
}

void GDataDisplayLayer::ContextMenuKill()
{
   if(m_pContextMenu)
   {
      g_Joshua.GUIManager()->ReleaseObjectAsync(m_pContextMenu);
      m_pContextMenu = NULL;
   }
}

/*!
* Populates the main bar list of countries
**/
void GDataDisplayLayer::ComboboxSetCountries(const SP2::GCountries& in_iCountries, GUI::GComboBox* in_pCombo)
{
   //Fill the Combo Box that allows to select a different country from 
   //it with the dirrerent values

   GCountries::const_iterator l_It = in_iCountries.begin();
   map<GString, INT32> l_Countries;
   while(l_It != in_iCountries.end())
   {
      if(l_It->IsActive() )
      {
         l_Countries.insert(map<GString, INT32>::value_type(l_It->Name(),l_It->Id()));
      }
      l_It++;
   }

   map<GString, INT32>::iterator l_It2 = l_Countries.begin();
   while(l_It2 != l_Countries.end())
   {
      in_pCombo->Add_Item(l_It2->first);
      l_It2++;
   }
}

void GDataDisplayLayer::ComboboxSetCountriesAndFlag(const SP2::GCountries& in_iCountries, GUI::GComboBox* in_pCombo, 
                                                    bool in_bUseListBoxLessCbo)
{
   //Fill the Combo Box that allows to select a different country from 
   //it with the dirrerent values
   GCountries::const_iterator l_It = in_iCountries.begin();
   // real combo to be feed in the function
   GUI::GComboBox* l_pCombo = in_pCombo;

   // need to typecast to special cbo type ?
   if(in_bUseListBoxLessCbo)
   {
      l_pCombo = dynamic_cast<GComboListBoxLess* >(in_pCombo);
      if(l_pCombo)
      {
         CountriesListBoxFillUp( in_iCountries );
      }
      return;
   }

   // retrieve the listbox associated with our cbo
   GUI::GListBox* l_pListbox = l_pCombo->GetListBox();

   // feed the map with our countries
   map<GString, INT32> l_Countries;
   while(l_It != in_iCountries.end())
   {
      if( !l_It->IsActive() )
      {
         l_It++;
         continue;
      }

      l_Countries.insert(map<GString, INT32>::value_type(l_It->Name(),l_It->Id()));
      l_It++;
   }

   // loop into the map to feed the cbo
   map<GString, INT32>::iterator l_It2 = l_Countries.begin();
   while(l_It2 != l_Countries.end())
   {
      const GCountry& l_pCtry = g_ClientDAL.Country(l_It2->second);

      // add the item in our cbo
      l_pCombo->Add_Item(l_It2->first);
      l_It2++;
      
      // get a ptr to our current row to set its props
      GUI::GListBox::GRow* l_pRow   = l_pListbox->Get_Row_At(l_pListbox->Get_Nb_Rows() - 1);
      GUI::GPictureBox*    l_pFlag  = NULL;
      GUI::GLabel*         l_pLabel = NULL;
      
      // do we use a standard cbo ?
      if(!in_bUseListBoxLessCbo)
      {
         // yes ? proceed normally by creating the picture box
         l_pLabel = (GUI::GLabel*)l_pRow->Object();
         l_pFlag  = CREATE_GUI_OBJECT_NAMED_VISIBLE(GUI::GPictureBox, L"picFlag", l_pRow->m_pObject);
         // text offset so we can display the flag
         l_pLabel->TextOffset(25, 0);
      }
      else
      {
         // no ? retrieve the objects in our row and set the frame props
         GUI::GFrame* l_pFrame = NULL;
         l_pLabel = (GUI::GLabel*)l_pRow->Object()->Child("txtName");
         l_pFlag  = (GUI::GPictureBox*)l_pRow->Object()->Child("picFlag");
         // frame get succesfully ?
         if((l_pFrame = (GUI::GFrame*)l_pRow->Object()) != NULL)
         {
            // Default look and feel 
            l_pFrame->Height(14);         
            l_pFrame->Borders(GBorders(1, 1, 1, 1));

            // Default picture 
            l_pFrame->PictureNormal("000647");
            l_pLabel->TextOffset(2, 0);
            l_pLabel->FontColorNormal( GColorRGBInt(255,255,255,255) );
         }
      }      
      // render the flag
      l_pFlag->Position(0, 0);
      l_pFlag->Size(22, 13);
      l_pFlag->PictureNormal(m_vSmallFlagsTextures[l_pCtry.Id()]);
   }

   g_Joshua.GUIManager()->SetDirty();
}

void GDataDisplayLayer::ComboboxSetCountriesCodeAndFlag(const SP2::GCountries& in_iCountries, GUI::GComboBox* in_pCombo)
{
   //Fill the Combo Box that allows to select a different country from 
   //it with the dirrerent values

   GCountries::const_iterator l_It = in_iCountries.begin();
   GUI::GListBox* l_pListbox = in_pCombo->GetListBox();
   map<GString, INT32> l_Countries;

   while(l_It != in_iCountries.end())
   {
      l_Countries.insert(map<GString, INT32>::value_type(GString(l_It->Flag()).substr(0,3),l_It->Id()));
      l_It++;
   }

   map<GString, INT32>::iterator l_It2 = l_Countries.begin();
   while(l_It2 != l_Countries.end())
   {
      const GCountry& l_pCtry = g_ClientDAL.Country(l_It2->second);

      in_pCombo->Add_Item(l_It2->first);
      l_It2++;

      GUI::GListBox::GRow* l_pRow = l_pListbox->Get_Row_At(l_pListbox->Get_Nb_Rows() - 1);
      GUI::GLabel* l_pLabel = (GUI::GLabel*)l_pRow->Object();
      l_pLabel->TextOffset(24, 0);

      GUI::GPictureBox* l_pFlag = CREATE_GUI_OBJECT_NAMED_VISIBLE(GUI::GPictureBox, L"picFlag", l_pRow->m_pObject);
      l_pFlag->Position(0, 0);
      l_pFlag->Size(22, 13);
      l_pFlag->PictureNormal(m_vSmallFlagsTextures[l_pCtry.Id()]);
   }

   g_Joshua.GUIManager()->SetDirty();
}
/*
void GDataDisplayLayer::ListBoxSetCountriesCodeAndFlag(const SP2::GCountries& in_vCountries,
                                                       GUI::GListBox* in_pListBox2Fill)
{
   gassert(in_pListBox2Fill,L"GDataDisplayLayer::ListBoxSetCountriesCodeAndFlag Invalid null pointer");
   //Fill the Combo Box that allows to select a different country from 
   //it with the dirrerent values

   GCountries::const_iterator l_It = in_vCountries.begin();
   map<GString, INT32> l_Countries;

   while(l_It != in_vCountries.end())
   {
      l_Countries.insert(map<GString, INT32>::value_type(GString(l_It->Flag()).substr(0,3),l_It->Id()));
      l_It++;
   }

   map<GString, INT32>::iterator l_It2 = l_Countries.begin();
   while(l_It2 != l_Countries.end())
   {
      const GCountry& l_Ctry = g_ClientDAL.Country(l_It2->second);

      GUI::GListBox::GRow* l_pRow = in_pListBox2Fill->Insert_New_Row();
      GUI::GLabel* l_pLabel = (GUI::GLabel*)l_pRow->Object();
      l_pLabel->TextOffset(24, 0);

      GUI::GPictureBox* l_pFlag = CREATE_GUI_OBJECT_NAMED_VISIBLE(GUI::GPictureBox, L"picFlag", l_pRow->m_pObject);
      l_pFlag->Position(0, 0);
      l_pFlag->Size(22, 13);
      l_pFlag->PictureNormal(GString(FLAG_SMALL_DEFAULT_FOLDER)+ l_Ctry.Flag(), false);
   }

   g_Joshua.GUIManager()->SetDirty();
}
*/

//Internal laws window functions
GInternalLawsWindow* GDataDisplayLayer::InternalLawsWindow()
{
   return m_pInternalLawsWindow;
}

bool GDataDisplayLayer::InternalLawsWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pInternalLawsWindow,GInternalLawsWindow::TypeName, g_ClientDAL.DataInternalLawsWindow[0].PreviouslyShown != 0, true, m_pBackgroundEarth))
      return true;

   return false;

}

bool GDataDisplayLayer::InternalLawsWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pInternalLawsWindow);
}

//!Popup a msg window
/*!
 * Popup a msg window which will be automatically deleted when the ok button is pressed
 **/
void GDataDisplayLayer::PopupMessage(GString in_sMsg)
{
   static UINT32 l_MsgId = 0;
   SP2::GErrorWindow* l_pWindow = CREATE_GUI_OBJECT_NAMED_VISIBLE(GErrorWindow, L"ERRORMSG" + GString(l_MsgId), NULL);      
   if(l_pWindow)
      l_pWindow->Set(in_sMsg);
   return;
}

bool GDataDisplayLayer::GameLobbySpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pGameLobbyWindow,GGameLobbyWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}


bool GDataDisplayLayer::GameLobbyKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pGameLobbyWindow);
}

GGameLobbyWindow * GDataDisplayLayer::GameLobby()
{
   return m_pGameLobbyWindow;
}

bool GDataDisplayLayer::StartSinglePlayerWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pStartSinglePlayerWindow,GStartSinglePlayerWindow::TypeName, true, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::StartSinglePlayerWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pStartSinglePlayerWindow);
}

GStartSinglePlayerWindow* GDataDisplayLayer::StartSinglePlayerWindow()
{
   return m_pStartSinglePlayerWindow;
}

bool GDataDisplayLayer::TestWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pTestWindow,GTestWindow::TypeName, true, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::TestWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pTestWindow);
}

GTestWindow * GDataDisplayLayer::TestWindow()
{
   return m_pTestWindow;
}

GUnitAttackConfirmWindow* GDataDisplayLayer::UnitAttackConfirmWindow(UINT32 in_iTargetCountry)
{
   return m_UnitAttackConfirmWindows.count(in_iTargetCountry) ? m_UnitAttackConfirmWindows[in_iTargetCountry] : NULL;
}

bool GDataDisplayLayer::UnitAttackConfirmWindowSpawn(UINT32 in_iTargetCountry)
{
   if(m_UnitAttackConfirmWindows.count(in_iTargetCountry) == 0)
   {
      GUnitAttackConfirmWindow** l_pWindow = &(m_UnitAttackConfirmWindows[in_iTargetCountry] );
      *l_pWindow = NULL;
      if(ObjectSpawn( (GUI::GBaseObject**) l_pWindow, GUnitAttackConfirmWindow::TypeName, false, true, m_pBackgroundEarth) )
      {
         m_UnitAttackConfirmWindows[in_iTargetCountry]->TargetCountry(in_iTargetCountry);
         return true;
      }
      return false;
   }

   return true;
}

bool GDataDisplayLayer::UnitAttackConfirmWindowKill(UINT32 in_iID)
{
   if(m_UnitAttackConfirmWindows.count(in_iID) )
   {
      ObjectKill( (GUI::GBaseObject**) &(m_UnitAttackConfirmWindows[in_iID] ) );
      m_UnitAttackConfirmWindows.erase(in_iID);
   }

   return true;
}

bool GDataDisplayLayer::UnitAttackConfirmWindowKillAll()
{
   hash_map<UINT32, GUnitAttackConfirmWindow*>::iterator it = m_UnitAttackConfirmWindows.begin();
   while(it != m_UnitAttackConfirmWindows.end() )
   {
      ObjectKill( (GUI::GBaseObject**) &(it->second) );
      ++ it;
   }
   m_UnitAttackConfirmWindows.clear();

   return true;
}

GUnitMovementDenialWindow* GDataDisplayLayer::UnitMovementDenialWindow(UINT32 in_iID)
{
   return m_UnitMovementDenialWindows.count(in_iID) ? m_UnitMovementDenialWindows[in_iID] : NULL;
}

UINT32 GDataDisplayLayer::UnitMovementDenialWindowSpawn()
{
   GUnitMovementDenialWindow* l_pWindow = NULL;
   if(ObjectSpawn( (GUI::GBaseObject**) &l_pWindow, GUnitMovementDenialWindow::TypeName, false, true, m_pBackgroundEarth) )
   {
      l_pWindow->ID(m_iUnitMovementDenialNextID);
      m_UnitMovementDenialWindows[m_iUnitMovementDenialNextID] = l_pWindow;
      m_iUnitMovementDenialNextID ++;

      return l_pWindow->ID();
   }

   return 0xFFFFFFFF;
}

bool GDataDisplayLayer::UnitMovementDenialWindowKill(UINT32 in_iID)
{
   if(m_UnitMovementDenialWindows.count(in_iID) )
   {
      ObjectKill( (GUI::GBaseObject**) &(m_UnitMovementDenialWindows[in_iID] ) );
      m_UnitMovementDenialWindows.erase(in_iID);
   }

   return true;
}

bool GDataDisplayLayer::UnitMovementDenialWindowKillAll()
{
   hash_map<UINT32, GUnitMovementDenialWindow*>::iterator it = m_UnitMovementDenialWindows.begin();
   while(it != m_UnitMovementDenialWindows.end() )
   {
      ObjectKill( (GUI::GBaseObject**) &(it->second) );
      ++ it;
   }
   m_UnitMovementDenialWindows.clear();

   return true;
}

GUnitGroundDeploymentWindow* GDataDisplayLayer::UnitGroundDeploymentWindow()
{
   return m_pUnitGroundDeploymentWindow;
}

bool GDataDisplayLayer::UnitGroundDeploymentWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pUnitGroundDeploymentWindow,GUnitGroundDeploymentWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::UnitGroundDeploymentWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pUnitGroundDeploymentWindow);
}

bool GDataDisplayLayer::DesignWindowSpawn(SP2::GUnitDesign* in_pDesign)
{
   bool l_bSpawnSuccessful = false;
   if (ObjectSpawn((GUI::GBaseObject**)&m_pDesignWindow,GDesignWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      l_bSpawnSuccessful = true;
   }

   if(in_pDesign)
   {
      m_pDesignWindow->SelectedDesign(in_pDesign);
      return true;
   }

   if(l_bSpawnSuccessful)
      return true;
   else
      return false;

}

bool GDataDisplayLayer::DesignWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pDesignWindow);
}

GDesignWindow* GDataDisplayLayer::DesignWindow()
{
   return m_pDesignWindow;
}


bool GDataDisplayLayer::CreateDesignWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pCreateDesignWindow,GCreateDesignWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::CreateDesignWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pCreateDesignWindow);
}

GCreateDesignWindow* GDataDisplayLayer::CreateDesignWindow()
{
   return m_pCreateDesignWindow;
}



bool GDataDisplayLayer::UnitInfoSmallWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pUnitInfoSmallWindow,GUnitInfoSmallWindow::TypeName, false, false, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::UnitInfoSmallWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pUnitInfoSmallWindow);
}

GUnitInfoSmallWindow* GDataDisplayLayer::UnitInfoSmallWindow()
{
   return m_pUnitInfoSmallWindow;
}

GUnitListWindow* GDataDisplayLayer::UnitListWindow()
{
   return m_pUnitListWindow;
}

bool GDataDisplayLayer::UnitListWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pUnitListWindow,GUnitListWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::UnitListWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pUnitListWindow);
}

GUnitListWindowNew* GDataDisplayLayer::UnitListWindowNew()
{
   return m_pUnitListWindowNew;
}

bool GDataDisplayLayer::UnitListWindowNewSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pUnitListWindowNew,GUnitListWindowNew::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::UnitListWindowNewKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pUnitListWindowNew);
}

GUnitSplitWindow* GDataDisplayLayer::UnitSplitWindow()
{
   return m_pUnitSplitWindow;
}

bool GDataDisplayLayer::UnitSplitWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pUnitSplitWindow,GUnitSplitWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::UnitSplitWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pUnitSplitWindow);
}

GGroupSplitWindow* GDataDisplayLayer::GroupSplitWindow()
{
   return m_pGroupSplitWindow;
}

bool GDataDisplayLayer::GroupSplitWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pGroupSplitWindow,GGroupSplitWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::GroupSplitWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pGroupSplitWindow);
}

GUnitProductionWindow* GDataDisplayLayer::UnitProductionWindow()
{
   return m_pUnitProductionWindow;
}

bool GDataDisplayLayer::UnitProductionWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pUnitProductionWindow,GUnitProductionWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::UnitProductionWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pUnitProductionWindow);
}



GUsedUnitWindow* GDataDisplayLayer::UsedUnitWindow()
{
   return m_pUsedUnitWindow;
}

bool GDataDisplayLayer::UsedUnitWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pUsedUnitWindow,GUsedUnitWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::UsedUnitWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pUsedUnitWindow);
}




GUnitResearchWindow* GDataDisplayLayer::UnitResearchWindow()
{
   return m_pUnitResearchWindow;
}

bool GDataDisplayLayer::UnitResearchWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pUnitResearchWindow,GUnitResearchWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::UnitResearchWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pUnitResearchWindow);
}

//***************** SPHERE CONTROL WINDOW**********************

bool GDataDisplayLayer::SphereControlWindowSpawn()
{
   return ObjectSpawn((GUI::GBaseObject**)&m_pSphereControlWindow,GSphereControlWindow::TypeName, false, false, m_pBackgroundEarth);
}

bool GDataDisplayLayer::SphereControlWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pSphereControlWindow);
}


GSphereControlWindow* GDataDisplayLayer::SphereControlWindow()
{
	return m_pSphereControlWindow;
}

//***************** STRATEGIC WARFARE RELATED STUFF **********************
//GStrategicAlertWindow* GDataDisplayLayer::StrategicAlertWindow()
//{
//   return m_pStrategicAlertWindow;
//}

GStrategicLaunchWindow* GDataDisplayLayer::StrategicLaunchWindow()
{
   return m_pStrategicLaunchWindow;
}

GStrategicMissileListWindow* GDataDisplayLayer::StrategicMissileListWindow()
{
   return m_pStrategicMissileListWindow;
}

//GStrategicNewsWindow* GDataDisplayLayer::StrategicNewsWindow()
//{
//   return m_pStrategicNewsWindow;
//}

GStrategicTargetCountryWindow* GDataDisplayLayer::StrategicTargetCountryWindow()
{
   return m_pStrategicTargetCountryWindow;
}

GStrategicTargetMultipleCountriesWindow* GDataDisplayLayer::StrategicTargetMultipleCountriesWindow()
{
   return m_pStrategicTargetMultipleCountriesWindow;
}

//bool GDataDisplayLayer::StrategicAlertWindowSpawn()
//{
//   if (ObjectSpawn((GUI::GBaseObject**)&m_pStrategicAlertWindow,GStrategicAlertWindow::TypeName, false, true, m_pBackgroundEarth))
//   {
//      return true;
//   }
//   return false;
//}

bool GDataDisplayLayer::StrategicLaunchWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pStrategicLaunchWindow,GStrategicLaunchWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::StrategicMissileListWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pStrategicMissileListWindow,GStrategicMissileListWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

//bool GDataDisplayLayer::StrategicNewsWindowSpawn()
//{
//   if (ObjectSpawn((GUI::GBaseObject**)&m_pStrategicNewsWindow,GStrategicNewsWindow::TypeName, false, true, m_pBackgroundEarth))
//   {
//      return true;
//   }
//   return false;
//}

bool GDataDisplayLayer::StrategicTargetCountryWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pStrategicTargetCountryWindow,GStrategicTargetCountryWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::StrategicTargetMultipleCountriesWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pStrategicTargetMultipleCountriesWindow,GStrategicTargetMultipleCountriesWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

//bool GDataDisplayLayer::StrategicAlertWindowKill()
//{
//   return ObjectKill((GUI::GBaseObject**)&m_pStrategicAlertWindow);
//}

bool GDataDisplayLayer::StrategicLaunchWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pStrategicLaunchWindow);
}

bool GDataDisplayLayer::StrategicMissileListWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pStrategicMissileListWindow);
}

//bool GDataDisplayLayer::StrategicNewsWindowKill()
//{
//   return ObjectKill((GUI::GBaseObject**)&m_pStrategicNewsWindow);
//}

bool GDataDisplayLayer::StrategicTargetCountryWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pStrategicTargetCountryWindow);
}

bool GDataDisplayLayer::StrategicTargetMultipleCountriesWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pStrategicTargetMultipleCountriesWindow);
}

GStrategicSelectTargetWindow* GDataDisplayLayer::StrategicSelectTargetWindow()
{
   return m_pStrategicSelectTargetWindow;
}

bool GDataDisplayLayer::StrategicSelectTargetWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pStrategicSelectTargetWindow,GStrategicSelectTargetWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::StrategicSelectTargetWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pStrategicSelectTargetWindow);
}

//***************** END OF STRATEGIC WARFARE RELATED STUFF **********************

void GDataDisplayLayer::SwapToStrategicWarfare()
{
   //g_ClientDDL.SpawnStrategicWarfare();

   for(UINT16 i=0; i<m_vSpawnedWindows.size(); i++)
   {
      if((m_vSpawnedWindows[i])->Visible() && (m_vSpawnedWindows[i] != m_pBackgroundEarth) && (m_vSpawnedWindows[i] != m_pMainBar) )
      {
         m_vPreviouslyDisplayedWindows.push_back(m_vSpawnedWindows[i]);
         if(m_vSpawnedWindows[i] != m_pMiniMap)
         {
            (m_vSpawnedWindows[i])->Hide();
         }
      }
   }

   MiniMap()->ToggleMapLook(false);

   //if(!StrategicNewsWindow())
   //   StrategicNewsWindowSpawn();
   
   //if(StrategicNewsWindow()->Visible())
   //   StrategicNewsWindow()->Hide();
   //StrategicNewsWindow()->Show();

   if(!StrategicTargetCountryWindow())
      StrategicTargetCountryWindowSpawn();

   if(StrategicTargetCountryWindow()->Visible())
      StrategicTargetCountryWindow()->Hide();
   StrategicTargetCountryWindow()->Show();
   
   BackgroundEarth()->SwitchView(SP2::EBackgroundView::Strategic,1.f);

   AnimationPlaySliding(GFX::PLAYBACK_REVERSE, MainBar()->AnimationSlidingTrack() );
   AnimationPlaySliding(GFX::PLAYBACK_FORWARD, StrategicTargetCountryWindow()->AnimationSlidingTrack() );
   //AnimationPlaySliding(GFX::PLAYBACK_FORWARD, StrategicNewsWindow()->AnimationSlidingTrack() );
}

void GDataDisplayLayer::SwapToMilitary()
{
   // E3 Hack - Fix (JMercier) its possible that we end up here event if it has been destroyed..(ASynch destruction + monkey typing)
   if(!StrategicTargetCountryWindow())
      return;

   MiniMap()->ToggleMapLook(true);

   //if(StrategicAlertWindow())
   //   StrategicAlertWindowKill();

   if(StrategicLaunchWindow())
      StrategicLaunchWindowKill();
   
   if(StrategicMissileListWindow())
      StrategicMissileListWindowKill();
     
   if(StrategicTargetMultipleCountriesWindow())
      StrategicTargetMultipleCountriesWindowKill();

   BackgroundEarth()->SwitchView(SP2::EBackgroundView::Realistic,1.f);
   
   AnimationPlaySliding(GFX::PLAYBACK_REVERSE, StrategicTargetCountryWindow()->AnimationSlidingTrack() );
   //AnimationPlaySliding(GFX::PLAYBACK_REVERSE, StrategicNewsWindow()->AnimationSlidingTrack() );

   if(MainBar()->Visible())
      MainBar()->Hide();
   MainBar()->Show();
   MainBar()->BringToFront();
   
   AnimationPlaySliding(GFX::PLAYBACK_FORWARD, MainBar()->AnimationSlidingTrack() );

   for(UINT16 i=0; i<m_vPreviouslyDisplayedWindows.size(); i++)
   {
      if(m_vPreviouslyDisplayedWindows[i])
         m_vPreviouslyDisplayedWindows[i]->Show();
   }

   g_ClientDDL.KillStrategicWarfare();

   m_vPreviouslyDisplayedWindows.clear();
}

/*!
 * Create the sliding animation for the given primitive
 *
 * @param  in_fAnimationTime : Animation total length
 * @param  in_fDelay         : Delay before animation
 * @param  in_iFromY         : Start y position
 * @param  in_iToY           : End y position
 * @param  in_pTransform     : Primitive to apply anim. on
 * @return CGFX_Track_Itf*   : The newly created track
 **/
CGFX_Track_Itf* GDataDisplayLayer::AnimationCreateSliding(REAL32 in_fAnimationTime, REAL32 in_fDelay, INT32 in_iFrom, INT32 in_iTo, CGFX_Transform_Itf* in_pTransform, bool in_bYAnim)
{
   CGFX_Key_Linear l_pAnimKeys[3] =
   {
      {0.f,                               (REAL32)in_iFrom},
      {in_fDelay,                         (REAL32)in_iFrom},
      {in_fAnimationTime + in_fDelay,     (REAL32)in_iTo},
      //{in_fAnimationTime + in_fDelay * 2, (REAL32)in_iToY},
   };

   CGFX_Action_Itf* l_pAction = g_Joshua.Renderer()->Create_Action();
   CGFX_Track_Itf*  l_pTrack  = g_Joshua.Renderer()->Create_Track();

   CGFX_Animation_Linear_Itf* l_pAnim = g_Joshua.Renderer()->Create_Animation_Linear();
   l_pAnim->Setup_Keys(l_pAnimKeys,3);
   
   if(in_bYAnim)
      l_pAnim->Connect(in_pTransform,GFX::PARAM_POSITION_Y);
   else
      l_pAnim->Connect(in_pTransform,GFX::PARAM_POSITION_X);

   l_pAction->Length(l_pAnimKeys[2].m_Time);
   l_pAction->Add_Animation(l_pAnim);

   l_pTrack->Set_Action(l_pAction);
   l_pTrack->ExtrapolationBefore(true);
   l_pTrack->ExtrapolationAfter(true);
   l_pTrack->Playback_Count(1);
   l_pTrack->Playback(GFX::PLAYBACK_FORWARD);
   l_pTrack->Update_Animations(0);

   SAFE_RELEASE(l_pAnim);
   SAFE_RELEASE(l_pAction);

   return l_pTrack;
}

/*!
 * Start the sliding animation
 * @param   in_PlaybackMode : Set the anim mode (Forward or Reverse)
 * @param   in_pTrack       : The track to play
 **/
void GDataDisplayLayer::AnimationPlaySliding(GFX::EPlayback_Type in_PlaybackMode, CGFX_Track_Itf* in_pTrack)
{
   CGFX_Scene_Itf* l_pScene = g_Joshua.GUIManager()->Scene();
   l_pScene->Get_Animation_System()->Remove_Track(in_pTrack);
   in_pTrack->Playback(in_PlaybackMode);
   in_pTrack->Set_Start_Time(l_pScene->Get_Animation_System()->Get_Animation_Time() );

   l_pScene->Get_Animation_System()->Add_Track(in_pTrack);
}

void GDataDisplayLayer::AnimationCreateRectangle()
{
   CGFX_Transform_2D_Itf* l_pTfx;
   GFX::GCascadePixelShader* l_pPS;
   if(!m_pModelRectangle)
   {
      // Create rectangle GFX objects
      m_pModelRectangle = g_Joshua.Renderer()->Create_Model();
      GFX::GMesh* l_pRectangle = g_Joshua.Renderer()->CreateMesh();
      l_pTfx = g_Joshua.Renderer()->Create_Transform_2D();
      GFX::GDefaultVertexShader* l_pVS;
      GFX::GMaterial* l_pMat = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, 
                                                                                    GFX::PSTYPE_DEF_FACTOR, 
                                                                                    &l_pVS,
                                                                                    &l_pPS);


      // Setup rectangle GFX object hierarchy
      g_Joshua.GUIManager()->Scene()->Get_Root_Model()->Add_Child(m_pModelRectangle);
      m_pModelRectangle->Set_Transform(l_pTfx);
      m_pModelRectangle->Set_Material(l_pMat);
      m_pModelRectangle->Set_Primitive(l_pRectangle);

      // Setup rectangle material
      GFX::GRenderStateCollection* l_pRS = l_pMat->RenderStates(0);
      l_pRS->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = 1;
      l_pRS->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      l_pRS->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;

      l_pPS->ColorFactor().A = 0.5f;
      l_pPS->ColorFactor().R = 0.25f;
      l_pPS->ColorFactor().G = 0.25f;
      l_pPS->ColorFactor().B = 1.f;

      // Setup rectangle mesh
      GShape& l_Shape = l_pRectangle->Shape();
      l_pRectangle->NbFacetGroups(1);
      l_pRectangle->FacetGroup(0).Facets().Type(FACETS_LINE_STRIP, false);

      static REAL32 s_pRectPositions[5][3] = 
      {
         {0, 0, 0},
         {0, 1, 0},
         {1, 1, 0},
         {1, 0, 0},
         {0, 0, 0},
      };
      l_Shape.SetupPositions( (GVector3D<REAL32>*) s_pRectPositions, 5);

      GLineStrip& l_RectLines = l_pRectangle->FacetGroup(0).Facets().LineStrip();
      l_RectLines.Start() = 0;
      l_RectLines.NbLines(4);

      l_pMat->Release();
      l_pVS->Release();
      l_pPS->Release();
      l_pRectangle->Release();
      l_pTfx->Release();

      g_Joshua.GUIManager()->SetDirty();
   }
   else
   {
      l_pTfx = (CGFX_Transform_2D_Itf*) m_pModelRectangle->Get_Transform();
      l_pPS = (GFX::GCascadePixelShader*) m_pModelRectangle->Get_Material()->PixelShader(0);
   }

   if(m_pActionRectangle)
   {
      g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Remove_Action(m_pActionRectangle);
      SAFE_RELEASE(m_pActionRectangle);
      m_pActionRectangle = NULL;
   }

   CGFX_Key_Linear l_BasicKey[2] =
   {
      {0.f,  0.f},
      {0.f,  0.f},
   };

   CGFX_Key_Constant l_VisibilityKey[2] =
   {
      {0.f, 1.f},
      {0.f, 0.f},
   };

   m_pActionRectangle = g_Joshua.Renderer()->Create_Action();

   CGFX_Animation_Linear_Itf* l_pPositionXAnim = g_Joshua.Renderer()->Create_Animation_Linear();
   l_pPositionXAnim->Setup_Keys(l_BasicKey, 2);
   l_pPositionXAnim->Connect(l_pTfx, GFX::PARAM_POSITION_X);

   CGFX_Animation_Linear_Itf* l_pPositionYAnim = g_Joshua.Renderer()->Create_Animation_Linear();
   l_pPositionYAnim->Setup_Keys(l_BasicKey, 2);
   l_pPositionYAnim->Connect(l_pTfx, GFX::PARAM_POSITION_Y);

   CGFX_Animation_Linear_Itf* l_pSizeXAnim = g_Joshua.Renderer()->Create_Animation_Linear();
   l_pSizeXAnim->Setup_Keys(l_BasicKey, 2);
   l_pSizeXAnim->Connect(l_pTfx, GFX::PARAM_SIZE_X);

   CGFX_Animation_Linear_Itf* l_pSizeYAnim = g_Joshua.Renderer()->Create_Animation_Linear();
   l_pSizeYAnim->Setup_Keys(l_BasicKey, 2);
   l_pSizeYAnim->Connect(l_pTfx, GFX::PARAM_SIZE_Y);
   
   CGFX_Animation_Linear_Itf* l_pColorAAnim = g_Joshua.Renderer()->Create_Animation_Linear();
   l_pColorAAnim->Setup_Keys(l_BasicKey, 2);
   l_pColorAAnim->Connect(l_pPS, GFX::PARAM_COLOR_FACTOR_A);

   CGFX_Animation_Linear_Itf* l_pColorRAnim = g_Joshua.Renderer()->Create_Animation_Linear();
   l_pColorRAnim->Setup_Keys(l_BasicKey, 2);
   l_pColorRAnim->Connect(l_pPS, GFX::PARAM_COLOR_FACTOR_R);

   CGFX_Animation_Linear_Itf* l_pColorGAnim = g_Joshua.Renderer()->Create_Animation_Linear();
   l_pColorGAnim->Setup_Keys(l_BasicKey, 2);
   l_pColorGAnim->Connect(l_pPS, GFX::PARAM_COLOR_FACTOR_G);

   CGFX_Animation_Linear_Itf* l_pColorBAnim = g_Joshua.Renderer()->Create_Animation_Linear();
   l_pColorBAnim->Setup_Keys(l_BasicKey, 2);
   l_pColorBAnim->Connect(l_pPS, GFX::PARAM_COLOR_FACTOR_B);

   CGFX_Animation_Constant_Itf* l_pVisibilityAnim = g_Joshua.Renderer()->Create_Animation_Constant();
   l_pVisibilityAnim->Setup_Keys(l_VisibilityKey, 2);
   l_pVisibilityAnim->Connect(m_pModelRectangle, GFX::PARAM_VISIBILITY);

   m_pActionRectangle->Length(1.f);
   m_pActionRectangle->Add_Animation(l_pPositionXAnim);
   m_pActionRectangle->Add_Animation(l_pPositionYAnim);
   m_pActionRectangle->Add_Animation(l_pSizeXAnim);
   m_pActionRectangle->Add_Animation(l_pSizeYAnim);
   m_pActionRectangle->Add_Animation(l_pColorAAnim);
   m_pActionRectangle->Add_Animation(l_pColorRAnim);
   m_pActionRectangle->Add_Animation(l_pColorGAnim);
   m_pActionRectangle->Add_Animation(l_pColorBAnim);
   m_pActionRectangle->Add_Animation(l_pVisibilityAnim);

   SAFE_RELEASE(l_pPositionXAnim);
   SAFE_RELEASE(l_pPositionYAnim);
   SAFE_RELEASE(l_pSizeXAnim);
   SAFE_RELEASE(l_pSizeYAnim);
   SAFE_RELEASE(l_pColorAAnim);
   SAFE_RELEASE(l_pColorRAnim);
   SAFE_RELEASE(l_pColorGAnim);
   SAFE_RELEASE(l_pColorBAnim);
   SAFE_RELEASE(l_pVisibilityAnim);
}

void GDataDisplayLayer::AnimationPlayRectangle(GUI::GBaseObject* in_pFromObj, GUI::GBaseObject* in_pToObj)
{
   GVector2D<INT32> l_FromPos    = in_pFromObj->RelativeToAbsolute(GVector2D<INT32>(0, 0));
   GVector2D<INT32> l_ToPos      = in_pToObj->RelativeToAbsolute(GVector2D<INT32>(0, 0));
   GVector2D<INT32> l_FromSize   = GVector2D<INT32>(in_pFromObj->Width(), in_pFromObj->Height());
   GVector2D<INT32> l_ToSize     = GVector2D<INT32>(in_pToObj->Width(),   in_pToObj->Height());

   GVector4D<REAL32> l_FromColor = GVector4D<REAL32>(0.5f, 0.5f, 0.5f, 0.5f);
   GVector4D<REAL32> l_ToColor   = GVector4D<REAL32>(1.0f, 1.0f, 1.0f, 1.0f);
   
   AnimationPlayRectangle(0.5f, l_FromPos, l_FromSize, l_FromColor, l_ToPos, l_ToSize, l_ToColor);
}

void GDataDisplayLayer::AnimationPlayRectangle(REAL32 in_fAnimationTime, GVector2D<INT32> in_FromPos, GVector2D<INT32> in_FromSize, GVector4D<REAL32> in_FromColor, GVector2D<INT32> in_ToPos, GVector2D<INT32> in_ToSize, GVector4D<REAL32> in_ToColor)
{
   CGFX_Scene_Itf* l_pScene = g_Joshua.GUIManager()->Scene();
   UINT32 l_iId = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Action_Index(m_pActionRectangle);
   if(l_iId == INVALID_INDEX)
   {
      l_iId = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Add_Action(m_pActionRectangle);
   }
   else
   {
      g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Stop_Action(l_iId);
   }

   CGFX_Key_Linear               l_pKey;
   CGFX_Key_Constant             l_pConstKey;
   CGFX_Animation_Linear_Itf*    l_pAnim;
   CGFX_Animation_Constant_Itf*  l_pConstAnim;
   
   l_pAnim = (CGFX_Animation_Linear_Itf*)m_pActionRectangle->Get_Animation(0);
   l_pAnim->Get_Key(0, l_pKey);
   l_pKey.m_Value = (REAL32)in_FromPos.x;
   l_pAnim->Set_Key(0, l_pKey);
   
   l_pAnim->Get_Key(1, l_pKey);
   l_pKey.m_Time  = in_fAnimationTime;
   l_pKey.m_Value = (REAL32)in_ToPos.x;
   l_pAnim->Set_Key(1, l_pKey);
   l_pAnim->Setup();

   l_pAnim = (CGFX_Animation_Linear_Itf*)m_pActionRectangle->Get_Animation(1);
   l_pAnim->Get_Key(0, l_pKey);
   l_pKey.m_Value = (REAL32)in_FromPos.y;
   l_pAnim->Set_Key(0, l_pKey);
   
   l_pAnim->Get_Key(1, l_pKey);
   l_pKey.m_Time  = in_fAnimationTime;
   l_pKey.m_Value = (REAL32)in_ToPos.y;
   l_pAnim->Set_Key(1, l_pKey);
   l_pAnim->Setup();

   l_pAnim = (CGFX_Animation_Linear_Itf*)m_pActionRectangle->Get_Animation(2);
   l_pAnim->Get_Key(0, l_pKey);
   l_pKey.m_Value = (REAL32)in_FromSize.x - 0.5f;
   l_pAnim->Set_Key(0, l_pKey);
   
   l_pAnim->Get_Key(1, l_pKey);
   l_pKey.m_Time  = in_fAnimationTime;
   l_pKey.m_Value = (REAL32)in_ToSize.x - 0.5f;
   l_pAnim->Set_Key(1, l_pKey);
   l_pAnim->Setup();

   l_pAnim = (CGFX_Animation_Linear_Itf*)m_pActionRectangle->Get_Animation(3);
   l_pAnim->Get_Key(0, l_pKey);
   l_pKey.m_Value = (REAL32)in_FromSize.y - 0.5f;
   l_pAnim->Set_Key(0, l_pKey);
   
   l_pAnim->Get_Key(1, l_pKey);
   l_pKey.m_Time  = in_fAnimationTime;
   l_pKey.m_Value = (REAL32)in_ToSize.y - 0.5f;
   l_pAnim->Set_Key(1, l_pKey);
   l_pAnim->Setup();

   l_pAnim = (CGFX_Animation_Linear_Itf*)m_pActionRectangle->Get_Animation(4);
   l_pAnim->Get_Key(0, l_pKey);
   l_pKey.m_Value = in_FromColor.w;
   l_pAnim->Set_Key(0, l_pKey);
   
   l_pAnim->Get_Key(1, l_pKey);
   l_pKey.m_Time  = in_fAnimationTime;
   l_pKey.m_Value = in_ToColor.w;
   l_pAnim->Set_Key(1, l_pKey);
   l_pAnim->Setup();

   l_pAnim = (CGFX_Animation_Linear_Itf*)m_pActionRectangle->Get_Animation(5);
   l_pAnim->Get_Key(0, l_pKey);
   l_pKey.m_Value = in_FromColor.x;
   l_pAnim->Set_Key(0, l_pKey);
   
   l_pAnim->Get_Key(1, l_pKey);
   l_pKey.m_Time  = in_fAnimationTime;
   l_pKey.m_Value = in_ToColor.x;
   l_pAnim->Set_Key(1, l_pKey);
   l_pAnim->Setup();

   l_pAnim = (CGFX_Animation_Linear_Itf*)m_pActionRectangle->Get_Animation(6);
   l_pAnim->Get_Key(0, l_pKey);
   l_pKey.m_Value = in_FromColor.y;
   l_pAnim->Set_Key(0, l_pKey);
   
   l_pAnim->Get_Key(1, l_pKey);
   l_pKey.m_Time  = in_fAnimationTime;
   l_pKey.m_Value = in_ToColor.y;
   l_pAnim->Set_Key(1, l_pKey);
   l_pAnim->Setup();

   l_pAnim = (CGFX_Animation_Linear_Itf*)m_pActionRectangle->Get_Animation(7);
   l_pAnim->Get_Key(0, l_pKey);
   l_pKey.m_Value = in_FromColor.z;
   l_pAnim->Set_Key(0, l_pKey);
   
   l_pAnim->Get_Key(1, l_pKey);
   l_pKey.m_Time  = in_fAnimationTime;
   l_pKey.m_Value = in_ToColor.z;
   l_pAnim->Set_Key(1, l_pKey);
   l_pAnim->Setup();

   l_pConstAnim = (CGFX_Animation_Constant_Itf*)m_pActionRectangle->Get_Animation(8);
   l_pConstAnim->Get_Key(1, l_pConstKey);
   l_pConstKey.m_Time = in_fAnimationTime;
   l_pConstAnim->Set_Key(1, l_pConstKey);
   l_pConstAnim->Setup();

   m_pActionRectangle->Length(in_fAnimationTime);
   l_pScene->Get_Animation_System()->Set_Action_Time(l_iId, 0);
   l_pScene->Get_Animation_System()->Play_Action(l_iId);
}


/*!
 * Spawn all the first level windows
 **/
void GDataDisplayLayer::SpawnWindowsLvl()
{
   PoliticWindowSpawn();
   MilitaryWindowSpawn();
   EconomicWindowSpawn();
   SaveConfirmationWindowSpawn();
   CountryInformationWindowSpawn();
   RelationsWindowSpawn();
   MailWindowSpawn();
   EventLogWindowSpawn();
}

/*!
 * Kill all the first level windows
 **/
void GDataDisplayLayer::KillWindowsLvl()
{
   EventLogWindowKill();
   MailWindowKill();
   RelationsWindowKill();
   CountryInformationWindowKill();
   EconomicWindowKill();
   MilitaryWindowKill();
   PoliticWindowKill();
}

/*!
 * Spawn all the second economic level windows
 **/
void GDataDisplayLayer::SpawnWindowsEconomicLv2()
{
   if(!BudgetWindow())
      BudgetWindowSpawn();
   
   if(!EconomicHealthWindow())
      EconomicHealthWindowSpawn();

   if(!ResourcesWindow())
      ResourcesWindowSpawn();
}

/*!
 * Spawn all the second politic level windows
 **/
void GDataDisplayLayer::SpawnWindowsPoliticLv2()
{
   if(!ConstitutionalFormWindow())
      ConstitutionalFormWindowSpawn();
   
   if(!InternalLawsWindow())
      InternalLawsWindowSpawn();

   if(!TreatiesWindow())
      TreatiesWindowSpawn();
}

/*!
 * Spawn all the second military level windows
 **/
void GDataDisplayLayer::SpawnWindowsMilitaryLv2()
{
   //if(!UnitListWindow())
   //   UnitListWindowSpawn();

   if(!UnitListWindowNew())
      UnitListWindowNewSpawn();

   if(!DesignWindow())
      DesignWindowSpawn();

   if(!UnitProductionWindow())
      UnitProductionWindowSpawn();

   if(!UnitResearchWindow())
      UnitResearchWindowSpawn();

   if(!UnitGroundDeploymentWindow())
      UnitGroundDeploymentWindowSpawn();

   if(!CovertActionsWindow())
      CovertActionsWindowSpawn();
}

void GDataDisplayLayer::SpawnStrategicWarfare()
{
   //StrategicAlertWindowSpawn();
   StrategicLaunchWindowSpawn();
   StrategicMissileListWindowSpawn();
   //StrategicNewsWindowSpawn();
   StrategicTargetCountryWindowSpawn();
   StrategicTargetMultipleCountriesWindowSpawn();
   StrategicSelectTargetWindowSpawn();
}

/*!
 * Destroy all second level economic windows
 * @param   in_bForce : Force destruction of windows
 **/
void GDataDisplayLayer::KillWindowsEconomicLv2(bool in_bForce)
{
   if(BudgetWindow() && (!BudgetWindow()->Visible() || in_bForce))
      BudgetWindowKill();

   if(EconomicHealthWindow() && (!EconomicHealthWindow()->Visible() || in_bForce))
      EconomicHealthWindowKill();

   if(IncreaseProductionWindow() && ( !IncreaseProductionWindow()->Visible() || in_bForce) && ( !ResourcesWindow() || !ResourcesWindow()->Visible() ) )
      IncreaseProductionWindowKill();

   if(ResourcesWindow() && ( !ResourcesWindow()->Visible() || in_bForce))
      ResourcesWindowKill();

   if(TradeDetailedWindow() && (!TradeDetailedWindow()->Visible() || in_bForce))
      TradeDetailedWindowKill();
}

/*!
 * Destroy all second level politic windows
 * @param   in_bForce : Force destruction of windows
 **/
void GDataDisplayLayer::KillWindowsPoliticLv2(bool in_bForce)
{
   if(ConstitutionalFormWindow() && (in_bForce || !ConstitutionalFormWindow()->Visible()))
      ConstitutionalFormWindowKill();
   
   if(InternalLawsWindow() && (in_bForce || !InternalLawsWindow()->Visible()))
      InternalLawsWindowKill();

   if(TreatiesWindow() && (in_bForce || !TreatiesWindow()->Visible()))
      TreatiesWindowKill();

   if(AddRemoveWindow() && (in_bForce || !m_pAddRemoveWindow->Visible())  && ( !TreatyDetailsWindow() || !TreatyDetailsWindow()->Visible() ))
      AddRemoveWindowKill();

   if(TreatyDetailsWindow() && (in_bForce || !TreatyDetailsWindow()->Visible()))
      TreatyDetailsWindowKill();
}

/*!
 * Destroy all second level military windows
 * @param   in_bForce : Force destruction of windows
 **/
void GDataDisplayLayer::KillWindowsMilitaryLv2(bool in_bForce)
{
   //if(UnitListWindow() && (in_bForce || !UnitListWindow()->Visible()))
   //   UnitListWindowKill();

   if(UnitListWindowNew() && (in_bForce || !UnitListWindowNew()->Visible()))
      UnitListWindowNewKill();

   if(DesignWindow() && (in_bForce || !DesignWindow()->Visible()))
      DesignWindowKill();

   if(CreateDesignWindow() && (in_bForce || !CreateDesignWindow()->Visible()))
      CreateDesignWindowKill();
   
   if(UnitProductionWindow() && (in_bForce || !UnitProductionWindow()->Visible()))
      UnitProductionWindowKill();
   
   if(UsedUnitWindow() && (in_bForce || !UsedUnitWindow()->Visible()))
      UsedUnitWindowKill();

   if(UnitResearchWindow() && (in_bForce || !UnitResearchWindow()->Visible()))
      UnitResearchWindowKill();

   if(UnitGroundDeploymentWindow() && (in_bForce || !UnitGroundDeploymentWindow()->Visible()))
      UnitGroundDeploymentWindowKill();

   if(UnitSplitWindow() && (in_bForce || !UnitSplitWindow()->Visible()))
      UnitSplitWindowKill();

   if(CovertActionsCellTrainingWindow() && (in_bForce || ( !CovertActionsCellTrainingWindow()->Visible() && ( !CovertActionsWindow() || !CovertActionsWindow()->Visible() ) ) ))
      CovertActionsCellTrainingWindowKill();

   if(CovertActionsNewCellWindow() && (in_bForce || ( !CovertActionsNewCellWindow()->Visible() && ( !CovertActionsWindow() || !CovertActionsWindow()->Visible() ) ) ))
      CovertActionsNewCellWindowKill();

   if(CovertActionsNewMissionWindow() && (in_bForce || ( !CovertActionsNewMissionWindow()->Visible() && ( !CovertActionsWindow() || !CovertActionsWindow()->Visible() ) ) ))
      CovertActionsNewMissionWindowKill();

   if(CovertActionsMissionOutcomeAttackedWindow() && (in_bForce || !CovertActionsMissionOutcomeAttackedWindow()->Visible()))
      CovertActionsMissionOutcomeAttackedWindowKill();

   if(CovertActionsMissionOutcomeWindow() && (in_bForce || !CovertActionsMissionOutcomeWindow()->Visible()))
      CovertActionsMissionOutcomeWindowKill();

   if(CovertActionsWindow() && (in_bForce || !CovertActionsWindow()->Visible()))
      CovertActionsWindowKill();

   if(WarListWindow() && (in_bForce || !WarListWindow()->Visible()))
      WarListWindowKill();
}

void GDataDisplayLayer::KillStrategicWarfare()
{
   //StrategicAlertWindowKill();
   StrategicLaunchWindowKill();
   StrategicMissileListWindowKill();
   //StrategicNewsWindowKill();
   StrategicTargetCountryWindowKill();
   StrategicTargetMultipleCountriesWindowKill();
   StrategicSelectTargetWindowKill();
}

bool GDataDisplayLayer::TimedVisibilityUpdate(GUI::GBaseObject* in_pTarget, bool in_bVisible, REAL32 in_fDelay)
{
   //If the object is already in the timed object to display list, ignore this request
   hash_map<GUI::GBaseObject*, GTimedDisplayObjectData>::iterator l_It = m_TimedDisplayObjects.find(in_pTarget);
   if(l_It != m_TimedDisplayObjects.end())
   {
      return false;
   }
   else
   {
      REAL32 l_fAnimTime = g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Get_Animation_Time();
      GTimedDisplayObjectData l_Obj;
      l_Obj.m_pObject = in_pTarget;
      l_Obj.m_bVisible = in_bVisible;
      l_Obj.m_fWhen = l_fAnimTime + in_fDelay;
      m_TimedDisplayObjects[in_pTarget] = l_Obj;
      return true;
   }

}

GTooltipWindow* GDataDisplayLayer::TooltipWindow()
{
   return m_pTooltipWindow;
}

bool GDataDisplayLayer::TooltipWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pTooltipWindow,GTooltipWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::TooltipWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pTooltipWindow);
}

GTreatiesWindow* GDataDisplayLayer::TreatiesWindow()
{
   return m_pTreatiesWindow;
}

bool GDataDisplayLayer::TreatiesWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pTreatiesWindow,GTreatiesWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::TreatiesWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pTreatiesWindow);
}

GTreatyDetailsWindow* GDataDisplayLayer::TreatyDetailsWindow()
{
   return m_pTreatyDetailsWindow;
}

bool GDataDisplayLayer::TreatyDetailsWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pTreatyDetailsWindow,GTreatyDetailsWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::TreatyDetailsWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pTreatyDetailsWindow);
}

GRelationsWindow* GDataDisplayLayer::RelationsWindow()
{
   return m_pRelationsWindow;
}

bool GDataDisplayLayer::RelationsWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pRelationsWindow,GRelationsWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::RelationsWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pRelationsWindow);
}

GCountryInformationWindow* GDataDisplayLayer::CountryInformationWindow()
{
   return m_pCountryInformationWindow;
}

bool GDataDisplayLayer::CountryInformationWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pCountryInformationWindow,GCountryInformationWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::GameScenarioWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pGameScenarioWindow);
}

GGameScenarioWindow* GDataDisplayLayer::GameScenarioWindow()
{
   return m_pGameScenarioWindow;
}

bool GDataDisplayLayer::GameScenarioWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pGameScenarioWindow,GGameScenarioWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

GObjectivesWindow* GDataDisplayLayer::ObjectivesWindow()
{
   return m_pObjectivesWindow;
}

bool GDataDisplayLayer::ObjectivesWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pObjectivesWindow,GObjectivesWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::ObjectivesWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pObjectivesWindow);
}

bool GDataDisplayLayer::EndGameWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pGameScenarioWindow);
}

GEndGameWindow* GDataDisplayLayer::EndGameWindow()
{
   return m_pEndGameWindow;
}

bool GDataDisplayLayer::EndGameWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pEndGameWindow,GEndGameWindow::TypeName, false, true, m_pBackgroundEarth))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::ServerInfoWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pServerInfoWindow);
}

GServerInfoWindow* GDataDisplayLayer::ServerInfoWindow()
{
   return m_pServerInfoWindow;
}

bool GDataDisplayLayer::ServerInfoWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pServerInfoWindow,GServerInfoWindow::TypeName, false, true, NULL ))
   {
      return true;
   }
   return false;
}

GCreditsWindow* GDataDisplayLayer::CreditsWindow()
{
   return m_pCreditsWindow;
}

bool GDataDisplayLayer::CreditsWindowSpawn()
{
   if (ObjectSpawn((GUI::GBaseObject**)&m_pCreditsWindow,GCreditsWindow::TypeName, false, true))
   {
      return true;
   }
   return false;
}

bool GDataDisplayLayer::CreditsWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pCreditsWindow);
}

bool GDataDisplayLayer::CountryInformationWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pCountryInformationWindow);
}


GString GDataDisplayLayer::FormatNumberContracted(REAL64 in_fValue)
{
   GString l_sString;

   if(in_fValue > 10000000)
   {
      l_sString = GString::FormatNumber(in_fValue / 1000000.f, L"", L"", L"", L" M", 0, 0);
   }
   else
   {
      if(in_fValue > 10000)
      {
         l_sString = GString::FormatNumber(in_fValue / 1000.f, L"", L"", L"", L" K", 0, 0);
      }
      else
      {
         l_sString = GString::FormatNumber(in_fValue, L"", L"", L"", L"", 0, 0);
      }
   }

   return l_sString;
}

bool GDataDisplayLayer::Cleanup()
{
   GameLobby()->Hide();

   BackgroundEarth()->ClearAllRegionColors();
   BackgroundEarth()->DestroyAllNukes();
   SwapToMilitary();
   BackgroundEarth()->m_vCountryUnitsToDisplay.clear();
   BackgroundEarth()->m_bUnitDragging = false;
   BackgroundEarth()->m_bUnitPicked = false;
   BackgroundEarth()->m_bEarthDragging = false;
   BackgroundEarth()->m_bSelectBox = false;
   BackgroundEarth()->m_vCombatAnims.clear();       

   BattleOverviewWindowKill();
   BattleSummaryWindowKill();
   ThematicMapLegendKill();
   RegionControlChangeWindowKill();
   HistoryWindowKill();
   
   MiniMap()->Hide();
   MainBar()->Hide();
   SystemBarWindow()->Hide();
   KillWindowsLvl();
   KillWindowsEconomicLv2();
   KillWindowsMilitaryLv2();
   KillWindowsPoliticLv2();   
   KillStrategicWarfare();

   MainBar()->m_pObjEconomic->ToggleState(false);
   MainBar()->m_pObjMilitary->ToggleState(false);
   MainBar()->m_pObjPolitic->ToggleState(false);
   MainBar()->m_pObjAdvisor->ToggleState(false);
   MainBar()->m_pObjEventLog->ToggleState(false);
   MainBar()->m_pObjHistory->ToggleState(false);
   MainBar()->m_pObjMail->ToggleState(false);

   m_vPreviouslyDisplayedWindows.clear();
   m_TimedDisplayObjects.clear();

   if(m_pCovertActionsWindow)
   {
      m_pCovertActionsWindow->ClearBeforeUpdate(true);
      m_pCovertActionsWindow->Update();
   }

   return true;
}

/*!
 * Thematic map legend window methods
 **/
GThematicMapLegendWindow*  GDataDisplayLayer::ThematicMapLegend()
{
   return m_pThematicMapLegendWindow;
}
bool                       GDataDisplayLayer::ThematicMapLegendSpawn()
{
   return ObjectSpawn((GUI::GBaseObject**)&m_pThematicMapLegendWindow,GThematicMapLegendWindow::TypeName,true,true,m_pBackgroundEarth);
}
bool                       GDataDisplayLayer::ThematicMapLegendKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pThematicMapLegendWindow);
}

GConnectingWindow* GDataDisplayLayer::ConnectingWindow()
{
   return m_pConnectingWindow;
}

bool GDataDisplayLayer::ConnectingWindowSpawn()
{
   GDZLOG(EDZLogLevel::Entry, L"");

   const bool l_bResult = ObjectSpawn((GUI::GBaseObject**)&m_pConnectingWindow, GConnectingWindow::TypeName, true, true, NULL);

   GDZLOG(EDZLogLevel::Exit, L"Returning " + GString(l_bResult));
   return l_bResult;
}

bool GDataDisplayLayer::ConnectingWindowKill()
{
   return ObjectKill((GUI::GBaseObject**)&m_pConnectingWindow);
}

//********************* COUNTRIES LIST BOX RELATED STUFF **************************
bool GDataDisplayLayer::CountriesListBoxSpawn()
{
   // one list box for every country cbos of the game
   m_pObjCountryLst = GComboListBoxLess::CreateListBox(L"lstComboArrowUp", L"lstComboListBox" ,
                                                       L"lstCountryFlagNameItem");
   // if it was created properly
   if(m_pObjCountryLst)
   {
      // Set up our label child name
      m_pObjCountryLst->LabelText("txtName");
      // Set up default row height
      m_pObjCountryLst->RowHeight(14);
   }

   // succcess ?
   return (m_pObjCountryLst != NULL);
}

bool GDataDisplayLayer::CountriesListBoxKill()
{
   // release the cbo object owned by the ddl
   GComboListBoxLess::ReleaseListBox(m_pObjCountryLst);
   // success ?
   return (m_pObjCountryLst == NULL);
}

void GDataDisplayLayer::CountriesListBoxEnableNotAvailable(bool in_bEnable)
{
   // do we at least have our box ?
   if(!m_pObjCountryLst)
      // error
      return;

   // loop into every row
   for (INT32 i = 0; i < m_pObjCountryLst->Get_Nb_Rows(); i++)
   {
      // the country id will be the additionnal info
      GUI::GListBox::GRow* l_pRow   = m_pObjCountryLst->Get_Row_At(i);
      GUI::GLabel*         l_pLabel = (GUI::GLabel*)l_pRow->Object()->Child("txtName");

      // enable and disable desired rows
      if( (!in_bEnable) && 
          (find(m_pGameLobbyWindow->m_AvailableCountries.begin(), 
                m_pGameLobbyWindow->m_AvailableCountries.end(), 
                l_pRow->m_iAdditionalId) != m_pGameLobbyWindow->m_AvailableCountries.end() ) )
      {
         // Disable this row
         l_pRow->Object()->Enabled(false);
         l_pLabel->FontColor(c_colorTakenCountry);

         // If we already had selected this country, put the edit box as black
         // If its our country, its black anyway...
         /*
         if(m_pObjCountryCbo->Selected_Content() == g_ClientDAL.Countries()[l_bCountryIndex].Name())
         {
            GUI::GEditBox* l_pEditBox = (GUI::GEditBox *)m_pObjCountryCbo->Child("Edit");
            l_pEditBox->FontColorNormal(c_colorTakenCountry);
         }
         */
      }
      else
      {
         l_pRow->Object()->Enabled(true);
         l_pLabel->FontColor(c_colorAvailableCountry);

         // If we already had selected this country, its now available...
         /*
         if(m_pObjCountryCbo->Selected_Content() == g_ClientDAL.Countries()[i].Name())
         {
            GUI::GEditBox* l_pEditBox = (GUI::GEditBox *)m_pObjCountryCbo->Child("Edit");
            l_pEditBox->FontColorNormal(c_colorAvailableCountry);
         }
         */
      }
   }
}

bool GDataDisplayLayer::CountriesListBoxFillUp( const SP2::GCountries& in_iCountries )
{
   // do we at least have our box ?
   if(!m_pObjCountryLst)
      // error
      return false;

   // small flags textures fill up if needed
   if(!m_vSmallFlagsTextures.size())
      SmallFlagsTexturesCreate();

   // row pointer and counter used
   GUI::GListBox::GRow* l_pRow = NULL;
   INT32 l_iCounter            = 1;

   // map will be alphabetically ordered by country name
   map<GString, INT32> l_Countries;
   // feed the map hence the countries
   SP2::GCountries::const_iterator l_ItCountry = in_iCountries.begin();
   while(l_ItCountry != in_iCountries.end())
   {
      // only add if country is still active
      if(l_ItCountry->IsActive())
      {
         l_Countries.insert(map<GString, INT32>::value_type(l_ItCountry->Name(),l_ItCountry->Id()));
      }
      // next
      ++l_ItCountry;
   }

   // feed the map with our countries
   for(map<GString, INT32>::const_iterator l_It = l_Countries.begin();
       l_It != l_Countries.end(); 
       l_It++)
   {
      // country id
      INT32 l_iCountryId = l_It->second;

      // add the item in our list box and retrieve a ptr to the row
      // the country id will be the additionnal info
      if( m_pObjCountryLst->Get_Nb_Rows() < l_iCounter )
         l_pRow = m_pObjCountryLst->Insert_New_Row(l_iCountryId);
      else
      {
         l_pRow = m_pObjCountryLst->Get_Row_At( l_iCounter - 1 );
         l_pRow->m_iAdditionalId = l_iCountryId;
         l_pRow->m_bHidden = false;
      }
      
      // Retrieve the objects in our row and set the frame props    
      GUI::GFrame*      l_pFrame = NULL;
      GUI::GLabel*      l_pLabel = (GUI::GLabel*)l_pRow->Object()->Child("txtName");         
      GUI::GPictureBox* l_pFlag  = (GUI::GPictureBox*)l_pRow->Object()->Child("picFlag");

      // frame get succesfully ?
      if((l_pFrame = (GUI::GFrame*)l_pRow->Object()) != NULL)
      {
         // Default look and feel 
         l_pFrame->Height(14);         
         l_pFrame->Borders(GBorders(1, 1, 1, 1));
         // Default picture 
         l_pFrame->PictureNormal("000647");
         
      }
      else
      {
         l_iCounter++;
         // error
         return false;
      }

      // set the label props
      l_pLabel->Text(l_It->first);
      l_pLabel->TextOffset(2, 0);
      l_pLabel->FontColorNormal(GColorRGBInt(255,255,255,255));

      // set the flags props
      l_pFlag->Position(0, 0);
      l_pFlag->Size(22, 13);

      l_pFlag->PictureNormal(m_vSmallFlagsTextures[l_iCountryId]);
      l_iCounter++;
   }

   bool l_bUpdated = true;

   for( INT32 i=m_pObjCountryLst->Get_Nb_Rows() - 1; i >= l_iCounter - 1; i-- )
   {
      m_pObjCountryLst->Get_Row_At( i )->m_bHidden = true;
      
      if(l_bUpdated)
         l_bUpdated = false;
   }

   if(!l_bUpdated)
   {
      m_pObjCountryLst->Update();
      
   }
   
   // filled ok !
   return true;
}
//***************** END OF COUNTRIES LIST BOX RELATED STUFF **********************

//**************** SMALL FLAGS TEXTURES VECTOR RELATED STUFF *********************
bool GDataDisplayLayer::SmallFlagsTexturesCreate()
{
   // Free at first
   SmallFlagsTexturesDestroy();

   // setup our size
   UINT32 l_iNbFlags = g_ClientDAL.Countries().size() + 1;

   // set the good size to our vector, +1 means our country id will
   // be the index of our flag texture
   m_vSmallFlagsTextures.resize(l_iNbFlags);

   // first texture is null, no country id with 0
   m_vSmallFlagsTextures[0] = NULL;

   // local texture file and filename
   GFile l_FlagFile;
   GString l_sFileName;

   // create every flag textures
   for(UINT32 i = 1; i < l_iNbFlags; i++)
   {      
      // set up small flag filname
      l_sFileName = GString(FLAG_SMALL_DEFAULT_FOLDER)+ g_ClientDAL.Country(i).Flag();

      // load the file through the manager
      if(g_Joshua.FileManager()->File(l_sFileName,l_FlagFile))
      {
         // if the files exists ? Create our texture
         m_vSmallFlagsTextures[i] = g_Joshua.Renderer()->TextureManager()->Create_Texture(l_FlagFile, true, l_FlagFile.Name());
      }
   }

   // success
   return true;
}

bool GDataDisplayLayer::SmallFlagsTexturesDestroy()
{
   // release every flag textures
   for(UINT32 i = 0; i < m_vSmallFlagsTextures.size(); i++)
   {
      SAFE_RELEASE(m_vSmallFlagsTextures[i]);
   }

   // clear our vector
   m_vSmallFlagsTextures.clear();

   // success
   return true;
}
//************ END OF SMALL FLAGS TEXTURES VECTOR RELATED STUFF ******************


GGenericQuestion::GGenericQuestion() :    m_pParentWindow(NULL),
                                          m_pOberserToNotify(NULL),
                                          m_pOnOkValue(NULL),
                                          m_pOnCancelValue(NULL)

{

}


const DesignPattern::GSubject*  GDataDisplayLayer::GenericQuestion(GGenericQuestion& inQuestion)
{
   gassert(inQuestion.m_pOberserToNotify,"We need at least an observer, otherwise, caller are not using proper windows. If you don't care about the answer, use the generic message");

   // Spawn a question
   GGenericQuestionWindow* l_pQuestionWindow(NULL);

   // Spawn question window
   l_pQuestionWindow= (GGenericQuestionWindow*)CREATE_GUI_OBJECT(GGenericQuestionWindow::TypeName, GGenericQuestionWindow::TypeName, m_pBackgroundEarth, false, true);
   gassert(l_pQuestionWindow,"Could't spawn a generic question window");
   
   // Show the question
   l_pQuestionWindow->Show(inQuestion);

   return l_pQuestionWindow;
}
