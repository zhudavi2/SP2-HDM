/**************************************************************
*
* sp2_overlay_city.cpp
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

#include "../include/sp2_overlay_city.h"

#include "../include/sp2_background_earth.h"

CWSTR c_sConfigCityNameFont   = L"Background_Earth>>Atlas>>City_Name_Font";
CWSTR c_sConfigCityNameSize   = L"Background_Earth>>Atlas>>City_Name_Size";
CWSTR c_sConfigCityNameColor  = L"Background_Earth>>Atlas>>City_Name_Color";

CWSTR c_sDefaultCityNameFont                 = L"Arial";
const INT32 c_iDefaultCityNameSize           = 12;
const GColorRGBInt c_DefaultCityNameColor   = GColorRGBInt(222, 222, 222, 240);

CWSTR c_sConfigCityTxtr[c_iNbCityTypes + 1] =
{  L"Background_Earth>>Atlas>>Town"
,  L"Background_Earth>>Atlas>>City"
,  L"Background_Earth>>Atlas>>Metropolis"
,  L"Background_Earth>>Atlas>>Megapolis"
,  L"Background_Earth>>Atlas>>Capital"
};

CWSTR c_sDefaultCityTxtr[c_iNbCityTypes + 1] =
{  L"/data/textures/atlas/town.png"
,  L"/data/textures/atlas/city.png"
,  L"/data/textures/atlas/metropolis.png"
,  L"/data/textures/atlas/megapolis.png"
,  L"/data/textures/atlas/capital.png"
};

const UINT32 c_iCityTypePoluationMin[c_iNbCityTypes] =
{  0
,  20000
,  200000
,  2000000
};

GOverlayCity::GOverlayCity(GBackgroundEarth* in_pBGEarth)
{
   m_pBGEarth = in_pBGEarth;
}

GOverlayCity::~GOverlayCity()
{
   SAFE_RELEASE(m_pCitiesMasterModel);

   for(UINT32 i = 0;i < c_iNbCityTypes + 1;i ++)
   {
      SAFE_RELEASE(m_pCityMeshes[i] );
      SAFE_RELEASE(m_pCityTSamplers[i] );
   }

   SAFE_RELEASE(m_pCityNameModel);
   SAFE_RELEASE(m_pCityNameTfx);
   SAFE_RELEASE(m_pCityNameText);
}

void GOverlayCity::InitCities()
{
   // Create city overlay on earth
   m_pCitiesMasterModel = g_Joshua.Renderer()->Create_Model();

   // Create city GFX objects
   GFX::GDefaultVertexShader* l_pCityVS;
   GFX::GCascadePixelShader* l_pCityPS;
   GFX::GMaterial* l_pCityMaterial =
      g_Joshua.Renderer()->Helper()->CreateCompleteMaterial(GFX::VSTYPE_DEF_NO_LIGHT,
      GFX::PSTYPE_DEF_SIMPLE_TXTR,
      &l_pCityVS,
      &l_pCityPS);

   // Initialize city Model
   m_pBGEarth->m_pOverlay->Add_Child(m_pCitiesMasterModel);
   m_pBGEarth->m_pOverlay->Send_To_Back(m_pCitiesMasterModel);
   m_pCitiesMasterModel->Set_Material(l_pCityMaterial);

   // Initialize city material
   l_pCityMaterial->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
   l_pCityMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_BLEND_ENABLE) = true;
   l_pCityMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_SRCALPHA;
   l_pCityMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_INVSRCALPHA;

   CGFX_Texture_Itf* l_pCityTxtr =
      g_Joshua.Renderer()->Helper()->LoadTexture(m_pBGEarth->ConfigValue(c_sConfigCityTxtr[0],
                                                      c_sDefaultCityTxtr[0] ),
                              true);
   l_pCityPS->TextureSampler(0)->Texture(l_pCityTxtr);

   // Initialize all city overrides & meshes
   for(UINT32 i = 0;i < c_iNbCityTypes + 1;i ++)
   {
      m_pCityMeshes[i] = g_Joshua.Renderer()->CreateMesh();

      CGFX_Texture_Itf* l_pOverrideTxtr =
         g_Joshua.Renderer()->Helper()->LoadTexture(m_pBGEarth->ConfigValue(c_sConfigCityTxtr[i], c_sDefaultCityTxtr[i] ), true);

      m_CitySize[i].x = l_pOverrideTxtr->PartialWidth();
      m_CitySize[i].y = l_pOverrideTxtr->PartialHeight();

      m_CitySizeHalf[i] = m_CitySize[i] / 2;

      if(l_pOverrideTxtr != l_pCityTxtr)
      {
         m_pCityTSamplers[i] = g_Joshua.Renderer()->CreateOverrideTSampler();
         m_pCityTSamplers[i]->AddOverrides(GFX::TSOVERRIDE_TEXTURE);
         m_pCityTSamplers[i]->OriginalTextureSamplerID(0);
         m_pCityTSamplers[i]->OverrideTextureSampler()->Texture(l_pOverrideTxtr);

         g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pCityMeshes[i], l_pOverrideTxtr);
      }
      else
      {
         m_pCityTSamplers[i] = NULL;
         g_Joshua.Renderer()->Helper()->SetupSpriteMesh(m_pCityMeshes[i], l_pCityTxtr);
      }
      l_pOverrideTxtr->Release();
   }

   // Initialize city name display
   m_pCityNameModel = g_Joshua.Renderer()->Create_Model();
   m_pCityNameTfx = g_Joshua.Renderer()->Create_Transform_2D();
   m_pCityNameText = g_Joshua.Renderer()->Create_Text_2D();

   GFX::GWindow* l_pBG = g_Joshua.Renderer()->CreateWindow();
   l_pBG->Color() = GColorRGBReal(0, 0, 0, 0.7f);
   m_pCityNameModel->Set_Primitive(l_pBG);
   SAFE_RELEASE(l_pBG);

   CGFX_Model_Itf* l_pTextModel = g_Joshua.Renderer()->Create_Model();
   m_pCityNameModel->Add_Child(l_pTextModel);
   m_pBGEarth->m_pOverlay->Add_Child(m_pCityNameModel);
   m_pCityNameModel->Set_Transform(m_pCityNameTfx);
   l_pTextModel->Set_Primitive(m_pCityNameText);
   SAFE_RELEASE(l_pTextModel);

   m_pCityNameModel->Visibility() = 0;

   m_pCityNameText->Init(m_pBGEarth->ConfigValue(c_sConfigCityNameFont, c_sDefaultCityNameFont), 
                         m_pBGEarth->ConfigValue(c_sConfigCityNameSize, c_iDefaultCityNameSize), 
                         false);

   m_pCityNameText->Color() = m_pBGEarth->ConfigValue(c_sConfigCityNameColor, c_DefaultCityNameColor);

   // Release temporary objects
   SAFE_RELEASE(l_pCityMaterial);
   SAFE_RELEASE(l_pCityVS);
   SAFE_RELEASE(l_pCityPS);
   SAFE_RELEASE(l_pCityTxtr);
}


void GOverlayCity::HideCities()
{
   m_pCitiesMasterModel->Visibility() = 0;
}

void GOverlayCity::ShowCities()
{
   m_pCitiesMasterModel->Visibility() = 1;
}

void GOverlayCity::UpdateCities()
{
   if(m_pCitiesMasterModel->Visibility() == 0)
      return;

   g_ClientDAL.VisibleCities().clear();

   // Compute factors to convert from screen coordiantes to 3D coordinates
   GVector2D<REAL32> l_ScreenTransformScale = GVector2D<REAL32>(m_pBGEarth->m_Mode.m_Resolution) * 0.5f;
   GVector2D<REAL32> l_ScreenTransformOffset(l_ScreenTransformScale);
   l_ScreenTransformScale.y *= -1;
   l_ScreenTransformOffset.x -= 0.5f;
   l_ScreenTransformOffset.y -= 0.5f;

   UINT32 l_iCurrentChild = 0;

   // Calculate threshold for city display
   UINT32 l_iThreshold = ( (c_iNbCityTypes - RoundToInt(g_ClientDCL.m_EarthState.m_Zoom * c_iNbCityTypes) ) * 3) >> 1;

   if(l_iThreshold < c_iNbCityTypes)
   {
      // Display all capitals
      for(UINT32 i = 1;i <= g_ClientDAL.NbCountry();i ++)
      {
         // Fetch capital location
         if(g_ClientDAL.CapitalExists(i) )
         {
            const GVector2D<REAL32>& l_CapLoc = g_ClientDAL.CapitalPosition(i);
            GVector3D<REAL32> l_Pos = m_pBGEarth->ConvertSphericalToRectangular(l_CapLoc);

            GVector2D<INT32> l_ScreenPos;
            if(m_pBGEarth->ProjectAndClipOnEarth3DPointIn2D(l_Pos, l_ScreenPos) )
            {
               // Verify if a child is already created for this city
               CGFX_Model_Itf* l_pModel = NULL;
               CGFX_Transform_2D_Itf* l_pTransform = NULL;
               if(l_iCurrentChild < m_pCitiesMasterModel->Get_Nb_Children() )
               {
                  // Update child data
                  l_pModel = m_pCitiesMasterModel->Get_Child(l_iCurrentChild);
                  l_pModel->Visibility() = true;
                  l_pTransform = (CGFX_Transform_2D_Itf*) l_pModel->Get_Transform();
                  l_pModel->RemoveOverrideAt(0);

                  if(l_pModel->Get_Primitive() != m_pCityMeshes[c_iNbCityTypes] )
                  {
                     l_pModel->Set_Primitive(m_pCityMeshes[c_iNbCityTypes] );
                     g_Joshua.GUIManager()->SetDirty();
                  }
               }
               else
               {
                  // Create a new child & its transform
                  l_pModel = g_Joshua.Renderer()->Create_Model();
                  l_pTransform = g_Joshua.Renderer()->Create_Transform_2D();

                  // Setup child
                  m_pCitiesMasterModel->Add_Child(l_pModel);
                  l_pModel->Set_Transform(l_pTransform);
                  l_pModel->Set_Primitive(m_pCityMeshes[c_iNbCityTypes] );

                  g_Joshua.GUIManager()->SetDirty();

                  l_pModel->Release();
                  l_pTransform->Release();
               }

               // Go to next child
               l_iCurrentChild ++;

               l_pModel->AddOverride(m_pCityTSamplers[c_iNbCityTypes] );

               // Setup transform
               l_pTransform->Position() = m_pBGEarth->ScreenAlignTransform(l_ScreenPos, GVector2D<INT32>(m_CitySizeHalf[c_iNbCityTypes] ) * -1);
               l_pTransform->Size().X = (REAL32) m_CitySize[c_iNbCityTypes].x;
               l_pTransform->Size().Y = (REAL32) m_CitySize[c_iNbCityTypes].y;
            }
         }
      }

      // Fetch the list of cities
      const vector<SCityInfo>& l_vCities = g_ClientDAL.Cities();

      // For each city, compute its location
      for(UINT32 i = 0;i < l_vCities.size();i ++)
      {
         const SCityInfo& l_City = l_vCities[i];
         if(l_City.m_iId != 0xFFFFFFFF)
         {
            // Make sure city is large enough to be displayed
            UINT32 l_iCityType = CityPopToType(l_City.m_iPopulation);
            if(l_iCityType < l_iThreshold)
               continue;

            // Convert city earth coordinates into world coordinates
            REAL32 l_fPhi = l_City.m_Position.x * (REAL32) Math::PI / 180.f;
            REAL32 l_fTheta = (90 - l_City.m_Position.y) * (REAL32) Math::PI / 180.f;

            GVector3D<REAL32> l_CityPos;
            l_CityPos.z = - sinf(l_fTheta) * sinf(l_fPhi);
            l_CityPos.y = cosf(l_fTheta);
            l_CityPos.x = - sinf(l_fTheta) * cosf(l_fPhi);

            // Make sure this location on earth is visible
            GVector3D<REAL32> l_ViewCityPos = m_pBGEarth->m_ViewMat * l_CityPos;

            // Verify if city is in front of earth
            if(l_ViewCityPos.z < m_pBGEarth->m_OriginPos.z - 1 / m_pBGEarth->m_OriginPos.z)
            {
               GVector2D<REAL32> l_ScreenCityPos = ( (m_pBGEarth->m_ProjMat * GVector4D<REAL32>(l_ViewCityPos) ).ApplyWeigth() );

               // Verify if city is visible in screen
               if( (l_ScreenCityPos.x >= -1.1f) && 
                  (l_ScreenCityPos.x <=  1.1f) && 
                  (l_ScreenCityPos.y >= -1.1f) && 
                  (l_ScreenCityPos.y <=  1.1f) )
               {
                  // Update current child object with arena screen position
                  l_ScreenCityPos *= l_ScreenTransformScale;
                  l_ScreenCityPos += l_ScreenTransformOffset;

                  SScreenOverlayInfo l_CityInfo;
                  l_CityInfo.m_iId = l_City.m_iId;
                  l_CityInfo.m_eType = EOverlayType::City;
                  l_CityInfo.m_Pos = l_ScreenCityPos - m_CitySizeHalf[l_iCityType];
                  l_CityInfo.m_Size = m_CitySize[l_iCityType];
                  g_ClientDAL.VisibleCities().push_back(l_CityInfo);

                  INT32 l_iNbElems = 1;
                  if(g_ClientDAL.CityIsCapital(l_City) )
                  {
                     l_iNbElems = 2;
                  }

                  for(INT32 e = l_iNbElems - 1;e >= 0;e --)
                  {
                     UINT32 l_iActualCityType = (e == 0) ? l_iCityType : c_iNbCityTypes;

                     // Verify if a child is already created for this city
                     CGFX_Model_Itf* l_pModel = NULL;
                     CGFX_Transform_2D_Itf* l_pTransform = NULL;
                     if(l_iCurrentChild < m_pCitiesMasterModel->Get_Nb_Children() )
                     {
                        // Update child data
                        l_pModel = m_pCitiesMasterModel->Get_Child(l_iCurrentChild);
                        l_pModel->Visibility() = true;
                        l_pTransform = (CGFX_Transform_2D_Itf*) l_pModel->Get_Transform();
                        l_pModel->RemoveOverrideAt(0);

                        if(l_pModel->Get_Primitive() != m_pCityMeshes[l_iActualCityType] )
                        {
                           l_pModel->Set_Primitive(m_pCityMeshes[l_iActualCityType] );
                           g_Joshua.GUIManager()->SetDirty();
                        }
                     }
                     else
                     {
                        // Create a new child & its transform
                        l_pModel = g_Joshua.Renderer()->Create_Model();
                        l_pTransform = g_Joshua.Renderer()->Create_Transform_2D();

                        // Setup child
                        m_pCitiesMasterModel->Add_Child(l_pModel);
                        l_pModel->Set_Transform(l_pTransform);
                        l_pModel->Set_Primitive(m_pCityMeshes[l_iActualCityType] );

                        g_Joshua.GUIManager()->SetDirty();

                        l_pModel->Release();
                        l_pTransform->Release();
                     }

                     // Go to next child
                     l_iCurrentChild ++;

                     l_pModel->AddOverride(m_pCityTSamplers[l_iActualCityType] );

                     // Setup transform
                     l_pTransform->Position() = l_ScreenCityPos- m_CitySizeHalf[l_iActualCityType];
                     l_pTransform->Size().X = (REAL32) m_CitySize[l_iActualCityType].x;
                     l_pTransform->Size().Y = (REAL32) m_CitySize[l_iActualCityType].y;
                  }
               }
            }
         }
      }
   }

   // Hide remaining children
   while(l_iCurrentChild < m_pCitiesMasterModel->Get_Nb_Children() )
   {
      m_pCitiesMasterModel->Get_Child(l_iCurrentChild)->Visibility() = false;

      l_iCurrentChild ++;
   }

   ShowCityName(g_Joshua.GUIManager()->MousePosition() );
}

void GOverlayCity::ShowCityName(const GVector2D<INT32>& in_MousePos)
{
   // Check all visible cities to see if the mouse is over one of them
   for(UINT32 i = 0;i < g_ClientDAL.VisibleCities().size();i ++)
   {
      const SScreenOverlayInfo& l_City = g_ClientDAL.VisibleCities().at(g_ClientDAL.VisibleCities().size() - i - 1);
      GVector2D<INT32> l_MouseCoordCity = in_MousePos - l_City.m_Pos;
      if( (l_MouseCoordCity.x >= 0) && (l_MouseCoordCity.x < l_City.m_Size.x) && 
          (l_MouseCoordCity.y >= 0) && (l_MouseCoordCity.y < l_City.m_Size.y) )
      {
         // The mouse overlaps this city, display its name
         m_pCityNameModel->Visibility() = 1;
         m_pCityNameText->Text(g_ClientDAL.CityName(l_City.m_iId) );
         m_pCityNameTfx->Position().X = (REAL32) (l_City.m_Pos.x + l_City.m_Size.x);
         m_pCityNameTfx->Position().Y = (REAL32) l_City.m_Pos.y;
         m_pCityNameTfx->Size().X = (REAL32) m_pCityNameText->Width();
         m_pCityNameTfx->Size().Y = (REAL32) m_pCityNameText->Height();

         return;
      }
   }

   m_pCityNameModel->Visibility() = 0;
}

UINT32 GOverlayCity::CityPopToType(UINT32 in_iPopulation) const
{
   UINT32 l_iType = 0;
   while( (l_iType < c_iNbCityTypes) && (c_iCityTypePoluationMin[l_iType] <= in_iPopulation) )
   {
      ++ l_iType;
   }

   return (l_iType - 1);
}

