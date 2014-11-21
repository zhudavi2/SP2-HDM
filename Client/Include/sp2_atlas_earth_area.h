/**************************************************************
*
* sp2_atlas_earth_area.h
*
* Description
* ===========
*  Description & implementation of class GAtlasEarthArea
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef __GOLEM_ATLAS_EARTH_AREA_H__
#define __GOLEM_ATLAS_EARTH_AREA_H__

namespace SP2
{
   /*!
   * GUI object that represents an atlas view of a certain area
   * of the earth surface.
   **/
   class GAtlasEarthArea : public GUIContainer
   {
   public:
      GAtlasEarthArea();
      ~GAtlasEarthArea();

      // GUI event handlers
      bool OnCreate();
      bool OnDestroy();

      void OnShow();

      GUI::EEventStatus::Enum OnMouseDown(const GUI::GEventData &, GUI::GBaseObject *);

      //! Sets the earth area to display
      bool DisplayArea(const GVector2D<REAL32>& in_Center);

      //! Called at each frame to update display (units, combats, etc.)
      void Animate();

      //!Handle the context menu commands
      void HandleContextMenu(class GContextMenuItem* in_pMenuItem);
      static const GString TypeName;

   private:
      //! Current area information
      GRectangle<REAL32>      m_Location;

      //! Transformation from long/lat to screen
      GVector2D<REAL32>       m_LLtoScreenScale;
      GVector2D<REAL32>       m_LLtoScreenOffset;

      //! Transformation from screen to long/lat
      GVector2D<REAL32>       m_ScreentoLLScale;
      GVector2D<REAL32>       m_ScreentoLLOffset;

      //! Atlas view GFX objects
      CGFX_Mesh_Itf*          m_pAtlasMesh;
      GFX::GMaterial*         m_pAtlasMat;
      CGFX_Texture_Itf*       m_pAtlasTxtr;

      //! Cities view GFX objects
      CGFX_Model_Itf*         m_pCitiesMasterModel;

      //! Unit display GFX objects
      CGFX_Mesh_Itf*          m_pIconMesh;
      CGFX_Model_Itf*         m_pOwnedUnitsMasterModel;
      CGFX_Model_Itf*         m_pAlliedUnitsMasterModel;
      CGFX_Model_Itf*         m_pEnemyUnitsMasterModel;

      //! Combat display GFX objects
      CGFX_Model_Itf*         m_pCombatsMasterModel;

      //! Unit selection display GFX objects
      CGFX_Model_Itf*         m_pUnitSelectModel;
      CGFX_Transform_2D_Itf*  m_pUnitSelectTx;

      //! Combat selection display GFX objects
      CGFX_Model_Itf*         m_pCombatSelectModel;
      CGFX_Transform_2D_Itf*  m_pCombatSelectTx;

      typedef pair<CGFX_Model_Itf*, UINT32> GUnitPathInfo;
      typedef stdext::hash_map<UINT32, GUnitPathInfo> GUnitPathMap;
      typedef GUnitPathMap::iterator GUnitPathMapIter;

      //! Pointer to current list of paths
      GUnitPathMap            m_UnitPaths;

      //! Unit path GFX objects
      CGFX_Model_Itf*         m_pUnitPathDisplayModel;

      GVector2D<UINT32>       m_ImSize;

      GPixelX8R8G8B8*         m_pAtlasPalette;

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();
   };

}

#endif //__GOLEM_DETAILED_EARTH_AREA_H__
