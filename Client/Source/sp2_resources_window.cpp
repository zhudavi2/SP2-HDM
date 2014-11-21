/**************************************************************
*
* sp2_resources_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_resources_window.h"
#include "../include/sp2_increase_production_window.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"
#include <limits.h>

const GString GResourcesWindow::TypeName(L"Resources_Window");

GUI::GBaseObject* GResourcesWindow::New()
{
   return (GUI::GBaseObject*) new GResourcesWindow();
}

GResourcesWindow::GResourcesWindow() : SP2::GBaseWindow()
{
   m_pData       = &(g_ClientDAL.DataResourcesWindow);
   m_pWindowData = &(g_ClientDAL.DataResourcesWindow);

   m_pObjGlobalTaxMod            = NULL;
   m_pObjSectorTax               = NULL;
   m_pObjDesired                 = NULL;
   m_pObjPartGdp                 = NULL;
   m_pObjMarketShare             = NULL;
   m_pObjMarketAvail             = NULL;
   m_pObjActual                  = NULL;
   m_pObjManagement              = NULL;
   m_pObjStatus                  = NULL;
   m_pObjTrade                   = NULL;
   m_pObjIncreaseProduction      = NULL;
   m_pObjMeetConsumption         = NULL;
   m_pObjScroll                  = NULL;
   m_pObjInnerFrame              = NULL;
   m_pObjResourcesFrm            = NULL;
   m_pObjInnerResourcesFrm       = NULL;
   m_pObjTree[0]                 = NULL;
   m_pObjTree[1]                 = NULL;
   m_pObjTree[2]                 = NULL;
   m_pObjTree[3]                 = NULL;
   m_pObjTree[4]                 = NULL;
   m_pObjTree[5]                 = NULL;
   m_pLastSelectedNode           = NULL;
   m_pSelectedNode               = NULL;
   m_iSelectedId                 = -1;
   m_bWindowCanDisable = true;
}

GResourcesWindow::~GResourcesWindow()
{
}

bool GResourcesWindow::Enabled()
{
   return m_bEnabled;
}

void GResourcesWindow::Enabled(bool in_bEnabled)
{
   __super::Enabled(in_bEnabled);

   m_pObjInnerFrame->Child(L"lblResource")->Enabled(in_bEnabled);
   m_pObjInnerFrame->Child(L"lblProduction")->Enabled(in_bEnabled);
   m_pObjInnerFrame->Child(L"lblConsumption")->Enabled(in_bEnabled);
   m_pObjInnerFrame->Child(L"lblTrade")->Enabled(in_bEnabled);
   m_pObjInnerFrame->Child(L"lblAvailable")->Enabled(in_bEnabled);

   m_pObjGlobalTaxMod->Enabled(in_bEnabled);
   m_pObjGlobalTaxModLbl->Enabled(in_bEnabled);
}

GUI::EEventStatus::Enum GResourcesWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);

   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjIncreaseProduction)
         {
            gassert(m_iSelectedId >= 0,"Trying to increase production on an invalid resources, check selection");
            g_ClientDDL.IncreaseProductionWindow()->Update(m_pData->m_fProduction[m_iSelectedId]);
            g_ClientDDL.IncreaseProductionWindow()->Show();
         }
         else if(in_pCaller == m_pObjMeetConsumption)
         {
            if(in_pCaller->Enabled())
            {
               m_pData->m_bMeetConsumption[m_iSelectedId] = ((GUI::GToggle*)in_pCaller)->ToggleState();
               
               if(m_pData->m_bMeetConsumption[m_iSelectedId])
               {
                  m_pObjMeetConsumption->ToggleState(true);

                  m_pObjDesired->TextValueObj()->Text(L"");
                  m_pObjDesired->Enabled(false);
                  m_pObjDesiredLbl->Enabled(false);

                  m_pObjTrade->Selected_Content(L"");
                  m_pObjTrade->Enabled(false);
                  m_pObjTradeLbl->Enabled(false);
               }
               else
               {
                  m_pObjMeetConsumption->ToggleState(false);

                  m_pObjTrade->Enabled(true);
                  m_pObjTradeLbl->Enabled(true);
                  m_pObjDesired->Enabled(true);
                  m_pObjDesiredLbl->Enabled(true);

                  if(m_pData->m_fDesired[m_iSelectedId] < 0)
                  {
                     //import desired
                     m_pObjTrade->Selected_Content(g_ClientDAL.GetString(100387));
                     m_pObjDesired->Value((-m_pData->m_fDesired[m_iSelectedId]) / 1000000, false, false);
                  }
                  else
                  {
                     //export desired
                     m_pObjTrade->Selected_Content(g_ClientDAL.GetString(100388));
                     m_pObjDesired->Value(m_pData->m_fDesired[m_iSelectedId] / 1000000, false, false);
                  }
               }

               m_pData->m_bDirty[m_iSelectedId] = true;
               Dirty(true);
            }
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GResourcesWindow::OnMouseWheel(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   GUI::GBaseObject* l_pObject = in_EventData.InitialRecipient; 
   
   while(l_pObject && l_pObject != m_pObjInnerResourcesFrm)
      l_pObject = l_pObject->Parent();
   
   if(l_pObject == m_pObjInnerResourcesFrm)
   {
      m_pObjScroll->Value(m_pObjScroll->Value() - (in_EventData.Mouse.WheelDelta * 16));
      m_pObjInnerResourcesFrm->Top(-(INT16)m_pObjScroll->Value());
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GResourcesWindow::OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   __super::OnMouseDown(in_EventData, in_pCaller);

   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller->Name() == L"btnUp" || in_pCaller->Name() == L"btnDown")
         {
            if(in_pCaller->Owner()->Enabled())
            {
               m_pData->m_bDirty[m_iSelectedId] = true;
               Dirty(true);
            }
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

GUI::EEventStatus::Enum GResourcesWindow::OnCustomEvent(UINT32 in_EventID, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller)
{
   switch (in_EventID)
   {
   case ECustomEvents::OnComboBoxSelectionEvent:
   case ECustomEvents::OnNodeCollapse:
   case ECustomEvents::OnNodeExpand:
   case ECustomEvents::OnNodeSelected:
   case ECustomEvents::OnNodeUnSelected:
   case c_OnResourceIncreaseProduction:
      break;

   case ECustomEvents::OnScrollVertical:
      if((in_pCaller == m_pObjScroll) && m_pObjTree[0])
         m_pObjInnerResourcesFrm->Top(-(INT16)in_EventData.Data);

      return GUI::EEventStatus::Handled;

   case c_OnUpdatableValueChange:     
      {
         if(in_pCaller)
         {
            if(in_pCaller == m_pObjGlobalTaxMod)
            {
               m_pData->m_fGlobalTaxMod = (REAL32)m_pObjGlobalTaxMod->Value() / 100;
            }

            if(in_pCaller == m_pObjSectorTax)
            {
               m_pData->m_fSectorTax[m_iSelectedId] = (REAL32)(m_pObjSectorTax->Value() / 100.f);
            }

            if(in_pCaller == m_pObjDesired)
            {
               //If trade is set to import
               if(m_pObjTrade->Selected_Content() == g_ClientDAL.GetString(100387))
                  m_pData->m_fDesired[m_iSelectedId] = -(m_pObjDesired->Value() * 1000000);
               else
                  m_pData->m_fDesired[m_iSelectedId] = (m_pObjDesired->Value() * 1000000);
            }
         }
         m_pData->m_bDirty[m_iSelectedId] = true;
      }
      Dirty(true);
      break;

   default: 
      return GBaseObject::OnCustomEvent(in_EventID, in_EventData, in_pCaller);      
   }

   switch (in_EventID)
   {
   case ECustomEvents::OnComboBoxSelectionEvent:
   {
      GUI::GComboBox * l_pCombo = (GUI::GComboBox *)in_EventData.Data;
      if(l_pCombo)
      {
         if(l_pCombo == m_pObjManagement)
         {
            if(m_pData->m_bManagement[m_iSelectedId])
            {
               //if it was public and now it's private
               if(l_pCombo->Selected_Content() == g_ClientDAL.GetString(100090))
               {
                  m_pData->m_bManagement[m_iSelectedId] = false;
                  m_pData->m_bDirty[m_iSelectedId] = true;
                  
                  Dirty(true);

                  UpdateResourcesFrame();
               }
            }
            else
            {
               //if it was private and now it's public
               if(l_pCombo->Selected_Content() == g_ClientDAL.GetString(100386))
               {
                  m_pData->m_bManagement[m_iSelectedId] = true;
                  m_pData->m_bDirty[m_iSelectedId] = true;
                  m_pData->m_bMeetConsumption[m_iSelectedId] = true;
                  Dirty(true);

                  UpdateResourcesFrame();
               }
            }
         }

         if(l_pCombo == m_pObjStatus)
         {
            if(m_pData->m_bStatus[m_iSelectedId])
            {
               //if it was legal and now it's illegal
               if(l_pCombo->Selected_Content() == g_ClientDAL.GetString(100092))
               {
                  m_pData->m_bStatus[m_iSelectedId] = false;
                  m_pData->m_bDirty[m_iSelectedId] = true;
                  Dirty(true);
                  UpdateResourcesFrame();
               }
            }
            else
            {
               //if it was illegal and now it's legal
               if(l_pCombo->Selected_Content() == g_ClientDAL.GetString(100093))
               {
                  m_pData->m_bStatus[m_iSelectedId] = true;
                  m_pData->m_bDirty[m_iSelectedId] = true;
                  Dirty(true);
                  UpdateResourcesFrame();
               }
            }
         }
  
         if(l_pCombo == m_pObjTrade)
         {
            if(m_pData->m_fDesired[m_iSelectedId] < 0)
            {
               //if it was import and now it's export
               if(l_pCombo->Selected_Content() == g_ClientDAL.GetString(100388))
               {
                  m_pData->m_fDesired[m_iSelectedId] = -m_pData->m_fDesired[m_iSelectedId];
                  m_pData->m_bDirty[m_iSelectedId] = true;
                  Dirty(true);
               }
            }
            else
            {
               //if it was export and now it's import
               if(l_pCombo->Selected_Content() == g_ClientDAL.GetString(100387))
               {
                  m_pData->m_fDesired[m_iSelectedId] = -m_pData->m_fDesired[m_iSelectedId];
                  m_pData->m_bDirty[m_iSelectedId] = true;
                  Dirty(true);
               }
            }
         }
      }
   }
      break;

   case ECustomEvents::OnNodeCollapse:
   case ECustomEvents::OnNodeExpand:
      UpdateTreePosition();
      break;

   case ECustomEvents::OnNodeSelected:
      {
         if(m_pLastSelectedNode)
         {
            bool l_pParentNode = true;
            if(m_pLastSelectedNode->Nodes().size() == 0)
               l_pParentNode = false;

            FormatCurrency((GUI::GLabel*)m_pLastSelectedNode->Item()->Child(L"txtProduction"), DBL_MAX, false, false, l_pParentNode);
            FormatCurrency((GUI::GLabel*)m_pLastSelectedNode->Item()->Child(L"txtDemand"), DBL_MAX,     false, false, l_pParentNode);
            FormatCurrency((GUI::GLabel*)m_pLastSelectedNode->Item()->Child(L"txtTrade"), DBL_MAX,      false,  false, l_pParentNode);
            FormatCurrency((GUI::GLabel*)m_pLastSelectedNode->Item()->Child(L"txtAvailable"), DBL_MAX,  true, false, l_pParentNode);
            FormatCurrency((GUI::GLabel*)m_pLastSelectedNode->Item()->Child(L"txtName"), DBL_MAX,       false, false, l_pParentNode);

            m_pLastSelectedNode->View()->SelectedNode(NULL);
            m_pLastSelectedNode = NULL;
         }
         
         GTreeBranch * l_pNode = (GTreeBranch*)((GUI::GTreeView*)in_EventData.Data)->SelectedNode();
         m_pLastSelectedNode = l_pNode;

         if(l_pNode)
         {
            bool l_pParentNode = true;
            if(l_pNode->Nodes().size() == 0)
               l_pParentNode = false;

            FormatCurrency((GUI::GLabel*)l_pNode->Item()->Child(L"txtProduction"), DBL_MAX, false, true, l_pParentNode);
            FormatCurrency((GUI::GLabel*)l_pNode->Item()->Child(L"txtDemand"), DBL_MAX,     false, true, l_pParentNode);
            FormatCurrency((GUI::GLabel*)l_pNode->Item()->Child(L"txtTrade"), DBL_MAX,      false,  true, l_pParentNode);
            FormatCurrency((GUI::GLabel*)l_pNode->Item()->Child(L"txtAvailable"), DBL_MAX,  true, true, l_pParentNode);
            FormatCurrency((GUI::GLabel*)l_pNode->Item()->Child(L"txtName"), DBL_MAX,       false, true, l_pParentNode);

            m_pSelectedNode = l_pNode;
            
            DisplayResourceInfo();
         }
      }
      break;
   case ECustomEvents::OnNodeUnSelected:
      {
         GTreeBranch * l_pNode = (GTreeBranch*)((GUI::GTreeView*)in_EventData.Data)->SelectedNode();
         
         if(l_pNode)
         {
            bool l_pParentNode = true;
            if(l_pNode->Nodes().size() == 0)
               l_pParentNode = false;

            FormatCurrency((GUI::GLabel*)l_pNode->Item()->Child(L"txtProduction"), DBL_MAX, false, false, l_pParentNode);
            FormatCurrency((GUI::GLabel*)l_pNode->Item()->Child(L"txtDemand"),     DBL_MAX, false, false, l_pParentNode);
            FormatCurrency((GUI::GLabel*)l_pNode->Item()->Child(L"txtTrade"),      DBL_MAX, false, false, l_pParentNode);
            FormatCurrency((GUI::GLabel*)l_pNode->Item()->Child(L"txtAvailable"),  DBL_MAX,  true, false, l_pParentNode);
            FormatCurrency((GUI::GLabel*)l_pNode->Item()->Child(L"txtName"),       DBL_MAX, false, false, l_pParentNode);

            m_pLastSelectedNode = NULL;
         }
      }
      break;

   case c_OnResourceIncreaseProduction:
      {
         //Update server according to increase production value
         g_ClientDDL.IncreaseProductionWindow()->Hide();
         INT32 l_iFactor = (INT32)in_EventData.Data;

         SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GIncreaseProduction);
         SP2::Event::GIncreaseProduction* l_pUpdate = (SP2::Event::GIncreaseProduction*)l_pEvent.get();
         
         l_pUpdate->m_iSource = g_SP2Client->Id();;
         l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;
         l_pUpdate->m_iResourceID = m_iSelectedId;
         l_pUpdate->m_fIncrease   = ((REAL64)l_iFactor) / 100.f;      

         g_ClientDCL.ClientAdvisor().RaiseEvent(l_pEvent);

         g_ClientDAL.FetchResourcesInfo(g_ClientDAL.ControlledCountryID());
      }
      break;
   }

   return GUI::EEventStatus::Handled;
}

bool GResourcesWindow::OnCreate()
{
   __super::OnCreate();

   m_pObjInnerFrm           = (GUI::GFrame*)Child(L"frmInner");
   m_pObjInnerFrame         = (GUI::GFrame*)Child(L"frmInner")->Child(L"frmResources");

   m_pObjGlobalTaxMod       = (SP2::GUpdatableValue*)m_pObjInnerFrame->Child(L"frmGlobalTaxMod");
   m_pObjSectorTax          = (SP2::GUpdatableValue*)m_pObjInnerFrame->Child(L"frmSectorTax");
   m_pObjDesired            = (SP2::GUpdatableValue*)m_pObjInnerFrame->Child(L"frmDesired");
   m_pObjPartGdp            = (GUI::GLabel*)m_pObjInnerFrame->Child(L"txtGdp");
   m_pObjMarketShare        = (GUI::GLabel*)m_pObjInnerFrame->Child(L"txtMarketShare");
   m_pObjMarketAvail        = (GUI::GLabel*)m_pObjInnerFrame->Child(L"txtAvailability");
   m_pObjActual             = (GUI::GLabel*)m_pObjInnerFrame->Child(L"txtActual");
   m_pObjManagement         = (GUI::GComboBox*)m_pObjInnerFrame->Child(L"cboManagement");
   m_pObjStatus             = (GUI::GComboBox*)m_pObjInnerFrame->Child(L"cboStatus");
   m_pObjTrade              = (GUI::GComboBox*)m_pObjInnerFrame->Child(L"cboTrade");
   m_pObjIncreaseProduction = (GUI::GButton*)m_pObjInnerFrame->Child(L"btnIncreaseProduction");
   m_pObjMeetConsumption    = (GUI::GToggle*)m_pObjInnerFrame->Child(L"togMeetConsumption");
   m_pObjScroll             = (GUI::GVScrollBar*)m_pObjInnerFrame->Child(L"scrFrame");
   m_pObjResourcesFrm       = (GUI::GFrame*)m_pObjInnerFrame->Child(L"frmResources");
   m_pObjInnerResourcesFrm  = (GUI::GFrame*)m_pObjResourcesFrm->Child(L"frmInner");

   m_pObjManagementLbl      = (GUI::GLabel*)m_pObjInnerFrame->Child(L"lblManagement");
   m_pObjStatusLbl          = (GUI::GLabel*)m_pObjInnerFrame->Child(L"lblStatus");
   m_pObjSectorTaxLbl       = (GUI::GLabel*)m_pObjInnerFrame->Child(L"lblSectorTax");
   m_pObjPartGdpLbl         = (GUI::GLabel*)m_pObjInnerFrame->Child(L"lblGdp");
   m_pObjMarketShareLbl     = (GUI::GLabel*)m_pObjInnerFrame->Child(L"lblMarketShare");
   m_pObjMarketAvailLbl     = (GUI::GLabel*)m_pObjInnerFrame->Child(L"lblMarketAvailability");
   m_pObjTradeLbl           = (GUI::GLabel*)m_pObjInnerFrame->Child(L"lblTrade2");
   m_pObjDesiredLbl         = (GUI::GLabel*)m_pObjInnerFrame->Child(L"lblDesired");
   m_pObjActualLbl          = (GUI::GLabel*)m_pObjInnerFrame->Child(L"lblActual");
   m_pObjMeetComsumptionLbl = (GUI::GLabel*)m_pObjInnerFrame->Child(L"lblMeetConsumption");
   m_pObjGlobalTaxModLbl    = (GUI::GLabel*)m_pObjInnerFrame->Child(L"lblGlobalTaxMod");

   vector<UINT32> l_vNbStep;
   vector<REAL64> l_vStep;

   l_vNbStep.push_back(0);
   l_vNbStep.push_back(10);
   l_vStep.push_back(0.1);
   l_vStep.push_back(1);

   m_pObjGlobalTaxMod->Initialize(l_vNbStep, l_vStep, L"", L" %", 1, 0, 0.0f, 100.0f);
  
   m_pObjManagement->Enabled(false);
   m_pObjStatus->Enabled(false);
   m_pObjSectorTax->Enabled(false);
   m_pObjPartGdp->Enabled(false);
   m_pObjMarketShare->Enabled(false);
   m_pObjIncreaseProduction->Enabled(false);
   m_pObjMarketAvail->Enabled(false);
   m_pObjTrade->Enabled(false);
   m_pObjDesired->Enabled(false);
   m_pObjActual->Enabled(false);
   m_pObjMeetConsumption->Enabled(false);

   m_pObjManagementLbl->Enabled(false);     
   m_pObjStatusLbl->Enabled(false);         
   m_pObjSectorTaxLbl->Enabled(false);      
   m_pObjPartGdpLbl->Enabled(false);        
   m_pObjMarketShareLbl->Enabled(false);    
   m_pObjMarketAvailLbl->Enabled(false);    
   m_pObjTradeLbl->Enabled(false);          
   m_pObjDesiredLbl->Enabled(false);        
   m_pObjActualLbl->Enabled(false);         
   m_pObjMeetComsumptionLbl->Enabled(false);
      
   m_pObjSectorTax->Initialize(l_vNbStep, l_vStep, L"", L" %", 1, 0, 0, 100.f);
   m_pObjDesired->Initialize(L"$ ", L" M", 0, 3, 0, 1000000000000000);

   m_pObjPartGdp->TextAlignment(L"MiddleCenter");
   m_pObjMarketShare->TextAlignment(L"MiddleCenter");
   m_pObjSectorTax->TextValueObj()->TextAlignment(L"MiddleCenter");
   m_pObjGlobalTaxMod->TextValueObj()->TextAlignment(L"MiddleCenter");
   m_pObjDesired->TextValueObj()->TextAlignment(L"MiddleCenter");
   
   m_pObjManagement->Add_Item(g_ClientDAL.GetString(100386));
   m_pObjManagement->Add_Item(g_ClientDAL.GetString(100090));
   m_pObjManagement->Update();
   
   m_pObjStatus->Add_Item(g_ClientDAL.GetString(100093));
   m_pObjStatus->Add_Item(g_ClientDAL.GetString(100092));
   m_pObjStatus->Update();
   
   m_pObjTrade->Add_Item(g_ClientDAL.GetString(100387));
   m_pObjTrade->Add_Item(g_ClientDAL.GetString(100388));
   m_pObjTrade->Update();
   
   m_pObjInnerFrame->Child(L"picTrim")->SendToBack();

   ((GFX::GWindow*)m_pObjResourcesFrm->Model()->Get_Primitive())->ClipChildren(true);

   m_pObjTree[0]            = CREATE_GUI_OBJECT_NAMED_VISIBLE(GUI::GTreeView, L"tree_view_0", m_pObjInnerResourcesFrm);
   m_pObjTree[1]            = CREATE_GUI_OBJECT_NAMED_VISIBLE(GUI::GTreeView, L"tree_view_1", m_pObjInnerResourcesFrm);
   m_pObjTree[2]            = CREATE_GUI_OBJECT_NAMED_VISIBLE(GUI::GTreeView, L"tree_view_2", m_pObjInnerResourcesFrm);
   m_pObjTree[3]            = CREATE_GUI_OBJECT_NAMED_VISIBLE(GUI::GTreeView, L"tree_view_3", m_pObjInnerResourcesFrm);
   m_pObjTree[4]            = CREATE_GUI_OBJECT_NAMED_VISIBLE(GUI::GTreeView, L"tree_view_4", m_pObjInnerResourcesFrm);
   m_pObjTree[5]            = CREATE_GUI_OBJECT_NAMED_VISIBLE(GUI::GTreeView, L"tree_view_5", m_pObjInnerResourcesFrm);

   for(INT32 i=0; i<6; i++)
   {
      m_pObjTree[i]->Size(m_pObjResourcesFrm->Width(), m_pObjResourcesFrm->Height());
      m_pObjTree[i]->StartNode(0);
      m_pObjTree[i]->ItemIndent(16);
      m_pObjTree[i]->ItemHeight(16);
      m_pObjTree[i]->SetImage(GUI::ETI_Branch,     L"images\\new\\tree_branch.RC#000225.tga");
      m_pObjTree[i]->SetImage(GUI::ETI_Collapse,   L"images\\new\\tree_minus.RC#000223.tga");
      m_pObjTree[i]->SetImage(GUI::ETI_Empty,      L"images\\new\\tree_empty.RC#000227.tga");
      m_pObjTree[i]->SetImage(GUI::ETI_Expand,     L"images\\new\\tree_plus.RC#000231.tga");
      m_pObjTree[i]->SetImage(GUI::ETI_Last,       L"images\\new\\tree_end.RC#000229.tga");
      m_pObjTree[i]->SetImage(GUI::ETI_Leaf,       L"images\\new\\tree_leaf.RC#000235.tga");
      m_pObjTree[i]->SetImage(GUI::ETI_Trunk,      L"images\\new\\tree_trunk.RC#000233.tga");
      m_pObjTree[i]->ColorNormal(L"RGBA: 0, 0, 0",L"0");
      m_pObjTree[i]->ImageSize(16,16);
      m_pObjTree[i]->Owner(this);
      m_pObjTree[i]->ItemType(L"lstResourceItem");
      m_pObjTree[i]->FlatTreeView(true);
   }

   m_pObjTree[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[0]->CreateNode(), g_ClientDAL.GetString(100353)));
   m_pObjTree[1]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[1]->CreateNode(), g_ClientDAL.GetString(100354)));
   m_pObjTree[2]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[2]->CreateNode(), g_ClientDAL.GetString(100355)));
   m_pObjTree[3]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[3]->CreateNode(), g_ClientDAL.GetString(100356)));
   m_pObjTree[4]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[4]->CreateNode(), g_ClientDAL.GetString(100357)));
   m_pObjTree[5]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[5]->CreateNode(), g_ClientDAL.GetString(100358)));

   m_pObjTree[0]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[0]->CreateNode(), g_ClientDAL.GetString(100359)));
   m_pObjTree[0]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[0]->CreateNode(), g_ClientDAL.GetString(100360)));
   m_pObjTree[0]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[0]->CreateNode(), g_ClientDAL.GetString(100361)));
   m_pObjTree[0]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[0]->CreateNode(), g_ClientDAL.GetString(100362)));
   m_pObjTree[0]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[0]->CreateNode(), g_ClientDAL.GetString(100363)));
   m_pObjTree[0]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[0]->CreateNode(), g_ClientDAL.GetString(100364)));

   m_pObjTree[1]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[1]->CreateNode(), g_ClientDAL.GetString(100365)));
   m_pObjTree[1]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[1]->CreateNode(), g_ClientDAL.GetString(100366)));

   m_pObjTree[2]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[2]->CreateNode(), g_ClientDAL.GetString(100367)));
   m_pObjTree[2]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[2]->CreateNode(), g_ClientDAL.GetString(100368)));
   m_pObjTree[2]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[2]->CreateNode(), g_ClientDAL.GetString(100369)));
   m_pObjTree[2]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[2]->CreateNode(), g_ClientDAL.GetString(100370)));

   m_pObjTree[3]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[3]->CreateNode(), g_ClientDAL.GetString(100371)));
   m_pObjTree[3]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[3]->CreateNode(), g_ClientDAL.GetString(100372)));
   m_pObjTree[3]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[3]->CreateNode(), g_ClientDAL.GetString(100373)));
   m_pObjTree[3]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[3]->CreateNode(), g_ClientDAL.GetString(100374)));

   m_pObjTree[4]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[4]->CreateNode(), g_ClientDAL.GetString(100375)));
   m_pObjTree[4]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[4]->CreateNode(), g_ClientDAL.GetString(100376)));
   m_pObjTree[4]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[4]->CreateNode(), g_ClientDAL.GetString(100377)));
   m_pObjTree[4]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[4]->CreateNode(), g_ClientDAL.GetString(100378)));
   m_pObjTree[4]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[4]->CreateNode(), g_ClientDAL.GetString(100379)));

   m_pObjTree[5]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[5]->CreateNode(), g_ClientDAL.GetString(100380)));
   m_pObjTree[5]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[5]->CreateNode(), g_ClientDAL.GetString(100381)));
   m_pObjTree[5]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[5]->CreateNode(), g_ClientDAL.GetString(100382)));
   m_pObjTree[5]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[5]->CreateNode(), g_ClientDAL.GetString(100383)));
   m_pObjTree[5]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[5]->CreateNode(), g_ClientDAL.GetString(100384)));
   m_pObjTree[5]->Nodes()[0]->Nodes().add(PrepareTreeResourceItem(m_pObjTree[5]->CreateNode(), g_ClientDAL.GetString(100385)));
   
   for(INT32 i=0; i<6; i++)
   {
      m_pObjTree[i]->Show();
   }
  
   UpdateTreePosition();
   m_pObjInnerResourcesFrm->Position(0,0);

   g_ClientDDL.IncreaseProductionWindowSpawn();
   RegisteredModalWindow.push_back(g_ClientDDL.IncreaseProductionWindow());

   return true;
}

GUI::GTreeBranch* GResourcesWindow::PrepareTreeResourceItem(GUI::GTreeBranch * in_pNode, GString in_sName)
{
   ((GUI::GFrame*)in_pNode->Item())->ColorNormal(GColorRGBInt(20,20,20,255));
   ((GUI::GLabel*)in_pNode->Item()->Child(L"txtName"))->Text(in_sName);
   ((GUI::GLabel*)in_pNode->Item()->Child(L"txtName"))->FontColorNormal(GColorRGBInt(150, 150, 150, 255));

   return in_pNode;
}

void GResourcesWindow::UpdateTreePosition()
{
   INT32 l_iPosition = 0;

   for(INT32 i=0; i<6; i++)
   {
      m_pObjTree[i]->Position(0, l_iPosition);
      INT32 l_iTreeSize = m_pObjTree[i]->CountVisible() * m_pObjTree[i]->ItemHeight();
      l_iPosition += l_iTreeSize;
      l_iPosition += 1;
   }

   m_pObjInnerResourcesFrm->Height(l_iPosition);
   
   INT32 l_iValue = m_pObjScroll->Value();
   
   if((m_pObjInnerResourcesFrm->Height() - m_pObjResourcesFrm->Height()) < l_iValue)
      l_iValue = (m_pObjInnerResourcesFrm->Height() - m_pObjResourcesFrm->Height());
   if(l_iValue < 0)
      l_iValue = 0;

   m_pObjScroll->Initialize(this, 0, max((m_pObjInnerResourcesFrm->Height() - m_pObjResourcesFrm->Height()),0), 16, 80, l_iValue);
   m_pObjInnerResourcesFrm->Top(-(INT16)l_iValue);
}

bool GResourcesWindow::OnDestroy()
{
   for(INT32 i=0; i<6; i++)
   {
      m_pObjTree[i]->Hide();
      g_Joshua.GUIManager()->ReleaseObject(m_pObjTree[i]);
      m_pObjTree[i] = NULL;
   }

   RegisteredModalWindow.clear();
   g_ClientDDL.IncreaseProductionWindowKill();
   
   return __super::OnDestroy();
}

void GResourcesWindow::OnShow()
{
   __super::OnShow();

   if(m_pObjInnerFrame)
      Update();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();

   g_ClientDAL.Attach(this, (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::Resources);
}

void GResourcesWindow::OnHide()
{  
   g_ClientDAL.Detach(this);
   // remove the request associated with our ressource laws window (if any)
   g_ClientDCL.RequestManager().RemoveRequest(this);

   __super::OnHide();
}

void GResourcesWindow::Update(bool in_bForce)
{
   if(Dirty() && !in_bForce)
      return;

   if(!m_pObjInnerFrame)
      return;
  
   if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID() && m_pLastSelectedNode)
   {
      m_pObjManagement->Enabled(true);
      m_pObjStatus->Enabled(true);
      m_pObjSectorTax->Enabled(true);
      m_pObjPartGdp->Enabled(true);
      m_pObjMarketShare->Enabled(true);
      m_pObjIncreaseProduction->Enabled(true);
      m_pObjMarketAvail->Enabled(true);
      m_pObjTrade->Enabled(true);
      m_pObjDesired->Enabled(true);
      m_pObjActual->Enabled(true);
      m_pObjMeetConsumption->Enabled(true);

      m_pObjManagementLbl->Enabled(true);     
      m_pObjStatusLbl->Enabled(true);         
      m_pObjSectorTaxLbl->Enabled(true);      
      m_pObjPartGdpLbl->Enabled(true);        
      m_pObjMarketShareLbl->Enabled(true);    
      m_pObjMarketAvailLbl->Enabled(true);    
      m_pObjTradeLbl->Enabled(true);          
      m_pObjDesiredLbl->Enabled(true);        
      m_pObjActualLbl->Enabled(true);         
      m_pObjMeetComsumptionLbl->Enabled(true);
   }
   else
   {
      m_pObjManagement->Enabled(false);
      m_pObjStatus->Enabled(false);
      m_pObjSectorTax->Enabled(false);
      m_pObjPartGdp->Enabled(false);
      m_pObjMarketShare->Enabled(false);
      m_pObjIncreaseProduction->Enabled(false);
      m_pObjMarketAvail->Enabled(false);
      m_pObjTrade->Enabled(false);
      m_pObjDesired->Enabled(false);
      m_pObjActual->Enabled(false);
      m_pObjMeetConsumption->Enabled(false);

      m_pObjManagementLbl->Enabled(false);     
      m_pObjStatusLbl->Enabled(false);         
      m_pObjSectorTaxLbl->Enabled(false);      
      m_pObjPartGdpLbl->Enabled(false);        
      m_pObjMarketShareLbl->Enabled(false);    
      m_pObjMarketAvailLbl->Enabled(false);    
      m_pObjTradeLbl->Enabled(false);          
      m_pObjDesiredLbl->Enabled(false);        
      m_pObjActualLbl->Enabled(false);         
      m_pObjMeetComsumptionLbl->Enabled(false);

      m_pObjManagement->Selected_Content(L"");
      m_pObjStatus->Selected_Content(L"");
      m_pObjSectorTax->TextValueObj()->Text(L"");
      m_pObjPartGdp->Text(L"");
      m_pObjMarketShare->Text(L"");
      m_pObjMarketAvail->Text(L"");
      m_pObjTrade->Selected_Content(L"");
      m_pObjDesired->TextValueObj()->Text(L"");
      m_pObjActual->Text(L"");
      m_pObjMeetConsumption->ToggleState(false);
   }

   if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
   {
      /*
      // TO DO : REMOVE LATER, REQUEST MANAGER WILL DO THE JOB
      if(((m_fWhenSaved + g_Joshua.Latency()) > (g_ClientDAL.m_PlayerCountryData.m_fWhenSent + (g_Joshua.Latency() / 2))))
         return;
      */

      for(INT32 i=0; i<EResources::ItemCount; i++)
      {
         GCountryDataClient & l_Data = g_ClientDAL.m_PlayerCountryData;
         m_pData->m_fProduction[i]   = l_Data.ResourceProduction((EResources::Enum)i);
         m_pData->m_fConsumption[i]  = l_Data.ResourceDemand((EResources::Enum)i);
         m_pData->m_fTrade[i]        = l_Data.ResourceExport((EResources::Enum)i) - l_Data.ResourceImport((EResources::Enum)i);
         m_pData->m_bStatus[i]       = l_Data.ResourceLegal((EResources::Enum)i);
         m_pData->m_bManagement[i]   = l_Data.ResourceGvtCtrl((EResources::Enum)i);
         m_pData->m_bMeetConsumption[i] = l_Data.ResourceMeetConsumption((EResources::Enum)i);
         m_pData->m_fSectorTax[i]    = l_Data.ResourceTaxes((EResources::Enum)i);
         m_pData->m_fPartGdp[i]      = l_Data.ResourceGDP((EResources::Enum)i);
         m_pData->m_fDesired[i]      = l_Data.ResourceExportDesired((EResources::Enum)i) - l_Data.ResourceImportDesired((EResources::Enum)i);
         m_pData->m_fGlobalTaxMod    = l_Data.GlobalTaxMod();
         m_pData->m_fMarketShare[i]  = l_Data.ResourceMarketShare((EResources::Enum)i);
         m_pData->m_fMarketAvailability[i] = l_Data.m_pResourceMarketAvailability[i];
      }
   }

   UpdateResourcesFrame();

   Dirty(false);
}

void GResourcesWindow::UpdateResourcesFrame()
{
   GUI::GFrame* l_pFrame = NULL;
   
   REAL64 l_fProductionTotal  = 0.f;
   REAL64 l_fConsumptionTotal = 0.f;
   REAL64 l_fTradeTotal       = 0.f;
   REAL64 l_fAvailableTotal   = 0.f;
   bool   l_bLegal            = false;
   bool   l_bGvtCtrl          = false;
   bool   l_bPublic           = false;

   m_pObjGlobalTaxMod->Value(m_pData->m_fGlobalTaxMod * 100, false, false);


   bool l_bTimeToUpdate = false;
   INT8 l_iIDToUpdate   = 0;
   
   for(INT32 i=0; i<EResources::ItemCount; i++)
   {
      l_fProductionTotal  += m_pData->m_fProduction[i] / 1000000;
      l_fConsumptionTotal += m_pData->m_fConsumption[i] / 1000000;
      l_fTradeTotal       += m_pData->m_fTrade[i] / 1000000;
      l_fAvailableTotal   = (l_fProductionTotal - l_fConsumptionTotal - l_fTradeTotal);

      if(m_pData->m_bStatus[i])
      {
         l_bLegal = true;
      }

      if(m_pData->m_bManagement[i])
      {
         l_bGvtCtrl = true;
      }
      else
      {
         l_bPublic = true;
      }

      switch(i)
      {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
         l_pFrame = (GUI::GFrame*)m_pObjTree[0]->Nodes()[0]->Nodes()[i]->Item();
         l_iIDToUpdate   = 0;
         if(i == 5)
         {
            l_bTimeToUpdate = true;
         }
         break;

      case 6:
      case 7:
         l_pFrame = (GUI::GFrame*)m_pObjTree[1]->Nodes()[0]->Nodes()[i - 6]->Item();
         l_iIDToUpdate   = 1;
         if(i == 7)
         {
            l_bTimeToUpdate = true;
         }
         break;
         
      case 8:
      case 9:
      case 10:
      case 11:
         l_pFrame = (GUI::GFrame*)m_pObjTree[2]->Nodes()[0]->Nodes()[i - 8]->Item();
         l_iIDToUpdate   = 2;
         if(i == 11)
         {
            l_bTimeToUpdate = true;
         }
         break;

      case 12:
      case 13:
      case 14:
      case 15:
         l_pFrame = (GUI::GFrame*)m_pObjTree[3]->Nodes()[0]->Nodes()[i - 12]->Item();
         l_iIDToUpdate   = 3;
         if(i == 15)
         {
            l_bTimeToUpdate = true;
         }
         break;

      case 16:
      case 17:
      case 18:
      case 19:
      case 20:
         l_pFrame = (GUI::GFrame*)m_pObjTree[4]->Nodes()[0]->Nodes()[i - 16]->Item();
         l_iIDToUpdate   = 4;
         if(i == 20)
         {
            l_bTimeToUpdate = true;
         }
         break;

      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:
         l_pFrame = (GUI::GFrame*)m_pObjTree[5]->Nodes()[0]->Nodes()[i - 21]->Item();
         l_iIDToUpdate   = 5;
         if(i == 26)
         {
            l_bTimeToUpdate = true;
         }
         break;
      }

      if(l_bTimeToUpdate)
      {
         //Update resource
         FormatCurrency((GUI::GLabel*)m_pObjTree[l_iIDToUpdate]->Nodes()[0]->Item()->Child(L"txtProduction"), l_fProductionTotal, false, false, true);
         FormatCurrency((GUI::GLabel*)m_pObjTree[l_iIDToUpdate]->Nodes()[0]->Item()->Child(L"txtDemand"), l_fConsumptionTotal, false, false, true);
         FormatCurrency((GUI::GLabel*)m_pObjTree[l_iIDToUpdate]->Nodes()[0]->Item()->Child(L"txtTrade"), l_fTradeTotal, false, false, true);
         FormatCurrency((GUI::GLabel*)m_pObjTree[l_iIDToUpdate]->Nodes()[0]->Item()->Child(L"txtAvailable"), l_fAvailableTotal, true, false, true);
         FormatCurrency((GUI::GLabel*)m_pObjTree[l_iIDToUpdate]->Nodes()[0]->Item()->Child(L"txtName"), DBL_MAX, false, false, true);
         if(l_bLegal)
            ((GUI::GPictureBox*)m_pObjTree[l_iIDToUpdate]->Nodes()[0]->Item()->Child(L"picLegal"))->PictureNormal(L"000242");
         else
            ((GUI::GPictureBox*)m_pObjTree[l_iIDToUpdate]->Nodes()[0]->Item()->Child(L"picLegal"))->PictureNormal(L"000244");

         if(l_bGvtCtrl && l_bPublic)
         {
            ((GUI::GPictureBox*)m_pObjTree[l_iIDToUpdate]->Nodes()[0]->Item()->Child(L"picControl"))->PictureNormal(L"000755");
         }
         else
         {
            if(l_bGvtCtrl)
               ((GUI::GPictureBox*)m_pObjTree[l_iIDToUpdate]->Nodes()[0]->Item()->Child(L"picControl"))->PictureNormal(L"000238");
            else
               ((GUI::GPictureBox*)m_pObjTree[l_iIDToUpdate]->Nodes()[0]->Item()->Child(L"picControl"))->PictureNormal(L"000240");
         }

         l_fProductionTotal  = 0.f;
         l_fConsumptionTotal = 0.f;
         l_fTradeTotal       = 0.f;
         l_fAvailableTotal   = 0.f;
         l_bLegal            = false;
         l_bGvtCtrl          = false;
         l_bPublic           = false;
         l_bTimeToUpdate     = false;
      }
      
      bool l_bIsSelected = false;

      //If the node was selected, reselect it
      if(m_pSelectedNode)
         if( ((GUI::GFrame*)m_pSelectedNode->Item()) == l_pFrame )
            l_bIsSelected = true;      
      
      //Update Total
      FormatCurrency((GUI::GLabel*)l_pFrame->Child(L"txtProduction"), m_pData->m_fProduction[i] / 1000000, false, l_bIsSelected, false);
      FormatCurrency((GUI::GLabel*)l_pFrame->Child(L"txtDemand"),     m_pData->m_fConsumption[i] / 1000000, false, l_bIsSelected, false);
      FormatCurrency((GUI::GLabel*)l_pFrame->Child(L"txtTrade"),      m_pData->m_fTrade[i] / 1000000, false, l_bIsSelected, false);
      FormatCurrency((GUI::GLabel*)l_pFrame->Child(L"txtAvailable"),  (m_pData->m_fProduction[i] - m_pData->m_fConsumption[i] - m_pData->m_fTrade[i]) / 1000000, true, l_bIsSelected, false);
      FormatCurrency((GUI::GLabel*)l_pFrame->Child(L"txtName"), DBL_MAX,       false, l_bIsSelected, false);
      
      if(m_pData->m_bStatus[i])
      {
         ((GUI::GPictureBox*)l_pFrame->Child(L"picLegal"))->PictureNormal(L"000242");
      }
      else
      {
         ((GUI::GPictureBox*)l_pFrame->Child(L"picLegal"))->PictureNormal(L"000244");
      }

      if(m_pData->m_bManagement[i])
      {
         ((GUI::GPictureBox*)l_pFrame->Child(L"picControl"))->PictureNormal(L"000238");
      }
      else
      {
         ((GUI::GPictureBox*)l_pFrame->Child(L"picControl"))->PictureNormal(L"000240");
      }
   }

   if(m_pSelectedNode)
      DisplayResourceInfo();
}

void GResourcesWindow::FormatCurrency(GUI::GLabel* in_pLabel, REAL64 in_fValue, bool in_bColored, bool in_bSelected, bool in_bSum)
{
   if(in_fValue != DBL_MAX)
   {
      GString l_sTemp = GString::FormatNumber(in_fValue, L" ", L".", L"$ ", L" M", 3, 0);      
      in_pLabel->Text(l_sTemp);      
   }
   else
   {
      GString l_sText = in_pLabel->Text();
      if (l_sText.find('-') != GString::npos)
         in_fValue = -1.f;
   }

   // check for -0 to display colors properly    
   if((in_fValue < 0) && (in_fValue > -1))
      in_fValue = 0;

   if(in_bColored)
   {
      if(in_bSelected)
      {
         if(in_bSum)
         {
            if(in_fValue >= 0)
            {
               in_pLabel->FontColor(GColorRGBInt(162, 250, 168, 255));
            }
            else
            {
               in_pLabel->FontColor(GColorRGBInt(233, 119, 119, 255));
            }
         }
         else
         {
            if(in_fValue >= 0)
            {
               in_pLabel->FontColor(GColorRGBInt(113, 230, 111, 255));
            }
            else
            {
               in_pLabel->FontColor(GColorRGBInt(253, 157, 157, 255));
            }
         }
      }
      else
      {
         if(in_bSum)
         {
            if(in_fValue >= 0)
            {
               in_pLabel->FontColor(GColorRGBInt(113, 230, 111, 255));
            }
            else
            {
               in_pLabel->FontColor(GColorRGBInt(233, 119, 119, 255));
            }
         }
         else
         {
            if(in_fValue >= 0)
            {
               in_pLabel->FontColor(GColorRGBInt(98, 176, 98, 255));
            }
            else
            {
               in_pLabel->FontColor(GColorRGBInt(175, 89, 89, 255));
            }
         }
      }
   }
   else
   {
      if(in_bSelected)
      {
         in_pLabel->FontColor(GColorRGBInt(255, 255, 255, 255));
      }
      else
      {
         if(in_bSum)
         {
            in_pLabel->FontColor(GColorRGBInt(200, 200, 200, 255));
         }
         else
         {
            in_pLabel->FontColor(GColorRGBInt(150, 150, 150, 255));
         }
      }
   }

   if(in_bSelected)
      in_pLabel->ColorBackground(GColorRGBInt(46, 46, 46, 255));
   else
      in_pLabel->ColorBackground(GColorRGBInt(20, 20, 20, 255));
}

void GResourcesWindow::DisplayResourceInfo()
{
   bool l_bRootNode = true;
   
   if(m_pSelectedNode->Nodes().size() == 0)
      l_bRootNode = false;

   //if it's a root node disable stuff
   if(l_bRootNode)
   {
      m_pObjManagement->Enabled(false);
      m_pObjStatus->Enabled(false);
      m_pObjSectorTax->Enabled(false);
      m_pObjPartGdp->Enabled(false);
      m_pObjMarketShare->Enabled(false);
      m_pObjIncreaseProduction->Enabled(false);
      m_pObjMarketAvail->Enabled(false);
      m_pObjTrade->Enabled(false);
      m_pObjDesired->Enabled(false);
      m_pObjActual->Enabled(false);
      m_pObjMeetConsumption->Enabled(false);

      m_pObjManagementLbl->Enabled(false);     
      m_pObjStatusLbl->Enabled(false);         
      m_pObjSectorTaxLbl->Enabled(false);      
      m_pObjPartGdpLbl->Enabled(false);        
      m_pObjMarketShareLbl->Enabled(false);    
      m_pObjMarketAvailLbl->Enabled(false);    
      m_pObjTradeLbl->Enabled(false);          
      m_pObjDesiredLbl->Enabled(false);        
      m_pObjActualLbl->Enabled(false);         
      m_pObjMeetComsumptionLbl->Enabled(false);

      m_pObjManagement->Selected_Content(L"");
      m_pObjStatus->Selected_Content(L"");
      m_pObjSectorTax->TextValueObj()->Text(L"");
      m_pObjPartGdp->Text(L"");
      m_pObjMarketShare->Text(L"");
      m_pObjMarketAvail->Text(L"");
      m_pObjTrade->Selected_Content(L"");
      m_pObjDesired->TextValueObj()->Text(L"");
      m_pObjActual->Text(L"");

      if( g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID() )
      {
         m_pObjGlobalTaxMod->Enabled(true);
         m_pObjGlobalTaxModLbl->Enabled(true);
      }
      else
      {
         m_pObjGlobalTaxMod->Enabled(false);
         m_pObjGlobalTaxModLbl->Enabled(false);
      }

      return;
   }
   else
   {
      if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
      {
         m_pObjManagement->Enabled(true);
         m_pObjStatus->Enabled(true);
         m_pObjSectorTax->Enabled(true);
         m_pObjPartGdp->Enabled(true);
         m_pObjMarketShare->Enabled(true);
         m_pObjIncreaseProduction->Enabled(true);
         m_pObjMarketAvail->Enabled(true);
         m_pObjTrade->Enabled(true);
         m_pObjDesired->Enabled(true);
         m_pObjActual->Enabled(true);
         m_pObjMeetConsumption->Enabled(true);

         m_pObjManagementLbl->Enabled(true);     
         m_pObjStatusLbl->Enabled(true);         
         m_pObjSectorTaxLbl->Enabled(true);      
         m_pObjPartGdpLbl->Enabled(true);        
         m_pObjMarketShareLbl->Enabled(true);    
         m_pObjMarketAvailLbl->Enabled(true);    
         m_pObjTradeLbl->Enabled(true);          
         m_pObjDesiredLbl->Enabled(true);        
         m_pObjActualLbl->Enabled(true);         
         m_pObjMeetComsumptionLbl->Enabled(true);

         m_pObjGlobalTaxMod->Enabled(true);
         m_pObjGlobalTaxModLbl->Enabled(true);
      }
      else
      {
         m_pObjManagement->Enabled(false);
         m_pObjStatus->Enabled(false);
         m_pObjSectorTax->Enabled(false);
         m_pObjPartGdp->Enabled(false);
         m_pObjMarketShare->Enabled(false);
         m_pObjIncreaseProduction->Enabled(false);
         m_pObjMarketAvail->Enabled(false);
         m_pObjTrade->Enabled(false);
         m_pObjDesired->Enabled(false);
         m_pObjActual->Enabled(false);
         m_pObjMeetConsumption->Enabled(false);

         m_pObjManagementLbl->Enabled(false);     
         m_pObjStatusLbl->Enabled(false);         
         m_pObjSectorTaxLbl->Enabled(false);      
         m_pObjPartGdpLbl->Enabled(false);        
         m_pObjMarketShareLbl->Enabled(false);    
         m_pObjMarketAvailLbl->Enabled(false);    
         m_pObjTradeLbl->Enabled(false);          
         m_pObjDesiredLbl->Enabled(false);        
         m_pObjActualLbl->Enabled(false);         
         m_pObjMeetComsumptionLbl->Enabled(false);

         m_pObjGlobalTaxMod->Enabled(false);
         m_pObjGlobalTaxModLbl->Enabled(false);
      }
   }

   INT32 i = 0;
   for(i=0; i<6; i++)
   {
      if(m_pObjTree[i] == m_pSelectedNode->View())
         break;
   }

   if(i >= 6)
      return;

   INT32 j = 0;
   for(j=0; j<(INT32)m_pObjTree[i]->Nodes()[0]->Nodes().size(); j++)
   {
      if(m_pObjTree[i]->Nodes()[0]->Nodes()[j] == m_pSelectedNode)
         break;
   }

   INT32 l_iIndex = 0 + j;
   if(i == 1)
      l_iIndex = 6 + j;

   if(i == 2)
      l_iIndex = 8 + j;

   if(i == 3)
      l_iIndex = 12 + j;

   if(i == 4)
      l_iIndex = 16 + j;

   if(i == 5)
      l_iIndex = 21 + j;

   m_iSelectedId = l_iIndex;

   //Set Gvt Control to Government/Private
   if(m_pData->m_bManagement[l_iIndex])
      m_pObjManagement->Selected_Content(g_ClientDAL.GetString(100386));
   else
      m_pObjManagement->Selected_Content(g_ClientDAL.GetString(100090));

   //Legal/Illegal
   if(m_pData->m_bStatus[l_iIndex])
      m_pObjStatus->Selected_Content(g_ClientDAL.GetString(100093));
   else
      m_pObjStatus->Selected_Content(g_ClientDAL.GetString(100092));

   //Sector tax
   m_pObjSectorTax->Value(m_pData->m_fSectorTax[l_iIndex] * 100, false, false);

   //% of GDP
   m_pObjPartGdp->Text(GString::FormatNumber(m_pData->m_fPartGdp[l_iIndex] * 100, L"", L".", L"", L" %", 0, 2));

   //Market share
   m_pObjMarketShare->Text(GString::FormatNumber(m_pData->m_fMarketShare[l_iIndex] * 100, L"", L".", L"", L" %", 0, 2));

   //Market availability
   m_pObjMarketAvail->Text(GString::FormatNumber(m_pData->m_fMarketAvailability[l_iIndex] / 1000000, L" ", L"", L"", L" M", 3, 0));

   if(m_pData->m_bMeetConsumption[l_iIndex])
   {
      m_pObjMeetConsumption->ToggleState(true);

      m_pObjDesired->TextValueObj()->Text(L"");
      m_pObjDesired->Enabled(false);
      m_pObjDesiredLbl->Enabled(false);

      m_pObjTrade->Selected_Content(L"");
      m_pObjTrade->Enabled(false);
      m_pObjTradeLbl->Enabled(false);
   }
   else
   {
      m_pObjMeetConsumption->ToggleState(false);

      if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
      {
         m_pObjTrade->Enabled(true);
         m_pObjTradeLbl->Enabled(true);
         m_pObjDesired->Enabled(true);
         m_pObjDesiredLbl->Enabled(true);
      }
      else
      {
         m_pObjTrade->Enabled(false);
         m_pObjTradeLbl->Enabled(false);
         m_pObjDesired->Enabled(false);
         m_pObjDesiredLbl->Enabled(false);
      }

      if(m_pData->m_fDesired[l_iIndex] < 0)
      {
         //import desired
         m_pObjTrade->Selected_Content(g_ClientDAL.GetString(100387));
         m_pObjDesired->Value((-m_pData->m_fDesired[l_iIndex]) / 1000000, false, false);
      }
      else
      {
         //export desired
         m_pObjTrade->Selected_Content(g_ClientDAL.GetString(100388));
         m_pObjDesired->Value(m_pData->m_fDesired[l_iIndex] / 1000000, false, false);
      }
   }

   if(m_pData->m_bManagement[l_iIndex])
   {
      m_pObjSectorTax->Enabled(false);
      m_pObjSectorTax->TextValueObj()->Text(L"");
      m_pObjSectorTaxLbl->Enabled(false);      

      if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
      {
         m_pObjActual->Enabled(true);
         m_pObjMeetConsumption->Enabled(true);

         m_pObjActualLbl->Enabled(true);         
         m_pObjMeetComsumptionLbl->Enabled(true);
      }
      else
      {
         m_pObjActual->Enabled(false);
         m_pObjMeetConsumption->Enabled(false);

         m_pObjActualLbl->Enabled(false);         
         m_pObjMeetComsumptionLbl->Enabled(false);
      }

      m_pObjActual->Text(GString::FormatNumber(m_pData->m_fTrade[l_iIndex], L" ", L".", L"$ ", L"", 3, 0, false));
   }
   else
   {
      if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
      {
         m_pObjSectorTax->Enabled(true);
         m_pObjSectorTaxLbl->Enabled(true);    
      }
      else
      {
         m_pObjSectorTax->Enabled(false);
         m_pObjSectorTaxLbl->Enabled(false);    
      }

      m_pObjActual->Enabled(false);
      m_pObjMeetConsumption->Enabled(false);
      m_pObjMeetConsumption->ToggleState(false);

      m_pObjActualLbl->Enabled(false);         
      m_pObjMeetComsumptionLbl->Enabled(false);

      m_pObjDesired->Enabled(false);
      m_pObjDesiredLbl->Enabled(false);

      m_pObjTrade->Enabled(false);
      m_pObjTradeLbl->Enabled(false);

      m_pObjTrade->Selected_Content(L"");
      m_pObjDesired->TextValueObj()->Text(L"");
      m_pObjActual->Text(L"");
   }
}

void GResourcesWindow::OnSubjectChange(DesignPattern::GSubject&  in_Subject,const DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   // is there any request pending for our ressource window, if yes do not notify
   if(!RequestPending())
      if(&in_Subject == &g_ClientDAL)
         if(in_Notification == (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::Resources)
            Update();
}

void GResourcesWindow::Save()
{
   if(!Dirty())
      return;

   m_fWhenSaved = g_Joshua.GameTime();

   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GResourcesUpdate);
   SP2::Event::GResourcesUpdate* l_pUpdate = (SP2::Event::GResourcesUpdate*)l_pEvent.get();

   l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pUpdate->m_iSource = g_SP2Client->Id();
   l_pUpdate->m_iCountryID = g_ClientDAL.ControlledCountryID();
   l_pUpdate->m_bGetData   = false;
   l_pUpdate->m_fGeneralTaxMod = m_pData->m_fGlobalTaxMod;

   UINT8 l_iIndex = 0;
   for(INT32 i=0; i<EResources::ItemCount; i++)
   {
      if(m_pData->m_bDirty[i])
      {
         l_pUpdate->m_iID[l_iIndex]              = i;
         l_pUpdate->m_fTaxes[l_iIndex]           = m_pData->m_fSectorTax[i];
         l_pUpdate->m_bGvtCtrl[l_iIndex]         = m_pData->m_bManagement[i];
         l_pUpdate->m_bLegal[l_iIndex]           = m_pData->m_bStatus[i];
         l_pUpdate->m_bMeetConsumption[l_iIndex] = m_pData->m_bMeetConsumption[i];
         
         if(m_pData->m_fDesired[i] < 0)
         {
            l_pUpdate->m_fImportDesired[l_iIndex]   = -m_pData->m_fDesired[i];
            l_pUpdate->m_fExportDesired[l_iIndex]   = 0.f;
         }
         else
         {
            l_pUpdate->m_fExportDesired[l_iIndex]   = m_pData->m_fDesired[i];
            l_pUpdate->m_fImportDesired[l_iIndex]   = 0.f;
         }

         l_iIndex++;
      }
   }

   l_pUpdate->m_iNbResources = l_iIndex;
   // next answer accepted by the client from our server will be our request acknowledgement
   g_ClientDCL.RequestManager().AddRequest(this, l_pUpdate);   

   g_Joshua.RaiseEvent(l_pEvent);
   Dirty(false);
}