/**************************************************************
*
* sp2_unit_view_scene.h
*
* Description
* ===========
*  Description & implementation of SP2::GUnitViewScene that
*  allows a unit design to be displayed in a simple scene,
*  which can be used in a 2D viewport.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _SP2_UNIT_VIEW_SCENE_H_
#define _SP2_UNIT_VIEW_SCENE_H_

#include "../include/sp2_unit_view_camera.h"

namespace SP2
{
   /*!
    * Class that displays a unit using a movable camera
    **/
   class GUnitViewScene
   {
   public:
      GUnitViewScene();
      ~GUnitViewScene();

      //! Change the aspect ratio of the scene camera
      void ChangeAspectRatio(REAL32 in_fWidth, REAL32 in_fHeight);

      //! Fetch the current camera control object
      GUnitViewCamera& CameraControl();

      //! Get the scene that contains the displayed unit
      CGFX_Scene_Itf* Scene() { return m_pScene; }

      //! Select the unit that we want to display
      void DisplayUnit(UINT32 in_iType, const SP2::GUnitDesign::GDisplay& in_DesignVariation);

      //! Hide the currently displayed unit
      void HideUnit();

      // Show the currentyl displayed unit
      void ShowUnit(void);

      void DisplayAndAnimate(GFile& in_GolemModelFile);

   private:

      //! Finds the specified texture in the model and replace it
      GFX::GCascadePixelShader* ReplaceTexture(CGFX_Model_Itf* in_pModel, 
                                               const GString& in_sOldTxtrName, 
                                               CGFX_Texture_Itf* in_pNewTxtr, 
                                               GColorRGBReal& in_Color);

      // Scence GFX object
      CGFX_Scene_Itf*                     m_pScene;

      // Light GFX objects
      CGFX_Model_Itf*                     m_pLightModel;
      CGFX_Transform_SRT_Itf*             m_pLightTransform;
      CGFX_Light_Itf*                     m_pLight;

      // Unit display main model
      CGFX_Model_Itf*                     m_pUnitRootModel;

      // Animation Track
      CGFX_Track_Itf*                     m_pTrack;

      // Manage the scene camera
      GUnitViewCamera                     m_Camera;

      // Currently displayed unit
      UINT32                              m_iType;
      SP2::GUnitDesign::GDisplay          m_DesignVariation;
      GFX::GCascadePixelShader*           m_pDesignPiecePS[c_iMaxDesignPieceCount];
   };

}

#endif // #ifndef _SP2_UNIT_VIEW_SCENE_H_
