/**************************************************************
*
* sp2_atlas_earth.h
*
* Description
* ===========
*  Description & implementation of class SP2::GAtlasEarth, a
*  graphical object that can display an atlas view on the 
*  background earth.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef __GOLEM_ATLAS_EARTH_H__
#define __GOLEM_ATLAS_EARTH_H__

#include "sp2_atlas_txtr_generator.h"

namespace SP2
{
   namespace ELevelFGroup
   {
      enum Enum
      {
         Elev,
         Detail,
         Count,
      };
   }

   class GAtlasEarth
   {
      friend class GAtlasEarthThread;
   public:
      GAtlasEarth(class GBackgroundEarth* in_pBGEarth, 
                  UINT32 in_iNbHDivs, 
                  UINT32 in_iNbVDivs, 
                  REAL32 in_fFadeTime);
      ~GAtlasEarth();

      //! Fetch the master gfx model that encapsulate the whole atlas display
      CGFX_Model_Itf* Master();

      //! Update the atlas view (called each frame to be able to load a detailed texture asynchronously)
      void UpdateContent();

      //! Update the content of the atlas view (called each time the camera is moved)
      void UpdateCamera();

      //! Display and set the color of the specified region (if it is currently generated)
      void SetRegionColor(UINT32 in_iRegionID, const GColorRGBReal& in_Color) {;}

      //! Hide the specified region (if it is currently generated)
      void ClearRegionColor(UINT32 in_iRegionID) {;}

      //! Set the possible zoom level
      void SetZoomLevel(EZoomLevel::Enum in_eZoomLevel);

   private:
      //! Create the top atlas level
      void CreateTopAtlasLevel(const GVector2D<REAL32>& in_Center);

      //! Destroy the top atlas level
      void DestroyTopAtlasLevel();

      //! Construct a facet located at the desired location
      UINT32 CreateAtlasFacet(UINT32 in_iStartFacetId,
                              const GFacetList& in_FacetContent,
                              const GVector2D<REAL32>& in_Position,
                              const GVector2D<REAL32>& in_Size);

      GBackgroundEarth*       m_pBGEarth;

      CGFX_Camera_Itf*        m_pCamera;
      CGFX_Transform_SRT_Itf* m_pCamTfx;

      static const UINT32     c_iNbAtlasLevelCorners = 6;
      static const GVector2D<REAL32> c_pScreenCorners[c_iNbAtlasLevelCorners];
      static const bool       c_pCornerLimitsHorizontal[c_iNbAtlasLevelCorners];

      static const UINT32     c_iNbAtlasLevels = 3;
      static const REAL32     c_pLevelDistanceTable[c_iNbAtlasLevels];

      static const REAL32     c_fAtlasGenerationSizeOffset;

      static const UINT32     c_AtlasElevation = 1;
      static const UINT32     c_AtlasWater = 2;
      static const UINT32     c_AtlasRiver = 4;
      static const UINT32     c_AtlasRoad = 8;
      static const UINT32     c_AtlasDetails = c_AtlasWater | c_AtlasRiver | c_AtlasRoad;

      static const UINT32     c_pLevelContentTable[c_iNbAtlasLevels];

      UINT32 m_pAtlasLevelContent[c_iNbAtlasLevels];

      struct GAtlasLevel
      {
         UINT32               m_pFGroups[ELevelFGroup::Count];
         UINT32               m_iGenerationID;
         GRectangle<REAL32>   m_Rect;
         GVector3D<REAL32>    m_Corners[c_iNbAtlasLevelCorners];
      };

      vector<GAtlasLevel>     m_vLevels;
      UINT32                  m_pCurrentFGroups[ELevelFGroup::Count];

      CGFX_Model_Itf*         m_pMaster;
      GFX::GMesh*             m_pEarth;
      GFX::GMaterial*         m_pMat;
      CGFX_Texture_Itf*       m_pTxtr;

      //CGFX_Model_Itf*         m_pRegionMaster;
      //GFX::GMesh*             m_pRegions;
      //GFX::GMaterial*         m_pRegionMat;

      GTriangleListIndexed    m_EarthTris;
      UINT32                  m_iNbHDivs;
      UINT32                  m_iNbVDivs;
      UINT32                  m_iHTriPitch;

      class GAtlasEarthThread : public GThread
      {
         void Process(void*);
      }                       m_GenThread;

      GAtlasTxtrGenerator     m_TxtrGen;
      GMutex                  m_Sync;
      GSemaphore              m_Queue;

      struct GGenParams
      {
         GVector2D<REAL32>    m_Center;
         GVector2D<REAL32>    m_Size;
         UINT32               m_iLevel;
         UINT32               m_iContent;
         UINT32               m_iGenerationID;
      };

      list<GGenParams>        m_vGenParams;
      UINT32                  m_iCurGenLevel;

      struct GResultElem
      {
         UINT32               m_iInfo;
         CGFX_Texture_Itf*    m_pTxtr;
      };

      struct GGenResult
      {
         UINT32               m_iLevel;
         UINT32               m_iContent;
         UINT32               m_iGenerationID;
         vector<GResultElem>  m_vTxtrs;
      };
      vector<GGenResult>      m_vGenResults;

      UINT32                  m_iNextGenerationID;

      void SendResult(GGenResult& in_Results);

      vector<bool>            m_vActiveFacets;

      //void SetupFade(UINT32 in_iFacetID, bool in_bFadeIn);

      //struct GFadeAction
      //{
      //   CGFX_Action_Itf*     m_pAction;
      //   UINT32               m_iFacetID;
      //   UINT32               m_bActive;
      //   UINT32               m_bFadeIn;
      //};

      //list<GFadeAction>       m_vFadeActions;
      //CGFX_Key_Linear         m_FadeInKeys[2];
      //CGFX_Key_Linear         m_FadeOutKeys[2];

      //void DisplayRegion(const GGenParams& in_Params, GRegionTxtr& in_RegionTxtr);

      //// Associates a region ID (first) & an atlas facet (second) to each region facet
      //vector<pair<UINT32, UINT32> > m_vDisplayedRegions;

      //// Associates a region ID to all region facets containing it
      //hash_multimap<UINT32, UINT32> m_RegionFacets;
   };

}

#endif //__GOLEM_DETAILED_EARTH_AREA_H__
