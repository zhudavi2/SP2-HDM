/**************************************************************
*
* sp2_detailed_earth_area.h
*
* Description
* ===========
*  Description & implementation of class GDetailedEarthArea
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef __GOLEM_DETAILED_EARTH_AREA_H__
#define __GOLEM_DETAILED_EARTH_AREA_H__

class GTriFinder;

namespace SP2
{

   /*!
   * GUI object that represents a detailed 3D view of a certain area
   * of the earth surface.
   **/
   class GDetailedEarthArea : public GUIContainer
   {
   public:
      static const GString TypeName;
      GDetailedEarthArea();

      // GUI event handlers
      bool OnCreate();
      bool OnDestroy();
      GUI::EEventStatus::Enum OnKeyDown(const GEventData & in_EventData, GUI::GBaseObject* callerPtr);
      GUI::EEventStatus::Enum on_mouse_down(INT16 x, INT16 y, INT16 z, INT16 buttons, GUI::GBaseObject* callerPtr);
      GUI::EEventStatus::Enum on_mouse_move(INT16 x, INT16 y, INT16 z, INT16 buttons, GUI::GBaseObject* callerPtr);
      GUI::EEventStatus::Enum on_mouse_up(INT16 x, INT16 y, INT16 z, INT16 buttons, GUI::GBaseObject* callerPtr);
      GUI::EEventStatus::Enum on_mouse_wheel(INT16 x, INT16 y, INT16 z, INT16 buttons, GUI::GBaseObject* callerPtr);
      GUI::EEventStatus::Enum on_click(INT16 x, INT16 y, INT16 z, INT16 buttons, GUI::GBaseObject* callerPtr);

      //! Sets the earth area to display
      bool DisplayArea(const GVector2D<REAL32>& in_Center, const GVector2D<REAL32>& in_Size);

      //! Create an animation for a unit
      bool CreateUnitAnimation(const vector< pair<REAL32, GVector2D<REAL32> > >& in_vPath);

   private:
      bool MouseToCoordinate(const GVector2D<UINT32>& in_MousePos, GVector3D<REAL32>& out_Coord);
      void ConvertSphericalCoordinate(GVector3D<REAL32>& in_Point);
      void GDetailedEarthArea::ConvertRectangularCoordinate(GVector3D<REAL32>& io_Point);

      // Sky box camera
      CGFX_Camera_Itf*              m_SkyCamera;

      // Scence GFX objects
      CGFX_Scene_Itf*   				m_Scene;

      // Camera GFX objects
      CGFX_Model_Itf*   				m_CameraModel;
      CGFX_Transform_SRT_Itf*   		m_CameraTransform;
      CGFX_Camera_Itf*   				m_Camera;

      // Sun GFX objects
      CGFX_Model_Itf*   				m_SunModel;
      CGFX_Transform_SRT_Itf*   		m_SunTransform;
      CGFX_Light_Itf*   				m_SunLight;

      // Land GFX objects
      CGFX_Model_Itf*   				m_LandModel;
      GFX::GMesh*   				      m_LandMesh;
      GFX::GMaterial*               m_LandMaterial;
      CGFX_Texture_Itf*   			   m_LandTexture;

      // Temp outline GFX objects
      CGFX_Model_Itf*   				m_OutlineModel;
      GFX::GMesh*   				      m_OutlineMesh;

      CGFX_Model_Itf*   				m_WaterModel;
      GFX::GMesh*   				      m_WaterMesh;

      // Location display gizmo GFX objects
      CGFX_Model_Itf*               m_GizmoModel;
      CGFX_Transform_SRT_Itf*       m_GizmoTransform;
      GFX::GMesh*                   m_GizmoMesh;

      // Land subdivision
      UINT32                        m_NbDivs;

      // Land rotation matrix
      GMatrix4x4<REAL32>            m_RotMatrix;
      REAL32                        m_fRadToUnitRatio;

      // Land unit to meter onversion ratio
      REAL32                        m_fUnitToMeterRatio;

      // Camera movement speeds
      REAL32                        m_FreeCamSpeed;
      REAL32                        m_LandCamSpeed;

      // Close button
      GUI::GBaseObject*              m_CloseButton;

      // Used to quickly locate a triangle in the land mesh
      GTriFinder*                   m_TriFinder;

      // Sub-scene objects (displays camera orientation info)
      CGFX_Scene_Itf*   				m_SubScene;
      CGFX_Transform_Itf*           m_SubTransform;

      // Clicked triangle information
      CGFX_Text_2D_Itf*             m_ClickText;

      // Used to remember what triangles are clicked
      vector<UINT32>                m_ClickedTris;
      vector<UINT32>                m_ClickedTrisColors;

      //! Current path master Model
      CGFX_Model_Itf*               m_pUnitPathModel;

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();
   };

}

#endif //__GOLEM_DETAILED_EARTH_AREA_H__
