/**************************************************************
*
* sp2_economic_event_handler.cpp
*
* Description
* ===========
*  Used to handle every economic events that happen
*  on client side
*
*  This file is currently empty, i'm not even sure it's supposed to
*  exist but by the way it's here if ever we want to handle something
*  on client side
*  
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

#include "../include/sp2_economic_window.h"
#include "../include/sp2_budget_window.h"
#include "../include/sp2_economic_health_window.h"
#include "../include/sp2_resources_window.h"
#include "../include/sp2_mail_window.h"
#include "../include/sp2_trade_detailed_window.h"

GEconomicEventHandler::GEconomicEventHandler()
{
}

GEconomicEventHandler::~GEconomicEventHandler()
{
}

bool GEconomicEventHandler::HandleEconomicWindowUpdate(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GEconomicUpdate* l_pUpdate = (SP2::Event::GEconomicUpdate*)in_pEvent.get();

   GDataEconomicWindow & DataEconomicWindow = g_ClientDAL.DataEconomicWindow;

   DataEconomicWindow.AvailableMoney = l_pUpdate->AvailableMoney;
   DataEconomicWindow.Expense       = l_pUpdate->Expense;
   DataEconomicWindow.Income        = l_pUpdate->Income;
   DataEconomicWindow.RessRatio     = (REAL32)l_pUpdate->RessRatio;
   DataEconomicWindow.EconomicModel = l_pUpdate->EconomicModel;
	DataEconomicWindow.EconomicHealth = l_pUpdate->EconomicHealth;

   if(g_ClientDDL.EconomicWindow())   
      g_ClientDDL.EconomicWindow()->Update();
   
   return true;
}

bool GEconomicEventHandler::HandleEconomicHealthWindowUpdate(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GEconomicHealthUpdate* l_pUpdate = (SP2::Event::GEconomicHealthUpdate*)in_pEvent.get();

   GDataEconomicHealthWindow & Data = g_ClientDAL.DataEconomicHealthWindow;

   Data.m_fIncomeTax          = l_pUpdate->m_fIncomeTax;
   Data.m_fInterestLevel      = l_pUpdate->m_fInterestLevel;
   Data.m_fPovertyRate        = l_pUpdate->m_fPovertyRate;
   Data.m_fUnemploymentRate   = l_pUpdate->m_fUnemploymentRate;
   Data.m_fGdp                = l_pUpdate->m_fGdp;
   Data.m_fInflation          = l_pUpdate->m_fInflation;
   Data.m_fInflationVar       = l_pUpdate->m_fInflationVar;
   Data.m_iPopulation         = l_pUpdate->m_iPopulation;

   if(g_ClientDDL.EconomicHealthWindow() && g_ClientDDL.EconomicHealthWindow()->Visible())
      g_ClientDDL.EconomicHealthWindow()->Update(true);

   return true;
}

bool GEconomicEventHandler::HandleResourcesWindowUpdate(SDK::GGameEventSPtr in_pEvent)
{
   SP2::Event::GResourcesUpdate* l_pUpdate = (SP2::Event::GResourcesUpdate*)in_pEvent.get();

   GDataResourcesWindow & Data = g_ClientDAL.DataResourcesWindow;

   for(INT32 i=0; i<l_pUpdate->m_iNbResources; i++)
   {
      Data.m_fProduction[i]   = l_pUpdate->m_fProduction[i];
      Data.m_fConsumption[i]  = l_pUpdate->m_fDemand[i];
      Data.m_fProduction[i]   = l_pUpdate->m_fProduction[i];
      Data.m_fTrade[i]        = l_pUpdate->m_fExport[i] - l_pUpdate->m_fImport[i];
      Data.m_fDesired[i]      = l_pUpdate->m_fExportDesired[i] - l_pUpdate->m_fImportDesired[i];
      Data.m_fPartGdp[i]      = l_pUpdate->m_fGdp[i];
      Data.m_fSectorTax[i]    = l_pUpdate->m_fTaxes[i];
      Data.m_bManagement[i]   = l_pUpdate->m_bGvtCtrl[i];
      Data.m_bStatus[i]       = l_pUpdate->m_bLegal[i];
      Data.m_fGlobalTaxMod    = l_pUpdate->m_fGeneralTaxMod;
      Data.m_fMarketAvailability[i] = l_pUpdate->m_fMarketAvailability[i];
		Data.m_fMarketShare[i]	= l_pUpdate->m_fMarketShare[i];
      Data.m_bMeetConsumption[i] = l_pUpdate->m_bMeetConsumption[i];
      Data.m_bDirty[i] = false;
   }

   if(l_pUpdate->m_iCountryID == g_ClientDCL.SelectedCountryID())
   {
      if(g_ClientDDL.ResourcesWindow() && g_ClientDDL.ResourcesWindow()->Visible())
         g_ClientDDL.ResourcesWindow()->Update(true);
   }

   return true;
}

/*!
 * Handle an update of the regions values from the server
 **/
bool GEconomicEventHandler::HandleRegionsValuesUpdate(SDK::GGameEventSPtr in_pEvent)
{
   // retrieve the GEventGetRegionsValues pointer
   SP2::Event::GEventGetRegionsValues* l_pUpdate = (SP2::Event::GEventGetRegionsValues*)in_pEvent.get();
   // is trade detailed window visible ?
   if(g_ClientDDL.TradeDetailedWindow() && g_ClientDDL.TradeDetailedWindow()->Visible())
   {
      g_ClientDDL.TradeDetailedWindow()->SetRegionsValues(l_pUpdate->m_RegionsValues);
   }
   // everythins Aok
   return true;
}

/*!
 * Handle an update of the technologies values from the server
 **/
bool GEconomicEventHandler::HandleTechnologiesValuesUpdate(SDK::GGameEventSPtr in_pEvent)
{
   // retrieve the GEventGetRegionsValues pointer
   SP2::Event::GEventGetTechnologiesValues* l_pUpdate = (SP2::Event::GEventGetTechnologiesValues*)in_pEvent.get();
   // is trade detailed window visible ?
   if(g_ClientDDL.TradeDetailedWindow() && g_ClientDDL.TradeDetailedWindow()->Visible())
   {
      // set our calues for the requested country
      g_ClientDDL.TradeDetailedWindow()->SetTechnologiesLevels(l_pUpdate->m_iCountryIdRequested, 
                                                               l_pUpdate->m_Technologies);
   }
   // everythins Aok
   return true;
}

/*!
 * Handle a trade proposal request from the server
 **/
bool GEconomicEventHandler::HandleRegionsTradeProposal(SDK::GGameEventSPtr in_pEvent)
{
   // retrieve the GEventTradeProposal pointer
   SP2::Event::GEventTradeProposal* l_pUpdate = (SP2::Event::GEventTradeProposal*)in_pEvent.get();
   // Add a mail for that
   g_ClientDAL.AddTradeMail(l_pUpdate->m_trdData);
   // everythins Aok
   return true;
}