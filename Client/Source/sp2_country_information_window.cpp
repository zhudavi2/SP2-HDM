/**************************************************************
*
* sp2_country_information_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_country_information_window.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GColorRGBInt c_BgndColor(20, 20, 20, 255);
const GColorRGBInt c_BorderColor(128, 192, 255, 255);
const GColorRGBInt c_RegionColor(0, 128, 255, 255);
const GColorRGBInt c_FillColor(0, 64, 128, 255);

const GString GCountryInformationWindow::TypeName(L"Country_Information_Window");

GUI::GBaseObject* GCountryInformationWindow::New()
{
	return (GUI::GBaseObject*) new GCountryInformationWindow();
}


GCountryInformationWindow::GCountryInformationWindow() : GBaseWindow()
{
   m_pMapTxtr = NULL;

   m_pMapRender = NULL;

   m_pScene = NULL;
   m_pCamera = NULL;
   m_pCountryModel = NULL;
   m_pCountryMesh = NULL;
   m_pCountryBorderModel = NULL;
   m_pCountryBorderMesh = NULL;
   m_pCountryRegionsModel = NULL;
   m_pCountryRegionsMesh = NULL;
   m_pOverrideRegionColor = NULL;
   m_pOverrideFillColor = NULL;

   m_pImage = NULL;
}

GCountryInformationWindow::~GCountryInformationWindow()
{
}

bool GCountryInformationWindow::OnDestroy()
{
   SAFE_RELEASE(m_pMapTxtr);

   SAFE_RELEASE(m_pMapRender);

   SAFE_RELEASE(m_pScene);
   SAFE_RELEASE(m_pCamera);
   SAFE_RELEASE(m_pCountryModel);
   SAFE_RELEASE(m_pCountryMesh);
   SAFE_RELEASE(m_pCountryBorderModel);
   SAFE_RELEASE(m_pCountryBorderMesh);
   SAFE_RELEASE(m_pCountryRegionsModel);
   SAFE_RELEASE(m_pCountryRegionsMesh);
   SAFE_RELEASE(m_pOverrideRegionColor);
   SAFE_RELEASE(m_pOverrideFillColor);

   GImage2D::FreeObject(m_pImage);

   return __super::OnDestroy();
}

bool GCountryInformationWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm = (GUI::GFrame*)Child(L"frmInner");
   m_pObjColorGradientPic = (GUI::GPictureBox*)Child(L"picBlueGradient");

   m_pObjMap = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmLand")->Child(L"frmMap")->Child(L"frmMap");

   // Setup country map texture renderer
   {
      // Create texture for the frame
      m_pMapTxtr = 
         g_Joshua.Renderer()->TextureManager()->CreateTexture(PIXELFMT_X8R8G8B8, 
                                                            m_pObjMap->Width(),
                                                            m_pObjMap->Height(), 
                                                            true, 
                                                            L"Country Info Map");
      ( (GFX::GWindow*) m_pObjMap->Model()->Get_Primitive() )->SetImage(m_pMapTxtr);

      // Create the texture renderer
      m_pMapRender = g_Joshua.Renderer()->TextureManager()->
         CreateTextureRenderer(PIXELFMT_X8R8G8B8, 
                              m_pObjMap->Width(), 
                              m_pObjMap->Height(), 
                              true, 
                              L"Country Info Renderer");

      // Create the scene that will render the country
      m_pScene                = g_Joshua.Renderer()->Create_Scene();
      m_pCamera               = g_Joshua.Renderer()->Create_Camera();
      m_pCountryModel         = g_Joshua.Renderer()->Create_Model();
      m_pCountryMesh          = g_Joshua.Renderer()->CreateMeshComplex();
      m_pCountryBorderModel   = g_Joshua.Renderer()->Create_Model();
      m_pCountryBorderMesh    = g_Joshua.Renderer()->CreateMeshComplex();
      m_pCountryRegionsModel  = g_Joshua.Renderer()->Create_Model();
      m_pCountryRegionsMesh   = g_Joshua.Renderer()->CreateMeshComplex();
      m_pOverrideRegionColor  = g_Joshua.Renderer()->CreateOverridePSCascade();
      m_pOverrideFillColor    = g_Joshua.Renderer()->CreateOverridePSCascade();

      // Setup the scene hierarchy
      m_pScene->Get_Root_Model()->Set_Primitive(m_pCamera);
      m_pScene->Get_Root_Model()->Add_Child(m_pCountryModel);
      m_pCountryModel->Set_Primitive(m_pCountryMesh);
      m_pScene->Get_Root_Model()->Add_Child(m_pCountryBorderModel);
      m_pCountryBorderModel->Set_Primitive(m_pCountryBorderMesh);
      m_pScene->Get_Root_Model()->Add_Child(m_pCountryRegionsModel);
      m_pCountryRegionsModel->Set_Primitive(m_pCountryRegionsMesh);

      // Setup the scene colors & material
      m_pScene->ClearFrameColor() = GColorRGBReal(c_BgndColor);
      GFX::GMaterial* l_pMat = m_pScene->DefaultMaterial();
      l_pMat->RenderStates(0)->Value(GFX::RSTATE_CULL_MODE) = GFX::CULL_NONE;
      GFX::GDefaultVertexShader* l_pVS = (GFX::GDefaultVertexShader*) l_pMat->VertexShader(0);
      l_pVS->LightEnable() = 0;
      GFX::GCascadePixelShader* l_pPS = (GFX::GCascadePixelShader*) l_pMat->PixelShader(0);
      l_pPS->ColorFactor() = GColorRGBReal(c_BorderColor);
      GFX::GCascadeStage* l_pStage = l_pPS->Stage(0);
      l_pStage->ColorOperation(GFX::OPERATOR_SELECT_ARG_2);
      l_pStage->ColorArgument2(GFX::ARG_COLOR_FACTOR);

      m_pCountryRegionsModel->AddOverride(m_pOverrideRegionColor);
      m_pOverrideRegionColor->UseColorFactorOverride(true);
      m_pOverrideRegionColor->ColorFactorOverride() = GColorRGBReal(c_RegionColor);

      m_pCountryModel->AddOverride(m_pOverrideFillColor);
      m_pOverrideFillColor->UseColorFactorOverride(true);
      m_pOverrideFillColor->ColorFactorOverride() = GColorRGBReal(c_FillColor);

      // Setup the scene camera as orthognal so that coordinates 
      // goes from (0,0) in lower-left corner to (1,1) in upper-right corner
      m_pCamera->View_Type(GFX::VIEW_ORTHOGONAL);
      m_pCamera->Left() = -0.02f;
      m_pCamera->Top() = 1.02f;
      m_pCamera->Right() = 1.02f;
      m_pCamera->Bottom() = -0.02f;
      m_pCamera->Near_Plane() = -1.f;
      m_pCamera->Far_Plane() = 1.f;

      // Associate the scene to the renderer
      m_pMapRender->RenderTree()->AddScene(m_pScene);
      m_pMapRender->RenderFrequency(GFX::TXTRENDER_NEVER);

      m_pImage = GImage2D::AllocObject();
   }

   m_pObjArableMtr         = (SP2::GGradientBar*)m_pObjInnerFrm->Child(L"frmLand")->Child(L"frmArableLand");
   m_pObjForestMtr         = (SP2::GGradientBar*)m_pObjInnerFrm->Child(L"frmLand")->Child(L"frmForests");
   m_pObjParksMtr          = (SP2::GGradientBar*)m_pObjInnerFrm->Child(L"frmLand")->Child(L"frmNationalParks");
   m_pObjUnusableMtr       = (SP2::GGradientBar*)m_pObjInnerFrm->Child(L"frmLand")->Child(L"frmUnusable");

   m_pObjArableLbl         = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmLand")->Child(L"txtArableLand");
   m_pObjForestLbl         = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmLand")->Child(L"txtForests");
   m_pObjParksLbl          = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmLand")->Child(L"txtNationalParks");
   m_pObjUnusableLbl       = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmLand")->Child(L"txtUnusable");

   m_pObjPop15Lbl          = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmPopulation")->Child(L"txt15");
   m_pObjPop1565Lbl        = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmPopulation")->Child(L"txt1565");
   m_pObjPop65Lbl          = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmPopulation")->Child(L"txt65");
   m_pObjPopTotalLbl       = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmPopulation")->Child(L"txtTotal");

   m_pObjPop15RatioLbl     = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmPopulation")->Child(L"txt15Ratio");
   m_pObjPop1565RatioLbl   = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmPopulation")->Child(L"txt1565Ratio");
   m_pObjPop65RatioLbl     = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmPopulation")->Child(L"txt65Ratio");
   m_pObjPopTotalRatioLbl  = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmPopulation")->Child(L"txtTotalRatio");

   m_pObjLandAreaLbl       = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmTerrain")->Child(L"txtLandArea");
   m_pObjWaterAreaLbl      = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmTerrain")->Child(L"txtWaterArea");
   m_pObjTotalAreaLbl      = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmTerrain")->Child(L"txtTotalArea");

   m_pObjBirthRateTxt      = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmPopulation")->Child(L"txtBirthRate");
   m_pObjDeathRateTxt      = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmPopulation")->Child(L"txtDeathRate");
   m_pObjTelecomTxt        = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmHumanDevelopment")->Child(L"txtTelecom");
   m_pObjInfrastructureTxt = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmHumanDevelopment")->Child(L"txtInfrastructure");

   m_pObjClimateFrm        = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmClimate")->Child(L"frmInner");
   m_pObjClimateLbl        = (GUI::GLabel*)m_pObjClimateFrm->Child(L"lblClimate");
   m_pObjClimateScr        = (GUI::GVScrollBar*)m_pObjInnerFrm->Child(L"frmClimate")->Child(L"scrClimate");

   m_pObjCountryLbl        = (GUI::GLabel*)m_pObjInnerFrm->Child(L"frmHumanDevelopment")->Child(L"frmInner")->Child(L"lblCountry");
   m_pObjCountryPic        = (GUI::GPictureBox*)m_pObjInnerFrm->Child(L"frmHumanDevelopment")->Child(L"frmInner")->Child(L"picCountry");
   m_pObjWorldPic          = (GUI::GPictureBox*)m_pObjInnerFrm->Child(L"frmHumanDevelopment")->Child(L"frmInner")->Child(L"picWorld");

   m_pObjArableMtr->RightBound(93);
   m_pObjForestMtr->RightBound(93);  
   m_pObjParksMtr->RightBound(93);
   m_pObjUnusableMtr->RightBound(93);
   
   m_pObjArableMtr->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));
   m_pObjForestMtr->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));
   m_pObjParksMtr->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));
   m_pObjUnusableMtr->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));

   ((GFX::GWindow*)m_pObjClimateFrm->Model()->Get_Primitive())->ClipChildren(true);

   m_pObjApplyBtn->Hide();
   m_pObjLockTog->Hide();

   m_pData = &g_ClientDAL.m_DataCountryInformationWindow;

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GCountryInformationWindow::OnShow()
{
   __super::OnShow();

   Update();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

GUI::EEventStatus::Enum GCountryInformationWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {
            Hide();
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

EEventStatus::Enum GCountryInformationWindow::OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_Data, GUI::GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      if(in_iEventID == ECustomEvents::OnScrollVertical)
      {
         if(in_pCaller == m_pObjClimateScr)
         {
            m_pObjClimateLbl->Top(-(INT32)in_Data.Data);
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

bool GCountryInformationWindow::Update()
{
   if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
   {
      GCountryDataClient* l_pData = &g_ClientDAL.m_PlayerCountryData;
      
      m_pData->m_iCountryID        = g_ClientDAL.ControlledCountryID();
      m_pData->m_fArableLand       = l_pData->ArableLandLevel();
      m_pData->m_fForestLand       = l_pData->ForestLandLevel();
      m_pData->m_fParksLand        = l_pData->ParksLandLevel();
      m_pData->m_fUnusableLand     = l_pData->NotUsedLandLevel();

      m_pData->m_iPop15            = l_pData->Pop15();
      m_pData->m_iPop1565          = l_pData->Pop1565();
      m_pData->m_iPop65            = l_pData->Pop65();

      m_pData->m_fLandArea         = l_pData->AreaLandTotal();
      m_pData->m_fWaterArea        = l_pData->AreaWaterTotal();
      m_pData->m_fTotalArea        = l_pData->AreaTotal();

      m_pData->m_fHumanDev         = l_pData->HumanDevelopment();
      m_pData->m_fHumanDevAverage  = l_pData->HumanDevelopmentAvg();

      m_pData->m_iClimateStid      = l_pData->ClimateNameID();

      m_pData->m_fBirthRate            = l_pData->BirthRate();
      m_pData->m_fDeathRate            = l_pData->DeathRate();
      m_pData->m_fInfrastructure       = l_pData->Infrastructure();
      m_pData->m_fTelecommunications   = l_pData->TelecomLevel();
   }

   m_pObjArableMtr->Value(m_pData->m_fArableLand, 0, false, false);
   m_pObjForestMtr->Value(m_pData->m_fForestLand, 0, false, false);
   m_pObjParksMtr->Value(m_pData->m_fParksLand, 0, false, false);
   m_pObjUnusableMtr->Value(m_pData->m_fUnusableLand, 0, false, false); 

   m_pObjArableLbl->Text( GString::FormatNumber(m_pData->m_fArableLand * 100.f, L"", L".", L"", L" %", 0, 1, false ) );
   m_pObjForestLbl->Text( GString::FormatNumber(m_pData->m_fForestLand * 100.f, L"", L".", L"", L" %", 0, 1, false ) );
   m_pObjParksLbl->Text( GString::FormatNumber(m_pData->m_fParksLand * 100.f, L"", L".", L"", L" %", 0, 1, false ) );
   m_pObjUnusableLbl->Text( GString::FormatNumber(m_pData->m_fUnusableLand * 100.f, L"", L".", L"", L" %", 0, 1, false ) );

   INT64 l_iPopTotal = m_pData->m_iPop15 + m_pData->m_iPop1565 + m_pData->m_iPop65;
   
   m_pObjPop15Lbl->Text( GString::FormatNumber((REAL64)m_pData->m_iPop15, L" ", L".", L"", L"", 3, 0, false) );
   m_pObjPop1565Lbl->Text( GString::FormatNumber((REAL64)m_pData->m_iPop1565, L" ", L".", L"", L"", 3, 0, false) );
   m_pObjPop65Lbl->Text( GString::FormatNumber((REAL64)m_pData->m_iPop65, L" ", L".", L"", L"", 3, 0, false) );
   m_pObjPopTotalLbl->Text( GString::FormatNumber((REAL64)l_iPopTotal, L" ", L".", L"", L"", 3, 0, false) );

   m_pObjPop15RatioLbl->Text( GString::FormatNumber((REAL64)m_pData->m_iPop15 / (REAL64)l_iPopTotal * 100.f, L"", L".", L"", L" %", 0, 1, false) );
   m_pObjPop1565RatioLbl->Text( GString::FormatNumber((REAL64)m_pData->m_iPop1565 / (REAL64)l_iPopTotal * 100.f, L"", L".", L"", L" %", 0, 1, false) );
   m_pObjPop65RatioLbl->Text( GString::FormatNumber((REAL64)m_pData->m_iPop65 / (REAL64)l_iPopTotal * 100.f, L"", L".", L"", L" %", 0, 1, false) );
   m_pObjPopTotalRatioLbl->Text(L"100.0 %");

   m_pObjBirthRateTxt->Text( GString::FormatNumber(m_pData->m_fBirthRate * 100.f, L"", L".", L"", L" %", 0, 1, false ) );  
   m_pObjDeathRateTxt->Text( GString::FormatNumber(m_pData->m_fDeathRate * 100.f, L"", L".", L"", L" %", 0, 1, false ) );  
   m_pObjTelecomTxt->Text( GString::FormatNumber(m_pData->m_fTelecommunications * 100.f, L"", L".", L"", L" %", 0, 1, false ) );
   m_pObjInfrastructureTxt->Text( GString::FormatNumber(m_pData->m_fInfrastructure * 100.f, L"", L".", L"", L" %", 0, 1, false ) );

   m_pObjLandAreaLbl->Text( GString::FormatNumber(m_pData->m_fLandArea, L" ", L"", L"", L"", 3, 0, false ) );
   m_pObjWaterAreaLbl->Text( GString::FormatNumber(m_pData->m_fWaterArea, L" ", L"", L"", L"", 3, 0, false ) );
   m_pObjTotalAreaLbl->Text( GString::FormatNumber(m_pData->m_fTotalArea, L" ", L"", L"", L"", 3, 0, false ) );

   m_pObjClimateLbl->Text( g_ClientDAL.GetString(m_pData->m_iClimateStid) );
   m_pObjClimateLbl->Height( m_pObjClimateLbl->Text2D()->Height() );
   m_pObjClimateScr->Initialize(this, 0, max(m_pObjClimateLbl->Height() - m_pObjClimateFrm->Height(), 0), 1, 45, 0 );

   m_pObjCountryLbl->Text( g_ClientDAL.Country(g_ClientDCL.SelectedCountryID()).Name() );
   ((GUI::GLabel*)m_pObjFrameTitle->Child(L"lblTitle"))->Text(g_ClientDAL.Country(g_ClientDCL.SelectedCountryID()).Name().ToUpper() );
   m_pObjCountryPic->Width( (INT32)( m_pData->m_fHumanDev * (REAL64)m_pObjCountryLbl->Width() ) );
   m_pObjWorldPic->Width( (INT32)( m_pData->m_fHumanDevAverage * (REAL64)m_pObjCountryLbl->Width() ) );

   // Update country content
   {
      set<UINT32> l_RegionCopy = g_ClientDAL.CountryPoliticalControl(m_pData->m_iCountryID);

      if(!l_RegionCopy.empty() )
      {
         // Update the country border mesh
         GShape& l_Shape = m_pCountryBorderMesh->Shape();

         UINT32 l_iStartRegion = g_ClientDAL.CapitalRegion(m_pData->m_iCountryID);
         if(!l_iStartRegion)
         {
            l_iStartRegion = *(l_RegionCopy.begin() );
         }

         const GRegionConnectGraph& l_Graph = g_ClientDAL.RegionGraph();

         vector<Map::ID> l_vVisiting(1, l_iStartRegion);
         hash_set<Map::ID> l_Visited;
         while(!l_vVisiting.empty() )
         {
            Map::ID l_Cur = l_vVisiting.back();
            l_vVisiting.pop_back();
            l_Visited.insert(l_Cur);

            for(UINT32 n = 0;n < l_Graph.Graph() [l_Cur].m_iNbNeighbors;n ++)
            {
               Map::ID l_ToVisit = l_Graph.Graph() [l_Cur].m_pNeighborIDs[n];
               if( (l_ToVisit != 0) && (l_RegionCopy.count(l_ToVisit) > 0) && (l_Visited.count(l_ToVisit) == 0) )
               {
                  l_vVisiting.push_back(l_ToVisit);
               }
            }
         }

         vector<Map::ID> l_vMapRegions(l_Visited.begin(), l_Visited.end() );
         Map::GBox l_BBox;

         UINT32 l_iModificationCount = 1;
         while( (l_iModificationCount > 0) && (l_RegionCopy.size() != l_vMapRegions.size() ) )
         {
            l_iModificationCount = 0;
            l_BBox = g_ClientDAL.CountryView()->ComputeBoundingBox(l_vMapRegions);

            //! \TODO TBM : could be optimized by keeping a list of non visited regions (Reverse of l_Visited)

            // Try to include all regions that fit into that bounding box
            for(set<UINT32>::const_iterator it = l_RegionCopy.begin();
               it != l_RegionCopy.end();it ++)
            {
               // Do not include regions that already has been inserted
               if(l_Visited.count(*it) == 0)
               {
                  const Map::GBox l_CurrentBBox = g_ClientDAL.CountryView()->ComputeBoundingBox(*it);
                  if(l_BBox.Contains(l_CurrentBBox.LowerLeftCorner() ) ||
                     l_BBox.Contains(l_CurrentBBox.LowerRightCorner() ) ||
                     l_BBox.Contains(l_CurrentBBox.UpperLeftCorner() ) ||
                     l_BBox.Contains(l_CurrentBBox.UpperRightCorner() ) )
                  {
                     l_vMapRegions.push_back(*it);
                     l_Visited.insert(*it);
                     l_iModificationCount ++;
                  }
               }
            }
         }

         // Re-fetch BBox since it could have changed
         l_BBox = g_ClientDAL.CountryView()->ComputeBoundingBox(l_vMapRegions);

         GMultiFacetList l_ExternalContours, l_InternalContours;
         g_ClientDAL.CountryView()->BuildContours( 
            l_vMapRegions, 
            l_Shape, 
            l_ExternalContours, 
            l_InternalContours);

         REAL32 l_fCenterX = (l_BBox.Xmax + l_BBox.Xmin) * 0.5f;

         // Detect if regions cross the -180/180 longitude boundary
         if(l_BBox.Width() > 270.f)
         {
            REAL32 l_fMinBoxX = FLT_MAX;
            REAL32 l_fMaxBoxX = -FLT_MAX;

            // Adjust all points 
            for(UINT32 i = 0;i < l_Shape.NbPositions();i ++)
            {
               GVector3D<REAL32>& l_Pos = l_Shape.Position(i);
               if(l_Pos.x < -90.f)
               {
                  l_Pos.x += 360.f;
               }

               l_fMinBoxX = min(l_Pos.x, l_fMinBoxX);
               l_fMaxBoxX = max(l_Pos.x, l_fMaxBoxX);
            }

            // Recompute center
            l_fCenterX = (l_fMaxBoxX + l_fMinBoxX) * 0.5f;
         }

         REAL32 l_fMinX = FLT_MAX;
         REAL32 l_fMaxX = -FLT_MAX;
         for(UINT32 i = 0;i < l_Shape.NbPositions();i ++)
         {
            GVector3D<REAL32>& l_Pos = l_Shape.Position(i);
            l_Pos.x -= l_fCenterX;
            l_Pos.x *= cosf(l_Pos.y * (REAL32) Math::PI / 360.f);//180.f);

            l_fMinX = min(l_Pos.x, l_fMinX);
            l_fMaxX = max(l_Pos.x, l_fMaxX);
         }

         GVector3D<REAL32> l_Offset(-l_fMinX, -l_BBox.Ymin, 0);
         REAL32 l_fScale = 0;
         if( (l_fMaxX - l_fMinX) > (l_BBox.Ymax - l_BBox.Ymin) )
         {
            l_fScale = 1.f / (l_fMaxX - l_fMinX);
            l_Offset.y += ( (l_fMaxX - l_fMinX) - (l_BBox.Ymax - l_BBox.Ymin) ) * 0.5f;
         }
         else
         {
            l_fScale = 1.f / (l_BBox.Ymax - l_BBox.Ymin);
            l_Offset.x += ( (l_BBox.Ymax - l_BBox.Ymin) - (l_fMaxX - l_fMinX) ) * 0.5f;
         }

         for(UINT32 i = 0;i < l_Shape.NbPositions();i ++)
         {
            GVector3D<REAL32>& l_Pos = l_Shape.Position(i);
            l_Pos += l_Offset;
            l_Pos *= l_fScale;
         }

         m_pCountryBorderMesh->NbFacetGroups(0);
         m_pCountryBorderMesh->NbFacetGroups(l_ExternalContours.NbFacetLists() );

         for(UINT32 i = 0;i < l_ExternalContours.NbFacetLists();i ++)
         {
            m_pCountryBorderMesh->FacetGroup(i).Facets() = l_ExternalContours.Facet_List(i);
         }

         m_pCountryRegionsMesh->Shape() = l_Shape;
         m_pCountryRegionsMesh->NbFacetGroups(0);
         m_pCountryRegionsMesh->NbFacetGroups(l_InternalContours.NbFacetLists() );

         for(UINT32 i = 0;i < l_InternalContours.NbFacetLists();i ++)
         {
            m_pCountryRegionsMesh->FacetGroup(i).Facets() = l_InternalContours.Facet_List(i);
         }

         // Update the country fill mesh
         GShape& l_FillShape = m_pCountryMesh->Shape();
         
         GFaceList<GPolygon> l_CountryFaces;
         g_ClientDAL.CountryView()->BuildFaceList(
            l_vMapRegions, 
            l_FillShape, 
            l_CountryFaces);

         m_pCountryMesh->NbFacetGroups(0);
         m_pCountryMesh->NbFacetGroups(l_CountryFaces.NbFaces() );
         for(UINT32 i = 0;i < l_CountryFaces.NbFaces();i ++)
         {
            l_CountryFaces.Face(i).ConvertToTriangleList(
               l_FillShape, 
               m_pCountryMesh->FacetGroup(i).Facets().TriangleListIndexed() );
         }

         if(l_BBox.Width() > 270.f)
         {
            // Adjust all points 
            for(UINT32 i = 0;i < l_FillShape.NbPositions();i ++)
            {
               GVector3D<REAL32>& l_Pos = l_FillShape.Position(i);
               if(l_Pos.x < -90.f)
               {
                  l_Pos.x += 360.f;
               }
            }
         }

         for(UINT32 i = 0;i < l_FillShape.NbPositions();i ++)
         {
            GVector3D<REAL32>& l_Pos = l_FillShape.Position(i);
            l_Pos.x -= l_fCenterX;
            l_Pos.x *= cosf(l_Pos.y * (REAL32) Math::PI / 360.f);//180.f);

            l_Pos += l_Offset;
            l_Pos *= l_fScale;
         }

         // Render the country scene
         m_pMapRender->RenderFrequency(GFX::TXTRENDER_ONCE);
         g_Joshua.Renderer()->TextureManager()->UpdateTextureRenderer(m_pMapRender);

         m_pMapRender->LockImage(m_pImage);
         m_pMapTxtr->SetImage(*m_pImage);
         m_pMapRender->Unlock();
      }
   }

   return true;
}
