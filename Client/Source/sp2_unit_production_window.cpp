/**************************************************************
*
* sp2_unit_production_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Francois Durand 
*  Implementation : Jonathan Mercier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

// Includes
#include "golem_pch.hpp"
#include "../include/sp2_unit_production_window.h"
#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_gui_helper.h"
#include "../include/sp2_design_window.h"


// Constants
const GColorRGBInt c_colorHighPriority(233,99,99,255);
const GColorRGBInt c_colorMediumPriority(219,212,107,255);
const GColorRGBInt c_colorLowPriority(47,109,152,255);
const GColorRGBInt c_colorHold(200,200,200,255);
const GColorRGBInt c_colorCancel(255,255,255,255);
const GColorRGBInt c_colorSelectedItem(35,35,35,255);
const GColorRGBInt c_colorUnselectedItem(20,20,20,255);
const GColorRGBInt c_colorItemIsNotBeingBuilt(64,64,64,255);
const GColorRGBInt c_colorItemIsBeingBuilt(255,255,255,255);


const UINT32       c_iMaximumOrder = 50;
const UINT32       c_iTreeViewItemHeight = 15;

const GString GUnitProductionWindow::TypeName(L"Unit_Production_Window");

GUI::GBaseObject* GUnitProductionWindow::New()
{
   return new GUnitProductionWindow();
}

GUnitProductionWindow::GUnitProductionWindow() : GBaseWindow()
{
   // Dynamicly build objects
   m_pObjDesignTree  = NULL;
   m_pSelectedDesign = NULL;
   m_iCurrentInfantryUnitsInProduction = 0;

   // Clear ETAs
   for(UINT32 i =0 ;i<SP2::EUnitProductionPriority::PriorityCount;i++)
   {
      m_iLastETAByPriority[i] = 0;
   }


}

GUnitProductionWindow::~GUnitProductionWindow()
{
   g_Joshua.UnitManager().Detach(this);
}

bool GUnitProductionWindow::OnCreate()
{
   __super::OnCreate();
 
   // Collect pointers
   GUI::GBaseObject* l_pDesignFrm = Child(L"frmDesignList");
   GUI::GBaseObject* l_pProductionFrm = Child(L"frmRight")->Child(L"frmProduction");
   
   m_pObjProductionList          = (GUI::GListBox*)l_pProductionFrm->Child(L"lstProduction");
   m_pObjUnitNameLbl             = (GUI::GLabel*)l_pDesignFrm->Child(L"frmUnitName")->Child(L"txtUnitName");
   m_pObjPriorityCbo             = (GUI::GComboBox*)l_pDesignFrm->Child(L"frmInfo")->Child(L"cboPriority");
   m_pObjProductionSpeedCbo      = (GUI::GComboBox*)l_pDesignFrm->Child(L"frmInfo")->Child(L"cboSpeed");
   m_pObjUnitCostLbl             = (GUI::GLabel*)l_pDesignFrm->Child(L"frmCost")->Child(L"txtUnitCost");
   m_pObjBuildingCountryCbo      = (SP2::GComboListBoxLess*)l_pDesignFrm->Child(L"frmDesignList")->Child(L"cboCountry");
   
   m_pObjBuildingCountryCbo->SetListBox(g_ClientDDL.CountriesListBox());

   m_pObjTotalPossibleUnit       = (GUI::GLabel*)l_pDesignFrm->Child(L"frmCost")->Child(L"lblTotalUnits");
   m_pObjUnitCostLabel           = (GUI::GLabel*)l_pDesignFrm->Child(L"frmCost")->Child(L"lblUnitCost");
   m_pObjQuantity                = (SP2::GUpdatableValue*)l_pDesignFrm->Child(L"frmCost")->Child(L"frmQty");
   m_pObjTotalCostLbl            = (GUI::GLabel*)l_pDesignFrm->Child(L"frmCost")->Child(L"txtTotalCost");
   
   m_pObjBuildBtn                = (GUI::GButton*)l_pDesignFrm->Child(L"btnBuild");
   m_pObjAcceptForeignOrderTog   = (GUI::GToggle*)l_pProductionFrm->Child(L"togAcceptForeign");

   GUI::GBaseObject* l_pFilterFrm   = l_pDesignFrm->Child(L"frmFilter");
   m_pObjFilter[SP2::EUnitCategory::Ground]  = (GUI::GToggle*)l_pFilterFrm->Child(L"togGround");
   m_pObjFilter[SP2::EUnitCategory::Air]     = (GUI::GToggle*)l_pFilterFrm->Child(L"togAir");
   m_pObjFilter[SP2::EUnitCategory::Naval]   = (GUI::GToggle*)l_pFilterFrm->Child(L"togNaval");
   m_pObjFilter[SP2::EUnitCategory::Nuclear] = (GUI::GToggle*)l_pFilterFrm->Child(L"togStrategic");

   m_pObjCapacityTxt    = (GUI::GLabel*)l_pProductionFrm->Child(L"txtValue");
   m_pObjCapacityFrm    = (GCursorMeter*)l_pProductionFrm->Child(L"frmCapacity");
   m_pObjCapacityFrm->RightBound(361); 
   m_pObjCapacityFrm->MeterBtn()[0]->PictureNormal(GString(SP2::PIC_GRADIENT_GREEN));

   m_pObjDesignFrm              = (GUI::GFrame*)l_pDesignFrm->Child(L"frmDesignList")->Child(L"frmList");
   m_pObjDesignScr              = (GUI::GVScrollBar*)l_pDesignFrm->Child(L"frmDesignList")->Child(L"scrList");
   m_pObjDesignInnerFrm         = (GUI::GFrame*)m_pObjDesignFrm->Child(L"frmInner");
   m_pObjTreeViewContainer      = m_pObjDesignInnerFrm;
   

   m_pObjEtaLabel               = (GUI::GLabel*)l_pDesignFrm->Child("frmEta")->Child("txtEta");

   m_pObjQuantity->TextValueObj()->TextAlignment(L"MiddleRight");

   // Hide this label
   m_pObjTotalPossibleUnit->Hide();

   // Select building country 
   m_iSelectedBuildingCountry = g_ClientDAL.ControlledCountryID();
   

   // Clip tree view.
   ((GFX::GWindow *)m_pObjDesignFrm->Model()->Get_Primitive())->ClipChildren(true);

   l_pDesignFrm->Child(L"picBg")->SendToBack();
   l_pDesignFrm->BringToFront();
  
   vector<UINT32> m_viStep;
   vector<REAL64> m_vfStep;
   m_viStep.push_back(1);
   m_viStep.push_back(9);
   m_viStep.push_back(99);
   m_vfStep.push_back(1);
   m_vfStep.push_back(10);
   m_vfStep.push_back(100);

   m_pObjQuantity->Initialize(m_viStep, m_vfStep, L"", L"", 0, 3, 0, 1000000);
   // Set Qty value to default 
   m_pObjQuantity->Value(50);

   // Build Tree 
   m_pObjDesignTree =  GUIHelper::CreateDefaultTreeView(m_pObjTreeViewContainer,this);

   // Setup logo 
   m_UnitDesignIDLogo[SP2::EUnitCategory::Infantry]= "000823";
   m_UnitDesignIDLogo[SP2::EUnitCategory::Air]     = "000778";
   m_UnitDesignIDLogo[SP2::EUnitCategory::Ground]  = "000779";
   m_UnitDesignIDLogo[SP2::EUnitCategory::Naval]   = "000780";
   m_UnitDesignIDLogo[SP2::EUnitCategory::Nuclear] = "000781";

   // Strings
   m_TextPriority[SP2::EUnitProductionPriority::OnHold]  = g_ClientDAL.GetString(101138);//"HOLD";
   m_TextPriority[SP2::EUnitProductionPriority::Low]     = g_ClientDAL.GetString(101139);//"LOW";
   m_TextPriority[SP2::EUnitProductionPriority::Normal]  = g_ClientDAL.GetString(101140);//"MEDIUM";
   m_TextPriority[SP2::EUnitProductionPriority::High]    = g_ClientDAL.GetString(101141);//"HIGH";



   // Fill out Priority Box
   GListBox* l_pListBox    = m_pObjPriorityCbo->GetListBox();
   GListBox::GRow* l_pRow  = l_pListBox->Insert_New_Row();
   GUI::GLabel* l_pLabel   = (GUI::GLabel*)l_pRow->Object();
   l_pLabel->Text(m_TextPriority[SP2::EUnitProductionPriority::High]);
   l_pLabel->FontColorNormal(c_colorHighPriority);

   l_pRow  = l_pListBox->Insert_New_Row();
   l_pLabel   = (GUI::GLabel*)l_pRow->Object();
   l_pLabel->Text(m_TextPriority[SP2::EUnitProductionPriority::Normal]);
   l_pLabel->FontColorNormal(c_colorMediumPriority);
   
   l_pRow  = l_pListBox->Insert_New_Row();
   l_pLabel   = (GUI::GLabel*)l_pRow->Object();
   l_pLabel->Text(m_TextPriority[SP2::EUnitProductionPriority::Low]);
   l_pLabel->FontColorNormal(c_colorLowPriority);

   m_pObjPriorityCbo->Selected_Content(m_TextPriority[SP2::EUnitProductionPriority::High]);
   GUI::GEditBox* l_pEditBox = (GUI::GEditBox *)m_pObjPriorityCbo->Child("Edit");
   l_pEditBox->FontColorNormal(c_colorHighPriority);
   l_pEditBox->FontColor(c_colorHighPriority);


   // Fill out Production speed box
   l_pListBox    = m_pObjProductionSpeedCbo->GetListBox();
   l_pRow  = l_pListBox->Insert_New_Row();
   l_pLabel   = (GUI::GLabel*)l_pRow->Object();
   l_pLabel->Text(g_ClientDAL.GetString(EStrId::Normal));

   l_pRow  = l_pListBox->Insert_New_Row();
   l_pLabel   = (GUI::GLabel*)l_pRow->Object();
   l_pLabel->Text(g_ClientDAL.GetString(EStrId::Fast));


   // Select a default production speed
   m_pObjProductionSpeedCbo->Selected_Content(g_ClientDAL.GetString(EStrId::Normal));

   // Attach to UnitManager
   g_Joshua.UnitManager().Attach(this,SDK::Combat::c_notificationNewUnitDesign);

   // Generate List
   GenerateUnitDesignList();

   // Select a default category
   SelectedCategory(SP2::EUnitCategory::Ground);

   // Set capacity to 100%
   Capacity(1);

   return true;
}

void GUnitProductionWindow::OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   // On new design
   if(in_Notification == SDK::Combat::c_notificationNewUnitDesign)
   {
      // Regenerate design list
      GenerateUnitDesignList();

      // reselect current category 
      SelectedCategory(m_eSelectedCategory);
   }
}


void GUnitProductionWindow::BuildUnitDesignTree(SP2::EUnitCategory::Enum in_CategoryToDisplay)
{

   // To clean the design tree, destroy everything
   if(m_pObjDesignTree)
   {
      m_pObjDesignTree->Hide();
      g_Joshua.GUIManager()->ReleaseObjectAsync(m_pObjDesignTree);
      m_pObjDesignTree = NULL;
   }

   // Then recreate it.
   m_pObjDesignTree =  GUIHelper::CreateDefaultTreeView(m_pObjTreeViewContainer,this);

   INT32 l_iLastId      = -1;
   INT32 l_iNodeCounter = -1;

   // Build tree from design map
   multimap<UINT32,SP2::GUnitDesign*>::iterator l_ItUnitDesign = m_UnitDesignList[in_CategoryToDisplay].begin();
   while(l_ItUnitDesign != m_UnitDesignList[in_CategoryToDisplay].end())
   {
      SP2::GUnitDesign* l_pUnitDesign = (SP2::GUnitDesign*)l_ItUnitDesign->second;

      INT32 l_iUnitTypeId = (INT32)l_pUnitDesign->Type()->Id();
      if(l_iUnitTypeId != l_iLastId)
      {
         m_pObjDesignTree->Nodes().add(GUIHelper::PrepareTreeItem(m_pObjDesignTree->CreateNode(),NULL,l_pUnitDesign->Type()->Name()));
         l_iLastId = l_iUnitTypeId;
         l_iNodeCounter++;
      }

      m_pObjDesignTree->Nodes()[l_iNodeCounter]->Nodes().add(GUIHelper::PrepareTreeItem(m_pObjDesignTree->CreateNode(),l_pUnitDesign,l_pUnitDesign->Name()));

      l_ItUnitDesign++;
   }

   // Init tree view with scroll bar
   m_pObjTreeViewContainer->Height(15 * (INT16)m_pObjDesignTree->CountVisible());
   m_pObjDesignTree->Size(m_pObjTreeViewContainer->Width(),m_pObjTreeViewContainer->Height());
   m_pObjDesignScr->Initialize(this, 0,max(m_pObjTreeViewContainer->Height() - m_pObjTreeViewContainer->Owner()->Height(),0),15,45);
   m_pObjTreeViewContainer->Top(0);
   m_pObjDesignTree->Show();
}

void GUnitProductionWindow::GenerateUnitDesignList(void)
{

   // clear Design list
   for(UINT32 i=0;i<EUnitCategory::ItemCount;i++)
   {
      m_UnitDesignList[i].clear();
   }

   // Iterate over design  and gets our design and the one that designerID == 0 (whatever that mean)
   const hash_map<UINT32,SDK::Combat::Design::GUnit*>& l_Designs = g_Joshua.UnitManager().RegisteredUnitDesigns();
   hash_map<UINT32,SDK::Combat::Design::GUnit*>::const_iterator l_ItUnitDesign = l_Designs.begin();
   while(l_ItUnitDesign != l_Designs.end())
   {
      SP2::GUnitDesign* l_pUnitDesign  = (SP2::GUnitDesign*)l_ItUnitDesign->second;
      UINT32 l_iTypeId                 = l_pUnitDesign->Type()->Id();
      UINT32 l_iDesignerId             = l_pUnitDesign->DesignerID();

      if(l_iDesignerId == 0 || l_iDesignerId == (UINT32) m_iSelectedBuildingCountry)
      {

         // We want all the infantry to go into ground.
         if(l_pUnitDesign->Type()->Category() == EUnitCategory::Infantry)
         {
            // Only have infantry for our country
            if(m_iSelectedBuildingCountry == g_ClientDAL.ControlledCountryID())
               m_UnitDesignList[EUnitCategory::Ground].insert(multimap<UINT32,SP2::GUnitDesign*>::value_type(l_iTypeId,l_pUnitDesign));
         }
         else
         {

            if(l_pUnitDesign->Type()->Category() == EUnitCategory::Nuclear)
            {
               if(m_iSelectedBuildingCountry == g_ClientDAL.ControlledCountryID() && g_ClientDAL.m_PlayerCountryData.NuclearReady() >=1.0f)
                  m_UnitDesignList[l_pUnitDesign->Type()->Category()].insert(multimap<UINT32,SP2::GUnitDesign*>::value_type(l_iTypeId,l_pUnitDesign));
            }else
            {
               m_UnitDesignList[l_pUnitDesign->Type()->Category()].insert(multimap<UINT32,SP2::GUnitDesign*>::value_type(l_iTypeId,l_pUnitDesign));
            }
            
         }
      }
      l_ItUnitDesign++;
   }
}




GUI::EEventStatus::Enum GUnitProductionWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         // Capture value update event
         // Assuming there is only one btnUp\btDown in the window
         if( (in_pCaller->Name() == "btnUp") || (in_pCaller->Name() == "btnDown") )
         {
            UpdateSelectedDesign(m_pSelectedDesign);
         }
      }
   }

   return __super::OnMouseDown(in_EventData, in_pCaller);
}

GUI::EEventStatus::Enum GUnitProductionWindow::OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller->Name() == "btnExit")
         {
            Hide();
         }
         else if(in_pCaller->Name() == "togGround")
         {
            SelectedCategory(SP2::EUnitCategory::Ground);
         }
         else if(in_pCaller->Name() == "togAir")
         {
            SelectedCategory(SP2::EUnitCategory::Air);
         }
         else if(in_pCaller->Name() == "togNaval")
         {
            SelectedCategory(SP2::EUnitCategory::Naval);
         }
         else if(in_pCaller->Name() == "togStrategic")
         {
            // Generate List for missile and update
            SelectedCategory(SP2::EUnitCategory::Nuclear);
         }
         else if(in_pCaller->Name() == "btnBuild")
         {
            gassert(m_pSelectedDesign,"Build button should have been disabled");
            RequestBuildOrder();
         }
         else if(in_pCaller->Name() == m_pObjAcceptForeignOrderTog->Name())
         {
            AcceptForeignOrder(m_pObjAcceptForeignOrderTog->ToggleState());
         }
      }
   }

   return __super::OnMouseClick(in_EventData, in_pCaller);   
}

GUI::EEventStatus::Enum GUnitProductionWindow::OnCustomEvent(UINT32 in_EventID, const GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   // Node expand / colasp functionnaly 
   switch (in_EventID)
   {
      case ECustomEvents::OnNodeCollapse:
      {
         m_pObjTreeViewContainer->Height(c_iTreeViewItemHeight * (INT16)m_pObjDesignTree->CountVisible());
         m_pObjDesignTree->Size(m_pObjTreeViewContainer->Width(),m_pObjTreeViewContainer->Height());
         m_pObjDesignScr->Maximum(max(m_pObjTreeViewContainer->Height() - m_pObjTreeViewContainer->Owner()->Height(),0));
         if(m_pObjDesignScr->Value() > m_pObjDesignScr->Maximum())
            m_pObjDesignScr->Value(m_pObjDesignScr->Maximum());

         if((m_pObjTreeViewContainer->Height() - m_pObjTreeViewContainer->Owner()->Height()) <= 0)
         {
            GEventData l_Event;
            l_Event.Data = (void*)0;
            OnCustomEvent(ECustomEvents::OnScrollVertical, l_Event, m_pObjDesignScr);         
         }
         break;
      }
      case ECustomEvents::OnNodeExpand:
      {
         m_pObjTreeViewContainer->Height(c_iTreeViewItemHeight * (INT16)m_pObjDesignTree->CountVisible());
         m_pObjDesignTree->Size(m_pObjTreeViewContainer->Width(),m_pObjTreeViewContainer->Height());
         m_pObjDesignScr->Maximum(max(m_pObjTreeViewContainer->Height() - m_pObjTreeViewContainer->Owner()->Height(),0));
         break;
      }
      case ECustomEvents::OnNodeSelected:
      {
         GTreeBranch * l_pNode = (GTreeBranch*)((GUI::GTreeView*)in_EventData.Data)->SelectedNode();
         ((GUI::GLabel*)l_pNode->Item())->ColorNormal(c_colorSelectedItem);
         SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)l_pNode->Value();
         UpdateSelectedDesign(l_pDesign);
      }
      break;
   case ECustomEvents::OnNodeUnSelected:  
      {
         GTreeBranch * l_pNode = (GTreeBranch*)((GUI::GTreeView*)in_EventData.Data)->SelectedNode();
         ((GUI::GLabel*)l_pNode->Item())->ColorNormal(c_colorUnselectedItem);
         UpdateSelectedDesign(NULL);
      }
      break;
   case c_OnUpdatableValueChange:
      {
         UpdateSelectedDesign(m_pSelectedDesign);
      }
      break;
   case ECustomEvents::OnScrollVertical:
      if(in_pCaller == m_pObjDesignScr)
         m_pObjTreeViewContainer->Top(-(INT16)in_EventData.Data);

      return GUI::EEventStatus::Handled;
   //Treenode doubleclicked
   case ECustomEvents::OnNodeDoubleClicked:
   {
      SDK::GUI::GTreeView* l_pTreeView = (SDK::GUI::GTreeView*)in_EventData.Data;
      gassert(l_pTreeView,"Invalid treeview pointer");
      
      //Double click on the item means that we must spawn the unit design window and display the actual design
      SDK::GUI::GTreeBranch* l_pSelectedItem = l_pTreeView->SelectedNode();
      
      //Spawn and show the design window with the selected design
      {
         // retrieve the unitdesgin ptr
         SP2::GUnitDesign* l_pUnitDesign = (SP2::GUnitDesign*)l_pSelectedItem->Value();

         if(!l_pUnitDesign)
         {          
            //Double clicked node is probably a unit type category
         }
         else
         {
            gassert(l_pUnitDesign,"Invalid unitdesign pointer");

            // cannot display infantry info
            if(l_pUnitDesign->Type()->Id() != EUnitType::Infantry)
            {
               g_ClientDDL.DesignWindowSpawn(l_pUnitDesign);
               if(!g_ClientDDL.DesignWindow()->Visible())
                  g_ClientDDL.DesignWindow()->Show();
               g_ClientDDL.DesignWindow()->BringToFront();

               //Display the window at the right of the screen
               CGFX_Renderer_Init_Mode l_Mode;
               g_Joshua.Renderer()->Get_Current_Mode(&l_Mode);
               g_ClientDDL.DesignWindow()->Left(l_Mode.m_Resolution.x-g_ClientDDL.DesignWindow()->Width());
            }
         }
      }
      break;
   }
   case ECustomEvents::OnComboBoxBeforeOpen:
      {
         if(in_EventData.Data == m_pObjBuildingCountryCbo)
         {
            SP2::GCountries l_Countries;
            
            for(set<UINT32>::iterator l_It = m_BuildingCountryIdList.begin(); l_It != m_BuildingCountryIdList.end(); l_It++)
            {
               l_Countries.push_back( g_ClientDAL.Country(*l_It) );
            }

            g_ClientDDL.ComboboxSetCountriesAndFlag(l_Countries, (GUI::GComboBox*)in_EventData.Data, true);
         }

         return GUI::EEventStatus::Handled;
      }
   case ECustomEvents::OnComboBoxSelectionEvent:
      {
         GUI::GComboBox* l_pComboBox = (GUI::GComboBox*)in_EventData.Data;
         // Priority List of the current unit to build
         if(l_pComboBox == m_pObjPriorityCbo)
         {
            SetComboboxColorFromText(m_pObjPriorityCbo);
            // Update Selected design, price and ETa, might have change.
            if(m_pSelectedDesign)
               UpdateSelectedDesign(m_pSelectedDesign);
         }
         else if(l_pComboBox == m_pObjProductionSpeedCbo)
         {
            if(m_pSelectedDesign)
               UpdateSelectedDesign(m_pSelectedDesign);
         }
         else if(l_pComboBox == m_pObjBuildingCountryCbo)
         {
            // Get selected country id
            const GCountry&    l_country =   g_ClientDAL.Country(m_pObjBuildingCountryCbo->Selected_Content());

            // Update Selected country if it has changed.
            if(m_iSelectedBuildingCountry != l_country.Id())
            {
               m_iSelectedBuildingCountry = l_country.Id();
               GenerateUnitDesignList();
               SelectedCategory(m_eSelectedCategory);
            }
         }
         else
         // Priority list of the production queue
         {
            //Get the selected list item
            GUI::GBaseObject* l_pItem = l_pComboBox->Parent();
            //Find the Row
            for(INT32 i = 0 ; i < m_pObjProductionList->Get_Nb_Rows() ; i++)
            {
               GUI::GListBox::GRow* l_pRow = m_pObjProductionList->Get_Row_At(i);
               if(l_pRow->m_pObject == l_pItem)
               {
                  ChangePriorityOfOrder(l_pRow);
                  return GUI::EEventStatus::Handled;
               }
            }
         }
      }
      break;
   default: 
      return GBaseObject::OnCustomEvent(in_EventID, in_EventData, in_pCaller);      
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GUnitProductionWindow::OnMouseWheel(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   GUI::GBaseObject* l_pTemp = in_EventData.InitialRecipient;
   
   while(l_pTemp && l_pTemp != m_pObjTreeViewContainer)
   {
      l_pTemp = l_pTemp->Parent();
   }

   if(l_pTemp == m_pObjTreeViewContainer)
   {
      m_pObjDesignScr->OnMouseWheel(in_EventData, NULL);
   }

   return GUI::EEventStatus::Handled;
}

void GUnitProductionWindow::UpdateSelectedDesign(SP2::GUnitDesign *in_pUnitDesign)
{

   m_pSelectedDesign = in_pUnitDesign;

   if(in_pUnitDesign)
   {

      // Enable build button
      if(m_pObjProductionList->Get_Nb_Rows()<c_iMaximumOrder)
         m_pObjBuildBtn->Enabled(true);


      //If the Unit Design Window is displayed and not military unit desing,
      // update it with the selected design
      if(g_ClientDDL.DesignWindow() && g_ClientDDL.DesignWindow()->Visible() && 
         (in_pUnitDesign->Type()->Id() != EUnitType::Infantry))
      {         
         g_ClientDDL.DesignWindow()->SelectedDesign(in_pUnitDesign);
         g_ClientDDL.DesignWindow()->BringToFront();
      }

      // Update design name
      m_pObjUnitNameLbl->Text(in_pUnitDesign->Name());

      // Calculate Unit cost
      REAL64 l_fUnitCost = (float)in_pUnitDesign->Cost();



      // Set proper caption on the buttons and label
      if(in_pUnitDesign->Type()->Category() == EUnitCategory::Infantry)
      {
         m_pObjBuildBtn->Text(g_ClientDAL.GetString(EStrId::Enroll));
         m_pObjTotalPossibleUnit->Show();
         m_pObjUnitCostLabel->Hide();
 
         REAL64 l_fTotalUnitPossible = (REAL64)g_ClientDAL.m_PlayerCountryData.MiliManpowerAvailable()- m_iCurrentInfantryUnitsInProduction;
         l_fTotalUnitPossible = max(l_fTotalUnitPossible,0);
         m_pObjUnitCostLbl->Text(GString::FormatNumber(l_fTotalUnitPossible, L" ", L".", L"", L"", 3, 0));

         m_pObjQuantity->Maximum(l_fTotalUnitPossible);
         
      }
      else
      {
         m_pObjBuildBtn->Text(g_ClientDAL.GetString(EStrId::Build));
         m_pObjTotalPossibleUnit->Hide();
         m_pObjUnitCostLabel->Show();
      
         m_pObjUnitCostLbl->Text(GString::FormatNumber(l_fUnitCost, L" ", L".", L"", L"", 3, 0));
         m_pObjQuantity->Maximum(1000000);
      }

      // Calculate Total cost
      UINT16 l_iProductionSpeed = GetProductionSpeedFromCombobox(m_pObjProductionSpeedCbo);
      REAL64 l_fTotalCost = (float)(l_fUnitCost * m_pObjQuantity->Value() * g_ClientDAL.ProductionSpeedCostModificator(l_iProductionSpeed));
      if(m_iSelectedBuildingCountry != g_ClientDAL.ControlledCountryID())
         l_fTotalCost*=  g_ClientDAL.SellingUnitCostRatio();
      m_pObjTotalCostLbl->Text(GString::FormatNumber(l_fTotalCost, L" ", L".", L"", L"", 3, 0));

      // Calculate ETA
      m_pObjEtaLabel->Text(GString(CalculateETA()) + g_ClientDAL.GetString(EStrId::EtaDays));

   
   } 
   else
   {
      m_pObjUnitNameLbl->Text("");
      m_pObjUnitCostLbl->Text("");
      m_pObjTotalCostLbl->Text("");
      m_pObjEtaLabel->Text("");
      m_pObjBuildBtn->Text(g_ClientDAL.GetString(EStrId::Build));
      m_pObjBuildBtn->Enabled(false);
      m_pObjUnitCostLabel->Show();
      m_pObjTotalPossibleUnit->Hide();
   }

   // Disable build button if not in our country, disable priority and    
   if(m_iSelectedBuildingCountry != g_ClientDAL.ControlledCountryID())
   {
      m_pObjProductionSpeedCbo->Selected_Content(g_ClientDAL.GetString(EStrId::Normal));
      m_pObjProductionSpeedCbo->Enabled(false);

      m_pObjPriorityCbo->Selected_Content(m_TextPriority[SP2::EUnitProductionPriority::High]);
      m_pObjPriorityCbo->Enabled(false);
   }
   else
   {
      m_pObjPriorityCbo->Enabled(true);
      m_pObjProductionSpeedCbo->Enabled(true);
      SetComboboxColorFromText(m_pObjPriorityCbo);
   }
}


void GUnitProductionWindow::UpdateBuildingCountry(set<UINT32>& in_BuildingCountryIdList)
{
   
   GCountries  l_buildersList;
   GCountry    l_country;
   bool        l_bSelectedCountryIsInList;   
   m_BuildingCountryIdList = in_BuildingCountryIdList;


   // If we had selected our country, it is now in the list
   if(m_iSelectedBuildingCountry == (UINT16)g_ClientDAL.ControlledCountryID())
      l_bSelectedCountryIsInList = true;
   else 
      l_bSelectedCountryIsInList  = false;

   // Then add everybody else
   set<UINT32>::iterator  l_CountryIterator = in_BuildingCountryIdList.begin();
   while(l_CountryIterator != in_BuildingCountryIdList.end())
   {
      // Don't add ourself twice.
      if(*l_CountryIterator != (UINT32)g_ClientDAL.ControlledCountryID())
      {
         l_country = g_ClientDAL.Country(*l_CountryIterator);
         //g_ClientDAL.Country(*l_CountryIterator,l_country);
         l_buildersList.push_back(l_country);
      }

      // If we have selected this country, it still in the list
      if(m_iSelectedBuildingCountry == *l_CountryIterator)
         l_bSelectedCountryIsInList = true;

      // Next country
      l_CountryIterator++;
   }

   // Add Ourselves to the top of the list(At the end)
   l_country = g_ClientDAL.Country(g_ClientDAL.ControlledCountryID());
   l_buildersList.push_back(l_country);

   
   // Fill out production country
   //m_pObjBuildingCountryCbo->Clear();
   //g_ClientDDL.ComboboxSetCountriesAndFlag(l_buildersList, m_pObjBuildingCountryCbo, true);

   // Is the selected country still is gone from the list
   if(l_bSelectedCountryIsInList == false)
   {
      // Select our country
      m_iSelectedBuildingCountry = g_ClientDAL.ControlledCountryID();
      GenerateUnitDesignList();
      SelectedCategory(m_eSelectedCategory);
   }

   // Show Selected country
   l_country = g_ClientDAL.Country(m_iSelectedBuildingCountry);
   m_pObjBuildingCountryCbo->Selected_Content(l_country.Name());

   // Set If we accept foreign order or not.
   m_pObjAcceptForeignOrderTog->ToggleState(g_ClientDAL.m_PlayerCountryData.AcceptForeignOrder());
}

void GUnitProductionWindow::UpdateProductionStatus(list<GUnitProductionStatus> &in_ProductionStatusList,REAL32 in_fProductionCapacity)
{
   // Clear Last ETAs
   for(UINT32 i =0 ;i<SP2::EUnitProductionPriority::PriorityCount;i++)
   {
      m_iLastETAByPriority[i] = 0;
   }

   
  
   // Clear total Units in production
   m_iCurrentInfantryUnitsInProduction = 0;
   
   
   // Resize list for appropriate qty.
   INT32 l_iRowDifference  = m_pObjProductionList->Get_Nb_Rows() - in_ProductionStatusList.size();
   if(l_iRowDifference>0)
   {
      for(INT32 i=0;i<l_iRowDifference;i++)
         m_pObjProductionList->RemoveRow(m_pObjProductionList->Get_Nb_Rows()-1);

      // Update everything.
      m_pObjProductionList->Update();

   }
   else
   if(l_iRowDifference < 0)
   {
      l_iRowDifference = - l_iRowDifference;
      for(INT32 i=0;i<l_iRowDifference;i++)
      {
         GListBox::GRow *l_pRow = m_pObjProductionList->Insert_New_Row();
         GProgressMeterYellowValue*       l_pProgressBar = (GProgressMeterYellowValue*)l_pRow->Object()->Child("frmProgress");
         ((GButton*)l_pProgressBar->Child("btnFill"))->Enabled(false);
      }
 
      // Update everything.
      m_pObjProductionList->Update();

   }

   // Clear order pointers->ID.
   m_Orders.clear();

   // Iterate over list and display information
   UINT32 l_iRowIndex = 0;
   list<GUnitProductionStatus>::const_iterator l_ProductionStatusIterator = in_ProductionStatusList.begin();
   while(l_ProductionStatusIterator != in_ProductionStatusList.end())
   {
      // Get row
      GListBox::GRow* l_pRow  = m_pObjProductionList->Get_Row_At(l_iRowIndex);

      // Get Elements of this new row.
      GUI::GPictureBox*                l_pPictureBox  = (GUI::GPictureBox*)l_pRow->Object()->Child("picType");
      GUI::GPictureBox*                l_pFromPic     = (GUI::GPictureBox*)l_pRow->Object()->Child("picFlagFrom");
      GUI::GPictureBox*                l_pToPic       = (GUI::GPictureBox*)l_pRow->Object()->Child("picFlagTo");
      GUI::GLabel*                     l_pFromCode    = (GUI::GLabel*)l_pRow->Object()->Child("txtCodeFrom");
      GUI::GLabel*                     l_pToCode      = (GUI::GLabel*)l_pRow->Object()->Child("txtCodeTo");
      GUI::GLabel*                     l_pUnitQuantity= (GUI::GLabel*)l_pRow->Object()->Child("txtQty");
      GUI::GLabel*                     l_pDesignName  = (GUI::GLabel*)l_pRow->Object()->Child("txtUnit");
      GUI::GComboBox*                  l_pPriorityCbo = (GUI::GComboBox*)l_pRow->Object()->Child("cboPriority");
      GProgressMeterYellowValue*       l_pProgressBar = (GProgressMeterYellowValue*)l_pRow->Object()->Child("frmProgress");

      // Get Corresponding design
      GUnitDesign*  l_pUnitDesign= (SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign(l_ProductionStatusIterator->m_iDesignID);
      gassert(l_pUnitDesign,"Design should exist in design list, we probably just order one of these!!!");

      
      // Set Data
      UINT32 l_iTotalUnitShipped;
      if(l_ProductionStatusIterator->m_fProgress < 1 )
      {
         l_pProgressBar->Value(l_ProductionStatusIterator->m_fProgress,0,false,false); 
         ((GButton*)l_pProgressBar->Child("btnFill"))->ColorNormal(GColorRGBInt(0,0,0,64));
         ((GButton*)l_pProgressBar->Child("btnFill"))->ColorBackground(GColorRGBInt(0,0,0,64));
         l_iTotalUnitShipped =0 ; 
      }
      else 
      {
         l_pProgressBar->Value(l_ProductionStatusIterator->m_fProgress - 1,0,false,false); 
         ((GButton*)l_pProgressBar->Child("btnFill"))->ColorNormal(GColorRGBInt(0,0,0,255));
         ((GButton*)l_pProgressBar->Child("btnFill"))->ColorBackground(GColorRGBInt(0,0,0,255));
         l_iTotalUnitShipped = (UINT32) ((l_ProductionStatusIterator->m_fProgress-1) * l_ProductionStatusIterator->m_iQuantity + 0.5f);
      }

      l_pProgressBar->TextValueObj()->TextAlignment(L"MiddleRight");
      l_pProgressBar->TextValueObj()->TextOffset(-3, 0);
      l_pProgressBar->TextValueObj()->Text( l_pProgressBar->TextValueObj()->Text() + g_ClientDAL.GetString(EStrId::EtaDays) );
      l_pProgressBar->TextValueObj()->FontColorNormal(GColorRGBInt(200,200,200,255));


      
      l_pUnitQuantity->Text(GString(l_iTotalUnitShipped) + L"/" +  GString(l_ProductionStatusIterator->m_iQuantity));
      l_pUnitQuantity->FontColorNormal(GColorRGBInt(255,255,255,255));
      l_pUnitQuantity->FontSize(9);
      l_pDesignName->Text(l_pUnitDesign->Name());
      l_pDesignName->FontSize(9);

      GCountry l_BuildingCountry,l_PayingCountry;
      l_BuildingCountry = g_ClientDAL.Country(l_ProductionStatusIterator->m_iBuildingCountry);
      // Store building country in additional id.
      l_pRow->m_iAdditionalId = l_ProductionStatusIterator->m_iBuildingCountry;
      l_PayingCountry = g_ClientDAL.Country(l_ProductionStatusIterator->m_iPayingCountry);
      l_pToPic->PictureNormal(SP2::FLAG_DEFAULT_FOLDER + GString(l_PayingCountry.Flag()));
      l_pFromPic->PictureNormal(SP2::FLAG_DEFAULT_FOLDER + GString(l_BuildingCountry.Flag()));
      l_pToCode->Text(l_PayingCountry.Code());
      l_pFromCode->Text(l_BuildingCountry.Code());
      
      l_pFromPic->TooltipText( l_BuildingCountry.Name() );
      l_pToPic->TooltipText( l_PayingCountry.Name() );   
      l_pFromCode->TooltipText( l_BuildingCountry.Name() );
      l_pToCode->TooltipText( l_PayingCountry.Name() );  

      if(l_ProductionStatusIterator->m_iDaysLeft != 0xFFFFFFFF)
      {
         l_pProgressBar->TextValueObj()->Text( GString(l_ProductionStatusIterator->m_iDaysLeft) + g_ClientDAL.GetString(EStrId::EtaDays) );
      }
      else
      {
         l_pProgressBar->TextValueObj()->Text(g_ClientDAL.GetString(EStrId::NA));
      }
      
      l_pPictureBox->PictureNormal(m_UnitDesignIDLogo[l_pUnitDesign->Type()->Category()]);

      // Set Priority ComboBox
      l_pPriorityCbo->Enabled(true);
      BuildPriorityComboBox(l_pPriorityCbo);
   
      // Set row color and comboBox
      SetRowPriority(l_pRow,l_ProductionStatusIterator->m_ePriority);
      if(m_iLastETAByPriority[l_ProductionStatusIterator->m_ePriority] < l_ProductionStatusIterator->m_iDaysLeft)
         m_iLastETAByPriority[l_ProductionStatusIterator->m_ePriority] = l_ProductionStatusIterator->m_iDaysLeft;

      // Keep order ID with row ptr.
      m_Orders[l_pRow] = l_ProductionStatusIterator->m_iRequestID;
      

      // Keep the total unit ship in military
      if(l_pUnitDesign->Type()->Category() == EUnitCategory::Infantry)
         m_iCurrentInfantryUnitsInProduction += l_ProductionStatusIterator->m_iQuantity - l_iTotalUnitShipped;

      // Set Row Status darker if unit isn't being built
      if(!l_ProductionStatusIterator->m_bIsUnitBeingBuilt)
      {
         l_pUnitQuantity->FontColorNormal(c_colorItemIsNotBeingBuilt);
         l_pDesignName->FontColorNormal(c_colorItemIsNotBeingBuilt);
         l_pToCode->FontColorNormal(c_colorItemIsNotBeingBuilt);
         l_pFromCode->FontColorNormal(c_colorItemIsNotBeingBuilt);
         l_pProgressBar->TextValueObj()->FontColorNormal(c_colorItemIsNotBeingBuilt);
      }
      else
      {
         l_pUnitQuantity->FontColorNormal(c_colorItemIsBeingBuilt);
         l_pDesignName->FontColorNormal(c_colorItemIsBeingBuilt);
         l_pToCode->FontColorNormal(c_colorItemIsBeingBuilt);
         l_pFromCode->FontColorNormal(c_colorItemIsBeingBuilt);
         l_pProgressBar->TextValueObj()->FontColorNormal(c_colorItemIsBeingBuilt);
      }
      l_ProductionStatusIterator++;
      l_iRowIndex++;
   }


   // If there is too many order, cancel build button
   if(m_pObjProductionList->Get_Nb_Rows()<c_iMaximumOrder && m_pSelectedDesign != NULL)
      m_pObjBuildBtn->Enabled(true);
   else
      m_pObjBuildBtn->Enabled(false);

   // ETa of lower rank should be at least egual to their upper rank
   for(UINT32 i =1 ;i<SP2::EUnitProductionPriority::PriorityCount;i++)
   {
      if(m_iLastETAByPriority[i] < m_iLastETAByPriority[i-1])
         m_iLastETAByPriority[i] = m_iLastETAByPriority[i-1];
   }

   // Since some data calculation have changed, reupdate current selected items.
   if(m_pSelectedDesign)
      UpdateSelectedDesign(m_pSelectedDesign);
}

//void GUnitProductionWindow::UpdateProducitonRow(GUI::GListBox::GRow* in_pRowToUpdate,const& GUnitProductionStatus

void GUnitProductionWindow::BuildPriorityComboBox(GUI::GComboBox* in_pComboBox)
{
   gassert(in_pComboBox,"Invalid null pointer");

   GListBox* l_pListBox             = in_pComboBox->GetListBox();

   // Is it already built ?
   if(l_pListBox->Get_Nb_Rows())
      return;

   GListBox::GRow* l_pComboBoxRow   = l_pListBox->Insert_New_Row();
   GUI::GLabel* l_pLabel            = (GUI::GLabel*)l_pComboBoxRow->Object();
   l_pLabel->Text(m_TextPriority[SP2::EUnitProductionPriority::High]);
   l_pLabel->FontColor(c_colorHighPriority);

   l_pComboBoxRow  = l_pListBox->Insert_New_Row();
   l_pLabel   = (GUI::GLabel*)l_pComboBoxRow->Object();
   l_pLabel->Text(m_TextPriority[SP2::EUnitProductionPriority::Normal]);
   l_pLabel->FontColor(c_colorMediumPriority);
   
   l_pComboBoxRow  = l_pListBox->Insert_New_Row();
   l_pLabel   = (GUI::GLabel*)l_pComboBoxRow->Object();
   l_pLabel->Text(m_TextPriority[SP2::EUnitProductionPriority::Low]);
   l_pLabel->FontColor(c_colorLowPriority);

   l_pComboBoxRow  = l_pListBox->Insert_New_Row();
   l_pLabel   = (GUI::GLabel*)l_pComboBoxRow->Object();
   l_pLabel->Text(m_TextPriority[SP2::EUnitProductionPriority::OnHold]);
   l_pLabel->FontColor(c_colorHold);

   l_pComboBoxRow  = l_pListBox->Insert_New_Row();
   l_pLabel   = (GUI::GLabel*)l_pComboBoxRow->Object();
   l_pLabel->Text(g_ClientDAL.GetString(EStrId::Cancel));
   l_pLabel->FontColor(c_colorCancel);
}

void GUnitProductionWindow::SetRowPriority(GListBox::GRow* in_pRow,EUnitProductionPriority::Enum in_ePriority)
{
   gassert(in_pRow,"Invalid Null pointer");

   GColorRGBInt   l_RowColor;

   switch(in_ePriority)
   {
   case EUnitProductionPriority::OnHold:
      l_RowColor     = c_colorHold;
      break;
   case EUnitProductionPriority::Low:
      l_RowColor     = c_colorLowPriority;
      break;
   case EUnitProductionPriority::Normal:
      l_RowColor     = c_colorMediumPriority;
      break;
   case EUnitProductionPriority::High:
      l_RowColor     = c_colorHighPriority;
      break;
   default:
      gassert(false,"Unsuported priority");
   }

   // Set the color and the text of the combo box
   GUI::GComboBox* l_pPriorityCbo = (GUI::GComboBox*)in_pRow->Object()->Child("cboPriority");
   l_pPriorityCbo->Selected_Content(m_TextPriority[in_ePriority]);
   GUI::GEditBox* l_pEditBox = (GUI::GEditBox *)l_pPriorityCbo->Child("Edit");
   l_pEditBox->FontColor(l_RowColor);

   // Set the color of the row
   ((GUI::GFrame *)in_pRow->Object())->ColorNormal(l_RowColor);
   ((GUI::GFrame *)in_pRow->Object())->ColorBackground(l_RowColor);
}

UINT8 GUnitProductionWindow::GetProductionSpeedFromCombobox(GUI::GComboBox* in_pPriorityCbo)
{
   gassert(in_pPriorityCbo,"pointer is null");
   if(in_pPriorityCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::Normal))
   {
      return 0;
   }
   else if(in_pPriorityCbo->Selected_Content() == g_ClientDAL.GetString(EStrId::Fast))
   {
      return 1;
   }

   gassert(false,"Speed level not implemented");
   return 0;
}

EUnitProductionPriority::Enum GUnitProductionWindow::GetPriorityFromCombobox(GUI::GComboBox* in_pPriorityCbo)
{
   gassert(in_pPriorityCbo,"pointer is null");
   for(UINT32 i=SP2::EUnitProductionPriority::OnHold;
      i<SP2::EUnitProductionPriority::PriorityCount;
      i++)
   {
      if(m_TextPriority[i] == in_pPriorityCbo->Selected_Content() )
      {
         return (SP2::EUnitProductionPriority::Enum )i;
      }
   }

   // Probably Cancel...or some new options.
   return EUnitProductionPriority::PriorityCount;
}

void GUnitProductionWindow::ChangePriorityOfOrder(GListBox::GRow* in_pRow)
{
   bool l_bCancelOrder = false;

   // Get new Priority
   GUI::GComboBox* l_pPriorityCbo = (GUI::GComboBox*)in_pRow->Object()->Child("cboPriority");
   EUnitProductionPriority::Enum l_eOrderPriority = GetPriorityFromCombobox(l_pPriorityCbo);

   // Get Country ID (producer)

   if(l_eOrderPriority != EUnitProductionPriority::PriorityCount)
   {
      SetRowPriority(in_pRow,l_eOrderPriority);
   }
   else
   // Lets assume cancel is the only other item in the combo box.
   {
      // cancel this order
      l_bCancelOrder = true;
      // Set the worst status , just in case.
      l_eOrderPriority = EUnitProductionPriority::OnHold;
   }

   // Create game event to change the order
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GChangeUnitOrder);
   SP2::Event::GChangeUnitOrder* l_pChangeOrderEvent = (SP2::Event::GChangeUnitOrder*) (l_pEvent.get() );

   // Set Target and Source for this event
   l_pChangeOrderEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pChangeOrderEvent->m_iSource = g_SP2Client->Id();

   // Fill out changes
   l_pChangeOrderEvent->m_iOrderId     = m_Orders[in_pRow];
   l_pChangeOrderEvent->m_bCancelOrder = l_bCancelOrder;
   l_pChangeOrderEvent->m_eNewPriority = l_eOrderPriority;
   
   // Order from Production country
   l_pChangeOrderEvent->m_iCountryId   = (UINT16)in_pRow->m_iAdditionalId;

   // Fire event
   g_Joshua.RaiseEvent(l_pEvent);	

   // If we have received a cancel order, remove it from the list
   if(l_bCancelOrder)
   { 
      m_pObjProductionList->RemoveRow(in_pRow);
      m_pObjProductionList->Update();
   }
}


void GUnitProductionWindow::SelectedCategory(SP2::EUnitCategory::Enum in_CategoryToShow)
{
   m_eSelectedCategory = in_CategoryToShow;

   for(int i=SP2::EUnitCategory::Ground;i<SP2::EUnitCategory::ItemCount;i++)
   {
      m_pObjFilter[i]->ToggleState(false);
   }

   // Enable category button
   m_pObjFilter[in_CategoryToShow]->ToggleState(true);

   // Build te design tree
   BuildUnitDesignTree(in_CategoryToShow);

   // Clean Selection
   UpdateSelectedDesign(NULL);
}


void  GUnitProductionWindow::OnShow()
{
   // call default show.
   __super::OnShow();

   // Ask for a refresh when showing the window.
   // Send querry to server.
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GRequestProductionQueue);
   SP2::Event::GRequestProductionQueue* l_pRequestProductionQueueEvent = (SP2::Event::GRequestProductionQueue*) (l_pEvent.get() );

   // Set Target and Source for this event
   l_pRequestProductionQueueEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pRequestProductionQueueEvent->m_iSource = g_SP2Client->Id();

   // Raise It
   g_Joshua.RaiseEvent(l_pEvent);	

   // Also ask for the list of capable build counrty
   RequestBuildingCountry();

   // Enable nuclear possibilites only if allowed
   m_pObjFilter[SP2::EUnitCategory::Nuclear]->Enabled(g_ClientDAL.GameOptions().NuclearAllowed());
} 

/*
 *  Request the building country list
 */
void GUnitProductionWindow::RequestBuildingCountry()
{
   // Send querry to server.
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GRequestForeignBuildingCountryList);
   SP2::Event::GRequestForeignBuildingCountryList* l_pRequestEvent = (SP2::Event::GRequestForeignBuildingCountryList*) (l_pEvent.get() );

   // Set Target and Source for this event
   l_pRequestEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pRequestEvent->m_iSource = g_SP2Client->Id();

   // Raise It
   g_Joshua.RaiseEvent(l_pEvent);	
}


void GUnitProductionWindow::RequestBuildOrder()
{
   // Do some validate
   gassert(m_pSelectedDesign,"Invalid null pointer");

   // Order quantity of zero or negative are not interesting
   if((UINT32)m_pObjQuantity->Value() <=0)
      return;


   //Request the unit production
   g_ClientDCL.RequestUnitProduction(m_pSelectedDesign->Id(),
                                     (UINT32)m_pObjQuantity->Value(),
                                     GetProductionSpeedFromCombobox(m_pObjProductionSpeedCbo),
                                     m_iSelectedBuildingCountry,
                                     GetPriorityFromCombobox(m_pObjPriorityCbo));
}

INT32 GUnitProductionWindow::CalculateETA(void)
{
   if(m_pSelectedDesign == NULL)
      return 0;

 
   REAL32 l_iQtyOrdered             = (REAL32)m_pObjQuantity->Value();
   UINT16 l_iProductionSpeed        = GetProductionSpeedFromCombobox(m_pObjProductionSpeedCbo); 
   REAL32 l_fTotalProductionPoint = (REAL32)(log10(g_ClientDAL.m_PlayerCountryData.GDPValue() / 1000000000.0));


   REAL32 l_fInitialTimeForPreparation = (REAL32)g_ClientDAL.ProductionInformation((EUnitType::Enum)m_pSelectedDesign->Type()->Id()).m_fInitialTimeInDays;
   REAL32 l_fTimeInProduction = (REAL32)(  l_iQtyOrdered / 
                                             ( g_ClientDAL.ProductionInformation((EUnitType::Enum)m_pSelectedDesign->Type()->Id()).m_fUnitsPerDays * 
                                               1 * // For better approximation calculate this number
                                               (l_iProductionSpeed+ 1)
                                              )
                                            );

   if(l_fTotalProductionPoint > 1)
   {
       
      return (UINT32) (l_fInitialTimeForPreparation + l_fTimeInProduction) ;
   }
   
   return (UINT32) (l_fInitialTimeForPreparation + l_fTimeInProduction  / l_fTotalProductionPoint);

}

void GUnitProductionWindow::AcceptForeignOrder(bool in_bAcceptForeignOrder)
{
   // Send information to server
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GRegisterAsBuildingCountry);
   SP2::Event::GRegisterAsBuildingCountry* l_pRegisterEvent = (SP2::Event::GRegisterAsBuildingCountry*) (l_pEvent.get() );

   // Set Target and Source for this event
   l_pRegisterEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pRegisterEvent->m_iSource = g_SP2Client->Id();
   l_pRegisterEvent->m_bAcceptForeignCountryOrder = in_bAcceptForeignOrder;

   // Raise It
   g_Joshua.RaiseEvent(l_pEvent);	
}

void GUnitProductionWindow::SetComboboxColorFromText(GUI::GComboBox* in_pComboBox)
{
   GUI::GEditBox* l_pEditBox = (GUI::GEditBox *)in_pComboBox->Child("Edit");

   if(in_pComboBox->Selected_Content() == m_TextPriority[SP2::EUnitProductionPriority::High])
   {
      l_pEditBox->FontColorNormal(c_colorHighPriority);
      l_pEditBox->FontColor(c_colorHighPriority);
   }
   else if(in_pComboBox->Selected_Content() == m_TextPriority[SP2::EUnitProductionPriority::Normal])
   {
      l_pEditBox->FontColorNormal(c_colorMediumPriority);
      l_pEditBox->FontColor(c_colorMediumPriority);
   }
   else if(in_pComboBox->Selected_Content() == m_TextPriority[SP2::EUnitProductionPriority::Low])
   {
      l_pEditBox->FontColorNormal(c_colorLowPriority);
      l_pEditBox->FontColor(c_colorLowPriority);
   }
   else 
   {
      gassert(false,"Unsupported priority for text color setting");
   }
}

void GUnitProductionWindow::Capacity(REAL32 in_iProductionCapacity)
{
   m_pObjCapacityTxt->Text( GString::FormatNumber(in_iProductionCapacity * 100, L"", L".", L"", L" %", 0, 1) );
   m_pObjCapacityFrm->Value( in_iProductionCapacity, 0 );
}