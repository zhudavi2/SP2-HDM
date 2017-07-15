/**************************************************************
*
* sp2_unit_list_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_unit_list_window.h"

#include "../include/sp2_generic_question_window.h"
#include "../include/sp2_military_window.h"
#include "../include/sp2_unit_split_window.h"

const GString GUnitListWindow::TypeName(L"Unit_List_Window");



const UINT32 c_iLongitudeStrId = 101133;//Long:
const UINT32 c_iLatitudeStrId  = 101134;//Lat:



GUI::GBaseObject* GUnitListWindow::New()
{
   return new GUnitListWindow();
}

GUnitListWindow::GUnitListWindow() : GBaseWindow()
{
   m_pObjUnitTree = NULL;
   m_bUpdateFromSelectedUnits = false;
   m_pSelectedItem  = NULL;
   m_iSelectedUnitId = -1;
   m_pObjSplitBtn = NULL;

   m_UnitTrainingIDLogo[SP2::ETrainingLevel::Regular] = "000825";
   m_UnitTrainingIDLogo[SP2::ETrainingLevel::Veteran] = "000826";
   m_UnitTrainingIDLogo[SP2::ETrainingLevel::Elite]   = "000827";
}

GUnitListWindow::~GUnitListWindow()
{
}

bool GUnitListWindow::OnCreate()
{
   __super::OnCreate();

   m_pObjAccuracyOfInformationFrm = (GUI::GFrame*)Child(L"frmInner")->Child(L"frmInformationAccuracy");
   m_pObjButtonsFrm = (GUI::GFrame*)Child(L"frmInner")->Child(L"frmActions");
   
   m_pObjTrainBtn   = (GUI::GButton*)m_pObjButtonsFrm->Child(L"btnTrain");
   m_pObjSellBtn    = (GUI::GButton*)m_pObjButtonsFrm->Child(L"btnSell");
   m_pObjMergeBtn   = (GUI::GButton*)m_pObjButtonsFrm->Child(L"btnMerge");
   m_pObjSplitBtn   = (GUI::GButton*)m_pObjButtonsFrm->Child(L"btnSplit");
   
   m_pObjTreeScr = (GUI::GVScrollBar*)Child(L"frmInner")->Child(L"frmList")->Child(L"scrTree");
   m_pObjTreeFrm = (GUI::GFrame*)Child(L"frmInner")->Child(L"frmList")->Child(L"frmInner")->Child(L"frmTree");

   m_pObjApplyBtn->Hide();

   ((GFX::GWindow*)((GUI::GFrame*)m_pObjTreeFrm->Owner())->Model()->Get_Primitive() )->ClipChildren(true);
   m_pObjAccuracyOfInformationFrm->Hide();

   m_pObjAccuracy    = (SP2::GGradientBar*)m_pObjAccuracyOfInformationFrm->Child(L"frmAccuracy");
   m_pObjAccuracyTxt = (GUI::GLabel*)m_pObjAccuracyOfInformationFrm->Child(L"txtValue");

   m_pObjAccuracy->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));
   m_pObjAccuracy->Value(0.f, 0, false, false);
   m_pObjAccuracyTxt->Text(GString::FormatNumber(0.f, L"", L"", L"", L" %", 0, 0));
   
   return true;
}

void GUnitListWindow::OnShow()
{
   m_iSelectedUnitId = -1;
   GenerateUnitTree();
}

void GUnitListWindow::OnHide()
{
   if(m_pObjUnitTree)
   {
      m_pObjUnitTree->Hide();
      g_Joshua.GUIManager()->ReleaseObject(m_pObjUnitTree);
      m_pObjUnitTree = NULL;
   }
}

GUI::EEventStatus::Enum GUnitListWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExitBtn)
         {
            g_ClientDDL.UnitListWindow()->Hide();
         }
      }
   }

   return __super::OnMouseDown(in_EventData, in_pCaller);
}

bool GUnitListWindow::Update()
{   	
	UINT32 l_iCountryWhereTheUnitsAre = UINT_MAX;
	UINT32 l_iUnitGroupCountryID = 0;
	UINT32 l_iCountryID = 0;
	REAL32 l_fAccuracyOfInformation = 0.f;
	REAL32 l_fAccuracyOfInformationBonus = 0.f;
	bool l_bInfantryInformation = true;
	bool l_bGroundInformation = true;
	bool l_bAirInformation = true;
	bool l_bNavalInformation = true;
		

   GUnitListByType l_UnitList[EUnitType::TotalCount];
   set<UINT32>     l_DesignsToShow[EUnitType::TotalCount];
   
   //Fetch the units for all the units of the controlled country
   if(!m_bUpdateFromSelectedUnits)
   {
      l_iUnitGroupCountryID = g_ClientDAL.ControlledCountryID();
		l_iCountryID = l_iUnitGroupCountryID;
		g_ClientDAL.FetchUnitListInformation(g_ClientDAL.ControlledCountryID(),
                                          l_UnitList);

      m_pObjButtonsFrm->Show();
      m_pObjAccuracyOfInformationFrm->Hide();
   }
   //Fetch the units list for the selected group
   else
   {
      const set<UINT32> l_IDs = g_ClientDAL.SelectedUnitsID();
      
      g_ClientDAL.FetchUnitListInformation(0,
                                           l_UnitList,
                                           &l_IDs);

      m_pObjAccuracyOfInformationFrm->Show();
      m_pObjButtonsFrm->Hide();
   }
   m_pObjUnitTree->Nodes().clear();

   //For each type of unit, if there are units in the given type, check if that unit type must be displayed
   //because of the accuracy of information
   for(UINT32 i = EUnitType::Infantry ; i < EUnitType::TotalCount ; i++)
   {
      GUnitListByType* l_pActualUnitTypeList = &l_UnitList[i];

      //If there are units, check if they are visible because of the accuracy of information      
      if(l_pActualUnitTypeList->size() > 0)
	   {
         if(l_pActualUnitTypeList->at(0).second.size() > 0)
		   {
            l_iUnitGroupCountryID = l_pActualUnitTypeList->at(0).second[0].m_pUnit->Group()->OwnerId();			
			   if(l_iCountryWhereTheUnitsAre == UINT_MAX)
			   {
				   UINT32 l_iRegionID = g_ClientDCL.EarthLocateRegion(l_pActualUnitTypeList->at(0).second[0].m_Position.x,l_pActualUnitTypeList->at(0).second[0].m_Position.y);
				   l_iCountryWhereTheUnitsAre = g_ClientDAL.RegionControl(l_iRegionID).m_iMilitary;
				   if(g_ClientDAL.DiplomaticStatus(l_iCountryWhereTheUnitsAre,g_ClientDAL.m_PlayerCountryData.CountryID()) == EDiplomaticStatus::Allied)
					   l_fAccuracyOfInformationBonus = SP2::c_fSameRegionAccuracyBonus; 
			   }			
			   l_iCountryID = l_iUnitGroupCountryID;
			   l_fAccuracyOfInformation = g_ClientDAL.m_PlayerCountryData.AccuracyInformation(l_iUnitGroupCountryID);
			   l_fAccuracyOfInformation = min(l_fAccuracyOfInformationBonus+l_fAccuracyOfInformation,1.f);
			   m_Rand.Seed(l_iCountryID * (UINT32)floor(l_fAccuracyOfInformation*100.f) * (UINT32)i);
			   for(UINT32 j=0 ; j< l_pActualUnitTypeList->size() ; j++)
			   {						
				   if(m_Rand.RandomReal() <= l_fAccuracyOfInformation)
                  l_DesignsToShow[i].insert(l_pActualUnitTypeList->at(j).first);				
			   }
		   }		
	   }//end if there are units
   }


   UINT32            l_iIndex = 0;
   UINT32            l_iQty   = 0;
   UINT32            l_iQty2  = 0;

   
   //For each unit Category
   for(UINT32 i = 0; //Infantry
              i < EUnitCategory::ItemCount;
              i++)
   {
      //Add the node that gives the Category name (air,naval,ground,infantry...)
      m_pObjUnitTree->Nodes().add(PrepareTreeItem(m_pObjUnitTree->CreateNode(),
                                                  g_ClientDAL.GetString(c_iUnitCategoryStringID[i]),
                                                  0));

      INT32             l_iQty3  = 0;
      INT32             l_iInnerIndex = 0;
      INT32             l_iInnerIndex2 = 0;

      GString           l_sName;
      GUnitListByType*  l_pUnitListType = NULL;


	   bool l_bShowingAtLeastOneDesign = false;
	   bool l_bShowDesign              = true;
	   EUnitType::Enum l_eUnitType     = EUnitType::TotalCount;

      //For each unit type of the given category
      for(UINT32 l_iType = 1 ; l_iType < EUnitType::TotalCount ; l_iType++)
      {
         l_bShowDesign = true;
         //If the unit type is in the correct category
         if(c_eUnitCategoryByType[l_iType] == (EUnitCategory::Enum)i)
         {
            l_eUnitType = (EUnitType::Enum)l_iType;

            //If there are designs to show
            if(l_DesignsToShow[l_eUnitType].size() == 0)
            {
			      l_bShowDesign = false;
            }
		      else
            {
			      l_bShowingAtLeastOneDesign = true;
            }
            l_pUnitListType = &l_UnitList[l_eUnitType];
            l_sName         = g_ClientDAL.GetString(c_iUnitTypeStringID[l_eUnitType]);

            l_iQty = 0;
            //If showing any unit of the actual unit type
            if(l_bShowDesign)
			      m_pObjUnitTree->Nodes()[l_iIndex]->Nodes().add(PrepareTreeItem(m_pObjUnitTree->CreateNode(),l_sName,0));
            //Not showing any unit of the actual unit type
		      else
			      m_pObjUnitTree->Nodes()[l_iIndex]->Nodes().add(PrepareTreeItem(m_pObjUnitTree->CreateNode(),L"???",0));

            bool l_bShowingSomething = false;

		      for(UINT32 i=0; i < l_pUnitListType->size(); i++)
            {
			      bool l_bShowingDesign = false;
               l_iQty2 = 0;
			      l_sName = L"???";


               if(l_DesignsToShow[l_eUnitType].find(l_pUnitListType->at(i).first) != l_DesignsToShow[l_eUnitType].end())
               {
				      l_bShowingSomething = true;
				      l_bShowingDesign    = true;
				      l_sName = ((SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign(l_pUnitListType->at(i).first))->Name();
               }

      			
			      m_pObjUnitTree->Nodes()[l_iIndex]->Nodes()[l_iInnerIndex]->Nodes().add(PrepareTreeItem(m_pObjUnitTree->CreateNode(),l_sName,0));
               for(UINT32 j=0; j < l_pUnitListType->at(i).second.size(); j++)
               {
                  GUnitListInformation & l_UnitInfo = l_pUnitListType->at(i).second[j];
      				
				      UINT32 l_iQtyToAdd = 0;
				      m_Rand.Seed(l_iCountryID * (UINT32)floor(l_fAccuracyOfInformation*100.f) * l_pUnitListType->at(i).first);
				      if(m_Rand.RandomReal() <= l_fAccuracyOfInformation)
					      l_iQtyToAdd = (UINT32)(l_UnitInfo.m_pUnit->Qty() * 1.f + ( (1.f-l_fAccuracyOfInformation) * (m_Rand.RandomReal(2.f)-1.f)  ));

                  
                  GString l_sPosition = g_ClientDAL.GetString(c_iLongitudeStrId) + GString::FormatNumber(l_UnitInfo.m_Position.x,2) + g_ClientDAL.GetString(c_iLatitudeStrId)+ GString::FormatNumber(l_UnitInfo.m_Position.y,2);
                  m_pObjUnitTree->Nodes()[l_iIndex]->Nodes()[l_iInnerIndex]->Nodes()[l_iInnerIndex2]->Nodes().add(PrepareTreeItem(m_pObjUnitTree->CreateNode(),l_sPosition,l_iQtyToAdd,l_UnitInfo.m_Status,l_UnitInfo.m_pUnit->Id()));
                  l_iQty += l_iQtyToAdd;
                  l_iQty2 += l_iQtyToAdd;
               
               }
               if(l_iQty2 == 0 && !l_bShowingDesign)
               {
                  m_pObjUnitTree->Nodes()[l_iIndex]->Nodes()[l_iInnerIndex]->Nodes()[l_iInnerIndex2]->Item()->Hide();
                  g_Joshua.GUIManager()->ReleaseObject(m_pObjUnitTree->Nodes()[l_iIndex]->Nodes()[l_iInnerIndex]->Nodes()[l_iInnerIndex2]->Item());
                  m_pObjUnitTree->Nodes()[l_iIndex]->Nodes()[l_iInnerIndex]->Nodes().erase_at(l_iInnerIndex2);
               }
               else
               {
				      if(l_iQty2 == 0)
					      ((GUI::GLabel*)((GUI::GFrame*)m_pObjUnitTree->Nodes()[l_iIndex]->Nodes()[l_iInnerIndex]->Nodes()[l_iInnerIndex2]->Item())->Child(L"lblQty"))->Text(L"???");
				      else
					      ((GUI::GLabel*)((GUI::GFrame*)m_pObjUnitTree->Nodes()[l_iIndex]->Nodes()[l_iInnerIndex]->Nodes()[l_iInnerIndex2]->Item())->Child(L"lblQty"))->Text(GString(l_iQty2));

                  l_iInnerIndex2++;
               }
            }

            l_iInnerIndex2 = 0;

            if(l_iQty == 0 && !l_bShowingSomething)
            {
               m_pObjUnitTree->Nodes()[l_iIndex]->Nodes()[l_iInnerIndex]->Item()->Hide();
               g_Joshua.GUIManager()->ReleaseObject(m_pObjUnitTree->Nodes()[l_iIndex]->Nodes()[l_iInnerIndex]->Item());
               m_pObjUnitTree->Nodes()[l_iIndex]->Nodes().erase_at(l_iInnerIndex);
            }
            else
            {
			      if(l_iQty == 0)
				      ((GUI::GLabel*)((GUI::GFrame*)m_pObjUnitTree->Nodes()[l_iIndex]->Nodes()[l_iInnerIndex]->Item())->Child(L"lblQty"))->Text(L"???");
			      else
				      ((GUI::GLabel*)((GUI::GFrame*)m_pObjUnitTree->Nodes()[l_iIndex]->Nodes()[l_iInnerIndex]->Item())->Child(L"lblQty"))->Text(GString(l_iQty));

               l_iInnerIndex++;
            }

            l_iQty3 += l_iQty;
         }
      }
      if(l_iQty3 == 0 && !l_bShowingAtLeastOneDesign)
      {
         m_pObjUnitTree->Nodes()[l_iIndex]->Item()->Hide();
         g_Joshua.GUIManager()->ReleaseObject(m_pObjUnitTree->Nodes()[l_iIndex]->Item());
         m_pObjUnitTree->Nodes().erase_at(l_iIndex);
		   l_bGroundInformation = false;
      }
      else
      {
		   if(l_iQty3 == 0)
			   ((GUI::GLabel*)((GUI::GFrame*)m_pObjUnitTree->Nodes()[l_iIndex]->Item())->Child(L"lblQty"))->Text(L"???");
		   else
			   ((GUI::GLabel*)((GUI::GFrame*)m_pObjUnitTree->Nodes()[l_iIndex]->Item())->Child(L"lblQty"))->Text(GString(l_iQty3));

         l_iIndex++;
      }


   }

	if(!l_bInfantryInformation && !l_bAirInformation && !l_bNavalInformation && !l_bGroundInformation)
	{
		GTreeBranch* l_pNode = m_pObjUnitTree->CreateNode();
		GUI::GFrame* l_pItem = (GUI::GFrame*)l_pNode->Item();
		l_pItem->ColorNormal(GColorRGBInt(0,0,0,0));
		((GUI::GLabel*)l_pItem->Child(L"lblName"))->Text(g_ClientDAL.GetString(101291));
		((GUI::GLabel*)l_pItem->Child(L"txtStatus"))->Hide();
      ((GUI::GLabel*)l_pItem->Child(L"lblStatus"))->Hide();
		m_pObjUnitTree->Nodes().add(l_pNode);
	}		

	if(l_iCountryID != 0)
	{
		REAL32 l_fAccuracy = g_ClientDAL.m_PlayerCountryData.AccuracyInformation(l_iCountryID);
		m_pObjAccuracy->Value(l_fAccuracy, 0, false, false);
		m_pObjAccuracyTxt->Text(GString::FormatNumber(l_fAccuracy * 100.f, L"", L"", L"", L" %", 0, 0));
	}

   return true;
}

void GUnitListWindow::GenerateUnitTree()
{
   if(m_pObjUnitTree)
   {
      m_pObjUnitTree->Hide();
      g_Joshua.GUIManager()->ReleaseObject(m_pObjUnitTree);
      m_pObjUnitTree = NULL;
   }

   GTreeView * tree = CREATE_GUI_OBJECT_NAMED_VISIBLE(GTreeView, L"treeUnits", m_pObjTreeFrm);   

   m_pObjUnitTree = tree;
   m_pObjUnitTree->Owner(this);
   m_pObjUnitTree->ItemType(L"lstUnitListItem");
   m_pObjUnitTree->Position(0,0);
   
   Update();

   m_pObjTreeFrm->Height(40 * (INT16)m_pObjUnitTree->CountVisible());
   m_pObjUnitTree->Size(m_pObjTreeFrm->Width(),m_pObjTreeFrm->Height());
   m_pObjUnitTree->StartNode(0);
   m_pObjUnitTree->ItemIndent(15);
   m_pObjUnitTree->ItemHeight(40);
   m_pObjUnitTree->ImageSize(16,16);
   m_pObjUnitTree->SetImage(GUI::ETI_Branch,     L"images\\new\\tree_branch.RC#000225.tga");
   m_pObjUnitTree->SetImage(GUI::ETI_Collapse,   L"images\\new\\tree_minus.RC#000223.tga");
   m_pObjUnitTree->SetImage(GUI::ETI_Empty,      L"images\\new\\tree_empty.RC#000227.tga");
   m_pObjUnitTree->SetImage(GUI::ETI_Expand,     L"images\\new\\tree_plus.RC#000231.tga");
   m_pObjUnitTree->SetImage(GUI::ETI_Last,       L"images\\new\\tree_end.RC#000229.tga");
   m_pObjUnitTree->SetImage(GUI::ETI_Leaf,       L"images\\new\\tree_leaf.RC#000235.tga");
   m_pObjUnitTree->SetImage(GUI::ETI_Trunk,      L"images\\new\\tree_trunk.RC#000233.tga");
   m_pObjUnitTree->ColorNormal(L"RGBA: 255, 0, 0",L"0");
   m_pObjUnitTree->Update();
   m_pObjUnitTree->Show();

   m_pObjTreeScr->Initialize(this, 0, max(m_pObjTreeFrm->Height() - m_pObjTreeFrm->Owner()->Height(),0),40,120);
}

GTreeBranch* GUnitListWindow::PrepareTreeItem(GTreeBranch* node, GString in_sUnitName, INT32 in_iUnitQty, SP2::EMilitaryStatus::Enum in_Status ,INT32 in_iUnitId)
{
   GUI::GFrame* l_pItem = (GUI::GFrame*)node->Item();
   l_pItem->ColorNormal(GColorRGBInt(0,0,0,0));
	
	((GUI::GLabel*)l_pItem->Child(L"lblName"))->Text(in_sUnitName);

	if(in_iUnitQty > 0)
		((GUI::GLabel*)l_pItem->Child(L"lblQty"))->Text(GString(in_iUnitQty));
	else
		((GUI::GLabel*)l_pItem->Child(L"lblQty"))->Text(L"???");

   
   if(in_iUnitId > 0)
   {
      SP2::GUnit* l_pUnit = (SP2::GUnit* ) g_Joshua.UnitManager().Unit(in_iUnitId);
      if(l_pUnit->Training() > ETrainingLevel::Recruit)
      {
         gassert(l_pUnit->Training() <= ETrainingLevel::Elite,"Training should not be higher than elite");
         ((GUI::GPictureBox*)l_pItem->Child(L"picTraining"))->PictureNormal(m_UnitTrainingIDLogo[l_pUnit->Training()]);
         ((GUI::GPictureBox*)l_pItem->Child(L"picTraining"))->BringToFront();
      }

      if(g_ClientDAL.IsUnitForSale(in_iUnitId))
      {
         l_pItem->Child(L"picSell")->Show();
      }
      else
      {
         l_pItem->Child(L"picSell")->Hide();
      }
   }
   else
   {
      l_pItem->Child(L"picSell")->Hide();
   }

   GString l_sStatus;
   switch(in_Status)
   {
   case SP2::EMilitaryStatus::Ready:
      l_sStatus = g_ClientDAL.GetString(100243);
      ((GUI::GLabel*)l_pItem->Child(L"txtStatus"))->FontColorNormal(GColorRGBInt(117,219,94,255));
      break;
   case SP2::EMilitaryStatus::Fortified:
      l_sStatus = g_ClientDAL.GetString(100241);
      ((GUI::GLabel*)l_pItem->Child(L"txtStatus"))->FontColorNormal(GColorRGBInt(242,227,124,255));
      break;
   case SP2::EMilitaryStatus::Parked:
      l_sStatus = g_ClientDAL.GetString(100242);
      ((GUI::GLabel*)l_pItem->Child(L"txtStatus"))->FontColorNormal(GColorRGBInt(174,205,255,255));
      break;
   }
   if(in_iUnitQty == 0)
   {
      ((GUI::GLabel*)l_pItem->Child(L"txtStatus"))->Hide();
      ((GUI::GLabel*)l_pItem->Child(L"lblStatus"))->Hide();
   }
   else
      ((GUI::GLabel*)l_pItem->Child(L"txtStatus"))->Text(l_sStatus);

   node->Value((void*)in_iUnitId);
   
   return node;
}


GUI::EEventStatus::Enum GUnitListWindow::OnCustomEvent(UINT32 in_EventID, const GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{   
   switch (in_EventID)
   {
   case ECustomEvents::OnNodeCollapse:
   case ECustomEvents::OnNodeExpand:
   case ECustomEvents::OnNodeSelected:
   case ECustomEvents::OnNodeUnSelected:
      break;   
   case ECustomEvents::OnScrollVertical:
      if(in_pCaller == m_pObjTreeScr)
      {
         INT32 l_iValue = m_pObjTreeScr->Value();
         
         if((m_pObjTreeFrm->Height() - m_pObjTreeFrm->Owner()->Height()) < l_iValue)
            l_iValue = (m_pObjTreeFrm->Height() - m_pObjTreeFrm->Owner()->Height());
         if(l_iValue < 0)
            l_iValue = 0;

         m_pObjTreeFrm->Top(-(INT16)l_iValue);
      }
      return GUI::EEventStatus::Handled;
   default: 
      return GBaseObject::OnCustomEvent(in_EventID, in_EventData, in_pCaller);
   }


   

   switch (in_EventID)
   {
      case ECustomEvents::OnScrollVertical:
         break;

      case ECustomEvents::OnNodeCollapse:
         {
            m_pObjTreeFrm->Height(40 * (INT16)m_pObjUnitTree->CountVisible());
            m_pObjUnitTree->Size(m_pObjTreeFrm->Width(),m_pObjTreeFrm->Height());
            m_pObjTreeScr->Maximum(max(m_pObjTreeFrm->Height() - m_pObjTreeFrm->Owner()->Height(),0));
            
            INT32 l_iValue = m_pObjTreeScr->Value();
            
            if((m_pObjTreeFrm->Height() - m_pObjTreeFrm->Owner()->Height()) < l_iValue)
               l_iValue = (m_pObjTreeFrm->Height() - m_pObjTreeFrm->Owner()->Height());
            if(l_iValue < 0)
               l_iValue = 0;

            m_pObjTreeScr->Value(l_iValue);

            if((m_pObjTreeFrm->Height() - m_pObjTreeFrm->Owner()->Height()) <= 0)
            {
               GEventData l_Event;
               l_Event.Data = (void*)0;
               OnCustomEvent(ECustomEvents::OnScrollVertical, l_Event, m_pObjTreeScr);         
            }         
         }
         break;
      case ECustomEvents::OnNodeExpand:
         m_pObjTreeFrm->Height(40 * (INT16)m_pObjUnitTree->CountVisible());
         m_pObjUnitTree->Size(m_pObjTreeFrm->Width(),m_pObjTreeFrm->Height());
         m_pObjTreeScr->Maximum(max(m_pObjTreeFrm->Height() - m_pObjTreeFrm->Owner()->Height(),0));
         break;
      case ECustomEvents::OnNodeSelected:
      {
         GTreeBranch * l_pNode = (GTreeBranch*)((GUI::GTreeView*)in_EventData.Data)->SelectedNode();
         ((GUI::GFrame*)l_pNode->Item())->ColorNormal(GColorRGBInt(60,60,60,255));
         m_iSelectedUnitId = (INT32) l_pNode->Value();
         m_pSelectedItem = (GUI::GFrame*)l_pNode->Item();
         break;
      }
      case ECustomEvents::OnNodeUnSelected:
      {
         GTreeBranch * l_pNode = (GTreeBranch*)((GUI::GTreeView*)in_EventData.Data)->SelectedNode();
         ((GUI::GFrame*)l_pNode->Item())->ColorNormal(GColorRGBInt(20,20,20,255));
         m_pSelectedItem = 0;
         break;
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GUnitListWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjSplitBtn)
         {
            if(!g_ClientDDL.UnitSplitWindow())
               g_ClientDDL.UnitSplitWindowSpawn();

            g_ClientDDL.UnitSplitWindow()->Show();
         }
         if(in_pCaller == m_pObjTrainBtn)
         {
            if(m_iSelectedUnitId >= 0)
            {
               // Build a list of units to train
               list<UINT32> l_UnitList;
               l_UnitList.push_back(m_iSelectedUnitId);
               // Old unit training window (will be changed anyway)
               //g_ClientDCL.TrainUnits(l_UnitList, this, this);
            }
         }
         else if(in_pCaller == m_pObjSellBtn)
         {
            if(m_iSelectedUnitId >= 0)
            {

               // If this unit 'was' for sale, remove icon
               if(g_ClientDAL.IsUnitForSale(m_iSelectedUnitId))
               {
                  m_pSelectedItem ->Child(L"picSell")->Hide();
               }
               else
               {
                  m_pSelectedItem ->Child(L"picSell")->Show();
               }

               SellUnit(m_iSelectedUnitId);
            }
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GUnitListWindow::OnMouseWheel(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   GUI::GBaseObject* l_pObject = in_EventData.InitialRecipient; 
   
   while(l_pObject != m_pObjTreeFrm->Owner() && l_pObject != NULL) 
      l_pObject = l_pObject->Parent();

   if(l_pObject == m_pObjTreeFrm->Owner())
   {
      m_pObjTreeScr->Value(m_pObjTreeScr->Value() - (in_EventData.Mouse.WheelDelta * 40));
      m_pObjTreeFrm->Top(-(INT16)m_pObjTreeScr->Value());
   }

   return GUI::EEventStatus::Handled;
}

void GUnitListWindow::UpdateFromSelectedUnits(bool in_bState)
{
   m_bUpdateFromSelectedUnits = in_bState;
}


void GUnitListWindow::SellUnit(UINT32 in_iUnitId)
{
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GTransfertUnit);
   SP2::Event::GTransfertUnit* l_pTransfertUnit = (SP2::Event::GTransfertUnit*) (l_pEvent.get() );

   // Set Target and Source for this event
   l_pTransfertUnit->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pTransfertUnit->m_iSource = g_SP2Client->Id();
   l_pTransfertUnit->m_bForSale= true;
   l_pTransfertUnit->m_iQty    = 0;
   l_pTransfertUnit->m_iUnitId = in_iUnitId;

   // Fire event
   g_Joshua.RaiseEvent(l_pEvent);	
}