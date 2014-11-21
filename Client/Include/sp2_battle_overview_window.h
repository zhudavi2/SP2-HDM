/**************************************************************
*
* sp2_battle_overview_window.h
*
* Description
* ===========
*
*
* Owner
* =====
* Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_BATTLE_OVERVIEW_WINDOW_H_
#define _CSP2_BATTLE_OVERVIEW_WINDOW_H_

#include "../include/sp2_battle_overview_unit.h"

namespace SP2
{
   class GBattleOverviewMeter;
   class GBattleOverviewUnit;


   struct GBattleInformation 
   {
      SP2::GArenaInfo*     m_pArenaInfo;
      UINT32               m_iSelectedAllyCountry;
      UINT32               m_iSelectedEnnemyCountry;
      ECombatSide::Enum    m_eCombatSide;
   };

   class GBattleMap : public G2DComponent
   {
      // Map showing units
      GSprite     m_AllyTerritory;
      GSprite     m_EnnemyTerritory;
      GSprite     m_FrontLine;
      GSprite     m_DynamicMap;
      GImage2D*   m_pUnitPositionLayer;
      
      hash_map<UINT32,GVector2D<UINT32> > m_AllyPosition;
      hash_map<UINT32,GVector2D<UINT32> > m_EnnemyPosition;
      GBattleOverviewUnit *m_pSelectedUnit;
   public:

      GBattleMap();
      ~GBattleMap();

      void ClearPosition(bool in_bClearAlly);
      void DrawUnitsOnMap(void);

      void OnCreate(CGFX_Model_Itf* in_pParentModel);
      void SetFrontLine(float in_fProgress);

      void CreateBattleField(GBattleInformation *in_pBattleInformation);
      void DrawUnit(GVector2D<UINT32> in_Position, GColorRGBInt &in_UnitColor);
      void DrawSelection(void);

      void Select(GBattleOverviewUnit* in_pSelectedUnit);
      void Unselect(void);

      //! If a unit is 'click' the information is passs by UnitType and Ally and the method return true;
      bool OnMouseClick(const GVector2D<UINT32> in_MousePosition,UINT32 &out_iUnitType,bool &out_bAlly);

   };

   struct GCombatStrategy
   {
      GString  m_sName;
      UINT8     m_iStrategyType;
      UINT8    m_vUnitTypeActionID[EUnitType::TotalCount];
   };


   class GBattleScene
   {
      // List of all visible units (The ones that are alive)
      static hash_map<UINT32, GBattleOverviewUnit *>   m_AllyUnits;
      static hash_map<UINT32, GBattleOverviewUnit *>   m_EnnemyUnits;
      GTree<GXMLNode>*	                        m_pConfigFile;
      CTreeXMLNode*                             m_pSceneNode;

   public:
      static void LoadAnimations(void);
      static void ClearAnimations(void);
      void ReadConfigFile(const GString &in_sXMLFileName, const GString &in_ThemeName);
      void SetUnitPosition(void);
      void CreateBattleField(GBattleInformation *in_pBattleInformation);
      void OnProcess(float in_fTime);
      void AttachModel(CGFX_Model_Itf* in_pModelToAttachTo);
      GTree<GXMLNode>* GetConfigFile(void);

      //! If a unit is 'click' it is returned
      GBattleOverviewUnit* OnMouseClick(const GVector2D<UINT32> in_MousePosition);

      void ProcessCombat(const GCombatInformationPacket& in_Packet,GBattleInformation *in_pBattleInformation);

      GBattleScene();
      ~GBattleScene();

      //! Gets a pointer to the battleoverviewunit of the given type, NULL if nothing
      GBattleOverviewUnit* GetUnit(UINT32 in_eUnitType, bool in_bAlly);


      GString& GetSceneImageFileName(void);
      GString& GetMapImageFileName(void);

      friend class GBattleOverviewWindow;
   };




   /*!
    * Class that defines the battle overview window
    **/
   class GBattleOverviewWindow : public GUI::GFrame
   {
      friend class GTutorials;

   public:
      static const GString TypeName;
      friend class GBattleOverviewSmallUnitInfoWindow;
	   GBattleOverviewWindow();
	   ~GBattleOverviewWindow();

	   bool OnCreate();
      bool OnDestroy();

      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventID, const GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      bool OnGotFocus(const GUI::GBaseObject *in_pFocusFrom, GUI::GBaseObject *in_pCaller);

      void Iterate(void);

      //! Set which one is the arena to display
      void SetArena(SP2::GArenaInfo* in_pArena);
      UINT32 ArenaId();
      void UpdateArenaInfo(void);

      static GBattleOverviewWindow* Instance;

      bool HandleCombatInformationUDPPacket(const GCombatInformationPacket& in_Packet);

      inline GArenaInfo& ArenaInfoCopy(){return m_ArenaInfoCopy;}
      void SetSceneFromTheme(const GString in_Theme);


      GBattleScene& BattleScene();
      void SimulationIsOver(bool in_bSimulationIsOver);
      bool DoFinalUpdate(SP2::GArenaInfo& in_pArenaInfo);


   private:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

      float CalculateBattleProgress(void);
      void UpdateCountrListFlag(void);
      void UnSelect(void);
      void BuildStrategies(void);
      void UpdateStrategiesFromTheme(const GString in_Theme);
      void SelectStrategy(UINT32 in_iStrategyId);
      void SendUnitStatus(bool in_bRetreat);
      void ShowOverallCombat(void);
      


      vector<GCombatStrategy> m_vStrategies;
      vector<GCombatStrategy*> m_vStrategiesForCurrentTheme;
      UINT32                  m_iCurrentStrategy;
      SP2::GArenaInfo         m_ArenaInfoCopy;

      // UI Elements
      GUI::GButton*           m_pObjCloseBtn;
      GBattleOverviewMeter*   m_pObjOverviewMeter;

      GUI::GPictureBox*       m_pAllyFlag;
      GUI::GListBox*          m_pAllyFlagList;
      GUI::GPictureBox*       m_pEnemyFlag;
      GUI::GListBox*          m_pEnemyFlagList;

      GUI::GFrame*            m_pBattleFrame;
      GUI::GFrame*            m_pMapFrame;
      GUI::GLabel*            m_pBattleNameText;
      GUI::GComboBox*         m_pStrategies;
      GUI::GButton*           m_pRetreatButton;
      

      GFX::GOverridePSCascade* m_pSelectedCursorOverride;


      INT32                   m_iArenaID;
      GBattleOverviewUnit*    m_pSelectedUnit;
      GSprite                 m_UnitSelectCursor;
      CGFX_Model_Itf*         m_pSelectionContainer;
      CGFX_Track_Itf*         m_pAnimatedCursorTrack;

      // Battle Information 
      GBattleInformation      m_BattleInformation;
      UINT32                  m_LastIterationTotalAttaker;
      UINT32                  m_LastIterationTotalDefender;

      // Mew System
      GBattleScene            m_BattleScene;
      GBattleMap              m_BattleMap;
      bool                    m_bSimulationIsOver;
      hash_map<GButton*,UINT32> m_FlagButtonToCountryIdMap;

      bool                    m_bWon;
   protected:


   };
};

#endif //_CSP2_BATTLE_OVERVIEW_WINDOW_H_
