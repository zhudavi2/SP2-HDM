/**************************************************************
*
* sp2_game_objective_evaluator.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"



GGameObjectiveEvaluator::GGameObjectiveEvaluator()
{
   m_bWorldPeace = false;
}

GGameObjectiveEvaluator::~GGameObjectiveEvaluator()
{
}

void GGameObjectiveEvaluator::EvaluateSteamAchievementsForPlayer(SDK::GPlayer* in_pPlayer)
{
   gassert(in_pPlayer,"Player Pointer is NULL!!!");

   // check 8 (self sufficient)
   if(!g_Joshua.ServerGetSteamAchievementState(8))
   {   
      bool lCheck8 = true;
      for(EResources::Enum r = (EResources::Enum)0; r < (EResources::Enum)EResources::ItemCount; r = (EResources::Enum)(r + 1))
      {
         if(g_ServerDAL.CountryData(in_pPlayer->ModID())->ResourceProduction(r) < g_ServerDAL.CountryData(in_pPlayer->ModID())->ResourceDemand(r))
         {
            lCheck8 = false;
            break;
         }
      }

      if(lCheck8)
      {
         g_Joshua.ServerSetSteamAchievementState(8, lCheck8);

         SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GObjectiveCompleted);
         l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
         l_Event->m_iTarget = in_pPlayer->Id();
         SP2::Event::GObjectiveCompleted* l_pObjectiveCompleted = (SP2::Event::GObjectiveCompleted*)l_Event.get();
         l_pObjectiveCompleted->m_iCompletedObjectivesID = 10000 + 8;
         l_pObjectiveCompleted->m_eStatus = EObjectiveStatus::Reached;
         g_Joshua.RaiseEvent(l_Event);
      }
   }

   // check 9 (genocide)
   if(!g_Joshua.ServerGetSteamAchievementState(9))
   {
      INT64 lPopulation = 0;
      for(UINT32 i = 0 ; i < g_ServerDAL.NbCountry() ; i++)
      {
         SP2::GCountryData* l_pCountry = g_ServerDAL.CountryData(i+1);
         if(l_pCountry)
            lPopulation += l_pCountry->Population();
      }

      if(lPopulation < 3000000000)  // about half
      {
         g_Joshua.ServerSetSteamAchievementState(9, true);

         SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GObjectiveCompleted);
         l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
         l_Event->m_iTarget = in_pPlayer->Id();
         SP2::Event::GObjectiveCompleted* l_pObjectiveCompleted = (SP2::Event::GObjectiveCompleted*)l_Event.get();
         l_pObjectiveCompleted->m_iCompletedObjectivesID = 10000 + 9;
         l_pObjectiveCompleted->m_eStatus = EObjectiveStatus::Reached;
         g_Joshua.RaiseEvent(l_Event);
      }
   }

   // check 10 (conquer the world)
   if(!g_Joshua.ServerGetSteamAchievementState(10))
   {
      const vector<SP2::GRegionControl>& l_vRegionControl = g_ServerDAL.RegionControlArray();
      bool lCheck10 = true;
      for(UINT32 i = 0 ; i < l_vRegionControl.size() ; i++)
      {
         if(l_vRegionControl[i].m_iPolitical != 0 && l_vRegionControl[i].m_iPolitical != in_pPlayer->ModID())
         {
            lCheck10 = false;
            break;
         }
      }

      if(lCheck10)
      {
         g_Joshua.ServerSetSteamAchievementState(10, true);

         SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GObjectiveCompleted);
         l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
         l_Event->m_iTarget = in_pPlayer->Id();
         SP2::Event::GObjectiveCompleted* l_pObjectiveCompleted = (SP2::Event::GObjectiveCompleted*)l_Event.get();
         l_pObjectiveCompleted->m_iCompletedObjectivesID = 10000 + 10;
         l_pObjectiveCompleted->m_eStatus = EObjectiveStatus::Reached;
         g_Joshua.RaiseEvent(l_Event);
      }
   }   

   // check 11 (world peace)
   if(!g_Joshua.ServerGetSteamAchievementState(11) && m_bWorldPeace)
   {
      g_Joshua.ServerSetSteamAchievementState(11, true);

      SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GObjectiveCompleted);
      l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
      l_Event->m_iTarget = in_pPlayer->Id();
      SP2::Event::GObjectiveCompleted* l_pObjectiveCompleted = (SP2::Event::GObjectiveCompleted*)l_Event.get();
      l_pObjectiveCompleted->m_iCompletedObjectivesID = 10000 + 11;
      l_pObjectiveCompleted->m_eStatus = EObjectiveStatus::Reached;
      g_Joshua.RaiseEvent(l_Event);
   }   

   // check 12 (everyone likes you)
   SP2::GCountryData* l_pCountry = g_ServerDAL.CountryData(in_pPlayer->ModID());
   if(!g_Joshua.ServerGetSteamAchievementState(12) && l_pCountry)
   {
      if(l_pCountry->GvtApproval() > 0.95f && l_pCountry->GvtStability() > 0.90f)
      {
         g_Joshua.ServerSetSteamAchievementState(12, true);

         SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GObjectiveCompleted);
         l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
         l_Event->m_iTarget = in_pPlayer->Id();
         SP2::Event::GObjectiveCompleted* l_pObjectiveCompleted = (SP2::Event::GObjectiveCompleted*)l_Event.get();
         l_pObjectiveCompleted->m_iCompletedObjectivesID = 10000 + 12;
         l_pObjectiveCompleted->m_eStatus = EObjectiveStatus::Reached;
         g_Joshua.RaiseEvent(l_Event);
      }
   }

   // check 13 (Psilon)
   if(!g_Joshua.ServerGetSteamAchievementState(13))
   {
      bool lCheck13 = true;
      if(l_pCountry)
      {
         GResearchInformationItf* lResearch = l_pCountry->ResearchInfo();
         if(lResearch)
         {
            for(int x = 0; x < EUnitCategory::ItemCount; x++)
            {
               for(int y = 0; y < EUnitDesignCharacteristics::ItemCount; y++)
               {
                   //For ground, don't check stealth or countermeasures
                   if(x == EUnitCategory::Ground && y == EUnitDesignCharacteristics::Stealth)
                       break;

                   //For strategic, don't check anything except missile range, precision, and damage
                   if(x == EUnitCategory::Nuclear)
                   {
                       if(y < EUnitDesignCharacteristics::MissileRange)
                           continue;
                       else if(y > EUnitDesignCharacteristics::MissileDamage)
                           break;
                   }

                  if(lResearch->m_fMaxValues[x][y] >= 0.0f && lResearch->m_fMaxValues[x][y] < 10.0f)
                  {
                     lCheck13 = false;
                     break;
                  }
               }

               if(!lCheck13)
                  break;
            }
         }
      }

      if(lCheck13)
      {
         g_Joshua.ServerSetSteamAchievementState(13, true);

         SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GObjectiveCompleted);
         l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
         l_Event->m_iTarget = in_pPlayer->Id();
         SP2::Event::GObjectiveCompleted* l_pObjectiveCompleted = (SP2::Event::GObjectiveCompleted*)l_Event.get();
         l_pObjectiveCompleted->m_iCompletedObjectivesID = 10000 + 13;
         l_pObjectiveCompleted->m_eStatus = EObjectiveStatus::Reached;
         g_Joshua.RaiseEvent(l_Event);
      }   
   }
   //check 6- new nuclear power
   if(!g_Joshua.ServerGetSteamAchievementState(6) && l_pCountry)
   {
      if(l_pCountry->AMDSLevel() >= 1.0f)
      {
         g_Joshua.ServerSetSteamAchievementState(6, true);

         SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GObjectiveCompleted);
         l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
         l_Event->m_iTarget = in_pPlayer->Id();
         SP2::Event::GObjectiveCompleted* l_pObjectiveCompleted = (SP2::Event::GObjectiveCompleted*)l_Event.get();
         l_pObjectiveCompleted->m_iCompletedObjectivesID = 10000 + 6;
         l_pObjectiveCompleted->m_eStatus = EObjectiveStatus::Reached;
         g_Joshua.RaiseEvent(l_Event);
      }
   }

   // raise events
   for(int c = 0; c < 14; c++)
   {
      if(g_Joshua.ServerGetSteamAchievementState(c))
      {
         SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GObjectiveCompleted);
         l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
         l_Event->m_iTarget = in_pPlayer->Id();
         SP2::Event::GObjectiveCompleted* l_pObjectiveCompleted = (SP2::Event::GObjectiveCompleted*)l_Event.get();
         l_pObjectiveCompleted->m_iCompletedObjectivesID = 10000 + c;
         l_pObjectiveCompleted->m_eStatus = EObjectiveStatus::Reached;
         g_Joshua.RaiseEvent(l_Event);

         // clear events flags
         g_Joshua.ServerSetSteamAchievementState(c, false);
      }
   }
}

/*!
 * Evaluates the objectives for the given player
 **/
bool GGameObjectiveEvaluator::EvaluateObjectives()
{
   //For every human player, Evaluate the objectives for player
   vector<UINT32> l_vPlayersToRemove;
   for(SDK::GPlayers::const_iterator l_HumanPlayersIt = g_Joshua.HumanPlayers().begin() ;
      l_HumanPlayersIt != g_Joshua.HumanPlayers().end();
      l_HumanPlayersIt++)
   {
      if(l_HumanPlayersIt->second->PlayerStatus() == SDK::PLAYER_STATUS_ACTIVE)
      {
         // evaluate steam objectives as well
         EvaluateSteamAchievementsForPlayer(l_HumanPlayersIt->second);


         bool                    l_bAllObjectivesSuccessfullyMet;
         vector<GGameObjective*> l_vCompletedObjectives;
         vector<GGameObjective*> l_vTimedOutObjectives;
         vector<GGameObjective*> l_vGameOverObjectives;
         EvaluateObjectivesForPlayer(l_HumanPlayersIt->second,
                                    l_bAllObjectivesSuccessfullyMet,
                                    l_vCompletedObjectives,
                                    l_vTimedOutObjectives,
                                    l_vGameOverObjectives);


         //If the game is over (objectives that werent supposed to be met are met)
         if(l_vGameOverObjectives.size())
         {
            g_Joshua.Log("-----Game Over--- for player, a negative objective was reached, bye bye");

            //Mention to him that his game is over and the reason why
            SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GGameOver);
            l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
            l_Event->m_iTarget = SDK::Event::ESpecialTargets::BroadcastHumanPlayers;
            SP2::Event::GGameOver* l_pGameOver = (SP2::Event::GGameOver*)l_Event.get();
            SDK::GPlayer* l_pPlayer = g_Joshua.HumanPlayer(l_HumanPlayersIt->second->Id());
            l_pGameOver->m_iTerminatedCountry = l_pPlayer->ModID();
            CopyGameObjectives(l_vGameOverObjectives,l_pGameOver->m_vGameOverObjectives);
            g_Joshua.RaiseEvent(l_Event);

            //Mark that player as to be removed
            l_vPlayersToRemove.push_back(l_HumanPlayersIt->second->Id());
         }
         //If all objectives successfully met, game is over because we have a winner
         //Possibility to continue the game
         else if(l_bAllObjectivesSuccessfullyMet && l_vCompletedObjectives.size())
         {
            gassert((l_vTimedOutObjectives.size() == 0) && 
                  (l_vGameOverObjectives.size() == 0),"GGameObjectiveEvaluator::EvaluateObjectives If all objectives successfully met, game over or time out is illogic");


            g_Joshua.Log("----- A Player won --- for player (all objectives met), wish to continue ?");

            //Build & Send the event and wait for a confirmation of the new status
            {
               SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GGameOver);
               l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
               l_Event->m_iTarget = SDK::Event::ESpecialTargets::BroadcastHumanPlayers;
               SP2::Event::GGameOver* l_pGameOver = (SP2::Event::GGameOver*)l_Event.get();
               l_pGameOver->m_iWinnerCountry = l_HumanPlayersIt->second->ModID();
               CopyGameObjectives(l_vCompletedObjectives,l_pGameOver->m_vCompletedObjectives);            
               g_Joshua.RaiseEvent(l_Event);
            }
         }
         //Some Objectives timed out
         //Possibility to continue the game.
         else if(l_vTimedOutObjectives.size())
         {  
           
            g_Joshua.Log("----- One or more objective timed out... wish to continue ?");

            //Build & send the game event
            {
               SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GGameOver);
               l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
               l_Event->m_iTarget = SDK::Event::ESpecialTargets::BroadcastHumanPlayers;
               SP2::Event::GGameOver* l_pGameOver = (SP2::Event::GGameOver*)l_Event.get();
               CopyGameObjectives(l_vTimedOutObjectives,l_pGameOver->m_vGameTimedOutObjectives);
               g_Joshua.RaiseEvent(l_Event);
            }

            //Mark that player as to be removed
            l_vPlayersToRemove.push_back(l_HumanPlayersIt->second->Id());
         }
      }//End of if player is active
   }
   //Remove all the players that should be removed
   for(UINT32 i = 0 ; i < l_vPlayersToRemove.size() ;i++)
   {
      //Game is over for a player, remove him
      g_Joshua.ReplaceHumanByAIPlayer(g_Joshua.HumanPlayer(l_vPlayersToRemove[i] ) );
   }

   //Clear the lists that are filled because we do not want to accumulate the items if the
   //objectives are not evaluated (and if they were to be used, they would have been in the previous
   //loops
   {
      m_vCountryWherePoliticalPartyChanged.clear();
      m_vCountryTargetOfSuccessfulCoupEtat.clear();
      m_vCountryThatLaunchedNukes.clear();
   }

   return true;
}

void GGameObjectiveEvaluator::EvaluateObjectivesForPlayer(SDK::GPlayer* in_pPlayer,
                                                          bool& out_vAllObjectivesMetSuccessfully,
                                                          vector<GGameObjective*>& out_vCompletedObjectives,
                                                          vector<GGameObjective*>& out_vTimedOutObjectives,
                                                          vector<GGameObjective*>& out_vGameOverObjectives)
{
   gassert(in_pPlayer,"Player Pointer is NULL!!!");
   static GIOBuffer l_CopyOfGIOBuffer;

   out_vAllObjectivesMetSuccessfully = true;
   hash_map<UINT32,vector<GGameObjective> >::iterator l_It = g_ServerDAL.PlayersObjectives().find(in_pPlayer->Id());
   if(l_It == g_ServerDAL.PlayersObjectives().end())
   {  //no objective to reach
      return;
   }

   //There are 2 ways for a game over
   //  1- All objectives successfully completed
   //  2- One objective was failed or a negative objective was reached
   bool l_bGameOver                         = false;

   //For each objective
   vector<GGameObjective>& l_Objectives = l_It->second;
   for(UINT32 i = 0; i < l_Objectives.size() ; i++)
   {

      //If the objective is Always Reached, do not reevalutate
      if(l_Objectives[i].m_eObjectiveStatus == EObjectiveStatus::AlwaysReached)
      {
         continue;
      }

      bool l_bObjectiveMustBeReached = true;
      //If objective is a negative objective, it must not be reached
      if(l_Objectives[i].m_eObjectiveType >= EObjectiveType::FirstNegativeObjective &&
         l_Objectives[i].m_eObjectiveType <= EObjectiveType::LastNegativeObjective)
      {
         l_bObjectiveMustBeReached = false;
      }

      bool l_bBufferTransfered = false;
      // Get a copy
      if(l_Objectives[i].m_Buffer.Size())
      {
         l_CopyOfGIOBuffer = l_Objectives[i].m_Buffer;
         l_bBufferTransfered = true;
      }

      EObjectiveStatus::Enum l_eObjectiveStatus = EObjectiveStatus::TimedOut;
      switch(l_Objectives[i].m_eObjectiveType)
      {
      //Positive Objectives
      //-------------------------------------
         case EObjectiveType::ConquerTheWorld:
            l_eObjectiveStatus = EvalConquerTheWorld(in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::RaiseGDPPerCapita:
            l_eObjectiveStatus = EvalRaiseGDPPerCapita(in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::AchieveWorldPeace:
            l_eObjectiveStatus = EvalAchieveWorldPeace(in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::DevelopNation:
            l_eObjectiveStatus = EvalDevelopNation(in_pPlayer,l_Objectives[i]);
            break;
      //Negative Objectives
      //-------------------------------------
         case EObjectiveType::BeAnnexed:
            l_eObjectiveStatus = EvalBeAnnexed(in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::EconomicFailure:
            l_eObjectiveStatus = EvalEconomicFailure(in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::BeTargetOfSuccessfulCoupEtat:
            //Need convert ops to evaluate this objective
            l_eObjectiveStatus = EvalBeTargetOfSuccessfulCoupEtat(in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::LoseElections:
            l_eObjectiveStatus = EvalLoseElections(in_pPlayer,l_Objectives[i]);;
            break;
       //Scenario Objectives
       //--------------------------------------
         case EObjectiveType::RaiseStability:
            l_eObjectiveStatus = EvalStabilizeCountry(*in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::RaiseEconomicHealth:
            l_eObjectiveStatus = EvalRaiseEconomicHealth(*in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::MeetResourcesNeeds:
            l_eObjectiveStatus = EvalMeetRessourcesNeed(*in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::InvadeCountry:
            l_eObjectiveStatus = EvalInvasion(*in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::BeInTreaty:
            l_eObjectiveStatus = EvalBeInATreaty(*in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::AnnexCountry:
            l_eObjectiveStatus = EvalAnnexCountry(*in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::ResourceControl:
            l_eObjectiveStatus = EvalRessourceControl(*in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::PureCommunistEonomy:
            l_eObjectiveStatus = EvalPureCommunistEonomy(*in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::GovernementType:
            l_eObjectiveStatus = EvalGovernmentType(*in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::LaunchANuke:
            l_eObjectiveStatus = EvalLaunchANuke(*in_pPlayer,l_Objectives[i]);
            break;
         case EObjectiveType::BuildUnitCategory:
            l_eObjectiveStatus = EvalBuildUnitCategory(*in_pPlayer,l_Objectives[i]);
            break;
         default:
            gassert(0,"GGameObjectiveEvaluator::EvaluateObjectives: Unhandled switch statement");
            break;
      }

      // Restore GIOBuffer
      if(l_bBufferTransfered)
         l_Objectives[i].m_Buffer = l_CopyOfGIOBuffer;


      switch(l_Objectives[i].m_eObjectiveStatus)
      {
         case EObjectiveStatus::AlwaysReached:
            gassert(0,"This objective shouldnt have been reevaluated");
            break;
         //If if objective was completed and is not completed anymore, set as not completed, and
         //warn of the objective status change
         case EObjectiveStatus::Reached:
         {
            if(l_eObjectiveStatus == EObjectiveStatus::NotReached)
            {
               l_Objectives[i].m_eObjectiveStatus = l_eObjectiveStatus;
               //Send the not completed objective ID 
               SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GObjectiveCompleted);
               l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
               l_Event->m_iTarget = in_pPlayer->Id();
               SP2::Event::GObjectiveCompleted* l_pObjectiveCompleted = (SP2::Event::GObjectiveCompleted*)l_Event.get();
               l_pObjectiveCompleted->m_iCompletedObjectivesID = l_Objectives[i].m_Id;
               l_pObjectiveCompleted->m_eStatus = l_eObjectiveStatus;
               g_Joshua.RaiseEvent(l_Event);
            }
            else if(l_eObjectiveStatus == EObjectiveStatus::TimedOut)
            {
               gassert(0,"How can a previously completed objective timeout ?");
            }
            break;
         }
         //If the objective was not completed and is now completed
         case EObjectiveStatus::NotReached:
         {
            if(l_eObjectiveStatus == EObjectiveStatus::Reached ||
               l_eObjectiveStatus == EObjectiveStatus::AlwaysReached)
            {
               l_Objectives[i].m_eObjectiveStatus = l_eObjectiveStatus;
               //Send the completed objective ID 
               SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GObjectiveCompleted);
               l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
               l_Event->m_iTarget = in_pPlayer->Id();
               SP2::Event::GObjectiveCompleted* l_pObjectiveCompleted = (SP2::Event::GObjectiveCompleted*)l_Event.get();
               l_pObjectiveCompleted->m_iCompletedObjectivesID = l_Objectives[i].m_Id;
               l_pObjectiveCompleted->m_eStatus = l_eObjectiveStatus;
               g_Joshua.RaiseEvent(l_Event);
            }

         }
         case EObjectiveStatus::TimedOut:
            break;
         default:
            gassert(0,"Unhandled case");
            break;
      }

      //If objective timed out or an objective that wasnt supposed to be reached
      //was reached, game is over
      if((l_eObjectiveStatus == EObjectiveStatus::TimedOut) ||
         (!l_bObjectiveMustBeReached && (l_eObjectiveStatus == EObjectiveStatus::Reached)))
      {
         l_bGameOver = true;
         out_vAllObjectivesMetSuccessfully = false;
      }
      //If objective must be reached and is not reached, game is not over
      //because all the objectives are not met.
      else if(l_bObjectiveMustBeReached && (l_eObjectiveStatus == EObjectiveStatus::NotReached))
      {
         out_vAllObjectivesMetSuccessfully = false;
      }
      else if(l_bObjectiveMustBeReached && ((l_eObjectiveStatus == EObjectiveStatus::Reached) || (l_eObjectiveStatus == EObjectiveStatus::AlwaysReached)))
      {
         out_vCompletedObjectives.push_back(&l_Objectives[i]);
      }

      //If game is over because of FAILURE
      if(l_bGameOver)
      {
         //if game is overbecause of a timeout and player is the admin player, 
         //Possibility to extend the game
         if(l_eObjectiveStatus == EObjectiveStatus::TimedOut)
         {
            out_vTimedOutObjectives.push_back(&l_Objectives[i]);
         }
         //Game is definitively over for that player
         else
         {            
            out_vGameOverObjectives.push_back(&l_Objectives[i]);
         }
         l_bGameOver = false;

      }//end game over
   } 
}


/*!
 * Creates a copy of all the objectives contained in in_vObjectives and adds them to out_vObjectives
 **/
void GGameObjectiveEvaluator::CopyGameObjectives(vector<GGameObjective*>& in_vObjectives,vector<GGameObjective>& out_vObjectives)
{  
   //Create a copy of the completed objectives and add them to the event
   for(UINT32 i = 0 ; i < in_vObjectives.size() ; i++)
   {
      SP2::GGameObjective l_ObjectiveCpy;
      GIBuffer l_SerializedBuffer;
      in_vObjectives[i]->Serialize(l_SerializedBuffer);
      GOBuffer l_Unserialize(l_SerializedBuffer);
      l_ObjectiveCpy.Unserialize(l_Unserialize);
      out_vObjectives.push_back(l_ObjectiveCpy);
   }
   return;
}

/*!
 * Observer received a notification
 **/
void GGameObjectiveEvaluator::OnSubjectChange(Hector::DesignPattern::GSubject&                   in_Subject,
                                              const Hector::DesignPattern::GSubjectNotification& in_Notification,
                                              void*                                              in_pAdditionalMsg)
{
   if(&in_Subject == &g_SP2Server->WorldBehavior())
   {
      //If you lost the elections
      if(in_Notification == c_iNotificationLeadingPoliticalPartyChangeAfterElections)
      {
         //There was a political party change in the country
         UINT32 l_iCountryId = (UINT32)in_pAdditionalMsg;

         //If the country is owned by a human player
         if(!g_Joshua.ActivePlayerByModID(l_iCountryId)->AIControlled())
         {
            //Add the country where there was a political party change into the list
            m_vCountryWherePoliticalPartyChanged.push_back(l_iCountryId);
         }
      }
   }
   if(&in_Subject == &g_SP2Server->DCL())
   {
      // Target of a coup d'etat
      //-------------------------------------------------------
      if(in_Notification == c_iNotificationOnSuccessfulCoupEtat)
      {
         //There was a political party change in the country
         UINT32 l_iCountryId = (UINT32)in_pAdditionalMsg;

         //If the country is owned by a human player
         if(!g_Joshua.ActivePlayerByModID(l_iCountryId)->AIControlled())
         {
            //Add the country where there was a political party change into the list
            m_vCountryTargetOfSuccessfulCoupEtat.push_back(l_iCountryId);
         }
      }
      // Nuke was launched
      //-------------------------------------------------------
      else if(in_Notification == c_iNotificationOnNukeLaunched)
      {
         GLaunchedNuke* l_pTempLaunchNuke = (GLaunchedNuke*) in_pAdditionalMsg;
         UINT32 l_iLauncherCountryID = l_pTempLaunchNuke->m_Missile.m_iOwnerID;
         //If the country is owned by a human player
         if(!g_Joshua.ActivePlayerByModID(l_iLauncherCountryID)->AIControlled())
         {
            //Locate the target country
            UINT32 l_iTargetRegion = g_ServerDCL.EarthLocateRegion(l_pTempLaunchNuke->m_TargetPosition.x,l_pTempLaunchNuke->m_TargetPosition.y);
            UINT32 l_iTargetCountry = g_ServerDAL.RegionControl(l_iTargetRegion).m_iPolitical;
            //Insert into the nuke launched vector
            m_vCountryThatLaunchedNukes.push_back(make_pair(l_iLauncherCountryID,l_iTargetCountry));
         }
      }
      // World peace was signed
      //-------------------------------------------------------
      else if(in_Notification == c_iWorldPeaceStatusChanged)
      {
         m_bWorldPeace = *(bool*)in_pAdditionalMsg;
      }
   }
}

EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalRaiseGDPPerCapita(SDK::GPlayer* in_pPlayer,
                                                                         GGameObjective& in_Objective)
{
   //Get the country data
   SP2::GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_pPlayer->ModID());
   gassert(l_pCountryData,"GGameObjectiveEvaluator::EvalRaiseGDPPerCapita, NULL Pointer exception");


   if(in_Objective.IsTimedOut())
   {
      return EObjectiveStatus::TimedOut;
   }
   else
   {//Check if objective reached
      REAL64 l_fGDPPerCapitaActual = l_pCountryData->GDPPerCapita();
      REAL64 l_fInitialGDPPerCapita;
      in_Objective.m_Buffer >> l_fInitialGDPPerCapita;
      REAL64 l_fGDPPerCapitaValueToReach = l_fInitialGDPPerCapita * in_Objective.m_fValueToReach;
      if(l_fGDPPerCapitaActual >= l_fGDPPerCapitaValueToReach)
      {
         return EObjectiveStatus::Reached;
      }
      else
      {
         return EObjectiveStatus::NotReached;
      }
   }
}

/*!
 * Evaluate if the player has reached world conquest
 **/
EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalConquerTheWorld(SDK::GPlayer* in_pPlayer,
                                                                    GGameObjective& in_Objective)
{
   if(in_Objective.IsTimedOut())
   {
      return EObjectiveStatus::TimedOut;
   }
   UINT32 l_iCountryId = in_pPlayer->ModID();

   const vector<SP2::GRegionControl>& l_vRegionControl = g_ServerDAL.RegionControlArray();
   bool l_bWorldIsConquered = true;
   for(UINT32 i = 0 ; i < l_vRegionControl.size() ; i++)
   {
      if(l_vRegionControl[i].m_iPolitical != 0 && l_vRegionControl[i].m_iPolitical != l_iCountryId)
      {
         l_bWorldIsConquered = false;
         break;
      }
      if(!l_bWorldIsConquered)
         break;
   }

   if(l_bWorldIsConquered)
      return EObjectiveStatus::Reached;
   else
      return EObjectiveStatus::NotReached;
}

/*!
 * Check if the world peace has been achieved, through 
 * an alliance with every remaining coutries and every remaining coutries
 * must be allied with each others
 **/
EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalAchieveWorldPeace(SDK::GPlayer* in_Player, 
                                                                      GGameObjective& in_Objective)
{
   // timed out ?
   if(in_Objective.IsTimedOut())
   {
      return EObjectiveStatus::TimedOut;
   }
   // is world at peace ?
   else if(m_bWorldPeace)
   {
      // world is soooo peacefull
      return EObjectiveStatus::AlwaysReached;
   }
   else
   {
      // world not at peace :(
      return EObjectiveStatus::NotReached;
   }
}

/*!
 * Check if the the nation is developping in a good way through human developpement level
 **/
EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalDevelopNation(SDK::GPlayer* in_Player, 
                                                                  GGameObjective& in_Objective)
{
   // human developpement high enough ?
   if(g_SP2Server->WorldBehavior().FindHumanDevelopmentFactor(in_Player->ModID()) >= 1.0f)
   {
      // impressive ! will never time out anymore
      return EObjectiveStatus::AlwaysReached;
   }
   // timed out ?   
   else if(in_Objective.IsTimedOut())
   {
      return EObjectiveStatus::TimedOut;
   }
   else
   {
      // not developped enough
      return EObjectiveStatus::NotReached;
   }
}

/*!
 * Evaluate the economic failure game ending condition
 **/
EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalEconomicFailure(SDK::GPlayer* in_pPlayer,GGameObjective& in_Objective)
{
   //That objective has no timeout.

   //Fetch country data
   GCountryData* l_pData = g_ServerDAL.CountryData(in_pPlayer->ModID());
   gassert(l_pData,"GGameObjectiveEvaluator::EvalEconomicFailure invalid country data (NULL)");

   //Economic failure is when your expenses on your debt are higher than your revenues
   REAL64 l_fDebtExpenses = l_pData->BudgetExpenseDebt();
   REAL64 l_fRevenues     = g_ServerDCL.GetRevenuesForEconomicFailure(in_pPlayer->ModID());

   if(l_fDebtExpenses > l_fRevenues)
      return EObjectiveStatus::Reached;
   else
      return EObjectiveStatus::NotReached;
}


/*!
 * Evaluate the : Be Annexed ending condition
 **/
EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalBeAnnexed(SDK::GPlayer* in_pPlayer,GGameObjective& in_Objective)
{
   //That objective cant timeout
   
   //Country without region with political control is game over
   if(g_ServerDAL.CountryPoliticalControl(in_pPlayer->ModID()).empty())
      return EObjectiveStatus::Reached;
   else
      return EObjectiveStatus::NotReached;
}

EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalLoseElections(SDK::GPlayer* in_pPlayer,
                                                                  GGameObjective& in_Objective)
{
   //Objective has no time out

   //Check if the player country is in the list of country that lost the elections
   vector<UINT32>::iterator l_It = find(m_vCountryWherePoliticalPartyChanged.begin(),m_vCountryWherePoliticalPartyChanged.end(),in_pPlayer->ModID());
   if( l_It == m_vCountryWherePoliticalPartyChanged.end())
   {//Not present in the list, did not lose the elections
      return EObjectiveStatus::NotReached;
   }
   else
   {//Present in the list, lost the elections
      m_vCountryWherePoliticalPartyChanged.erase(l_It);
      return EObjectiveStatus::Reached;
   }
}


/******************************************************************************************************************************************/
/* Scenarios Game Objectives
/******************************************************************************************************************************************/
EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalStabilizeCountry(SDK::GPlayer& in_pPlayer,GGameObjective& in_Objective)
{   
   //Get the country data
   SP2::GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_pPlayer.ModID());
   gassert(l_pCountryData,"GGameObjectiveEvaluator::EvalRaiseGDPPerCapita, NULL Pointer exception");

   if(in_Objective.IsTimedOut())
   {
      return EObjectiveStatus::TimedOut;
   }
   else
   {
      //Check if objective reached
      REAL32  l_fStability = l_pCountryData->GvtStability();
      REAL32 l_fInitialStability;
      in_Objective.m_Buffer >> l_fInitialStability;
      REAL32 l_fStabilityValueToReach = l_fInitialStability * (1 + in_Objective.m_fValueToReach);
      if(l_fStability >= l_fStabilityValueToReach)
      {
         return EObjectiveStatus::Reached;
      }
      else
      {
         return EObjectiveStatus::NotReached;
      }
   }
}

EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalMeetRessourcesNeed(SDK::GPlayer& in_pPlayer,GGameObjective& in_Objective)
{
   if(g_SP2Server->WorldBehavior().FindResourceFactor(in_pPlayer.ModID()) >= 1.0f)
   {      
      return EObjectiveStatus::AlwaysReached;
   }
   else if(in_Objective.IsTimedOut())
   {      
      return EObjectiveStatus::TimedOut;
   }               
   else
   {
      return EObjectiveStatus::NotReached;
   }
}

EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalInvasion(SDK::GPlayer& in_Player,GGameObjective& in_Objective)
{
   if(in_Objective.IsTimedOut())
   {
      return EObjectiveStatus::TimedOut;
   }
   else
   {
      UINT32 l_iTotalRegion;
      in_Objective.m_Buffer >> l_iTotalRegion;
      for(UINT i=0;i<l_iTotalRegion;i++)
      {
         UINT32 l_iRegionID;
         in_Objective.m_Buffer >> l_iRegionID;
         GRegionControl l_RegionControl = g_ServerDAL.RegionControl(l_iRegionID);
         if(l_RegionControl.m_iMilitary != in_Player.ModID())
         {
            return EObjectiveStatus::NotReached;
         }
      }
      
      return EObjectiveStatus::Reached;
   }
}

EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalBeInATreaty(SDK::GPlayer& in_Player,GGameObjective& in_Objective)
{
   if(in_Objective.IsTimedOut())
   {
      return EObjectiveStatus::TimedOut;
   }
   else
   {      
      // Get the country that we want to invade
      UINT32   l_iTreatyToBe;
      UINT32   l_iSideToBe;
      in_Objective.m_Buffer >> l_iTreatyToBe;
      in_Objective.m_Buffer >> l_iSideToBe;

      GTreaty* l_pTreaty = g_ServerDAL.Treaty(l_iTreatyToBe);
      gassert(l_pTreaty,"There is no treaty with that value");
      if(l_pTreaty->CountrySide(in_Player.ModID()) == l_iSideToBe)
      {
         return EObjectiveStatus::Reached;
      }
      else
      {
         return EObjectiveStatus::NotReached;
      }
   }
}

EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalRaiseEconomicHealth(SDK::GPlayer& in_Player,GGameObjective& in_Objective)
{
   if(in_Objective.IsTimedOut())
   {
      return EObjectiveStatus::TimedOut;
   }
   else
   {
      //Get the country data
      SP2::GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_Player.ModID());
      gassert(l_pCountryData,"GGameObjectiveEvaluator::EvalRaiseGDPPerCapita, NULL Pointer exception");

      //Check if objective reached
      REAL32 l_fEconomicHealth = l_pCountryData->EconomicHealth();
      REAL32 l_fInitialHealth;
      in_Objective.m_Buffer >> l_fInitialHealth;
      REAL64 l_fEconomicHealthValueToReach = l_fInitialHealth * (1+in_Objective.m_fValueToReach);
      if(l_fEconomicHealth >= l_fEconomicHealthValueToReach)
      {
         return EObjectiveStatus::Reached;
      }
      else
      {
         return EObjectiveStatus::NotReached;
      }
   }
}



EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalRessourceControl(SDK::GPlayer& in_Player,GGameObjective& in_Objective)
{
   if(in_Objective.IsTimedOut())
   {
      return EObjectiveStatus::TimedOut;
   }
   else
   {
      //Get the country data
      SP2::GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_Player.ModID());
      gassert(l_pCountryData,"GGameObjectiveEvaluator::EvalRaiseGDPPerCapita, NULL Pointer exception");

      UINT32   l_iRessourceIdToControl;
      in_Objective.m_Buffer >> l_iRessourceIdToControl;

      REAL64 l_fWorldProduction = g_ServerDAL.MarketProduction((EResources::Enum) l_iRessourceIdToControl);
      REAL64 l_fContryProduciton = l_pCountryData->ResourceProduction((EResources::Enum)l_iRessourceIdToControl);

      if((l_fContryProduciton / l_fWorldProduction ) > in_Objective.m_fValueToReach)
         return EObjectiveStatus::Reached;
      else
         return EObjectiveStatus::NotReached;
   }
}

EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalGovernmentType(SDK::GPlayer& in_Player,GGameObjective& in_Objective)
{
   if(in_Objective.IsTimedOut())
   {
      return EObjectiveStatus::TimedOut;
   }
   else
   {
      //Get the country data
      SP2::GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_Player.ModID());
      gassert(l_pCountryData,"GGameObjectiveEvaluator::EvalRaiseGDPPerCapita, NULL Pointer exception");

      // Get Gouvernement type to acheive
      INT32   l_iGovernmentType;
      in_Objective.m_Buffer >> l_iGovernmentType;
 
      if(l_pCountryData->GvtType() == l_iGovernmentType)
         return EObjectiveStatus::Reached;
      else
         return EObjectiveStatus::NotReached;
   }
}

EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalAnnexCountry(SDK::GPlayer& in_Player,GGameObjective& in_Objective)
{
   if(in_Objective.IsTimedOut())
   {
      return EObjectiveStatus::TimedOut;
   }
   else
   {
      UINT32 l_iTotalRegion;
      in_Objective.m_Buffer >> l_iTotalRegion;
      for(UINT i=0;i<l_iTotalRegion;i++)
      {
         UINT32 l_iRegionID;
         in_Objective.m_Buffer >> l_iRegionID;
         GRegionControl l_RegionControl = g_ServerDAL.RegionControl(l_iRegionID);
         if(l_RegionControl.m_iPolitical != in_Player.ModID())
         {
            return EObjectiveStatus::NotReached;
         }
      }
   
      return EObjectiveStatus::Reached;
   }
}

EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalPureCommunistEonomy(SDK::GPlayer& in_Player,GGameObjective& in_Objective)
{
   if(in_Objective.IsTimedOut())
   {
      return EObjectiveStatus::TimedOut;
   }
   else
   {
      //Get the country data
      SP2::GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_Player.ModID());
      gassert(l_pCountryData,"GGameObjectiveEvaluator::EvalRaiseGDPPerCapita, NULL Pointer exception");

      for(UINT32 i=0;i<EResources::ItemCount;i++)
      {
         if(l_pCountryData->ResourceGvtCtrl((EResources::Enum) i) == false)
            return EObjectiveStatus::NotReached;     
      }
      
      return EObjectiveStatus::Reached;     
   }
}

EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalBuildUnitCategory(SDK::GPlayer& in_Player,GGameObjective& in_Objective)
{
   if(in_Objective.IsTimedOut())
   {
      return EObjectiveStatus::TimedOut;
   }
   else
   {
      UINT32 l_iCategoryToBuild;
      UINT32 l_iQuantity;
      UINT32 l_iTotalUnitBuildSoFar = 0;
      in_Objective.m_Buffer >> l_iCategoryToBuild;
      in_Objective.m_Buffer >> l_iQuantity;

      // Get all the group for this country
      const set<UINT32>& l_Groups = g_Joshua.UnitManager().CountryUnitGroups(in_Player.ModID());
      set<UINT32>::const_iterator l_GroupIterator = l_Groups.begin();
      while(l_GroupIterator != l_Groups.end())
      {
         // Get All unit for this group
         GUnitGroup* l_pUnitGroup = (GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_GroupIterator);
         vector<SDK::Combat::GUnit*>::const_iterator l_UnitIterator = l_pUnitGroup->Units().begin();
         while(l_UnitIterator != l_pUnitGroup->Units().end())
         {
            GUnit* l_pUnit = (GUnit* )*l_UnitIterator;
            SP2::GUnitDesign* l_pUnitDesign =(SP2::GUnitDesign*) l_pUnit->Design();
            if(l_pUnitDesign->Type()->Category() == (INT32)l_iCategoryToBuild)
            {
               l_iTotalUnitBuildSoFar+=l_pUnit->Qty();
            }
            l_UnitIterator++;
         }
         l_GroupIterator++;
      }

      if(l_iTotalUnitBuildSoFar >= l_iQuantity)
         return EObjectiveStatus::AlwaysReached;
      else
         return EObjectiveStatus::NotReached;
   }
}

/*!
 * Check if we have been the targtet of a successful coup d'état
 **/
EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalBeTargetOfSuccessfulCoupEtat(SDK::GPlayer* in_pPlayer,GGameObjective& in_Objective)
{
   //Check if the player country is in the list of country that lost the elections
   vector<UINT32>::iterator l_It = find(m_vCountryTargetOfSuccessfulCoupEtat.begin(),m_vCountryTargetOfSuccessfulCoupEtat.end(),in_pPlayer->ModID());
   if( l_It == m_vCountryTargetOfSuccessfulCoupEtat.end())
   {//Not present in the list, we have not been the target of a successful coup d'état
      return EObjectiveStatus::NotReached;
   }
   else
   {//Present in the list been a target of a successful coup d'etat
      m_vCountryTargetOfSuccessfulCoupEtat.erase(l_It);
      return EObjectiveStatus::Reached;
   }
}

/*!
 * Check if a nuke was successfully launched given the country.
 * The buffer of the game event will contain the target country, if the target country is 0, means successfully reached
 * if you nuke any country.
 **/
EObjectiveStatus::Enum GGameObjectiveEvaluator::EvalLaunchANuke(SDK::GPlayer& in_Player,GGameObjective& in_Objective)
{
   //Fetch the country that must be attacked in order to complete the objective
   UINT32 l_iTargetCountryToSuccess = 0;
   {
      in_Objective.m_Buffer >> l_iTargetCountryToSuccess;
   }

   //Check if a nuke is launched having this country as a target.
   for(UINT32 i = 0 ; i < m_vCountryThatLaunchedNukes.size() ; i++)
   {
      if(m_vCountryThatLaunchedNukes[i].first == (UINT32)in_Player.ModID())
      {
         if( 
             ( l_iTargetCountryToSuccess            == 0                        ) || 
             ( m_vCountryThatLaunchedNukes[i].second == l_iTargetCountryToSuccess)
           )
         {
            //The list will be cleared at the end of the iteration for all the human players
            return EObjectiveStatus::AlwaysReached;
         }
      }
   }
   //The list will be cleared at the end of the iteration for all the human players
   return EObjectiveStatus::NotReached;
}

bool GGameObjectiveEvaluator::OnSave(GIBuffer& io_Buffer)
{
   io_Buffer << m_vCountryWherePoliticalPartyChanged
             << m_vCountryTargetOfSuccessfulCoupEtat
             << m_vCountryThatLaunchedNukes;

   return true;
}

bool GGameObjectiveEvaluator::OnLoad(GOBuffer& io_Buffer)
{
   io_Buffer >> m_vCountryWherePoliticalPartyChanged
             >> m_vCountryTargetOfSuccessfulCoupEtat
             >> m_vCountryThatLaunchedNukes;

   return true;
}

bool GGameObjectiveEvaluator::OnNew(GDatabase* in_pDatabase)
{
   return true;
}

void GGameObjectiveEvaluator::OnClean()
{
   m_vCountryWherePoliticalPartyChanged.clear();
   m_vCountryTargetOfSuccessfulCoupEtat.clear();
   m_vCountryThatLaunchedNukes.clear();
}
