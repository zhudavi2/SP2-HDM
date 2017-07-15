/**************************************************************
*
* sp2_unit_view_parts.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_unit_view_parts.h"

// Uncomment to log benchmark of piece loading
//#define GOLEM_BENCH_UNIT_LOAD

const GVector3D<REAL32> c_ViewDirection(1, 0, 0);

GUnitViewParts::GUnitViewParts(UINT32 in_iWidth, UINT32 in_iHeight)
{
   Memory::Clear(this);

   // Create the base scene objects
   {
      m_pScene             = g_Joshua.Renderer()->Create_Scene();

      m_pCameraModel       = g_Joshua.Renderer()->Create_Model();
      m_pCameraTransform   = g_Joshua.Renderer()->Create_Transform_SRT();
      m_pCamera            = g_Joshua.Renderer()->Create_Camera();

      m_pLightModel        = g_Joshua.Renderer()->Create_Model();
      m_pLightTransform    = g_Joshua.Renderer()->Create_Transform_SRT();
      m_pLight             = g_Joshua.Renderer()->Create_Light();

      m_pPartRootModel     = g_Joshua.Renderer()->Create_Model();

      m_pTxtrRender        = g_Joshua.Renderer()->TextureManager()->
                              CreateTextureRenderer(PIXELFMT_A8R8G8B8,
                                                    in_iWidth,
                                                    in_iHeight,
                                                    true,
                                                    GString("SP2ViewParts : ") + 
                                                    GString( (UINT32) ( (void *) this) ) );
   }

   // Create the base scene hierarchy
   {
      // Create the model hierarchy
      m_pScene->Get_Root_Model()->Add_Child(m_pCameraModel);
      m_pScene->Get_Root_Model()->Add_Child(m_pLightModel);
      m_pScene->Get_Root_Model()->Add_Child(m_pPartRootModel);
      m_pScene->Set_Active_Camera(m_pCamera);

      // Setup the camera model
      m_pCameraModel->Set_Transform(m_pCameraTransform);
      m_pCameraModel->Set_Primitive(m_pCamera);

      // Setup the light model
      m_pLightModel->Set_Transform(m_pLightTransform);
      m_pLightModel->Set_Primitive(m_pLight);
   }

   // Setup the base scene parameters
   {
      // Setup Ambient
      m_pScene->GlobalAmbient().R = 0.25f;
      m_pScene->GlobalAmbient().G = 0.25f;
      m_pScene->GlobalAmbient().B = 0.25f;
      m_pScene->GlobalAmbient().A = 0.25f;
      m_pScene->ClearFrameColor() = GColorRGBReal(0, 0, 0, 255);
      m_pScene->ClearFrameBuffer(true);

      m_pScene->DefaultMaterial()->RenderStates(0)->Value(GFX::RSTATE_ZBUFFER_TEST_FUNCTION) = GFX::TEST_ALWAYS;

      // Setup Camera
      m_pCamera->View_Type(GFX::VIEW_PERSPECTIVE);
      m_pCamera->Up_Vector() = GVector3D<REAL32>(0,0,-1);
      m_pCamera->Near_Plane() = 10.f;
      m_pCamera->Far_Plane() = 10000.f;
      m_pCamera->Aspect_Ratio() = (REAL32) in_iWidth / (REAL32) in_iHeight;
      m_pCamera->FOV() = (REAL32) Math::PI / 8.f;

      // Setup Transform
      m_pCameraTransform->Translation().Y = -900.f;
      m_pCameraTransform->Translation().Z = -550.0f;
      m_pCameraTransform->Translation().X = 200.0f;

      // Setup Light
      m_pLight->Color().R = 1.0;
      m_pLight->Color().G = 1.0;
      m_pLight->Color().B = 1.0;
      m_pLight->Color().A = 1.0;

      m_pLight->Range() = 1000.0;
      m_pLight->Attenuation_Constant() = 1.0f;
      m_pLight->Attenuation_Linear() = 0.f;

      // Setup Light Transform
      m_pLightTransform->Translation().X = -100.0;
      m_pLightTransform->Translation().Y = -400.0;
      m_pLightTransform->Translation().Z = -400.0;
   }

   // Setup the texture renderer
   {
      m_pTxtrRender->RenderTree()->AddScene(m_pScene);

      m_pTxtrRender->RenderFrequency(GFX::TXTRENDER_NEVER);

      m_pImage = GImage2D::AllocObject();
   }
}

GUnitViewParts::~GUnitViewParts()
{
   SAFE_RELEASE(m_pScene);
   SAFE_RELEASE(m_pCameraModel);
   SAFE_RELEASE(m_pCameraTransform);
   SAFE_RELEASE(m_pCamera);
   SAFE_RELEASE(m_pLightModel);
   SAFE_RELEASE(m_pLightTransform);
   SAFE_RELEASE(m_pLight);
   SAFE_RELEASE(m_pPartRootModel);
   SAFE_RELEASE(m_pTxtrRender);

   GImage2D::FreeObject(m_pImage);
}

UINT32 GUnitViewParts::LoadUnitPictures(UINT32 in_iTypeID, vector<GPicture> out_vPieceImages[c_iMaxDesignPieceCount] )
{
   UINT32 l_iPieceCount = 0;
   GTimeCounter l_Timer;
   REAL64 l_fStart, l_fStop;
   REAL64 l_fTotalTime = 0;

   // Make sure a design exists for this unit type
   GDesignVariationHolder::GType* l_pType = g_ClientDAL.DesignVariationHolder.Type(in_iTypeID);
   if(l_pType)
   {
      // Load each piece
      for(UINT32 i = 0;i < c_iMaxDesignPieceCount;i ++)
      {
         if(l_pType->Pieces[i] )
         {
            l_iPieceCount ++;

            // Load each variation of the piece
            vector<GDesignVariationHolder::GType::GPiece::GVariation>& l_vVariations = 
               l_pType->Pieces[i]->Variations;
            for(UINT32 j = 0;j < l_vVariations.size();j ++)
            {
               l_fStart = l_Timer.GetPrecisionTime();
               CGFX_Model_Itf* l_pVariationModel = g_Joshua.Renderer()->Helper()->LoadGFXModel(l_vVariations[j].Model);
               l_fStop = l_Timer.GetPrecisionTime();
               l_fTotalTime += l_fStop - l_fStart;
#ifdef GOLEM_BENCH_UNIT_LOAD
               g_Joshua.Logger()->Log(MSGTYPE_MSG, GString("Load model : ") + GString(l_fStop - l_fStart) );
#endif
               l_fStart = l_Timer.GetPrecisionTime();;
               // Make sure variation was loaded correctly
               if(l_pVariationModel)
               {
                  // Setup scene for render
                  m_pPartRootModel->Clear_Children();
                  m_pPartRootModel->Add_Child(l_pVariationModel);

                  // Make sure camera looks correctly at object
                  UpdateCamera();

                  l_fStop = l_Timer.GetPrecisionTime();
                  l_fTotalTime += l_fStop - l_fStart;
#ifdef GOLEM_BENCH_UNIT_LOAD
                  g_Joshua.Logger()->Log(MSGTYPE_MSG, GString("Update scene : ") + GString(l_fStop - l_fStart) );
#endif
                  l_fStart = l_Timer.GetPrecisionTime();;

                  out_vPieceImages[i].resize(out_vPieceImages[i].size() + 1);
                  out_vPieceImages[i].back().m_iVariation = j;

                  // Replace texture if needed
                  if(l_pType->Pieces[i]->Colorize)
                  {
                     GFX::GTextureSampler* l_pSampler = NULL;
                     GString& l_sOldTexture = l_vVariations[j].DefaultTexture;
                     CGFX_Texture_Itf* l_pDefaultTxtr = FindDefaultTexture(l_pVariationModel, l_sOldTexture, &l_pSampler);

                     if(l_pDefaultTxtr && l_pSampler)
                     {
                        CGFX_Texture_Itf* l_pNewTexture = 
                           g_Joshua.Renderer()->TextureManager()->
                           Create_Texture(l_vVariations[j].ColorizeTexture, 
                                          false, 
                                          l_vVariations[j].ColorizeTexture.Name() );
                        if(l_pNewTexture)
                        {
                           l_pSampler->Texture(l_pNewTexture);
                           l_pNewTexture->Release();

                           l_fStop = l_Timer.GetPrecisionTime();
                           l_fTotalTime += l_fStop - l_fStart;
#ifdef GOLEM_BENCH_UNIT_LOAD
                           g_Joshua.Logger()->Log(MSGTYPE_MSG, GString("Load Txtr : ") + GString(l_fStop - l_fStart) );
#endif
                           l_fStart = l_Timer.GetPrecisionTime();;
                        }
                     }
                  }

                  // Render into texture
                  m_pTxtrRender->RenderFrequency(GFX::TXTRENDER_ONCE);
                  g_Joshua.Renderer()->TextureManager()->UpdateTextureRenderer(m_pTxtrRender);

                  l_fStop = l_Timer.GetPrecisionTime();
                  l_fTotalTime += l_fStop - l_fStart;
#ifdef GOLEM_BENCH_UNIT_LOAD
                  g_Joshua.Logger()->Log(MSGTYPE_MSG, GString("Render scene : ") + GString(l_fStop - l_fStart) );
#endif
                  l_fStart = l_Timer.GetPrecisionTime();;

                  // Create texture
                  m_pTxtrRender->LockImage(m_pImage);
                  CGFX_Texture_Itf* l_pTxtr = g_Joshua.Renderer()->TextureManager()->Create_Texture(
                                                *m_pImage, 
                                                true, 
                                                l_vVariations[j].Model.Name() );

                  l_fStop = l_Timer.GetPrecisionTime();
                  l_fTotalTime += l_fStop - l_fStart;
#ifdef GOLEM_BENCH_UNIT_LOAD
                  g_Joshua.Logger()->Log(MSGTYPE_MSG, GString("Copy image : ") + GString(l_fStop - l_fStart) );
#endif
                  l_fStart = l_Timer.GetPrecisionTime();;

                  m_pTxtrRender->Unlock();

                  out_vPieceImages[i].back().m_pTxtr = l_pTxtr;
               }
               SAFE_RELEASE(l_pVariationModel);
            }
         }
      }
   }
   else
   {
      for(UINT32 i = 0;i < c_iMaxDesignPieceCount;i ++)
      {
         out_vPieceImages[i].clear();
      }
   }
#ifdef GOLEM_DEBUG
   g_Joshua.Log(GString("Loading unit type parts : ") + GString(l_fTotalTime) );
#endif

   return l_iPieceCount;
}

CGFX_Texture_Itf* GUnitViewParts::FindDefaultTexture(CGFX_Model_Itf* in_pModel, const GString& in_sDefaultTxtrName, GFX::GTextureSampler** out_pSampler)
{
   GFX::GMaterial* l_pMat = in_pModel->Get_Material();

   if(l_pMat)
   {
      GFX::GCascadePixelShader* l_pPS = (GFX::GCascadePixelShader*) l_pMat->PixelShader(0);

      if(  l_pPS->NbTextureSamplers() &&
          (l_pPS->TextureSampler(0)->Texture() ) && 
          (l_pPS->TextureSampler(0)->Texture()->Name() == in_sDefaultTxtrName) )
      {
         *out_pSampler = l_pPS->TextureSampler(0);

         return (*out_pSampler)->Texture();
      }
   }

   // The texture was not found yet, explore the hierarchy
   for(UINT32 i = 0;i < in_pModel->Get_Nb_Children();i ++)
   {
      CGFX_Texture_Itf* l_pResult = FindDefaultTexture(in_pModel->Get_Child(i), in_sDefaultTxtrName, out_pSampler);
      if(l_pResult)
      {
         return l_pResult;
      }
   }

   return NULL;
}

void GUnitViewParts::UpdateCamera()
{
   const REAL32 c_fThresholdRelative = 0.01f;

   GBox<REAL32> l_BBox;
   g_Joshua.Renderer()->Helper()->ComputeBoundingBox(m_pPartRootModel, l_BBox);
   GVector3D<REAL32> l_pCorners[8];
   l_BBox.Corners(l_pCorners);

   GVector3D<REAL32> l_Center = l_BBox.Center();
   l_BBox.Offset(l_Center * -1);
   l_BBox.Corners(l_pCorners);

   // Fetch camera matrices
   GVector3D<REAL32> l_ViewVector(1, -1, -1);
   l_ViewVector.Normalize();
   GVector3D<REAL32> l_ViewVectorInv = l_ViewVector * -1;

   m_pCamera->Look_At() = l_ViewVectorInv;
   GMatrix4x4<REAL32> l_Identity;
   l_Identity.LoadIdentity();
   GMatrix4x4<REAL32> l_View = m_pCamera->Get_View_Matrix(l_Identity);
   GMatrix4x4<REAL32> l_Proj = m_pCamera->Get_Projection_Matrix();

   // Fetch the camera planes
   GVector3D<REAL32> l_PlaneNormals[4];
   REAL32 l_PlaneDists[4];

   l_PlaneNormals[0].x = l_Proj(0, 3) + l_Proj(0, 0);
   l_PlaneNormals[0].y = l_Proj(1, 3) + l_Proj(1, 0);
   l_PlaneNormals[0].z = l_Proj(2, 3) + l_Proj(2, 0);
   l_PlaneDists[0]     = l_Proj(3, 3) + l_Proj(3, 0);

   l_PlaneNormals[1].x = l_Proj(0, 3) - l_Proj(0, 0);
   l_PlaneNormals[1].y = l_Proj(1, 3) - l_Proj(1, 0);
   l_PlaneNormals[1].z = l_Proj(2, 3) - l_Proj(2, 0);
   l_PlaneDists[1]     = l_Proj(3, 3) - l_Proj(3, 0);

   l_PlaneNormals[2].x = l_Proj(0, 3) + l_Proj(0, 1);
   l_PlaneNormals[2].y = l_Proj(1, 3) + l_Proj(1, 1);
   l_PlaneNormals[2].z = l_Proj(2, 3) + l_Proj(2, 1);
   l_PlaneDists[2]     = l_Proj(3, 3) + l_Proj(3, 1);

   l_PlaneNormals[3].x = l_Proj(0, 3) - l_Proj(0, 1);
   l_PlaneNormals[3].y = l_Proj(1, 3) - l_Proj(1, 1);
   l_PlaneNormals[3].z = l_Proj(2, 3) - l_Proj(2, 1);
   l_PlaneDists[3]     = l_Proj(3, 3) - l_Proj(3, 1);

   // Normalize plane equations
   for(UINT32 p = 0;p < 4;p ++)
   {
      REAL32 l_fLengthInv = 1.f / l_PlaneNormals[p].Length();
      l_PlaneNormals[p] *= l_fLengthInv;
      l_PlaneDists[p] *= l_fLengthInv;
   }

   const REAL32 c_fThreshold = max(max(l_BBox.Width(), l_BBox.Height() ), l_BBox.Depth() ) * c_fThresholdRelative;

   // Transform BBox in view space & compute the distance for the camera
   REAL32 l_fDist = -FLT_MAX;
   REAL32 l_fZNear = FLT_MAX;
   REAL32 l_fZFar = -FLT_MAX;
   for(UINT32 i = 0;i < 8;i ++)
   {
      l_pCorners[i] = l_View * l_pCorners[i];
      for(UINT32 p = 0;p < 4;p ++)
      {
         REAL32 l_fDistTemp = (c_fThreshold - l_PlaneNormals[p].DotProduct(l_pCorners[i] ) - l_PlaneDists[p] ) / l_PlaneNormals[p].z;
         l_fDist = max(l_fDist, l_fDistTemp);
      }
      l_fZNear = min(l_fZNear, l_pCorners[i].z);
      l_fZFar = max(l_fZFar, l_pCorners[i].z);
   }

   l_fZNear -= l_fDist;
   l_fZFar -= l_fDist;

   // Update the camera transform
   m_pCameraTransform->Translation() = l_Center + (l_ViewVector * l_fDist);
   m_pCamera->Look_At() = l_Center;
   m_pCamera->Near_Plane() = -l_fZFar / 2.f;
   m_pCamera->Far_Plane() = -l_fZNear + m_pCamera->Near_Plane();
}
