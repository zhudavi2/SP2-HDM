/**************************************************************
*
* sp2_economic_event_handler.cpp
*
* Description
* ===========
*  Used to handle every economic events that happen
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#include "golem_pch.hpp"

using namespace SP2::Event;
using namespace SDK::Event;

SP2::GEconomicEventHandler::GEconomicEventHandler()
{
}

SP2::GEconomicEventHandler::~GEconomicEventHandler()
{
}

bool SP2::GEconomicEventHandler::HandleUpdateEconomicHealth(SDK::GGameEventSPtr in_Event)
{
   //Get the game event and the player from who it's coming
   SP2::Event::GEconomicHealthUpdate* l_pUpdate = (SP2::Event::GEconomicHealthUpdate*)in_Event.get();
   
   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayer(l_pUpdate->m_iSource);
   if(!l_pPlayer)
      return false;
   
   GCountryData* l_pData = g_ServerDAL.CountryData(l_pUpdate->m_iCountryId);

   if(l_pUpdate->m_bGetData)
   {      
      l_pUpdate->m_fIncomeTax = l_pData->PersonalIncomeTax();
      l_pUpdate->m_fInflation = l_pData->InflationLevel();
      l_pUpdate->m_fInterestLevel = l_pData->InterestLevel();
      l_pUpdate->m_fPovertyRate = l_pData->PopInPoverty();
      l_pUpdate->m_fUnemploymentRate = l_pData->Pop1565Unemployed();
      l_pUpdate->m_fGdp = l_pData->GDPValue();
      l_pUpdate->m_iPopulation = l_pData->Population();
      
      l_pUpdate->m_iTarget = l_pUpdate->m_iSource;
      l_pUpdate->m_iSource = SDK::Event::ESpecialTargets::Server;

      g_Joshua.RaiseEvent(in_Event);
   }
   else
   {
      g_ServerDCL.ChangePersonalIncomeTax(l_pUpdate->m_iCountryId, l_pData->PersonalIncomeTax(), l_pUpdate->m_fIncomeTax);
      g_ServerDCL.ChangeInterestRate(l_pUpdate->m_iCountryId, l_pData->InterestLevel(), (REAL32)l_pUpdate->m_fInterestLevel);
      // This event requires an acknowledgement !
      g_ServerDCL.AcknowledgeRequest(l_pUpdate->m_iRequestID, l_pUpdate->m_iSource);
   }

   return true;
}

bool SP2::GEconomicEventHandler::HandleUpdateResources(SDK::GGameEventSPtr in_Event)
{
   //Get the game event and the player from who it's coming
   SP2::Event::GResourcesUpdate* l_pUpdate = (SP2::Event::GResourcesUpdate*)in_Event.get();
   
   if(l_pUpdate->m_bGetData)
   {
      INT32 l_iCountryID = l_pUpdate->m_iCountryID;
      GCountryData* l_pData = g_ServerDAL.CountryData(l_iCountryID);
      l_pUpdate->m_iNbResources = EResources::ItemCount;
      for(INT32 i=0; i<EResources::ItemCount; i++)
      {
         l_pUpdate->m_fDemand[i]          = l_pData->ResourceDemand((SP2::EResources::Enum)i);
         l_pUpdate->m_fProduction[i]      = l_pData->ResourceProduction((SP2::EResources::Enum)i);

         l_pUpdate->m_fImport[i]          = l_pData->ResourceImport((SP2::EResources::Enum)i);
         l_pUpdate->m_fExport[i]          = l_pData->ResourceExport((SP2::EResources::Enum)i);

         l_pUpdate->m_fImportDesired[i]   = l_pData->ResourceImportDesired((SP2::EResources::Enum)i);
         l_pUpdate->m_fExportDesired[i]   = l_pData->ResourceExportDesired((SP2::EResources::Enum)i);

         l_pUpdate->m_fGdp[i]             = l_pData->ResourceGDP((SP2::EResources::Enum)i);
         l_pUpdate->m_fTaxes[i]           = l_pData->ResourceTaxes((SP2::EResources::Enum)i);

         l_pUpdate->m_bGvtCtrl[i]         = l_pData->ResourceGvtCtrl((SP2::EResources::Enum)i);
         l_pUpdate->m_bLegal[i]           = l_pData->ResourceLegal((SP2::EResources::Enum)i);
			l_pUpdate->m_bMeetConsumption[i] = l_pData->ResourceMeetConsumption((SP2::EResources::Enum)i);

			l_pUpdate->m_fMarketAvailability[i]	= l_pData->m_pResourceMarketAvailability[i];
			l_pUpdate->m_fMarketShare[i]		=	l_pData->ResourceMarketShare((SP2::EResources::Enum)i);

      }

      l_pUpdate->m_iNbResources = EResources::ItemCount;
      l_pUpdate->m_fGeneralTaxMod = l_pData->GlobalTaxMod();

      l_pUpdate->m_iTarget = l_pUpdate->m_iSource;
      l_pUpdate->m_iSource = SDK::Event::ESpecialTargets::Server;

      g_Joshua.RaiseEvent(in_Event);
   }
   else
   {
      SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayer(l_pUpdate->m_iSource);
      if(!l_pPlayer)
         return false;
      
      INT16 l_iCountryId = (INT16)l_pPlayer->ModID();
		GCountryData* l_pData = g_ServerDAL.CountryData(l_iCountryId);

      g_ServerDCL.ChangeGlobalModTax(l_iCountryId, l_pUpdate->m_fGeneralTaxMod);

      if(l_pUpdate->m_fGeneralTaxMod <= g_SP2Server->GlobalTaxLimit())
      {
          for(INT32 i=0; i<l_pUpdate->m_iNbResources; i++)
          {
              g_ServerDCL.ChangeResourceGvtControlled(l_iCountryId, (SP2::EResources::Enum)l_pUpdate->m_iID[i], l_pUpdate->m_bGvtCtrl[i]);
              g_ServerDCL.ChangeResourceStatus(l_iCountryId, (SP2::EResources::Enum)l_pUpdate->m_iID[i], l_pUpdate->m_bLegal[i]);
              g_ServerDCL.ChangeResourceTax(l_iCountryId, (SP2::EResources::Enum)l_pUpdate->m_iID[i], l_pUpdate->m_fTaxes[i]);

              l_pData->ResourceMeetConsumption((SP2::EResources::Enum)l_pUpdate->m_iID[i], l_pUpdate->m_bMeetConsumption[i]);

              if(l_pUpdate->m_fImportDesired[i] > 0)
                  g_ServerDCL.ChangeResourceImportDesired(l_iCountryId, (SP2::EResources::Enum)l_pUpdate->m_iID[i], l_pUpdate->m_fImportDesired[i]);
              else
                  g_ServerDCL.ChangeResourceImportDesired(l_iCountryId, (SP2::EResources::Enum)l_pUpdate->m_iID[i], 0.f);

              if(l_pUpdate->m_fExportDesired[i] > 0)
                  g_ServerDCL.ChangeResourceExportDesired(l_iCountryId, (SP2::EResources::Enum)l_pUpdate->m_iID[i], l_pUpdate->m_fExportDesired[i]); 
              else
                  g_ServerDCL.ChangeResourceExportDesired(l_iCountryId, (SP2::EResources::Enum)l_pUpdate->m_iID[i], 0.f); 
          }
      }
      else
      {
          const INT32 l_iGlobalTaxBoxValue = static_cast<INT32>(l_pUpdate->m_fGeneralTaxMod * 100);

          // Special GTM cases.
          for(INT32 i=0; i<SP2::EResources::ItemCount; i++)
          {
              const SP2::EResources::Enum l_eResourceID = static_cast<SP2::EResources::Enum>(i);

              if(l_pData->ResourceLegal(l_eResourceID))
              {
                  if(l_iGlobalTaxBoxValue == g_SP2Server->GlobalTaxSpecial(EGlobalTaxSpecialType::ExportAll))
                  {
                      g_ServerDCL.ChangeResourceGvtControlled(l_iCountryId, l_eResourceID, true);
                      l_pData->ResourceMeetConsumption(l_eResourceID, false);
                      g_ServerDCL.ChangeResourceImportDesired(l_iCountryId, l_eResourceID, 0);
                      g_ServerDCL.ChangeResourceExportDesired(l_iCountryId, l_eResourceID, l_pData->ResourceProduction(l_eResourceID));
                      GDZLOG(g_ServerDAL.GetString(g_ServerDAL.StringIdResource(l_eResourceID)) + L" resource, " +
                             L"exporting " +
                             GString::FormatNumber(l_pData->ResourceExportDesired(l_eResourceID)/1000000, L" ", L".", L"$", L"M"),
                             EDZLogLevel::Info1);
                  }
                  else if(l_iGlobalTaxBoxValue == g_SP2Server->GlobalTaxSpecial(EGlobalTaxSpecialType::MeetDomestic))
                  {
                      g_ServerDCL.ChangeResourceGvtControlled(l_iCountryId, l_eResourceID, true);
                      g_ServerDCL.ChangeResourceExportDesired(l_iCountryId, l_eResourceID, 0);
                      g_ServerDCL.ChangeResourceImportDesired(l_iCountryId, l_eResourceID, 0);
                      l_pData->ResourceMeetConsumption(l_eResourceID, true);
                  }
                  else if(l_iGlobalTaxBoxValue == g_SP2Server->GlobalTaxSpecial(EGlobalTaxSpecialType::ImportAll))
                  {
                      g_ServerDCL.ChangeResourceGvtControlled(l_iCountryId, l_eResourceID, true);
                      l_pData->ResourceMeetConsumption(l_eResourceID, false);
                      g_ServerDCL.ChangeResourceExportDesired(l_iCountryId, l_eResourceID, 0);
                      g_ServerDCL.ChangeResourceImportDesired(l_iCountryId, l_eResourceID, l_pData->ResourceDemand(l_eResourceID));
                  }
                  else if(l_iGlobalTaxBoxValue == g_SP2Server->GlobalTaxSpecial(EGlobalTaxSpecialType::PrivatizeAll))
                      g_ServerDCL.ChangeResourceGvtControlled(l_iCountryId, l_eResourceID, false);
                  else if(!l_pData->ResourceGvtCtrl(l_eResourceID))
                      g_ServerDCL.ChangeResourceTax(l_iCountryId, l_eResourceID, l_pUpdate->m_fGeneralTaxMod);
              }
          }
      }

      // This event requires an acknowledgement !
      g_ServerDCL.AcknowledgeRequest(l_pUpdate->m_iRequestID, l_pUpdate->m_iSource);

      if(l_pUpdate->m_fGeneralTaxMod > g_SP2Server->GlobalTaxLimit())
      {
          // Send a resource window update immediately, to reduce confusion and delay.
          SDK::GGameEventSPtr l_Event                                  = CREATE_GAME_EVENT(SP2::Event::GResourcesUpdate);
          SP2::Event::GResourcesUpdate* l_pUpdateResourcesAfterSpecial = reinterpret_cast<SP2::Event::GResourcesUpdate*>(l_Event.get());
          l_pUpdateResourcesAfterSpecial->m_bGetData                   = true;
          l_pUpdateResourcesAfterSpecial->m_iCountryID                 = l_iCountryId;
          l_pUpdateResourcesAfterSpecial->m_iSource                    = l_pUpdate->m_iSource;
          HandleUpdateResources(l_Event);
      }
   }

   return true;
}

bool GEconomicEventHandler::HandleUpdateBudget(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GBudgetUpdate* l_pUpdate = (SP2::Event::GBudgetUpdate*)in_Event.get();

	g_ServerDCL.ChangeBudgetExpenseRatio( l_pUpdate->CountryId, EBudgetExpenses::Education,      l_pUpdate->BudgetExpenseEducation );
	g_ServerDCL.ChangeBudgetExpenseRatio( l_pUpdate->CountryId, EBudgetExpenses::Environment,    l_pUpdate->BudgetExpenseEnvironment );
	g_ServerDCL.ChangeBudgetExpenseRatio( l_pUpdate->CountryId, EBudgetExpenses::Government,     l_pUpdate->BudgetExpenseGovernment );
	g_ServerDCL.ChangeBudgetExpenseRatio( l_pUpdate->CountryId, EBudgetExpenses::Healthcare,     l_pUpdate->BudgetExpenseHealthcare );
	g_ServerDCL.ChangeBudgetExpenseRatio( l_pUpdate->CountryId, EBudgetExpenses::IMF,            l_pUpdate->BudgetExpenseImf );
	g_ServerDCL.ChangeBudgetExpenseRatio( l_pUpdate->CountryId, EBudgetExpenses::Infrastructure, l_pUpdate->BudgetExpenseInfrastructure );
	g_ServerDCL.ChangeBudgetExpenseRatio( l_pUpdate->CountryId, EBudgetExpenses::Propaganda,     l_pUpdate->BudgetExpensePropaganda );
	g_ServerDCL.ChangeBudgetExpenseRatio( l_pUpdate->CountryId, EBudgetExpenses::Research,       l_pUpdate->BudgetExpenseResearch );
	g_ServerDCL.ChangeBudgetExpenseRatio( l_pUpdate->CountryId, EBudgetExpenses::Telecom,        l_pUpdate->BudgetExpenseTelecom );
	g_ServerDCL.ChangeBudgetExpenseRatio( l_pUpdate->CountryId, EBudgetExpenses::Tourism,        l_pUpdate->BudgetExpenseTourism );      

   // This event requires an acknowledgement !
   g_ServerDCL.AcknowledgeRequest(l_pUpdate->m_iRequestID, l_pUpdate->m_iSource);

   return true;
}

bool GEconomicEventHandler::HandleIncreaseProduction(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GIncreaseProduction* l_pUpdate = (SP2::Event::GIncreaseProduction*)in_pEvent.get();

   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayer(l_pUpdate->m_iSource);
   if(!l_pPlayer)
      return false;
   
   INT16 l_iCountryId = (INT16)l_pPlayer->ModID();
   
   g_ServerDCL.IncreaseProduction(l_iCountryId, (SP2::EResources::Enum)l_pUpdate->m_iResourceID, l_pUpdate->m_fIncrease);

   return true;
}

bool GEconomicEventHandler::HandleGetRegionValues(SDK::GGameEventSPtr in_pEvent)
{
	SP2::Event::GEventGetRegionsValues* l_pRegionValue = (SP2::Event::GEventGetRegionsValues*)in_pEvent.get();
	l_pRegionValue->m_RegionsValues.clear();
	UINT32 l_iCountryId = l_pRegionValue->m_iCountryIdRequested;

	const set<UINT32>& l_Politicals = g_ServerDAL.CountryPoliticalControl(l_iCountryId);
	for(set<UINT32>::const_iterator it = l_Politicals.begin();
		 it != l_Politicals.end(); it++)
	{
		l_pRegionValue->m_RegionsValues[*it] = g_ServerDAL.RegionValue(*it);
	}

	//send trade towards the client destination			
	SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventGetRegionsValues);
	l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;	
	l_Event->m_iTarget = l_pRegionValue->m_iSource;
	SP2::Event::GEventGetRegionsValues* l_pNewGetRegionValue = (SP2::Event::GEventGetRegionsValues*)l_Event.get();
	l_pNewGetRegionValue->m_RegionsValues = l_pRegionValue->m_RegionsValues;
	g_Joshua.RaiseEvent(l_Event);

	return true;
}

bool GEconomicEventHandler::HandleGetTechnologiesValues(SDK::GGameEventSPtr in_pEvent)
{
	SP2::Event::GEventGetTechnologiesValues* l_pTechValue = (SP2::Event::GEventGetTechnologiesValues*)in_pEvent.get();
   // retrieve our requested country id
   INT16 l_iCountryId = l_pTechValue->m_iCountryIdRequested;	
	
	//send trade towards the client destination			
	SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventGetTechnologiesValues);
	l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;	
	l_Event->m_iTarget = l_pTechValue->m_iSource;
	SP2::Event::GEventGetTechnologiesValues* l_pNewGetTechValue = (SP2::Event::GEventGetTechnologiesValues*)l_Event.get();
   l_pNewGetTechValue->m_iCountryIdRequested = l_iCountryId;

	const GResearchInformationItf* l_pResearchInfo = g_ServerDAL.CountryData(l_iCountryId)->ResearchInfo();	
	for(UINT32 i=0; i < EUnitCategory::ItemCount; i++)
	{
		for(UINT32 j=0; j < EUnitDesignCharacteristics::ItemCount; j++)
		{
			l_pNewGetTechValue->m_Technologies[i][j] = (UINT8)(floor(l_pResearchInfo->m_fMaxValues[i][j]));
		}
	}

	g_Joshua.RaiseEvent(l_Event);

   return true;
}

bool GEconomicEventHandler::HandleTradeOffer(SDK::GGameEventSPtr in_pEvent)
{
	SP2::Event::GEventTradeProposal* l_pTrade = (SP2::Event::GEventTradeProposal*)in_pEvent.get();
	if(l_pTrade->m_trdData.Status() == ETradeStatus::Accepted)
	{
		//Execute trade

		UINT32 l_iCountryOnSideA = l_pTrade->m_trdData.GetTradeDataFromSide(true).CountryId();
		UINT32 l_iCountryOnSideB = l_pTrade->m_trdData.GetTradeDataFromSide(false).CountryId();
		REAL64 l_fMoneyToGiveToSideA = l_pTrade->m_trdData.GetTradeDataFromSide(false).Money();
		REAL64 l_fMoneyToGiveToSideB = l_pTrade->m_trdData.GetTradeDataFromSide(true).Money();
		g_ServerDCL.TakeMoney(l_iCountryOnSideA,l_fMoneyToGiveToSideB);
		g_ServerDCL.TakeMoney(l_iCountryOnSideB,l_fMoneyToGiveToSideA);
		g_ServerDCL.GiveMoney(l_iCountryOnSideA,l_fMoneyToGiveToSideA);
		g_ServerDCL.GiveMoney(l_iCountryOnSideB,l_fMoneyToGiveToSideB);

		GRegionsValues& l_RegionsToGiveToSideA = l_pTrade->m_trdData.GetTradeDataFromSide(false).GetRegions();
		GRegionsValues& l_RegionsToGiveToSideB = l_pTrade->m_trdData.GetTradeDataFromSide(true).GetRegions();
		
		for(GRegionsValues::const_iterator it = l_RegionsToGiveToSideA.begin();
			 it != l_RegionsToGiveToSideA.end(); it++)
		{
			if(g_ServerDAL.RegionControl(it->first).m_iMilitary == 
				g_ServerDAL.RegionControl(it->first).m_iPolitical)
			{
				g_ServerDCL.ChangeRegionMilitaryControl(it->first,l_iCountryOnSideA);				
			}
			//Force the political change
			g_ServerDCL.ChangeRegionPoliticalControl(it->first,l_iCountryOnSideA, true);
			
		}

		for(GRegionsValues::const_iterator it = l_RegionsToGiveToSideB.begin();
			 it != l_RegionsToGiveToSideB.end(); it++)
		{
			if(g_ServerDAL.RegionControl(it->first).m_iMilitary == 
				g_ServerDAL.RegionControl(it->first).m_iPolitical)
			{
				g_ServerDCL.ChangeRegionMilitaryControl(it->first,l_iCountryOnSideB);				
			}
			//Force the political change
			g_ServerDCL.ChangeRegionPoliticalControl(it->first,l_iCountryOnSideB, true);
		}		
      
		GTechnologiesValues::const_iterator l_It;
		// retrieve trade data from requested side
		GTradeDataForOneSide &l_TechsToGiveToSideB = l_pTrade->m_trdData.GetTradeDataFromSide(true);
		GTradeDataForOneSide &l_TechsToGiveToSideA = l_pTrade->m_trdData.GetTradeDataFromSide(false);
		// loop into the techs and add them to that country
		for(l_It = l_TechsToGiveToSideB.GetTechnologies().begin(); l_It != l_TechsToGiveToSideB.GetTechnologies().end(); l_It++)
		{
			EUnitCategory::Enum l_eCategory = l_It->first.Type();
			EUnitDesignCharacteristics::Enum l_eChar = l_It->first.Name();
			UINT8 l_iLevel = (UINT8)l_It->first.Level();
			g_ServerDAL.CountryData(l_iCountryOnSideB)->ResearchInfo()->m_fMaxValues[l_eCategory][l_eChar] = max(l_iLevel,
				g_ServerDAL.CountryData(l_iCountryOnSideB)->ResearchInfo()->m_fMaxValues[l_eCategory][l_eChar]);
		}
		for(l_It = l_TechsToGiveToSideA.GetTechnologies().begin(); l_It != l_TechsToGiveToSideA.GetTechnologies().end(); l_It++)
		{
			EUnitCategory::Enum l_eCategory = l_It->first.Type();
			EUnitDesignCharacteristics::Enum l_eChar = l_It->first.Name();
			UINT8 l_iLevel = (UINT8)l_It->first.Level();
			g_ServerDAL.CountryData(l_iCountryOnSideA)->ResearchInfo()->m_fMaxValues[l_eCategory][l_eChar] = max(l_iLevel,
				g_ServerDAL.CountryData(l_iCountryOnSideA)->ResearchInfo()->m_fMaxValues[l_eCategory][l_eChar]);
		}
		
		if( l_pTrade->m_trdData.EvaluateTrade() != SP2::ETradeEvaluationResults::Fair )
		{
			//Find the winner GDP, to calculate the bonus to relations
			REAL64 l_fWinnerGDP = l_pTrade->m_trdData.EvaluateTrade() > SP2::ETradeEvaluationResults::Fair ?
                               g_ServerDAL.CountryData(l_iCountryOnSideA)->GDPValue()                   :
                               g_ServerDAL.CountryData(l_iCountryOnSideB)->GDPValue();	
			REAL64 l_fTotalTradeValue = max(l_pTrade->m_trdData.GetTradeDataFromSide(true).GetTotalValue(),
													  l_pTrade->m_trdData.GetTradeDataFromSide(false).GetTotalValue());
			if(l_fWinnerGDP > 0.f)
			{
				REAL32 l_fRatio = (REAL32)(l_fTotalTradeValue / l_fWinnerGDP);
				REAL32 l_fRelationsGain = l_fRatio * SP2::c_fRelationsGainTrade;
				g_ServerDAL.RelationBetweenCountries( l_iCountryOnSideA, l_iCountryOnSideB, 
					g_ServerDAL.RelationBetweenCountries( l_iCountryOnSideA, l_iCountryOnSideB) + l_fRelationsGain);
			}

		}

		if(!g_Joshua.ActivePlayerByModID(l_pTrade->m_trdData.GetTradeDataFromSide(true).CountryId() )->AIControlled())
		{
			//Send refused event to player
			SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventTradeProposal);
			l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;	
			l_Event->m_iTarget = g_Joshua.ActivePlayerByModID(l_pTrade->m_trdData.GetTradeDataFromSide(true).CountryId() )->Id();
			SP2::Event::GEventTradeProposal* l_pNewTrade = (SP2::Event::GEventTradeProposal*)l_Event.get();
			l_pNewTrade->m_trdData = l_pTrade->m_trdData;
			g_Joshua.RaiseEvent(l_Event);
		}

		//Synchronize the 2 countries
		g_ServerDAL.CountryData(l_iCountryOnSideA)->SynchronizeWithRegions();
		g_ServerDAL.CountryData(l_iCountryOnSideB)->SynchronizeWithRegions();
	}
	else if(l_pTrade->m_trdData.Status() == ETradeStatus::Pending)
	{
		if(g_Joshua.ActivePlayerByModID(l_pTrade->m_trdData.GetTradeDataFromSide(false).CountryId() )->AIControlled())
		{
			//AI will handle it
			GAI::HandleTradeOffer(l_pTrade->m_trdData);

		}
		else
		{
			//send trade towars the client destination			
			SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventTradeProposal);
			l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;	
			l_Event->m_iTarget = g_Joshua.ActivePlayerByModID(l_pTrade->m_trdData.GetTradeDataFromSide(false).CountryId() )->Id();
			SP2::Event::GEventTradeProposal* l_pNewTrade = (SP2::Event::GEventTradeProposal*)l_Event.get();
			l_pNewTrade->m_trdData = l_pTrade->m_trdData;
			g_Joshua.RaiseEvent(l_Event);
		}
		
	}
	else
	{
		if(!g_Joshua.ActivePlayerByModID(l_pTrade->m_trdData.GetTradeDataFromSide(true).CountryId() )->AIControlled())
		{
			//Send refused event to player
			SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GEventTradeProposal);
			l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;	
			l_Event->m_iTarget = g_Joshua.ActivePlayerByModID(l_pTrade->m_trdData.GetTradeDataFromSide(true).CountryId() )->Id();
			SP2::Event::GEventTradeProposal* l_pNewTrade = (SP2::Event::GEventTradeProposal*)l_Event.get();
			l_pNewTrade->m_trdData = l_pTrade->m_trdData;
			g_Joshua.RaiseEvent(l_Event);
		}
	}

	return true;
}

bool GEconomicEventHandler::HandleEconomicWindowUpdate(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GEconomicUpdate* l_pUpdate = (SP2::Event::GEconomicUpdate*)in_pEvent.get();

   l_pUpdate->m_iTarget = l_pUpdate->m_iSource;
   l_pUpdate->m_iSource = SDK::Event::ESpecialTargets::Server;

   GCountryData* l_pData = g_ServerDAL.CountryData(l_pUpdate->CountryId);

   REAL64 l_fResProd = 0.f;
   l_fResProd += l_pData->ResourceProduction(EResources::Appliances);
   l_fResProd += l_pData->ResourceProduction(EResources::Cereals);
   l_fResProd += l_pData->ResourceProduction(EResources::Chemicals);
   l_fResProd += l_pData->ResourceProduction(EResources::Commodities);
   l_fResProd += l_pData->ResourceProduction(EResources::Construction);
   l_fResProd += l_pData->ResourceProduction(EResources::Dairy);
   l_fResProd += l_pData->ResourceProduction(EResources::Drugs);
   l_fResProd += l_pData->ResourceProduction(EResources::Electricity);
   l_fResProd += l_pData->ResourceProduction(EResources::Engineering);
   l_fResProd += l_pData->ResourceProduction(EResources::Fabrics);
   l_fResProd += l_pData->ResourceProduction(EResources::Fossile_Fuels);
   l_fResProd += l_pData->ResourceProduction(EResources::Health_Care);
   l_fResProd += l_pData->ResourceProduction(EResources::Iron_Steel);
   l_fResProd += l_pData->ResourceProduction(EResources::Legal_Services);
   l_fResProd += l_pData->ResourceProduction(EResources::Luxury_Commodities);
   l_fResProd += l_pData->ResourceProduction(EResources::Machinery);
   l_fResProd += l_pData->ResourceProduction(EResources::Marketing_Advertising);
   l_fResProd += l_pData->ResourceProduction(EResources::Meat);
   l_fResProd += l_pData->ResourceProduction(EResources::Minerals);
   l_fResProd += l_pData->ResourceProduction(EResources::Pharmaceuticals);
   l_fResProd += l_pData->ResourceProduction(EResources::Plastics);
   l_fResProd += l_pData->ResourceProduction(EResources::Precious_Stones);
   l_fResProd += l_pData->ResourceProduction(EResources::Retail);
   l_fResProd += l_pData->ResourceProduction(EResources::Tobacco);
   l_fResProd += l_pData->ResourceProduction(EResources::Vegetable_Fruits);
   l_fResProd += l_pData->ResourceProduction(EResources::Vehicules);
   l_fResProd += l_pData->ResourceProduction(EResources::Wood_Paper);

   REAL64 l_fResDemd = 0.f;
   l_fResDemd += l_pData->ResourceDemand(EResources::Appliances);
   l_fResDemd += l_pData->ResourceDemand(EResources::Cereals);
   l_fResDemd += l_pData->ResourceDemand(EResources::Chemicals);
   l_fResDemd += l_pData->ResourceDemand(EResources::Commodities);
   l_fResDemd += l_pData->ResourceDemand(EResources::Construction);
   l_fResDemd += l_pData->ResourceDemand(EResources::Dairy);
   l_fResDemd += l_pData->ResourceDemand(EResources::Drugs);
   l_fResDemd += l_pData->ResourceDemand(EResources::Electricity);
   l_fResDemd += l_pData->ResourceDemand(EResources::Engineering);
   l_fResDemd += l_pData->ResourceDemand(EResources::Fabrics);
   l_fResDemd += l_pData->ResourceDemand(EResources::Fossile_Fuels);
   l_fResDemd += l_pData->ResourceDemand(EResources::Health_Care);
   l_fResDemd += l_pData->ResourceDemand(EResources::Iron_Steel);
   l_fResDemd += l_pData->ResourceDemand(EResources::Legal_Services);
   l_fResDemd += l_pData->ResourceDemand(EResources::Luxury_Commodities);
   l_fResDemd += l_pData->ResourceDemand(EResources::Machinery);
   l_fResDemd += l_pData->ResourceDemand(EResources::Marketing_Advertising);
   l_fResDemd += l_pData->ResourceDemand(EResources::Meat);
   l_fResDemd += l_pData->ResourceDemand(EResources::Minerals);
   l_fResDemd += l_pData->ResourceDemand(EResources::Pharmaceuticals);
   l_fResDemd += l_pData->ResourceDemand(EResources::Plastics);
   l_fResDemd += l_pData->ResourceDemand(EResources::Precious_Stones);
   l_fResDemd += l_pData->ResourceDemand(EResources::Retail);
   l_fResDemd += l_pData->ResourceDemand(EResources::Tobacco);
   l_fResDemd += l_pData->ResourceDemand(EResources::Vegetable_Fruits);
   l_fResDemd += l_pData->ResourceDemand(EResources::Vehicules);
   l_fResDemd += l_pData->ResourceDemand(EResources::Wood_Paper);

   l_pUpdate->AvailableMoney = l_pData->MonetarySupplyAmount();
   l_pUpdate->Expense       = l_pData->BudgetExpenses();
   l_pUpdate->Income        = l_pData->BudgetRevenues();
	if(l_fResDemd <= 0.f)
		l_pUpdate->RessRatio = 0.f;
	else
		l_pUpdate->RessRatio     = l_fResProd / l_fResDemd;
   l_pUpdate->EconomicModel = l_pData->CountryEconomicModel();
	l_pUpdate->EconomicHealth = l_pData->EconomicHealth();

   g_Joshua.RaiseEvent(in_pEvent);

   return true;
}