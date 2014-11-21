/**************************************************************
*
* sp2_data_display_layer.h
*
* Description
* ===========
* Hold the GDataDisplayLayer class implemantation
* This class will be used to display everything
*
* Owner
* =====
*  François Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _GOLEM_SP2_GDDL_H_
#define _GOLEM_SP2_GDDL_H_

#include "sp2_frame_updatable_value.h"
#include "../include/sp2_combo_listboxless.h"

CWSTR FILL_COLOR_PROPERTY      = L"Fill_Color";
CWSTR BORDER_COLOR_PROPERTY    = L"Border_Color";

const INT32 c_OnMeterChangeEvent     = 16070809;
const INT32 c_CountryAvailable    = 1;
const INT32 c_CountryNotAvailable = 0;

namespace SP2
{
   struct GOnMeterChangeData
   {
      INT32  m_iBtnId;
      REAL32 m_fBtnValue;
   };

   struct GTimedDisplayObjectData
   {
      GUI::GBaseObject* m_pObject;
      REAL32 m_fWhen;
      bool   m_bVisible;
   };

   struct GUpdateValueData
   {
      SP2::GUpdatableValue* m_pObject;
      REAL64            m_fWhen;
      bool              m_bIncrement;
   };

   class GGenericQuestion
   {
	   public:
	   GGenericQuestion();

	   GString 		                        m_sMessage;
      GString                             m_sOkCaption;
      GString                             m_sCancelCaption;
	   GUI::GBaseObject* 	               m_pParentWindow;
      Hector::DesignPattern::GObserver*   m_pOberserToNotify;
      void*                               m_pOnOkValue;
      void*                               m_pOnCancelValue;
   };


   class GDataDisplayLayer
   {
   private:
      class GBackgroundEarth*                         m_pBackgroundEarth;
      class GUI::GConsole*                            m_pConsoleWindow;
      class CGFX_Action_Itf*                          m_pConsoleWindowShowAction;
      class CGFX_Action_Itf*                          m_pConsoleWindowHideAction;
      class GMainMenu*                                m_pMainMenu;
      class GMiniMap*                                 m_pMiniMap;
      class GDetailedEarthArea*                       m_pDetailedEarthArea;
      class GInternalLawsWindow*                      m_pInternalLawsWindow;
      class GResourcesWindow*                         m_pResourcesWindow;
      class GJoinMPGameWindow*                        m_pJoinMPGameWindow;
      class GMainBar*                                 m_pMainBar;
      class GSystemBarWindow*                         m_pSystemBarWindow;
      class GPoliticWindow*                           m_pPoliticWindow;
      class GConstitutionalFormWindow*                m_pConstitutionalFormWindow;
      class GEconomicWindow*                          m_pEconomicWindow;
      class GEconomicHealthWindow*                    m_pEconomicHealthWindow;
      class GMilitaryWindow*                          m_pMilitaryWindow;
      class GAdvisorAdviseWindow*                     m_pAdvisorAdviseWindow;
      class GProgressMeter*                           m_pProgressMeter;
      class GFrame*                                   m_pSplashScreen;
      class GStrategicWarfareWindow*                  m_pStrategicWarfareWindow;
      class GBudgetWindow*                            m_pBudgetWindow;
      class GHostMPGameWindow*                        m_pHostMultiplayerGameWindow;
      class GGameLobbyWindow*                         m_pGameLobbyWindow;
      //class GActionListWindow*                        m_pActionListWindow;
      class GDesignWindow*                            m_pDesignWindow;
      class GCreateDesignWindow*                      m_pCreateDesignWindow;
      class GUnitInfoSmallWindow*                     m_pUnitInfoSmallWindow;
      class GBattleInfoWindow*                        m_pBattleInfoWindow;
      class GBattleOverviewWindow*                    m_pBattleOverviewWindow;
      class GUnitListWindow*                          m_pUnitListWindow;
      class GUnitListWindowNew*                       m_pUnitListWindowNew;
      class GUnitProductionWindow*                    m_pUnitProductionWindow;
      class GUsedUnitWindow*                          m_pUsedUnitWindow;
      class GUnitResearchWindow*                      m_pUnitResearchWindow;
      class GSaveConfirmationWindow*                  m_pSaveConfirmationWindow;
      //class GStrategicAlertWindow*                    m_pStrategicAlertWindow;
      class GStrategicLaunchWindow*                   m_pStrategicLaunchWindow;
      class GStrategicMissileListWindow*              m_pStrategicMissileListWindow;
      //class GStrategicNewsWindow*                     m_pStrategicNewsWindow;
      class GStrategicTargetCountryWindow*            m_pStrategicTargetCountryWindow;
      class GStrategicTargetMultipleCountriesWindow*  m_pStrategicTargetMultipleCountriesWindow;
      class GStrategicSelectTargetWindow*             m_pStrategicSelectTargetWindow;
	   class GSphereControlWindow*					      m_pSphereControlWindow;
      class GTooltipWindow*                           m_pTooltipWindow;
      class GGenericMessageWindow*                    m_pGenericMessageWindow;
      class GGenericQuestionWindow*                   m_pGenericQuestionWindow;
      class GHistoryWindow*                           m_pHistoryWindow;
      class GBattleSummaryWindow*                     m_pBattleSummaryWindow;
      class GTreatiesWindow*                          m_pTreatiesWindow;
      class GUnitGroundDeploymentWindow*              m_pUnitGroundDeploymentWindow;
      class GThematicMapLegendWindow*                 m_pThematicMapLegendWindow;
      class GTreatyDetailsWindow*                     m_pTreatyDetailsWindow;
      class GAddRemoveWindow*                         m_pAddRemoveWindow;
      class GUnitSplitWindow*                         m_pUnitSplitWindow;
      class GGroupSplitWindow*                        m_pGroupSplitWindow;
      class GIncreaseProductionWindow*                m_pIncreaseProductionWindow;
      class GAdvisorWindow*                           m_pAdvisorWindow;
      class GAdvisorSettingsWindow*                   m_pAdvisorSettingsWindow;
      class GStartSinglePlayerWindow*                 m_pStartSinglePlayerWindow;
      class GMailWindow*                              m_pMailWindow;
      class GRelationsWindow*                         m_pRelationsWindow;
      class GCountryInformationWindow*                m_pCountryInformationWindow;
      class GEventLogWindow*                          m_pEventLogWindow;
      class GConnectingWindow*                        m_pConnectingWindow;
      class GCovertActionsWindow*                     m_pCovertActionsWindow;
      class GCovertActionsCellTrainingWindow*         m_pCovertActionsCellTrainingWindow;
      class GCovertActionsMissionOutcomeWindow*       m_pCovertActionsMissionOutcomeWindow;
      class GCovertActionsNewCellWindow*              m_pCovertActionsNewCellWindow;
      class GCovertActionsNewMissionWindow*           m_pCovertActionsNewMissionWindow;
      class GCovertActionsMissionOutcomeAttackedWindow* m_pCovertActionsMissionOutcomeAttackedWindow;
      class GGameScenarioWindow*                      m_pGameScenarioWindow;
      class GEndGameWindow*                           m_pEndGameWindow;
      class GCreditsWindow*                           m_pCreditsWindow;
      class GTradeWindow*                             m_pTradeWindow;
      class GTradeDetailedWindow*                     m_pTradeDetailedWindow;
      class GContextMenu*                             m_pContextMenu;
      class GCountryProfileWindow*                    m_pCountryProfileWindow;
      class GPartySelectionWindow*                    m_pPartySelectionWindow;
      class GWarListWindow*                           m_pWarListWindow;
      class GTextOverlay*                             m_pTextOverlay;
      class GGameOptionsWindow*                       m_pGameOptionsWindow;
      class GSaveGameWindow*                          m_pSaveGameWindow;
      class GLoadGameWindow*                          m_pLoadGameWindow;
      class GUI::GListBox*                            m_pObjCountryLst;
      class GBombardmentResultWindow*                 m_pBombardmentResultWindow;
      class GObjectivesWindow*                        m_pObjectivesWindow;
      class GServerInfoWindow*                        m_pServerInfoWindow;
      class GGenericInputWindow*                      m_pGenericInputWindow;
      class GGenericMessageWindow*                    m_pGenericMessageDesktopWindow;
      class GSubmenuMulti*                            m_pSubmenuMulti;
      class GSubmenuSingle*                           m_pSubmenuSingle;
      class GRegionControlChangeWindow*               m_pRegionControlChangeWindow;
	  class GLoginRGAPIWindow*							m_pLoginRGAPIWindow;


      hash_map<UINT32, class GUnitAttackConfirmWindow*>  m_UnitAttackConfirmWindows;
      hash_map<UINT32, class GUnitMovementDenialWindow*> m_UnitMovementDenialWindows;
      UINT32                                             m_iUnitMovementDenialNextID;

      //Test stuff
      class GTestWindow*         m_pTestWindow;

      vector<GUI::GBaseObject*>  m_vSpawnedWindows;
      vector<GUI::GBaseObject*>  m_vPreviouslyDisplayedWindows;
      hash_map<GUI::GBaseObject*, GTimedDisplayObjectData> m_TimedDisplayObjects;
      vector<GUpdateValueData>   m_vUpdateValueList;
      vector<CGFX_Texture_Itf*>  m_vSmallFlagsTextures;

      GFile m_CursorDefault;
      
      CGFX_Action_Itf* m_pActionRectangle;
      CGFX_Model_Itf*  m_pModelRectangle;

      GFX::GTextureSequence*  m_pLaunchSequence;

   protected:

      bool ObjectSpawn(GUI::GBaseObject** in_pObject,const GString& in_sType,bool in_bVisible, bool in_bFocusable,GUI::GBaseObject* in_pParent = NULL);
      bool ObjectKill(GUI::GBaseObject**  in_pObject);


   public:
      GDataDisplayLayer();
      ~GDataDisplayLayer();

      void ResetMousePointer();

      GLogger* m_pLogger;

      //! Initializes the DDL, registers constructors ... 
      bool Initialize();
      void Iterate();
      void IterateTimedShowWindow();
      void IterateWindowCanBeKilled();

      void IterateUpdateCursor();
      void AddToIterateUpdateCursor(const GUpdateValueData & in_UpdateValueData);
      void RemoveFromIterateUpdateCursor(SP2::GUpdatableValue* in_pObject);

      bool Cleanup();
      //! Clean all DDL objects
      bool Shutdown();

      //Start the animation system
      bool StartAnimationSystem();

      //Show of hide the wanted object at a given time
      bool TimedVisibilityUpdate(GUI::GBaseObject* in_pTarget, bool in_bVisible, REAL32 in_fDelay);

      //Swap to another window set
      void SwapToStrategicWarfare();
      void SwapToMilitary();

      //Action list window functions
      //GActionListWindow* ActionListWindow();
      //bool ActionListWindowSpawn();
      //bool ActionListWindowKill();

      //Server information window functions
      GServerInfoWindow* ServerInfoWindow();
      bool ServerInfoWindowSpawn();
      bool ServerInfoWindowKill();

      //Event log window functions
      GEventLogWindow* EventLogWindow();
      bool EventLogWindowSpawn();
      bool EventLogWindowKill();

      //Mail window functions
      GMailWindow* MailWindow();
      bool MailWindowSpawn();
      bool MailWindowKill();

      //War list window functions
      GWarListWindow* WarListWindow();
      bool WarListWindowSpawn();
      bool WarListWindowKill();

      //End Game Window
      GEndGameWindow* EndGameWindow();
      bool EndGameWindowSpawn();
      bool EndGameWindowKill();

      //Region control change window
      GRegionControlChangeWindow* RegionControlChangeWindow();
      bool RegionControlChangeWindowSpawn();
      bool RegionControlChangeWindowKill();

      //Credits window functions
      GCreditsWindow* CreditsWindow();
      bool CreditsWindowSpawn();
      bool CreditsWindowKill();

      //Text overlay functions;
      GTextOverlay* TextOverlay();
      bool TextOverlaySpawn();
      bool TextOverlayKill();

      //Add remove country window functions
      GAddRemoveWindow* AddRemoveWindow();
      bool AddRemoveWindowSpawn();
      bool AddRemoveWindowKill();

      //Advisor window functions
      GAdvisorWindow* AdvisorWindow();
      bool AdvisorWindowSpawn();
      bool AdvisorWindowKill();

      //Advisor settings window functions
      GAdvisorSettingsWindow* AdvisorSettingsWindow();
      bool AdvisorSettingsWindowSpawn();
      bool AdvisorSettingsWindowKill();

      GAdvisorAdviseWindow* AdvisorAdviseWindow();
      bool AdvisorAdviseWindowSpawn();
      bool AdvisorAdviseWindowKill();
      void AdvisorAdviseWindowSet(GString in_sText, GUI::GBaseObject* in_pObject);

      //Animation helper functions
      CGFX_Track_Itf* AnimationCreateSliding(REAL32 in_fAnimationTime, REAL32 in_fDelay, INT32 in_iFrom, INT32 in_iTo, CGFX_Transform_Itf* in_pTransform, bool in_bYAnim = true);
      void AnimationPlaySliding(GFX::EPlayback_Type in_PlaybackMode, CGFX_Track_Itf* in_pTrack);

      void AnimationCreateRectangle();
      void AnimationPlayRectangle(REAL32 in_fAnimationTime, GVector2D<INT32> in_FromPos, GVector2D<INT32> in_FromSize, GVector4D<REAL32> in_FromColor, GVector2D<INT32> in_ToPos, GVector2D<INT32> in_ToSize, GVector4D<REAL32> in_ToColor);
      void AnimationPlayRectangle(GUI::GBaseObject* in_pFromObj, GUI::GBaseObject* in_pToObj);

      //Background Earth functions
      GBackgroundEarth* BackgroundEarth();
      bool BackgroundEarthSpawn();
      bool BackgroundEarthKill();

      //Battle overview window functions
      GBattleOverviewWindow* BattleOverviewWindow();
      bool BattleOverviewWindowSpawn();
      bool BattleOverviewWindowKill();

      //Battle summary window
      GBattleSummaryWindow* BattleSummaryWindow();
      bool BattleSummaryWindowSpawn(GUI::GBaseObject* in_pParent);
      bool BattleSummaryWindowKill();

      //Battle overview window functions
      GBattleInfoWindow* BattleInfoWindow();
      bool BattleInfoWindowSpawn();
      bool BattleInfoWindowKill();

      //Budget window functions
      GBudgetWindow* BudgetWindow();
      bool BudgetWindowSpawn();
      bool BudgetWindowKill();

      //Set all wanted country sorted in the combobox
      void ComboboxSetCountries(const SP2::GCountries& in_vCountries, GUI::GComboBox* in_pCombo);
      void ComboboxSetCountriesAndFlag(const SP2::GCountries& in_vCountries, GUI::GComboBox* in_pCombo, bool in_bUseListBoxLessCbo = false);
      void ComboboxSetCountriesCodeAndFlag(const SP2::GCountries& in_vCountries, GUI::GComboBox* in_pCombo);

      //void ListBoxSetCountriesCodeAndFlag(const SP2::GCountries& in_vCountries,GUI::GListBox* in_pListBox2Fil);


      //Console window functions
      GUI::GConsole* ConsoleWindow();
      bool ConsoleWindowSpawn();
      bool ConsoleWindowShow();
      bool ConsoleWindowHide();
      bool ConsoleWindowKill();

      //Constitutional window functions
      GConstitutionalFormWindow* ConstitutionalFormWindow();
      bool ConstitutionalFormWindowSpawn();
      bool ConstitutionalFormWindowKill();

      //Country Information window functions
      GCountryInformationWindow* CountryInformationWindow();
      bool CountryInformationWindowSpawn();
      bool CountryInformationWindowKill();

      //Country Profile window functions
      GCountryProfileWindow* CountryProfileWindow();
      bool CountryProfileWindowSpawn();
      bool CountryProfileWindowKill();

      //Game Scenario window functions
      GGameScenarioWindow* GameScenarioWindow();
      bool GameScenarioWindowSpawn();
      bool GameScenarioWindowKill();

      //Objectives window functions
      GObjectivesWindow* ObjectivesWindow();
      bool ObjectivesWindowSpawn();
      bool ObjectivesWindowKill();

      //Connecting window
      bool ConnectingWindowSpawn();
      bool ConnectingWindowKill();
      GConnectingWindow* ConnectingWindow();

      //Context menu functions, only spawn because it kills itself.
      class GContextMenu* ContextMenuSpawn();
      class GContextMenu* ContextMenu();
      void                ContextMenuKill();
      

      GCovertActionsWindow* CovertActionsWindow();
      bool CovertActionsWindowSpawn();
      bool CovertActionsWindowKill();

      GCovertActionsCellTrainingWindow* CovertActionsCellTrainingWindow();
      bool CovertActionsCellTrainingWindowSpawn();
      bool CovertActionsCellTrainingWindowKill();

      GCovertActionsMissionOutcomeWindow* CovertActionsMissionOutcomeWindow();
      bool CovertActionsMissionOutcomeWindowSpawn();
      bool CovertActionsMissionOutcomeWindowKill();

      GCovertActionsMissionOutcomeAttackedWindow* CovertActionsMissionOutcomeAttackedWindow();
      bool CovertActionsMissionOutcomeAttackedWindowSpawn();
      bool CovertActionsMissionOutcomeAttackedWindowKill();

      GCovertActionsNewCellWindow* CovertActionsNewCellWindow();
      bool CovertActionsNewCellWindowSpawn();
      bool CovertActionsNewCellWindowKill();

      GCovertActionsNewMissionWindow* CovertActionsNewMissionWindow();
      bool CovertActionsNewMissionWindowSpawn();
      bool CovertActionsNewMissionWindowKill();

      GString FormatNumberContracted(REAL64 in_fValue);

      //Economic window functions
      GEconomicWindow* EconomicWindow();
      bool EconomicWindowSpawn();
      bool EconomicWindowKill();

      GEconomicHealthWindow* EconomicHealthWindow();
      bool EconomicHealthWindowSpawn();
      bool EconomicHealthWindowKill();

      GGameOptionsWindow* GameOptionsWindow();
      bool GameOptionsWindowSpawn();
      bool GameOptionsWindowKill();

      GGenericMessageWindow* GenericMessageWindow();
      bool GenericMessageWindowSpawn();
      bool GenericMessageWindowKill();

      GGenericMessageWindow* GenericMessageDesktopWindow();
      bool GenericMessageDesktopWindowSpawn();
      bool GenericMessageDesktopWindowKill();

      //Generic input window functions
      GGenericInputWindow* GenericInputWindow();
      bool GenericInputWindowSpawn();
      bool GenericInputWindowKill();

      const DesignPattern::GSubject*  GenericQuestion(GGenericQuestion& inQuestion);

      GHistoryWindow* HistoryWindow();
      bool HistoryWindowSpawn();
      bool HistoryWindowKill();

      //Host multiplayer game functions
      GHostMPGameWindow* HostMPGameWindow();
      bool HostMPGameWindowSpawn();
      bool HostMPGameWindowKill();

	  //RGAPI functions
	  bool LoginRGAPIWindowSpawn();
      bool LoginRGAPIWindowKill();
	  GLoginRGAPIWindow* GetLoginRGAPIWindow();

      //Thematic map legend window methods
      GThematicMapLegendWindow*  ThematicMapLegend();
      bool                       ThematicMapLegendSpawn();
      bool                       ThematicMapLegendKill();

      //Internal laws window functions
      GInternalLawsWindow* InternalLawsWindow();
      bool InternalLawsWindowSpawn();
      bool InternalLawsWindowKill();

      //Join Multiplayer Game window functions
      GJoinMPGameWindow* JoinMPGameWindow();
      bool JoinMPGameWindowSpawn();
      bool JoinMPGameWindowKill();

      void KillWindowsLvl();
      void KillWindowsEconomicLv2(bool in_bForce = false);
      void KillWindowsPoliticLv2(bool in_bForce = false);
      void KillWindowsMilitaryLv2(bool in_bForce = false);
      void KillStrategicWarfare();

      //Main bar functions
      GMainBar* MainBar();
      bool MainBarSpawn();
      bool MainBarKill();
      bool MainBarUpdateRank();
      void MainBarScaleRank(GFrame* in_pFrame, INT16 in_iRank);

      //System bar functions
      GSystemBarWindow* SystemBarWindow();
      bool SystemBarWindowSpawn();
      bool SystemBarWindowKill();

      //Progress meter functions
      GProgressMeter* ProgressMeter();
      bool            ProgressMeterSpawn();
      bool            ProgressMeterKill();

      //Main menu functions
      GMainMenu* MainMenu();
      bool MainMenuSpawn();
      bool MainMenuKill();

      //submenu multi functions
      GSubmenuMulti* SubmenuMulti();
      bool SubmenuMultiSpawn();
      bool SubmenuMultiKill();

      //submenu single functions
      GSubmenuSingle* SubmenuSingle();
      bool SubmenuSingleSpawn();
      bool SubmenuSingleKill();

      //Load game window functions
      GLoadGameWindow* LoadGameWindow();
      bool LoadGameWindowSpawn();
      bool LoadGameWindowKill();

      //Save game window functions
      GSaveGameWindow* SaveGameWindow();
      bool SaveGameWindowSpawn();
      bool SaveGameWindowKill();

      //Military window functions
      GMilitaryWindow* MilitaryWindow();
      bool MilitaryWindowSpawn();
      bool MilitaryWindowKill();

      //Mini map functions
      GMiniMap* MiniMap();
      bool MiniMapSpawn();
      bool MiniMapKill();
   
      //Party selection window functions
      GPartySelectionWindow* PartySelectionWindow();
      bool PartySelectionWindowSpawn();
      bool PartySelectionWindowKill();

      //Popup a msg window
      void PopupMessage(GString in_sMsg);

      GPoliticWindow* PoliticWindow();
      bool PoliticWindowSpawn();
      bool PoliticWindowKill();
      bool PoliticWindowUpdateRank(INT16 in_iRank);

      GRelationsWindow* RelationsWindow();
      bool RelationsWindowSpawn();
      bool RelationsWindowKill();

      GResourcesWindow* ResourcesWindow();
      bool ResourcesWindowSpawn();
      bool ResourcesWindowKill();

      GIncreaseProductionWindow* IncreaseProductionWindow();
      bool IncreaseProductionWindowSpawn();
      bool IncreaseProductionWindowKill();

      GSaveConfirmationWindow* SaveConfirmationWindow();
      bool SaveConfirmationWindowSpawn();
      bool SaveConfirmationWindowKill();

      void SelectedCountryWindowDisplayCountry(GString in_sFlagFile, GString in_sCountryName);

      void SpawnWindowsLvl();
      void SpawnWindowsEconomicLv2();
      void SpawnWindowsPoliticLv2();
      void SpawnWindowsMilitaryLv2();
      void SpawnStrategicWarfare();

      //GStrategicAlertWindow*                    StrategicAlertWindow();
      GStrategicLaunchWindow*                   StrategicLaunchWindow();
      GStrategicMissileListWindow*              StrategicMissileListWindow();
      //GStrategicNewsWindow*                     StrategicNewsWindow();
      GStrategicTargetCountryWindow*            StrategicTargetCountryWindow();
      GStrategicTargetMultipleCountriesWindow*  StrategicTargetMultipleCountriesWindow();

      // Sphere Control Window access functions
	   GSphereControlWindow*						   SphereControlWindow();
	   bool SphereControlWindowSpawn();
	   bool SphereControlWindowKill();


      //bool StrategicAlertWindowSpawn();
      bool StrategicLaunchWindowSpawn();
      bool StrategicMissileListWindowSpawn();
      //bool StrategicNewsWindowSpawn();
      bool StrategicTargetCountryWindowSpawn();
      bool StrategicTargetMultipleCountriesWindowSpawn();

      //bool StrategicAlertWindowKill();
      bool StrategicLaunchWindowKill();
      bool StrategicMissileListWindowKill();
      //bool StrategicNewsWindowKill();
      bool StrategicTargetCountryWindowKill();
      bool StrategicTargetMultipleCountriesWindowKill();

      GStrategicSelectTargetWindow* StrategicSelectTargetWindow();
      bool StrategicSelectTargetWindowSpawn();
      bool StrategicSelectTargetWindowKill();

      //Strategic warfare window functions
      GStrategicWarfareWindow* StrategicWarfareWindow();
      bool StrategicWarfareWindowSpawn();
      bool StrategicWarfareWindowKill();

      //Tooltip window functions
      GTooltipWindow* TooltipWindow();
      bool TooltipWindowSpawn();
      bool TooltipWindowKill();

      //Trade window functions
      GTradeWindow* TradeWindow();
      bool TradeWindowSpawn();
      bool TradeWindowKill();

      //Trade detailed window functions
      GTradeDetailedWindow* TradeDetailedWindow();
      bool TradeDetailedWindowSpawn();
      bool TradeDetailedWindowKill();

      //Treaties window functions
      GTreatiesWindow* TreatiesWindow();
      bool TreatiesWindowSpawn();
      bool TreatiesWindowKill();

      //Treaty details window functions
      GTreatyDetailsWindow* TreatyDetailsWindow();
      bool TreatyDetailsWindowSpawn();
      bool TreatyDetailsWindowKill();

      //Unit Attack Confirm Window
      GUnitAttackConfirmWindow* UnitAttackConfirmWindow(UINT32 in_iTargetCountry);
      bool UnitAttackConfirmWindowSpawn(UINT32 in_iTargetCountry);
      bool UnitAttackConfirmWindowKill(UINT32 in_iTargetCountry);
      bool UnitAttackConfirmWindowKillAll();
      UINT32 UnitAttackConfirmWindowsSize(){return m_UnitAttackConfirmWindows.size();}

      //Unit Ground Deployment Window
      GUnitGroundDeploymentWindow* UnitGroundDeploymentWindow();
      bool UnitGroundDeploymentWindowSpawn();
      bool UnitGroundDeploymentWindowKill();

      //Unit Design Window (new version)
      GDesignWindow* DesignWindow();
      bool DesignWindowSpawn(SP2::GUnitDesign* in_pDesign = NULL);
      bool DesignWindowKill();
      //Create unit Design Window
      GCreateDesignWindow* CreateDesignWindow();
      bool                 CreateDesignWindowSpawn();
      bool                 CreateDesignWindowKill();




      //Unit Small Info Window
      GUnitInfoSmallWindow* UnitInfoSmallWindow();
      bool UnitInfoSmallWindowSpawn();
      bool UnitInfoSmallWindowKill();

      //Unit List Window functions
      GUnitListWindow*         UnitListWindow();
      bool                     UnitListWindowSpawn();
      bool                     UnitListWindowKill();

      GUnitListWindowNew*      UnitListWindowNew();
      bool                     UnitListWindowNewSpawn();
      bool                     UnitListWindowNewKill();


      //Unit Movement Denial Windows
      GUnitMovementDenialWindow* UnitMovementDenialWindow(UINT32 in_iID);
      // ID is 0xFFFFFFFF if creation failed
      UINT32 UnitMovementDenialWindowSpawn();
      bool UnitMovementDenialWindowKill(UINT32 in_iID);
      bool UnitMovementDenialWindowKillAll();

      //Unit Production Window functions
      GUnitProductionWindow*   UnitProductionWindow();
      bool                     UnitProductionWindowSpawn();
      bool                     UnitProductionWindowKill();

      //Used Unit window functions
      GUsedUnitWindow*         UsedUnitWindow();
      bool                     UsedUnitWindowSpawn();
      bool                     UsedUnitWindowKill();


      //Unit Research Window functions
      GUnitResearchWindow*     UnitResearchWindow();
      bool                     UnitResearchWindowSpawn();
      bool                     UnitResearchWindowKill();

      //Unit Split Window functions
      GUnitSplitWindow*        UnitSplitWindow();
      bool                     UnitSplitWindowSpawn();
      bool                     UnitSplitWindowKill();

      //Group split window functions
      GGroupSplitWindow*       GroupSplitWindow();
      bool                     GroupSplitWindowSpawn();
      bool                     GroupSplitWindowKill();

      //Splash Screen functions
      GFrame* SplashScreen();
      bool SplashScreenSpawn();
      bool SplashScreenKill();

      //Game Lobby
      bool GameLobbySpawn();
      bool GameLobbyKill();
      GGameLobbyWindow * GameLobby();

      //Start single player game window
      bool StartSinglePlayerWindowSpawn();
      bool StartSinglePlayerWindowKill();
      GStartSinglePlayerWindow* StartSinglePlayerWindow();

      //Test stuff
      GTestWindow* TestWindow();
      bool TestWindowSpawn();
      bool TestWindowKill();

      // countries global listbox
      inline GUI::GListBox* CountriesListBox() const {return m_pObjCountryLst;};
      bool CountriesListBoxSpawn();
      bool CountriesListBoxKill();
      bool CountriesListBoxFillUp( const SP2::GCountries& in_iCountries );
      void CountriesListBoxEnableNotAvailable(bool in_bEnable = true);

      // countries small flags textures vector
      inline vector<CGFX_Texture_Itf*> &SmallFlagsTextures() {return m_vSmallFlagsTextures;};
      bool SmallFlagsTexturesCreate();
      bool SmallFlagsTexturesDestroy();

      GFX::GTextureSequence*  LaunchSequence()
      {
         return m_pLaunchSequence;
      }

      bool m_bLAN;
   };
};

#endif //_GOLEM_SP2_GDDL_H_
