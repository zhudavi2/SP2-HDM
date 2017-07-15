/**************************************************************
*
* sp2_frame_scenario.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_frame_scenario.h"
#include "../../includes/common_lib//sp2_game_scenario.h"

const GString GFrameScenario::TypeName(L"frmScenarioInner");

GUI::GBaseObject* GFrameScenario::New()
{
	return (GUI::GBaseObject*) new GFrameScenario();
}


GFrameScenario::GFrameScenario()
   : GFrame()
   , m_pSelectedScenario(NULL)
{
}

GFrameScenario::~GFrameScenario()
{
}

bool GFrameScenario::OnCreate()
{
	__super::OnCreate();

   m_pObjScenarioTab          = (GUI::GTabbed*)Child(L"tabInfo");
   m_pObjScenarioTitle        = (GUI::GLabel*)Child(L"frmInfo")->Child(L"txtTitle");
   m_pObjScenarioCountry      = (GUI::GLabel*)Child(L"frmInfo")->Child(L"txtCountry");
   m_pObjObjectiveList        = (GUI::GListBox*)m_pObjScenarioTab->m_vTabPage[1]->Child(L"frmObjectives")->Child(L"lstGoals");
   m_pObjTipsFrm              = (GUI::GFrame*)m_pObjScenarioTab->m_vTabPage[2]->Child(L"frmInner");
   m_pObjDescriptionFrm       = (GUI::GFrame*)m_pObjScenarioTab->m_vTabPage[0]->Child(L"frmInner");
   m_pObjTipsScr              = (GUI::GVScrollBar*)m_pObjScenarioTab->m_vTabPage[2]->Child(L"scrText");
   m_pObjDescriptionScr       = (GUI::GVScrollBar*)m_pObjScenarioTab->m_vTabPage[0]->Child(L"scrText");
   m_pObjTipsLbl              = (GUI::GLabel*)m_pObjTipsFrm->Child(L"txtTips");
   m_pObjDescriptionLbl       = (GUI::GLabel*)m_pObjDescriptionFrm->Child(L"txtDescription");

   ((GFX::GWindow*)m_pObjTipsFrm->Model()->Get_Primitive())->ClipChildren(true);
   ((GFX::GWindow*)m_pObjDescriptionFrm->Model()->Get_Primitive())->ClipChildren(true);

   m_pObjScenarioTab->m_vTabPage[0]->ColorNormal(GColorRGBInt(0,0,0,255));
   m_pObjScenarioTab->m_vTabPage[1]->ColorNormal(GColorRGBInt(0,0,0,255));
   m_pObjScenarioTab->m_vTabPage[2]->ColorNormal(GColorRGBInt(0,0,0,255));

   m_pObjScenarioTab->m_vTabPage[0]->PictureNormal("000647");
   m_pObjScenarioTab->m_vTabPage[1]->PictureNormal("000647");
   m_pObjScenarioTab->m_vTabPage[2]->PictureNormal("000647");

   m_pObjScenarioTab->m_vTabPage[0]->Borders( GBorders(1, 1, 1, 1) );
   m_pObjScenarioTab->m_vTabPage[1]->Borders( GBorders(1, 1, 1, 1) );
   m_pObjScenarioTab->m_vTabPage[2]->Borders( GBorders(1, 1, 1, 1) );

   m_pObjScenarioTab->m_vTabPage[0]->TooltipText( g_ClientDAL.GetString(102231) );
   m_pObjScenarioTab->m_vTabPage[1]->TooltipText( g_ClientDAL.GetString(102232) );
   m_pObjScenarioTab->m_vTabPage[2]->TooltipText( g_ClientDAL.GetString(102233) );

   m_pObjScenarioTab->m_vTabButtons[0]->TooltipText( g_ClientDAL.GetString(102231) );
   m_pObjScenarioTab->m_vTabButtons[1]->TooltipText( g_ClientDAL.GetString(102232) );
   m_pObjScenarioTab->m_vTabButtons[2]->TooltipText( g_ClientDAL.GetString(102233) );

   m_pObjScenarioTab->m_vTabButtons[0]->Text( g_ClientDAL.GetString(EStrId::Description) );
   m_pObjScenarioTab->m_vTabButtons[1]->Text( g_ClientDAL.GetString(EStrId::Objectives) );
   m_pObjScenarioTab->m_vTabButtons[2]->Text( g_ClientDAL.GetString(EStrId::Tips) );

   m_pObjScenarioTab->m_vTabButtons[0]->BringToFront();
   m_pObjScenarioTab->m_vTabButtons[1]->BringToFront();
   m_pObjScenarioTab->m_vTabButtons[2]->BringToFront();

   m_pObjScenarioCountry->Enabled(false);

   return true;
}

GSmartPtr<SP2::GGameScenario> GFrameScenario::Scenario()
{
   return m_pSelectedScenario;
}

void GFrameScenario::Scenario( GSmartPtr<GGameScenario> in_pScenario )
{
   m_pSelectedScenario = in_pScenario;
}

INT32 GFrameScenario::SelectedCountryID()
{
   return m_pSelectedScenario->Country();
}

void GFrameScenario::UpdateSelectedScenario()
{
   m_pSelectedScenario = g_ClientDAL.Scenario(); 

   if(m_pSelectedScenario.get() )
   {
      // Is it a custom scenario or not ?
      if(m_pSelectedScenario->Title().ToINT32() > 0)
      {
         m_pObjScenarioTitle->Text( g_ClientDAL.StringReplaceNewLineChar(g_ClientDAL.GetString(m_pSelectedScenario->Title().ToINT32())));
         m_pObjDescriptionLbl->Text( g_ClientDAL.StringReplaceNewLineChar(g_ClientDAL.GetString(m_pSelectedScenario->Description().ToINT32())));
         m_pObjTipsLbl->Text( g_ClientDAL.StringReplaceNewLineChar(g_ClientDAL.GetString(m_pSelectedScenario->Tips().ToINT32())));
      }
      else
      { 
         m_pObjScenarioTitle->Text( m_pSelectedScenario->Title());
         m_pObjDescriptionLbl->Text(m_pSelectedScenario->Description());
         m_pObjTipsLbl->Text(m_pSelectedScenario->Tips());
      }

      m_pObjDescriptionLbl->Height( m_pObjDescriptionLbl->Text2D()->Height() );
      m_pObjTipsLbl->Height( m_pObjTipsLbl->Text2D()->Height() );
      
      m_pObjTipsScr->Initialize( this, 0, max(0, m_pObjTipsLbl->Height() - m_pObjTipsFrm->Height()), 15, 45 );
      m_pObjDescriptionScr->Initialize( this, 0, max(0, m_pObjDescriptionLbl->Height() - m_pObjDescriptionFrm->Height()), 15, 45 );

      // Add up country
      if(m_pSelectedScenario->Country() > 0)
      {
         m_pObjScenarioCountry->Text(g_ClientDAL.Country(m_pSelectedScenario->Country()).Name());
      }
      else
         m_pObjScenarioCountry->Text("");

      if(g_ClientDAL.ControlledCountryID() > 0)
         m_pObjScenarioCountry->Text(g_ClientDAL.Country(g_ClientDAL.ControlledCountryID()).Name());
     
      // Then put objectives 
      INT32 l_iCounter = 0;
      list<GGameObjective*>::const_iterator l_ObjectiveIterator = m_pSelectedScenario->Objectives().begin();
      while(l_ObjectiveIterator != m_pSelectedScenario->Objectives().end())
      {
         GUI::GListBox::GRow* l_pRow = NULL;
         
         if( m_pObjObjectiveList->Get_Nb_Rows() <= l_iCounter )
            l_pRow = m_pObjObjectiveList->Insert_New_Row();            
         else
            l_pRow = m_pObjObjectiveList->Get_Row_At( l_iCounter );

         l_iCounter++;

         GUI::GLabel*      l_pTimeTxt  = (GUI::GLabel*)l_pRow->Object()->Child("txtTime");
         GUI::GLabel*      l_pTitleLbl = (GUI::GLabel*)l_pRow->Object()->Child("txtGoal");
         GUI::GToggle*     l_pObjectiveComplete = (GUI::GToggle*) l_pRow->Object()->Child("frmTog")->Child("togGoal");

         l_pTitleLbl->Text( g_ClientDAL.StringReplaceNewLineChar(g_ClientDAL.GetString( (*l_ObjectiveIterator)->m_Title.ToINT32() ) ) );
         if((*l_ObjectiveIterator)->m_fTimeOutTime > 0 )
            l_pTimeTxt->Text(GString::FormatNumber((*l_ObjectiveIterator)->m_fTimeOutTime/365.25f,2)+g_ClientDAL.GetString(101957));
         else
            l_pTimeTxt->Text( g_ClientDAL.GetString(101956) );


         switch((*l_ObjectiveIterator)->m_eObjectiveStatus)
         {
            case EObjectiveStatus::Reached:
            case EObjectiveStatus::AlwaysReached:
               l_pObjectiveComplete->ToggleState( true );
               break;
            case EObjectiveStatus::NotReached:
            case EObjectiveStatus::TimedOut:
               l_pObjectiveComplete->ToggleState( false );
               break;
            default:
               gassert(0,"Unhandled case");
               break;
         }
         l_pObjectiveComplete->Enabled(false);

         // Prendre le prochain objectif
         l_ObjectiveIterator++;
      }

      for( INT32 i = m_pObjObjectiveList->Get_Nb_Rows() - 1; i >= l_iCounter; i-- )
      {
         m_pObjObjectiveList->RemoveRow( i );
      }

      m_pObjObjectiveList->Update();
   }
}

EEventStatus::Enum GFrameScenario::OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   switch(in_iEventID)
   {
      case ECustomEvents::OnScrollVertical:
      {
         if(in_pCaller == m_pObjDescriptionScr)
         {
            m_pObjDescriptionLbl->Top( -(INT32)in_EventData.Data );
         }
         else if(in_pCaller == m_pObjTipsScr)
         {
            m_pObjTipsLbl->Top( -(INT32)in_EventData.Data );
         }
      }
      break;
   }

   return GUI::EEventStatus::Handled;
}