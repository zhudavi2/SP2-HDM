/**************************************************************
*
* sp2_atlas_earth.cpp
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

#include "../include/sp2_atlas_earth.h"

#include "../include/sp2_background_earth.h"

const REAL32 GAtlasEarth::c_fAtlasGenerationSizeOffset = 0.1f;

const GVector2D<REAL32> GAtlasEarth::c_pScreenCorners[c_iNbAtlasLevelCorners] = 
{
   GVector2D<REAL32>(-1, -1),
   GVector2D<REAL32>( 0, -1),
   GVector2D<REAL32>( 1, -1),
   GVector2D<REAL32>(-1,  1),
   GVector2D<REAL32>( 0,  1),
   GVector2D<REAL32>( 1,  1),
};

const bool GAtlasEarth::c_pCornerLimitsHorizontal[c_iNbAtlasLevelCorners] =
{
   true,
   false,
   true,
   true,
   false,
   true,
};

const REAL32 GAtlasEarth::c_pLevelDistanceTable[c_iNbAtlasLevels] = 
{
   0.2f,
   0.125f,
   0.075f,
};

const UINT32 GAtlasEarth::c_pLevelContentTable[c_iNbAtlasLevels] = 
{
   c_AtlasElevation,
   c_AtlasElevation | c_AtlasWater,
   c_AtlasElevation | c_AtlasDetails,
};

GAtlasEarth::GAtlasEarth(class GBackgroundEarth* in_pBGEarth,
                         UINT32 in_iNbHDivs, 
                         UINT32 in_iNbVDivs, 
                         REAL32 in_fFadeTime)
{
   for(UINT32 i=0; i<c_iNbAtlasLevels; i++)
      m_pAtlasLevelContent[i] = c_pLevelContentTable[i];

   m_iNextGenerationID = 0;

   for(UINT32 i = 0;i < ELevelFGroup::Count;i ++)
   {
      m_pCurrentFGroups[i] = INVALID_INDEX;
   }

   m_pBGEarth = in_pBGEarth;
   m_iNbHDivs = in_iNbHDivs;
   m_iNbVDivs = in_iNbVDivs;

   // Create gfx objects
   {
      m_pCamera = g_Joshua.Renderer()->Create_Camera();
      m_pCamTfx = g_Joshua.Renderer()->Create_Transform_SRT();

      m_pMaster = g_Joshua.Renderer()->Create_Model();
      m_pEarth = g_Joshua.Renderer()->CreateMesh();

      m_pMat = g_Joshua.Renderer()->Helper()->
                  CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);
   }

   // Organise hierarchy
   {
      m_pMaster->Set_Primitive(m_pEarth);
      m_pMaster->Set_Material(m_pMat);
   }

   // Initialize atlas material
   {
      GFX::GRenderStateCollection* l_pRS = m_pMat->RenderStates(0);
      GFX::GCascadePixelShader* l_pPS = (GFX::GCascadePixelShader*) m_pMat->PixelShader(0);

      l_pRS->Value(GFX::RSTATE_ZBUFFER_WRITE_ENABLE) = false;
      l_pRS->Value(GFX::RSTATE_ZBUFFER_ENABLE) = false;
      l_pRS->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_pRS->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_DESTCOLOR;
      l_pRS->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_SRCALPHA;

      l_pPS->Stage(0)->ColorOperation(GFX::OPERATOR_MULTIPLY);
      l_pPS->Stage(0)->ColorArgument1(GFX::ARG_TEXTURE);
      l_pPS->Stage(0)->ColorArgument2(GFX::ARG_COLOR_FACTOR);
      l_pPS->Stage(0)->AlphaOperation(GFX::OPERATOR_MULTIPLY);
      l_pPS->Stage(0)->AlphaArgument1(GFX::ARG_TEXTURE);
      l_pPS->Stage(0)->AlphaArgument2(GFX::ARG_COLOR_FACTOR);
      l_pPS->ColorFactor().R = 1.f;
      l_pPS->ColorFactor().G = 1.f;
      l_pPS->ColorFactor().B = 1.f;
      l_pPS->ColorFactor().A = 1.f;
      m_pTxtr = g_Joshua.Renderer()->TextureManager()->CreateTexture(PIXELFMT_A8R8G8B8, 1, 1, false, L"DummyAtlas");
      GImage2D* l_pImage = GImage2D::AllocObject();
      m_pTxtr->LockImage(l_pImage);
      GPixelA8R8G8B8 l_Pix;
      l_Pix.A = 255;
      l_Pix.R = 0;
      l_Pix.G = 0;
      l_Pix.B = 0;
      l_pImage->Clear(&l_Pix);
      m_pTxtr->Unlock();
      GImage2D::FreeObject(l_pImage);
      l_pPS->TextureSampler(0)->Texture(m_pTxtr);
      l_pPS->TextureSampler(0)->MinificationFilter(GFX::FILTER_LINEAR);
      l_pPS->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
      l_pPS->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
      l_pPS->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);
   }

   // Initialize meshes
   {
      // Create a temporary mesh with a complete sphere
      g_Joshua.Renderer()->Helper()->CreateSphereSurfacePart(
         m_pEarth, 1, 
         (REAL32) -Math::PI * 2, 0, 
         (REAL32) -Math::PI * 0.5f, (REAL32) Math::PI * 0.5f, 
         m_iNbHDivs, m_iNbVDivs, HELPER_MESH_CREATE_ALL);
      GTriangleListIndexed& l_SphereTris = m_pEarth->FacetGroup(0).Facets().TriangleListIndexed();

      GShape& l_EarthShape = m_pEarth->Shape();

      // Add a quarter to the sphere (this will allow to display textures that are
      // crossing the -180/180 longitude line)
      m_iHTriPitch = m_iNbHDivs * 5 / 4;
      m_iHTriPitch *= 2;

      m_EarthTris.NbTriangles(m_iHTriPitch * m_iNbVDivs);

      UINT32 l_iNbPoints = 3 * ( (m_iHTriPitch - m_iNbHDivs * 2) * m_iNbVDivs);
      UINT32 l_iShapeIndex = l_EarthShape.NbPositions();
      l_iNbPoints += l_iShapeIndex;
      l_EarthShape.NbPositions(l_iNbPoints);
      l_EarthShape.NbNormals(l_iNbPoints);
      l_EarthShape.NbUVCoords(0, l_iNbPoints);

      UINT32 l_iTriIndex = 0;
      for(UINT32 y = m_iNbVDivs - 1;y != 0xFFFFFFFF;y --)
      {
         for(UINT32 x = 0;x < m_iNbHDivs;x ++)
         {
            m_EarthTris.Triangle(l_iTriIndex) = l_SphereTris.Triangle( (y * m_iNbHDivs + x) * 2);
            m_EarthTris.Triangle(l_iTriIndex + 1) = l_SphereTris.Triangle( (y * m_iNbHDivs + x) * 2 + 1);

            l_iTriIndex += 2;
         }

         for(UINT32 x = 0;x < m_iHTriPitch / 2 - m_iNbHDivs;x ++)
         {
            GTriangleIndex l_NonOverlappedTri = l_SphereTris.Triangle( (y * m_iNbHDivs + x) * 2);
            GTriangleIndex l_OverlappedTri = l_NonOverlappedTri;

            for(UINT32 t = 0;t < 3;t ++, l_iShapeIndex ++)
            {
               l_OverlappedTri.m_pVertices[t] = l_iShapeIndex;
               l_EarthShape.Position(l_iShapeIndex) = l_EarthShape.Position(l_NonOverlappedTri.m_pVertices[t] );
               l_EarthShape.Normal(l_iShapeIndex) = l_EarthShape.Normal(l_NonOverlappedTri.m_pVertices[t] );
               l_EarthShape.UVCoord(0, l_iShapeIndex) = l_EarthShape.UVCoord(0, l_NonOverlappedTri.m_pVertices[t] );
               l_EarthShape.UVCoord(0, l_iShapeIndex).x += 1;
            }

            m_EarthTris.Triangle(l_iTriIndex) = l_OverlappedTri;

            l_NonOverlappedTri = l_SphereTris.Triangle( (y * m_iNbHDivs + x) * 2 + 1);

            for(UINT32 t = 0;t < 3;t ++, l_iShapeIndex ++)
            {
               l_OverlappedTri.m_pVertices[t] = l_iShapeIndex;
               l_EarthShape.Position(l_iShapeIndex) = l_EarthShape.Position(l_NonOverlappedTri.m_pVertices[t] );
               l_EarthShape.Normal(l_iShapeIndex) = l_EarthShape.Normal(l_NonOverlappedTri.m_pVertices[t] );
               l_EarthShape.UVCoord(0, l_iShapeIndex) = l_EarthShape.UVCoord(0, l_NonOverlappedTri.m_pVertices[t] );
               l_EarthShape.UVCoord(0, l_iShapeIndex).x += 1;
            }

            m_EarthTris.Triangle(l_iTriIndex + 1) = l_OverlappedTri;

            l_iTriIndex += 2;
         }
      }
      m_pEarth->NbFacetGroups(0);
//#      m_pRegions->Shape() = l_EarthShape;
   }

   m_GenThread.Run(this);
}

GAtlasEarth::~GAtlasEarth()
{
   m_TxtrGen.DisableGeneration();
   m_GenThread.Terminate(true);

   //#for(list<GFadeAction>::iterator i = m_vFadeActions.begin();
   //    i != m_vFadeActions.end();++ i)
   //{
   //   if(i->m_bActive)
   //   {
   //      if (m_pBGEarth->m_pScene)
   //      {
   //         UINT32 l_iID = m_pBGEarth->m_pScene->Get_Animation_System()->Get_Action_Index(i->m_pAction);
   //         m_pBGEarth->m_pScene->Get_Animation_System()->Remove_Action_At(l_iID);
   //      }
   //   }
   //   SAFE_RELEASE(i->m_pAction);
   //}

   SAFE_RELEASE(m_pCamera);
   SAFE_RELEASE(m_pCamTfx);

   SAFE_RELEASE(m_pMaster);
   SAFE_RELEASE(m_pEarth);
   SAFE_RELEASE(m_pMat);
   SAFE_RELEASE(m_pTxtr);
}

CGFX_Model_Itf* GAtlasEarth::Master()
{
   return m_pMaster;
}

void GAtlasEarth::UpdateCamera()
{

   GVector2D<REAL32> l_CamPosition(g_ClientDCL.m_EarthState.m_Longitude, 
                                   g_ClientDCL.m_EarthState.m_Latitude);
   REAL32 l_fCameraDistance = m_pBGEarth->m_CameraPos.Length() - 1.f;
   gassert(l_fCameraDistance >= 0.f, "Camera should not be \"behind\" earth...");

   // Hide any generated atlas level that is not completely seen by the camera
   bool l_bClipped = false;
   while(!m_vLevels.empty() )
   {
      GAtlasLevel& l_Level = m_vLevels.back();

      // Level must be hidden if lookpoint of camera is outside level area
      l_bClipped = !l_Level.m_Rect.Contains(l_CamPosition * 180.f / (REAL32) Math::PI);

      // Level must be hidden if one of its corner is "visible" on earth
      for(UINT32 i = 0;(i < c_iNbAtlasLevelCorners) && !l_bClipped;i ++)
      {
         GVector2D<INT32> l_ScreenPos;

         m_pBGEarth->Project3DPointIn2D(l_Level.m_Corners[i], l_ScreenPos);
         if( ( (l_ScreenPos.x > 0) && (l_ScreenPos.x < m_pBGEarth->m_Mode.m_Resolution.x - 1) && c_pCornerLimitsHorizontal[i] ) ||
             ( (l_ScreenPos.y > 0) && (l_ScreenPos.y < m_pBGEarth->m_Mode.m_Resolution.y - 1) ) )
         {
            l_bClipped = true;
         }
      }

      // Destroy level content if flaged as such
      if(l_bClipped)
      {
         DestroyTopAtlasLevel();
      }
      else
      {
         // No need to check other levels, we have necessary generated one inside the other
         break;
      }
   }

   if( m_pAtlasLevelContent[0] )
   {
      // Compute which level must be shown
      UINT32 l_iLevelToShow = 0;
      while( (c_pLevelDistanceTable[l_iLevelToShow] > l_fCameraDistance) && 
            (l_iLevelToShow < c_iNbAtlasLevels) )
      {
         l_iLevelToShow ++;
      }

      // Generate needed levels 
      while(m_vLevels.size() < l_iLevelToShow)
      {
         CreateTopAtlasLevel(l_CamPosition);
      }
   }
}

void GAtlasEarth::CreateTopAtlasLevel(const GVector2D<REAL32>& in_Center)
{
   UINT32 l_iNextLevel = m_vLevels.size();
   if(l_iNextLevel >= c_iNbAtlasLevels)
   {
      gassert(0, "Atlas level generation request algo asked for top level generation when it exists");
      return;
   }

   gassert(m_pAtlasLevelContent[l_iNextLevel], "Atlas level has no content");
   const GAtlasLevel& l_PrevLevel = m_vLevels.back();

   GAtlasLevel l_NewLevel;
   REAL32 l_fEarthDistance = c_pLevelDistanceTable[l_iNextLevel];

   // Create a camera which would view the earth at the level distance
   m_pBGEarth->UpdateCameraParameters(in_Center, l_fEarthDistance, m_pCamera, m_pCamTfx);

   m_pCamTfx->Update();

   GMatrix4x4<REAL32> l_ViewMat = m_pCamera->Get_View_Matrix(m_pCamTfx->Get_Matrix() );
   GMatrix4x4<REAL32> l_ViewMatInv = l_ViewMat.Inverse();
   GMatrix4x4<REAL32> l_ProjMat = m_pCamera->Get_Projection_Matrix();

   GVector2D<REAL32> l_CenterDeg(in_Center * (180.f / (REAL32) Math::PI) );
   // Compute the earth location of 6 screen coordinates
   for(UINT32 i = 0;i < c_iNbAtlasLevelCorners;i ++)
   {
      // Compute the vector of the pick ray in screen space
      GVector3D<REAL32> l_ScreenPos;
      l_ScreenPos.x = c_pScreenCorners[i].x / l_ProjMat(0, 0);
      l_ScreenPos.y = c_pScreenCorners[i].y / l_ProjMat(1, 1);
      l_ScreenPos.z = 1.f;

      // Transform the screen space pick ray into 3D space
      GRay<REAL32> l_Ray;
      l_Ray.m_Direction.x  = l_ScreenPos.x * l_ViewMatInv(0, 0) + l_ScreenPos.y * l_ViewMatInv(1, 0) + l_ScreenPos.z * l_ViewMatInv(2, 0);
      l_Ray.m_Direction.y  = l_ScreenPos.x * l_ViewMatInv(0, 1) + l_ScreenPos.y * l_ViewMatInv(1, 1) + l_ScreenPos.z * l_ViewMatInv(2, 1);
      l_Ray.m_Direction.z  = l_ScreenPos.x * l_ViewMatInv(0, 2) + l_ScreenPos.y * l_ViewMatInv(1, 2) + l_ScreenPos.z * l_ViewMatInv(2, 2);

      l_Ray.m_Center.x = l_ViewMatInv(3, 0);
      l_Ray.m_Center.y = l_ViewMatInv(3, 1);
      l_Ray.m_Center.z = l_ViewMatInv(3, 2);

#ifdef GOLEM_DEBUG
      bool l_bIntersects = 
#endif
      l_Ray.IntersectSphere(1, l_NewLevel.m_Corners[i] );
#ifdef GOLEM_DEBUG
      gassert(l_bIntersects, "Space is visible, but we want to display atlas...");
#endif

      // Convert 3D cartesian coordinate to spherical coordinate (latitude/longitude)
      l_NewLevel.m_Corners[i] = m_pBGEarth->ConvertRectangularToSpherical(l_NewLevel.m_Corners[i] );
      if(fabs(l_NewLevel.m_Corners[i].x - l_CenterDeg.x) > 180.f)
      {
         if(l_NewLevel.m_Corners[i].x > 0)
         {
            l_NewLevel.m_Corners[i].x -= 360.f;
         }
         else
         {
            l_NewLevel.m_Corners[i].x += 360.f;
         }
      }
   }

   l_NewLevel.m_Rect.Set(l_NewLevel.m_Corners[0].x, l_NewLevel.m_Corners[0].y, 0, 0);

   for(UINT32 i = 1;i < c_iNbAtlasLevelCorners;i ++)
   {
      l_NewLevel.m_Rect.Union(GRectangle<REAL32>(l_NewLevel.m_Corners[i].x, l_NewLevel.m_Corners[i].y, 0, 0) );
   }

   // Offset rect by a certain amount (min 5%) in all directions 
   // (so moving the camera slightly won't reask a generation)
   REAL32 l_fWidth = l_NewLevel.m_Rect.m_Width;
   REAL32 l_fHeight = l_NewLevel.m_Rect.m_Height;

   l_NewLevel.m_Rect.m_Left -= l_fWidth * c_fAtlasGenerationSizeOffset;
   l_NewLevel.m_Rect.m_Top -= l_fHeight * c_fAtlasGenerationSizeOffset;

   l_NewLevel.m_Rect.m_Width += 2 * l_fWidth * c_fAtlasGenerationSizeOffset;
   l_NewLevel.m_Rect.m_Height += 2 * l_fHeight * c_fAtlasGenerationSizeOffset;

   // Ask for atlas level generation
   {
      gassert(l_NewLevel.m_Rect.m_Width < 90.f, "Atlas width is much too big");

      gassert(!l_NewLevel.m_Rect.Contains( GVector2D<REAL32>(-90.f, l_CenterDeg.x) ), "Atlas level should not contain south pole");
      gassert(!l_NewLevel.m_Rect.Contains( GVector2D<REAL32>(90.f, l_CenterDeg.x) ), "Atlas level should not contain north pole");

      // Queue the generation request 
      m_Sync.Lock();
      m_vGenParams.resize(m_vGenParams.size() + 1);
      l_NewLevel.m_iGenerationID = m_iNextGenerationID ++;
      m_vGenParams.back().m_iGenerationID = l_NewLevel.m_iGenerationID;
      m_vGenParams.back().m_iContent = m_pAtlasLevelContent[l_iNextLevel];
      m_vGenParams.back().m_Center = l_NewLevel.m_Rect.Center();
      m_vGenParams.back().m_iLevel = l_iNextLevel;
      m_vGenParams.back().m_Size.x = l_NewLevel.m_Rect.m_Width;
      m_vGenParams.back().m_Size.y = l_NewLevel.m_Rect.m_Height;
      m_Sync.Unlock();
      m_Queue.Increment();

      // Compute content of facet group that will display this atlas level
      GTriangleListIndexed l_AtlasTris;

      if(l_NewLevel.m_Rect.m_Left < -180.f)
      {
         l_NewLevel.m_Rect.m_Left += 360.f;
      }

      // Determine what sphere triangles to use
      UINT32 l_iYStart = (UINT32) floorf( ( (-l_NewLevel.m_Rect.m_Top - l_NewLevel.m_Rect.m_Height + 90.f) * m_iNbVDivs ) / 180.f);
      UINT32 l_iYEnd = (UINT32) ceilf( ( (-l_NewLevel.m_Rect.m_Top + 90.f) * m_iNbVDivs ) / 180.f);

      gassert(l_iYStart != l_iYEnd, "Invalid atlas subdiv size");

      UINT32 l_iXStart = (UINT32) floorf( ( (l_NewLevel.m_Rect.m_Left + 180.f) * m_iNbHDivs ) / 360.f);
      UINT32 l_iXEnd = (UINT32) ceilf( ( (l_NewLevel.m_Rect.m_Left + l_NewLevel.m_Rect.m_Width + 180.f) * m_iNbHDivs ) / 360.f);

      gassert(l_iXStart != l_iXEnd, "Invalid atlas subdiv size");

      // Copy sphere triangle to current atlas facet
      l_AtlasTris.NbTriangles( (l_iYEnd - l_iYStart) * (l_iXEnd - l_iXStart) * 2);
      UINT32 l_iTriIndex = 0;
      for(UINT32 y = l_iYStart;y < l_iYEnd;y ++)
      {
         for(UINT32 x = l_iXStart;x < l_iXEnd;x ++)
         {
            l_AtlasTris.Triangle(l_iTriIndex) = m_EarthTris.Triangle(y * m_iHTriPitch + x * 2);
            l_AtlasTris.Triangle(l_iTriIndex + 1) = m_EarthTris.Triangle(y * m_iHTriPitch + x * 2 + 1);

            l_iTriIndex += 2;
         }
      }

      // Initialize atlas texture transform
      GVector2D<REAL32> l_Start;
      l_Start.x = (l_NewLevel.m_Rect.m_Left + 180.f) / 360.f;
      l_Start.y = (-l_NewLevel.m_Rect.m_Top - l_NewLevel.m_Rect.m_Height + 90.f) / 180.f;

      GVector2D<REAL32> l_End;
      l_End.x = (l_NewLevel.m_Rect.m_Left + l_NewLevel.m_Rect.m_Width + 180.f) / 360.f;
      l_End.y = (-l_NewLevel.m_Rect.m_Top + 90.f) / 180.f;

      GVector2D<REAL32> l_Position(l_Start.x / (l_Start.x - l_End.x),
                                   l_Start.y / (l_Start.y - l_End.y) );
      GVector2D<REAL32> l_Size(-1.f / (l_Start.x - l_End.x),
                               -1.f / (l_Start.y - l_End.y) );

      if(m_pAtlasLevelContent[l_iNextLevel] & c_AtlasElevation)
      {
         // Select (or create) a facet group for the atlas elevation level
         l_NewLevel.m_pFGroups[ELevelFGroup::Elev] = 
            CreateAtlasFacet(0, l_AtlasTris, l_Position, l_Size);
      }
      else
      {
         l_NewLevel.m_pFGroups[ELevelFGroup::Elev] = INVALID_INDEX;
      }

      if(m_pAtlasLevelContent[l_iNextLevel] & c_AtlasDetails)
      {
         UINT32 l_iStartFacet = 0;
         if(l_iNextLevel > 0)
         {
            l_iStartFacet = l_PrevLevel.m_pFGroups[ELevelFGroup::Detail] != INVALID_INDEX ? l_PrevLevel.m_pFGroups[ELevelFGroup::Detail] : 0;
         }

         // Select (or create) a facet group for the atlas detail level
         l_NewLevel.m_pFGroups[ELevelFGroup::Detail] = 
            CreateAtlasFacet(l_iStartFacet, l_AtlasTris, l_Position, l_Size);
      }
      else
      {
         l_NewLevel.m_pFGroups[ELevelFGroup::Detail] = INVALID_INDEX;
      }
   }

   // Re-compute all level corners in earth space
   for(UINT32 i = 0;i < c_iNbAtlasLevelCorners;i ++)
   {
      GVector2D<REAL32> l_Coord;
      l_Coord.x = 0.5f * c_pScreenCorners[i].x * l_NewLevel.m_Rect.m_Width + l_NewLevel.m_Rect.m_Left + 0.5f * l_NewLevel.m_Rect.m_Width;
      l_Coord.y = 0.5f * c_pScreenCorners[i].y * l_NewLevel.m_Rect.m_Height + l_NewLevel.m_Rect.m_Top + 0.5f * l_NewLevel.m_Rect.m_Height;
      l_NewLevel.m_Corners[i] = m_pBGEarth->ConvertSphericalToRectangular(l_Coord);
   }

   m_vLevels.push_back(l_NewLevel);

   gassert(m_vLevels.size() == l_iNextLevel + 1, "Atlas content inconsistent");
}

UINT32 GAtlasEarth::CreateAtlasFacet(UINT32 in_iStartFacetId,
                                     const GFacetList& in_FacetContent,
                                     const GVector2D<REAL32>& in_Position,
                                     const GVector2D<REAL32>& in_Size)
{
   // Find the first non-active facet group on the mesh
   UINT32 l_iFacetID = in_iStartFacetId;
   while(l_iFacetID < m_pEarth->NbFacetGroups() )
   {
      if(!m_vActiveFacets[l_iFacetID] )
      {
         break;
      }
      l_iFacetID ++;
   }

   // If no facet list is available, create a new one
   CGFX_Transform_2D_Itf* l_pAtlasTrans = NULL;
   if(l_iFacetID >= m_pEarth->NbFacetGroups() )
   {
      m_vActiveFacets.push_back(false);
      m_pEarth->NbFacetGroups(l_iFacetID + 1);

      // Create a texture transform override for atlas
      GFX::GOverridePSCascade* l_pPSOverride = g_Joshua.Renderer()->CreateOverridePSCascade();
      l_pAtlasTrans = g_Joshua.Renderer()->Create_Transform_2D();
      l_pPSOverride->AddUVTransformOverride(0, l_pAtlasTrans);
      l_pPSOverride->UseColorFactorOverride(true);
      l_pPSOverride->ColorFactorOverride().R = 1.f;
      l_pPSOverride->ColorFactorOverride().G = 1.f;
      l_pPSOverride->ColorFactorOverride().B = 1.f;
      l_pPSOverride->ColorFactorOverride().A = 1.f;

      // Create a texture shader override for atlas
      GFX::GOverrideTSampler* l_pTSOverride = g_Joshua.Renderer()->CreateOverrideTSampler();
      l_pTSOverride->OriginalTextureSamplerID(0);
      l_pTSOverride->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
      l_pTSOverride->OverrideTextureSampler()->Texture(m_pTxtr);

      // Set those override on the atlas facet list
      m_pEarth->FacetGroup(l_iFacetID).AddOverride(l_pTSOverride);
      m_pEarth->FacetGroup(l_iFacetID).AddOverride(l_pPSOverride);

      l_pAtlasTrans->Release();
      l_pPSOverride->Release();
      l_pTSOverride->Release();
   }
   else
   {
      gassert(m_pEarth->FacetGroup(l_iFacetID).Override(1)->Type() == GFX::TYPE_OVERRIDE_PS_CASCADE, "Invalid atlas facet override");
      l_pAtlasTrans = (CGFX_Transform_2D_Itf*) ( (GFX::GOverridePSCascade*) m_pEarth->FacetGroup(l_iFacetID).Override(1) )->UVTransformOverride(0);
      gassert(l_pAtlasTrans, "Invalid atlas facet override transform");

      gassert(m_pEarth->FacetGroup(l_iFacetID).Override(0)->Type() == GFX::TYPE_OVERRIDE_TSAMPLER, "Invalid atlas facet override");
      GFX::GOverrideTSampler* l_pOverride = 
         (GFX::GOverrideTSampler*) m_pEarth->FacetGroup(l_iFacetID).Override(0);
      l_pOverride->OverrideTextureSampler()->Texture(m_pTxtr);
   }

   // Update facet content
   m_pEarth->FacetGroup(l_iFacetID).Facets() = in_FacetContent;
   m_pEarth->FacetGroup(l_iFacetID).Visibility() = 1.f;

   gassert(m_pEarth->NbFacetGroups() == m_vActiveFacets.size(), "Facet activity & atlas mesh does not match");
   m_vActiveFacets[l_iFacetID] = true;

   // Update facet texture transform
   l_pAtlasTrans->Position() = in_Position;
   l_pAtlasTrans->Size() = in_Size;

   return l_iFacetID;
}

//GAtlasEarth::GAtlasEarth(class GBackgroundEarth* in_pBGEarth, 
//                         UINT32 in_iNbHDivs, 
//                         UINT32 in_iNbVDivs, 
//                         REAL32 in_fHeight,
//                         REAL32 in_fHeightDetail,
//                         REAL32 in_fFadeTime)
//{
//   m_pBGEarth = in_pBGEarth;
//
//   m_iNbHDivs = in_iNbHDivs;
//   m_iNbVDivs = in_iNbVDivs;
//   m_fHeight = in_fHeight;
//   m_fHeightDetail = in_fHeightDetail;
//
//   // Create gfx objects
//   {
//      m_pMaster = g_Joshua.Renderer()->Create_Model();
//      m_pEarth = g_Joshua.Renderer()->CreateMesh();
//
//      m_pMat = g_Joshua.Renderer()->Helper()->
//                  CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);
//
//      m_pRegionMaster = g_Joshua.Renderer()->Create_Model();
//      m_pRegions = g_Joshua.Renderer()->CreateMesh();
//      m_pRegionMat = g_Joshua.Renderer()->Helper()->
//                  CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);
//   }
//
//   // Organise hierarchy
//   {
//      m_pMaster->Set_Primitive(m_pEarth);
//      m_pMaster->Set_Material(m_pMat);
//
//      m_pMaster->Add_Child(m_pRegionMaster);
//
//      m_pRegionMaster->Set_Primitive(m_pRegions);
//      m_pRegionMaster->Set_Material(m_pRegionMat);
//   }
//
//   // Initialize atlas material
//   {
//      GFX::GRenderStateCollection* l_pRS = m_pMat->RenderStates(0);
//      GFX::GCascadePixelShader* l_pPS = (GFX::GCascadePixelShader*) m_pMat->PixelShader(0);
//
//      l_pRS->Value(GFX::RSTATE_ZBUFFER_WRITE_ENABLE) = false;
//      l_pRS->Value(GFX::RSTATE_ZBUFFER_ENABLE) = false;
//      l_pRS->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
//      l_pRS->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_DESTCOLOR;
//      l_pRS->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_SRCALPHA;
//
//      l_pPS->Stage(0)->ColorOperation(GFX::OPERATOR_MULTIPLY);
//      l_pPS->Stage(0)->ColorArgument1(GFX::ARG_TEXTURE);
//      l_pPS->Stage(0)->ColorArgument2(GFX::ARG_COLOR_FACTOR);
//      l_pPS->Stage(0)->AlphaOperation(GFX::OPERATOR_MULTIPLY);
//      l_pPS->Stage(0)->AlphaArgument1(GFX::ARG_TEXTURE);
//      l_pPS->Stage(0)->AlphaArgument2(GFX::ARG_COLOR_FACTOR);
//      l_pPS->ColorFactor().R = 1.f;
//      l_pPS->ColorFactor().G = 1.f;
//      l_pPS->ColorFactor().B = 1.f;
//      l_pPS->ColorFactor().A = 1.f;
//      m_pTxtr = g_Joshua.Renderer()->TextureManager()->CreateTexture(PIXELFMT_A8R8G8B8, 1, 1, false, L"DummyAtlas");
//      GImage2D* l_pImage = GImage2D::AllocObject();
//      m_pTxtr->LockImage(l_pImage);
//      GPixelA8R8G8B8 l_Pix;
//      l_Pix.A = 128;
//      l_Pix.R = 128;
//      l_Pix.G = 128;
//      l_Pix.B = 128;
//      l_pImage->Clear(&l_Pix);
//      m_pTxtr->Unlock();
//      GImage2D::FreeObject(l_pImage);
//      l_pPS->TextureSampler(0)->Texture(m_pTxtr);
//      l_pPS->TextureSampler(0)->MinificationFilter(GFX::FILTER_LINEAR);
//      l_pPS->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
//      l_pPS->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
//      l_pPS->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);
//   }
//
//   // Initialize region material
//   {
//      GFX::GRenderStateCollection* l_pRS = m_pRegionMat->RenderStates(0);
//      GFX::GCascadePixelShader* l_pPS = (GFX::GCascadePixelShader*) m_pRegionMat->PixelShader(0);
//
//      l_pRS->Value(GFX::RSTATE_ZBUFFER_WRITE_ENABLE) = false;
//      l_pRS->Value(GFX::RSTATE_ZBUFFER_ENABLE) = false;
//      l_pRS->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
//      l_pRS->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
//      l_pRS->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_ONE;
//
//      l_pPS->Stage(0)->ColorOperation(GFX::OPERATOR_SELECT_ARG_2);
//      l_pPS->Stage(0)->ColorArgument2(GFX::ARG_COLOR_FACTOR);
//      l_pPS->Stage(0)->AlphaOperation(GFX::OPERATOR_MULTIPLY);
//      l_pPS->Stage(0)->AlphaArgument1(GFX::ARG_TEXTURE);
//      l_pPS->Stage(0)->AlphaArgument2(GFX::ARG_COLOR_FACTOR);
//      l_pPS->ColorFactor().R = 1.f;
//      l_pPS->ColorFactor().G = 1.f;
//      l_pPS->ColorFactor().B = 1.f;
//      l_pPS->ColorFactor().A = 0.5f;
//
//      l_pPS->TextureSampler(0)->Texture(m_pTxtr);
//      l_pPS->TextureSampler(0)->MinificationFilter(GFX::FILTER_LINEAR);
//      l_pPS->TextureSampler(0)->MagnificationFilter(GFX::FILTER_LINEAR);
//      l_pPS->TextureSampler(0)->AddressingModeU(GFX::ADDRESS_CLAMP);
//      l_pPS->TextureSampler(0)->AddressingModeV(GFX::ADDRESS_CLAMP);
//   }
//
//   // Initialize meshes
//   {
//      // Create a temporary mesh with a complete sphere
//      g_Joshua.Renderer()->Helper()->CreateSphereSurfacePart(
//         m_pEarth, 1, 
//         (REAL32) -Math::PI * 2, 0, 
//         (REAL32) -Math::PI * 0.5f, (REAL32) Math::PI * 0.5f, 
//         m_iNbHDivs, m_iNbVDivs, HELPER_MESH_CREATE_ALL);
//      GTriangleListIndexed& l_SphereTris = m_pEarth->FacetGroup(0).Facets().TriangleListIndexed();
//
//      GShape& l_EarthShape = m_pEarth->Shape();
//
//      // Add a quarter to the sphere (this will allow to display textures that are
//      // crossing the -180/180 longitude line)
//      m_iHTriPitch = m_iNbHDivs * 5 / 4;
//      m_iHTriPitch *= 2;
//
//      m_EarthTris.NbTriangles(m_iHTriPitch * m_iNbVDivs);
//
//      UINT32 l_iNbPoints = 3 * ( (m_iHTriPitch - m_iNbHDivs * 2) * m_iNbVDivs);
//      UINT32 l_iShapeIndex = l_EarthShape.NbPositions();
//      l_iNbPoints += l_iShapeIndex;
//      l_EarthShape.NbPositions(l_iNbPoints);
//      l_EarthShape.NbNormals(l_iNbPoints);
//      l_EarthShape.NbUVCoords(0, l_iNbPoints);
//
//      UINT32 l_iTriIndex = 0;
//      for(UINT32 y = m_iNbVDivs - 1;y != 0xFFFFFFFF;y --)
//      {
//         for(UINT32 x = 0;x < m_iNbHDivs;x ++)
//         {
//            m_EarthTris.Triangle(l_iTriIndex) = l_SphereTris.Triangle( (y * m_iNbHDivs + x) * 2);
//            m_EarthTris.Triangle(l_iTriIndex + 1) = l_SphereTris.Triangle( (y * m_iNbHDivs + x) * 2 + 1);
//
//            l_iTriIndex += 2;
//         }
//
//         for(UINT32 x = 0;x < m_iHTriPitch / 2 - m_iNbHDivs;x ++)
//         {
//            GTriangleIndex l_NonOverlappedTri = l_SphereTris.Triangle( (y * m_iNbHDivs + x) * 2);
//            GTriangleIndex l_OverlappedTri = l_NonOverlappedTri;
//
//            for(UINT32 t = 0;t < 3;t ++, l_iShapeIndex ++)
//            {
//               l_OverlappedTri.m_pVertices[t] = l_iShapeIndex;
//               l_EarthShape.Position(l_iShapeIndex) = l_EarthShape.Position(l_NonOverlappedTri.m_pVertices[t] );
//               l_EarthShape.Normal(l_iShapeIndex) = l_EarthShape.Normal(l_NonOverlappedTri.m_pVertices[t] );
//               l_EarthShape.UVCoord(0, l_iShapeIndex) = l_EarthShape.UVCoord(0, l_NonOverlappedTri.m_pVertices[t] );
//               l_EarthShape.UVCoord(0, l_iShapeIndex).x += 1;
//            }
//
//            m_EarthTris.Triangle(l_iTriIndex) = l_OverlappedTri;
//
//            l_NonOverlappedTri = l_SphereTris.Triangle( (y * m_iNbHDivs + x) * 2 + 1);
//
//            for(UINT32 t = 0;t < 3;t ++, l_iShapeIndex ++)
//            {
//               l_OverlappedTri.m_pVertices[t] = l_iShapeIndex;
//               l_EarthShape.Position(l_iShapeIndex) = l_EarthShape.Position(l_NonOverlappedTri.m_pVertices[t] );
//               l_EarthShape.Normal(l_iShapeIndex) = l_EarthShape.Normal(l_NonOverlappedTri.m_pVertices[t] );
//               l_EarthShape.UVCoord(0, l_iShapeIndex) = l_EarthShape.UVCoord(0, l_NonOverlappedTri.m_pVertices[t] );
//               l_EarthShape.UVCoord(0, l_iShapeIndex).x += 1;
//            }
//
//            m_EarthTris.Triangle(l_iTriIndex + 1) = l_OverlappedTri;
//
//            l_iTriIndex += 2;
//         }
//      }
//      m_pEarth->NbFacetGroups(0);
//      m_pRegions->Shape() = l_EarthShape;
//   }
//
//   m_FadeInKeys[0].m_Time = 0;
//   m_FadeInKeys[0].m_Value = 0;
//   m_FadeInKeys[1].m_Time = in_fFadeTime;
//   m_FadeInKeys[1].m_Value = 1;
//
//   m_FadeOutKeys[0].m_Time = 0;
//   m_FadeOutKeys[0].m_Value = 1;
//   m_FadeOutKeys[1].m_Time = in_fFadeTime;
//   m_FadeOutKeys[1].m_Value = 0;
//
//   m_GenThread.Run(this);
//}
//
//GAtlasEarth::~GAtlasEarth()
//{
//   m_TxtrGen.DisableGeneration();
//   m_GenThread.Terminate(true);
//
//   for(list<GFadeAction>::iterator i = m_vFadeActions.begin();
//       i != m_vFadeActions.end();++ i)
//   {
//      if(i->m_bActive)
//      {
//         if (m_pBGEarth->m_pScene)
//         {
//            UINT32 l_iID = m_pBGEarth->m_pScene->Get_Animation_System()->Get_Action_Index(i->m_pAction);
//            m_pBGEarth->m_pScene->Get_Animation_System()->Remove_Action_At(l_iID);
//         }
//      }
//      SAFE_RELEASE(i->m_pAction);
//   }
//
//   SAFE_RELEASE(m_pRegionMaster);
//   SAFE_RELEASE(m_pRegions);
//   SAFE_RELEASE(m_pRegionMat);
//
//   SAFE_RELEASE(m_pMaster);
//   SAFE_RELEASE(m_pEarth);
//   SAFE_RELEASE(m_pMat);
//   SAFE_RELEASE(m_pTxtr);
//}
//

void GAtlasEarth::UpdateContent()
{
//   // Update all animations
//   for(list<GFadeAction>::iterator i = m_vFadeActions.begin();
//       i != m_vFadeActions.end();++ i)
//   {
//      // Test if animation is still active
//      if(i->m_bActive)
//      {
//         // Test if animation is complete
//         UINT32 l_iID = m_pBGEarth->m_pScene->Get_Animation_System()->Get_Action_Index(i->m_pAction);
//         if(m_pBGEarth->m_pScene->Get_Animation_System()->Get_Action_Time(l_iID) > i->m_pAction->Length() * 2.5f)
//         {
//            // Animation is complete, desactivate it 
//            m_pBGEarth->m_pScene->Get_Animation_System()->Remove_Action_At(l_iID);
//            i->m_bActive = false;
//            if(!i->m_bFadeIn)
//            {
//               m_pEarth->FacetGroup(i->m_iFacetID).Visibility(false);
//            }
//         }
//      }
//   }

   // Set all completed texture to their corresponding facets
   m_Sync.Lock();
   for(UINT32 i = 0;i < m_vGenResults.size();i ++)
   {
      UINT32 l_iLevel = m_vGenResults[i].m_iLevel;
      if( (l_iLevel < m_vLevels.size() ) && (m_vLevels[l_iLevel].m_iGenerationID == m_vGenResults[i].m_iGenerationID) )
      {
         for(UINT32 j = 0;j < m_vGenResults[i].m_vTxtrs.size();j ++)
         {
            CGFX_Texture_Itf* l_pTxtr = m_vGenResults[i].m_vTxtrs[j].m_pTxtr;
            if(l_pTxtr)
            {
               if(m_vGenResults[i].m_iContent & c_AtlasElevation)
               {
                  // Set texture on elevation facet
                  GFX::GOverrideTSampler* l_pOverride = 
                     (GFX::GOverrideTSampler*) m_pEarth->FacetGroup(m_vLevels[l_iLevel].m_pFGroups[ELevelFGroup::Elev] ).Override(0);
                  l_pOverride->OverrideTextureSampler()->Texture(l_pTxtr);

                  // Remove previous level content 
                  // (or background earth bump if this is first level)
                  if(m_pCurrentFGroups[ELevelFGroup::Elev] != INVALID_INDEX)
                  {
                     m_pEarth->FacetGroup(m_pCurrentFGroups[ELevelFGroup::Elev] ).Visibility() = 0.f;
                  }
                  else
                  {
                     m_pBGEarth->ApplyElevationBump(false);
                  }

                  // Remember what facet is currently displayed
                  m_pCurrentFGroups[ELevelFGroup::Elev] = m_vLevels[l_iLevel].m_pFGroups[ELevelFGroup::Elev];
               }
               else if(m_vGenResults[i].m_iContent & c_AtlasDetails)
               {
                  // Set texture on elevation facet
                  GFX::GOverrideTSampler* l_pOverride = 
                     (GFX::GOverrideTSampler*) m_pEarth->FacetGroup(m_vLevels[l_iLevel].m_pFGroups[ELevelFGroup::Detail] ).Override(0);
                  l_pOverride->OverrideTextureSampler()->Texture(l_pTxtr);

   //         SetupFade(l_iFacetId, true);
   //
   //         for(UINT32 j = 0;j < m_vGenResults[i].m_vRegionTxtrs.size();j ++)
   //         {
   //            DisplayRegion(m_vGenResults[i].m_Params, m_vGenResults[i].m_vRegionTxtrs[j] );
   //         }

                  // Remove previous level content 
                  // (or background earth bump if this is first level)
                  if(m_pCurrentFGroups[ELevelFGroup::Detail] != INVALID_INDEX)
                  {
                     m_pEarth->FacetGroup(m_pCurrentFGroups[ELevelFGroup::Detail] ).Visibility() = 0.f;
                  }

                  // Remember what facet is currently displayed
                  m_pCurrentFGroups[ELevelFGroup::Detail] = m_vLevels[l_iLevel].m_pFGroups[ELevelFGroup::Detail];
               }
               SAFE_RELEASE(l_pTxtr);
            }
         }
      }
   }
   m_vGenResults.clear();
   m_Sync.Unlock();
}

void GAtlasEarth::DestroyTopAtlasLevel()
{
   gassert(!m_vLevels.empty(), "Asking to destroy top atlas level when there is no level at all");

   UINT32 l_iTopLevel = m_vLevels.size() - 1;

   m_Sync.Lock();
   // Check if atlas to hide is currently generating
   if(m_iCurGenLevel == l_iTopLevel)
   {
      // Simply cancel the generation, the thread will update the facet
      m_TxtrGen.DisableGeneration();
   }

   // Make sure to remove the atlas from the generation queue
   list<GGenParams>::iterator l_It = m_vGenParams.begin();
   while(l_It != m_vGenParams.end() )
   {
      if(l_It->m_iLevel == l_iTopLevel)
      {
         list<GGenParams>::iterator l_ItErased = l_It;
         ++ l_It;
         m_vGenParams.erase(l_ItErased);
      }
      else
      {
         ++ l_It;
      }
   }

      // Hide the atlas
//      SetupFade(in_iID, false);

//      // Hide all region generated for this facet
//      for(UINT32 f = 0;f < m_vDisplayedRegions.size();f ++)
//      {
//         if(m_vDisplayedRegions[f].second == in_iID)
//         {
//            m_pRegions->FacetGroup(f).Visibility(false);
//            INT32 l_iMaxID = -1;
//            hash_multimap<UINT32, UINT32>::iterator l_RegionIt = m_RegionFacets.find(m_vDisplayedRegions[f].first);
//            while( (l_RegionIt != m_RegionFacets.end() ) && (l_RegionIt->first == m_vDisplayedRegions[f].first) )
//            {
//               UINT32 l_iCurFacet = l_RegionIt->second;
//               if(l_iCurFacet == f)
//               {
//                  m_RegionFacets.erase(l_RegionIt ++);
//               }
//               else
//               {
//                  l_iMaxID = max(l_iMaxID, (INT32) l_iCurFacet);
//                  ++ l_RegionIt;
//               }
//            }
//            if( (m_pRegions->FacetGroup(f).Visibility() ) && (l_iMaxID >= 0) )
//            {
//               m_pRegions->FacetGroup(l_iMaxID).Visibility(true);
//            }
//
//            m_vDisplayedRegions[f].first = 0xFFFFFFFF;
//            m_vDisplayedRegions[f].second = 0xFFFFFFFF;
//         }
//      }
   m_Sync.Unlock();

   const GAtlasLevel& l_LastLevel = m_vLevels.back();

   // Hide & deactivate all facets associated with this level
   for(UINT32 i = 0;i < ELevelFGroup::Count;i ++)
   {
      if(l_LastLevel.m_pFGroups[i] != INVALID_INDEX)
      {
         m_pEarth->FacetGroup(l_LastLevel.m_pFGroups[i] ).Visibility() = 0.f;
         m_vActiveFacets[l_LastLevel.m_pFGroups[i] ] = false;
         m_pEarth->FacetGroup(l_LastLevel.m_pFGroups[i] ).Facets().NbAllocatedIndexes(0);
      }
   }

   m_vLevels.pop_back();

   // Display the previous atlas elevation & detail levels
   m_pCurrentFGroups[ELevelFGroup::Elev] = INVALID_INDEX;
   m_pCurrentFGroups[ELevelFGroup::Detail] = INVALID_INDEX;
   for(UINT32 i = m_vLevels.size();(i > 0) && (m_pCurrentFGroups[ELevelFGroup::Elev] == INVALID_INDEX);i --)
   {
      const GAtlasLevel& l_PrevLevel = m_vLevels[i - 1];
      if(l_PrevLevel.m_pFGroups[ELevelFGroup::Elev] != INVALID_INDEX)
      {
         m_pEarth->FacetGroup(l_PrevLevel.m_pFGroups[ELevelFGroup::Elev] ).Visibility() = 1.f;
         m_pCurrentFGroups[ELevelFGroup::Elev] = l_PrevLevel.m_pFGroups[ELevelFGroup::Elev];
      }
      if(l_PrevLevel.m_pFGroups[ELevelFGroup::Detail] != INVALID_INDEX)
      {
         m_pEarth->FacetGroup(l_PrevLevel.m_pFGroups[ELevelFGroup::Detail] ).Visibility() = 1.f;
         m_pCurrentFGroups[ELevelFGroup::Detail] = l_PrevLevel.m_pFGroups[ELevelFGroup::Detail];
      }
   }

   if(m_pCurrentFGroups[ELevelFGroup::Elev] == INVALID_INDEX)
   {
      m_pBGEarth->ApplyElevationBump(true);
   }
}

//void GAtlasEarth::SetRegionColor(UINT32 in_iRegionID, const GColorRGBReal& in_Color)
//{
//   // Update all generated region with the specified ID with the required color
//   INT32 l_iVisFacet = -1;
//   hash_multimap<UINT32, UINT32>::iterator l_It = m_RegionFacets.find(in_iRegionID);
//   while( (l_It != m_RegionFacets.end() ) && (l_It->first == in_iRegionID) )
//   {
//      UINT32 l_iCurFacet = l_It->second;
//      l_iVisFacet = max( (INT32) m_vDisplayedRegions[l_iCurFacet].second, l_iCurFacet);
//
//      ( (GFX::GOverridePSCascade*) m_pRegions->FacetGroup(l_iCurFacet).Override(1) )->ColorFactorOverride() = in_Color;
//      ( (GFX::GOverridePSCascade*) m_pRegions->FacetGroup(l_iCurFacet).Override(1) )->ColorFactorOverride().A = 0.2f;
//
//      ++ l_It;
//   }
//
//   // Only display the generated region that was generated with the most details
//   if(l_iVisFacet >= 0)
//   {
//      m_pRegions->FacetGroup(l_iVisFacet).Visibility(true);
//   }
//}
//
//void GAtlasEarth::ClearRegionColor(UINT32 in_iRegionID)
//{
//   // Hide all generated region with the specified ID
//   hash_multimap<UINT32, UINT32>::iterator l_It = m_RegionFacets.find(in_iRegionID);
//   while( (l_It != m_RegionFacets.end() ) && (l_It->first == in_iRegionID) )
//   {
//      UINT32 l_iCurFacet = l_It->second;
//      m_pRegions->FacetGroup(l_iCurFacet).Visibility(false);
//
//      ++ l_It;
//   }
//}
//
//void GAtlasEarth::SetupFade(UINT32 in_iFacetID, bool in_bFadeIn)
//{
//   UINT32 l_iFadeValue = in_bFadeIn ? 1 : 0;
//
//   // Create animation that will fade in the facet
//   GFadeAction* l_pFadeAction = NULL;
//   CGFX_Animation_Linear_Itf* l_pAnim = NULL;
//
//   // Try to find a non-activated action
//   for(list<GFadeAction>::iterator i = m_vFadeActions.begin();
//       (i != m_vFadeActions.end() ) && !l_pFadeAction;++ i)
//   {
//      if( (!i->m_bActive) && (i->m_bFadeIn == l_iFadeValue) )
//      {
//         l_pFadeAction = &(*i);
//         l_pAnim = (CGFX_Animation_Linear_Itf*) l_pFadeAction->m_pAction->Get_Animation(0);
//         gassert(l_pAnim->Type() == GFX::TYPE_ANIMATION_LINEAR, "Invalid anim type");
//      }
//   }
//
//   // If there is no action not activated, create a new one
//   if(!l_pFadeAction)
//   {
//      m_vFadeActions.resize(m_vFadeActions.size() + 1);
//      l_pFadeAction = &(m_vFadeActions.back() );
//
//      l_pFadeAction->m_pAction = g_Joshua.Renderer()->Create_Action();
//
//      l_pAnim = g_Joshua.Renderer()->Create_Animation_Linear();
//      if(in_bFadeIn)
//      {
//         l_pAnim->Setup_Keys(m_FadeInKeys, 2);
//      }
//      else
//      {
//         l_pAnim->Setup_Keys(m_FadeOutKeys, 2);
//      }
//      l_pFadeAction->m_pAction->Add_Animation(l_pAnim);
//      l_pFadeAction->m_pAction->Length(m_FadeInKeys[1].m_Time);
//      l_pFadeAction->m_bFadeIn = l_iFadeValue;
//
//      l_pAnim->Release();
//   }
//
//   // Connect the animation to the facet alpha
//   GFX::GOverridePSCascade* l_pPSOverride = 
//      (GFX::GOverridePSCascade*) m_pEarth->FacetGroup(in_iFacetID).Override(1);
//
//   l_pAnim->Connect(l_pPSOverride, GFX::PARAM_COLOR_FACTOR_A);
//
//   l_pFadeAction->m_bActive = 1;
//   l_pFadeAction->m_iFacetID = in_iFacetID;
//
//   UINT32 l_iActionID = m_pBGEarth->m_pScene->Get_Animation_System()->Add_Action(l_pFadeAction->m_pAction);
//   m_pBGEarth->m_pScene->Get_Animation_System()->Play_Action(l_iActionID);
//}
//
//void GAtlasEarth::DisplayRegion(const GGenParams& in_Params, GRegionTxtr& in_RegionTxtr)
//{
//   // Find the first available (non-visible) facet list for this region
//   gassert(m_vDisplayedRegions.size() == m_pRegions->NbFacetGroups(), "Incoherent region count" );
//   vector<pair<UINT32, UINT32> >::iterator l_RegionDisplayIt = m_vDisplayedRegions.begin();
//   UINT32 l_iFacetId = 0;
//   while(l_RegionDisplayIt != m_vDisplayedRegions.end() )
//   {
//      if(l_RegionDisplayIt->first == 0xFFFFFFFF)
//      {
//         break;
//      }
//      ++ l_iFacetId;
//      ++ l_RegionDisplayIt;
//   }
//
//   // If no facet list is available, create a new one
//   CGFX_Transform_2D_Itf* l_pRegionTrans;
//   GFX::GOverrideTSampler* l_pRegionOverride;
//   if(l_iFacetId >= m_pRegions->NbFacetGroups() )
//   {
//      m_pRegions->NbFacetGroups(l_iFacetId + 1);
//
//      // Create a texture transform override for atlas
//      GFX::GOverridePSCascade* l_pPSOverride = g_Joshua.Renderer()->CreateOverridePSCascade();
//      l_pRegionTrans = g_Joshua.Renderer()->Create_Transform_2D();
//      l_pPSOverride->AddUVTransformOverride(0, l_pRegionTrans);
//      l_pPSOverride->UseColorFactorOverride(true);
//
//      // Create a texture shader override for atlas
//      l_pRegionOverride = g_Joshua.Renderer()->CreateOverrideTSampler();
//      l_pRegionOverride->OriginalTextureSamplerID(0);
//      l_pRegionOverride->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
//
//      // Set those override on the atlas facet list
//      m_pRegions->FacetGroup(l_iFacetId).AddOverride(l_pRegionOverride);
//      m_pRegions->FacetGroup(l_iFacetId).AddOverride(l_pPSOverride);
//
//      l_pRegionTrans->Release();
//      l_pPSOverride->Release();
//      l_pRegionOverride->Release();
//
//      m_vDisplayedRegions.resize(m_vDisplayedRegions.size() + 1);
//
//      l_RegionDisplayIt = m_vDisplayedRegions.end() - 1;
//   }
//   else
//   {
//      gassert(m_pRegions->FacetGroup(l_iFacetId).Override(1)->Type() == GFX::TYPE_OVERRIDE_PS_CASCADE, "Invalid atlas facet override");
//      l_pRegionTrans = (CGFX_Transform_2D_Itf*) ( (GFX::GOverridePSCascade*) m_pRegions->FacetGroup(l_iFacetId).Override(1) )->UVTransformOverride(0);
//      gassert(l_pRegionTrans, "Invalid atlas facet override transform");
//
//      gassert(m_pRegions->FacetGroup(l_iFacetId).Override(0)->Type() == GFX::TYPE_OVERRIDE_TSAMPLER, "Invalid atlas facet override");
//      l_pRegionOverride = 
//         (GFX::GOverrideTSampler*) m_pRegions->FacetGroup(l_iFacetId).Override(0);
//   }
//
//   // Update the region data structures
//   l_RegionDisplayIt->first = in_RegionTxtr.m_iID;
//   l_RegionDisplayIt->second = in_Params.m_iFacetId;
//   m_RegionFacets.insert(make_pair(in_RegionTxtr.m_iID, l_iFacetId) );
//   l_pRegionOverride->OverrideTextureSampler()->Texture(in_RegionTxtr.m_pTxtr);
//
//   // Hide all previous regions with the same ID
//   hash_multimap<UINT32, UINT32>::iterator l_It = m_RegionFacets.find(in_RegionTxtr.m_iID);
//   while( (l_It != m_RegionFacets.end() ) && (l_It->first == in_RegionTxtr.m_iID) )
//   {
//      UINT32 l_iCurFacet = l_It->second;
//      m_pRegions->FacetGroup(l_iCurFacet).Visibility(false);
//      ++ l_It;
//   }
//
//   // Update visibility if it is currently displyed
//   if( (m_pBGEarth->m_pFillMesh->NbFacetGroups() > in_RegionTxtr.m_iID) &&
//       (m_pBGEarth->m_pFillMesh->FacetGroup(in_RegionTxtr.m_iID).Visibility() ) )
//   {
//      m_pRegions->FacetGroup(l_iFacetId).Visibility(true);
//      ( (GFX::GOverridePSCascade*) m_pRegions->FacetGroup(l_iFacetId).Override(1) )->ColorFactorOverride() = 
//         ( (GFX::GOverridePSCascade*) m_pBGEarth->m_pFillMesh->FacetGroup(in_RegionTxtr.m_iID).Override(0) )->ColorFactorOverride();
//      ( (GFX::GOverridePSCascade*) m_pRegions->FacetGroup(l_iFacetId).Override(1) )->ColorFactorOverride().A = 0.2f;
//   }
//   else
//   {
//      m_pRegions->FacetGroup(l_iFacetId).Visibility(false);
//   }
//
//   // Determine what sphere triangles to use
//   GTriangleListIndexed& l_RegionTris = m_pRegions->FacetGroup(l_iFacetId).Facets().TriangleListIndexed();
//
//   UINT32 l_iYStart = RoundToInt(floorf( ( (-in_RegionTxtr.m_Coord.y - in_RegionTxtr.m_Size.y + 90.f) * m_iNbVDivs ) / 180.f) );
//   UINT32 l_iYEnd = RoundToInt(ceilf( ( (-in_RegionTxtr.m_Coord.y  + 90.f) * m_iNbVDivs ) / 180.f) );
//
//   gassert(l_iYStart != l_iYEnd, "Invalid atlas subdiv size");
//
//   UINT32 l_iXStart = RoundToInt(floorf( ( (in_RegionTxtr.m_Coord.x + 180.f) * m_iNbHDivs ) / 360.f) );
//   UINT32 l_iXEnd = RoundToInt(ceilf( ( (in_RegionTxtr.m_Coord.x + in_RegionTxtr.m_Size.x + 180.f) * m_iNbHDivs ) / 360.f) );
//
//   gassert(l_iXStart != l_iXEnd, "Invalid atlas subdiv size");
//
//   // Copy sphere triangle to current atlas facet
//   l_RegionTris.NbTriangles( (l_iYEnd - l_iYStart) * (l_iXEnd - l_iXStart) * 2);
//   UINT32 l_iTriIndex = 0;
//   for(UINT32 y = l_iYStart;y < l_iYEnd;y ++)
//   {
//      for(UINT32 x = l_iXStart;x < l_iXEnd;x ++)
//      {
//         l_RegionTris.Triangle(l_iTriIndex) = m_EarthTris.Triangle(y * m_iHTriPitch + x * 2);
//         l_RegionTris.Triangle(l_iTriIndex + 1) = m_EarthTris.Triangle(y * m_iHTriPitch + x * 2 + 1);
//
//         l_iTriIndex += 2;
//      }
//   }
//
//   // Initialize atlas texture transform
//   GVector2D<REAL32> l_Start;
//   l_Start.x = (in_RegionTxtr.m_Coord.x + 180.f) / 360.f;
//   l_Start.y = (-in_RegionTxtr.m_Coord.y + 90.f) / 180.f;
//
//   GVector2D<REAL32> l_End;
//   l_End.x = (in_RegionTxtr.m_Coord.x + in_RegionTxtr.m_Size.x + 180.f) / 360.f;
//   l_End.y = (-in_RegionTxtr.m_Coord.y - in_RegionTxtr.m_Size.y + 90.f) / 180.f;
//
//   l_pRegionTrans->Position().X = l_Start.x / (l_Start.x - l_End.x);
//   l_pRegionTrans->Position().Y = l_Start.y / (l_Start.y - l_End.y);
//   l_pRegionTrans->Size().X = -1.f / (l_Start.x - l_End.x);
//   l_pRegionTrans->Size().Y = -1.f / (l_Start.y - l_End.y);
//}
//

void GAtlasEarth::GAtlasEarthThread::Process(void* in_pParam)
{
   GAtlasEarth* l_pThis = (GAtlasEarth*) in_pParam;

   // Loop until the thread is asked to terminate
   while(Status() != e_Thread_Terminating)
   {
      // Wait for a new element in the generation queue
      l_pThis->m_Queue.WaitAndDecrement();

      // Check if a new texture must be generated
      l_pThis->m_Sync.Lock();
      if(l_pThis->m_vGenParams.size() > 0)
      {
         // Fetch texture generation parameters
         GGenParams l_Params = l_pThis->m_vGenParams.front();
         l_pThis->m_vGenParams.pop_front();
         l_pThis->m_TxtrGen.EnableGeneration();
         l_pThis->m_Sync.Unlock();

         GGenResult l_Results;
         l_Results.m_iLevel = l_Params.m_iLevel;
         l_Results.m_iGenerationID = l_Params.m_iGenerationID;

         // Prepare atlas generation
         l_pThis->m_TxtrGen.Prepare(l_Params.m_Center, 
                                    l_Params.m_Size, 
                                    GVector2D<UINT32>(1024, 768) );

         // Generate elevation
         if(l_Params.m_iContent & c_AtlasElevation)
         {
            l_Results.m_iContent = c_AtlasElevation;
            l_Results.m_vTxtrs.resize(1);
            l_Results.m_vTxtrs.back().m_iInfo = 0;
            l_Results.m_vTxtrs.back().m_pTxtr = l_pThis->m_TxtrGen.GenerateElevation();

            l_pThis->SendResult(l_Results);
         }

         // Generate details
         if(l_Params.m_iContent & c_AtlasDetails)
         {
            l_Results.m_iContent = l_Params.m_iContent & c_AtlasDetails;
            l_Results.m_vTxtrs.resize(1);
            l_Results.m_vTxtrs.back().m_iInfo = 0;
            l_Results.m_vTxtrs.back().m_pTxtr = 
               l_pThis->m_TxtrGen.GenerateDetails( (l_Params.m_iContent & c_AtlasRiver) != 0, 
                                                   (l_Params.m_iContent & c_AtlasRoad) != 0);

            l_pThis->SendResult(l_Results);
         }

         // Clear memory used by atlas texture generator
         l_pThis->m_TxtrGen.Reset();
      }
      else
      {
         // This can happen when a generation was cancelled before being processed
         l_pThis->m_Sync.Unlock();
      }
   }

   Status(e_Thread_Inactive);
}

void GAtlasEarth::SendResult(GGenResult& in_Results)
{
   // Notify the completion of the atlas txtr generation
   m_Sync.Lock();
   if(!m_TxtrGen.IsEnabled() )
   {
      for(UINT32 i = 0;i < in_Results.m_vTxtrs.size();i ++)
      {
         SAFE_RELEASE(in_Results.m_vTxtrs[i].m_pTxtr);
      }
   }

   m_vGenResults.push_back(in_Results);
   m_iCurGenLevel = 0xFFFFFFFF;
   m_Sync.Unlock();
}

void GAtlasEarth::SetZoomLevel(SP2::EZoomLevel::Enum in_eZoomLevel)
{
   switch(in_eZoomLevel)
   {
   case SP2::EZoomLevel::All:
      memcpy(&m_pAtlasLevelContent, &c_pLevelContentTable, sizeof(c_pLevelContentTable));
      break;
   
   case SP2::EZoomLevel::Elevation:
      m_pAtlasLevelContent[0] = c_AtlasElevation;
      m_pAtlasLevelContent[1] = c_AtlasElevation;
      m_pAtlasLevelContent[2] = c_AtlasElevation;
      break;
   
   case SP2::EZoomLevel::ElevationWater:
      m_pAtlasLevelContent[0] = c_AtlasElevation;
      m_pAtlasLevelContent[1] = c_AtlasElevation | c_AtlasWater;
      m_pAtlasLevelContent[2] = c_AtlasElevation | c_AtlasWater;
      break;
   
   case SP2::EZoomLevel::None:
      memset(&m_pAtlasLevelContent, 0, sizeof(m_pAtlasLevelContent));
      break;
   
   default:
      break;
   }
}