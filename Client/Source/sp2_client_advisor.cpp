/**************************************************************
*
* sp2_client_advisor.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"
#include "../include/sp2_generic_message_window.h"
#include "../include/sp2_generic_question_window.h"
#include "../include/sp2_mail_window.h"
#include "../include/sp2_region_control_change_window.h"


const GString c_sClientAdvisorConfig = L"/data/advisor/client_advisor.xml";


/*!
 * List of existing tags in the client advisor configuration file
 **/
namespace EAdvisorConfigTags
{
   enum Enum
   {
      AdvisorRoot,
      Taxes,
      RaiseTaxDelta,
      RaiseTaxMessage,
      BuildUnits,
      BuildUnitsMessageImpossible,
      BuildUnitsMessageWarning,
      NuclearResearch,
      NuclearResearchAreYouSure,
      NuclearResearchImpossible,
      NuclearResearchWarning,
      AMDSResearch,
      AMDSResearchAreYouSure,
      AMDSResearchImpossible,
      AMDSResearchWarning,
      BuyUsedUnits,
      BuyUsedUnitsMessageImpossible,
      BuyUsedUnitsMessageWarning,
      IncreaseResourceProduction,
      IncreaseResourceProductionMessageImpossible,
      IncreaseResourceProductionMessageWarning,
      TrainCovertActionCell,
      TrainCovertActionCellMessageImpossible,
      TrainCovertActionCellMessageWarning,
      CreateCovertActionCell,
      CreateCovertActionCellMessageImpossible,
      CreateCovertActionCellMessageWarning,
      TradeProposal,
      TradeProposalMessageImpossible,
      TradeProposalMessageWarning,
      ItemCount
   };
};


/*!
 * List of tags values for the client advisor configuration [xml] file
 **/
const GString c_sAdvisorConfigTags[EAdvisorConfigTags::ItemCount] = 
{
   L"Advisor",                    // AdvisorRoot,
   L"Taxes",                      // Taxes,
   L"RaiseDelta",                 // RaiseTaxDelta,                              
   L"MessageRaise",               // RaiseTaxMessage,                            
   L"BuildUnits",                 // Build                                       
   L"MessageImpossible",          // BuildMessageImpossible                      
   L"MessageWarning",             // BuildMessageWarning                         
   L"NuclearResearch",            // NuclearResearch,
   L"MessageAreYouSure",          // NuclearResearchAreYouSure,                     
   L"MessageImpossible",          // NuclearResearchImpossible,                     
   L"MessageWarning",             // NuclearResearchWarning,                        
   L"AMDSResearch",               // AMDSResearch,                               
   L"MessageAreYouSure",          // AMDSResearchAreYouSure,                     
   L"MessageImpossible",          // AMDSResearchImpossible,                     
   L"MessageWarning",             // AMDSResearchWarning,                        
   L"BuyUsedUnits",               // BuyUsedUnits                                
   L"MessageImpossible",          // BuyUsedUnitsMessageImpossible               
   L"MessageWarning",             // BuyUsedUnitsMessageWarning                  
   L"IncreaseResourceProduction", // IncreaseResourceProduction,                 
   L"MessageImpossible",          // IncreaseResourceProductionMessageImpossible,
   L"MessageWarning",             // IncreaseResourceProductionMessageWarning,   
   L"TrainCovertActionCell",      // TrainCovertActionCell,
   L"MessageImpossible",          // TrainCovertActionCellMessageImpossible,
   L"MessageWarning",             // TrainCovertActionCellMessageWarning,
   L"CreateCovertActionCell",     // CreateCovertActionCell,
   L"MessageImpossible",          // CreateCovertActionCellMessageImpossible,
   L"MessageWarning",             // CreateCovertActionCellMessageWarning,
   L"TradeProposal",              // TradeProposal
   L"MessageImpossible",          // TradeProposalMessageImpossible,
   L"MessageWarning",             // TradeProposalMessageWarning,
};                                                                                        


GClientAdvisor::GClientAdvisor()
{
   m_fNextGameTimeCheckForElectionsIn6Months =    0;
   m_fNextGameTimeCheckForElectionsIn1Month  =    0;
   m_fLastMsgTimeGeneratingDeficits          = -1.f;
   m_fLastMsgTimePopulationRestless          = -1.f;
   m_fLastMsgTimeNeedResources               = -1.f;
   m_iNextMessageId                          =    1;
   m_pAdvisorConfigRoot                      = NULL;
}
GClientAdvisor::~GClientAdvisor()
{
   SAFE_DELETE(m_pAdvisorConfigRoot);
}


/*!
 * Initializes the client advisor
 **/
void GClientAdvisor::Initialize()
{
   //Load the client advisor configuration file and parse it
   {
      GFile l_AdvisorConfig;
  
      const bool l_bResult = g_Joshua.FileManager()->File(c_sClientAdvisorConfig,l_AdvisorConfig);
      gassert(l_bResult,"Client Advisor config file must be found");

      //Parse the xml file
      GXMLParser l_Parser;
      m_pAdvisorConfigRoot =  l_Parser.Parse(l_AdvisorConfig);
      gassert(m_pAdvisorConfigRoot,"Error parsing client advisor config file");
   }  

   //Create the hash between the methods that handles the raise event and the 
   //registered events
   {
      INT32 l_iAdjustTaxesEvtType        = GAME_EVENT_TYPE(SP2::Event::GEconomicHealthUpdate);
      INT32 l_iBuildUnitsEvtType         = GAME_EVENT_TYPE(SP2::Event::GBuildUnitOrder);
      INT32 l_iAMDSResearchEvtType       = GAME_EVENT_TYPE(SP2::Event::GMilitaryMainUpdate);
      INT32 l_iBuyUsedUnitsEvtType       = GAME_EVENT_TYPE(SP2::Event::GTransfertUnit);
      INT32 l_iIncreaseResProdEvtType    = GAME_EVENT_TYPE(SP2::Event::GIncreaseProduction);
      INT32 l_iTrainCovActionCellEvtType = GAME_EVENT_TYPE(SP2::Event::GEventCellUpdateStatus);
      INT32 l_iCreateCovActionCellEvtType= GAME_EVENT_TYPE(SP2::Event::GEventCellCreation);
      INT32 l_iTradeProposalEvtTyper     = GAME_EVENT_TYPE(SP2::Event::GEventTradeProposal);


      m_EventIdHandlerMethodMap[l_iAdjustTaxesEvtType ]        = &GClientAdvisor::IsAdjustTaxOk;
      m_EventIdHandlerMethodMap[l_iBuildUnitsEvtType  ]        = &GClientAdvisor::IsBuildUnitsOk;
      m_EventIdHandlerMethodMap[l_iAMDSResearchEvtType]        = &GClientAdvisor::IsBuyAMDSOk;
      m_EventIdHandlerMethodMap[l_iBuyUsedUnitsEvtType ]       = &GClientAdvisor::IsBuyUsedUnitsOk;
      m_EventIdHandlerMethodMap[l_iIncreaseResProdEvtType  ]   = &GClientAdvisor::IsIncreaseResourceProductionOk;
      m_EventIdHandlerMethodMap[l_iTrainCovActionCellEvtType]  = &GClientAdvisor::IsTrainCovertActionCellOk;
      m_EventIdHandlerMethodMap[l_iCreateCovActionCellEvtType] = &GClientAdvisor::IsCreateCovertActionCellOk;
      m_EventIdHandlerMethodMap[l_iTradeProposalEvtTyper]      = &GClientAdvisor::IsTradeProposalOk;
   }


   CleanUp();

}

void GClientAdvisor::CleanUp()
{
   m_iRegionGainedMailID = 0;
   m_iRegionLostMailID = 0;

   m_RegionsGained.clear();
   m_RegionsLost.clear();
}



//! Mail system window handling method
void GClientAdvisor::DisplayAdvisorMsg(void* in_pParam)
{
   //The void param corresponds to the ID of the message in the pending message list
   UINT32 l_iMessageID = (UINT32)in_pParam;


   GClientAdvisor& l_ClientAdvisor = g_ClientDCL.ClientAdvisor();

   //Find the message in the list
   hash_map<UINT32,pair<EClientAdvisorMsg::Enum,GString> >::iterator l_It = l_ClientAdvisor.m_PendingMessages.find(l_iMessageID);

   //make sure the message still exists
   gassert(l_It != l_ClientAdvisor.m_PendingMessages.end(),"GClientAdvisor::DisplayAdvisorMsg Message should be in the list");

   // make sure message isnt empty
   gassert(((GString)l_It->second.second).Trim() != "", "GClientAdvisor::DisplayAdvisorMsg Message shouldn't be empty");

   //Pop a generic message window displaying the message
   g_ClientDDL.GenericMessageWindowSpawn();
   g_ClientDDL.GenericMessageWindow()->Show(l_It->second.second,NULL);

   //Remove the message from the list
   l_ClientAdvisor.m_PendingMessages.erase(l_It);
}

/*!
 * Adds a message to the message queue
 **/
void GClientAdvisor::AddMessage(EClientAdvisorMsg::Enum in_eMsg,
                                  UINT32                  in_iParam1,
                                  UINT32                  in_iParam2,
                                  UINT32                  in_iParam3)
{
   switch(in_eMsg)
   {
      //Handle a war declaration message
      //case EClientAdvisorMsg::WarDeclaration:
      //{
      //   HandleWarDeclaration(in_iParam1,in_iParam2);
      //   break;
      //}
      case EClientAdvisorMsg::TakenMilitaryControl:
      {
         HandleTakenMilitaryControl(in_iParam1,in_iParam2,in_iParam3);
         break;
      }
      case EClientAdvisorMsg::CountryWasNuked:
      {
         HandleCountryWasNuked(in_iParam1);
         break;
      }
      case EClientAdvisorMsg::CountryToBeAnnexed:
      {
         HandleCountryToBeAnnexed(in_iParam1,in_iParam2);
         break;
      }
      case EClientAdvisorMsg::Undefined:
      default:
         gassert(0,"Should be working");

   }
}

/*!
 * Handle war declaration, adds the message to the advisor message queue
 **/
/*
void GClientAdvisor::HandleWarDeclaration(UINT32 in_iCountry1,UINT32 in_iCountry2)
{
   GString l_sGenericMsg;
   //If country 1 is us, ignore the message cause we know that
   if(in_iCountry1 == (UINT32)g_ClientDAL.ControlledCountryID())
   {
      return;
   }
   //If we are the target of the war declaration, message is different
   else if(in_iCountry2 == (UINT32)g_ClientDAL.ControlledCountryID())
   {
      //todo, fetch this from the string table
      l_sGenericMsg = g_ClientDAL.GetString(c_iWarDeclarationOnYou);
      GCountry l_Country;
      l_Country = g_ClientDAL.Country(in_iCountry1);
      l_sGenericMsg+= l_Country.Name();     
   }
   //War declaration between 2 countries, you are not implied
   else
   {
      l_sGenericMsg = g_ClientDAL.GetString(c_iWarDeclarationOther);
      GCountry l_Country1;
      GCountry l_Country2;
      l_Country1 = g_ClientDAL.Country(in_iCountry1);
      l_Country2 = g_ClientDAL.Country(in_iCountry2);
      l_sGenericMsg += l_Country1.Name();
      l_sGenericMsg += "\n  - ";
      l_sGenericMsg += l_Country2.Name();
   }  

   QueueMessage(EClientAdvisorMsg::WarDeclaration,l_sGenericMsg);
}
*/

/*!
 * Handle the case when a region has been take control of
 **/
void GClientAdvisor::HandleTakenMilitaryControl(UINT32 in_iNewOwner, 
                                                UINT32 in_iOldOwner, 
                                                UINT32 in_iRegionNameStrId)
{
   if( (m_iRegionLostMailID != 0) && 
       (!g_ClientDDL.MailWindow()->MailExists(m_iRegionLostMailID) ) )
   {
      m_iRegionGainedMailID = 0;
      m_RegionsGained.clear();
   }

   if( (m_iRegionLostMailID != 0) && 
       (!g_ClientDDL.MailWindow()->MailExists(m_iRegionLostMailID) ) )
   {
      m_iRegionLostMailID = 0;
      m_RegionsLost.clear();
   }

   UINT32 l_iLastGainCount = m_RegionsGained.size();
   UINT32 l_iLastLostCount = m_RegionsLost.size();

   if(in_iNewOwner == (UINT32)g_ClientDAL.ControlledCountryID() )
   {
      // A region was gained, remove from lost list & add to gain list
      m_RegionsGained.insert(in_iRegionNameStrId);
      m_RegionsLost.erase(in_iRegionNameStrId);
   }
   else if(in_iOldOwner == (UINT32)g_ClientDAL.ControlledCountryID() )
   {
      // A region was lost, remove from gain list & add to lost list
      m_RegionsLost.insert(in_iRegionNameStrId);
      m_RegionsGained.erase(in_iRegionNameStrId);
   }

   // Update mails if information changed
   if(l_iLastGainCount != m_RegionsGained.size() )
   {
      if(m_RegionsGained.empty() )
      {
         if(m_iRegionGainedMailID != 0 &&
            g_ClientDDL.MailWindow()->MailExists(m_iRegionGainedMailID) )
         {
            // No more region gained, remove mail associated to gain of region
            g_ClientDDL.MailWindow()->RemoveMail(m_iRegionGainedMailID);
            m_iRegionGainedMailID = 0;
         }
      }
      else
      {
         GString l_sMailSubject = g_ClientDAL.GetString(102311);
         l_sMailSubject = l_sMailSubject.ReplaceNextPattern(GString(m_RegionsGained.size() ), "[number]");

         if(m_iRegionGainedMailID != 0 &&
            g_ClientDDL.MailWindow()->MailExists(m_iRegionGainedMailID) )
         {
            // Update the current mail header to contain a new region
            g_ClientDDL.MailWindow()->ChangeMail(m_iRegionGainedMailID,l_sMailSubject,GClientAdvisor::DisplayRegionGained,(void*)this);
         }
         else
         {
            // No mail sent for the moment or was deleted, create a new one
            m_iRegionGainedMailID = g_ClientDDL.MailWindow()->AddMail(l_sMailSubject,GClientAdvisor::DisplayRegionGained,(void*)this);
         }
      }
   }

   if(l_iLastLostCount != m_RegionsLost.size() )
   {
      if(m_RegionsLost.empty() )
      {
         // No more region lost, remove mail associated to lost of region
         if(m_iRegionLostMailID != 0 &&
            g_ClientDDL.MailWindow()->MailExists(m_iRegionLostMailID) )
         {
            g_ClientDDL.MailWindow()->RemoveMail(m_iRegionLostMailID);
            m_iRegionLostMailID = 0;
         }
      }
      else
      {
         GString l_sMailSubject = g_ClientDAL.GetString(102312);
         l_sMailSubject = l_sMailSubject.ReplaceNextPattern(GString(m_RegionsLost.size() ), "[number]");

         if(m_iRegionLostMailID != 0 &&
            g_ClientDDL.MailWindow()->MailExists(m_iRegionLostMailID) )
         {
            // Update the current mail header to contain a new region
            g_ClientDDL.MailWindow()->ChangeMail(m_iRegionLostMailID, l_sMailSubject,GClientAdvisor::DisplayRegionLost,(void*)this);
         }
         else
         {
            // No mail sent for the moment, create a new one
            m_iRegionLostMailID = g_ClientDDL.MailWindow()->AddMail(l_sMailSubject,GClientAdvisor::DisplayRegionLost,(void*)this);
         }
      }
   }
}


/*!
 * Handles when a country has been nuked, the advisor will say something
 **/
void GClientAdvisor::HandleCountryWasNuked(UINT32 in_iNukedCountryId)
{
   GString l_sGenericMsg = g_ClientDAL.GetString(c_iCountryWasNuked);
   const GCountry& l_Country = g_ClientDAL.Country(in_iNukedCountryId);
   l_sGenericMsg += l_Country.Name();
   QueueMessage(EClientAdvisorMsg::CountryWasNuked,l_sGenericMsg);
}

/*!
 * Handles when a country is about to be annexed, the advisor will say something
 **/
void GClientAdvisor::HandleCountryToBeAnnexed(UINT32 in_iCountryToBeAnnexed,UINT32 in_iAnnexer)
{
   const GCountry& l_CountryToBeAnnexed = g_ClientDAL.Country(in_iCountryToBeAnnexed);
   const GCountry& l_CountryThatAnnexes = g_ClientDAL.Country(in_iAnnexer);
   GString l_sGenericMsg = l_CountryToBeAnnexed.Name() ;
   l_sGenericMsg += g_ClientDAL.GetString(c_iCountryToBeAnnexed);
   l_sGenericMsg += l_CountryThatAnnexes.Name();
   QueueMessage(EClientAdvisorMsg::CountryToBeAnnexed,l_sGenericMsg);
   return;
}


/*!
 * Evaluates the situation with the actual context of the game
 **/
void GClientAdvisor::EvaluateSituation()
{

   //If we received at least one country data synchronisation, perform this, else, do not
   if(g_ClientDAL.m_PlayerCountryData.ReceivedAtLeastOneSync())
   {
      EvaluateGeneratingDeficits();
      EvaluatePopulationRestless();
      EvaluateElectionOutcome();
   }
   return;
}

/*!
 * Evaluate if the Country is generating deficits unsustainable in the long run
 **/
void GClientAdvisor::EvaluateGeneratingDeficits()
{
   SP2::GCountryDataClient& l_CountryData = g_ClientDAL.m_PlayerCountryData;

   //Check if the message popped a couple of minutes ago, if so, dont pop it again
   //so dont even evaluate
   if( (m_fLastMsgTimeGeneratingDeficits >= 0) && 
      ((g_Joshua.Clock() - m_fLastMsgTimeGeneratingDeficits) < c_iMessageFrequencySeconds)
     )
   {
      return;
   }


   //Economic failure is when your expenses on your debt are X% of your revenues
   REAL64 l_fDebtExpenses = l_CountryData.BudgetExpenseDebt();
   REAL64 l_fRevenues     = l_CountryData.BudgetRevenues();
   if(l_fDebtExpenses > (l_fRevenues*c_fEconomicProblemsLongRunRatio))
   {
      //Check if there is already a message in que queue saying Deficits in the long run, 
      //we only want one instance of that in the queue
      hash_map<UINT32,pair<EClientAdvisorMsg::Enum,GString> >::iterator l_It = LocateFirstMessageOfType(EClientAdvisorMsg::DeficitsUnsustainableLongRun);
      if(l_It == m_PendingMessages.end())
      {
         QueueMessage(EClientAdvisorMsg::DeficitsUnsustainableLongRun,g_ClientDAL.GetString(c_iCountryDeficitLongRun));
         m_fLastMsgTimeGeneratingDeficits = g_Joshua.Clock();
      }     
   }
}

/*!
 * Locate the 1st message of the given type in the pending message list, 
 * @returns m_vPendingMessages.end if nothing, else return the iterator to the value
 **/
hash_map<UINT32,pair<EClientAdvisorMsg::Enum,GString> >::iterator GClientAdvisor::LocateFirstMessageOfType(EClientAdvisorMsg::Enum in_eType)
{

   for(hash_map<UINT32,pair<EClientAdvisorMsg::Enum,GString> >::iterator l_It = m_PendingMessages.begin();
       l_It != m_PendingMessages.end();
       l_It++)
   {
      if(l_It->second.first == in_eType)
         return l_It;
   }
   return m_PendingMessages.end();
}


/*!
 * Evaluate if population is getting restless or turning on you
 **/
void GClientAdvisor::EvaluatePopulationRestless()
{
   //Check if the message popped a couple of minutes ago, if so, dont pop it again
   //so dont even evaluate
   if( (m_fLastMsgTimePopulationRestless >= 0) &&
       ((g_Joshua.Clock() - m_fLastMsgTimePopulationRestless) < c_iMessageFrequencySeconds)
     )
   {
      return;
   }


   GString                 l_sMsg;
   EClientAdvisorMsg::Enum l_eMsgType = EClientAdvisorMsg::Undefined;
   SP2::GCountryDataClient& l_CountryData = g_ClientDAL.m_PlayerCountryData;   

   //If population is turning on you
   if(l_CountryData.GvtApproval() <= c_fPopulationTurningOnYouApproval)
   {
      l_sMsg = g_ClientDAL.GetString(c_iPopTurningOnYou);
      l_eMsgType = EClientAdvisorMsg::PopulationTurningOnYou;
   }
   //Population is getting restless
   else if(l_CountryData.GvtApproval() <= c_fPopulationRestlessApproval)
   {
      l_sMsg = g_ClientDAL.GetString(c_iPopGettingRestless);
      l_eMsgType = EClientAdvisorMsg::PopulationGettingRestless;
   }
   
   //If something has happened
   if(l_eMsgType != EClientAdvisorMsg::Undefined)
   {
      if(LocateFirstMessageOfType(l_eMsgType) == m_PendingMessages.end())
      {
         QueueMessage(l_eMsgType,l_sMsg);
         m_fLastMsgTimePopulationRestless = g_Joshua.Clock();
      }
   }
}

/*!
 * Evaluate the outcome of the current elections
 **/
void GClientAdvisor::EvaluateElectionOutcome()
{

   //Only perform that check if we are in a multi-party democracy
   if(g_ClientDAL.m_PlayerCountryData.GvtType() != EGovernmentType::MultiPartyDemocracy)
   {
      return;
   }
   








   bool l_bMustEvaluateElections6Months = false;
   bool l_bMustEvaluateElections1Month  = false;

   SP2::GCountryDataClient& l_CountryData = g_ClientDAL.m_PlayerCountryData;  
   

   //Check if we must evaluate the outcome for the election that is in the next month
   //and set the next check clock
   {
      if(m_fNextGameTimeCheckForElectionsIn1Month <= g_Joshua.GameTime())
      {
         GDateTime l_ActualGameDate = g_ClientDAL.ActualDate();
         GTimeSpan l_Span  = l_CountryData.NextElection() - l_ActualGameDate;
         if(l_Span.Days() <= MAXDAY)
         {
            l_bMustEvaluateElections1Month = true;
            //Next time to evaluate is when the next election is passed
            //(so in at least 1 months - put a little more)
            m_fNextGameTimeCheckForElectionsIn1Month = (g_Joshua.GameTime() + MAXDAY + 2);
         }
         else
         {
            //Do not check again until the next day
            m_fNextGameTimeCheckForElectionsIn1Month = (g_Joshua.GameTime() + 1.f);
         }
      }
   }

   //Check if we must evaluate the outcome for the election in the next 6 months
   //and set the next check clock
   if(!l_bMustEvaluateElections1Month)
   {
      if(m_fNextGameTimeCheckForElectionsIn6Months <= g_Joshua.GameTime())
      {
         GDateTime l_ActualGameDate = g_ClientDAL.ActualDate();
         GTimeSpan l_Span  = l_CountryData.NextElection() - l_ActualGameDate;
         if(l_Span.Days() <= MAXDAY*6)
         {
            l_bMustEvaluateElections6Months = true;
            //Next time to evaluate is when the next election is passed
            //(so in at least 6 months - put a little more)
            m_fNextGameTimeCheckForElectionsIn6Months = (g_Joshua.GameTime() + (6*MAXDAY + 5));
         }
         else
         {
            //Do not check again until the next day
            m_fNextGameTimeCheckForElectionsIn6Months = (g_Joshua.GameTime() + 1.f);
         }
      }
   }

   gassert(!l_bMustEvaluateElections1Month || !l_bMustEvaluateElections6Months,"GClientAdvisor::EvaluateElectionOutcome, illogic pattern, evaluate elections that happens in 6 months, so election is not in less than 1 month (and vice versa)");
   //If must evaluate election outcome 
   if(l_bMustEvaluateElections6Months || l_bMustEvaluateElections1Month)
   {
      GString                 l_sGenericMsg;
      EClientAdvisorMsg::Enum l_eMsgType = EClientAdvisorMsg::Undefined;
      bool l_bOutcomeFavorable = true;
      //Check if the outcome appears favorable or not
      if(l_CountryData.GvtApproval() < c_fNonFavorableElectionOutcomeVal)
         l_bOutcomeFavorable = false; //not favorable
      else
         l_bOutcomeFavorable = true;  //favorable

      //Prepare the variables to create the message
      {
         if(l_bMustEvaluateElections6Months)
         {
            l_eMsgType = EClientAdvisorMsg::ElectionIn6MonthsOutcome;
            if(l_bOutcomeFavorable)
               l_sGenericMsg = g_ClientDAL.GetString(c_iElectionIn6Favorable);
            else
               l_sGenericMsg = g_ClientDAL.GetString(c_iElectionIn6NotFavorable);
         }
         else
         {
            gassert(l_bMustEvaluateElections1Month,"GClientAdvisor::EvaluateElectionOutcome Incoherence");
            l_eMsgType = EClientAdvisorMsg::ElectionIn1MonthOutcome;
            if(l_bOutcomeFavorable)
               l_sGenericMsg = g_ClientDAL.GetString(c_iElectionIn1Favorable);
            else
               l_sGenericMsg = g_ClientDAL.GetString(c_iElectionIn1NotFavorable);
         }
      }
      gassert(l_eMsgType != EClientAdvisorMsg::Undefined,"GClientAdvisor::EvaluateElectionOutcome, msgtype should be defined");
      QueueMessage(l_eMsgType,l_sGenericMsg);
   }
   return;
}

/*!
 * Evaluate Resources Needs
 **/
void GClientAdvisor::EvaluateResourceNeeds()
{
   //Check if the message popped a couple of minutes ago, if so, dont pop it again
   //so dont even evaluate
   if( (m_fLastMsgTimeNeedResources >=  0) &&
       ((g_Joshua.Clock() - m_fLastMsgTimeNeedResources) < c_iMessageFrequencySeconds)
     )
   {
      return;
   }

   //Check if there is not already an entry that says not enough resources
   if(LocateFirstMessageOfType(EClientAdvisorMsg::ResourceNeed) == m_PendingMessages.end())
   {
      vector<EResources::Enum> l_vResourcesWeNeed;
      SP2::GCountryDataClient l_CountryData = g_ClientDAL.m_PlayerCountryData;
      for(UINT32 i = 0 ; i < EResources::ItemCount ; i++)
      {
         REAL64 l_fResourceBalance = l_CountryData.ResourceBalance((EResources::Enum) i);
         REAL64 l_fResourceDemand  = l_CountryData.ResourceDemand ((EResources::Enum)i);

         if(l_fResourceBalance < 0)
         {
            REAL64 l_fRatio = (-l_fResourceBalance)/l_fResourceDemand;
            if(l_fRatio >= c_fResourceNeedVsDemandRatioIsBad)
            {
               l_vResourcesWeNeed.push_back((EResources::Enum)i);
            }
         }     
      }

      //Resource we need
      if(!l_vResourcesWeNeed.empty())
      {
         m_fLastMsgTimeNeedResources = g_Joshua.Clock();
         GString l_sGenericMsg = g_ClientDAL.GetString(c_iCountryBadlyNeedResource);
         for(UINT32 i = 0; i < l_vResourcesWeNeed.size() ; i++)
         {
            l_sGenericMsg += "\n  - ";
            l_sGenericMsg += g_ClientDAL.GetString(g_ClientDAL.StringIdResource(l_vResourcesWeNeed[i]));
         }
         QueueMessage(EClientAdvisorMsg::ResourceNeed,l_sGenericMsg);
      }
   }
}


/*!
 * Queues a message in the pending message list
 **/
void GClientAdvisor::QueueMessage(EClientAdvisorMsg::Enum in_eMsgType,const GString& in_sMsg)
{
   //Make sure everything is valid
   {
      gassert(in_eMsgType >= 0 && in_eMsgType < EClientAdvisorMsg::ItemCount,"GClientAdvisor::QueueMessage Invalid message type");
      gassert(g_ClientDDL.MailWindow(),"GClientAdvisor::QueueMessage Mail window should exist");
   }
 
   //Get a new id for the message
   UINT32 l_iMsgId = m_iNextMessageId++;
   m_PendingMessages[l_iMsgId] = make_pair(in_eMsgType,in_sMsg);

   //Fetch the mail header based on the msg type
   GString l_sMailHeader = g_ClientDAL.GetString(c_iMailSystemAdvisorWarningHeader[c_ClientAdvisorMessageClassification[in_eMsgType]]);

   //Notify the mail system there is a new advisor message
   g_ClientDDL.MailWindow()->AddMail(l_sMailHeader,GClientAdvisor::DisplayAdvisorMsg,(void*)l_iMsgId);
}



/*!
 * Raise a game event and have the client advisor evaluate its potential consequences (advisor might say something about it)
 **/
void GClientAdvisor::RaiseEvent(GGameEventSPtr in_Evt)
{
   hash_map<INT32,ClientAdvisorRaiseEventHandler>::const_iterator l_It = m_EventIdHandlerMethodMap.find(in_Evt->Type());

   //Event was not found in the map
   if(l_It == m_EventIdHandlerMethodMap.end())
   {
      SendEventToServer(in_Evt);
      return;
   }
   else
   {
      //Call the handling method
      GString  l_sMessage;
      EClientAdvisorIsActionOkResult::Enum l_eResult = (this->*l_It->second)(in_Evt,l_sMessage);

      //Depending on the result, perform the correct action
      switch(l_eResult)   
      {
         //Everything is fine, raise the event
         case EClientAdvisorIsActionOkResult::Ok:
         {
            SendEventToServer(in_Evt);
            break;
         }
         //Need a confirmation before sending the event
         case EClientAdvisorIsActionOkResult::NeedConfirmation:
         {
            //Generate a unique id for the game event with the already present data
            //and insert it in the pending game events that needs an answer
            INT32 l_iUniqueID = 0;
            {
					l_iUniqueID = in_Evt->Type()+in_Evt->m_iTarget+(INT32)(g_Joshua.TimeCounter().GetPrecisionTime()*1000000.f);
               gassert(m_EventsWaitingForConfirmation.find(l_iUniqueID) == m_EventsWaitingForConfirmation.end(),"Invalid unique id generated [already exists], check the id generation method");
               m_EventsWaitingForConfirmation.insert(make_pair(l_iUniqueID,in_Evt));
            }


            GGenericQuestion l_Question;
            l_Question.m_sMessage         = l_sMessage;
            l_Question.m_pOberserToNotify = this;
            l_Question.m_pOnOkValue       = (void*)l_iUniqueID;
            l_Question.m_pOnCancelValue   = (void*)l_iUniqueID;

            //Pop the question
            g_ClientDDL.GenericQuestion(l_Question);
            break;
         }
         //Action is impossible, would make the player game over
         case EClientAdvisorIsActionOkResult::Impossible:
         {

            //Pop a generic message window displaying the message
            g_ClientDDL.GenericMessageWindowSpawn();
            g_ClientDDL.GenericMessageWindow()->Show(l_sMessage,NULL);

            CancelEvent(in_Evt);
            break;
         }
         default:
            gassert(0,"Unhandled case...");
            break;
      }
   }
   return;
}

/*!
 * Tells if the tax adjustment is ok, if ok, send the event right away, else, pop the msg that was filled
 **/
EClientAdvisorIsActionOkResult::Enum GClientAdvisor::IsAdjustTaxOk(GGameEventSPtr in_Event,GString& out_sMsg)
{
   CXML_Util l_Util;
   //Fetch the Taxes node root
   const CTreeXMLNode* l_pTaxesRoot = NULL;
   {
      l_pTaxesRoot = l_Util.Search_Const_Node(m_pAdvisorConfigRoot->Root(),c_sAdvisorConfigTags[EAdvisorConfigTags::Taxes],L"",NULL,2);
      gassert(l_pTaxesRoot,"Taxes root shouldnt be null");
   }

   //Accepted delta value (fetch from config file)
   UINT32 l_iDelta;
   {
      const CTreeXMLNode* l_pDeltaNode = l_Util.Search_Const_Node(l_pTaxesRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::RaiseTaxDelta],L"",NULL,1);
      gassert(l_pDeltaNode,"Tax delta node shouldnt be null");
      l_iDelta = l_pDeltaNode->Data().m_value.ToINT32();
   }


   REAL64 l_fActualValue = g_ClientDAL.m_PlayerCountryData.PersonalIncomeTax()*100;
   //Fetch the new tax value (wanted)
   REAL64 l_fNewValueWanted = 100;
   {
      //Fetch the pointer to the correct type of event
      SP2::Event::GEconomicHealthUpdate* l_pEconomicHealthUpdate = (SP2::Event::GEconomicHealthUpdate*)in_Event.get();
      l_fNewValueWanted *= l_pEconomicHealthUpdate->m_fIncomeTax;
   }

   //If Raising the taxes too much
   if( (l_fNewValueWanted - l_fActualValue) >= (REAL64)l_iDelta)
   {
      //Fetch the warning message and issue the warning
      const CTreeXMLNode* l_pMsgNode = l_Util.Search_Const_Node(l_pTaxesRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::RaiseTaxMessage],L"",NULL,1);
      gassert(l_pMsgNode,"A Message node should be present");
      out_sMsg = g_ClientDAL.GetString(l_pMsgNode->Data().m_value.ToINT32());
      
      return EClientAdvisorIsActionOkResult::NeedConfirmation;
   }
   //Not raising too much
   else
   {
      //No warning
      return EClientAdvisorIsActionOkResult::Ok;
   }
}


/*!
 * Tells if the unit build order is ok, if ok, returns true, if not, returns false and fills the msg param
 **/
EClientAdvisorIsActionOkResult::Enum GClientAdvisor::IsBuildUnitsOk(GGameEventSPtr in_Event,GString& out_sMsg)
{
   //Check how much units we want to build
   SP2::Event::GBuildUnitOrder* l_pBuildUnitsEvt = (SP2::Event::GBuildUnitOrder*)in_Event.get();
   SP2::GUnitDesign* l_pDesign                   = (SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign(l_pBuildUnitsEvt->m_iDesignID);

   //Compute the cost for such an order
   REAL64 l_fOrderCost = l_pDesign->Cost()*l_pBuildUnitsEvt->m_iQuantity * g_ClientDAL.ProductionSpeedCostModificator(l_pBuildUnitsEvt->m_iProductionSpeed);


   //Check if we can afford the expense
   EClientAdvisorIsActionOkResult::Enum l_eCanAffordExpense = CanAffordExpense(l_fOrderCost);

   if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::Ok)
   {
      //dont do anything, the l_eCanAffordExpense value will be returned and everything will be ok
   }
   else
   {
      //Fetch the root node
      CXML_Util l_Util;
      const CTreeXMLNode* l_pBuildRoot = NULL;
      {
         l_pBuildRoot = l_Util.Search_Const_Node(m_pAdvisorConfigRoot->Root(),c_sAdvisorConfigTags[EAdvisorConfigTags::BuildUnits],L"",NULL,2);
         gassert(l_pBuildRoot,"Root shouldnt be null");
      }   

      //Fetch the message node
      const CTreeXMLNode* l_pMessageNode = NULL;
      {
         if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::Impossible)
         {
            l_pMessageNode = l_Util.Search_Const_Node(l_pBuildRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::BuildUnitsMessageImpossible],L"",NULL,2);
         }
         else if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::NeedConfirmation)
         {
            l_pMessageNode = l_Util.Search_Const_Node(l_pBuildRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::BuildUnitsMessageWarning],L"",NULL,2);
         }
         gassert(l_pMessageNode,"A Message node should be present, unhandled case ???");
      }
      //Get the msg
      out_sMsg      = g_ClientDAL.GetString(l_pMessageNode->Data().m_value.ToINT32());
   }

   return l_eCanAffordExpense;
}


/*!
 * Observer on subject change, used for the generic message window notification
 **/
void GClientAdvisor::OnSubjectChange(Hector::DesignPattern::GSubject& in_Subject,
                                     const Hector::DesignPattern::GSubjectNotification&      in_Notification,
                                     void* in_pAdditionalMsg)
{

   //Find the event with the given unique id, (ok value == cancel value)
   SP2::GGenericQuestionWindow* l_pQuestionWindow = (GGenericQuestionWindow*)in_pAdditionalMsg;
   gassert(l_pQuestionWindow->OkValue() == l_pQuestionWindow->CancelValue(),"Both value are setted the same, so should be the same");

   INT32 l_iUniqueID       = (INT32)l_pQuestionWindow->OkValue();
   hash_map<INT32,GGameEventSPtr>::iterator  l_It = m_EventsWaitingForConfirmation.find(l_iUniqueID);
   gassert(l_It != m_EventsWaitingForConfirmation.end(),"Confirmed event should be in the events waiting for confirmation list");

   switch(in_Notification)
   {
      //Raise the event
      case c_OnGenericQuestionWindowAnswerOk:
         SendEventToServer(l_It->second);
         break;
      case c_OnGenericQuestionWindowAnswerCancel:
         // Cancel request event if appropriate
         CancelEvent(l_It->second);
         break;
      default:
         gassert(0,"Unhandled case");
         break;

   }

   //Remove the event from the list of events waiting for confirmation
   m_EventsWaitingForConfirmation.erase(l_It);

   return;
}


/*!
 * Tells if starting the amds research is ok, if ok, returns true, if not, returns false and fills the msg param
 **/
EClientAdvisorIsActionOkResult::Enum GClientAdvisor::IsBuyAMDSOk(GGameEventSPtr in_Event,GString& out_sMsg)
{
   SP2::Event::GMilitaryMainUpdate* l_pEvt = (SP2::Event::GMilitaryMainUpdate*)in_Event.get();
   //If the event is about starting the nuclear Research
   if(l_pEvt->StartNuclearResearch)
   {
      //Check if we can afford the expense
      EClientAdvisorIsActionOkResult::Enum l_eCanAffordExpense = CanAffordExpense(c_fNuclearResearchCost);

      //Fetch the root node
      CXML_Util l_Util;
      const CTreeXMLNode* l_pNuclearResearchRoot = NULL;
      {
         l_pNuclearResearchRoot = l_Util.Search_Const_Node(m_pAdvisorConfigRoot->Root(),c_sAdvisorConfigTags[EAdvisorConfigTags::NuclearResearch],L"",NULL,2);
         gassert(l_pNuclearResearchRoot,"Root shouldnt be null");
      }

      //Fetch the message node
      const CTreeXMLNode* l_pMessageNode = NULL;
      {
         //We cant afford it
         if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::Impossible)
         {
            l_pMessageNode = l_Util.Search_Const_Node(l_pNuclearResearchRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::NuclearResearchImpossible],L"",NULL,2);
         }
         //Request a confirmation, cause it will be hard economically after
         else if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::NeedConfirmation)
         {
            l_pMessageNode = l_Util.Search_Const_Node(l_pNuclearResearchRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::NuclearResearchWarning],L"",NULL,2);
         }
         //If we can afford it, still request a confirmation
         else if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::Ok)
         {
            l_pMessageNode = l_Util.Search_Const_Node(l_pNuclearResearchRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::NuclearResearchAreYouSure],L"",NULL,2);
            l_eCanAffordExpense = EClientAdvisorIsActionOkResult::NeedConfirmation;
         }

         gassert(l_pMessageNode,"A Message node should be present, unhandled case ???");
      }
      //Get the message
      GString l_sNuclearResearchCost = GString::FormatNumber(c_fNuclearResearchCost,L" ",L".",L"",L" $",3,0);
      out_sMsg                       = g_ClientDAL.GetString(l_pMessageNode->Data().m_value.ToINT32()) + l_sNuclearResearchCost;   
      out_sMsg							    = out_sMsg.ReplaceNextPattern(GString::FormatNumber(c_fNuclearResearchTime,0), SP2::NUMBER_PATTERN);     

      return l_eCanAffordExpense;
   }
   //If the event is about starting the AMDS Research
   else if(l_pEvt->StartAmdsResearch)
   {
      //AMDS ResearchCost
      REAL64 l_fAMDSResearchCost = g_ClientDAL.AMDSResearchCost();


      //Check if we can afford the expense
      EClientAdvisorIsActionOkResult::Enum l_eCanAffordExpense = CanAffordExpense(l_fAMDSResearchCost);


      //Fetch the root node
      CXML_Util l_Util;
      const CTreeXMLNode* l_pAMDSResearchRoot = NULL;
      {
         l_pAMDSResearchRoot = l_Util.Search_Const_Node(m_pAdvisorConfigRoot->Root(),c_sAdvisorConfigTags[EAdvisorConfigTags::AMDSResearch],L"",NULL,2);
         gassert(l_pAMDSResearchRoot,"Root shouldnt be null");
      }   

      //Fetch the message node
      const CTreeXMLNode* l_pMessageNode = NULL;
      {
         //We cant afford it
         if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::Impossible)
         {
            l_pMessageNode = l_Util.Search_Const_Node(l_pAMDSResearchRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::AMDSResearchImpossible],L"",NULL,2);
         }
         //Request a confirmation, cause it will be hard economically after
         else if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::NeedConfirmation)
         {
            l_pMessageNode = l_Util.Search_Const_Node(l_pAMDSResearchRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::AMDSResearchWarning],L"",NULL,2);
         }
         //If we can afford it, still request a confirmation
         else if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::Ok)
         {
            l_pMessageNode = l_Util.Search_Const_Node(l_pAMDSResearchRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::AMDSResearchAreYouSure],L"",NULL,2);
            l_eCanAffordExpense = EClientAdvisorIsActionOkResult::NeedConfirmation;
         }

         gassert(l_pMessageNode,"A Message node should be present, unhandled case ???");
      }
      //Get the message
      GString l_sAMDSResearchCost = GString::FormatNumber(l_fAMDSResearchCost,L" ",L".",L"",L" $",3,0);
      out_sMsg                    = g_ClientDAL.GetString(l_pMessageNode->Data().m_value.ToINT32()) + l_sAMDSResearchCost; 
		out_sMsg							 = out_sMsg.ReplaceNextPattern(GString::FormatNumber(g_ClientDAL.AMDSResearchTimeDays(),0)
																					 ,SP2::NUMBER_PATTERN);
		
      return l_eCanAffordExpense;
   }

   //If the event is not about starting the AMDS Research, let it through
   return EClientAdvisorIsActionOkResult::Ok;
}


/*!
 * Method that analyses the economical situation of the player and tells if the player can afford the expense.
 * It will return a value of EClientAdvisorIsActionOkResult to tell the result of the analysis.
 *   Return Values and meanings:    
 *                   Ok                -> Can afford without a problem
 *                   NeedConfirmation  -> Can afford but comport some risks
 *                   Impossible        -> Can't afford
 * @return see method description
 **/
EClientAdvisorIsActionOkResult::Enum GClientAdvisor::CanAffordExpense(REAL64 in_fExpense) const
{
   SP2::GCountryDataClient& l_CountryData = g_ClientDAL.m_PlayerCountryData;



   //Actual Supply amount (money)
   REAL64 l_fSimulatedSupplyAmount = l_CountryData.MonetarySupplyAmount();
   
   //Supply amount after having performed the expense
   l_fSimulatedSupplyAmount        -= in_fExpense;


   //If we still are in the green (enough money in the bank)
   if(l_fSimulatedSupplyAmount >= 0)
   {
      return EClientAdvisorIsActionOkResult::Ok;
   }
   else
   {
      //We have a debt, check if the interests on the debt are too high
      REAL64 l_fSimulatedExpenseOnTheDebt = -l_fSimulatedSupplyAmount*DEBT_INTEREST_RATE;

      REAL64 l_fRevenues     = l_CountryData.BudgetRevenues();

      REAL64 l_fDangerousDebtValue  = l_fRevenues*c_fEconomicProblemsLongRunRatio;
      REAL64 l_fImpossibleDebtValue = l_fRevenues*c_fEconomicAlmostImpossibleRatio;

      //We have plenty of money 
      if(l_fSimulatedExpenseOnTheDebt < l_fDangerousDebtValue)
      {
         return EClientAdvisorIsActionOkResult::Ok;
      }
      //Is it border line
      else if(l_fSimulatedExpenseOnTheDebt >= l_fDangerousDebtValue && l_fSimulatedExpenseOnTheDebt < l_fImpossibleDebtValue)
      {
         return EClientAdvisorIsActionOkResult::NeedConfirmation;
      }
      //Impossible
      else if(l_fSimulatedExpenseOnTheDebt >= l_fImpossibleDebtValue)
      {
         return EClientAdvisorIsActionOkResult::Impossible;
      }
      else
      {
         gassert(0,"Unhandled case");
         return EClientAdvisorIsActionOkResult::Impossible;
      }
   }
}


/*!
 * Tells if buying used units is ok, if ok, returns true, if not, returns false and fills the msg param
 **/
EClientAdvisorIsActionOkResult::Enum GClientAdvisor::IsBuyUsedUnitsOk(GGameEventSPtr in_Event,GString& out_sMsg)
{
   //Check how much units we want to BuyUsed
   SP2::Event::GTransfertUnit* l_pBuyUsedUnitsEvt = (SP2::Event::GTransfertUnit*)in_Event.get();
   gassert(l_pBuyUsedUnitsEvt->m_bForSale == false,"For Sale should be at false");
   SP2::GUnit*       l_pUnit                      = (SP2::GUnit*)g_Joshua.UnitManager().Unit(l_pBuyUsedUnitsEvt->m_iUnitId);
   SP2::GUnitDesign* l_pDesign                    = (SP2::GUnitDesign*)l_pUnit->Design();

   //Compute the cost for such an order
   REAL64 l_fCost = l_pDesign->Cost()*l_pBuyUsedUnitsEvt->m_iQty;

   //Check if we can afford the expense
   EClientAdvisorIsActionOkResult::Enum l_eCanAffordExpense = CanAffordExpense(l_fCost);

   if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::Ok)
   {
      //dont do anything, the l_eCanAffordExpense value will be returned and everything will be ok
   }
   else
   {
      //Fetch the root node
      CXML_Util l_Util;
      const CTreeXMLNode* l_pBuyUsedRoot = NULL;
      {
         l_pBuyUsedRoot = l_Util.Search_Const_Node(m_pAdvisorConfigRoot->Root(),c_sAdvisorConfigTags[EAdvisorConfigTags::BuyUsedUnits],L"",NULL,2);
         gassert(l_pBuyUsedRoot,"Root shouldnt be null");
      }   

      //Fetch the message node
      const CTreeXMLNode* l_pMessageNode = NULL;
      {
         if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::Impossible)
         {
            l_pMessageNode = l_Util.Search_Const_Node(l_pBuyUsedRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::BuyUsedUnitsMessageImpossible],L"",NULL,2);
         }
         else if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::NeedConfirmation)
         {
            l_pMessageNode = l_Util.Search_Const_Node(l_pBuyUsedRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::BuyUsedUnitsMessageWarning],L"",NULL,2);
         }
         gassert(l_pMessageNode,"A Message node should be present, unhandled case ???");
      }
      //Get the msg
      out_sMsg      = g_ClientDAL.GetString(l_pMessageNode->Data().m_value.ToINT32());   
   }
   return l_eCanAffordExpense;
}

/*!
 * Tells if increasing the resource production is ok, if ok, returns true, if not, returns false and fills the msg param
 **/
EClientAdvisorIsActionOkResult::Enum GClientAdvisor::IsIncreaseResourceProductionOk(GGameEventSPtr in_Event,GString& out_sMsg)
{
   SP2::Event::GIncreaseProduction* l_pEvtIncreaseProd = (SP2::Event::GIncreaseProduction*)in_Event.get();

   //Fetch the cost of increasing the production of the resource
   REAL64 l_fCost = 0;
   {
      SP2::GCountryDataClient& l_CountryData = g_ClientDAL.m_PlayerCountryData;
      l_fCost = g_ClientDCL.IncreaseProductionApproxCost(l_pEvtIncreaseProd->m_fIncrease,l_CountryData.ResourceProduction((EResources::Enum)l_pEvtIncreaseProd->m_iResourceID) );
   }

   //Check if we can afford the expense
   EClientAdvisorIsActionOkResult::Enum l_eCanAffordExpense = CanAffordExpense(l_fCost);

   if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::Ok)
   {
      //dont do anything, the l_eCanAffordExpense value will be returned and everything will be ok
   }
   else
   {
      //Fetch the root node
      CXML_Util l_Util;
      const CTreeXMLNode* l_pIncreaseResourceProductionRoot = NULL;
      {
         l_pIncreaseResourceProductionRoot = l_Util.Search_Const_Node(m_pAdvisorConfigRoot->Root(),c_sAdvisorConfigTags[EAdvisorConfigTags::IncreaseResourceProduction],L"",NULL,2);
         gassert(l_pIncreaseResourceProductionRoot,"Root shouldnt be null");
      }   

      //Fetch the message node
      const CTreeXMLNode* l_pMessageNode = NULL;
      {
         if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::Impossible)
         {
            l_pMessageNode = l_Util.Search_Const_Node(l_pIncreaseResourceProductionRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::IncreaseResourceProductionMessageImpossible],L"",NULL,2);
         }
         else if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::NeedConfirmation)
         {
            l_pMessageNode = l_Util.Search_Const_Node(l_pIncreaseResourceProductionRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::IncreaseResourceProductionMessageWarning],L"",NULL,2);
         }
         gassert(l_pMessageNode,"A Message node should be present, unhandled case ???");
      }
      //Get the msg
      out_sMsg      = g_ClientDAL.GetString(l_pMessageNode->Data().m_value.ToINT32());
   }
   return l_eCanAffordExpense;
}

/*!
 * Tells if updating a covert action cell status is ok, if ok, returns true, if not, returns false and fills the msg param
 **/
EClientAdvisorIsActionOkResult::Enum GClientAdvisor::IsTrainCovertActionCellOk(GGameEventSPtr in_Event,GString& out_sMsg)
{
   SP2::Event::GEventCellUpdateStatus* l_pEvtUpdateCell = (SP2::Event::GEventCellUpdateStatus*)in_Event.get();
   if(!l_pEvtUpdateCell->m_bTrain)
      return EClientAdvisorIsActionOkResult::Ok;
   else
   {
      //Fetch the cost of increasing the production of the resource
      REAL64 l_fCost = 0;
      {
         SP2::GCountryDataClient& l_CountryData = g_ClientDAL.m_PlayerCountryData;
         //Fetch the cell
         const GCovertActionCell& l_Cell = l_CountryData.CovertActionCell(l_pEvtUpdateCell->m_iCellID);
         ECovertActionCellTraining::Enum l_eNextTraining  = l_Cell.ExperienceLevelType();
         switch(l_eNextTraining)
         {
         case ECovertActionCellTraining::Recruit:
            l_fCost = SP2::c_fCellTrainingCostRegular;
            break;
         case ECovertActionCellTraining::Regular:
            l_fCost = SP2::c_fCellTrainingCostVeteran;
            break;
         case ECovertActionCellTraining::Veteran:
            l_fCost = SP2::c_fCellTrainingCostElite;
            break;
         default:
            gassert(0,"Unhandled case in the client advisor train cell checker");
            return EClientAdvisorIsActionOkResult::Impossible;
         }
      }

      //Check if we can afford the expense
      EClientAdvisorIsActionOkResult::Enum l_eCanAffordExpense = CanAffordExpense(l_fCost);

      if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::Ok)
      {
         //dont do anything, the l_eCanAffordExpense value will be returned and everything will be ok
      }
      else
      {
         //Fetch the root node
         CXML_Util l_Util;
         const CTreeXMLNode* l_pTrainCellRoot = NULL;
         {
            l_pTrainCellRoot = l_Util.Search_Const_Node(m_pAdvisorConfigRoot->Root(),c_sAdvisorConfigTags[EAdvisorConfigTags::TrainCovertActionCell],L"",NULL,2);
            gassert(l_pTrainCellRoot,"Root shouldnt be null");
         }   

         //Fetch the message node
         const CTreeXMLNode* l_pMessageNode = NULL;
         {
            if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::Impossible)
            {
               l_pMessageNode = l_Util.Search_Const_Node(l_pTrainCellRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::TrainCovertActionCellMessageImpossible],L"",NULL,2);
            }
            else if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::NeedConfirmation)
            {
               l_pMessageNode = l_Util.Search_Const_Node(l_pTrainCellRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::TrainCovertActionCellMessageWarning],L"",NULL,2);
            }
            gassert(l_pMessageNode,"A Message node should be present, unhandled case ???");
         }
         //Get the msg
         out_sMsg      = g_ClientDAL.GetString(l_pMessageNode->Data().m_value.ToINT32());   
      }
      return l_eCanAffordExpense;   
   }
}

/*!
 * Tells if creating a covert action cell status is ok, if ok, returns true, if not, returns false and fills the msg param
 **/
EClientAdvisorIsActionOkResult::Enum GClientAdvisor::IsCreateCovertActionCellOk(GGameEventSPtr in_Event,GString& out_sMsg)
{
   SP2::Event::GEventCellCreation* l_pEvtCreateCell = (SP2::Event::GEventCellCreation*)in_Event.get();
   //Fetch the cost of increasing the production of the resource
   REAL64 l_fCost = GDALInterface::CovertActionCellSetupCost(l_pEvtCreateCell->m_eTraining);

   //Check if we can afford the expense
   EClientAdvisorIsActionOkResult::Enum l_eCanAffordExpense = CanAffordExpense(l_fCost);

   if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::Ok)
   {
      //dont do anything, the l_eCanAffordExpense value will be returned and everything will be ok
   }
   else
   {
      //Fetch the root node
      CXML_Util l_Util;
      const CTreeXMLNode* l_pCreateCellRoot = NULL;
      {
         l_pCreateCellRoot = l_Util.Search_Const_Node(m_pAdvisorConfigRoot->Root(),c_sAdvisorConfigTags[EAdvisorConfigTags::CreateCovertActionCell],L"",NULL,2);
         gassert(l_pCreateCellRoot,"Root shouldnt be null");
      }   

      //Fetch the message node
      const CTreeXMLNode* l_pMessageNode = NULL;
      {
         if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::Impossible)
         {
            l_pMessageNode = l_Util.Search_Const_Node(l_pCreateCellRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::CreateCovertActionCellMessageImpossible],L"",NULL,2);
         }
         else if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::NeedConfirmation)
         {
            l_pMessageNode = l_Util.Search_Const_Node(l_pCreateCellRoot,c_sAdvisorConfigTags[EAdvisorConfigTags::CreateCovertActionCellMessageWarning],L"",NULL,2);
         }
         gassert(l_pMessageNode,"A Message node should be present, unhandled case ???");
      }
      //Get the msg
      out_sMsg      = g_ClientDAL.GetString(l_pMessageNode->Data().m_value.ToINT32())   ;
   }
   return l_eCanAffordExpense;   
}

/*!
 * Tells if proposing or accepting a trade is ok, if ok, returns true, 
 * if not, returns false and fills the msg param
 **/
EClientAdvisorIsActionOkResult::Enum GClientAdvisor::IsTradeProposalOk(GGameEventSPtr in_Event, GString& out_sMsg)
{
   // retrieve the trade proposal data
   SP2::Event::GEventTradeProposal* l_pEvtTradeProposal = (SP2::Event::GEventTradeProposal*)in_Event.get();
   // retrieve which country we are in the trade
   bool l_bCountryInSideA = (l_pEvtTradeProposal->m_trdData.GetTradeDataFromSide(true).CountryId() == g_ClientDAL.ControlledCountryID());
   // retrieve our money expense for the trade
   REAL64 l_fTradeExpense = l_pEvtTradeProposal->m_trdData.GetTradeDataFromSide(l_bCountryInSideA).Money() - 
                            l_pEvtTradeProposal->m_trdData.GetTradeDataFromSide(!l_bCountryInSideA).Money();
   // Check if we can afford the expense
   EClientAdvisorIsActionOkResult::Enum l_eCanAffordExpense = CanAffordExpense(l_fTradeExpense);
   // if we cant afford and we were proposing or accepting, take actions
   if((l_eCanAffordExpense != EClientAdvisorIsActionOkResult::Ok) && 
      (l_pEvtTradeProposal->m_trdData.Status() != ETradeStatus::Refused))
   {
      //Fetch the message
      if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::Impossible)
      {
         out_sMsg = g_ClientDAL.GetString(c_iStrIdTradeImpossible);
      }
      else if(l_eCanAffordExpense == EClientAdvisorIsActionOkResult::NeedConfirmation)
      {
         out_sMsg = g_ClientDAL.GetString(c_iStrIdTradeNeedConfirmation);
      }            
   }
   // return our result
   return l_eCanAffordExpense; 
}

/*!
 * Event will be sent to our server through advisor
 **/
void GClientAdvisor::SendEventToServer(GGameEventSPtr in_Evt)
{
   // retrieve our event type
   UINT32 l_iEvtType = ((SDK::Event::GGameEvent*)in_Evt.get())->Type();
   // raise it
   g_Joshua.RaiseEvent(in_Evt);
   // inform observers that our event was raised
   NotifyObserver(EClientAdvisorNotification::EventIsOk, (void*)l_iEvtType);
}

/*!
 * Event eont be sent to our server
 **/
void GClientAdvisor::CancelEvent(GGameEventSPtr in_Evt)
{
   // retrieve our event type
   UINT32 l_iEvtType = ((SDK::Event::GGameEvent*)in_Evt.get())->Type();
   // inform our observers that our event wasnt raised
   NotifyObserver(EClientAdvisorNotification::EventWasCanceled, (void*)l_iEvtType);
}

void GClientAdvisor::DisplayRegionGained(void* in_pThis)
{
   // The void param corresponds to our object pointer
   GClientAdvisor* l_pThis = (GClientAdvisor*) in_pThis;

   // Compose the military region gained message
   GString l_sText = g_ClientDAL.GetString(102313);
   l_sText += L"\n";

   for(set<UINT32>::const_iterator it = l_pThis->m_RegionsGained.begin();
       it != l_pThis->m_RegionsGained.end();++ it)
   {
      l_sText += GString("   ") + g_ClientDAL.GetString(*it) + GString("\n");
   }

   // Pop a generic message window displaying the message
   g_ClientDDL.RegionControlChangeWindowSpawn();
   g_ClientDDL.RegionControlChangeWindow()->Show(l_sText);

   // Reset region control gains
   l_pThis->m_RegionsGained.clear();
   l_pThis->m_iRegionGainedMailID = 0;
}

void GClientAdvisor::DisplayRegionLost(void* in_pThis)
{
   // The void param corresponds to our object pointer
   GClientAdvisor* l_pThis = (GClientAdvisor*) in_pThis;

   // Compose the military region lost message
   GString l_sText = g_ClientDAL.GetString(102314);
   l_sText += L"\n";

   for(set<UINT32>::const_iterator it = l_pThis->m_RegionsLost.begin();
       it != l_pThis->m_RegionsLost.end();++ it)
   {
      l_sText += GString("   ") + g_ClientDAL.GetString(*it) + GString("\n");
   }

   // Pop a generic message window displaying the message
   g_ClientDDL.RegionControlChangeWindowSpawn();
   g_ClientDDL.RegionControlChangeWindow()->Show(l_sText);

   // Reset region control losts
   l_pThis->m_RegionsLost.clear();
   l_pThis->m_iRegionLostMailID = 0;
}

