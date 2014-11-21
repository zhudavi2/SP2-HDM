/**************************************************************
*
* sp2_end_game_window.cpp
*
* Description
* ===========
*  See Header file
*
* Owner
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

// Includes
#include "golem_pch.hpp"
#include "../include/sp2_end_game_window.h"
#include "../include/sp2_main_menu.h"

// Constants
const GString GEndGameWindow::TypeName(L"Game_Over_Window");

// Constructors and Destructors
GUI::GBaseObject* GEndGameWindow::New()
{
   return new GEndGameWindow();
}

GEndGameWindow::GEndGameWindow() : SP2::GBaseWindow()
{

}

GEndGameWindow::~GEndGameWindow()
{
   
}

bool GEndGameWindow::OnCreate()
{
   __super::OnCreate();

   m_pObjApplyBtn->Hide();
   //m_pObjExitBtn->Hide();
 
   // Collect gui items
   m_pObjMainMenuBtn             = (GUI::GButton*) Child("frmInner")->Child("btnMainMenu");
//   m_pObjChoiceBtn               = (GUI::GButton*) Child("frmInner")->Child("btnAction");
   m_pObjTitleLbl                = (GUI::GLabel*)  Child("frmTitle")->Child("lblTitle");
   m_pObjSummary                 = (GUI::GLabel*)  Child("frmInner")->Child("frmText")->Child("txtText");
   m_pObjSummary->Multiline(true);
   m_pObjObjectivesList          = (GUI::GListBox*)Child("frmInner")->Child("frmObjectives")->Child("lstObjectives");  
   
   return true;
}

void GEndGameWindow::ScenarioCompleted(void)
{
   if(!Visible() )
   {
      m_eMode = EndGameWindowMode::ScenarioCompleted;
      m_pObjTitleLbl->Text(g_ClientDAL.GetString(102129));
//      m_pObjChoiceBtn->Text(g_ClientDAL.GetString(102130));

      if( (g_SP2Client->CurrentGameType() == EGameTypes::JoinMultiPlayer) ||
          (g_SP2Client->CurrentGameType() == EGameTypes::HostMultiPlayer) )
      {
//         m_pObjChoiceBtn->Hide();
      }
      else
      {
//         m_pObjChoiceBtn->Show();
      }

      m_pObjObjectivesList->Clear();

      // The title of a bad objective contain the explication
      m_pObjSummary->Text(g_ClientDAL.GetString(102131));

      if( (g_SP2Client->CurrentGameType() == EGameTypes::Scenario) &&
         (g_SP2Client->ScenarioIsHard() ) )
      {
         g_SP2Client->PlayCredits(true);
      }

      for(list<GGameObjective*>::const_iterator it = g_ClientDAL.Scenario()->Objectives().begin();
          it != g_ClientDAL.Scenario()->Objectives().end();++ it)
      {
         vector<GGameObjective> l_vTemp(1, *(*it) );
         AddObjective(l_vTemp);
      }

      Show();
   }
}

void GEndGameWindow::GameOver(SP2::Event::GGameOver* in_pGaveOverEvent)
{
   if(!Visible() )
   {
      gassert(in_pGaveOverEvent,"Game Over event is null");
      m_eMode = EndGameWindowMode::GameOver;
      m_pObjTitleLbl->Text(g_ClientDAL.GetString(102133));  
//      m_pObjChoiceBtn->Text(g_ClientDAL.GetString(102134));


      if( (g_SP2Client->CurrentGameType() == EGameTypes::JoinMultiPlayer) ||
          (g_SP2Client->CurrentGameType() == EGameTypes::HostMultiPlayer) ) 
      {
//         m_pObjChoiceBtn->Hide();
      }
      else
      {
//         m_pObjChoiceBtn->Show();
      }


      // The title of a bad objective contain the explication
      if(in_pGaveOverEvent->m_vGameTimedOutObjectives.size())
      {
         m_pObjSummary->Text(g_ClientDAL.GetString(102253));
      }
      else if(in_pGaveOverEvent->m_vGameOverObjectives.size())
      {
         GGameObjective l_Objective = in_pGaveOverEvent->m_vGameOverObjectives[0];

         if(l_Objective.m_eObjectiveType == EObjectiveType::EconomicFailure)
         {
            m_pObjSummary->Text(g_ClientDAL.GetString(101220));
         }
         else if(l_Objective.m_eObjectiveType == EObjectiveType::LoseElections)
         {
            m_pObjSummary->Text(g_ClientDAL.GetString(101222));
         }
         else if(l_Objective.m_eObjectiveType == EObjectiveType::BeAnnexed)
         {
            m_pObjSummary->Text(g_ClientDAL.GetString(101221));
         }
         else if(l_Objective.m_eObjectiveType == EObjectiveType::BeTargetOfSuccessfulCoupEtat)
         {
            m_pObjSummary->Text(g_ClientDAL.GetString(101074));
         }
      }

      m_pObjObjectivesList->Clear();

      if(g_SP2Client->CurrentGameType() == EGameTypes::HostMultiPlayer)
      {
         m_pObjSummary->Text(m_pObjSummary->Text() + GString("\r\n\r\n") + g_ClientDAL.GetString(102252) );
      }

      AddObjective(in_pGaveOverEvent->m_vCompletedObjectives);
      AddObjective(in_pGaveOverEvent->m_vGameTimedOutObjectives);

      Show();
   }
}

GUI::EEventStatus::Enum GEndGameWindow::OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller)
{
   __super::OnMouseClick(in_EventData, in_pCaller);  

   if(in_EventData.Mouse.Actor.Bits.Left || in_EventData.Mouse.Down.Bits.Left)
   {
      if(in_pCaller)
      {
         if(in_pCaller == m_pObjExitBtn || in_pCaller == m_pObjMainMenuBtn)
         {
            Hide();
            g_SP2Client->StopGame();
         }
/*         else if(in_pCaller == m_pObjChoiceBtn)
         {
            if(m_eMode == EndGameWindowMode::GameOver)
            {
               // JMercier Note : I bet the following code don't reset any objectives...
               g_SP2Client->RestartGame();
               Hide();
            }
         }*/
      }
   }

   return GUI::EEventStatus::Handled;
}

void GEndGameWindow::AddObjective(vector<GGameObjective>& in_Objective)
{
   for(UINT32 i=0; i<in_Objective.size(); i++)
   {
      list<SP2::GGameObjective*>::const_iterator l_Objs = g_ClientDAL.Scenario()->Objectives().begin();

      while( l_Objs != g_ClientDAL.Scenario()->Objectives().end() )
      {  
         if(in_Objective[i].m_Id == (*l_Objs)->m_Id)
         {
            GUI::GListBox::GRow * l_pRow = m_pObjObjectivesList->Insert_New_Row();

            GLabel *l_pName = (GLabel*)l_pRow->Object()->Child("txtName");
            GLabel *l_pTime = (GLabel*)l_pRow->Object()->Child("txtTime");
            GLabel *l_pStatus = (GLabel*)l_pRow->Object()->Child("txtSuccess");

            l_pName->Text( g_ClientDAL.GetString( (*l_Objs)->m_Title.ToINT32() ) );
            
            if( !(*l_Objs)->m_fTimeOutTime )
               l_pTime->Text( g_ClientDAL.GetString(101956) );
            else
               l_pTime->Text( GString::FormatNumber( (*l_Objs)->m_fTimeOutTime, 0 ) + L" " + g_ClientDAL.GetString(100672));

            switch((*l_Objs)->m_eObjectiveStatus)
            {
               case EObjectiveStatus::Reached:
               case EObjectiveStatus::AlwaysReached:
                  l_pStatus->Text(g_ClientDAL.GetString(102132));
                  break;
               default:
                  break;
            }
            if((*l_Objs)->IsTimedOut()) 
               l_pStatus->Text(g_ClientDAL.GetString(102135));
         }

         l_Objs++;
      }
   }
}