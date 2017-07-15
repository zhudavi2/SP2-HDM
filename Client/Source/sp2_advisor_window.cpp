/**************************************************************
*
* sp2_advisor_window.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_advisor_window.h"

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

const GString GAdvisorWindow::TypeName(L"Advisor_Window");

GUI::GBaseObject* GAdvisorWindow::New()
{
	return (GUI::GBaseObject*) new GAdvisorWindow();
}


GAdvisorWindow::GAdvisorWindow() : SP2::GBaseWindow()
{
   m_iNbPart = 0;
   m_iCurrentPlan = -1;
}

GAdvisorWindow::~GAdvisorWindow()
{
}

bool GAdvisorWindow::OnCreate()
{
	__super::OnCreate();

   m_pObjInnerFrm    = (GUI::GFrame*)Child(L"frmInner");
   m_pObjProblemFrm  = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmProblem");
   m_pObjPlanFrm     = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmPlan");
   m_pObjResultFrm   = (GUI::GFrame*)m_pObjInnerFrm->Child(L"frmResult");
   m_pObjGradientFrm = (GUI::GFrame*)Child(L"picBlueGradient");
   m_pObjGradientBlackFrm = (GUI::GFrame*)Child(L"picShadowGradient");
   m_pObjExecuteBtn  = (GUI::GButton*)m_pObjInnerFrm->Child(L"btnExecute");

   m_pObjProblemLbl  = (GUI::GLabel*)m_pObjProblemFrm->Child(L"txtProblem");
   m_pObjResultLbl   = (GUI::GLabel*)m_pObjResultFrm->Child(L"frmResults")->Child(L"txtResult");

   ((GFX::GWindow*)m_pObjResultFrm->Child(L"frmResults")->Model()->Get_Primitive())->ClipChildren(true);

   m_pObjResultScr   = (GUI::GVScrollBar*)m_pObjResultFrm->Child(L"scrResult");

   m_pObjLockTog->Hide();
   m_pObjApplyBtn->Hide();

	return true;
}

//! Show Handler
/*!
 * Called when the object is displayed
 **/
void GAdvisorWindow::OnShow()
{
   __super::OnShow();

   g_ClientDDL.MainBar()->BringToFront();
   g_ClientDDL.MiniMap()->BringToFront();
}

GUI::EEventStatus::Enum GAdvisorWindow::OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller)
{
   if(in_pCaller)
   {
      if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
      {
         if(in_pCaller == m_pObjExecuteBtn || in_pCaller == m_pObjExitBtn)
         {
            vector<SP2::GAdvisorPlan>::iterator l_It = m_Plans.begin();
            while(l_It != m_Plans.end())
            {
               if(l_It->m_ObjectiveID == m_iCurrentPlan)
               {
                  SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GSendPlan);
                  SP2::Event::GSendPlan* l_pUpdate = (SP2::Event::GSendPlan*)l_pEvent.get();
                  l_pUpdate->m_iSource = g_SP2Client->Id();
                  l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;               
                  l_pUpdate->m_iObjectiveID = l_It->m_ObjectiveID;
                  l_pUpdate->m_TypeOfPlan = l_It->m_Type;
                  
                  if(l_It->m_Type == SP2::ETypeOfPlan::Specific)
                     for(UINT32 i=0; i<l_It->m_Actions.size(); i++)
                        l_pUpdate->m_vActionsID.push_back(pair<UINT32, UINT32>(l_It->m_Actions[i].first, l_It->m_Actions[i].second));
                  
                  if(in_pCaller == m_pObjExecuteBtn)
                     l_pUpdate->m_bAccept = true;
                  else
                     l_pUpdate->m_bAccept = false;
                  
                  g_Joshua.RaiseEvent(l_pEvent);
                  
                  m_Plans.erase(l_It);
                  break;
               }

               l_It++;
            }

            if(m_Plans.size())
            {
               DisplayPlan(m_Plans[0].m_ObjectiveID);
            }
            else
            {
               for(UINT32 i=0; i<m_Frames.size(); i++)
               {
                  m_Frames[i]->Hide();
                  g_Joshua.GUIManager()->ReleaseObject(m_Frames[i]);
               }
               m_Frames.clear();

               Hide();
            }
         }
      }
   }

   return GUI::EEventStatus::Handled;
}

void GAdvisorWindow::AddPlanPart(GString in_sText, INT32 in_iTargetCountry)
{
   if(in_iTargetCountry == -1)
      in_iTargetCountry = g_ClientDAL.ControlledCountryID();

   GUI::GFrame* l_pPlanFrm = (GUI::GFrame*)CREATE_GUI_OBJECT(L"frmAdvisorAction", GString(m_iNbPart) + L"PartOfPlan", m_pObjPlanFrm, true, false);
   m_Frames.push_back(l_pPlanFrm);

   const GCountry& l_Country = g_ClientDAL.Country(in_iTargetCountry);

   ((GUI::GLabel*)l_pPlanFrm->Child(L"txtTitle"))->Text(in_sText);
   ((GUI::GPictureBox*)l_pPlanFrm->Child(L"picTarget"))->PictureNormal(g_ClientDDL.SmallFlagsTextures()[l_Country.Id()]);
   ((GUI::GLabel*)l_pPlanFrm->Child(L"txtTarget"))->Text(l_Country.Name());

   l_pPlanFrm->Left(6);
   l_pPlanFrm->Top(21 + (m_iNbPart * 49));

   Height(Height() + 49);
   m_pObjInnerFrm->Height(m_pObjInnerFrm->Height() + 49);
   m_pObjGradientFrm->Height(m_pObjGradientFrm->Height() + 49);
   m_pObjGradientBlackFrm->Height(m_pObjGradientBlackFrm->Height() + 49);
   m_pObjResultFrm->Top(m_pObjResultFrm->Top() + 49);
   m_pObjPlanFrm->Height(m_pObjPlanFrm->Height() + 49);
   m_pObjExecuteBtn->Top(m_pObjExecuteBtn->Top() + 49);

   m_iNbPart++;
}

void GAdvisorWindow::Problem(GString in_sText)
{
   m_pObjProblemLbl->Text(in_sText);
}

void GAdvisorWindow::Result(GString in_sText)
{
   Height(Height() - 5);
   m_pObjInnerFrm->Height(m_pObjInnerFrm->Height() - 5);
   m_pObjGradientFrm->Height(m_pObjGradientFrm->Height() - 5);
   m_pObjGradientBlackFrm->Height(m_pObjGradientBlackFrm->Height() - 5);
   m_pObjResultFrm->Top(m_pObjResultFrm->Top() - 5);
   m_pObjPlanFrm->Height(m_pObjPlanFrm->Height() - 5);
   m_pObjExecuteBtn->Top(m_pObjExecuteBtn->Top() - 5);

   m_pObjResultLbl->Text(in_sText);
   
   m_pObjResultLbl->Top(0);
   m_pObjResultLbl->Height( m_pObjResultLbl->Text2D()->Height() );
   m_pObjResultScr->Initialize( this, 0, max(0,m_pObjResultLbl->Height() - m_pObjResultLbl->Owner()->Height()), 15, 45, 0 );
}

EEventStatus::Enum GAdvisorWindow::OnCustomEvent(UINT32 in_EventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller)
{
   switch(in_EventID)
   {
   case ECustomEvents::OnScrollVertical:
      {
         if(in_pCaller == m_pObjResultScr)
         {
            m_pObjResultLbl->Top( -(INT32)in_EventData.Data );
         }
      }
      break;
   }

   return GUI::EEventStatus::Handled;
}

void GAdvisorWindow::Plan(const GAdvisorPlan & in_Plan)
{
   m_Plans.push_back(in_Plan);

   if(m_iCurrentPlan == -1)
      DisplayPlan(in_Plan.m_ObjectiveID);
}

void GAdvisorWindow::DisplayPlan(INT32 in_iID)
{
   m_iNbPart = 0;

   for(UINT32 i=0; i<m_Frames.size(); i++)
   {
      m_Frames[i]->Hide();
      g_Joshua.GUIManager()->ReleaseObject(m_Frames[i]);
   }
   
   m_Frames.clear();

   m_pObjPlanFrm->Height(23);
   m_pObjResultFrm->Top(101);
   m_pObjExecuteBtn->Top(194);
   m_pObjInnerFrm->Height(217);
   m_pObjGradientFrm->Height(243);
   m_pObjGradientBlackFrm->Height(243);
   Height(250);

   //For each plan
   for(UINT32 i=0; i<m_Plans.size(); i++)
   {
      //Find the wanted plan
      if(m_Plans[i].m_ObjectiveID == in_iID)
      {
         m_iCurrentPlan = in_iID;
         
         //Problem
         GString l_sObjective = g_ClientDAL.GetString( g_ClientDAL.StringIdAdvisorObjective((SP2::EAdvisorObjectives::Enum)m_Plans[i].m_ObjectiveID) );
         Problem(l_sObjective);

         //Plan
         for(UINT32 j=0; j<m_Plans[i].m_Actions.size(); j++)
         {
            AddPlanPart(g_ClientDAL.GetString(g_ClientDAL.StringIdAdvisorAction((SP2::EAdvisorActions::Enum)m_Plans[i].m_Actions[j].second)), m_Plans[i].m_Actions[j].first);
         }

         //Result
         GString l_sText;			
         for(UINT32 j=0; j<m_Plans[i].m_Results.size(); j++)
         {
				l_sText += g_ClientDAL.GetString(g_ClientDAL.StringIdObjectiveResult((SP2::EAdvisorObjectives::Enum)m_Plans[i].m_Results[j].first,m_Plans[i].m_Results[j].second));
				l_sText += "\r\n";            
         }
         Result(l_sText);

         break;
      }
   }

   if(!Visible())
      Show();
}