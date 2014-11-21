/**************************************************************
*
* sp2_Military_event_handler.cpp
*
* Description
* ===========
*  Used to handle every Military events that happen
*  on client side
*
*  
*  
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_military_window.h"
#include "../include/sp2_unit_movement_denial_window.h"
#include "../include/sp2_battle_summary_window.h"
#include "../include/sp2_battle_overview_window.h"
#include "../include/sp2_mail_window.h"
#include "../include/sp2_covert_actions_window.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"

GMilitaryEventHandler::GMilitaryEventHandler()
{

}

GMilitaryEventHandler::~GMilitaryEventHandler()
{
}

void GMilitaryEventHandler::HandleUnitResearchUpdateRequest(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GUnitResearchUpdate* l_pEvent = (SP2::Event::GUnitResearchUpdate*)in_Event.get();

   SP2::GResearchInformationHolder & m_Info = g_ClientDAL.m_ResearchInfo;

   m_Info.m_fBudgetGround                          = l_pEvent->m_fBudgetGround;
   m_Info.m_fBudgetAir                             = l_pEvent->m_fBudgetAir;
   m_Info.m_fBudgetNaval                           = l_pEvent->m_fBudgetNaval;
   m_Info.m_fBudgetNuclear                         = l_pEvent->m_fBudgetNuclear;
   memcpy(m_Info.m_fBudget    ,l_pEvent->m_fBudget    ,sizeof(REAL64)*EUnitCategory::ItemCount*EUnitDesignCharacteristics::ItemCount);
   memcpy(m_Info.m_fMaxValues ,l_pEvent->m_fMaxValues ,sizeof(REAL32)*EUnitCategory::ItemCount*EUnitDesignCharacteristics::ItemCount);
}

void GMilitaryEventHandler::HandleRefuseMilitaryAction(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GRefuseMilitaryAction* l_pEvent = (SP2::Event::GRefuseMilitaryAction*) in_Event.get();

   // Group refused movement together
   UINT32 l_iRefused[SP2::ERefusedMoveReason::Count];
   Memory::Clear(l_iRefused, SP2::ERefusedMoveReason::Count);

   for(UINT32 i = 0;i < l_pEvent->m_vRefusedMoves.size();i ++)
   {
      gassert(l_pEvent->m_vRefusedMoves[i].second < SP2::ERefusedMoveReason::Count, "Invalid move refuse reason");
      l_iRefused[l_pEvent->m_vRefusedMoves[i].second] ++;
   }

   // Inform user of each different refusal type
   for(UINT32 i = 0;i < SP2::ERefusedMoveReason::Count;i ++)
   {
      if(l_iRefused[i] > 0)
      {
         g_ClientDCL.RefuseMovement(l_iRefused[i], (SP2::ERefusedMoveReason::Enum) i);
      }
   }
}


void GMilitaryEventHandler::HandleMissionOver(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GSendCovertOpsMissionDone* l_pEvent = (SP2::Event::GSendCovertOpsMissionDone*)in_Event.get();

   SDataMissionOver l_Data;
   
   l_Data.m_bSuccessful          = l_pEvent->m_bSuccessful;
   l_Data.m_eComplexity          = l_pEvent->m_eComplexity;
   l_Data.m_eTargetSector        = l_pEvent->m_eTargetSector;
   l_Data.m_eType                = l_pEvent->m_eType;
   l_Data.m_iCellID              = l_pEvent->m_iCellID;
   l_Data.m_iCellSourceCountry   = l_pEvent->m_iCellSourceCountry;
   l_Data.m_iFramedCountry       = l_pEvent->m_iFramedCountry;
   l_Data.m_iResourceOrUnitType  = l_pEvent->m_iResourceOrUnitType;
   l_Data.m_iTargetCountry       = l_pEvent->m_iTargetCountry;
   l_Data.m_sCellName            = l_pEvent->m_sCellName;
	l_Data.m_bCaptured				= l_pEvent->m_bCaptured;

   INT32 l_iMailID = 0;
   
   g_ClientDAL.m_DataMissionOver.insert(make_pair(g_ClientDDL.MailWindow()->NextMailID(), l_Data));

   if(l_Data.m_iTargetCountry == (UINT32)g_ClientDAL.ControlledCountryID())
   {
      GString l_sAttemptBy = g_ClientDAL.GetString(EStrId::AttemptBy);

      GString l_sResult;
      if(l_Data.m_bSuccessful)
      {
         if(g_ClientDDL.CovertActionsWindow() && g_ClientDDL.CovertActionsWindow()->Visible())
            g_ClientDCL.ShowMissionResult( (void*)g_ClientDDL.MailWindow()->NextMailID() );
         else
         {
            l_iMailID = g_ClientDDL.MailWindow()->AddMail(l_sResult + GString(L" ") + g_ClientDAL.ConvertMissionTypeToString(l_Data.m_eType) , GDataControlLayer::ShowMissionResult, (void*)g_ClientDDL.MailWindow()->NextMailID());
            l_sResult = g_ClientDAL.GetString(EStrId::CovertActionsAttackedSuccessful);
         }
      }
      else
      {
         if(g_ClientDDL.CovertActionsWindow() && g_ClientDDL.CovertActionsWindow()->Visible())
            g_ClientDCL.ShowMissionResult( (void*)g_ClientDDL.MailWindow()->NextMailID() );
         else
         {
            l_sResult = g_ClientDAL.GetString(EStrId::CovertActionsAttackedFailed);
            l_iMailID = g_ClientDDL.MailWindow()->AddMail(l_sResult + GString(L" ") + g_ClientDAL.ConvertMissionTypeToString(l_Data.m_eType) + L" " + l_sAttemptBy + L" " + g_ClientDAL.Country(l_Data.m_iCellSourceCountry).Name() , GDataControlLayer::ShowMissionResult, (void*)g_ClientDDL.MailWindow()->NextMailID());
         }
      }

   }
   else if(l_Data.m_iCellSourceCountry == (UINT32)g_ClientDAL.ControlledCountryID())
   {
      if(g_ClientDDL.CovertActionsWindow() && g_ClientDDL.CovertActionsWindow()->Visible())
         g_ClientDCL.ShowMissionResult( (void*)g_ClientDDL.MailWindow()->NextMailID() );
      else
      {
         GString l_sResult;
         if(l_Data.m_bSuccessful)
            l_sResult = g_ClientDAL.GetString(EStrId::CovertActionSuccessful);
         else
			{
				if(l_Data.m_bCaptured)
				  l_sResult = g_ClientDAL.GetString(EStrId::CovertActionFailedCaptured);
				else
					l_sResult = g_ClientDAL.GetString(EStrId::CovertActionFailedNotCaptured);
			}

         l_iMailID = g_ClientDDL.MailWindow()->AddMail(g_ClientDAL.ConvertMissionTypeToString(l_Data.m_eType) + GString(L" (") + g_ClientDAL.Country(l_Data.m_iCellSourceCountry).Name() + GString(L") : ") + l_sResult , GDataControlLayer::ShowMissionResult, (void*)g_ClientDDL.MailWindow()->NextMailID());
      }
   }
   else if(l_Data.m_iFramedCountry == (UINT32)g_ClientDAL.ControlledCountryID())
   {
      if(g_ClientDDL.CovertActionsWindow() && g_ClientDDL.CovertActionsWindow()->Visible())
         g_ClientDCL.ShowMissionResult( (void*)g_ClientDDL.MailWindow()->NextMailID() );
      else
      {
         GString l_sResult;
         if(l_Data.m_bSuccessful)
         {
            l_sResult = g_ClientDAL.GetString(EStrId::CovertActionsFramedByCountrySuccessful);
            l_iMailID = g_ClientDDL.MailWindow()->AddMail(l_sResult , GDataControlLayer::ShowMissionResult, (void*)g_ClientDDL.MailWindow()->NextMailID());
         }
         else
         {
            l_sResult = g_ClientDAL.GetString(EStrId::CovertActionsFramedByCountryFailed);
            l_iMailID = g_ClientDDL.MailWindow()->AddMail(g_ClientDAL.Country(l_Data.m_iCellSourceCountry).Name() + GString(L" ") + l_sResult , GDataControlLayer::ShowMissionResult, (void*)g_ClientDDL.MailWindow()->NextMailID());
         }
      }   
   }
   // remove the cell from the country data if it was captured !
   if(l_Data.m_bCaptured)
   {

      hash_map<UINT32, INT32>::iterator l_It = g_ClientDAL.m_CellsIdReady.find( l_Data.m_iCellID );
      if(l_It != g_ClientDAL.m_CellsIdReady.end())
      {
         //Remove the mail of the "Ready to execute" (if still present)
         if(g_ClientDDL.MailWindow()->MailExists(l_It->second))
            g_ClientDDL.MailWindow()->RemoveMail(l_It->second);
         else
            g_ClientDAL.m_CellsIdReady.erase(l_It);
      }
      
      if(g_ClientDDL.CovertActionsWindow() && g_ClientDDL.CovertActionsWindow()->Visible())
         g_ClientDDL.CovertActionsWindow()->ClearBeforeUpdate(true);
   }
}

/*!
 * Notification received from the server when a combat is over
 * @param in_Event: Smart pointer to a SP2::Event::GCombatOver event
 */
void GMilitaryEventHandler::HandleCombatOver(SDK::GGameEventSPtr in_Event)
{
   //If we were in the combat, display the combat summary
   SP2::Event::GCombatOver* l_pCombatOverEvt = (SP2::Event::GCombatOver*)in_Event.get();

   //If we were in either the attacker country list or the defender country list
   if(
      (find(l_pCombatOverEvt->m_ArenaInfo.m_vAttackerCountriesList.begin(),l_pCombatOverEvt->m_ArenaInfo.m_vAttackerCountriesList.end(),g_ClientDAL.ControlledCountryID()) != l_pCombatOverEvt->m_ArenaInfo.m_vAttackerCountriesList.end()) ||
      (find(l_pCombatOverEvt->m_ArenaInfo.m_vDefenderCountriesList.begin(),l_pCombatOverEvt->m_ArenaInfo.m_vDefenderCountriesList.end(),g_ClientDAL.ControlledCountryID()) != l_pCombatOverEvt->m_ArenaInfo.m_vDefenderCountriesList.end())
     )
   {
      // If we are in the battle overview don't show these in front and make sure that if its the combat we are looking to set the end combat
      if(g_ClientDDL.BattleOverviewWindow())
      {
         if(g_ClientDDL.BattleOverviewWindow()->DoFinalUpdate(l_pCombatOverEvt->m_ArenaInfo))
         {
            g_ClientDDL.BattleSummaryWindowSpawn(g_ClientDDL.BattleOverviewWindow());
            g_ClientDDL.BattleSummaryWindow()->Hide();
            g_ClientDDL.BattleSummaryWindow()->SetArenaInformation(l_pCombatOverEvt->m_ArenaInfo);       
            g_ClientDDL.MainBar()->SendToBack();
            g_ClientDDL.MiniMap()->SendToBack();
         }
         else
         {
            // JMercier : What to do?

         }
      }
      else
      {
         // only display if not in tutorial mode !
         if(GTutorials::Instance().StartedTutorial() == ETutorials::None)
         {
            // add the combat result as a mail
            g_ClientDAL.AddCombatOverMail(l_pCombatOverEvt->m_ArenaInfo);
         }
      }
   }
   else
   {
      //Display which side is victorious
      ECombatSide::Enum l_eWinnerSide = l_pCombatOverEvt->m_ArenaInfo.m_eWinnerSide;

      UINT32 l_iWinnerCountryID = 0;
      UINT32 l_iLoserCountryID  = 0;
      switch(l_eWinnerSide)
      {
         case ECombatSide::Attacker:
            l_iWinnerCountryID = l_pCombatOverEvt->m_ArenaInfo.m_vAttackerCountriesList[0];
            l_iLoserCountryID  = l_pCombatOverEvt->m_ArenaInfo.m_vDefenderCountriesList[0];
            break;
         case ECombatSide::Defender:
            l_iWinnerCountryID = l_pCombatOverEvt->m_ArenaInfo.m_vDefenderCountriesList[0];
            l_iLoserCountryID  = l_pCombatOverEvt->m_ArenaInfo.m_vAttackerCountriesList[0];
            break;
         default:
         {
            //Dont display a news
            return;
         }
      }

		/*
			//Display a news about the battle outcome
			News::GInfo l_News;
			l_News.m_iCountryA = l_iWinnerCountryID;
			l_News.m_iCountryB = l_iLoserCountryID;
			l_News.m_eType     = News::EType::BattleOver;
			l_News.m_iPriority = g_ClientDAL.NewsPriority(l_News.m_eType);
			l_News.m_eCategory = g_ClientDAL.NewsCategory(l_News.m_eType);
			l_News.m_vStrings.push_back(g_ClientDAL.RegionNameStringId(l_pCombatOverEvt->m_ArenaInfo.m_iRegionID));
			g_ClientDAL.InsertPendingNews(l_News);
		*/
   }
   return;
}

void GMilitaryEventHandler::HandleBombardmentNotification(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GBombardmentNotification* l_pEvt = (SP2::Event::GBombardmentNotification*)in_Event.get();

   g_ClientDCL.OnBombardmentNotification(l_pEvt->m_iTargetRegion,l_pEvt->m_bStarting,l_pEvt->m_vBombardedPoints);

   return;
}