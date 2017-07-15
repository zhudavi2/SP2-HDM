/**************************************************************
*
* sp2_unit_view_parts.h
*
* Description
* ===========
*  Description & implementation of SP2::GUnitViewParts that
*  allows the loading of pictures for each piece of a possible
*  unit type.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_UNIT_VIEW_PARTS_H_
#define _SP2_UNIT_VIEW_PARTS_H_


namespace SP2
{
   /*!
    * Class that create pictures of unit parts
    **/
   class GUnitViewParts
   {
   public:
      GUnitViewParts(UINT32 in_iWidth, UINT32 in_iHeight);
      ~GUnitViewParts();

      struct GPicture
      {
         UINT32               m_iVariation;
         CGFX_Texture_Itf*    m_pTxtr;
      };

      UINT32 LoadUnitPictures(UINT32 in_iTypeID, vector<GPicture> out_vPieceImages[c_iMaxDesignPieceCount] );

   private:
      //! Finds the specified texture in the model
      CGFX_Texture_Itf* FindDefaultTexture(CGFX_Model_Itf* in_pModel, const GString& in_sDefaultTxtrName, GFX::GTextureSampler** out_pSampler);

      //! Make sure the camera views the entire scene
      void UpdateCamera();

      // Scence GFX object
      CGFX_Scene_Itf*                     m_pScene;

      // Camera GFX objects
      CGFX_Model_Itf*                     m_pCameraModel;
      CGFX_Transform_SRT_Itf*             m_pCameraTransform;
      CGFX_Camera_Itf*                    m_pCamera;

      // Light GFX objects
      CGFX_Model_Itf*                     m_pLightModel;
      CGFX_Transform_SRT_Itf*             m_pLightTransform;
      CGFX_Light_Itf*                     m_pLight;

      // Part display main model
      CGFX_Model_Itf*                     m_pPartRootModel;

      // Scene renderering GFX object
      CGFX_Texture_Renderer_Itf*          m_pTxtrRender;
      GImage2D*                           m_pImage;
   };

}

#endif // #ifndef _SP2_UNIT_VIEW_PARTS_H_
