/**************************************************************
*
* sp2_atlas_earth_area.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_atlas_earth_area.h"

#include "../include/sp2_context_menu.h"
#include "../include/sp2_context_menu_item.h"

const GPixelX8R8G8B8 c_RoadColor = {20, 58, 84, 0};
const GPixelX8R8G8B8 c_OceanColor = {188, 59, 56, 0};
const GPixelX8R8G8B8 c_RiverColor = {240, 200, 60, 0};
const GPixelX8R8G8B8 c_WaterColor = {255, 236, 204, 0};

const UINT32 c_iNbHeigthLevels = 6;
const GPixelX8R8G8B8 c_LevelColor[c_iNbHeigthLevels] = 
{
   {113, 179, 113, 0},
   { 65, 211, 178, 0},
   {122, 238, 234, 0},
   { 89, 183, 203, 0},
   { 55, 123, 137, 0},
   {228, 228, 228, 0},
};

const INT16 c_iLevelHeight[c_iNbHeigthLevels] = 
{
   200,
   500,
   1000,
   2500,
   4000,
   32767,
};

const INT16 c_iLevelTransition[c_iNbHeigthLevels] = 
{
   20,
   30,
   50,
   150,
   150,
   1000,
};

const UINT32 c_iNbLevels = 65536;
const UINT32 c_iPaletteDivisor = 2;
const UINT32 c_iNbAtlasPaletteColors = c_iNbLevels >> c_iPaletteDivisor;

CWSTR c_sCityTxtrName = L"data/textures/atlas/City.png";
const UINT32 c_iCityIconSize = 16;
const UINT32 c_iCityIconSizeHalf = c_iCityIconSize / 2;
const UINT32 c_iCityIconOffset = 6;

CWSTR c_sUnitOwnedTxtrName = L"data/textures/atlas/UnitOwned.png";
CWSTR c_sUnitEnemyTxtrName = L"data/textures/atlas/UnitEnemy.png";
const UINT32 c_iUnitIconSize = 16;
const UINT32 c_iUnitIconSizeHalf = c_iUnitIconSize / 2;

CWSTR c_sCombatTxtrName = L"data/textures/atlas/Combat.png";
const UINT32 c_iCombatIconSize = 32;
const UINT32 c_iCombatIconSizeHalf = c_iCombatIconSize / 2;

CWSTR c_sUnitSelectTxtrName = L"data/textures/atlas/UnitTarget.png";
const UINT32 c_iUnitSelectSize = 32;
const UINT32 c_iUnitSelectSizeHalf = c_iUnitSelectSize / 2;

CWSTR c_sCombatSelectTxtrName = L"data/textures/atlas/CombatTarget.png";
const UINT32 c_iCombatSelectSize = 64;
const UINT32 c_iCombatSelectSizeHalf = c_iCombatSelectSize / 2;

CWSTR c_sPathTxtrName = L"data/textures/info/Path.png";
const REAL32 c_fMinPathDivLength = 10.f;
const REAL32 c_fPathWidth = 2.f;
const REAL32 c_fPathDashWidth = 0.01f;
const REAL32 c_fPathDashWidthInv = 1.f / c_fPathDashWidth;

const GString GAtlasEarthArea::TypeName(L"ATLAS_AREA");

/*!
* Function which is called to create a Detailed Earth Area GUI object
**/
GUI::GBaseObject* GAtlasEarthArea::New()
{
   return new GAtlasEarthArea();
}

/*!
* GAtlasEarthArea default constructor which sets all GFX member objects as empty
* & initializes all other members to default values.
**/
GAtlasEarthArea::GAtlasEarthArea() : GUIContainer()
{
   // Reset GFX objects
   m_pAtlasMesh = NULL;
   m_pAtlasMat = NULL;
   m_pAtlasTxtr = NULL;

   m_pCitiesMasterModel = NULL;

   m_pIconMesh = NULL;
   m_pOwnedUnitsMasterModel = NULL;
   m_pAlliedUnitsMasterModel = NULL;
   m_pEnemyUnitsMasterModel = NULL;

   m_pCombatsMasterModel = NULL;

   m_pUnitSelectModel = NULL;
   m_pUnitSelectTx = NULL;
   m_pCombatSelectModel = NULL;
   m_pCombatSelectTx = NULL;

   m_ImSize.x = 0;
   m_ImSize.y = 0;

   // Create atlas palette
   m_pAtlasPalette = new GPixelX8R8G8B8[c_iNbAtlasPaletteColors];

   for(UINT16 i = 0;i < c_iNbAtlasPaletteColors;i ++)
   {
      INT16 l_iHeigth = (INT16) (i << c_iPaletteDivisor);

      UINT32 l_iLevelIndex = 0;
      while(l_iHeigth > c_iLevelHeight[l_iLevelIndex])
      {
         l_iLevelIndex ++;
      }

      m_pAtlasPalette[i] = c_LevelColor[l_iLevelIndex];

      if(l_iLevelIndex < c_iNbHeigthLevels - 1)
      {
         INT16 l_iDelta = c_iLevelHeight[l_iLevelIndex] - l_iHeigth;
         if(l_iDelta < c_iLevelTransition[l_iLevelIndex] )
         {
            INT16 l_iDeltaInv = c_iLevelTransition[l_iLevelIndex] - l_iDelta;
            m_pAtlasPalette[i].R = (UINT8) ( ( (UINT16) c_LevelColor[l_iLevelIndex].R * (UINT16) l_iDelta + (UINT16) c_LevelColor[l_iLevelIndex + 1].R * (UINT16) l_iDeltaInv) / c_iLevelTransition[l_iLevelIndex] );
            m_pAtlasPalette[i].G = (UINT8) ( ( (UINT16) c_LevelColor[l_iLevelIndex].G * (UINT16) l_iDelta + (UINT16) c_LevelColor[l_iLevelIndex + 1].G * (UINT16) l_iDeltaInv) / c_iLevelTransition[l_iLevelIndex] );
            m_pAtlasPalette[i].B = (UINT8) ( ( (UINT16) c_LevelColor[l_iLevelIndex].B * (UINT16) l_iDelta + (UINT16) c_LevelColor[l_iLevelIndex + 1].B * (UINT16) l_iDeltaInv) / c_iLevelTransition[l_iLevelIndex] );
         }
      }
   }
}

GAtlasEarthArea::~GAtlasEarthArea()
{
   delete [] m_pAtlasPalette;
}

UINT32 ClosestPow2(UINT32 in_iValue)
{
   UINT32 l_iExp = 0;
   UINT32 l_iPure = 0;
   while(in_iValue > 1)
   {
      l_iPure |= in_iValue & 0x01;

      in_iValue >>= 1;
      l_iExp ++;
   }

   l_iExp += l_iPure;

   return 1 << l_iExp;
}

/*!
* Creation of all GFX objects used to display the Detailed Earth Area.
* Also, creation of children GUI objects.
* For parameters, see GUI::GBaseObject::OnCreate
**/
bool GAtlasEarthArea::OnCreate()
{
   __super::OnCreate();

   // Create atlas GFX objects
   GFile l_File;
   m_pAtlasMesh = g_Joshua.Renderer()->Create_Mesh();

   // Fill atlas mesh
   GShape& l_AtlasShape = m_pAtlasMesh->Shape();

   const UINT32 NB_ICON_VERTICES = 4;

   static REAL32 s_fIconPositions[NB_ICON_VERTICES][3] =
   {
      { 0.f,  0.f,  0.f},
      { 0.f,  1.f,  0.f},
      { 1.f,  0.f,  0.f},
      { 1.f,  1.f,  0.f},
   };

   static REAL32 s_fIconUVs[NB_ICON_VERTICES][2] =
   {
      { 0.f,   0.f},
      { 0.f,   1.f},
      { 1.f,   0.f},
      { 1.f,   1.f},
   };

   static REAL32 s_fIconUVsInv[NB_ICON_VERTICES][2] =
   {
      { 0.f,   1.f},
      { 0.f,   0.f},
      { 1.f,   1.f},
      { 1.f,   0.f},
   };

   const UINT32 NB_ICON_TRIANGLES = 2;

   static UINT32 s_iIconTris[NB_ICON_TRIANGLES][3] =
   {
      {0, 3, 1},
      {0, 2, 3},
   };

   l_AtlasShape.SetupPositions( (GVector3D<REAL32>*) s_fIconPositions, NB_ICON_VERTICES);
   l_AtlasShape.NbTxtrs(1);
   l_AtlasShape.SetupUVCoords(0, (GVector2D<REAL32>*) s_fIconUVsInv, NB_ICON_VERTICES);

   m_pAtlasMesh->NbFacetLists(1);
   m_pAtlasMesh->Facet_List(0).Positions().SetupIndexes( (UINT32*) s_iIconTris, NB_ICON_TRIANGLES * 3);

   // Initialize material
   GFX::GDefaultVertexShader* l_pAtlasVS;
   GFX::GCascadePixelShader* l_pAtlasPS;
   m_pAtlasMat = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, 
                                                                       GFX::PSTYPE_DEF_SIMPLE_TXTR,
                                                                       &l_pAtlasVS,
                                                                       &l_pAtlasPS);

   GFX::GRenderStateCollection* l_AtlasRS = m_pAtlasMat->RenderStates(0);
   l_AtlasRS->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;

   // Initialize texture
   m_ImSize.x = RoundToInt( ( (CGFX_Transform_2D_Itf*) Model()->Get_Transform() )->Size().X);
   m_ImSize.y = RoundToInt( ( (CGFX_Transform_2D_Itf*) Model()->Get_Transform() )->Size().Y);

   UINT32 l_iTxtrWidth = ClosestPow2(m_ImSize.x);
   UINT32 l_iTxtrHeigth = ClosestPow2(m_ImSize.y);

   m_pAtlasTxtr = g_Joshua.Renderer()->TextureManager()->CreateTexture(PIXELFMT_X8R8G8B8, l_iTxtrWidth, l_iTxtrHeigth, false, "TxtrAtlas");

   GImage2D* l_pImage = GImage2D::AllocObject();
   l_pImage->Create(GVector2D<UINT32>(l_iTxtrWidth, l_iTxtrHeigth), PIXELFMT_X8R8G8B8);
   GPixelX8R8G8B8 l_ClearColor;
   l_ClearColor.B = 255;
   l_ClearColor.G = 0;
   l_ClearColor.R = 0;
   l_pImage->Clear( (void*) &l_ClearColor);

   m_pAtlasTxtr->SetImage(*l_pImage);
   GImage2D::FreeObject(l_pImage);

   l_pAtlasPS->TextureSampler(0)->Texture(m_pAtlasTxtr);

   l_pAtlasVS->Release();
   l_pAtlasPS->Release();

   // Update mesh texture coordinates
   GVector2D<REAL32> l_iUVAdjust( (REAL32) m_ImSize.x / (REAL32) l_iTxtrWidth,
                                  (REAL32) m_ImSize.y / (REAL32) l_iTxtrHeigth);
   for(UINT32 i = 0;i < l_AtlasShape.NbUVCoords(0);i ++)
   {
      l_AtlasShape.UVCoord(0, i) *= l_iUVAdjust;
   }

   {
      // Create cities GFX objects
      m_pCitiesMasterModel = g_Joshua.Renderer()->Create_Model();

      GFX::GDefaultVertexShader* l_pCitiesVS;
      GFX::GCascadePixelShader* l_pCitiesPS;
      GFX::GMaterial* l_pCitiesMaterial = 
         g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, 
                                                               GFX::PSTYPE_DEF_SIMPLE_TXTR,
                                                               &l_pCitiesVS,
                                                               &l_pCitiesPS);

      // Initialize cities Model
      m_pModel->Add_Child(m_pCitiesMasterModel);
      m_pCitiesMasterModel->Set_Material(l_pCitiesMaterial);

      // Initialize cities material
      l_pCitiesMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
      l_pCitiesMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_pCitiesMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      l_pCitiesMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;
      g_Joshua.FileManager()->File(c_sCityTxtrName, l_File);
      CGFX_Texture_Itf* l_pCitiesTxtr = g_Joshua.Renderer()->TextureManager()->Create_Texture( l_File, true, GString(L"TxtAtlasCities") );
      l_pCitiesPS->TextureSampler(0)->Texture(l_pCitiesTxtr);

      // Releasing temp GFX objects
      l_pCitiesMaterial->Release();
      l_pCitiesVS->Release();
      l_pCitiesPS->Release();
      l_pCitiesTxtr->Release();
   }

   // Update container Model
   Model()->Set_Primitive(m_pAtlasMesh);
   Model()->Set_Material(m_pAtlasMat);

   {
      // Create units GFX objects
      m_pOwnedUnitsMasterModel = g_Joshua.Renderer()->Create_Model();
      m_pEnemyUnitsMasterModel = g_Joshua.Renderer()->Create_Model();

      GFX::GDefaultVertexShader* l_pOwnedUnitVS;
      GFX::GCascadePixelShader* l_pOwnedUnitPS;
      GFX::GMaterial* l_pOwnedUnitMaterial = 
         g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, 
                                                               GFX::PSTYPE_DEF_SIMPLE_TXTR,
                                                               &l_pOwnedUnitVS,
                                                               &l_pOwnedUnitPS);

      GFX::GDefaultVertexShader* l_pEnemyUnitVS;
      GFX::GCascadePixelShader* l_pEnemyUnitPS;
      GFX::GMaterial* l_pEnemyUnitMaterial = 
         g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, 
                                                               GFX::PSTYPE_DEF_SIMPLE_TXTR,
                                                               &l_pEnemyUnitVS,
                                                               &l_pEnemyUnitPS);

      m_pIconMesh = g_Joshua.Renderer()->Create_Mesh();

      // Initialize unit mesh
      m_pIconMesh->Shape().SetupPositions( (GVector3D<REAL32>*) s_fIconPositions, NB_ICON_VERTICES);
      m_pIconMesh->Shape().NbTxtrs(1);
      m_pIconMesh->Shape().SetupUVCoords(0, (GVector2D<REAL32>*) s_fIconUVs, NB_ICON_VERTICES);

      m_pIconMesh->NbFacetLists(1);
      m_pIconMesh->Facet_List(0).Positions().SetupIndexes( (UINT32*) s_iIconTris, NB_ICON_TRIANGLES * 3);

      // Initialize owned unit Model
      m_pModel->Add_Child(m_pOwnedUnitsMasterModel);
      m_pOwnedUnitsMasterModel->Set_Material(l_pOwnedUnitMaterial);

      // Initialize owned unit material
      l_pOwnedUnitMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
      l_pOwnedUnitMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_pOwnedUnitMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      l_pOwnedUnitMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;
      g_Joshua.FileManager()->File(c_sUnitOwnedTxtrName, l_File);
      CGFX_Texture_Itf* l_pOwnedUnitTxtr = g_Joshua.Renderer()->TextureManager()->Create_Texture( l_File, true, GString(L"TxtOwnedUnit") );
      l_pOwnedUnitPS->TextureSampler(0)->Texture(l_pOwnedUnitTxtr);

      // Initialize enemy unit Model
      m_pModel->Add_Child(m_pEnemyUnitsMasterModel);
      m_pEnemyUnitsMasterModel->Set_Material(l_pEnemyUnitMaterial);

      // Initialize enemy unit material
      l_pEnemyUnitMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
      l_pEnemyUnitMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_pEnemyUnitMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      l_pEnemyUnitMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;
      g_Joshua.FileManager()->File(c_sUnitEnemyTxtrName, l_File);
      CGFX_Texture_Itf* l_pEnemyUnitTxtr = g_Joshua.Renderer()->TextureManager()->Create_Texture( l_File, true, GString(L"TxtEnemyUnit") );
      l_pEnemyUnitPS->TextureSampler(0)->Texture(l_pEnemyUnitTxtr);

      // Releasing temp GFX objects
      l_pOwnedUnitMaterial->Release();
      l_pOwnedUnitVS->Release();
      l_pOwnedUnitPS->Release();
      l_pOwnedUnitTxtr->Release();

      l_pEnemyUnitMaterial->Release();
      l_pEnemyUnitVS->Release();
      l_pEnemyUnitPS->Release();
      l_pEnemyUnitTxtr->Release();
   }

   {
      // Create combat GFX objects
      m_pCombatsMasterModel = g_Joshua.Renderer()->Create_Model();

      GFX::GDefaultVertexShader* l_pCombatVS;
      GFX::GCascadePixelShader* l_pCombatPS;
      GFX::GMaterial* l_pCombatMaterial = 
         g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, 
                                                               GFX::PSTYPE_DEF_SIMPLE_TXTR,
                                                               &l_pCombatVS,
                                                               &l_pCombatPS);

      // Initialize combat Model
      m_pModel->Add_Child(m_pCombatsMasterModel);
      m_pCombatsMasterModel->Set_Material(l_pCombatMaterial);

      // Initialize combat material
      l_pCombatMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
      l_pCombatMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_pCombatMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      l_pCombatMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;
      g_Joshua.FileManager()->File(c_sCombatTxtrName, l_File);
      CGFX_Texture_Itf* l_pCombatTxtr = g_Joshua.Renderer()->TextureManager()->Create_Texture( l_File, true, GString(L"TxtAtlasCombat") );
      l_pCombatPS->TextureSampler(0)->Texture(l_pCombatTxtr);

      // Releasing temp GFX objects
      l_pCombatMaterial->Release();
      l_pCombatVS->Release();
      l_pCombatPS->Release();
      l_pCombatTxtr->Release();
   }

   {
      // Create selection GFX objects
      m_pUnitSelectModel = g_Joshua.Renderer()->Create_Model();
      m_pCombatSelectModel = g_Joshua.Renderer()->Create_Model();

      GFX::GDefaultVertexShader* l_pUnitSelectVS;
      GFX::GCascadePixelShader* l_pUnitSelectPS;
      GFX::GMaterial* l_pUnitSelectMaterial = 
         g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, 
                                                               GFX::PSTYPE_DEF_SIMPLE_TXTR,
                                                               &l_pUnitSelectVS,
                                                               &l_pUnitSelectPS);

      GFX::GDefaultVertexShader* l_pCombatSelectVS;
      GFX::GCascadePixelShader* l_pCombatSelectPS;
      GFX::GMaterial* l_pCombatSelectMaterial = 
         g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, 
                                                               GFX::PSTYPE_DEF_SIMPLE_TXTR,
                                                               &l_pCombatSelectVS,
                                                               &l_pCombatSelectPS);

      m_pUnitSelectTx = g_Joshua.Renderer()->Create_Transform_2D();
      m_pCombatSelectTx = g_Joshua.Renderer()->Create_Transform_2D();

      // Initialize selection models
      m_pModel->Add_Child(m_pUnitSelectModel);
      m_pUnitSelectModel->Set_Material(l_pUnitSelectMaterial);
      m_pUnitSelectModel->Set_Transform(m_pUnitSelectTx);
      m_pUnitSelectModel->Set_Primitive(m_pIconMesh);
      m_pUnitSelectTx->Size().X = c_iUnitSelectSize;
      m_pUnitSelectTx->Size().Y = c_iUnitSelectSize;

      m_pModel->Add_Child(m_pCombatSelectModel);
      m_pCombatSelectModel->Set_Material(l_pCombatSelectMaterial);
      m_pCombatSelectModel->Set_Transform(m_pCombatSelectTx);
      m_pCombatSelectModel->Set_Primitive(m_pIconMesh);
      m_pCombatSelectTx->Size().X = c_iCombatSelectSize;
      m_pCombatSelectTx->Size().Y = c_iCombatSelectSize;

      // Initialize selection materials
      l_pUnitSelectMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
      l_pUnitSelectMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_pUnitSelectMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      l_pUnitSelectMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;
      g_Joshua.FileManager()->File(c_sUnitSelectTxtrName, l_File);
      CGFX_Texture_Itf* l_pUnitSelectTxtr = g_Joshua.Renderer()->TextureManager()->Create_Texture( l_File, true, GString(L"TxtAtlasUnitSelect") );
      l_pUnitSelectPS->TextureSampler(0)->Texture(l_pUnitSelectTxtr);

      l_pCombatSelectMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
      l_pCombatSelectMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_pCombatSelectMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      l_pCombatSelectMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;
      g_Joshua.FileManager()->File(c_sCombatSelectTxtrName, l_File);
      CGFX_Texture_Itf* l_pCombatSelectTxtr = g_Joshua.Renderer()->TextureManager()->Create_Texture( l_File, true, GString(L"TxtAtlasCombatSelect") );
      l_pCombatSelectPS->TextureSampler(0)->Texture(l_pCombatSelectTxtr);

      // Releasing temp GFX objects
      l_pCombatSelectMaterial->Release();
      l_pCombatSelectVS->Release();
      l_pCombatSelectPS->Release();
      l_pCombatSelectTxtr->Release();

      l_pUnitSelectMaterial->Release();
      l_pUnitSelectVS->Release();
      l_pUnitSelectPS->Release();
      l_pUnitSelectTxtr->Release();
   }

   {
      // Setup unit path display
      m_pUnitPathDisplayModel = g_Joshua.Renderer()->Create_Model();
      GFX::GMaterial* l_UnitPathMat = g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT, GFX::PSTYPE_DEF_SIMPLE_TXTR);
      l_UnitPathMat->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
      l_UnitPathMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
      l_UnitPathMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
      l_UnitPathMat->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;
      g_Joshua.FileManager()->File(c_sPathTxtrName, l_File);
      CGFX_Texture_Itf* l_pUnitPathTxtr = g_Joshua.Renderer()->TextureManager()->Create_Texture(l_File, false, c_sPathTxtrName);
      l_UnitPathMat->PixelShader(0)->TextureSampler(0)->Texture(l_pUnitPathTxtr);
      m_pModel->Add_Child(m_pUnitPathDisplayModel);
      m_pModel->Send_To_Back(m_pUnitPathDisplayModel);
      m_pUnitPathDisplayModel->Set_Material(l_UnitPathMat);

      // Releasing temp GFX objects
      l_UnitPathMat->Release();
      l_pUnitPathTxtr->Release();
   }

   // France-Spain meditarean boundary
//   DisplayArea(GVector2D<REAL32>(3.f, 43.f), GVector2D<REAL32>(2.f, 2.f) );
   // Haiti
//   DisplayArea(GVector2D<REAL32>(-73.75f, 18.75f), GVector2D<REAL32>(2.5f, 2.5f) );
   // Hawaii
//   DisplayArea(GVector2D<REAL32>(-155.5f, 19.5f), GVector2D<REAL32>(1.f, 1.f) );
   // Saguenay
//   DisplayArea(GVector2D<REAL32>(-71.5f, 48.25f), GVector2D<REAL32>(1.f, 1.f) );
   // Yukon-BC-Alaska
//   DisplayArea(GVector2D<REAL32>(-137.5f, 58.5f), GVector2D<REAL32>(1.f, 1.f) );
   /////////

   // Corse
//   DisplayArea(GVector2D<REAL32>(9, 42), GVector2D<REAL32>(200000, 200000) );
   // 1/16th of planet
//   DisplayArea(GVector2D<REAL32>(0, 45), GVector2D<REAL32>(EARTH_RADIUS * 2 * Math::PI / 16, EARTH_RADIUS * 2 * Math::PI / 16) );
   // Haiti
//   DisplayArea(GVector2D<REAL32>(-72.85f, 18.90f) );
   // Montreal
//   DisplayArea(GVector2D<REAL32>(-73.70f, 45.5f) );
   // Saguenay
//   DisplayArea(GVector2D<REAL32>(-70.5f, 48.25f) );
   // Niagara
//   DisplayArea(GVector2D<REAL32>(-79.f, 43.f), GVector2D<REAL32>(100000, 100000) );
   // Yemen  
//   DisplayArea(GVector2D<REAL32>(44.0f, 15.75f) );
   // Sherbrooke
//   DisplayArea(GVector2D<REAL32>(-71.9f, 45.4f) );

   return true;
}

/*!
* Destroy the Background Earth, 
*     releases all the GUI & GFX objects that must be
*     released
**/
bool GAtlasEarthArea::OnDestroy()
{
   // Delete Atlas GFX objects
   SAFE_RELEASE(m_pAtlasMesh);
   SAFE_RELEASE(m_pAtlasTxtr);
   SAFE_RELEASE(m_pAtlasMat);

   SAFE_RELEASE(m_pCitiesMasterModel);

   SAFE_RELEASE(m_pIconMesh);
   SAFE_RELEASE(m_pOwnedUnitsMasterModel);
   SAFE_RELEASE(m_pAlliedUnitsMasterModel);
   SAFE_RELEASE(m_pEnemyUnitsMasterModel);

   SAFE_RELEASE(m_pCombatsMasterModel);

   SAFE_RELEASE(m_pUnitSelectModel);
   SAFE_RELEASE(m_pUnitSelectTx);
   SAFE_RELEASE(m_pCombatSelectModel);
   SAFE_RELEASE(m_pCombatSelectTx);

   // Releases the Model & our children
   GUI::GBaseObject::OnDestroy();

   return true;
}


/*!
* Called when the Detailed Earth Area must be displayed
**/
void GAtlasEarthArea::OnShow()
{
   g_ClientDAL.SelectCombat(0);
   g_ClientDCL.KillCombatInfo();

   __super::OnShow();
}


GUI::EEventStatus::Enum GAtlasEarthArea::OnMouseDown(const GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(in_pCaller)
      return GUI::EEventStatus::Handled;

   //If it is a left click
   if(in_EventData.Mouse.Actor.Bits.Left)
   {
      // Verify if click is selecting a unit
      {
         vector<SScreenUnitInfo>& l_Units = g_ClientDAL.VisibleUnits();
         for(UINT32 i = 0;i < l_Units.size();i ++)
         {
            INT32 l_UnitSpaceX = in_EventData.X() - l_Units[i].m_Pos.x;
            INT32 l_UnitSpaceY = in_EventData.Y() - l_Units[i].m_Pos.y;
            if( (l_UnitSpaceX < c_iUnitIconSize) && 
                  (l_UnitSpaceY < c_iUnitIconSize) &&
                  (l_UnitSpaceX >= 0) && 
                  (l_UnitSpaceY >= 0) )
            {
               if (g_ClientDAL.SelectedUnitID().find(l_Units[i].m_iId) != g_ClientDAL.SelectedUnitID().end())
               {
                  if (in_EventData.Keyboard.Ctrl)
                  {
                     g_ClientDAL.UnselectUnit(l_Units[i].m_iId);
                     return GUI::EEventStatus::Handled;
                  }
                  else
                     g_ClientDAL.UnselectUnits();
               }
               // Select the unit
               g_ClientDAL.SelectUnit(l_Units[i].m_iId, in_EventData.Keyboard.Ctrl || in_EventData.Keyboard.Shift);
               g_ClientDAL.SelectCombat(0);
               g_ClientDCL.KillCombatInfo();
               return GUI::EEventStatus::Handled;
            }
         }
      }

      // Verify if click is selecting a combat
      {
         vector<SScreenCombatInfo>& l_Combats = g_ClientDAL.VisibleCombats();
         for(UINT32 i = 0;i < l_Combats.size();i ++)
         {
            INT32 l_CombatSpaceX = in_EventData.X() - l_Combats[i].m_Pos.x;
            INT32 l_CombatSpaceY = in_EventData.Y() - l_Combats[i].m_Pos.y;
            if( (l_CombatSpaceX < c_iCombatIconSize) && 
                  (l_CombatSpaceY < c_iCombatIconSize) &&
                  (l_CombatSpaceX >= 0) && 
                  (l_CombatSpaceY >= 0) )
            {
               // Select the unit
               g_ClientDAL.SelectCombat(l_Combats[i].m_iId);
               g_ClientDAL.UnselectUnits();
               g_ClientDCL.DisplayCombatInfo(g_Joshua.UnitManager().ArenaInfo(g_ClientDAL.SelectedCombatID() ) );
               return GUI::EEventStatus::Handled;
            }
         }
      }

      //If we reach this point, there is no arena selected, kill it
      g_ClientDAL.SelectCombat(0);
      g_ClientDCL.KillCombatInfo();

      // If a unit is selected, move the unit
      if(!g_ClientDAL.SelectedUnitID().empty() && !g_ClientDAL.SelectedCombatID() )
      {
         // Convert mouse coordinate to wolrd coordinate
         GVector2D<REAL32> l_WorldCoord;         
         l_WorldCoord = in_EventData.Mouse.Position * m_ScreentoLLScale + m_ScreentoLLOffset;

         // Request a move for this unit
         for (set<UINT32>::const_iterator itr = g_ClientDAL.SelectedUnitID().begin(); itr != g_ClientDAL.SelectedUnitID().end(); itr++)
            g_ClientDCL.RequestUnitMove(*itr, l_WorldCoord);
         return GUI::EEventStatus::Handled;
      }

      g_ClientDAL.UnselectUnits();
   }
   //If it is a right click
   else if(in_EventData.Mouse.Actor.Bits.Right)
   {
      //Popup the context menu
      GContextMenu* l_pMenu = g_ClientDDL.ContextMenuSpawn();
      l_pMenu->Owner(this);
      l_pMenu->AddItem(L"To Globe View");
      l_pMenu->RegisterHandler(this, (CALLBACK_HANDLER_void_pvoid)HandleContextMenu);
   }

   return GUI::EEventStatus::Handled;
}

bool GAtlasEarthArea::DisplayArea(const GVector2D<REAL32>& in_Center)
{
   GVector2D<REAL32> l_Size;
   GVector2D<UINT32> l_Resolution = g_ClientDAL.LandInfoResolution();
   GVector2D<UINT32> l_Total(360, 180);

   l_Size = (GVector2D<REAL32>) (m_ImSize * l_Total) / (GVector2D<REAL32>) l_Resolution;

   GLandInfoData* l_pLandData = g_ClientDAL.LandInformationDataAlloc(in_Center, l_Size);

   const GVector2D<INT32>& l_Range = l_pLandData->Range();

   m_Location = l_pLandData->Location();

   g_Joshua.Logger()->Log(MSGTYPE_MSG, GString(L"Atlas location : ") + 
                            GString(m_Location.m_Left) + 
                            GString(L";") + 
                            GString(m_Location.m_Top) + 
                            GString(L":") + 
                            GString(m_Location.m_Width) + 
                            GString(L";") + 
                            GString(m_Location.m_Height) );

   m_LLtoScreenScale.x = (REAL32) m_ImSize.x / m_Location.m_Width;
   m_LLtoScreenScale.y = -( (REAL32) m_ImSize.y / m_Location.m_Height);
   m_LLtoScreenOffset.x = -m_Location.m_Left * m_LLtoScreenScale.x;
   m_LLtoScreenOffset.y = m_ImSize.y - m_Location.m_Top * m_LLtoScreenScale.y;

   m_ScreentoLLScale.x = 1.f / m_LLtoScreenScale.x;
   m_ScreentoLLScale.y = 1.f / m_LLtoScreenScale.y;
   m_ScreentoLLOffset = (m_LLtoScreenOffset * m_ScreentoLLScale * -1);

   GImage2D* l_pImage = GImage2D::AllocObject();
   l_pImage->Create(m_ImSize, PIXELFMT_X8R8G8B8);
   GPixelX8R8G8B8 l_ClearColor;
   l_ClearColor.B = 128;
   l_ClearColor.G = 64;
   l_ClearColor.R = 64;
   l_pImage->Clear( (void*) &l_ClearColor);

   GVector2D<UINT32> l_ImCoord;
   GPixelX8R8G8B8* l_pImageBuf = (GPixelX8R8G8B8*) l_pImage->Pixels();
   UINT32 l_iPitch = l_pImage->Pitch();
   UINT32 l_iLinePtrAdjust = (l_iPitch / sizeof(GPixelX8R8G8B8) ) - (UINT32) min( (INT32) l_pImage->Width(), l_Range.x);
   for(l_ImCoord.y = 0;( (INT32) l_ImCoord.y < l_Range.y) && ( (INT32) l_ImCoord.y < l_pImage->Height() );l_ImCoord.y ++)
   {
      for(l_ImCoord.x = 0;( (INT32) l_ImCoord.x < l_Range.x) && (l_ImCoord.x < l_pImage->Width() );l_ImCoord.x ++)
      {
         const GLandInfo* l_pInfo = l_pLandData->DataAt(l_ImCoord.x, l_ImCoord.y);
         if(l_pInfo->m_iRoad)
         {
            *l_pImageBuf = c_RoadColor;
         }
         else if(l_pInfo->m_iType == GLandInfo::OCEAN)
         {
            *l_pImageBuf = c_OceanColor;
         }
         else if(l_pInfo->m_iType == GLandInfo::RIVER)
         {
            *l_pImageBuf = c_RiverColor;
         }
         else if(l_pInfo->m_iType == GLandInfo::WATER)
         {
            *l_pImageBuf = c_WaterColor;
         }
         else
         {
            *l_pImageBuf = m_pAtlasPalette[ ( (UINT16) l_pInfo->m_iElev) >> c_iPaletteDivisor];
         }
         l_pImageBuf ++;
      }
      l_pImageBuf += l_iLinePtrAdjust;
   }


   //Display the cities
   UINT32 l_iCityChildIndex = 0;
   for(UINT32 i = 0 ; i < g_ClientDAL.Cities().size() ; i++)
   {
      for(UINT32 j = 0 ; j < g_ClientDAL.Cities()[i].size() ; j++)
      {
         const SCitiesInfo* l_pCity = &g_ClientDAL.Cities()[i][j];
         //Check if the city should be displayed in the atlas view
         if(
            (l_pCity->m_Position.x > m_Location.m_Left) && (l_pCity->m_Position.x < (m_Location.m_Left + m_Location.m_Width)) &&
            (l_pCity->m_Position.y  > m_Location.m_Top) &&  (l_pCity->m_Position.y < (m_Location.m_Top + m_Location.m_Height)) 
            )
         {
            // Create a label 
            GUI::GLabel* l_pLabel = CREATE_GUI_OBJECT_NAMED_STATIC(GLabel, l_pCity->m_sName, this, true);
            l_pLabel->Set_Font_Size(12);
            l_pLabel->Text(l_pCity->m_sName);
            GVector2D<REAL32> l_CityPos((REAL32)l_pCity->m_Position.x,(REAL32)l_pCity->m_Position.y);
            GVector2D<REAL32> l_ScreenPos = l_CityPos* m_LLtoScreenScale + m_LLtoScreenOffset;
            l_pLabel->Position((INT32)l_ScreenPos.x + c_iCityIconOffset,(INT32)l_ScreenPos.y);
            l_pLabel->Size((INT32)l_pLabel->Text2D()->Width(),(INT32)l_pLabel->Text2D()->Height());
            l_pLabel->Set_Background_Color(GColorRGBInt(0,0,0,0));
            l_pLabel->Set_Font_Color(GColorRGBInt(0,0,0,255));
            l_pLabel->SendToBack();

            // Create an icon for the city
            CGFX_Model_Itf* l_pCityModel = NULL;
            CGFX_Transform_2D_Itf* l_pCityTx = NULL;
            if(l_iCityChildIndex < m_pCitiesMasterModel->Get_Nb_Children() )
            {
               l_pCityModel = m_pCitiesMasterModel->Get_Child(l_iCityChildIndex);
               l_pCityTx = (CGFX_Transform_2D_Itf*) l_pCityModel->Get_Transform();
            }
            else
            {
               l_pCityModel = g_Joshua.Renderer()->Create_Model();
               l_pCityTx = g_Joshua.Renderer()->Create_Transform_2D();

               m_pCitiesMasterModel->Add_Child(l_pCityModel);
               l_pCityModel->Set_Transform(l_pCityTx);
               l_pCityModel->Set_Primitive(m_pIconMesh);

               l_pCityTx->Size().X = c_iCityIconSize;
               l_pCityTx->Size().Y = c_iCityIconSize;
            }

            l_pCityModel->Visibility() = true;

            l_pCityTx->Position() = l_ScreenPos;
            l_pCityTx->Position().X = l_pCityTx->Position().X - c_iCityIconSizeHalf - 0.5f;
            l_pCityTx->Position().Y = l_pCityTx->Position().Y - c_iCityIconSizeHalf - 0.5f;

            l_iCityChildIndex ++;    
         }
      }
   
   }

/*
   //Display the cities
   UINT32 l_iCityChildIndex = 0;
   for(UINT32 i = 0 ; i < g_ClientDAL.NbCities() ; i++)
   {
      SCitiesInfo* l_pCity = &g_ClientDAL.CitiesInfo()[i];

      //Check if the city should be displayed in the atlas view
      if(
         (l_pCity->m_fLongitude > m_Location.m_Left) && (l_pCity->m_fLongitude < (m_Location.m_Left + m_Location.m_Width)) &&
         (l_pCity->m_fLatitude  > m_Location.m_Top) &&  (l_pCity->m_fLatitude < (m_Location.m_Top + m_Location.m_Height)) 
         )
      {
         // Create a label 
         GUI::GLabel* l_pLabel = CREATE_GUI_OBJECT_NAMED_STATIC(GLabel, l_pCity->m_sName, this, true);
         l_pLabel->Set_Font_Size(12);
         l_pLabel->Text(l_pCity->m_sName);
         GVector2D<REAL32> l_CityPos((REAL32)l_pCity->m_fLongitude,(REAL32)l_pCity->m_fLatitude);
         GVector2D<REAL32> l_ScreenPos = l_CityPos* m_LLtoScreenScale + m_LLtoScreenOffset;
         l_pLabel->Position((INT32)l_ScreenPos.x + c_iCityIconOffset,(INT32)l_ScreenPos.y);
         l_pLabel->Size((INT32)l_pLabel->Text2D()->Width(),l_pLabel->Text2D()->Height());
         l_pLabel->Set_Background_Color(GColorRGBInt(0,0,0,0));
         l_pLabel->Set_Font_Color(GColorRGBInt(0,0,0,255));
         l_pLabel->SendToBack();

         // Create an icon for the city
         CGFX_Model_Itf* l_pCityModel = NULL;
         CGFX_Transform_2D_Itf* l_pCityTx = NULL;
         if(l_iCityChildIndex < m_pCitiesMasterModel->Get_Nb_Children() )
         {
            l_pCityModel = m_pCitiesMasterModel->Get_Child(l_iCityChildIndex);
            l_pCityTx = (CGFX_Transform_2D_Itf*) l_pCityModel->Get_Transform();
         }
         else
         {
            l_pCityModel = g_Joshua.Renderer()->Create_Model();
            l_pCityTx = g_Joshua.Renderer()->Create_Transform_2D();

            m_pCitiesMasterModel->Add_Child(l_pCityModel);
            l_pCityModel->Set_Transform(l_pCityTx);
            l_pCityModel->Set_Primitive(m_pIconMesh);

            l_pCityTx->Size().X = c_iCityIconSize;
            l_pCityTx->Size().Y = c_iCityIconSize;
         }

         l_pCityModel->Visibility() = true;

         l_pCityTx->Position() = l_ScreenPos;
         l_pCityTx->Position().X = l_pCityTx->Position().X - c_iCityIconSizeHalf - 0.5f;
         l_pCityTx->Position().Y = l_pCityTx->Position().Y - c_iCityIconSizeHalf - 0.5f;

         l_iCityChildIndex ++;
      }
   }
*/

   m_pAtlasTxtr->SetImage(*l_pImage);
   GImage2D::FreeObject(l_pImage);

   g_ClientDAL.LandInformationDataFree(l_pLandData);
   Animate();

   return false;
}

void GAtlasEarthArea::Animate()
{
   UINT32 l_iOwnedID = g_ClientDAL.ControlledCountryID();
   REAL32 l_fTime = (REAL32) g_Joshua.GameTime();

   // Display units
   {
      g_ClientDAL.VisibleUnits().clear();

      // Remeber path that are displayed
      set<UINT32> l_DisplayedPaths;
      UINT32 l_iCurrentPathChild = 0;

      m_pUnitSelectModel->Visibility() = false;
      const stdext::hash_map<UINT32, Combat::GUnitGroup*>& l_Units = g_Joshua.UnitManager().UnitGroups();

      UINT32 l_iCurrentOwnedChild = 0;
      UINT32 l_iCurrentEnemyChild = 0;

      stdext::hash_map<UINT32, Combat::GUnitGroup*>::const_iterator l_UnitIt = l_Units.begin();
      while(l_UnitIt != l_Units.end() )
      {
         const SP2::GUnitGroup& l_Unit = *( (SP2::GUnitGroup*) l_UnitIt->second);

         if( (l_Unit.Status(l_fTime) < EMilitaryStatus::CountVisible) &&
             (m_Location.PointInRect(l_Unit.Position() ) ) )
         {
            // Verify if unit is moving
            if(l_Unit.Path().Points().size() > 0)
            {
               l_DisplayedPaths.insert(l_Unit.Id() );

               // Verify if path was already created for that unit && path has not changed
               bool l_bUpdatePath = false;
               GUnitPathMapIter l_UnitPath = m_UnitPaths.find(l_Unit.Id() );
               if(l_UnitPath == m_UnitPaths.end() )
               {
                  l_bUpdatePath = true;
               }
               else if(l_UnitPath->second.second != l_Unit.Path().Id() )
               {
                  l_bUpdatePath = true;
               }

               if(l_bUpdatePath)
               {
                  CGFX_Model_Itf* l_pUnitPathModel = NULL;
                  CGFX_Mesh_Itf* l_pUnitPathMesh = NULL;
                  if(l_UnitPath != m_UnitPaths.end() )
                  {
                     // Path has changed, modify it
                     l_pUnitPathModel = l_UnitPath->second.first;
                     l_pUnitPathModel->Inc_Ref();

                     l_pUnitPathMesh = (CGFX_Mesh_Itf*) l_UnitPath->second.first->Get_Primitive();
                     l_pUnitPathMesh->Inc_Ref();

                     l_UnitPath->second.second = l_Unit.Path().Id();
                  }
                  else
                  {
                     // Path does not exist, create it
                     // Find a non-visible Model to show this new path
                     while(l_iCurrentPathChild < m_pUnitPathDisplayModel->Get_Nb_Children() )
                     {
                        if(m_pUnitPathDisplayModel->Get_Child(l_iCurrentPathChild)->Visibility() == 0)
                        {
                           // A non-visible Model was found, update its associated unit path
                           l_pUnitPathModel = m_pUnitPathDisplayModel->Get_Child(l_iCurrentPathChild);
                           l_pUnitPathModel->Inc_Ref();

                           l_pUnitPathMesh = (CGFX_Mesh_Itf*) l_pUnitPathModel->Get_Primitive();
                           l_pUnitPathMesh->Inc_Ref();

                           break;
                        }
                        ++ l_iCurrentPathChild;
                     }

                     // Verify a non visible Model was found
                     if(l_pUnitPathModel == NULL)
                     {
                        // No Model is available, create a new one
                        l_pUnitPathModel = g_Joshua.Renderer()->Create_Model();
                        l_pUnitPathMesh = g_Joshua.Renderer()->Create_Mesh();

                        // Setup the Model
                        m_pUnitPathDisplayModel->Add_Child(l_pUnitPathModel);
                        l_pUnitPathModel->Set_Primitive(l_pUnitPathMesh);

                        // Make sure render list will be recreated
                        g_Joshua.GUIManager()->SetDirty();
                     }

                     // Associate this GFX path to the unit
                     m_UnitPaths.insert(pair<UINT32, GUnitPathInfo>(l_Unit.Id(), GUnitPathInfo(l_pUnitPathModel, l_Unit.Path().Id() ) ) );
                  }

                  l_pUnitPathModel->Visibility() = true;

                  // Update the mesh
                  GShape& l_Shape = l_pUnitPathMesh->Shape();
                  l_Shape.ClearAll();
                  l_Shape.NbTxtrs(1);

                  l_pUnitPathMesh->NbFacetLists(0);
                  l_pUnitPathMesh->NbFacetLists(1);
                  GTriangleListIndexed& l_Tris = l_pUnitPathMesh->Facet_List(0).Positions().TriangleListIndexed();

                  REAL32 l_fPathDist = 0;

                  GVector2D<REAL32> l_LocationScale(1.f, 1.f);
                  l_LocationScale /= GVector2D<REAL32>(m_ImSize);
                  GVector2D<REAL32> l_PreviousPoint = (l_Unit.Path().Points().begin()->m_Position * m_LLtoScreenScale + m_LLtoScreenOffset) * l_LocationScale;
                  for(UINT32 i = 1;i < l_Unit.Path().Points().size();i ++)
                  {
                     GVector2D<REAL32> l_CurrentPoint = ( (l_Unit.Path().Points() )[i].m_Position * m_LLtoScreenScale + m_LLtoScreenOffset) * l_LocationScale;

                     GVector2D<REAL32> l_Dir = l_CurrentPoint - l_PreviousPoint;
                     if(l_Dir.x && l_Dir.y)
                     {
                        REAL32 l_fLength = l_Dir.Length();

                        // Compute endpoints
                        GVector2D<REAL32> l_WidthInc;

                        // This constant is used to adjust facing of triangles when path is reversed
                        UINT32 l_Inc = 0;

                        if(abs(l_Dir.x) > abs(l_Dir.y) )
                        {
                           l_WidthInc.Set(-l_Dir.y, abs(l_Dir.x) );
                           if(l_Dir.x < 0)
                           {
                              l_Inc = 1;
                              l_WidthInc.x *= -1;
                           }
                        }
                        else
                        {
                           l_WidthInc.Set(-abs(l_Dir.y), l_Dir.x);
                           if(l_Dir.y < 0)
                           {
                              l_Inc = 1;
                              l_WidthInc.y *= -1;
                           }
                        }

                        l_WidthInc.Normalize();
                        l_WidthInc *= c_fPathWidth;
                        l_WidthInc *= l_LocationScale;

                        GVector2D<REAL32> l_LineInc(l_Dir.x, l_Dir.y);

                        GVector2D<REAL32> l_Up = l_PreviousPoint + l_WidthInc;
                        GVector2D<REAL32> l_Down = l_PreviousPoint - l_WidthInc;

                        l_fPathDist += l_fLength;

                        l_Shape.AddPosition(l_Up);
                        l_Shape.AddPosition(l_Down);

                        l_Shape.AddUVCoord(0, GVector2D<REAL32>(l_fPathDist * c_fPathDashWidthInv * l_LocationScale.x, 0) );
                        l_Shape.AddUVCoord(0, GVector2D<REAL32>(l_fPathDist * c_fPathDashWidthInv * l_LocationScale.x, 1) );

                        l_Up += l_LineInc;
                        l_Down += l_LineInc;

                        l_Shape.AddPosition(l_Up);
                        l_Shape.AddPosition(l_Down);

                        l_Shape.AddUVCoord(0, GVector2D<REAL32>(l_fPathDist * c_fPathDashWidthInv, 0) );
                        l_Shape.AddUVCoord(0, GVector2D<REAL32>(l_fPathDist * c_fPathDashWidthInv, 1) );

                        // Create triangles
                        GTriangleIndex l_Tri;
                        l_Tri.m_pVertices[0] = (i - 1) * 2;
                        l_Tri.m_pVertices[1] = (i - 1) * 2 + 2 - l_Inc;
                        l_Tri.m_pVertices[2] = (i - 1) * 2 + 1 + l_Inc;
                        l_Tris.AddTriangle(l_Tri);

                        l_Tri.m_pVertices[0] = l_Tri.m_pVertices[2];
                        l_Tri.m_pVertices[2] += (2 - l_Inc);
                        l_Tris.AddTriangle(l_Tri);
                     }

                     l_PreviousPoint = l_CurrentPoint;
                  }

                  // Release temporary objects
                  l_pUnitPathModel->Release();
                  l_pUnitPathMesh->Release();
               }
               else
               {
                  // Simply make it visible
                  l_UnitPath->second.first->Visibility() = true;
               }
            }

            GVector2D<REAL32> l_ScreenCoordReal = (l_Unit.Position() * m_LLtoScreenScale) + m_LLtoScreenOffset;
            GVector2D<UINT32> l_ScreenCoord(RoundToInt(l_ScreenCoordReal.x), 
                                            RoundToInt(l_ScreenCoordReal.y) );

            CGFX_Model_Itf* l_pModel = NULL;
            CGFX_Transform_2D_Itf* l_pTransform = NULL;

            if(l_iOwnedID == l_Unit.OwnerId() )
            {
               // Verify if a child is already created for this owned unit
               if(l_iCurrentOwnedChild < m_pOwnedUnitsMasterModel->Get_Nb_Children() )
               {
                  // Update child data
                  l_pModel = m_pOwnedUnitsMasterModel->Get_Child(l_iCurrentOwnedChild);
                  l_pTransform = (CGFX_Transform_2D_Itf*) l_pModel->Get_Transform();
               }
               else
               {
                  // Create a new child & its transform
                  l_pModel = g_Joshua.Renderer()->Create_Model();
                  l_pTransform = g_Joshua.Renderer()->Create_Transform_2D();

                  // Setup child
                  m_pOwnedUnitsMasterModel->Add_Child(l_pModel);
                  l_pModel->Set_Transform(l_pTransform);
                  l_pModel->Set_Primitive(m_pIconMesh);

                  // Setup transform
                  l_pTransform->Size().X = c_iUnitIconSize;
                  l_pTransform->Size().Y = c_iUnitIconSize;

                  g_Joshua.GUIManager()->SetDirty();

                  l_pModel->Release();
                  l_pTransform->Release();
               }
               l_iCurrentOwnedChild ++;
            }
            else
            {
               // Verify if a child is already created for this owned unit
               if(l_iCurrentEnemyChild < m_pEnemyUnitsMasterModel->Get_Nb_Children() )
               {
                  // Update child data
                  l_pModel = m_pEnemyUnitsMasterModel->Get_Child(l_iCurrentEnemyChild);
                  l_pTransform = (CGFX_Transform_2D_Itf*) l_pModel->Get_Transform();
               }
               else
               {
                  // Create a new child & its transform
                  l_pModel = g_Joshua.Renderer()->Create_Model();
                  l_pTransform = g_Joshua.Renderer()->Create_Transform_2D();

                  // Setup child
                  m_pEnemyUnitsMasterModel->Add_Child(l_pModel);
                  l_pModel->Set_Transform(l_pTransform);
                  l_pModel->Set_Primitive(m_pIconMesh);

                  // Setup transform
                  l_pTransform->Size().X = c_iUnitIconSize;
                  l_pTransform->Size().Y = c_iUnitIconSize;

                  g_Joshua.GUIManager()->SetDirty();

                  l_pModel->Release();
                  l_pTransform->Release();
               }
               l_iCurrentEnemyChild ++;
            }

            l_pModel->Visibility() = true;
            l_pTransform->Position() = l_ScreenCoord;
            l_pTransform->Position().X = l_pTransform->Position().X - c_iUnitIconSizeHalf - 0.5f;
            l_pTransform->Position().Y = l_pTransform->Position().Y - c_iUnitIconSizeHalf - 0.5f;

            SScreenUnitInfo l_UnitInfo;
            l_UnitInfo.m_iId = l_Unit.Id();
            l_UnitInfo.m_Pos = GVector2D<INT16>( (INT16) l_ScreenCoord.x - c_iUnitIconSizeHalf,
                                                 (INT16) l_ScreenCoord.y - c_iUnitIconSizeHalf);
            g_ClientDAL.VisibleUnits().push_back(l_UnitInfo);

            // Verify if current unit is the currently selected one
            if(g_ClientDAL.SelectedUnitID().find(l_Unit.Id()) != g_ClientDAL.SelectedUnitID().end())
            {
               m_pUnitSelectModel->Visibility() = true;
               m_pUnitSelectTx->Position().X = l_ScreenCoord.x - c_iUnitSelectSizeHalf - 0.5f;
               m_pUnitSelectTx->Position().Y = l_ScreenCoord.y - c_iUnitSelectSizeHalf - 0.5f;
            }
         }

         ++ l_UnitIt;
      }

      // Hide remaining children
      while(l_iCurrentOwnedChild < m_pOwnedUnitsMasterModel->Get_Nb_Children() )
      {
         m_pOwnedUnitsMasterModel->Get_Child(l_iCurrentOwnedChild)->Visibility() = false;

         l_iCurrentOwnedChild ++;
      }

      while(l_iCurrentEnemyChild < m_pEnemyUnitsMasterModel->Get_Nb_Children() )
      {
         m_pEnemyUnitsMasterModel->Get_Child(l_iCurrentEnemyChild)->Visibility() = false;

         l_iCurrentEnemyChild ++;
      }

      // Hide non visible paths
      GUnitPathMapIter l_PathIter = m_UnitPaths.begin();
      vector<UINT32> l_vPathToRemove;

      while(l_PathIter != m_UnitPaths.end() )
      {
         if(l_DisplayedPaths.find(l_PathIter->first) == l_DisplayedPaths.end() )
         {
            l_vPathToRemove.push_back(l_PathIter->first);
         }
         ++ l_PathIter;
      }

      for(UINT32 p = 0;p < l_vPathToRemove.size();p ++)
      {
         GUnitPathMapIter l_It = m_UnitPaths.find(l_vPathToRemove[p] );
         l_It->second.first->Visibility() = false;
         m_UnitPaths.erase(l_It);
      }
   }

   // Display combats
   {
      g_ClientDAL.VisibleCombats().clear();
      m_pCombatSelectModel->Visibility() = false;
      const stdext::hash_map<UINT32, Combat::GArenaInfo*>& l_Combats = g_Joshua.UnitManager().ArenaInfos();

      UINT32 l_iCurrentCombatChild = 0;

      stdext::hash_map<UINT32, Combat::GArenaInfo*>::const_iterator l_CombatIt = l_Combats.begin();
      while(l_CombatIt != l_Combats.end() )
      {
         const Combat::GArenaInfo& l_Combat = *(l_CombatIt->second);

         if(m_Location.PointInRect(l_Combat.m_Coordinate) )
         {
            GVector2D<REAL32> l_ScreenCoordReal = (l_Combat.m_Coordinate * m_LLtoScreenScale) + m_LLtoScreenOffset;
            GVector2D<UINT32> l_ScreenCoord(RoundToInt(l_ScreenCoordReal.x), 
                                            RoundToInt(l_ScreenCoordReal.y) );

            CGFX_Model_Itf* l_pModel = NULL;
            CGFX_Transform_2D_Itf* l_pTransform = NULL;

            // Verify if a child is already created for this owned unit
            if(l_iCurrentCombatChild < m_pCombatsMasterModel->Get_Nb_Children() )
            {
               // Update child data
               l_pModel = m_pCombatsMasterModel->Get_Child(l_iCurrentCombatChild);
               l_pTransform = (CGFX_Transform_2D_Itf*) l_pModel->Get_Transform();
            }
            else
            {
               // Create a new child & its transform
               l_pModel = g_Joshua.Renderer()->Create_Model();
               l_pTransform = g_Joshua.Renderer()->Create_Transform_2D();

               // Setup child
               m_pCombatsMasterModel->Add_Child(l_pModel);
               l_pModel->Set_Transform(l_pTransform);
               l_pModel->Set_Primitive(m_pIconMesh);

               // Setup transform
               l_pTransform->Size().X = c_iCombatIconSize;
               l_pTransform->Size().Y = c_iCombatIconSize;

               g_Joshua.GUIManager()->SetDirty();

               l_pModel->Release();
               l_pTransform->Release();
            }
            l_iCurrentCombatChild ++;

            l_pModel->Visibility() = true;
            l_pTransform->Position() = l_ScreenCoord;
            l_pTransform->Position().X = l_pTransform->Position().X - c_iCombatIconSizeHalf - 0.5f;
            l_pTransform->Position().Y = l_pTransform->Position().Y - c_iCombatIconSizeHalf - 0.5f;

            SScreenCombatInfo l_CombatInfo;
            l_CombatInfo.m_iId = l_Combat.m_iId;
            l_CombatInfo.m_Pos = GVector2D<INT16>( (INT16) l_ScreenCoord.x - c_iCombatIconSizeHalf,
                                                   (INT16) l_ScreenCoord.y - c_iCombatIconSizeHalf);
            g_ClientDAL.VisibleCombats().push_back(l_CombatInfo);

            // Verify if current combat is the currently selected one
            if(l_Combat.m_iId == g_ClientDAL.SelectedCombatID() )
            {
               m_pCombatSelectModel->Visibility() = true;
               m_pCombatSelectTx->Position().X = l_ScreenCoord.x - c_iCombatSelectSizeHalf - 0.5f;
               m_pCombatSelectTx->Position().Y = l_ScreenCoord.y - c_iCombatSelectSizeHalf - 0.5f;
            }
         }
         ++ l_CombatIt;
      }

      while(l_iCurrentCombatChild < m_pCombatsMasterModel->Get_Nb_Children() )
      {
         m_pCombatsMasterModel->Get_Child(l_iCurrentCombatChild)->Visibility() = false;

         l_iCurrentCombatChild ++;
      }
   }
}


/*!
 * Handle the context menu commands
 **/
void GAtlasEarthArea::HandleContextMenu(GContextMenuItem* in_pMenuItem)
{
   g_ClientDCL.ContextSwitch(SP2::CONTEXT_BKGND_EARTH);
}