/**************************************************************
*
* sp2_unit_view_scene.cpp
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

#include "../include/sp2_unit_view_scene.h"

GUnitViewScene::GUnitViewScene()
{
   Memory::Clear(this);

   // Create the bases scene objects
   {
      m_pScene             = g_Joshua.Renderer()->Create_Scene();

      m_pLightModel        = g_Joshua.Renderer()->Create_Model();
      m_pLightTransform    = g_Joshua.Renderer()->Create_Transform_SRT();
      m_pLight             = g_Joshua.Renderer()->Create_Light();

      m_pUnitRootModel     = g_Joshua.Renderer()->Create_Model();
   }

   // Create the base scene hierarchy
   {
      // Create the model hierarchy
      m_pScene->Get_Root_Model()->Add_Child(m_pLightModel);
      m_pScene->Get_Root_Model()->Add_Child(m_pUnitRootModel);

      // Setup the light model
      m_pLightModel->Set_Transform(m_pLightTransform);
      m_pLightModel->Set_Primitive(m_pLight);
   }

   // Setup the base scene parameters
   {
      // Setup Ambient
      m_pScene->GlobalAmbient().R = 0.5f;
      m_pScene->GlobalAmbient().G = 0.5f;
      m_pScene->GlobalAmbient().B = 0.5f;
      m_pScene->GlobalAmbient().A = 0.5f;
      m_pScene->ClearFrameBuffer(false);
      m_pScene->ClearFrameColor() = GColorRGBReal(0.125, 0.03125, 0.0625, 0);

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

   m_Camera.Init(m_pScene);
}

GUnitViewScene::~GUnitViewScene()
{
   m_Camera.Reset();

   SAFE_RELEASE(m_pScene);
   SAFE_RELEASE(m_pLightModel);
   SAFE_RELEASE(m_pLightTransform);
   SAFE_RELEASE(m_pLight);
   SAFE_RELEASE(m_pUnitRootModel);
   SAFE_RELEASE(m_pTrack);
}

void GUnitViewScene::ChangeAspectRatio(REAL32 in_fWidth, REAL32 in_fHeight)
{
   m_Camera.SetPerspectiveFromViewportSize(GVector2D<REAL32>(in_fWidth, in_fHeight) );
}

GUnitViewCamera& GUnitViewScene::CameraControl()
{
   return m_Camera;
}

void GUnitViewScene::DisplayUnit(UINT32 in_iType, const SP2::GUnitDesign::GDisplay& in_DesignVariation)
{
   // Check if type & piece are the same
   if( (in_iType == m_iType) &&
       (in_DesignVariation.PieceIndex[0] == m_DesignVariation.PieceIndex[0]) && 
       (in_DesignVariation.PieceIndex[1] == m_DesignVariation.PieceIndex[1]) && 
       (in_DesignVariation.PieceIndex[2] == m_DesignVariation.PieceIndex[2]) )
   {
      // Nothing to do if color is the same
      if(in_DesignVariation.ColorIndex == m_DesignVariation.ColorIndex)
      {
         return;
      }

      // Simply change the material modulation color
      for(UINT32 i = 0;i < c_iMaxDesignPieceCount;i ++)
      {
         if(m_pDesignPiecePS[i] )
         {
            m_pDesignPiecePS[i]->ColorFactor() = c_MilitaryColorPalette[in_DesignVariation.ColorIndex];
         }
      }

      m_DesignVariation.ColorIndex = in_DesignVariation.ColorIndex;
      return;
   }

   m_pDesignPiecePS[0] = NULL;
   m_pDesignPiecePS[1] = NULL;
   m_pDesignPiecePS[2] = NULL;

   // Remove previous model display
   m_pUnitRootModel->Clear_Children();
   m_pUnitRootModel->Visibility() = true;

   // Make sure a design exists for this unit type
   bool l_bLoadGlobalDefault = false;
   GDesignVariationHolder::GType* l_pType = g_ClientDAL.DesignVariationHolder.Type(in_iType);
   if(l_pType)
   {
      // Load all pieces of this design
      CGFX_Model_Itf* l_pDesignPieces[c_iMaxDesignPieceCount];
      CGFX_Texture_Itf* l_pDesignTextures[c_iMaxDesignPieceCount];
      GString* l_pDefaultTextures[c_iMaxDesignPieceCount];
      GColorRGBReal l_DesignColor(0.5, 0.5, 0.5, 1.f);

      Memory::Clear(l_pDesignPieces, c_iMaxDesignPieceCount);
      Memory::Clear(l_pDesignTextures, c_iMaxDesignPieceCount);
      Memory::Clear(l_pDefaultTextures, c_iMaxDesignPieceCount);

      bool l_bLoadDefault = false;
      for(INT32 i = 0;i < c_iMaxDesignPieceCount;i ++)
      {
         if(l_pType->Pieces[i] )
         {
            INT32 l_iVariation = in_DesignVariation.PieceIndex[i];
#ifdef GOLEM_DEBUG
            g_Joshua.Log( GString("Using design Piece ") + GString(i) + GString(" Variation ") + GString(l_iVariation) );
#endif
            vector<GDesignVariationHolder::GType::GPiece::GVariation>& l_vVariations = 
               l_pType->Pieces[i]->Variations;
            if( (l_iVariation >= 0) && (l_iVariation < (INT32) l_vVariations.size() ) )
            {
               l_pDesignPieces[i] = g_Joshua.Renderer()->Helper()->LoadGFXModel(l_vVariations[l_iVariation].Model);

               if(l_pType->Pieces[i]->Colorize)
               {
                  l_pDesignTextures[i] = 
                     g_Joshua.Renderer()->TextureManager()->Create_Texture(
                     l_vVariations[l_iVariation].ColorizeTexture, 
                     false, 
                     l_vVariations[l_iVariation].ColorizeTexture.Name() );
                  l_pDefaultTextures[i] = &l_vVariations[l_iVariation].DefaultTexture;
               }
            }

            if(!l_pDesignPieces[i] )
            {
#ifdef GOLEM_DEBUG
               g_Joshua.Log( GString("Problem with piece") );
#endif
               l_bLoadDefault = true;
            }
         }
      }

      if(!l_pDesignPieces[0] )
      {
         l_bLoadDefault = true;
      }

      if(!l_bLoadDefault)
      {
         l_DesignColor = c_MilitaryColorPalette[in_DesignVariation.ColorIndex];

         // Connect pieces together & apply correct color
         for(INT32 i = 0;i < c_iMaxDesignPieceCount;i ++)
         {
            if(l_pDesignPieces[i] )
            {
               // Change material
               if(l_pDesignTextures[i] )
               {
                  m_pDesignPiecePS[i] = ReplaceTexture(l_pDesignPieces[i], 
                                                       *l_pDefaultTextures[i], 
                                                       l_pDesignTextures[i],
                                                       l_DesignColor);
               }

               // Check if model is parented to another loaded model
               INT32 l_iParentID = l_pType->Pieces[i]->ParentID;
               if(l_iParentID == -1)
               {
                  m_pUnitRootModel->Add_Child(l_pDesignPieces[i] );
               }
               else if( (l_iParentID != i) && (l_pDesignPieces[l_iParentID] ) )
               {
                  // Find the model on which this model is child
                  CGFX_Model_Itf* l_pParentModel = g_Joshua.Renderer()->Helper()->FindModelRecurse(l_pDesignPieces[l_iParentID], l_pType->Pieces[i]->ParentName);
                  if(l_pParentModel)
                  {
                     l_pParentModel->Add_Child(l_pDesignPieces[i] );
                  }
                  else
                  {
                     l_bLoadDefault = true;
                  }
               }
               else
               {
                  l_bLoadDefault = true;
               }
            }
         }
      }

      if(l_bLoadDefault)
      {
         // Make sure an unsuccessful load is cleaned
         m_pUnitRootModel->Clear_Children();

         // Simply load the default model for this unit type
         CGFX_Model_Itf* l_pDefaultModel = g_Joshua.Renderer()->Helper()->LoadGFXModel(l_pType->DefaultModel);
         if(l_pDefaultModel)
         {
            m_pUnitRootModel->Add_Child(l_pDefaultModel);
         }
         else
         {
            l_bLoadGlobalDefault = true;
         }
         SAFE_RELEASE(l_pDefaultModel);
      }

      SAFE_RELEASE(l_pDesignPieces[0] );
      SAFE_RELEASE(l_pDesignPieces[1] );
      SAFE_RELEASE(l_pDesignPieces[2] );
   }
   else
   {
      l_bLoadGlobalDefault = true;
   }

   if(l_bLoadGlobalDefault)
   {
      // Simply load the default design model
      CGFX_Model_Itf* l_pDefaultModel = g_Joshua.Renderer()->Helper()->LoadGFXModel(g_ClientDAL.DesignVariationHolder.DefaultModel() );

      if(!l_pDefaultModel)
      {
#ifdef GOLEM_DEBUG
         g_Joshua.Log(L"Global design model cannot be loaded");
#endif
      }

      m_pUnitRootModel->Add_Child(l_pDefaultModel);
      SAFE_RELEASE(l_pDefaultModel);
   }

   GVector3D<REAL32> l_PrevPosition = m_Camera.Position() - m_Camera.LookPoint();

   // Update the camera to display properly this model
   GBox<REAL32> l_BBox;
   g_Joshua.Renderer()->Helper()->ComputeBoundingBox(m_pScene->Get_Root_Model(), l_BBox);

   m_Camera.SetLimits(l_BBox, 
                      l_BBox.Center(), 
                      GVector3D<REAL32>(0, 0, -1), 
                      GVector3D<REAL32>(1, 0, 0) + l_BBox.Center(),
                      0,
                      (REAL32) (2 * Math::PI),
                      (REAL32) (Math::PI * -0.0),
                      (REAL32) (Math::PI *  0.4),
                      l_BBox.DiagonalLength() * 0.5f,
                      l_BBox.DiagonalLength() * 3.f);

   m_Camera.ChangePosition(l_PrevPosition + m_Camera.LookPoint() );
   if(in_iType == m_iType)
   {
      m_Camera.OffsetDistance(0);
   }
   else
   {
      m_Camera.OffsetDistance(l_BBox.DiagonalLength() * 3.f);
   }

   // Update light & its Transform
   m_pLight->Range() = GVector3D<REAL32>(l_BBox.Left(), l_BBox.Top(), l_BBox.Front() ).Distance(
                       GVector3D<REAL32>(l_BBox.Right(), l_BBox.Bottom(), l_BBox.Back() ) );
   m_pLightTransform->Translation().X = l_BBox.Left();
   m_pLightTransform->Translation().Y = l_BBox.Top();
   m_pLightTransform->Translation().Z = l_BBox.Front();

   m_iType = in_iType;
   m_DesignVariation = in_DesignVariation;
}

void GUnitViewScene::HideUnit()
{
   m_pUnitRootModel->Visibility() = false;
}

void GUnitViewScene::ShowUnit(void)
{
   m_pUnitRootModel->Visibility() = true;
}

GFX::GCascadePixelShader* GUnitViewScene::ReplaceTexture(CGFX_Model_Itf* in_pModel, 
                                                         const GString& in_sOldTxtrName, 
                                                         CGFX_Texture_Itf* in_pNewTxtr,
                                                         GColorRGBReal& in_Color)
{
   GFX::GMaterial* l_pMat = in_pModel->Get_Material();

   if(l_pMat)
   {
      GFX::GCascadePixelShader* l_pPS = (GFX::GCascadePixelShader*) l_pMat->PixelShader(0);

      if( (l_pPS->TextureSampler(0)->Texture() ) && 
          (l_pPS->TextureSampler(0)->Texture()->Name() == in_sOldTxtrName) )
      {
         l_pPS->NbStages(2);

         GFX::GCascadeStage* l_pStage = l_pPS->Stage(0);

         l_pStage->ColorOperation(GFX::OPERATOR_BLEND_TEXTURE_ALPHA);
         l_pStage->ColorArgument1(GFX::ARG_TEXTURE);
         l_pStage->ColorArgument2(GFX::ARG_COLOR_FACTOR);

         l_pStage->AlphaOperation(GFX::OPERATOR_SELECT_ARG_2);
         l_pStage->AlphaArgument2(GFX::ARG_COLOR_FACTOR);

         l_pStage->StageSampler(0);
         l_pStage->TextureCoordIndex(0);
         l_pStage->Destination(GFX::ARG_DEST_CURRENT);

         l_pStage = l_pPS->Stage(1);

         l_pStage->ColorOperation(GFX::OPERATOR_MULTIPLY);
         l_pStage->ColorArgument1(GFX::ARG_DIFFUSE);
         l_pStage->ColorArgument2(GFX::ARG_CURRENT);

         l_pStage->AlphaOperation(GFX::OPERATOR_NONE);

         l_pStage->Destination(GFX::ARG_DEST_CURRENT);

         l_pPS->TextureSampler(0)->Texture(in_pNewTxtr);
         l_pPS->ColorFactor() = in_Color;

         return l_pPS;
      }
   }

   // The texture was not found yet, explore the hierarchy
   for(UINT32 i = 0;i < in_pModel->Get_Nb_Children();i ++)
   {
      GFX::GCascadePixelShader* l_pReturn = 
         ReplaceTexture(in_pModel->Get_Child(i), in_sOldTxtrName, in_pNewTxtr, in_Color);
      if(l_pReturn)
      {
         return l_pReturn;
      }
   }

   return NULL;
}


void GUnitViewScene::DisplayAndAnimate(GFile& in_GolemModelFile)
{
   // Make sure an unsuccessful load is cleaned
   m_pUnitRootModel->Clear_Children();

   // Simply load the default model for this unit type
   CGFX_Model_Itf* l_pModel = g_Joshua.Renderer()->Helper()->LoadModelGolem(in_GolemModelFile);
   if(l_pModel)
   {
      m_pUnitRootModel->Add_Child(l_pModel);
   }
   
   /*
   CGFX_Action_Itf* l_pAnimation = g_Joshua.Renderer()->Helper()->LoadActionGolem(in_GolemModelFile,m_pUnitRootModel,false);
   if(l_pAnimation)
   {
      // Add action to our track
      m_pTrack = g_Joshua.Renderer()->Create_Track();
      m_pTrack->Set_Action(l_pAnimation);
      m_pTrack->Set_Length_Relative(1.f);
      m_pTrack->Playback(GFX::PLAYBACK_FORWARD);
      m_pTrack->Playback_Count(0);
      m_pScene->Get_Animation_System()->Add_Track(m_pTrack);
      m_pScene->Get_Animation_System()->Play_Animation();
   }
   SAFE_RELEASE(l_pAnimation);
   */

   SAFE_RELEASE(l_pModel);

   // Update the camera to display properly this model
   GBox<REAL32> l_BBox;
   g_Joshua.Renderer()->Helper()->ComputeBoundingBox(m_pScene->Get_Root_Model(), l_BBox);

   m_Camera.SetLimits(l_BBox, 
                      l_BBox.Center(), 
                      GVector3D<REAL32>(0, 0, -1), 
                      GVector3D<REAL32>(1, 0, 0),
                      0,
                      (REAL32) (2 * Math::PI),
                      (REAL32) (Math::PI * -0.25),
                      (REAL32) (Math::PI *  0.25),
                      l_BBox.DiagonalLength() * 2,
                      l_BBox.DiagonalLength() * 4);

   m_Camera.ChangePosition(GVector3D<REAL32>(1, 0, 0) );
   m_Camera.OffsetOrientation( (REAL32) Math::PI * 0.5f, (REAL32) Math::PI * 0.25f);
}
