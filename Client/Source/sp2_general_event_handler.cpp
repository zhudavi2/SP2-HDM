/**************************************************************
*
* sp2_general_event_handler.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

#include "../../includes/common_lib//sp2_event_history_markers.h"

#include "../include/sp2_background_earth.h"
#include "../include/sp2_fsm_main.h"
#include "../include/sp2_game_lobby_window.h"
#include "../include/sp2_generic_message_window.h"
#include "../include/sp2_start_single_player_window.h"
#include "../include/sp2_advisor_window.h"
#include "../include/sp2_main_bar.h"
#include "../include/sp2_country_information_window.h"
#include "../include/sp2_relations_window.h"
#include "../include/sp2_thematic_map_legend_window.h"
#include "../include/sp2_end_game_window.h"
#include "../include/sp2_objectives_window.h"
#include "../include/sp2_mail_window.h"
#include "../include/sp2_party_selection_window.h"
#include "../include/sp2_treaties_detailed_window.h"
#include "../../includes/common_lib//sp2_game_scenario.h"


using namespace SP2::Event;


/*!
 * Constructor
 **/
SP2::GGeneralEventHandler::GGeneralEventHandler()
{
}

/*!
 * Destructor
 **/
SP2::GGeneralEventHandler::~GGeneralEventHandler()
{
}

/*!
 * Handle the start game event
 * @param in_Event: A Smart Pointer to the Event that must be handled
 **/
bool SP2::GGeneralEventHandler::HandleStartGame(SDK::GGameEventSPtr in_Event)
{      
   GStartGame* l_StartGame = (GStartGame*)in_Event.get();

#ifdef GOLEM_DEBUG
   g_Joshua.Log("Start game event received");
#endif

   g_ClientDAL.ZeroDate(GDateTime(l_StartGame->StartDate) );

   // Set Country
   const GCountry& l_Country = g_ClientDAL.Country(g_ClientDAL.ControlledCountryID() );
   g_ClientDCL.SelectedCountrySetLookAt(l_Country);

   // Start Game 
   g_ClientDCL.StartGame();

   // Hide wait label
   if(g_ClientDDL.GameLobby())
      g_ClientDDL.GameLobby()->HideWaitLabel();

   return true;
}



/*!
 * Handle the event that sends all the available countries so the client
 * can pick one to be its controlled country.
 **/
bool SP2::GGeneralEventHandler::HandleReceiveAvailableCountries(SDK::GGameEventSPtr in_Event)
{
   // Received country and send it to the game lobby
   GSendAvailableCountries* l_pEvent = (GSendAvailableCountries*)in_Event.get();   
   vector<UINT16> l_vAvailableCountries = l_pEvent->AvailableCountries();
   if(g_ClientDDL.GameLobby())
      g_ClientDDL.GameLobby()->AvailableCountries(l_vAvailableCountries);
   return true;
}

/*!
 * Handle the event that notifies something to the client from the server
 **/
bool SP2::GGeneralEventHandler::HandleNotifyClient(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GNotifyClient* l_pNotifyEvent = (SP2::Event::GNotifyClient*)in_Event.get();

   if(g_ClientDDL.BackgroundEarth() )
   {
      g_ClientDDL.BackgroundEarth()->AddNotificationSignOnEarth(GVector2D<REAL32>(l_pNotifyEvent->Coords()),
                                                                                 l_pNotifyEvent->Msg());
   }

   return true;
}

bool SP2::GGeneralEventHandler::HandleRandomEvents(SDK::GGameEventSPtr in_Event)
{
   IF_RETURN(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing, false);

	SP2::Event::GRandomEvent* l_pRandomEvent = (SP2::Event::GRandomEvent*)in_Event.get();
	if(g_ClientDDL.BackgroundEarth() )
   {
		GString l_sEvent =L"";
		l_sEvent = g_ClientDAL.GetString(g_ClientDAL.StringIdRandomEvent(l_pRandomEvent->m_RandomEvent));
		if(l_sEvent != L"")
		{
			//Display sign on the globe
			g_ClientDDL.BackgroundEarth()->AddNotificationSignOnEarth(
			GVector2D<REAL32>(l_pRandomEvent->m_fLongitude,l_pRandomEvent->m_fLatitude),
			l_sEvent);

			//Insert news
			SP2::News::GInfo l_RandomEventNews;
			switch(l_pRandomEvent->m_RandomEvent)
			{
			case ERandomEvents::AntiGlobalizationProtest:
				l_RandomEventNews.m_eType = News::EType::REAntiGlobalProtest;break;
			case ERandomEvents::Avalanche:
				l_RandomEventNews.m_eType = News::EType::REAvalanche; 
				l_RandomEventNews.m_vNumbers.push_back((INT32)-(l_pRandomEvent->m_iPopulationChange));
				l_RandomEventNews.m_vNumbers.push_back((INT32)-(l_pRandomEvent->m_iPopulationChange*3));break;
			case ERandomEvents::Blizzard:
				l_RandomEventNews.m_eType = News::EType::REBlizzard; break;
			case ERandomEvents::BountifulHarvest:
				l_RandomEventNews.m_eType = News::EType::REBountifulHarvest; break;
			case ERandomEvents::ChemicalSpill:
				l_RandomEventNews.m_eType = News::EType::REChemicalSpill; 
				l_RandomEventNews.m_vNumbers.push_back(2000); break;
			case ERandomEvents::ColdWave:
				l_RandomEventNews.m_eType = News::EType::REColdWave; break;
			case ERandomEvents::Cyclone:
				l_RandomEventNews.m_eType = News::EType::RECyclone; break;
			case ERandomEvents::Drought:
				l_RandomEventNews.m_eType = News::EType::REDrought; break;
			case ERandomEvents::Earthquake:
				l_RandomEventNews.m_eType = News::EType::REEarthquake; 
				l_RandomEventNews.m_vNumbers.push_back((INT32)-(l_pRandomEvent->m_iPopulationChange)); break;
			case ERandomEvents::EconomicBoom:
				l_RandomEventNews.m_eType = News::EType::REEconomicBoom; break;
			case ERandomEvents::EconomicDepression:
				l_RandomEventNews.m_eType = News::EType::REEconomicDepression; break;
			case ERandomEvents::Epidemic:
				l_RandomEventNews.m_eType = News::EType::REEpidemic; 
				l_RandomEventNews.m_vNumbers.push_back((INT32)-(l_pRandomEvent->m_iPopulationChange)); break;
			case ERandomEvents::Famine:
				l_RandomEventNews.m_eType = News::EType::REFamine; 
				l_RandomEventNews.m_vNumbers.push_back((INT32)-(l_pRandomEvent->m_iPopulationChange)); break;
			case ERandomEvents::Flood:
				l_RandomEventNews.m_eType = News::EType::REFlood; 
				l_RandomEventNews.m_vNumbers.push_back((INT32)-(l_pRandomEvent->m_iPopulationChange)); break;
			case ERandomEvents::HeathWave:
				l_RandomEventNews.m_eType = News::EType::REHeatwave; 
				l_RandomEventNews.m_vNumbers.push_back((INT32)-(l_pRandomEvent->m_iPopulationChange)); break;
			case ERandomEvents::Hurricane:
				l_RandomEventNews.m_eType = News::EType::REHurricane; 
				l_RandomEventNews.m_vNumbers.push_back((INT32)-(l_pRandomEvent->m_iPopulationChange)); break;
			case ERandomEvents::IndustrialAccident:
				l_RandomEventNews.m_eType = News::EType::REIndustrialAccident; break;
			case ERandomEvents::InsectInfestation:
				l_RandomEventNews.m_eType = News::EType::REInsectInfestation; break;
			case ERandomEvents::Landslide:
				l_RandomEventNews.m_eType = News::EType::RELandslide; 
				l_RandomEventNews.m_vNumbers.push_back((INT32)-(l_pRandomEvent->m_iPopulationChange)); break;
			case ERandomEvents::LivestockEpidemic:
				l_RandomEventNews.m_eType = News::EType::RELivestockEpidemic; break;
			case ERandomEvents::NuclearMeltdown:
				l_RandomEventNews.m_eType = News::EType::RENuclearMeltdown; break;
			case ERandomEvents::PoorHarvest:
				l_RandomEventNews.m_eType = News::EType::REPoorHarvest; break;
			case ERandomEvents::PopulationBoom:
				l_RandomEventNews.m_eType = News::EType::REPopulationBoom; break;
			case ERandomEvents::ResourceDiscoveryEnergy:
				l_RandomEventNews.m_eType = News::EType::REResourceDiscoveryEnergy; break;
			case ERandomEvents::ResourceDiscoveryOre:
				{
					l_RandomEventNews.m_eType = News::EType::REResourceDiscoveryOre; 
					for(UINT32 k=0; k<EResources::ItemCount; k++)
					{
						if(l_pRandomEvent->m_pResourceProductionChange[k] > 0)
						{
							l_RandomEventNews.m_vStrings.push_back(g_ClientDAL.StringIdResource((EResources::Enum)k));
							break;
						}
					}				
				}
				break;
			case ERandomEvents::ResourceDiscoveryWealth:
				l_RandomEventNews.m_eType = News::EType::REResourceDiscoveryWealth; break;
			case ERandomEvents::StockMarketCrash:
				l_RandomEventNews.m_eType = News::EType::REStockMarketCrash; break;
			case ERandomEvents::Storm:
				l_RandomEventNews.m_eType = News::EType::REStorm; 
				l_RandomEventNews.m_vNumbers.push_back((INT32)((-l_pRandomEvent->m_iPopulationChange)+1)*10); break;
			case ERandomEvents::TidalWave:
				l_RandomEventNews.m_eType = News::EType::RETidalwave; 
				l_RandomEventNews.m_vNumbers.push_back((INT32)-(l_pRandomEvent->m_iPopulationChange)); break;
			case ERandomEvents::Tornado:
				l_RandomEventNews.m_eType = News::EType::RETornado; 
				l_RandomEventNews.m_vNumbers.push_back((INT32)-(l_pRandomEvent->m_iPopulationChange));break;
			case ERandomEvents::Tsunami:
				l_RandomEventNews.m_eType = News::EType::RETsunami; 
				l_RandomEventNews.m_vNumbers.push_back((INT32)-(l_pRandomEvent->m_iPopulationChange));break;
			case ERandomEvents::Typhoon:
				l_RandomEventNews.m_eType = News::EType::RETyphoon; 
				l_RandomEventNews.m_vNumbers.push_back((INT32)-(l_pRandomEvent->m_iPopulationChange));break;
			case ERandomEvents::VolcanicEruption:
				l_RandomEventNews.m_eType = News::EType::REVolcanicEruption; 
				l_RandomEventNews.m_vNumbers.push_back((INT32)(-(l_pRandomEvent->m_iPopulationChange))*10);break;
			case ERandomEvents::WildFire:
				l_RandomEventNews.m_eType = News::EType::REWildfire; break;
			default:
				gassert(false,"GGeneralEventHandler::HandleRandomEvents(): Invalid type");
				return false;
			}
			l_RandomEventNews.m_iCountryA = l_pRandomEvent->m_iCountryID;
			l_RandomEventNews.m_iPriority = g_ClientDAL.NewsPriority(l_RandomEventNews.m_eType);
			if((UINT32)g_ClientDAL.ControlledCountryID() == l_pRandomEvent->m_iCountryID)
				l_RandomEventNews.m_iPriority++;			
			l_RandomEventNews.m_eCategory = News::ECategory::Demographic;
			
			g_ClientDAL.InsertPendingNews(l_RandomEventNews);	
		}
   }

	return true;
}

bool SP2::GGeneralEventHandler::HandleGetRegionsCharacteristicResult(SDK::GGameEventSPtr in_Event)
{
   IF_RETURN(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing, false);

   //Results received, there are 2 cases
   // 1 - Results are by region
   // 2 - Results are by country 
   //    In each case, we have to determine the ranking based on those results
   //    A High value is always the best.
   // 2 cases that does not work that way: CountryEmpire and HumanControlledCountries
   Event::GGetRegionsCharacteristicResult * l_pEvent = (GGetRegionsCharacteristicResult *)in_Event.get();
   vector<REAL64>&            l_vResults             = l_pEvent->Results;
   vector<REAL64>             l_RegionResults   ;
   bool                       l_bSendAdditionalResultsOnly = false;

	const vector<GRegionControl>& l_vControls = g_ClientDAL.RegionControlArray();
	INT32 l_iNbRegions = (INT32)g_ClientDAL.NbRegion();

   EThematicMapLegendType::Enum l_eThematicMapLegendToDisplay = EThematicMapLegendType::Unknown;

	if(l_pEvent->Characteristic == ECharacteristicToHighlight::Geographic &&
		g_ClientDAL.m_DataOptionsWindow.m_bClouds)
		g_ClientDDL.BackgroundEarth()->CloudsEnable();
	else
	{
		g_ClientDDL.BackgroundEarth()->CloudsDisable();		
	}

   switch (l_pEvent->Characteristic)
   {
		//--- Political Control ---
		case ECharacteristicToHighlight::PoliticalControl:
		{
			vector<REAL32> l_vHueByCountries;
			vector<REAL32> l_vBrightnessByCountries;
			UINT32 l_iNumberOfCountries = (UINT32)g_ClientDAL.NbCountry();
			l_vHueByCountries.resize(l_iNumberOfCountries + 1);
			l_vBrightnessByCountries.resize(l_iNumberOfCountries + 1);
			REAL32 l_fCurrentColour = 100.f;
			REAL32 l_fCurrentBrightness = 30.f;
			REAL32 c_fIncrementer = 37.f;
			REAL32 c_fIncrementerBrightness = 2.f;
			for(UINT32 i=1; i<=l_iNumberOfCountries; i++)
			{
				l_vHueByCountries[i] = l_fCurrentColour;
				l_vBrightnessByCountries[i] = l_fCurrentBrightness;
				l_fCurrentColour += c_fIncrementer;
				if(l_fCurrentColour > 360.f)
				{
					l_fCurrentColour  = l_fCurrentColour - 360.f;
					l_fCurrentBrightness += c_fIncrementerBrightness;
				}
			}
			for (UINT32 i = 0; i < l_vResults.size(); i++)
         {         								
				g_ClientDCL.ThematicMapColorData[i+1] = GColorRGBInt(GColorHSBReal(l_vHueByCountries[(UINT32)l_vResults[i]],60.f,l_vBrightnessByCountries[(UINT32)l_vResults[i]]));
			}
			l_eThematicMapLegendToDisplay  = EThematicMapLegendType::Unknown;
			break;
		}
		//--- Empire ---
      case ECharacteristicToHighlight::CountryEmpire:
      {
         for (UINT32 i = 0; i < l_vResults.size(); i++)
         {         				 
            EThematicMapRegionDescription::Enum l_eValue = (EThematicMapRegionDescription::Enum)((int)l_vResults[i]);
            // Known region description value ?
            if((l_eValue > EThematicMapRegionDescription::Unknown) && (l_eValue < EThematicMapRegionDescription::ItemCount))
            {
               g_ClientDCL.ThematicMapColorData[i+1] = EThematicMapEmpireColors::c_EmpireColors[l_eValue];
            }
            else
            {
               g_ClientDCL.ThematicMapColorData[i+1] = EThematicMapEmpireColors::c_EmpireColorUnknown;
            }

         }
         l_eThematicMapLegendToDisplay = EThematicMapLegendType::Empire;         
         l_bSendAdditionalResultsOnly  = true;
         break;
      }//end CountryEmpire

      //Already done by region
      case ECharacteristicToHighlight::RegionPopulationTotal:
      case ECharacteristicToHighlight::RegionPopulationDensity:      
      case ECharacteristicToHighlight::RegionResourcesProduction:
      {
         l_bSendAdditionalResultsOnly  = true;
         l_eThematicMapLegendToDisplay = EThematicMapLegendType::HighLow;
         l_RegionResults = l_pEvent->Results;
         break;
      }

      //Results by country, applying the results to the regions of the country
      case ECharacteristicToHighlight::CountryPopulationGrowth:
      case ECharacteristicToHighlight::CountryGrossDomesticProduct:
      case ECharacteristicToHighlight::CountryGrossDomesticProductGrowth:
      case ECharacteristicToHighlight::CountryPopulationSupport:
      case ECharacteristicToHighlight::CountryPopulationStabilty:
      case ECharacteristicToHighlight::CountryEconomicalStrength:
      case ECharacteristicToHighlight::CountryPoliticalStrength:
      case ECharacteristicToHighlight::CountryExceedingResources:
      case ECharacteristicToHighlight::CountryMilitaryStrength:
      case ECharacteristicToHighlight::CountryNuclearStrength:
      case ECharacteristicToHighlight::CountryHumanDevelopment:
      case ECharacteristicToHighlight::CountryRelations:
      case ECharacteristicToHighlight::HumanControlledCountries:
      {
         l_eThematicMapLegendToDisplay = EThematicMapLegendType::HighLow;
         l_RegionResults.resize(g_ClientDAL.NbRegion());      			
         for (INT32 i = 1; i <= l_iNbRegions; i++)
            l_RegionResults[i - 1] = l_vResults[l_vControls[i].m_iPolitical - 1];

			if(l_pEvent->Characteristic == ECharacteristicToHighlight::HumanControlledCountries)
			{
				l_eThematicMapLegendToDisplay = EThematicMapLegendType::HumanControlled;
			}
         break;
         
      }
   }

   //If the characteristic is higher than the region resource production
   //We assume the thematic map is a Resource Production thematic map
   if(l_pEvent->Characteristic > ECharacteristicToHighlight::RegionResourcesProduction)
   {
      l_eThematicMapLegendToDisplay = EThematicMapLegendType::HighLow;
      l_RegionResults               = l_pEvent->Results;
      l_bSendAdditionalResultsOnly  = true;
   }	

   //If the paint map is for the country relations, painting is different 
   //than the rest
   if (l_pEvent->Characteristic == ECharacteristicToHighlight::CountryRelations)
   {
      l_eThematicMapLegendToDisplay = EThematicMapLegendType::GoodBad;

      static const GColorRGBReal l_Beige  (0.95f, 0.886f, 0.729f, 1.0f);
      static const GColorRGBReal l_Red    (0.80f, 0.000f, 0.000f, 1.0f);
      static const GColorRGBReal l_Green  (0.32f, 0.804f, 0.268f, 1.0f);
      static const GColorRGBReal l_Yellow (1.0f,  1.0f,   0.5f,   1.0f);      
      for (UINT32 i = 0; i < l_RegionResults.size(); i++)
      {         
         if (l_vControls[i + 1].m_iPolitical == g_ClientDCL.SelectedCountryID())
            g_ClientDCL.ThematicMapColorData[i+1] = l_Beige;
         else
         {
            REAL32 l_Value = (REAL32)l_RegionResults[i];
            l_Value = max(-1.0f, min(1.0f, l_Value));

            if (l_Value >= 0)
               g_ClientDCL.ThematicMapColorData[i+1] = (l_Yellow * (1 - l_Value) + l_Green * l_Value);
            else
               g_ClientDCL.ThematicMapColorData[i+1] = (l_Yellow * (1 + l_Value) - l_Red * l_Value);
         }         
      }
   }
   //Paint using the country standard colors
   else if(l_pEvent->Characteristic != ECharacteristicToHighlight::CountryEmpire)
   {
      for (UINT32 i = 0; i < l_RegionResults.size(); i++)
      {
         REAL32 l_Value = (REAL32)l_RegionResults[i];
         if (l_Value >= 0)
         {
            l_Value = min(1.0f, l_Value);
            g_ClientDCL.ThematicMapColorData[i+1] = GColorRGBReal(SP2_PAINT_MAP_LOW_VALUE_COLOR * (1 - l_Value) + SP2_PAINT_MAP_HIGH_VALUE_COLOR * l_Value);            
         }
         else
            g_ClientDCL.ThematicMapColorData[i+1] = GColorRGBReal(0,0,0,0);
      }
   }
   else if(l_pEvent->Characteristic == ECharacteristicToHighlight::HumanControlledCountries ||
		     l_pEvent->Characteristic == ECharacteristicToHighlight::PoliticalControl)
   {
      l_eThematicMapLegendToDisplay = EThematicMapLegendType::Unknown;
   }

   //Highlight the thematic map
   {      
      g_ClientDCL.CurrentThematicMap = l_pEvent->Characteristic;
      g_ClientDCL.HighlightCountry((void*)g_ClientDCL.SelectedCountryID());
   }

   //Display the thematic map legend
   if(l_eThematicMapLegendToDisplay != EThematicMapLegendType::Unknown)
   {
      g_ClientDDL.ThematicMapLegendSpawn();
      g_ClientDDL.ThematicMapLegend()->Show();

      if(l_bSendAdditionalResultsOnly)
      {
         g_ClientDDL.ThematicMapLegend()->Update(l_eThematicMapLegendToDisplay,
                                                l_pEvent->Characteristic,
                                                l_pEvent->m_vAdditionalResults);
      }
      else
      {
         g_ClientDDL.ThematicMapLegend()->Update(l_eThematicMapLegendToDisplay,
                                                l_pEvent->Characteristic,
                                                l_pEvent->Results);
      }
   }

   return true;
}


bool SP2::GGeneralEventHandler::HandleGetPlayersList(SDK::GGameEventSPtr in_Event)
{
   GGetPlayersList & gpl = *(GGetPlayersList *)in_Event.get();
   
   vector<GChat::GChatPeer> peerVec;

   if(g_Joshua.AdminPlayerID() != gpl.AdminPlayerID())
   {
      // Set admin ID
      g_Joshua.AdminPlayerID(gpl.AdminPlayerID());
      // are we the admin now ?
      if(g_Joshua.AdminPlayerID() == g_SP2Client->Id() && g_ClientDDL.MainBar())
      {
         g_ClientDDL.MainBar()->SetAdminView();
      }
   }

   GChat::GChatPeer peer;
   for(vector<GGetPlayersList::Player>::iterator itr = gpl.PlayersList.begin(); itr < gpl.PlayersList.end(); itr++)
   {      
      peer.ClientID = itr->ClientID;
      peer.CountryID = itr->CountryID;
      peer.PlayerName = itr->PlayerName;
      peer.PlayerStatus = itr->PlayerStatus;

      peerVec.push_back(peer);
      if(peer.ClientID == g_Joshua.Client()->Id() )
      {
         //g_ClientDAL.PlayerName(itr->PlayerName);
      }
   }   

   if (((SP2::GClient*)g_Joshua.Client())->Chat())
      if(g_ClientDAL.NbCountry() >0)
         ((SP2::GClient*)g_Joshua.Client())->Chat()->PlayersList(peerVec);
   return true;
}

void SP2::GGeneralEventHandler::HandleReceiveCountryList(SDK::GGameEventSPtr in_Event)
{
   g_ClientDCL.ReceiveCountryListFromServer( (Event::GReceiveCountryList*) in_Event.get() );
}

void SP2::GGeneralEventHandler::HandleReceiveCountryStaticData(SDK::GGameEventSPtr in_Event)
{
   g_ClientDCL.ReceiveStaticCountryDataFromServer( (Event::GReceiveCountryStaticData*) in_Event.get() );
}

void SP2::GGeneralEventHandler::HandleErrorCountryStaticData(SDK::GGameEventSPtr)
{
   g_ClientDCL.ReceiveStaticCountryDataError();
}

void SP2::GGeneralEventHandler::HandleCountryRanksResult(SDK::GGameEventSPtr in_Event)
{
   GGetCountryRanks* l_pEvent = (GGetCountryRanks*)in_Event.get();
   INT16 l_iPoliticRank = *(l_pEvent->PoliticRank());
   INT16 l_iEconomicRank = *(l_pEvent->EconomicRank());

   g_ClientDCL.HandleGetCountryRanks(l_iEconomicRank,l_iPoliticRank);
}

void SP2::GGeneralEventHandler::HandleCountriesRanksResult(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GGetCountriesRanks* l_pEvent = (SP2::Event::GGetCountriesRanks*)in_Event.get();

   if(g_ClientDDL.StartSinglePlayerWindow() && g_ClientDDL.StartSinglePlayerWindow()->Visible())
   {
      g_ClientDDL.StartSinglePlayerWindow()->SetCountryRanks(l_pEvent->m_viMilitaryRank, l_pEvent->m_viPoliticRank, l_pEvent->m_viEconomicRank, l_pEvent->m_viCountryID);
   }
   else if(g_ClientDDL.GameLobby() && g_ClientDDL.GameLobby()->Visible())
   {
      g_ClientDDL.GameLobby()->SetCountryRanks(l_pEvent->m_viMilitaryRank, l_pEvent->m_viPoliticRank, l_pEvent->m_viEconomicRank, l_pEvent->m_viCountryID);
   }
}

void SP2::GGeneralEventHandler::HandleStrategicWarfareNotify(SDK::GGameEventSPtr in_Event)
{
   if(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
      return;

   static INT32 NukeID = 0;
   GStrategicWarfareNotify * evt = (GStrategicWarfareNotify *)in_Event.get();

   // Play a sound , if some nukes
   bool l_bPlayWarningSound = false;
   
   for(UINT32 i = 0; i < evt->m_vLaunchedNukes.size() ; i++)
   {
      g_ClientDDL.BackgroundEarth()->DisplayNuke(evt->m_vLaunchedNukes[i] );
 
      //If the missile belong to our country, remove the missile from our missile list
      if(evt->m_vLaunchedNukes[i].m_Missile.m_iOwnerID == (UINT32)g_ClientDAL.ControlledCountryID())
      {
         g_ClientDAL.m_PlayerCountryData.RemoveMissileClientSide(evt->m_vLaunchedNukes[i].m_Missile);
      }
      else
      {
         l_bPlayWarningSound = true;
      }
   }

   if(l_bPlayWarningSound)
   {
      if(g_SP2Client->SoundBox()->GetTimeForDelayedSound(ESoundEventID::NuclearWarning,ESoundContext::Center) < (1.5f  + c_fNukeLaunchDelaySeconds))
      {
         g_SP2Client->PlaySoundWithDelay(ESoundEventID::NuclearWarning,0     + c_fNukeLaunchDelaySeconds,ESoundContext::Center);
         g_SP2Client->PlaySoundWithDelay(ESoundEventID::NuclearWarning,0.75f + c_fNukeLaunchDelaySeconds,ESoundContext::Center);
         g_SP2Client->PlaySoundWithDelay(ESoundEventID::NuclearWarning,1.5f  + c_fNukeLaunchDelaySeconds,ESoundContext::Center);
      }
   }

   // Change theme
   g_SP2Client->ImmediatlyChangeMusicalTheme(AdaptiveMusic::ESP2MusicalTheme::Nuclear);
}

/*!
 * Handles the event that sends initial data from the server to the client
 **/
void SP2::GGeneralEventHandler::HandleSendData(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GSendData* l_pSendData = (SP2::Event::GSendData*)in_Event.get();

   g_ClientDAL.ReceiveInitialData(l_pSendData);
}

/*!
 * Handles the event that sends updated data from the server to the client
 **/
void SP2::GGeneralEventHandler::HandleUpdateData(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GUpdateData* l_pUpdateData = (SP2::Event::GUpdateData*)in_Event.get();
   UINT32 l_iSelcountry = (UINT32) g_ClientDCL.SelectedCountryID();
   bool l_bUpdateSel = false;
   bool l_bMustUpdateWindow = false;

	const vector<GRegionControl>& l_vControls = g_ClientDAL.RegionControlArray();

   // Apply military control changes
   const vector<pair<UINT32, UINT32> >& l_vRegionMilitaryChanges = l_pUpdateData->m_vRegionMilitaryChanges;
   vector<pair<UINT32, UINT32> >::const_iterator l_It = l_vRegionMilitaryChanges.begin();
   while(l_It != l_vRegionMilitaryChanges.end() )
   {
      // Verify if region change concerns the currently selected country
      if( (l_vControls[l_It->first].m_iMilitary == l_iSelcountry) ||
          (l_It->second == l_iSelcountry) )
      {
         l_bUpdateSel = true;
         l_bMustUpdateWindow = true;
      }

      g_ClientDAL.ChangeMilitaryControl(l_It->first, l_It->second);
      l_It++;
   }

   // Apply political control changes
   const vector<pair<UINT32, UINT32> >& l_vRegionPoliticalChanges = l_pUpdateData->m_vRegionPoliticalChanges;
   l_It = l_vRegionPoliticalChanges.begin();
   while(l_It != l_vRegionPoliticalChanges.end() )
   {
      // Verify if region change concerns the currently selected country
      if( (l_vControls[l_It->first].m_iPolitical == l_iSelcountry) ||
          (l_It->second == l_iSelcountry) )
      {
         l_bUpdateSel = true;
      }

      g_ClientDAL.ChangePoliticalControl(l_It->first, l_It->second);
   
   l_It++;
   }

   // Apply city changes
   const vector<SCityInfo>& l_vCityChanges = l_pUpdateData->m_vCityChanges;
   vector<SCityInfo>::const_iterator l_CityIt = l_vCityChanges.begin();
   while(l_CityIt != l_vCityChanges.end() )
   {
      g_ClientDAL.UpdateCity(*l_CityIt);
      ++l_CityIt;
   }

   // Apply capital changes
   const vector<pair<UINT32, UINT32> >& l_vCapitalChanges = l_pUpdateData->m_vCapitalChanges;
   l_It = l_vCapitalChanges.begin();
   while(l_It != l_vCapitalChanges.end() )
   {
      if(l_It->first == l_iSelcountry)
         l_bMustUpdateWindow = true;

      g_ClientDAL.ChangeCapitalID(l_It->first, l_It->second);
      ++l_It;
   }

   // Apply region annex changes
   const vector<GRegionAnnex>& l_vAnnexChanges = l_pUpdateData->m_vAnnexChanges;
   vector<GRegionAnnex>::const_iterator l_AnnexIt = l_vAnnexChanges.begin();
   while(l_AnnexIt != l_vAnnexChanges.end() )
   {
      // Verify if region annex change concerns the currently selected country
      if( (l_vControls[l_AnnexIt->m_iAnnexedRegion].m_iMilitary == l_iSelcountry) ||
          (l_vControls[l_AnnexIt->m_iAnnexedRegion].m_iPolitical == l_iSelcountry) )
      {
         l_bUpdateSel = true;
      }

      g_ClientDAL.UpdateRegionAnnex(*l_AnnexIt);
      ++l_AnnexIt;
   }

   // Apply region annex removed
   const vector<UINT32>& l_vAnnexRemoved = l_pUpdateData->m_vAnnexRemoved;
   vector<UINT32>::const_iterator l_AnnexDelIt = l_vAnnexRemoved.begin();
   while(l_AnnexDelIt != l_vAnnexRemoved.end() )
   {
      // Verify if region annex remove concerns the currently selected country
      if( (l_vControls[*l_AnnexDelIt].m_iMilitary == l_iSelcountry) ||
          (l_vControls[*l_AnnexDelIt].m_iPolitical == l_iSelcountry) )
      {
         l_bUpdateSel = true;
      }

      g_ClientDAL.RemoveRegionAnnex(*l_AnnexDelIt);
      ++l_AnnexDelIt;
   }

   // Apply war declarations
   const vector<SWarPair>& l_vWarList = l_pUpdateData->m_vWarList;
   vector<SWarPair>::const_iterator l_WarIt = l_vWarList.begin();
   while(l_WarIt != l_vWarList.end() )
   {
      // Verify if war declaration concerns the currently selected country
      if( (l_WarIt->m_iFirstCountry == l_iSelcountry) ||
          (l_WarIt->m_iSecondCountry == l_iSelcountry) )
      {
         l_bUpdateSel = true;
      }

      g_ClientDAL.AddWarPair(*l_WarIt);
      ++ l_WarIt;
   }

   // Apply peace declarations
   const vector<SWarPair>& l_vPeaceList = l_pUpdateData->m_vPeaceList;
   l_WarIt = l_vPeaceList.begin();
   while(l_WarIt != l_vPeaceList.end() )
   {
      // Verify if war declaration concerns the currently selected country
      if( (l_WarIt->m_iFirstCountry == l_iSelcountry) ||
          (l_WarIt->m_iSecondCountry == l_iSelcountry) )
      {
         l_bUpdateSel = true;
      }

      g_ClientDAL.RemoveWarPair(*l_WarIt);
      ++ l_WarIt;
   }

   if(!l_vPeaceList.empty() || !l_vWarList.empty() )
   {
      g_ClientDAL.UpdateWarStatus();
   }

	//Apply war changes
	const vector<GWar>& l_vCurrentWars = l_pUpdateData->m_vWarChanges;
	vector<GWar>::const_iterator l_CurWarIt = l_vCurrentWars.begin();
   while(l_CurWarIt != l_vCurrentWars.end() )
	{
		g_ClientDAL.AddWar(*l_CurWarIt);
      g_ClientDAL.NotifyWarChanged((*l_CurWarIt).ID());
		++l_CurWarIt;
	}

	//Apply wars that are gone now
	const set<UINT32>& l_WarsToRemove = l_pUpdateData->m_WarsToRemove;
	set<UINT32>::const_iterator l_WarRemoveIt = l_WarsToRemove.begin();
   while(l_WarRemoveIt != l_WarsToRemove.end() )
	{
		g_ClientDAL.RemoveWar(*l_WarRemoveIt);
      g_ClientDAL.NotifyWarChanged(*l_WarRemoveIt);
		++l_WarRemoveIt;
	}

   // Re-select country if needed to update the display
   if( (l_bUpdateSel) && (l_iSelcountry > 0) && (l_iSelcountry <= g_ClientDAL.NbCountry() ) )
   {
      const GCountry& l_Country = g_ClientDAL.Country(l_iSelcountry);
      g_ClientDCL.SelectedCountrySet(l_Country, true);
   }

   if(l_bMustUpdateWindow)
   {
      g_ClientDAL.NotifyMilitaryControlChanged();
   }
}

void SP2::GGeneralEventHandler::HandleUpdateWar(SDK::GGameEventSPtr in_Event)
{
   if(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
      return;

   SP2::Event::GUpdateWar* l_pUpdateEvent = ( (SP2::Event::GUpdateWar*) in_Event.get() );

   if(l_pUpdateEvent->m_iRemovedWar != 0xFFFFFFFF)
   {
		g_ClientDAL.RemoveWar(l_pUpdateEvent->m_iRemovedWar);
      g_ClientDAL.NotifyWarChanged(l_pUpdateEvent->m_iRemovedWar);
	}
	else
	{
		g_ClientDAL.AddWar(l_pUpdateEvent->m_ModifiedWar);
      g_ClientDAL.NotifyWarChanged(l_pUpdateEvent->m_ModifiedWar.ID());
	}
}

void SP2::GGeneralEventHandler::HandleUpdateTreaty(SDK::GGameEventSPtr in_Event)
{
   if(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
      return;

   SP2::Event::GUpdateTreaty* l_pUpdateEvent = ( (SP2::Event::GUpdateTreaty*) in_Event.get() );

   if(l_pUpdateEvent->m_iRemovedTreaty != 0xFFFFFFFF)
   {
      if(g_ClientDDL.TreatyDetailsWindow() && g_ClientDDL.TreatyDetailsWindow()->Visible())
      {
         if(g_ClientDDL.TreatyDetailsWindow()->DisplayedTreatyID() == (INT32)l_pUpdateEvent->m_iRemovedTreaty)
         {
            GTreaty* l_pTreaty = g_ClientDDL.TreatyDetailsWindow()->DisplayedTreaty();
            if(l_pTreaty)
            {
               if(l_pTreaty->Ponctual())
               {
						if(l_pTreaty->Type() == ETreatyType::WeaponTradeEmbargo)
							g_ClientDAL.UpdateWeaponTradeEmbargos();

                  if(!g_ClientDDL.GenericMessageWindow())
                     g_ClientDDL.GenericMessageWindowSpawn();

                  if(l_pTreaty->MembersOriginalsA().size() == l_pTreaty->MembersSideA(true).size() && l_pTreaty->MembersOriginalsB().size() == l_pTreaty->MembersSideB(true).size())
                  {
                     g_ClientDDL.GenericMessageWindow()->Show(g_ClientDAL.GetString(EStrId::TreatyIsAccepted), g_ClientDDL.TreatyDetailsWindow());
                     g_ClientDDL.TreatyDetailsWindow()->m_bWaitForClose = true;
                  }
                  else
                  {
                     g_ClientDDL.GenericMessageWindow()->Show(g_ClientDAL.GetString(EStrId::TreatyHasBeenRemoved), g_ClientDDL.TreatyDetailsWindow());
                     g_ClientDDL.TreatyDetailsWindow()->m_bWaitForClose = true;
                  }
               }
            }
         }
      }
      
      //Try to find the mail that will be removed in the mail window
      hash_map<UINT32, INT32>::iterator l_It = g_ClientDAL.m_TreatiesMail.find(l_pUpdateEvent->m_iRemovedTreaty);
      if( l_It != g_ClientDAL.m_TreatiesMail.end() )
      {
         if( g_ClientDDL.MailWindow()->MailExists( l_It->second ) )
            g_ClientDDL.MailWindow()->RemoveMail( l_It->second );

      }

      GTreatyNotifyData l_Data;
      l_Data.m_iTreatyID = l_pUpdateEvent->m_iRemovedTreaty;
      l_Data.m_bToRemove = true;
      
      g_ClientDAL.NotifyTreatyChanged(l_Data);
      g_ClientDAL.RemoveTreaty(l_pUpdateEvent->m_iRemovedTreaty);
   }
   else
   {
      GTreatyNotifyData l_Data;
      l_Data.m_iTreatyID = l_pUpdateEvent->m_ModifiedTreaty.ID();
      l_Data.m_bToRemove = false;

		if(l_pUpdateEvent->m_ModifiedTreaty.Type() == ETreatyType::WeaponTradeEmbargo)
			g_ClientDAL.UpdateWeaponTradeEmbargos();

      g_ClientDAL.AddTreaty(l_pUpdateEvent->m_ModifiedTreaty);
      g_ClientDAL.NotifyTreatyChanged(l_Data);
   }
}

void SP2::GGeneralEventHandler::HandleUpdateUnitValue(SDK::GGameEventSPtr in_Event)
{
   if(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
      return;

   SP2::Event::GUpdateUnitValue* l_pUpdateUnits = (SP2::Event::GUpdateUnitValue*)in_Event.get();

   for(hash_set<UINT32>::const_iterator l_It = l_pUpdateUnits->m_DirtyCountryUnitValue.begin();
       l_It != l_pUpdateUnits->m_DirtyCountryUnitValue.end(); ++ l_It)
   {
      g_ClientDAL.DirtyCountryUnits(*l_It);
   }
}

void SP2::GGeneralEventHandler::HandleConquerCountry(SDK::GGameEventSPtr in_Event)
{   
   if(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
      return;

   SP2::Event::GConquerCountry* l_pConquerEvent = ( (SP2::Event::GConquerCountry*) in_Event.get() );

   const GCountry& l_CountryInfoConquered = g_ClientDAL.Country(l_pConquerEvent->m_iConqeredID);   

   g_ClientDAL.CountryActivated(l_pConquerEvent->m_iConqeredID, false);
   
	if(l_pConquerEvent->m_iConqueringID)
	{
      const GCountry& l_CountryInfoConqueror = g_ClientDAL.Country(l_pConquerEvent->m_iConqueringID);
		#ifdef GOLEM_DEBUG
			g_Joshua.Log(l_CountryInfoConquered.Name() + GString(" was conquered by ") + l_CountryInfoConqueror.Name() );
		#endif
	}

   g_ClientDAL.DestroyCountryEntity(l_pConquerEvent->m_iConqeredID);

   if(g_ClientDDL.TreatiesWindow() && g_ClientDDL.TreatiesWindow()->Visible())
      g_ClientDDL.TreatiesWindow()->FillCountries();
}

/*!
 * Handles the event when the client receives the client country data
 **/
void SP2::GGeneralEventHandler::HandleSynchronizeCountryData(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GSynchronizeCountryData* l_pEvt = (SP2::Event::GSynchronizeCountryData*)in_Event.get();


   //Update the country data with the received information
   g_ClientDAL.UpdatePlayerCountryData( l_pEvt->CountryData() );
   g_ClientDAL.m_PlayerCountryData.ReceivedAtLeastOneSync(true);


   return;
}

void SP2::GGeneralEventHandler::HandleAdvisorPlan(SDK::GGameEventSPtr in_Event)
{
   if(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
      return;

   SP2::Event::GSendPlan* l_pEvent = (SP2::Event::GSendPlan*)in_Event.get();

   SP2::GAdvisorPlan l_Plan;
   l_Plan.m_ObjectiveID = l_pEvent->m_iObjectiveID;
   l_Plan.m_Type = l_pEvent->m_TypeOfPlan;

   for(UINT32 i=0; i<l_pEvent->m_vActionsID.size(); i++)
   {
      l_Plan.m_Actions.push_back(pair<UINT32, UINT32>(l_pEvent->m_vActionsID[i].first, l_pEvent->m_vActionsID[i].second));
   }

   for(UINT32 i=0; i<l_pEvent->m_vObjectiveChanges.size(); i++)
   {
      l_Plan.m_Results.push_back(pair<UINT32, bool>(l_pEvent->m_vObjectiveChanges[i].first, l_pEvent->m_vObjectiveChanges[i].second));
   }

   g_ClientDDL.AdvisorWindow()->Plan(l_Plan);
}

void SP2::GGeneralEventHandler::HandleNews(SDK::GGameEventSPtr in_Event)
{
   if(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
      return;

   SP2::Event::GSendNews* l_pNews = ( (SP2::Event::GSendNews*) in_Event.get() );   
	g_ClientDAL.InsertPendingNews(l_pNews->m_News);	
}

GString SP2::GGeneralEventHandler::ReplaceNextPattern(GString in_sSentence, GString in_ReplacingString, GString in_sPattern) const
{
	UINT32 l_iCurrentPosition = 0;
	UINT32 l_iStartPosition = 0;

	bool l_bSuccess = false;

	for(UINT32 i=0; i<in_sSentence.size(); i++)
	{
		if(in_sSentence.at(i) == in_sPattern.at(l_iCurrentPosition))
		{
			if(l_iCurrentPosition == 0)
				l_iStartPosition = i;			
			l_iCurrentPosition++;
			if(l_iCurrentPosition == in_sPattern.size())
			{
				l_bSuccess = true;
				break;
			}
		}
		else
		{
			if(l_iCurrentPosition > 0)
			{
				l_iCurrentPosition = 0;
				i = l_iStartPosition + 1;
			}
		}			
	}
	if(l_bSuccess)
	{
		GString l_Results;
		l_Results += in_sSentence.substr(0,l_iStartPosition);
		l_Results += in_ReplacingString;
		l_Results += in_sSentence.substr(l_iStartPosition+in_sPattern.size(),in_sSentence.size());
		return l_Results;
	}
	return L"";
}

void DisplayObjectives(void *in_pData)
{
   if (g_ClientDDL.ObjectivesWindow() == NULL)
   {
      g_ClientDDL.ObjectivesWindowSpawn();
   }
   g_ClientDDL.ObjectivesWindow()->Show();
   g_ClientDDL.ObjectivesWindow()->UpdateSelectedScenario();
}

void SP2::GGeneralEventHandler::HandleObjectiveCompleted(SDK::GGameEventSPtr in_Event)
{
   if(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
      return;

   SP2::Event::GObjectiveCompleted* l_pObjectiveCompleted = (SP2::Event::GObjectiveCompleted*)in_Event.get();

   // steam achievement
   if(l_pObjectiveCompleted->m_iCompletedObjectivesID >= 10000)
   {
      g_Joshua.PassSteamAchievement(l_pObjectiveCompleted->m_iCompletedObjectivesID - 10000);
      return;
   }

   g_ClientDAL.Scenario()->CompletedObjective(l_pObjectiveCompleted->m_iCompletedObjectivesID,l_pObjectiveCompleted->m_eStatus);

   if(g_ClientDDL.ObjectivesWindow() && g_ClientDDL.ObjectivesWindow()->Visible())
      g_ClientDDL.ObjectivesWindow()->UpdateSelectedScenario();

   // Find Coresponding objective
   if(!g_ClientDAL.Scenario()->IsComplete())
   {
      switch(l_pObjectiveCompleted->m_eStatus)
      {
         case EObjectiveStatus::Reached:
         case EObjectiveStatus::AlwaysReached:
			 g_ClientDDL.MailWindow()->AddMail(g_ClientDAL.GetString(102364),DisplayObjectives,NULL);
            break;
         case EObjectiveStatus::NotReached:
         case EObjectiveStatus::TimedOut:
            g_ClientDDL.MailWindow()->AddMail(g_ClientDAL.GetString(102365) ,DisplayObjectives,NULL);
            break;
         default:
            gassert(0,"Unhandled case");
            break;
      }
   }
   else
   {
      // Game completed
      if(g_ClientDDL.EndGameWindow() == NULL)
         g_ClientDDL.EndGameWindowSpawn();
      g_ClientDDL.EndGameWindow()->ScenarioCompleted();
   }


   switch(l_pObjectiveCompleted->m_eStatus)
   {
      case EObjectiveStatus::Reached:
      case EObjectiveStatus::AlwaysReached:
         g_SP2Client->ImmediatlyChangeMusicalTheme(AdaptiveMusic::ESP2MusicalTheme::Winning);
         break;
      case EObjectiveStatus::NotReached:
      case EObjectiveStatus::TimedOut:
         g_SP2Client->ImmediatlyChangeMusicalTheme(AdaptiveMusic::ESP2MusicalTheme::Loosing);
         break;
      default:
         gassert(0,"Unhandled case");
         break;
   }
}


/*!
 * Handle the event when the game is over
 **/
void SP2::GGeneralEventHandler::HandleGameOver(SDK::GGameEventSPtr in_Event)
{
   if(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
      return;

   //Perform something wether the game is won, objectives timed out or game is simply over
   SP2::Event::GGameOver* l_pGameOver = (SP2::Event::GGameOver*)in_Event.get();
   
   if(l_pGameOver->m_vGameTimedOutObjectives.size())
   {
      if(g_ClientDDL.EndGameWindow() == NULL)
         g_ClientDDL.EndGameWindowSpawn();
      
      g_ClientDDL.EndGameWindow()->GameOver(l_pGameOver); 
   }
   else
   {
      //If game is over for us (without possibility to continue)
      if(l_pGameOver->m_iTerminatedCountry == (UINT32)g_ClientDAL.ControlledCountryID())
      {
         if(g_ClientDDL.EndGameWindow() == NULL)
            g_ClientDDL.EndGameWindowSpawn();
         g_ClientDDL.EndGameWindow()->GameOver(l_pGameOver);

         g_ClientDAL.GameIsOver(true);

      }
      else 
      {
         if(l_pGameOver->m_iWinnerCountry == (UINT32)g_ClientDAL.ControlledCountryID())
         {
            if(g_ClientDDL.EndGameWindow() == NULL)
               g_ClientDDL.EndGameWindowSpawn();
            g_ClientDDL.EndGameWindow()->ScenarioCompleted();
         }
         /*
         else
         {
            //If we are the admin player, ask to continue the game
            if(g_Joshua.Client()->Id() == g_Joshua.AdminPlayerID())
            {

               //Set the new game objectives
               SDK::GGameEventSPtr l_SetObjectivesEvt = CREATE_GAME_EVENT(SP2::Event::GSetGameObjectives);
               l_SetObjectivesEvt->m_iSource = g_Joshua.Client()->Id();
               l_SetObjectivesEvt->m_iTarget = SDK::Event::ESpecialTargets::Server;
               g_Joshua.RaiseEvent(l_SetObjectivesEvt);
               //todo mtremblay: logically set the game objectives
               //actually, this sets the game as no objectives

               //Possibility to continue the game
               //todo mtremblay: logically determine if game must be continued or not
               l_pGameOver->m_bWillContinue = true;
               l_pGameOver->m_vCompletedObjectives.clear();
               l_pGameOver->m_vGameOverObjectives.clear();
               l_pGameOver->m_vGameTimedOutObjectives.clear();
               l_pGameOver->m_iSource = g_Joshua.Client()->Id();
               l_pGameOver->m_iTarget = SDK::Event::ESpecialTargets::Server;
               g_Joshua.RaiseEvent(in_Event);
            }
         }
         */
      }
   }



}

void SP2::GGeneralEventHandler::HandleHistoryMarkers(SDK::GGameEventSPtr in_Event)
{
   if(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
      return;

   SP2::Event::GHistoryMarkers* l_pHistoryMarkers = (SP2::Event::GHistoryMarkers*)in_Event.get();

   g_ClientDAL.HistoryMarkersUpdate(l_pHistoryMarkers);
}

void SP2::GGeneralEventHandler::HandleCountryInfo(SDK::GGameEventSPtr in_Event)
{
   if(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
      return;

   SP2::Event::GEventCountryInfo* l_pUpdate = (SP2::Event::GEventCountryInfo*)in_Event.get();

   g_ClientDAL.m_DataCountryInformationWindow.m_iCountryID           = l_pUpdate->m_iCountryID;
   g_ClientDAL.m_DataCountryInformationWindow.m_fArableLand          = l_pUpdate->m_fArableLand;
   g_ClientDAL.m_DataCountryInformationWindow.m_fForestLand          = l_pUpdate->m_fForestLand;
   g_ClientDAL.m_DataCountryInformationWindow.m_fParksLand           = l_pUpdate->m_fParksLand;
   g_ClientDAL.m_DataCountryInformationWindow.m_fUnusableLand        = l_pUpdate->m_fUnusableLand;

   g_ClientDAL.m_DataCountryInformationWindow.m_iPop15               = l_pUpdate->m_iPop15;
   g_ClientDAL.m_DataCountryInformationWindow.m_iPop1565             = l_pUpdate->m_iPop1565;
   g_ClientDAL.m_DataCountryInformationWindow.m_iPop65               = l_pUpdate->m_iPop65;

   g_ClientDAL.m_DataCountryInformationWindow.m_fLandArea            = l_pUpdate->m_fLandArea;
   g_ClientDAL.m_DataCountryInformationWindow.m_fWaterArea           = l_pUpdate->m_fWaterArea;
   g_ClientDAL.m_DataCountryInformationWindow.m_fTotalArea           = l_pUpdate->m_fTotalArea;

   g_ClientDAL.m_DataCountryInformationWindow.m_fHumanDev            = l_pUpdate->m_fHumanDev;
   g_ClientDAL.m_DataCountryInformationWindow.m_fHumanDevAverage     = l_pUpdate->m_fHumanDevAverage;

   g_ClientDAL.m_DataCountryInformationWindow.m_iClimateStid         = l_pUpdate->m_iClimateStid;

   g_ClientDAL.m_DataCountryInformationWindow.m_fBirthRate           = l_pUpdate->m_fBirthRate;
   g_ClientDAL.m_DataCountryInformationWindow.m_fDeathRate           = l_pUpdate->m_fDeathRate;
   g_ClientDAL.m_DataCountryInformationWindow.m_fTelecommunications  = l_pUpdate->m_fTelecommunications;
   g_ClientDAL.m_DataCountryInformationWindow.m_fInfrastructure      = l_pUpdate->m_fInfrastructure;

   if( g_ClientDDL.CountryInformationWindow() && 
       g_ClientDDL.CountryInformationWindow()->Visible() && 
       (g_ClientDCL.SelectedCountryID() == l_pUpdate->m_iCountryID) 
     )
      g_ClientDDL.CountryInformationWindow()->Update();
}

void SP2::GGeneralEventHandler::HandleRelationsCountries(SDK::GGameEventSPtr in_Event)
{
	SP2::Event::GEventFetchCountriesRelations* l_pSendData = (SP2::Event::GEventFetchCountriesRelations*)in_Event.get();

   g_ClientDAL.SetRelations(l_pSendData);
}

/*!
 * Handles the reception of relation history date
 **/
void SP2::GGeneralEventHandler::HandleRelationHistoryReception(SDK::GGameEventSPtr in_Event)
{
   if(g_SP2Client->CurrentFSMState() != FSM::EStates::Playing)
      return;

   SP2::Event::GRelationHistoryFetch* l_pEvt = (SP2::Event::GRelationHistoryFetch*)in_Event.get();
   //Reception of relation history data
   if(g_ClientDDL.RelationsWindow() && g_ClientDDL.RelationsWindow()->Visible())
   {
      //Reverse the history relation so they are displayed correctly in the time
      reverse(l_pEvt->m_vRelationsHistoryValues.begin(),l_pEvt->m_vRelationsHistoryValues.end());
      g_ClientDDL.RelationsWindow()->SetHistoryRelation(l_pEvt->m_vRelationsHistoryValues);
   }
}

void SP2::GGeneralEventHandler::HandleGetCountryParties(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GGetCountryParties* l_pEvt = (SP2::Event::GGetCountryParties*)in_Event.get();

   if(g_ClientDDL.StartSinglePlayerWindow() && g_ClientDDL.StartSinglePlayerWindow()->Visible())
   {
      g_ClientDDL.StartSinglePlayerWindow()->SetPoliticalParties( l_pEvt->m_vPartyList );
   }
   else if(g_ClientDDL.GameLobby() && g_ClientDDL.GameLobby()->Visible())
   {
      g_ClientDDL.GameLobby()->SetPoliticalParties( l_pEvt->m_vPartyList );
   }
}

void SP2::GGeneralEventHandler::HandleAcknowledge(SDK::GGameEventSPtr)// in_Event)
{
//   SP2::Event::GAcknowledge* l_pEvent = (SP2::Event::GAcknowledge*)in_Event.get();
}

void SP2::GGeneralEventHandler::HandleProgress(SDK::GGameEventSPtr)// in_Event)
{
//   SP2::Event::GProgress* l_pEvent = (SP2::Event::GProgress*)in_Event.get();
}

void SP2::GGeneralEventHandler::HandleResetGame(SDK::GGameEventSPtr)
{
   g_ClientDCL.m_bServerIsReset = true;
}

void SP2::GGeneralEventHandler::HandleNewComplete(SDK::GGameEventSPtr in_Event)
{
   g_ClientDCL.FinalizeGameNew( ( (SP2::Event::GNewComplete*) in_Event.get() )->m_eResult);
}

void SP2::GGeneralEventHandler::HandleLoadComplete(SDK::GGameEventSPtr in_Event)
{
   g_ClientDCL.FinalizeGameLoad( ( (SP2::Event::GLoadComplete*) in_Event.get() )->m_eResult);
}

void SP2::GGeneralEventHandler::HandleSaveComplete(SDK::GGameEventSPtr in_Event)
{
   g_ClientDCL.FinalizeGameSave( ( (SP2::Event::GSaveComplete*) in_Event.get() )->m_eResult);
}


void SP2::GGeneralEventHandler::HandleSetGameObjectives(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GSetGameObjectives* l_pEvt = (SP2::Event::GSetGameObjectives*)in_Event.get();
   gassert(g_ClientDAL.Scenario().get(),"Scenario should exist");

   g_ClientDCL.m_vGameObjectives.clear();
   //dont add the negative game objective
   for(UINT32 i = 0 ; i < l_pEvt->m_vObjectives.size(); i++)
   {
      if(l_pEvt->m_vObjectives[i].m_eObjectiveType > EObjectiveType::FirstPositiveObjective)
      {
         // Add title to those guys comming from the server
         GGameObjective *l_pGameObjective = &l_pEvt->m_vObjectives[i];
         // JMercier Note : Assuming they are in order...
         if(l_pGameObjective->m_eObjectiveType  == EObjectiveType::AchieveWorldPeace)
            l_pGameObjective->m_Title = GString(GString(100560));
         if(l_pGameObjective->m_eObjectiveType  == EObjectiveType::MeetResourcesNeeds)
            l_pGameObjective->m_Title = GString(GString(100561));
         if(l_pGameObjective->m_eObjectiveType  == EObjectiveType::ConquerTheWorld)
            l_pGameObjective->m_Title = GString(GString(100562));
         if(l_pGameObjective->m_eObjectiveType  == EObjectiveType::DevelopNation)
            l_pGameObjective->m_Title = GString(GString(100563));
         if(l_pGameObjective->m_eObjectiveType  == EObjectiveType::RaiseGDPPerCapita)
            l_pGameObjective->m_Title = GString(GString(100564));

         g_ClientDCL.m_vGameObjectives.push_back(l_pEvt->m_vObjectives[i]);
      }
   }

   //Set the objectives on the scenario
   g_ClientDAL.Scenario()->ClearObjectives(); 
   g_ClientDAL.Scenario()->SetObjectives(g_ClientDCL.m_vGameObjectives);


}
