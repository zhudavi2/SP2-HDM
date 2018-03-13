/**************************************************************
*
* sp2_battle_overview_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"


#include "../include/sp2_battle_overview_window.h"

#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_battle_overview_small_unit_info_window.h"
#include "../include/sp2_unit_view_scene.h"
#include "../include/sp2_battle_summary_window.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

#include "../include/sp2_sound.h"


const GString GBattleOverviewWindow::TypeName(L"Battle_Window");


hash_map<UINT32, GBattleOverviewUnit *>   GBattleScene::m_AllyUnits;
hash_map<UINT32, GBattleOverviewUnit *>   GBattleScene::m_EnnemyUnits;


GBattleMap::GBattleMap()
{

   m_pSelectedUnit = NULL;
}

GBattleMap::~GBattleMap()
{

}
void GBattleMap::ClearPosition(bool in_bClearAlly)
{
   if(in_bClearAlly)
      m_AllyPosition.clear();
   else
      m_EnnemyPosition.clear();
}


void GBattleMap::OnCreate(CGFX_Model_Itf* in_pParentModel)
{
   gassert(in_pParentModel != NULL,"Parent pointer is null");

   // Attach ourself to this parent
   in_pParentModel->Add_Child(m_pModel);
   in_pParentModel->Add_Child(m_FrontLine.Model());

   CGFX_Transform_2D_Itf*  l_pParentTransform = (CGFX_Transform_2D_Itf*)m_pModel->Get_Parent()->Get_Transform();

   // Create Ally Side
   GPixelA8R8G8B8 c_AllyColorStart     = {34, 98,  63,   204};
   GPixelA8R8G8B8 c_EnnemyColorStart   = {33, 54,  89,   204};
   GPixelA8R8G8B8 c_AllyColorEnd       = {26, 69,  47,   204};
   GPixelA8R8G8B8 c_EnnemyColorEnd     = {32, 45,  67,   204};

   GImage2D*      l_pTestImage  = GImage2D::AllocObject();
   GVector2D<UINT32> l_ImageDimension;
   l_ImageDimension.x = 1;
   l_ImageDimension.y = 128 ;
   l_pTestImage->Create(l_ImageDimension, PIXELFMT_A8R8G8B8);

   for(UINT32 i=0;i<l_ImageDimension.y;i++)
   {
      GVector2D<UINT32> l_position;
      l_position.x = 0;
      l_position.y = i;

      GColorRGBInt l_GradientColor;
      l_GradientColor.A = (UINT8)(c_AllyColorStart.A + (c_AllyColorEnd.A - c_AllyColorStart.A) * i/l_ImageDimension.y * 1.0f);
      l_GradientColor.R = (UINT8)(c_AllyColorStart.R + (c_AllyColorEnd.R - c_AllyColorStart.R) * i/l_ImageDimension.y * 1.0f);
      l_GradientColor.G = (UINT8)(c_AllyColorStart.G + (c_AllyColorEnd.G - c_AllyColorStart.G) * i/l_ImageDimension.y * 1.0f);
      l_GradientColor.B = (UINT8)(c_AllyColorStart.B + (c_AllyColorEnd.B - c_AllyColorStart.B) * i/l_ImageDimension.y * 1.0f);

      l_pTestImage->WriteColor(l_position,l_GradientColor);
   }

   CGFX_Texture_Itf* l_pResultTxtr = g_Joshua.Renderer()->TextureManager()->Create_Texture(*l_pTestImage, false, GString(L"BattleOverviewTexture") + GString(g_Joshua.TimeCounter().GetTime() ) );
   m_AllyTerritory.SetTexture(l_pResultTxtr);
   SAFE_RELEASE(l_pResultTxtr);
   // Create Ennemy Side
     for(UINT32 i=0;i<l_ImageDimension.y;i++)
   {
      GVector2D<UINT32> l_position;
      l_position.x = 0;
      l_position.y = i;

      GColorRGBInt l_GradientColor;
      l_GradientColor.A = (UINT8)(c_EnnemyColorStart.A + (c_EnnemyColorEnd.A - c_EnnemyColorStart.A) * i/l_ImageDimension.y * 1.0f);
      l_GradientColor.R = (UINT8)(c_EnnemyColorStart.R + (c_EnnemyColorEnd.R - c_EnnemyColorStart.R) * i/l_ImageDimension.y * 1.0f);
      l_GradientColor.G = (UINT8)(c_EnnemyColorStart.G + (c_EnnemyColorEnd.G - c_EnnemyColorStart.G) * i/l_ImageDimension.y * 1.0f);
      l_GradientColor.B = (UINT8)(c_EnnemyColorStart.B + (c_EnnemyColorEnd.B - c_EnnemyColorStart.B) * i/l_ImageDimension.y * 1.0f);

      l_pTestImage->WriteColor(l_position,l_GradientColor);
   }

   l_pResultTxtr = g_Joshua.Renderer()->TextureManager()->Create_Texture(*l_pTestImage, false, GString(L"BattleOverviewTexture") + GString(g_Joshua.TimeCounter().GetTime() ) );
   m_EnnemyTerritory.SetTexture(l_pResultTxtr);
   SAFE_RELEASE(l_pResultTxtr);

   // Create Dynamic Map
   m_pUnitPositionLayer = GImage2D::AllocObject();

   l_ImageDimension.x = (UINT32)(l_pParentTransform->Size().X);
   l_ImageDimension.y = (UINT32)(l_pParentTransform->Size().Y);
   m_pUnitPositionLayer->Create(l_ImageDimension, PIXELFMT_A8R8G8B8);
   GPixelA8R8G8B8 c_ClearColor   = {0, 0, 0, 0};
   m_pUnitPositionLayer->Clear(&c_ClearColor);
   l_pResultTxtr = g_Joshua.Renderer()->TextureManager()->Create_Texture(*m_pUnitPositionLayer, false, GString(L"BattleOverviewTexture") + GString(g_Joshua.TimeCounter().GetTime() ) );
   m_DynamicMap.SetTexture(l_pResultTxtr);
   SAFE_RELEASE(l_pResultTxtr);

   m_pModel->Add_Child(m_AllyTerritory.Model());
   m_pModel->Add_Child(m_EnnemyTerritory.Model());
   m_pModel->Add_Child(m_DynamicMap.Model());

   SetFrontLine(0.50f);

   // Set few attributes to that material
   m_EnnemyTerritory.m_pMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_DESTCOLOR;
   m_EnnemyTerritory.m_pMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_ZERO;
   
   m_AllyTerritory.m_pMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_SOURCE_BLEND) = GFX::BLEND_DESTCOLOR;
   m_AllyTerritory.m_pMaterial->RenderStates(0)->Value(GFX::RSTATE_ALPHA_DEST_BLEND) = GFX::BLEND_ZERO;

   GImage2D::FreeObject(l_pTestImage);

   // Create front line
   m_FrontLine.LoadTexture("data\\Combat_Overview\\UI\\FrontLine.tga");
   m_FrontLine.SetSize(2,l_pParentTransform->Size().Y);
   ((GFX::GCascadePixelShader*)m_FrontLine.m_pMaterial->PixelShader(0))->ColorFactor() = GColorRGBReal(0.2f, 0.2f, 0.2f, 0.9f);

}

void GBattleMap::SetFrontLine(float in_fProgress)
{
   gassert(in_fProgress >= 0 && in_fProgress<=1,"Progress should be between zero and 1");

   CGFX_Transform_2D_Itf*  m_pParentTransform = (CGFX_Transform_2D_Itf* )m_pModel->Get_Parent()->Get_Transform();

   m_AllyTerritory.SetSize(m_pParentTransform->Size().X * in_fProgress,m_pParentTransform->Size().Y);
   m_EnnemyTerritory.SetSize(m_pParentTransform->Size().X * (1 - in_fProgress) + 1,m_pParentTransform->Size().Y);
   m_EnnemyTerritory.SetPosition(m_pParentTransform->Size().X * in_fProgress ,0);
   m_FrontLine.SetPosition(m_pParentTransform->Size().X * in_fProgress - 1,0);
}

void GBattleMap::Select(GBattleOverviewUnit* in_pSelectedUnit)
{
   m_pSelectedUnit = in_pSelectedUnit;
}
   
void GBattleMap::Unselect(void)
{
   m_pSelectedUnit = NULL;
}

void GBattleMap::DrawSelection(void)
{
   if(m_pSelectedUnit)
   {
      GVector2D<UINT32> l_UnitPosition;
      GColorRGBInt l_SelectionColor;
      if(m_pSelectedUnit->Side() == EBattleOverviewUnitSide::Ally)
      {   
         l_UnitPosition = m_AllyPosition[m_pSelectedUnit->TypeID()];
         l_SelectionColor = GColorRGBInt(135, 218, 119, 255);         
      }
      else
      { 
         l_UnitPosition = m_EnnemyPosition[m_pSelectedUnit->TypeID()];
         l_SelectionColor = GColorRGBInt(227, 134, 139, 255);   
      }
   
      // Draw a little square
      for(UINT i=0;i<7;i++)
      {
         GVector2D<UINT32> l_DrawPosition;
         // Top 
         l_DrawPosition = l_UnitPosition;
         l_DrawPosition.x += i - 3;
         l_DrawPosition.y -= 3;
         m_pUnitPositionLayer->WriteColor(l_DrawPosition,l_SelectionColor);
         // Bottom 
         l_DrawPosition.y += 6;
         m_pUnitPositionLayer->WriteColor(l_DrawPosition,l_SelectionColor);

         // Left
         l_DrawPosition = l_UnitPosition;
         l_DrawPosition.y += i - 3;
         l_DrawPosition.x -= 3;
         m_pUnitPositionLayer->WriteColor(l_DrawPosition,l_SelectionColor);
         // right 
         l_DrawPosition.x += 6;
         m_pUnitPositionLayer->WriteColor(l_DrawPosition,l_SelectionColor);

      }
   }
}


void GBattleMap::DrawUnit(GVector2D<UINT32> in_Position, GColorRGBInt &in_UnitColor)
{
   m_pUnitPositionLayer->WriteColor(in_Position,in_UnitColor);
   in_Position.y -=1;
   in_Position.x -=1;
   m_pUnitPositionLayer->WriteColor(in_Position,in_UnitColor);
   in_Position.x +=1;
   m_pUnitPositionLayer->WriteColor(in_Position,in_UnitColor);
   in_Position.x +=1;
   m_pUnitPositionLayer->WriteColor(in_Position,in_UnitColor);
   in_Position.y +=1;
   m_pUnitPositionLayer->WriteColor(in_Position,in_UnitColor);
   in_Position.y +=1;
   m_pUnitPositionLayer->WriteColor(in_Position,in_UnitColor);
   in_Position.x -=1;
   m_pUnitPositionLayer->WriteColor(in_Position,in_UnitColor);
   in_Position.x -=1;
   m_pUnitPositionLayer->WriteColor(in_Position,in_UnitColor);
   in_Position.y -=1;
   m_pUnitPositionLayer->WriteColor(in_Position,in_UnitColor);
}


void GBattleMap::DrawUnitsOnMap(void)
{
   // Clear Map
   m_DynamicMap.m_pTexture->LockImage(m_pUnitPositionLayer);

   GPixelA8R8G8B8 c_ClearColor   = {0, 0, 0, 0};
   m_pUnitPositionLayer->Clear(&c_ClearColor);

   // Set unit color
   GColorRGBInt l_UnitColor;
   l_UnitColor = GColorRGBInt(255,255,255,200);


   // Draw Allies
   hash_map<UINT32,GVector2D<UINT32> >::iterator l_AllyIterator = m_AllyPosition.begin();
   while(l_AllyIterator != m_AllyPosition.end())
   {
      DrawUnit(l_AllyIterator->second,l_UnitColor);
      l_AllyIterator++;
   }

   // Draw Enemies
   hash_map<UINT32,GVector2D<UINT32> >::iterator l_EnemyIterator = m_EnnemyPosition.begin();
   while(l_EnemyIterator != m_EnnemyPosition.end())
   {
      DrawUnit(l_EnemyIterator->second,l_UnitColor);
      l_EnemyIterator++;
   }
    
   m_DynamicMap.m_pTexture->Unlock();

   if(m_pSelectedUnit)
   {
      DrawSelection();
   }

}

void GBattleMap::CreateBattleField(GBattleInformation *in_pBattleInformation)
{
   gassert(in_pBattleInformation,"Invalid null pointer");


   ClearPosition(true);
   ClearPosition(false);

   // Are we attacking or defending ?
   bool l_bSwap = false;
   if(in_pBattleInformation->m_eCombatSide == ECombatSide::Attacker)
      l_bSwap = true;


   CGFX_Transform_2D_Itf*  l_pParentTransform = (CGFX_Transform_2D_Itf* )m_pModel->Get_Parent()->Get_Transform();

   UINT32 l_iMaxAllyUnitPost = 0;
   UINT32 l_iMinEnnmyUnitPost =  (UINT32) l_pParentTransform->Size().X;


   // Go through all the actors
   hash_map<UINT32,GActorInfo>::const_iterator l_ActorIterator = in_pBattleInformation->m_pArenaInfo->Actors().begin();
   while(l_ActorIterator != in_pBattleInformation->m_pArenaInfo->Actors().end())
   {
      GActorInfo l_ActorInfo = l_ActorIterator->second;

      //For every unit type for the Actor
      vector<GUnitTypeCbtInfo>::const_iterator l_UnitTypeCbtInfoIt = l_ActorIterator->second.m_vUnitTypeInfo.begin();
      while(l_UnitTypeCbtInfoIt != l_ActorIterator->second.m_vUnitTypeInfo.end())
      {
         
         INT32 l_iUnitType = l_UnitTypeCbtInfoIt-l_ActorIterator->second.m_vUnitTypeInfo.begin();

         //If there are units
         if((*l_UnitTypeCbtInfoIt).m_iUnitQty > 0 )
         {
            REAL32 l_fPosition = (*l_UnitTypeCbtInfoIt).m_fPosition;
            if(l_bSwap)
               l_fPosition = 100 - l_fPosition;

            GVector2D<UINT32> l_Position;

            l_Position.x = (UINT32)(l_fPosition/100.0f * (l_pParentTransform->Size().X - 16.0f) + 8.0f);
            l_Position.y = (l_iUnitType + 1) * 7;
            

            // Insert unit type position
            if(in_pBattleInformation->m_eCombatSide == l_ActorInfo.m_Side)
            {
               l_Position.x -= 4;
 
               m_AllyPosition[l_iUnitType] = l_Position;
               if(l_iMaxAllyUnitPost < l_Position.x)
                  l_iMaxAllyUnitPost = l_Position.x;
            }
            else
            {
               l_Position.x += 4;

               m_EnnemyPosition[l_iUnitType] = l_Position;
               if(l_iMinEnnmyUnitPost > l_Position.x)
                  l_iMinEnnmyUnitPost = l_Position.x;

            }

      

         }
         l_UnitTypeCbtInfoIt++;
      }
      l_ActorIterator++;      
  }

   // Update FrontLine
   SetFrontLine(((l_iMinEnnmyUnitPost +l_iMaxAllyUnitPost))/ (2.0f * l_pParentTransform->Size().X));

}

bool GBattleMap::OnMouseClick(const GVector2D<UINT32> in_MousePosition,UINT32 &out_iUnitType,bool &out_bAlly)
{
   // Go through all the unit list and make sure we didn't click anything
   // Iterate Ally Unit
   hash_map<UINT32,GVector2D<UINT32> >::iterator l_UnitPositionIterator = m_AllyPosition.begin();
   while(l_UnitPositionIterator!= m_AllyPosition.end())
   {
      GVector2D<UINT32> l_UnitPosition = l_UnitPositionIterator->second;
      if(in_MousePosition.x >= l_UnitPosition.x-3 && in_MousePosition.x <= l_UnitPosition.x+3  &&
         in_MousePosition.y >= l_UnitPosition.y-3&& in_MousePosition.y <= l_UnitPosition.y+3)
      {
         out_iUnitType =  l_UnitPositionIterator->first;
         out_bAlly = true;
         return true;
      }
      l_UnitPositionIterator++;
   }

   // Do the same for Ennemy.
   l_UnitPositionIterator = m_EnnemyPosition.begin();
   while(l_UnitPositionIterator!= m_EnnemyPosition.end())
   {
      GVector2D<UINT32> l_UnitPosition = l_UnitPositionIterator->second;
      if(in_MousePosition.x >= l_UnitPosition.x-1 && in_MousePosition.x <= l_UnitPosition.x+1  &&
         in_MousePosition.y >= l_UnitPosition.y-1 && in_MousePosition.y <= l_UnitPosition.y+1)
      {
         out_iUnitType = l_UnitPositionIterator->first;
         out_bAlly = false;
         return true;
      }
      l_UnitPositionIterator++;
   }

   return false;
}


GBattleScene::GBattleScene()
{
   m_pConfigFile  = NULL;
   m_pSceneNode   = NULL;
}

GBattleScene::~GBattleScene()
{
   // Clear confing file
   SAFE_DELETE(m_pConfigFile);
}




void GBattleScene::ReadConfigFile(const GString &in_sXMLFileName, const GString &in_ThemeName)
{
   GFile          l_XMLFile;
   GString        l_sTempName;
   GXMLParser		l_XmlParser;

   //Request the file to the file manager
   if(!g_Joshua.FileManager()->File(in_sXMLFileName,l_XMLFile))
   {
      g_Joshua.Log(SP2::ERROR_CANT_FIND_FILE + in_sXMLFileName,MSGTYPE_FATAL_ERROR);
   } 
   //Extract the file to a temp file on the disk so it can be parsed (xml)
   if(!l_XMLFile.Extract(l_sTempName))
   {
      g_Joshua.Log(SP2::ERROR_EXTRACT_FILE + l_XMLFile.Name(),MSGTYPE_FATAL_ERROR);
   }
   m_pConfigFile = l_XmlParser.Parse(l_sTempName);
   l_XMLFile.Unextract(); //free the temp file
   if(!m_pConfigFile)
   {
      g_Joshua.Log(SP2::ERROR_PARSING_XML_FILE + l_XMLFile.Name(),MSGTYPE_FATAL_ERROR);
   }

   gassert(m_pConfigFile,"Config file is not valid or couldn't be read");

   // Read Informations
   //Get the Scene node 
   CXML_Util l_XMLUtil;
   m_pSceneNode = l_XMLUtil.Search_Node(m_pConfigFile->Root(),in_ThemeName,GString(""),NULL,4);
   gassert(m_pSceneNode,"Invalid Theme");
}

GTree<GXMLNode>* GBattleScene::GetConfigFile(void)
{
   return m_pConfigFile;
}

GString& GBattleScene::GetSceneImageFileName(void)
{
   CXML_Util l_XMLUtil;

   // All units position
   CTreeXMLNode* l_pSceneImageFileName= l_XMLUtil.Search_Node(m_pSceneNode,"SceneImage",L"",NULL,1);   
   gassert(l_pSceneImageFileName,"Node couldn't be found");

   return l_pSceneImageFileName->Data().m_value;
}

GString& GBattleScene::GetMapImageFileName(void)
{
   CXML_Util l_XMLUtil;

   // All units position
   CTreeXMLNode* l_pMapImageFileName= l_XMLUtil.Search_Node(m_pSceneNode,"MapImage",L"",NULL,1);   
   gassert(l_pMapImageFileName,"Node couldn't be found");

   return l_pMapImageFileName->Data().m_value;
}


void GBattleScene::LoadAnimations(void)
{
   for(UINT32 i=0;i<EUnitType::AircraftCarrier;i++)
   {
      GUnitType* l_pUnitType = g_ClientDAL.UnitTypes()[i];

      // Create a new battle unit
      GBattleOverviewUnit * l_pUnit = new GBattleOverviewUnit(l_pUnitType, EBattleOverviewUnitSide::Ally, "Animation");
      m_AllyUnits[i] = l_pUnit;
      m_AllyUnits[i]->Model()->Visibility() = false;

      l_pUnit = new GBattleOverviewUnit(l_pUnitType, EBattleOverviewUnitSide::Enemy,"Animation");
      m_EnnemyUnits[i] = l_pUnit;
      m_EnnemyUnits[i]->Model()->Visibility() = false;
   }
}

void GBattleScene::ClearAnimations(void)
{
   for(UINT32 i=0;i<EUnitType::AircraftCarrier;i++)
   {
      delete m_AllyUnits[i];
      delete m_EnnemyUnits[i];
   }
}

void GBattleScene::SetUnitPosition(void)
{
   gassert(m_pConfigFile,"Config file hasn't been read");

   CXML_Util l_XMLUtil;

   // All units position
   CTreeXMLNode* l_pUnitsPositionNode= l_XMLUtil.Search_Node(m_pSceneNode,"UnitsPosition",L"",NULL,1);   
   gassert(l_pUnitsPositionNode,"Node couldn't be found");

   // Ally units position
   CTreeXMLNode* l_pAllyUnitPositionNode= l_XMLUtil.Search_Node(l_pUnitsPositionNode,"Ally",L"",NULL,1);   
   gassert(l_pAllyUnitPositionNode,"Node couldn't be found");

   // Create Ally units
   for(UINT32 i=0;i<EUnitType::AircraftCarrier;i++)
   {
      
      CTreeXMLNode* l_pUnitNode = l_XMLUtil.Search_Node(l_pAllyUnitPositionNode,c_UnitTypeXMLName[i],L"",NULL,1);   
      if(l_pUnitNode)
      {

         CTreeXMLNode* l_pUnitNodePositionX = l_XMLUtil.Search_Node(l_pUnitNode,"X",L"",NULL,1);   
         gassert(l_pUnitNodePositionX,"Node couldn't be found");

         CTreeXMLNode* l_pUnitNodePositionY = l_XMLUtil.Search_Node(l_pUnitNode,"Y",L"",NULL,1);   
         gassert(l_pUnitNodePositionY,"Node couldn't be found");
 
         REAL32 X = l_pUnitNodePositionX->Data().m_value.ToREAL32();
         REAL32 Y = l_pUnitNodePositionY->Data().m_value.ToREAL32();

         // Set its default position
         m_AllyUnits[i]->SetPosition(X,Y);
      }
      m_AllyUnits[i]->Model()->Visibility() = false;
      m_AllyUnits[i]->Deactivate();
   } 

   // Enemy units position
   CTreeXMLNode* l_pEnemyUnitPositionNode= l_XMLUtil.Search_Node(l_pUnitsPositionNode,"Enemy",L"",NULL,1);   
   gassert(l_pEnemyUnitPositionNode,"Node couldn't be found");

   // Create Enemy units
   for(UINT32 i=0;i<EUnitType::AircraftCarrier;i++)
   {
       
      CTreeXMLNode* l_pUnitNode = l_XMLUtil.Search_Node(l_pEnemyUnitPositionNode,c_UnitTypeXMLName[i],L"",NULL,1);   
      if(l_pUnitNode)
      {

         CTreeXMLNode* l_pUnitNodePositionX = l_XMLUtil.Search_Node(l_pUnitNode,"X",L"",NULL,1);   
         gassert(l_pUnitNodePositionX,"Node couldn't be found");

         CTreeXMLNode* l_pUnitNodePositionY = l_XMLUtil.Search_Node(l_pUnitNode,"Y",L"",NULL,1);   
         gassert(l_pUnitNodePositionY,"Node couldn't be found");

         REAL32 X = l_pUnitNodePositionX->Data().m_value.ToREAL32();
         REAL32 Y = l_pUnitNodePositionY->Data().m_value.ToREAL32();

         m_EnnemyUnits[i]->SetPosition(X,Y);
      }
      m_EnnemyUnits[i]->Model()->Visibility() = false;
      m_EnnemyUnits[i]->Deactivate();

   }
}


void GBattleScene::AttachModel(CGFX_Model_Itf* in_pModelToAttachTo)
{
   for(UINT32 i=0;i<EUnitType::AircraftCarrier;i++)
   {
      if(m_AllyUnits[i])
      {
          in_pModelToAttachTo->Add_Child(m_AllyUnits[i]->Model());
          m_AllyUnits[i]->Reset();
      }
      if(m_EnnemyUnits[i])
      {
         in_pModelToAttachTo->Add_Child(m_EnnemyUnits[i]->Model());
         m_EnnemyUnits[i]->Reset();
      }
   }
}

void GBattleScene::CreateBattleField(GBattleInformation *in_pBattleInformation)
{
   gassert(in_pBattleInformation,"Invalid null pointer");

   // Clean all units groups total
   for(UINT32 i=0;i<EUnitType::AircraftCarrier;i++)
   {
      if(m_AllyUnits[i])
         m_AllyUnits[i]->TotalUnitHP(0);
      if(m_EnnemyUnits[i])
         m_EnnemyUnits[i]->TotalUnitHP(0);
   }


   // Gather All the actors
   hash_map<UINT32,GActorInfo>::const_iterator l_ActorIterator = in_pBattleInformation->m_pArenaInfo->Actors().begin();
   while(l_ActorIterator != in_pBattleInformation->m_pArenaInfo->Actors().end())
   {
      GActorInfo l_ActorInfo = l_ActorIterator->second;
      hash_map<UINT32, GBattleOverviewUnit *> *l_pUnitList; 
      UINT32 l_iSelectedCountry;


      //Ge the side of the actor
      if(l_ActorInfo.m_Side == in_pBattleInformation->m_eCombatSide)
      {
         l_pUnitList = &m_AllyUnits;
         l_iSelectedCountry = in_pBattleInformation->m_iSelectedAllyCountry;
      }
      else
      {
         l_pUnitList = &m_EnnemyUnits;
         l_iSelectedCountry = in_pBattleInformation->m_iSelectedEnnemyCountry;
      } 

      //For every unit type for the Actor
      vector<GUnitTypeCbtInfo>::const_iterator l_UnitTypeCbtInfoIt = l_ActorIterator->second.m_vUnitTypeInfo.begin();
      while(l_UnitTypeCbtInfoIt != l_ActorIterator->second.m_vUnitTypeInfo.end())
      {
         UINT32 l_iUnitTypeID = l_UnitTypeCbtInfoIt-l_ActorIterator->second.m_vUnitTypeInfo.begin();
         GBattleOverviewUnit * l_pUnit = (*l_pUnitList)[l_iUnitTypeID];
         //gassert(l_pUnit,"No battle unit was defined for this kind-type of unit");
         if(l_pUnit)
         {
            UINT32 l_iUnitQty = (*l_UnitTypeCbtInfoIt).m_iUnitQty;
            EUnitAction::Enum l_eUnitAction = (*l_UnitTypeCbtInfoIt).m_eAction;
            //If there are units
               
            if(l_pUnit->Activated() == false)
            {
               if(l_iUnitQty > 0 )
               {
                  // Unit Just enter arena!!
                  // Activate This unit with proper value
                  l_pUnit->Activate();
                  if(l_ActorInfo.m_iCountryID == l_iSelectedCountry)
                  {
                     l_pUnit->UnitHP(l_iUnitQty);
                  
                  }
                  l_pUnit->TotalUnitHP(l_iUnitQty);
                  l_pUnit->AccumulatedHit(0);

                  l_pUnit->Status((GBattleOverviewUnit::UnitStatus)l_eUnitAction);
               }
            }
            else
            {
               // Look for our country
               if(l_ActorInfo.m_iCountryID == l_iSelectedCountry)
               {
                  // Its either a reinforcement or an update 
                  if(l_iUnitQty > l_pUnit->UnitHP() )
                  {
                     // Reinforcement, update qty and keeps the accumulated damage
                     l_pUnit->UnitHP(l_iUnitQty + l_pUnit->AccumulatedHit());
 
                  }
                  else
                  {
                     // Its an udapte and we have suffer some damage
                     l_pUnit->AccumulatedHit(l_pUnit->UnitHP() - l_iUnitQty);
                  }
               }
               else
               {
                  l_pUnit->Status((GBattleOverviewUnit::UnitStatus)l_eUnitAction);
               }

               // Update Group Total (already there  + new units)
               l_pUnit->TotalUnitHP(l_iUnitQty + l_pUnit->TotalUnitHP());                     
            }
         }

         l_UnitTypeCbtInfoIt++;
      }
      l_ActorIterator++;      
  }

   // Add up accumulated hits
   for(UINT32 i=0;i<EUnitType::TotalCount-2;i++)
   {
      // Display or not total Unit HP
      bool l_bHideTotalUnitHP = in_pBattleInformation->m_pArenaInfo->Actors().size() > 2;

      if(m_AllyUnits[i])
      {
         m_AllyUnits[i]->TotalUnitHP( m_AllyUnits[i]->TotalUnitHP() + m_AllyUnits[i]->AccumulatedHit());
         m_AllyUnits[i]->HideTotalHP(l_bHideTotalUnitHP);
      }
      if(m_EnnemyUnits[i])
      {
         m_EnnemyUnits[i]->TotalUnitHP(m_EnnemyUnits[i]->TotalUnitHP() + m_EnnemyUnits[i]->AccumulatedHit());
         m_EnnemyUnits[i]->HideTotalHP(l_bHideTotalUnitHP);
      }
   }

}

void GBattleScene::ProcessCombat(const GCombatInformationPacket& in_Packet,GBattleInformation *in_pBattleInformation)
{
   gassert(in_pBattleInformation,"Null pointer");
   gassert((ECombatAction::Enum)in_Packet.m_iAction == ECombatAction::Hit,"This function should only be called on Hit Packet");


   SP2::GUnit* l_pAttacker = (SP2::GUnit*)g_Joshua.UnitManager().Unit(in_Packet.m_iID1);
   SP2::GUnit* l_pTarget   = (SP2::GUnit*)g_Joshua.UnitManager().Unit(in_Packet.m_iID2);

   // Yes its possible they just don't exist yet...
   if(!l_pAttacker  || !l_pTarget)
      return ;

   // Find Unit Type
      
   UINT32 l_iAttackerUnitType = ((SP2::GUnitDesign*)l_pAttacker->Design())->Type()->Id() - 1;
   UINT32 l_iTargetUnitType = ((SP2::GUnitDesign*)l_pTarget->Design())->Type()->Id() - 1;

   if(l_pTarget->Group()->OwnerId() == in_pBattleInformation->m_iSelectedAllyCountry)
   {
      gassert(m_AllyUnits[l_iTargetUnitType],"Unit in combat that has not been loaded");
      gassert(m_EnnemyUnits[l_iAttackerUnitType],"Unit in combat that has not been loaded");
      if(m_AllyUnits[l_iTargetUnitType])
         if(m_AllyUnits[l_iTargetUnitType]->Activated())
            m_AllyUnits[l_iTargetUnitType]->ReceivedHit();

      if(m_EnnemyUnits[l_iAttackerUnitType])
         if(m_EnnemyUnits[l_iAttackerUnitType]->Activated())
            m_EnnemyUnits[l_iAttackerUnitType]->Attack(m_AllyUnits[l_iTargetUnitType]);
   }
   else
   if(l_pTarget->Group()->OwnerId() == in_pBattleInformation->m_iSelectedEnnemyCountry)
   {
      
      gassert(m_AllyUnits[l_iAttackerUnitType],"Unit in combat that has not been loaded");
      gassert(m_EnnemyUnits[l_iTargetUnitType],"Unit in combat that has not been loaded");

      if(m_EnnemyUnits[l_iTargetUnitType])
         if(m_EnnemyUnits[l_iTargetUnitType]->Activated())
            m_EnnemyUnits[l_iTargetUnitType]->ReceivedHit();
      if(m_AllyUnits[l_iAttackerUnitType])
         if(m_AllyUnits[l_iAttackerUnitType]->Activated())
            m_AllyUnits[l_iAttackerUnitType]->Attack(m_EnnemyUnits[l_iTargetUnitType]);
   }
}

void GBattleScene::OnProcess(float in_fTime)
{
   // Process all GUnit and Highlight the one that is over (with an anymation)
   for(UINT32 i=0;i<EUnitType::TotalCount-2;i++)
   {
      // If the unit is visible and has a valid pointer
      if(m_AllyUnits[i] && m_AllyUnits[i]->Activated() == true )
      {
         m_AllyUnits[i]->OnProcess(in_fTime);
      }

      if(m_EnnemyUnits[i] && m_EnnemyUnits[i]->Activated() == true )
      {
         m_EnnemyUnits[i]->OnProcess(in_fTime);
      }
   }
   
}

GBattleOverviewUnit* GBattleScene::OnMouseClick(const GVector2D<UINT32> in_MousePosition)
{
   // Process all GUnit and Highlight the one that is over (with an anymation)
   for(UINT32 i=0;i<EUnitType::TotalCount-2;i++)
   {
      // If the unit is visible
      if(m_AllyUnits[i] && m_AllyUnits[i]->Activated() == true && m_AllyUnits[i]->IsInsideMouseClick(in_MousePosition))
      {
         return m_AllyUnits[i];
      }
      if(m_EnnemyUnits[i] && m_EnnemyUnits[i]->Activated() == true &&  m_EnnemyUnits[i]->IsInsideMouseClick(in_MousePosition))
      {
         return m_EnnemyUnits[i];         
      }
   }

   return NULL;
}


GBattleOverviewWindow* GBattleOverviewWindow::Instance = NULL;

GUI::GBaseObject* GBattleOverviewWindow::New()
{
	return (GUI::GBaseObject*) new GBattleOverviewWindow();
}


GBattleOverviewWindow::GBattleOverviewWindow() : GUI::GFrame()
{
   // Set it to an Invalid Arena ID
   m_iArenaID = -1;
   m_pAnimatedCursorTrack = NULL;
   m_pSelectionContainer = NULL;
   m_pSelectedCursorOverride = NULL;
}

GBattleOverviewWindow::~GBattleOverviewWindow()
{
   // Unselect everything/
   UnSelect();
   if(m_pAnimatedCursorTrack)
   {
      g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Remove_Track(m_pAnimatedCursorTrack);
      SAFE_RELEASE(m_pAnimatedCursorTrack);
   }
   // Stop sounds.
   g_Joshua.SoundPlayer()->ClearDeleayedSound();

   SAFE_RELEASE(m_pSelectionContainer);
   SAFE_RELEASE(m_pSelectedCursorOverride);
}

bool GBattleOverviewWindow::OnCreate()
{
	__super::OnCreate();

   GBattleOverviewWindow::Instance = this;

   // There is no unit selected
   m_pSelectedUnit = NULL;

   // Gather UI Elements
   m_pObjCloseBtn    = (GUI::GButton*)Child(L"btnExit");

   Child(L"picRedBar")->SendToBack();

   m_pAllyFlag       = (GUI::GPictureBox*)Child(L"frmTrimYou")->Child(L"frmFlagYou")->Child(L"picFlag");
   m_pAllyFlagList   = (GUI::GListBox*)Child("lstFlagsYou");
   m_pEnemyFlag      = (GUI::GPictureBox*)Child(L"frmTrimEnemy")->Child(L"frmFlagEnemy")->Child(L"picFlag");
   m_pEnemyFlagList  = (GUI::GListBox*)Child("lstFlagsEnemy");

   m_pBattleFrame    = (GUI::GFrame*)Child("frmBattle")->Child("frm3D");
   m_pMapFrame       = (GUI::GFrame*)Child("frmBattle")->Child("frmMap");
   m_pStrategies     = (GUI::GComboBox*)Child("frmBattle")->Child("cboStrategies");
   m_pRetreatButton  = (GUI::GButton*) Child("frmBattle")->Child("btnRetreat");

   m_pBattleNameText = (GUI::GLabel*) Child("lblTitle");

   m_pObjOverviewMeter = (GBattleOverviewMeter*)Child(L"frmOverviewMeter");
   
   // Load Cursors
   m_UnitSelectCursor.LoadTexture("Data\\Combat_Overview\\UI\\SelectCursor.tga");
   m_UnitSelectCursor.Model()->Visibility() = false;
   m_pSelectionContainer = g_Joshua.Renderer()->Create_Model();
   CGFX_Transform_2D_Itf* l_pTransform = g_Joshua.Renderer()->Create_Transform_2D();
   m_pSelectionContainer->Set_Transform(l_pTransform);
   m_pSelectionContainer->Add_Child(m_UnitSelectCursor.Model());
   SAFE_RELEASE(l_pTransform);
      
   // Animate Cursor
   // Create Animation
   m_pAnimatedCursorTrack                    = g_Joshua.Renderer()->Create_Track();
   CGFX_Action_Itf* l_pAnimatedCursorAction  = g_Joshua.Renderer()->Create_Action();
   CGFX_Animation_Linear_Itf *l_pAnimation   = g_Joshua.Renderer()->Create_Animation_Linear();

   CGFX_Key_Linear   l_key;
   l_key.m_Time   = 0 ;
   l_key.m_Value  = -m_UnitSelectCursor.Width() ;
   l_pAnimation->Add_Key(l_key);

   l_key.m_Time   = 0.30f ;
   l_key.m_Value  = -m_UnitSelectCursor.Width() - 7.0f ;
   l_pAnimation->Add_Key(l_key);

   l_pAnimation->Setup();
   l_pAnimation->Connect(m_UnitSelectCursor.Transform(),GFX::PARAM_POSITION_X);
   l_pAnimatedCursorAction->Add_Animation(l_pAnimation);
   l_pAnimatedCursorAction->Length(0.3f);

   g_Joshua.GUIManager()->Scene()->Get_Animation_System()->Add_Track(m_pAnimatedCursorTrack);
   m_pAnimatedCursorTrack->Playback(GFX::PLAYBACK_PING_PONG);
   m_pAnimatedCursorTrack->Set_Action(l_pAnimatedCursorAction);
   m_pAnimatedCursorTrack->Playback_Count(0);
   m_pAnimatedCursorTrack->Set_Start_Time(0);

   SAFE_RELEASE(l_pAnimation);
   SAFE_RELEASE(l_pAnimatedCursorAction);

   // Add override for enemy 
   m_pSelectedCursorOverride = g_Joshua.Renderer()->CreateOverridePSCascade();
   l_pTransform = g_Joshua.Renderer()->Create_Transform_2D();
   l_pTransform->Size().X = -1;
   l_pTransform->Size().Y = 1;
   l_pTransform->Position().X = 1;
   l_pTransform->Position().Y = 0;
   m_pSelectedCursorOverride->AddUVTransformOverride(0, l_pTransform);
   SAFE_RELEASE(l_pTransform);


   //Child(L"picReOdBar")->SendToBack();

   m_BattleScene.AttachModel(m_pBattleFrame->Model()); 

   m_BattleMap.OnCreate(m_pMapFrame->Model());

   m_bSimulationIsOver = false;
   m_bWon = false;

	return true;
}

void GBattleOverviewWindow::SetSceneFromTheme(const GString in_Theme)
{

   // Read config for this scene
   m_BattleScene.ReadConfigFile("data\\Combat_Overview\\BattleOverview.xml",in_Theme);

   // Set Map and BG
   m_pBattleFrame->ColorNormal(GColorRGBInt(255,255,255,255));
   m_pBattleFrame->PictureNormal(GString("data\\Combat_Overview\\") + m_BattleScene.GetSceneImageFileName());
   m_pMapFrame->PictureNormal(GString("data\\Combat_Overview\\") + m_BattleScene.GetMapImageFileName() + L".dds");

   // Build Strategies list
   BuildStrategies();

   // Update from theme
   UpdateStrategiesFromTheme(in_Theme);

   // Create Battle scene
   m_BattleScene.SetUnitPosition();
}

void GBattleOverviewWindow::UpdateStrategiesFromTheme(const GString in_Theme)
{
   m_pStrategies->Clear();
   m_vStrategiesForCurrentTheme.clear();

   for(UINT i =0 ;i<m_vStrategies.size();i++)
   {
      if(m_vStrategies[i].m_iStrategyType == 0)
      {
         m_pStrategies->Add_Item(m_vStrategies[i].m_sName);
         m_vStrategiesForCurrentTheme.push_back(&m_vStrategies[i]);
      }
      else
      if (  (in_Theme == "Ocean") || 
            (in_Theme == "Artic"))
      {
         if(m_vStrategies[i].m_iStrategyType == 2)
         {
            m_pStrategies->Add_Item(m_vStrategies[i].m_sName);
            m_vStrategiesForCurrentTheme.push_back(&m_vStrategies[i]);
         }
      }
      else
      {
         if(m_vStrategies[i].m_iStrategyType == 1)
         {
            m_pStrategies->Add_Item(m_vStrategies[i].m_sName);
            m_vStrategiesForCurrentTheme.push_back(&m_vStrategies[i]);
         }
      }
   }

   // Set it back to the first one
   m_iCurrentStrategy = 0;
   m_pStrategies->Selected_Content(m_vStrategiesForCurrentTheme[m_iCurrentStrategy]->m_sName);
   SelectStrategy(m_iCurrentStrategy);
}

void GBattleOverviewWindow::BuildStrategies(void)
{
   // Clear Strategy vector
   m_vStrategies.clear();

   //Get the Strategy node 
   CXML_Util            l_XMLUtil;
   CTreeXMLNode*        l_pStrategiesNode;
   l_pStrategiesNode =  l_XMLUtil.Search_Node(m_BattleScene.GetConfigFile()->Root(),"Strategies",GString(""),NULL,1);
   gassert(l_pStrategiesNode,"Invalid Theme");

   CTreeXMLNode* l_pStrategyDescription = l_pStrategiesNode->FirstChild();
   while(l_pStrategyDescription)
   {
      GCombatStrategy l_NewStrategy;
      
      // Reat Strategy Name
      CTreeXMLNode* l_pStrategyName = l_XMLUtil.Search_Node(l_pStrategyDescription,"StringID",L"",NULL,1);   
      
      l_NewStrategy.m_sName = g_ClientDAL.GetString( l_pStrategyName->Data().m_value.ToINT32() );
      

      CTreeXMLNode* l_pStrategyType = l_XMLUtil.Search_Node(l_pStrategyDescription,"BattleType",L"",NULL,1);   
      if(l_pStrategyType)
      {
         if(l_pStrategyType->Data().m_value == "Ground")
            l_NewStrategy.m_iStrategyType = 1;
         else
         if(l_pStrategyType->Data().m_value == "Naval")
            l_NewStrategy.m_iStrategyType = 2;
         else
            l_NewStrategy.m_iStrategyType = 0;
         

      }
      else
         l_NewStrategy.m_iStrategyType = 0;

      for(UINT32 i=0;i<EUnitType::TotalCount;i++)
      {
         
         CTreeXMLNode* l_pUnitActionNode = l_XMLUtil.Search_Node(l_pStrategyDescription,c_UnitTypeXMLName[i],L"",NULL,1);   
         if(l_pUnitActionNode)
         {
            if(l_pUnitActionNode->Data().m_value == "Attack")
               l_NewStrategy.m_vUnitTypeActionID[i] = 2;
            if(l_pUnitActionNode->Data().m_value == "Defend")
               l_NewStrategy.m_vUnitTypeActionID[i] = 1;

         }
         else
         {
            // Don't change anything
            l_NewStrategy.m_vUnitTypeActionID[i] = 0;
         }
      }
      // Add Strategy to vector
      m_vStrategies.push_back(l_NewStrategy);

      l_pStrategyDescription = l_pStrategiesNode->NextChild();
   }
}

void GBattleOverviewWindow::SelectStrategy(UINT32 in_iStrategyId)
{
   gassert(in_iStrategyId < m_vStrategiesForCurrentTheme.size(),"This strategy doesnt exist");
   m_iCurrentStrategy = in_iStrategyId;
   GCombatStrategy *l_pStrategy = m_vStrategiesForCurrentTheme[m_iCurrentStrategy];
   // if Selected country == us.
   for(UINT32 i=0;i<EUnitType::AircraftCarrier;i++)
   {
      GBattleOverviewUnit* l_pAllyUnit = m_BattleScene.GetUnit(i+1, true);
      if(l_pAllyUnit && l_pAllyUnit->Activated() && l_pStrategy->m_vUnitTypeActionID[i])
      {
         if(l_pStrategy->m_vUnitTypeActionID[i] == 1)
            l_pAllyUnit->Status(GBattleOverviewUnit::DEFENDING);
         else
         if(l_pStrategy->m_vUnitTypeActionID[i] == 2)
            l_pAllyUnit->Status(GBattleOverviewUnit::ATTACKING);
      }
   }
}


GUI::EEventStatus::Enum GBattleOverviewWindow::OnCustomEvent(UINT32 in_EventID, const GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   switch (in_EventID)
   {

      case ECustomEvents::OnComboBoxSelectionEvent:
      {
         UINT32 l_iStrategyID = m_pStrategies->GetListBox()->Get_Selected_Item()[0];
         SelectStrategy(l_iStrategyID);
         SendUnitStatus(l_iStrategyID == m_vStrategiesForCurrentTheme.size()-1);
      }
      break;

      default: 
         return GBaseObject::OnCustomEvent(in_EventID, in_EventData, in_pCaller);      
   }

   return GUI::EEventStatus::Handled; 
}

bool GBattleOverviewWindow::OnGotFocus(const GUI::GBaseObject *in_pFocusFrom, GUI::GBaseObject *in_pCaller)
{
   BringToFront();  
   
   return false;
}

GUI::EEventStatus::Enum GBattleOverviewWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(in_pCaller)
      {
         if(in_pCaller == m_pObjCloseBtn)
         {
            if( g_ClientDDL.BattleSummaryWindow() && g_ClientDDL.BattleSummaryWindow()->Parent() == this)
            {
               g_ClientDDL.BattleSummaryWindowKill();
            }
            g_ClientDCL.KillCombatOverview();
         }
         else
         if(in_pCaller == m_pRetreatButton)
         {
            // Choose Retreat  (Last one)
            UINT32 l_iStrategyID = m_vStrategiesForCurrentTheme.size()-1;
            m_pStrategies->Selected_Content(m_vStrategiesForCurrentTheme[l_iStrategyID]->m_sName);
            SelectStrategy(l_iStrategyID);

            // Send Strategy to server
            SendUnitStatus(true);
         }
         else
         {
            // If its a button
            if(in_pCaller->Type() == GButton::TypeName)
            {
               GListBox* l_SelectedFlagList = (GListBox*)in_pCaller->Owner()->Owner();


               if(l_SelectedFlagList == m_pEnemyFlagList)
               {
                  
                  UINT32 l_iPreviousSelectedCountry = m_BattleInformation.m_iSelectedEnnemyCountry;
                  m_BattleInformation.m_iSelectedEnnemyCountry = m_FlagButtonToCountryIdMap[(GButton*)in_pCaller];

                  // If we change country , reset accumulated hit of all units
                  if(l_iPreviousSelectedCountry != m_BattleInformation.m_iSelectedEnnemyCountry)
                  {
                     // Go over all ally units 
                     for(UINT32 i=0;i<EUnitType::AircraftCarrier;i++)
                     {
                        GBattleOverviewUnit* l_pUnit = m_BattleScene.GetUnit(i+1,true);
                        l_pUnit->ResetHP();
                     }
                  }
               }
               else
               if(l_SelectedFlagList == m_pAllyFlagList)
               {

                  UINT32 l_iPreviousSelectedCountry = m_BattleInformation.m_iSelectedAllyCountry;
                  m_BattleInformation.m_iSelectedAllyCountry = m_FlagButtonToCountryIdMap[(GButton*)in_pCaller];

                  // If we change country , reset accumulated hit of all units
                  if(l_iPreviousSelectedCountry != m_BattleInformation.m_iSelectedAllyCountry)
                  {
                     // Go over all ally units 
                     for(UINT32 i=0;i<EUnitType::AircraftCarrier;i++)
                     {
                        GBattleOverviewUnit* l_pUnit = m_BattleScene.GetUnit(i+1,false);
                        l_pUnit->ResetHP();
                     }
                  }


               }

               // Set Flags
               const GCountry& l_Country = g_ClientDAL.Country(m_BattleInformation.m_iSelectedAllyCountry);
               m_pAllyFlag->PictureNormal(SP2::FLAG_DEFAULT_FOLDER + GString(l_Country.Flag()));

               const GCountry& l_CountryB = g_ClientDAL.Country(m_BattleInformation.m_iSelectedEnnemyCountry);
               m_pEnemyFlag->PictureNormal(SP2::FLAG_DEFAULT_FOLDER + GString(l_CountryB.Flag()));
            }
         }
      }
      else
      {

         // If a unit was previously selected, unselect it
         if(m_pSelectedUnit)
         {
            UnSelect();
         }


         GVector2D<INT32> l_MouseBattleFramePosition = in_EventData.Mouse.Position - m_pBattleFrame->Position() - m_pBattleFrame->Owner()->Position();
         m_pSelectedUnit = m_BattleScene.OnMouseClick(l_MouseBattleFramePosition);

         if(!m_pSelectedUnit)
         {
            UINT32   l_iUnitId;
            bool     l_bUnitIsAlly;
            GVector2D<INT32> l_MouseMapFramePosition = in_EventData.Mouse.Position - m_pMapFrame->Position() - m_pMapFrame->Owner()->Position();
         if(m_BattleMap.OnMouseClick(l_MouseMapFramePosition,l_iUnitId,l_bUnitIsAlly))
         {
            m_pSelectedUnit = m_BattleScene.GetUnit(l_iUnitId+1,l_bUnitIsAlly);
         }
         }


         // If a unit is selected, show it in the map
         if(m_pSelectedUnit)
         {
            // Place Cursor
            m_pSelectedUnit->Model()->Add_Child(m_pSelectionContainer);
            g_Joshua.GUIManager()->SetDirty();

            ((CGFX_Transform_2D_Itf *)(m_pSelectionContainer->Get_Transform()))->Position().Y = m_pSelectedUnit->Height() / 2.0f;

            if(m_pSelectedUnit->Side() == EBattleOverviewUnitSide::Ally)
            {
               m_UnitSelectCursor.Model()->ClearOverrides();
               ((CGFX_Transform_2D_Itf *)(m_pSelectionContainer->Get_Transform()))->Position().X = 0;
            }
            else
            {
               m_UnitSelectCursor.Model()->AddOverride(m_pSelectedCursorOverride);
               ((CGFX_Transform_2D_Itf *)(m_pSelectionContainer->Get_Transform()))->Position().X  = m_UnitSelectCursor.Width() + m_pSelectedUnit->Width();
            } 
            m_UnitSelectCursor.Model()->Visibility() = true;
            

            m_pSelectedUnit->Selected(true);
            GVector2D<REAL32> l_MousePositionInSpriteSpace = l_MouseBattleFramePosition - m_pSelectedUnit->Transform()->Position();

            if(m_pSelectedUnit->Side() == EBattleOverviewUnitSide::Ally)
            {
               if(m_pSelectedUnit->IsStatusIconClick(l_MousePositionInSpriteSpace) &&  m_BattleInformation.m_iSelectedAllyCountry   == (UINT32)g_ClientDAL.ControlledCountryID())
               {
                  if(m_pSelectedUnit->Status() == GBattleOverviewUnit::DEFENDING)
                  {
                     m_pSelectedUnit->Status(GBattleOverviewUnit::ATTACKING);
                     g_SP2Client->PlaySound(ESoundEventID::OrderAttack1);
                  }
                  else
                  {
                     m_pSelectedUnit->Status(GBattleOverviewUnit::DEFENDING);
                     g_SP2Client->PlaySound(ESoundEventID::OrderDefend1);
                  }

                  // Go back into manual mode
                  SelectStrategy(0);
                  m_pStrategies->Selected_Content(m_vStrategiesForCurrentTheme[m_iCurrentStrategy]->m_sName);
                  // Send Strategy to server
                  SendUnitStatus(false);
               }
            }
            m_BattleMap.Select(m_pSelectedUnit);
         }
         
      }
   }
   
   return GUI::EEventStatus::Handled; 
}


void GBattleOverviewWindow::UnSelect(void)
{
   if(m_pSelectedUnit)
   {
      m_pSelectedUnit->Selected(false);
      m_BattleMap.Unselect();

      if(m_pSelectionContainer->Get_Parent())
         m_pSelectionContainer->Get_Parent()->Remove_Child(m_UnitSelectCursor.Model());
      m_pSelectedUnit = NULL;
   
      m_UnitSelectCursor.Model()->Visibility() = false;
   }
}

/*!
 * Set which one is the arena to display
 **/
void GBattleOverviewWindow::SetArena(SP2::GArenaInfo* in_pArenaInfo)
{
   gassert(in_pArenaInfo,"Null pointer");
   m_iArenaID = in_pArenaInfo->m_iId;

   // Set Selected Country for Display
   bool l_bWeFoundOurself = false;

   // Select ourselves
   m_BattleInformation.m_iSelectedAllyCountry = g_ClientDAL.ControlledCountryID();

   // Are we an attacker
   for(UINT32 i=0;i<in_pArenaInfo->m_vAttackerCountriesList.size();i++)
   {
      if((UINT32)g_ClientDAL.ControlledCountryID() == in_pArenaInfo->m_vAttackerCountriesList[i])
      {
         m_BattleInformation.m_eCombatSide = ECombatSide::Attacker;
         // Select first ennemy country
         m_BattleInformation.m_iSelectedEnnemyCountry = in_pArenaInfo->m_vDefenderCountriesList[0];
         l_bWeFoundOurself = true;
      }
   }
   // If not in the attackers list, therefore lets assume we are in de defender !!!
   if(!l_bWeFoundOurself)
   {
      m_BattleInformation.m_eCombatSide = ECombatSide::Defender;
      // Select first attacker 
      m_BattleInformation.m_iSelectedEnnemyCountry = in_pArenaInfo->m_vAttackerCountriesList[0];
   }

   m_BattleInformation.m_pArenaInfo = in_pArenaInfo;

   // Set Arena Title
   if(in_pArenaInfo->m_iRegionID == 0)
      m_pBattleNameText->Text(g_ClientDAL.GetString(102255));
   else
      m_pBattleNameText->Text(g_ClientDAL.GetString(102254) + L" " + g_ClientDAL.RegionName(in_pArenaInfo->m_iRegionID));

   UpdateCountrListFlag();

   // Set Flags
   const GCountry& l_Country = g_ClientDAL.Country(m_BattleInformation.m_iSelectedAllyCountry);
   m_pAllyFlag->PictureNormal(SP2::FLAG_DEFAULT_FOLDER + GString(l_Country.Flag()));
   m_pAllyFlag->TooltipText( l_Country.Name() );

   const GCountry& l_CountryB = g_ClientDAL.Country(m_BattleInformation.m_iSelectedEnnemyCountry);
   m_pEnemyFlag->PictureNormal(SP2::FLAG_DEFAULT_FOLDER + GString(l_CountryB.Flag()));
   m_pEnemyFlag->TooltipText( l_CountryB.Name() );

   // Update Arena Information with Arena ID
   UpdateArenaInfo();
}

void GBattleOverviewWindow::UpdateCountrListFlag(void)
{
   // Clean country flags
   m_pEnemyFlagList->Clear();
   m_pAllyFlagList->Clear();
   m_FlagButtonToCountryIdMap.clear();

   m_LastIterationTotalAttaker = 0;
   m_LastIterationTotalDefender = 0;

   // Set attacker and defender list
   GListBox* l_pAttackList,*l_pDefenderlist;
   if(m_BattleInformation.m_eCombatSide == ECombatSide::Attacker)
   {
      l_pAttackList = m_pAllyFlagList;
      l_pDefenderlist = m_pEnemyFlagList;
   }
   else
   {
      l_pAttackList = m_pEnemyFlagList;
      l_pDefenderlist = m_pAllyFlagList;
   }


   GListBox::GRow* l_pRow = NULL;
   for(UINT32 i=0;i<m_BattleInformation.m_pArenaInfo->m_vAttackerCountriesList.size();i++)
   {
      // Do we need a new row ?
      if(((INT32)(i/7)) == l_pAttackList->Get_Nb_Rows())
      {
         l_pRow = l_pAttackList->Insert_New_Row();
         for(UINT32 j=0;j<7;j++)
         {
            GString l_ChildName(GString("btnFlag") + GString( (j%7) + 1) );
            GPictureBox* l_pFlagPicture = (GPictureBox*)l_pRow->Object()->Child(l_ChildName);
            l_pFlagPicture->Hide();
         }
      }

      GString l_ChildName(GString("btnFlag") + GString( (i%7) + 1) );
      GPictureBox* l_pFlagPicture = (GPictureBox*)l_pRow->Object()->Child(l_ChildName);
      m_FlagButtonToCountryIdMap.insert(make_pair((GButton*)l_pFlagPicture,m_BattleInformation.m_pArenaInfo->m_vAttackerCountriesList[i]));

      const GCountry& l_Country = g_ClientDAL.Country(m_BattleInformation.m_pArenaInfo->m_vAttackerCountriesList[i]);
      l_pFlagPicture->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Country.Id()]);
      l_pFlagPicture->Show();

      l_pFlagPicture->TooltipText( l_Country.Name() );

      m_LastIterationTotalAttaker++;
   }

   for(UINT32 i=0;i<m_BattleInformation.m_pArenaInfo->m_vDefenderCountriesList.size();i++)
   {
      // Do we need a new row ?
      if(((INT32)(i/7)) == l_pDefenderlist->Get_Nb_Rows())
      {
         l_pRow = l_pDefenderlist->Insert_New_Row();
         for(UINT32 j=0;j<7;j++)
         {
            GString l_ChildName(GString("btnFlag") + GString( (j%7) + 1) );
            GPictureBox* l_pFlagPicture = (GPictureBox*)l_pRow->Object()->Child(l_ChildName);
            l_pFlagPicture->Hide();
         }
      }

      GString l_ChildName(GString("btnFlag") + GString( (i%7) + 1) );
      GPictureBox* l_pFlagPicture = (GPictureBox*)l_pRow->Object()->Child(l_ChildName);
      l_pFlagPicture->Show();
      m_FlagButtonToCountryIdMap.insert(make_pair((GButton*)l_pFlagPicture,m_BattleInformation.m_pArenaInfo->m_vDefenderCountriesList[i]));
      const GCountry& l_Country = g_ClientDAL.Country(m_BattleInformation.m_pArenaInfo->m_vDefenderCountriesList[i]);

      l_pFlagPicture->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Country.Id()]);
      l_pFlagPicture->TooltipText( l_Country.Name() );

      m_LastIterationTotalDefender++;
   }

   // Update List
   m_pEnemyFlagList->Update();
   m_pAllyFlagList->Update();
}

bool GBattleOverviewWindow::DoFinalUpdate(SP2::GArenaInfo& in_pArenaInfo)
{

   m_bWon = in_pArenaInfo.m_eWinnerSide == m_BattleInformation.m_eCombatSide;
   
   // Is it the one we are showing ?
   if(in_pArenaInfo.m_iId == (UINT32)m_iArenaID)
   {
      // Kill the non winner side
      for(UINT32 i=0;i<EUnitType::AircraftCarrier;i++)
      {
 
         GBattleOverviewUnit* l_pUnit = m_BattleScene.GetUnit(i+1,in_pArenaInfo.m_eWinnerSide != m_BattleInformation.m_eCombatSide);
         if(l_pUnit->Activated())
         {
            l_pUnit->AccumulatedHit(l_pUnit->UnitHP());
            l_pUnit->TotalUnitHP(l_pUnit->UnitHP());
            l_pUnit->ReceivedHit();
         }
      }

      if(in_pArenaInfo.m_eWinnerSide == m_BattleInformation.m_eCombatSide)
      {
         // Update Batle Progess
         m_pObjOverviewMeter->Value(1,0);
         m_pObjOverviewMeter->Child(L"btnSeparator1")->Left(m_pObjOverviewMeter->MeterBtn()[0]->Left() + m_pObjOverviewMeter->MeterBtn()[0]->Width() - 2);
         m_BattleMap.ClearPosition(false);
      }
      else
      {
         m_pObjOverviewMeter->Value(0,0);
         m_pObjOverviewMeter->Child(L"btnSeparator1")->Left(m_pObjOverviewMeter->MeterBtn()[0]->Left() + m_pObjOverviewMeter->MeterBtn()[0]->Width() - 2);
         m_BattleMap.ClearPosition(true);
      }

      SimulationIsOver(true);   
      return true;
   }

   return false;
}

void GBattleOverviewWindow::UpdateArenaInfo(void)
{
   // Refresh battle Field
   m_BattleScene.CreateBattleField(&m_BattleInformation);
   m_BattleMap.CreateBattleField(&m_BattleInformation);

   if( (m_LastIterationTotalDefender != m_BattleInformation.m_pArenaInfo->m_vDefenderCountriesList.size()) ||
       (m_LastIterationTotalAttaker != m_BattleInformation.m_pArenaInfo->m_vAttackerCountriesList.size()))
   {
      UpdateCountrListFlag();
   }

   // Refresh Selection
   if(m_pSelectedUnit)
   { 
      // Turn off the selection if we don't have this unit anymore
      if(m_pSelectedUnit->Activated() == FALSE)
      {  
         UnSelect();
      }
   }

   // Calculate Battle progress 
   float l_battleProgress = CalculateBattleProgress();

   // Update Batle Progess
   m_pObjOverviewMeter->Value(l_battleProgress,0);
   m_pObjOverviewMeter->Child(L"btnSeparator1")->Left(m_pObjOverviewMeter->MeterBtn()[0]->Left() + m_pObjOverviewMeter->MeterBtn()[0]->Width() - 2);
}

float GBattleOverviewWindow::CalculateBattleProgress(void)
{

   REAL32 l_fTotalAllyValue =0;
   REAL32 l_fTotalValue =0;

   // Go throuh all the actors
   hash_map<UINT32,GActorInfo>::const_iterator l_ActorIterator = m_BattleInformation.m_pArenaInfo->Actors().begin();
   while(l_ActorIterator !=  m_BattleInformation.m_pArenaInfo->Actors().end())
   {
      GActorInfo l_ActorInfo = l_ActorIterator->second;

      //For every unit type for the Actor
      vector<GUnitTypeCbtInfo>::const_iterator l_UnitTypeCbtInfoIt = l_ActorIterator->second.m_vUnitTypeInfo.begin();
      while(l_UnitTypeCbtInfoIt != l_ActorIterator->second.m_vUnitTypeInfo.end())
      {
         //Ge the side of the actor
         if((*l_UnitTypeCbtInfoIt).m_iUnitQty > 0)
         {
            if(l_ActorInfo.m_Side == m_BattleInformation.m_eCombatSide)
            {
               l_fTotalAllyValue+=(*l_UnitTypeCbtInfoIt).m_fUnitsValue ;
            }
            l_fTotalValue+= (*l_UnitTypeCbtInfoIt).m_fUnitsValue;
         }
         l_UnitTypeCbtInfoIt++;

      }
      l_ActorIterator++;      
  }

   if(l_fTotalValue == 0)
      return 0;
   else
      return l_fTotalAllyValue / l_fTotalValue;
}

UINT32 GBattleOverviewWindow::ArenaId()
{
   return m_iArenaID;
}


/*!
 * Handles the reception of a UDP Packet that gives information on the combat
 **/
bool GBattleOverviewWindow::HandleCombatInformationUDPPacket(const GCombatInformationPacket& in_Packet)
{
   //m_BattleScene.ProcessCombat(in_Packet);
   ECombatAction::Enum l_eAction = (ECombatAction::Enum)in_Packet.m_iAction;
   switch(l_eAction)
   {
      //id1 hit id2
      case ECombatAction::Hit:
      {
         m_BattleScene.ProcessCombat(in_Packet,&m_BattleInformation);
      }
   };

   return true;
}


/*!
 * Updates general information about the combat
 * Updates the unit lists, removes killed units, updates existing ones (update the unit qty, 
 * values, action, last time clocks, etc.) and add new ones
 * Only thing that is not updated is the unit type positions
 **/
void GBattleOverviewWindow::Iterate(void)
{  
   m_BattleInformation.m_pArenaInfo = (SP2::GArenaInfo*)g_Joshua.UnitManager().ArenaInfo(m_iArenaID);
   m_BattleScene.OnProcess(0);
   if(m_BattleInformation.m_pArenaInfo && m_bSimulationIsOver == false)
   {
      UpdateArenaInfo();
   }
   else
   {
      ShowOverallCombat();
   }

   m_BattleMap.DrawUnitsOnMap();
}

bool GBattleOverviewWindow::OnDestroy()
{

   __super::OnDestroy();
   return true;
}



GBattleScene& GBattleOverviewWindow::BattleScene()
{
   return m_BattleScene;
}


//! Gets a pointer to the battleoverviewunit of the given type, NULL if nothing
GBattleOverviewUnit* GBattleScene::GetUnit(UINT32 in_eUnitType, bool in_bAlly)
{
   if(in_bAlly)
   {
      hash_map<UINT32, GBattleOverviewUnit*>::iterator l_It = m_AllyUnits.find(in_eUnitType-1);
      if(l_It == m_AllyUnits.end())
      {
         return NULL;
      }
      else
         return l_It->second;
   }
   else
   {
      hash_map<UINT32, GBattleOverviewUnit*>::iterator l_It = m_EnnemyUnits.find(in_eUnitType-1);
      if(l_It == m_EnnemyUnits.end())
      {
         return NULL;
      }
      else
         return l_It->second;
   }
}

void GBattleOverviewWindow::SendUnitStatus(bool in_bRetreat)
{

   // Only send status if simulation exist.
   if(m_bSimulationIsOver == false && m_BattleInformation.m_pArenaInfo)
   {
      SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GBattleOverviewUnitActionUpdate);
      SP2::Event::GBattleOverviewUnitActionUpdate* l_UpdateStatusEvent = (SP2::Event::GBattleOverviewUnitActionUpdate*) (l_Event.get() );
      l_Event->m_iSource = g_SP2Client->Id();
      l_Event->m_iTarget = SDK::Event::ESpecialTargets::Server;
      l_UpdateStatusEvent->m_iCombatId = m_BattleInformation.m_pArenaInfo->m_iId;
      l_UpdateStatusEvent->m_iCountryId = g_ClientDAL.ControlledCountryID();
      l_UpdateStatusEvent->m_bRetreat   = in_bRetreat;
      for(UINT32 i=0;i<EUnitType::AircraftCarrier;i++)
         l_UpdateStatusEvent->m_bAttack[i] = m_BattleScene.GetUnit(i + 1,true)->Status() == SP2::GBattleOverviewUnit::ATTACKING;
      g_Joshua.RaiseEvent(l_Event);	
   }
}

void GBattleOverviewWindow::ShowOverallCombat(void)
{

   // Only show overall combat when the combat is over
   // Combat is over when all units (on the same side ) are deactivated and  the simulatin is over
   if(m_bSimulationIsOver == false) 
      return;
   bool l_bActivatedAllyUnit  = false;
   bool l_bActivatedEnemyUnit = false;
   for(UINT32 i=0;i<EUnitType::AircraftCarrier;i++)
   {
      GBattleOverviewUnit* l_pUnit = m_BattleScene.GetUnit(i+1,true);
      if(l_pUnit->Activated())
         l_bActivatedAllyUnit = true;
      l_pUnit = m_BattleScene.GetUnit(i+1,false);
      if(l_pUnit->Activated())
         l_bActivatedEnemyUnit = true;
   }
   // If both still activated then combat is not over
   if(l_bActivatedAllyUnit && l_bActivatedEnemyUnit)
     return;

   // One of the side is over.
   if(g_ClientDDL.BattleSummaryWindow() && g_ClientDDL.BattleSummaryWindow()->Visible() == false)
   {
      UnSelect();
      g_Joshua.SoundPlayer()->ClearDeleayedSound();
      g_ClientDDL.BattleSummaryWindow()->Show();
      g_ClientDDL.MainBar()->SendToBack();
      g_ClientDDL.MiniMap()->SendToBack();


      if(m_bWon)
         g_SP2Client->PlaySound(ESoundEventID::CombatWin1);
      else
         g_SP2Client->PlaySound(ESoundEventID::CombatLost1);
   } 
}

void GBattleOverviewWindow::SimulationIsOver(bool in_bSimulationIsOver)
{
   m_bSimulationIsOver = in_bSimulationIsOver;
   
   // If Simulation is over, disable retreat button and strategies
   m_pRetreatButton->Enabled(!m_bSimulationIsOver);
   m_pStrategies->Enabled(!m_bSimulationIsOver);
}

 