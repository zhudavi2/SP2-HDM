/**************************************************************
*
* sp2_background_earth.h
*
* Description
* ===========
*  Definition & implementation of SP2::GBackgroundEarth,
*  a GUI Object that describes the background earth.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_BACKGROUND_EARTH_H_
#define _SP2_BACKGROUND_EARTH_H_

// Constants
const UINT32 NB_EARTH_DIVS_Y = 6;
const UINT32 NB_EARTH_DIVS_X = NB_EARTH_DIVS_Y * 2;
const UINT32 NB_EARTH_DIVS = NB_EARTH_DIVS_X * NB_EARTH_DIVS_Y;

const UINT32 CUSTOM_PARAM_LONGITUDE = 0;
const UINT32 CUSTOM_PARAM_LATITUDE  = 1;
const UINT32 CUSTOM_PARAM_DISTANCE  = 2;

const UINT32 c_NbFlares = 8;

namespace SP2
{
   // Forward Declarations
   class GTimeWindow;
   class GSelectedCountryWindow;
   class GMilitaryOverlay;
   class GAtlasEarth;
   class GOverlayCity;

   namespace EUnitFriendship
   {
      enum Enum
      {
         Enemy,
         Neutral,
         Ally,
         Player,
         Count,
      };
   }
   
   typedef void(GUI::GBaseObject::*ReturnClickedPosition)(const GVector2D<REAL32>);

   namespace EBackgroundView
   {
      enum Enum
      {
         Realistic,
         Strategic,
         Count,
      };
   }

   class GBackgroundEarth : public GUIContainer 
   {
      friend SP2::GDataDisplayLayer;
      friend SP2::GAtlasEarth;
      friend SP2::GOverlayCity;
      friend SP2::GClient;
   public:
      bool OnCreate();
      bool OnDestroy();
      void OnShow();
      void OnHide();

      GUI::EEventStatus::Enum OnHotkey(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      
      GUI::EEventStatus::Enum OnMouseDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
	   GUI::EEventStatus::Enum OnMouseWheel(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnKeyDown(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnKeyUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      
      bool OnLoseFocus(const GBaseObject * in_pFocusTo, GBaseObject * Caller);

      void OnDrag(const GEventData & in_EventData, const GVector2D<INT32> & in_MouseOffset, const GVector2D<INT32> & in_MouseRelative);
      void OnDragEnd(UINT8);

      void SwitchView(EBackgroundView::Enum in_eNewView, REAL32 in_fTime);

      bool Animate();
      void Initialize();
      //bool OutlineCountry(vector<UINT32> in_viRegionIDs);

      //! Fills a specific region with a color
      bool SetRegionColor(UINT32 in_iRegionID, const GColorRGBReal& in_Color, bool in_Animate = false);

      //! Removes the color from the specified region
      bool ClearRegionColor(UINT32 in_iRegionID);

      //! Removes the color from all regions
      bool ClearAllRegionColors();

      //! Send to back units model
      void SendToBackUnits();

      //! Structure used to describe a "spot" for the DisplaySpotInformation method
      struct GSpotInfo
      {
         GVector2D<REAL32>    m_Coord;
         REAL32               m_fRadius;
         GColorRGBInt         m_Color;
      };

      //! Display information using "spot" particles on the earth
      bool DisplaySpotInformation(const vector<GSpotInfo>& in_vData);

      // Notification signs management
      struct GNotificationSignData
      {
         CGFX_Model_Itf*         m_pModel;
         CGFX_Transform_2D_Itf*  m_pTrans;
         GVector3D<REAL32>       m_Pos;
         GUI::GBaseObject*       m_pWindow;
      };
      typedef list< pair<GNotificationSignData, REAL64> >::iterator GNotificationSignNode;
      
      //! Iterator for currently displayed notification signs
      typedef void* GNotificatonSign;

      
      //! Add a notification sign on the earth
      void AddNotificationSignOnEarth(const GVector2D<REAL32>& in_Coordinates, const GString& in_NotificationMessage);

      //! Remove a notification sign from the earth
      GNotificationSignNode RemoveNotificationSignOnEarth(GNotificatonSign in_Sign);

      //! Remove all notification signs
      void RemoveNotificationSigns();



      //! Create camera animation to go to another country
      void CreateCameraAnimation(const GVector2D<REAL32> * in_pLocation = NULL);
      REAL32 PlayCameraAnimation();

      //! Add a country that its units must be displayed
      void DisplayCountryUnits(UINT32 in_iCountryID);

      //! Remove a country from displaying its country list
      void HideCountryUnits(UINT32 in_iCountryID);

      //! Remove a country from displaying its country list
      void HideAllCountryUnits();

      //! Return if specified country units are displayed
      bool IsCountryUnitsDisplayed(UINT32 in_iCountryID);

      //! Change if combats must be displayed on the globe
      void DisplayCombats(bool in_bShow);

      //! Change if Bombardments must be displayed on the globe
      void DisplayBombardments(bool in_bShow);
      bool DisplayBombardments() const;


      //! Return if combats are displayed on the globe
      bool DisplayCombats() const;

      //! Display the lauch of a nuke
      bool DisplayNuke(const GLaunchedNuke& in_NukeInfo);

      bool UpdateEarthPosition(SEarthState & in_State);

      //! Pointer to another window, if !NULL left click is forwarded to the pointed window else click is catched normally
      GUI::GBaseObject*     m_pReturnWindow;
      ReturnClickedPosition m_ReturnClickedPosition;

	   // Sphere Control (Political vs Military control)
      void ShowSphereControlWindow(const GVector2D<INT32>& in_MousePos);
      void HideSphereControlWindow(void);

      static GVector3D<REAL32> ConvertSphericalToRectangular(const GVector2D<REAL32>& in_Position);
      static GVector3D<REAL32> ConvertSphericalToRectangular(const GVector3D<REAL32>& in_Position);
      static GVector2D<REAL32> ConvertRectangularToSpherical(const GVector3D<REAL32>& in_Position);

      inline bool ClipOnEarth(const GVector3D<REAL32>& in_Point) const;
      inline bool ClipInFrontOfEarth(const GVector3D<REAL32>& in_Point) const;
      bool Clip3DPointIn2D(const GVector3D<REAL32>& in_Point) const;
      inline void Project3DPointIn2D(const GVector3D<REAL32>& in_Point, GVector2D<INT32>& out_Result) const;
      inline bool ProjectAndClip3DPointIn2D(const GVector3D<REAL32>& in_Point, GVector2D<INT32>& out_Result) const;
      inline bool ProjectAndClipOnEarth3DPointIn2D(const GVector3D<REAL32>& in_Point, GVector2D<INT32>& out_Result) const;
      inline bool ProjectAndClipInFrontOfEarth3DPointIn2D(const GVector3D<REAL32>& in_Point, GVector2D<INT32>& out_Result) const;

      static GVector2D<REAL32> ScreenAlignTransform(const GVector2D<INT32>& in_ScreenPosition, const GVector2D<INT32>& in_ScreenOffset);
      static GVector2D<REAL32> ScreenAlignTransform(const GVector2D<REAL32>& in_ScreenPosition, const GVector2D<INT32>& in_ScreenOffset);
      static GVector2D<REAL32> ScreenAlignTransform(const GVector2D<INT16>& in_ScreenPosition, const GVector2D<INT32>& in_ScreenOffset);

      inline void CameraAlignTransform(CGFX_Transform_Itf* in_pTfx);

      void HideOverlayComponents(CGFX_Model_Itf* in_pModel, UINT32 l_iNextAvailableChild);

      void ToggleUnitDisplayIconsFlags();

      // Takes screen coordinates (x, y) and translates them onto earth coordinates
      // returns false if coordinates of the screen are not on the earth
      bool ScreenToLongLat(GVector2D<INT32>, GVector2D<REAL32> & out_LatLong);

      //! Turn elevation bump application on or off
      void ApplyElevationBump(bool in_bApply);

      void CloudsEnable();
      void CloudsDisable();
      bool CloudsEnabled();

      void MoonEnable();
      void MoonDisable();
      bool MoonEnabled();

      void SunEnable();
      void SunDisable();
      bool SunEnabled();

      void DayNightCycleEnable();
      void DayNightCycleDisable();
      bool DayNightCycleEnabled();

      void SetAtlasZoomLevel(SP2::EZoomLevel::Enum in_eZoomLevel);

      void ChangeEarthDetail(EQualityLevel::Enum);
      EQualityLevel::Enum CurrentEarthDetail() const;

      void ChangeTxtrQuality(EQualityLevel::Enum in_eTxtrQuality);
      EQualityLevel::Enum GBackgroundEarth::TxtrQuality();

      //! Earth position locking functions
      void LockEarthPosition(){m_bLocked = true;};
      void UnlockEarthPosition(){m_bLocked = false;};
      bool Locked() const {return m_bLocked;};

      void SendOverlayToBack();

   private:
      
   
      void ReadConfig(const Hector::GTreeNode<GXMLNode>* in_pTreeNode, 
                      const GString& in_sPrefix);

      INT32 ConfigValue(const GString& in_sParamName, 
                        INT32 in_iDefaultValue);

      REAL32 ConfigValue(const GString& in_sParamName, 
                         REAL32 in_fDefaultValue);

      GColorRGBInt ConfigValue(const GString& in_sParamName, 
                               GColorRGBInt in_DefaultValue);

      const GString& ConfigValue(const GString& in_sParamName, 
                                 const GString& in_sDefaultValue);

      void ClearConfig();

      hash_map<GString, GString>*      m_pConfigData;

      bool CreateBackgroundScene();

      bool CreateRealisticEarth();

      bool CreateStrategicEarth();

      bool CreateBackgroundInfo();

      bool CreateTransitionAnimation();

      void HandleContextMenu(class GContextMenuItem* in_pItem);
      void HandleContextMenuMilitary(class GContextMenuItem* in_pItem);
      void HandleContextMenuDeployement(class GContextMenuItem* in_pItem);

      void UpdateCamera();
      void UpdateHalo();
      void UpdateSun();
      void UpdateFlares();
      void UpdateMoon();
      void UpdateClouds();

      //! Updates units display on the globe
      void UpdateUnits();


      void UpdateCameraParameters(const GVector2D<REAL32>& in_Coord,
                                  REAL32 in_fDistance,
                                  CGFX_Camera_Itf* in_pCam, 
                                  CGFX_Transform_SRT_Itf* in_pTfx);

      // Convert the mouse position to a position on the earth sphere
      // Returns true if click is inside the sphere, false otherwise
      bool MouseToEarth(GVector2D<INT32>, GVector3D<REAL32>& out_EarthPos);

      // Creates a plane Model for the specified flare texture
      CGFX_Model_Itf* CreateFlarePlane(const GString& in_sFlareTxtr);

      EBackgroundView::Enum               m_eView;

      // Scence GFX objects
      CGFX_Scene_Itf*                     m_pScene;

      // Camera GFX objects
      CGFX_Model_Itf*                     m_pCameraModel;
      CGFX_Transform_SRT_Itf*             m_pCameraTransform;
      CGFX_Camera_Itf*                    m_pCamera;
      CGFX_Action_Itf*                    m_pCameraAction;

      // Sun GFX objects
      CGFX_Model_Itf*                     m_pSunModel;
      CGFX_Transform_SRT_Itf*             m_pSunTransform;
      CGFX_Light_Itf*                     m_pSunLight;

      // Realistic earth display objects
      CGFX_Model_Itf*                     m_pRealisticMasterModel;
      CGFX_Model_Itf*                     m_pRealisticOverlayModel;

      // Sun flares
      CGFX_Model_Itf*                     m_pFlareMasterModel;
      CGFX_Model_Itf*                     m_pFlareModel[c_NbFlares + 1];
      CGFX_Transform_Itf*                 m_pFlareTransform[c_NbFlares + 1];

      // Moon GFX objects
      CGFX_Model_Itf*                     m_pMoonModel;
      CGFX_Transform_Itf*                 m_pMoonTransform;
      GFX::GMaterial*                     m_pMoonMaterial;

      // Stars GFX objects
      CGFX_Model_Itf*                     m_pStarsModel;
      GFX::GMesh*                         m_pStarsMesh;
      GFX::GMaterial*                     m_pStarsMaterial;
      CGFX_Texture_Itf*                   m_pStarsTexture;

      // Earth GFX objects
      CGFX_Model_Itf*                     m_pEarthModelMaster;
      GFX::GMaterial*                     m_pEarthMaterial;
      GFX::GMaterial*                     m_pEarthMaterialNoSun;
      CGFX_Transform_SRT_Itf*             m_pEarthTransformTemp;

      CGFX_Model_Itf*                     m_pEarthModel[NB_EARTH_DIVS];
      GFX::GMesh*                         m_pEarthMesh[NB_EARTH_DIVS];
      GFX::GOverrideTSampler*             m_pEarthOverride[NB_EARTH_DIVS];
      CGFX_Texture_Itf*                   m_pEarthTexture[NB_EARTH_DIVS];
      CGFX_Texture_Itf*                   m_pEarthTextureLow[NB_EARTH_DIVS];
      GFX::GOverrideTSampler*             m_pDarkOverride[NB_EARTH_DIVS];
      GFX::GOverrideTSampler*             m_pBumpOverride[NB_EARTH_DIVS];
      CGFX_Texture_Itf*                   m_pBumpTexture[NB_EARTH_DIVS];

      GVector3D<REAL32>                   m_pEarthCorners[NB_EARTH_DIVS][5];

      vector<UINT32>                      m_pEarthRegions[NB_EARTH_DIVS];
      vector<INT8>                        m_vRegionRefCount;
      vector<CGFX_Parameter*>             m_vRegionVisibilityParams;
      vector<CGFX_Color_Parameter*>       m_vRegionColorParams;

      // Night GFX objects
      CGFX_Texture_Itf*                   m_pNightTexture;

      // Water specular GFX objects
      CGFX_Model_Itf*                     m_pWaterModel;
      GFX::GMesh*                         m_pWaterMesh;
      GFX::GMaterial*                     m_pWaterMaterial;
      CGFX_Texture_Itf*                   m_pWaterTexture;

      // Atlas GFX objects
      GAtlasEarth*                        m_pAtlasEarth;
      GOverlayCity*                       m_pCityOverlay;

      // Atmospheric halo GFX objects
      CGFX_Model_Itf*                     m_pAtmoHaloModel;
      CGFX_Transform_Itf*                 m_pAtmoHaloTransform;

      // Cloud GFX objects
      CGFX_Model_Itf*                     m_pCloudModel;
      GFX::GMesh*                         m_pCloudMesh;
      CGFX_Transform_SRT_Itf*             m_pCloudTransform;
      GFX::GMaterial*                     m_pCloudMaterial;
      CGFX_Texture_Itf*                   m_pCloudTexture;

      // Strategic warfare earth display objects
      CGFX_Model_Itf*                     m_pStrategicMasterModel;

      CGFX_Model_Itf*                     m_pStrategicEarthModelMaster;
      GFX::GMaterial*                     m_pStrategicEarthMaterial;

      CGFX_Model_Itf*                     m_pStrategicEarthModels[NB_EARTH_DIVS];
      GFX::GOverrideTSampler*             m_pStrategicEarthOverrides[NB_EARTH_DIVS];
      GFX::GOverrideTSampler*             m_pStrategicEarthBumpOverrides[NB_EARTH_DIVS];
      CGFX_Texture_Itf*                   m_pStrategicEarthTextures[NB_EARTH_DIVS];

      CGFX_Model_Itf*                     m_pStrategicEarthLinesModel;
      GFX::GMesh*                         m_pStrategicEarthLines;
      GFX::GMaterial*                     m_pStrategicEarthLinesMaterial;

      //! Track that handles transition between realistic & strategic warfare earth view
      CGFX_Track_Itf*                     m_pTransitionViewTrack;

      // Filled regions of earth GFX objects
      CGFX_Model_Itf*                     m_pFillModel;
      CGFX_Model_Itf*                     m_pFillModelStrategic;
      GFX::GMesh*                         m_pFillMesh;
      GFX::GDefaultVertexShader*          m_pFillVS;

      // Particles for earth information display GFX objects
      CGFX_Model_Itf*                     m_pDisplayParticlesModelMaster;
      GFX::GMesh*                         m_pDisplayParticlesMesh;
      GFX::GMaterial*                     m_pDisplayParticlesMaterial;

      // Country border GFX objects
      CGFX_Model_Itf*                     m_pOutlineModel;
      GFX::GMesh*                         m_pOutlineMesh;

      // Animation on the selected country color;
      CGFX_Track_Itf*                     m_SelectedCountryTrack;
      GFX::GAnimationMultiple*            m_SelectedCountryAnimation[3];
      CGFX_Animation_Linear_Itf *         m_SelectedCountryLinearAnimation[3];

      // Drag box GFX objects
      CGFX_Model_Itf*                     m_pDragBoxModel;
      CGFX_Transform_2D_Itf*              m_pDragBoxTransform;
      CGFX_Texture_Itf*                   m_pDragBoxTexture;

      //Notify objects
      CGFX_Model_Itf*                     m_pNotificationParent;
      GUI::GUIContainer*                  m_pNotificationContainer;


      HCURSOR                             m_DefaultMouseCursor;

      // Constants for camera distance calculation from zoom level
      REAL32                              m_fMinCamDist;
      REAL32                              m_fMaxCamDist;
      REAL32                              m_fDistCalcA;
      REAL32                              m_fDistCalcB;

      // Camera positioning
      CGFX_Parameter*                     m_pfCamDist;
      CGFX_Parameter*                     m_pfCamAngleX;
      CGFX_Parameter*                     m_pfCamAngleY;

      // Sun & moon positioning
      REAL32                              m_fLastTime;
      REAL32                              m_fDayFactor;
      REAL32                              m_fYearFactor;

      // Clouds positioning
      REAL32                              m_fCloudFadeStart;
      REAL32                              m_fCloudFadeStop;
      REAL32                              m_fCloudFadeDeltaInv;
      REAL32                              m_fCloudFactor;
      UINT32                              m_iNbClouds;
      UINT32                              m_iCurrentCloud;

      // Atlas view generation
      UINT32                              m_iAtlasID[2];
      GVector2D<REAL32>                   m_AtlasPos[2];

      // Country outline color
      GColorRGBInt                        m_OutlineColor;

      GFile m_PointerMoveUnit;
      GFile m_PointerMoveUnitTo;
      GFile m_PointerMoveNo;
      GFile m_PointerMergeUnit;
      GFile m_PointerMoveUnitAttack;

      list< pair<GNotificationSignData, REAL64> >        m_Signs;
      GFX::GMesh*                                        m_pSignMesh;
      GFX::GMaterial*                                    m_pSignMaterial;

      void UpdateAllNotificationSigns();
      void UpdateNotificationSign(const GNotificationSignData& in_Sign);

      bool  SelectCountryAt(const GVector2D<REAL32> & in_LongLat);
      void  ShowContextMenu();
      void  ShowMilitaryMenu();
      void  ShowDeployementMenu();
      void CheckAndShowOccupyTerritoryContextMenu(UINT32 in_iCountryID, GContextMenu* in_pMenu);
      void CheckAndShowBombardmentContextMenu    (UINT32 in_iRegionID , GContextMenu* in_pMenu);
      void CheckAndShowRelationsContextMenu(UINT32 in_iRegionID, GContextMenu* in_pMenu);
      void CheckAndShowQuickTreatyAnnexationContextMenu(GContextMenu* in_pMenu,UINT32 in_iHumanControlledCountry,UINT32 in_iAICountry);
      void CheckAndShowQuickTreatyMilitaryAccessContextMenu(GContextMenu* in_pMenu,UINT32 in_iHumanControlledCountry,UINT32 in_iAICountry);
      void CheckAndShowQuickTreatyAllianceContextMenu(GContextMenu* in_pMenu,UINT32 in_iHumanControlledCountry,UINT32 in_iAICountry);		
      
      UINT32 FindVisibleUnitAt(const GVector2D<INT32> & in_Position);
      UINT32 FindVisibleCombatAt(const GVector2D<INT32> & in_Position);

      bool                                m_bUnitPicked;
      EMilitaryAction::Flags              m_UnitPickedAction;
      bool                                m_bEarthDragging;
      bool                                m_bUnitDragging;
      bool                                m_bSelectBox;
      bool                                m_PreventShowUnitListJustAfterSelect;
      bool                                m_CancelAllMouseEventsUntilAllButtonsUp;
      REAL32                              m_fDragSpeed;
      set<UINT32>                         m_OrigSelectedGroups;
      GVector2D<INT32>                    m_DragCoord;
      SEarthState                         m_InitialState;
      SEarthState                         m_CurrentState;
      UINT32                              m_UnitToDrag;
      UINT32                              m_UnitToKeepOnDeferDeselect;
      UINT32                              m_UnitToDeferDeselect;

      UINT32                              m_iCountryToOccupy;
		UINT32										m_iCountryToCheckRelations;
      UINT32										m_iCountryToCheckMilitary;
      UINT32                              m_iRegionToBombard;

      // Position drag
      GVector3D<REAL32>                   m_vfEarthClick;      

      // Mini map
      GMiniMap*                           m_pMiniMap;

      //! Current camera matrices
      GMatrix4x4<REAL32>                  m_ViewMat;
      GMatrix4x4<REAL32>                  m_ProjMat;

      CGFX_Renderer_Init_Mode             m_Mode;

      GVector2D<REAL32>                   m_ScreenTransformScale;
      GVector2D<REAL32>                   m_ScreenTransformOffset;

      GVector3D<REAL32>                   m_OriginPos;
      REAL32                              m_fOriginDist;
      REAL32                              m_fOriginDistInv;
      GVector3D<REAL32>                   m_CameraPos;

      //! Last selected region
      INT32 m_iLastRegionSelected;

      //! Actual Latitude & Longitude of the click
      GVector2D<REAL32> m_ClickLongLat;      
      GVector2D<REAL32> m_ClickPosition;   

      //! Earth overlay master
      CGFX_Model_Itf*                     m_pOverlay;

      //! Updates combat display on the globe
      void UpdateCombats();

      //! Updates Bombardment display on the globe
      void UpdateBombardments();

      //! List of country units to display
      vector<UINT32>          m_vCountryUnitsToDisplay;

      //! Flag that indicates if combats must be displayed
      bool                    m_bShowCombat;
      bool                    m_bShowBombardment;

      //! Path display constants
      REAL32                  m_fMinPathDivLength;
      GColorRGBReal           m_UnitPathColors[EUnitFriendship::Count];
      REAL32                  m_fUnitPathElemOffset;

      typedef pair<CGFX_Model_Itf*, UINT32> GUnitPathInfo;
      typedef stdext::hash_map<UINT32, GUnitPathInfo> GUnitPathMap;
      typedef GUnitPathMap::iterator GUnitPathMapIter;

      //! Pointer to current list of paths
      GUnitPathMap            m_UnitPaths;

      //! Unit path GFX objects
      CGFX_Model_Itf*         m_pUnitPathDisplayModel;
      GFX::GOverridePSCascade*m_pUnitPathColorOverrides[EUnitFriendship::Count];

      //! Unit path elems GFX objects & constants
      CGFX_Model_Itf*         m_pUnitPathElemDisplayModel;
      CGFX_Model_Itf*         m_pUnitPathElemSubModels[EUnitFriendship::Count];
      GFX::GMesh*             m_pUnitPathElemMesh;

      GVector2D<INT32>        m_UnitPathElemSize;
      GVector2D<INT32>        m_UnitPathElemOffset;

      //! Unit target GFX objects & constants
      CGFX_Model_Itf*         m_pUnitTargetDisplayModel;
      GFX::GMesh*             m_pUnitTargetMesh[2];
      GFX::GOverrideTSampler* m_pUnitTargetTSamplers[2];

      GVector2D<INT32>        m_UnitTargetSize[2];
      GVector2D<INT32>        m_UnitTargetOffset[2];

      //! Unit GFX objects & constants
      CGFX_Model_Itf*         m_pUnitDisplayModel;
      GFX::GMesh*             m_pUnitMeshes[EUnitFriendship::Count];
      GFX::GOverrideTSampler* m_pUnitTSamplers[EUnitFriendship::Count];
      GFX::GOverrideTSampler* m_pUnitShipsTSamplers[EUnitFriendship::Count];
      GFX::GOverrideTSampler* m_pUnitShipsBombardingTSamplers[EUnitFriendship::Count];

      //Unit flags
      GFX::GMesh*                      m_pUnitFlagMesh;
      vector<GFX::GOverrideTSampler*>  m_vUnitFlagsTSamplers;
      GVector2D<UINT32>                m_UnitFlagSize;
      GVector2D<UINT32>                m_UnitFlagHalfSize;
      GFX::GMesh*                      m_pUnitFlagSelectionMeshes[EUnitFriendship::Count];
      GFX::GOverrideTSampler*          m_pUnitFlagSelectionTSamplers[EUnitFriendship::Count];
      GVector2D<UINT32>                m_UnitFlagSelectionSize[EUnitFriendship::Count];
      GVector2D<UINT32>                m_UnitFlagSelectionSizeHalf[EUnitFriendship::Count];
      


      GVector2D<UINT32>       m_UnitSize[EUnitFriendship::Count];
      GVector2D<UINT32>       m_UnitSizeHalf[EUnitFriendship::Count];

      //! Unit selection GFX objects & constants
      GFX::GMesh*             m_pUnitSelectionMeshes[EUnitFriendship::Count];
      GFX::GOverrideTSampler* m_pUnitSelectionTSamplers[EUnitFriendship::Count];

      GVector2D<UINT32>       m_UnitSelectionSize[EUnitFriendship::Count];
      GVector2D<UINT32>       m_UnitSelectionSizeHalf[EUnitFriendship::Count];

      //! Unit status GFX objects & constants
      CGFX_Model_Itf*         m_pUnitStatusDisplayModel;
      GFX::GMesh*             m_pUnitStatusMesh;

      GFX::GOverrideTSampler* m_pUnitStatusTSamplers[EMilitaryStatus::CountVisible];

      GVector2D<INT32>        m_UnitStatusSize;

      //! Unit progress GFX objects & constants
      CGFX_Model_Itf*         m_pUnitProgressMasterModel;
      GFX::GMesh*             m_pUnitProgressBGMesh;
      GFX::GMesh*             m_pUnitProgressPercentMesh;
      //CGFX_Model_Itf*         m_pUnitProgressDisplayModel;
      //CGFX_Model_Itf*         m_pUnitProgressBGModel;
      //CGFX_Transform_2D_Itf*  m_pUnitProgressTfx;
      //CGFX_Transform_Itf*     m_pUnitProgressPercentTfx;

      UINT32                  m_iUnitProgressHeight;
      UINT32                  m_iUnitProgressDistance;

      //! Combat GFX objects
      CGFX_Model_Itf*         m_pCombatDisplayModel;
      GFX::GMesh*             m_pCombatMesh;
      GFX::GTextureSequence*  m_pCombatSequence;
      GVector2D<INT32>        m_CombatSize;
      GVector2D<INT32>        m_CombatOffset;
      vector<CGFX_Track_Itf*> m_vCombatAnims;
      REAL32                  m_fCombatFPS;

      CGFX_Model_Itf*         m_pBombardmentDisplayModel;
      GFX::GMesh*             m_pBombardmentMesh;
      GFX::GTextureSequence*  m_pBombardmentSequence;
      GVector2D<INT32>        m_BombardmentSize;
      GVector2D<INT32>        m_BombardmentOffset;
      vector<CGFX_Track_Itf*> m_vBombardmentAnims;
      REAL32                  m_fBombardmentFPS;



      //! Update position of nuke icons
      void UpdateNukes();

      //! Destroy a nuke display
      bool DestroyNuke(UINT32 in_iNukeID);
      bool DestroyAllNukes();

      //! Nuke display GFX objects & constants
      CGFX_Model_Itf*         m_pNukeTrailMasterModel;
      CGFX_Model_Itf*         m_pNukeExplodeMasterModel;
      GFX::GTextureSequence*  m_pNukeExplodeSequence;
      CGFX_Model_Itf*         m_pNukeIconMasterModel;
      GFX::GMesh*             m_pNukeIconMesh;

      REAL32                  m_fNukeMinPathDiv;
      REAL32                  m_fNukeMaxPathHeightFactor;
      REAL32                  m_fNukeTrailFadeTime;

      GVector2D<UINT32>       m_NukeIconSize;
      GVector2D<UINT32>       m_NukeIconSizeHalf;

      REAL32                  m_fNukeBlinkFreq;
      REAL32                  m_fNukeBlinkFreqInv;
      REAL32                  m_fNukeBlinkScale;
      REAL32                  m_fNukeBlinkOffset;

      GColorRGBReal           m_NukeBlinkColors[3];

      REAL32                  m_fNukeExplodeDelay;
      REAL32                  m_fNukeExplodeSize;
      REAL32                  m_fNukeExplodeFPS;

      CGFX_Model_Itf*         m_pNukeFlashMasterModel;
      GFX::GMesh*             m_pNukeFlashMesh;
      GFX::GTextureSequence*  m_pNukeFlashSequence;

      REAL32                  m_fNukeFlashFPS;

      struct SNukeInfo
      {
         static INT32 const            c_fExploded = 1;

         REAL64                  m_fNukeStartTime;
         CGFX_Action_Itf*        m_pNukeAction;
         // 0 is nuke, 1 is long range AMDS, 2 is short range AMDS
         UINT32                  m_iModelIDs[3];
         // 0 is nuke explosion, 1 is long range AMDS explosion, 2 is short range AMDS explosion, 3 is flash
         UINT32                  m_iExplodeModelIDs[4];
         // 0 is miss, 1 is hit
         UINT32                  m_iAMDSExplodeType[2];

         // Can be a combination of those flags:
         //    c_fExploded
         UINT32                  m_iFlags;
      };

      void CreateAMDS(UINT32 in_iAmdsID, 
                      const GLaunchedNuke& in_NukeInfo, 
                      SNukeInfo& io_Info);

      UINT32 CreateMissileTrail(CGFX_Action_Itf* in_pAction,
                                const GVector2D<REAL32>& in_SourceCoord,
                                const GVector2D<REAL32>& in_TargetCoord,
                                REAL32 in_fStartTime,
                                REAL32 in_fTotalTime,
                                REAL32 in_fInterceptionRatio,
                                GVector3D<REAL32>* out_pLastPosition = NULL);

      UINT32 CreateExplosionAnimation(CGFX_Model_Itf* in_pModel, 
                                      CGFX_Action_Itf* in_pAction, 
                                      GFX::GTextureSequence* in_pSequence, 
                                      REAL32 in_fFPS,
                                      REAL32 in_fStartTime);

      //! Association between nuke IDs and information
      hash_map<UINT32, SNukeInfo>   m_NukeMap;

      //! AMDS explosion display GFX objects & constants
      // 1 is explosion, 0 is miss
      CGFX_Model_Itf*         m_pAMDSExplodeMasterModel[2];
      GFX::GMesh*             m_pAMDSExplodeMesh[2];
      GFX::GTextureSequence*  m_pAMDSExplodeSequence[2];

      REAL32                  m_fAMDSExplodeSize[2];
      REAL32                  m_fAMDSExplodeFPS[2];

      // 0 is long range, 1 is short range
      REAL32                  m_fAMDSStart[2];

      SP2::GProgressMeter*    m_pProgressMeter;

      bool                    m_bDisplayUnitsAsFlags;

      static const GString    TypeName;

      REAL32                  m_fMaxLatitude;
      REAL32                  m_fMinLatitude;
      
      
      REAL32                  m_fEarthSegmentModifier;

      EQualityLevel::Enum     m_eCurrentEarthQuality;

      bool                    m_bDayNightCycle;

      bool                    m_bCamDirty;

      EQualityLevel::Enum     m_eTxtrQuality;

      //! Earth position locking members
      bool m_bLocked;

   protected:
      GBackgroundEarth();

      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

      // register the global client hotkeys
      void RegisterHotkeys();
      // Unregister global hot keys
      void UnregisterHotkeys();
   };
}


#endif //_SP2_BACKGROUND_EARTH_H_
