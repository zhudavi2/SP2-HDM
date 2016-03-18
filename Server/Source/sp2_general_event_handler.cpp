/**************************************************************
*
* sp2_general_event_handler.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"
#include "sp2_game_control_events.h"

using namespace SP2::Event;
using namespace SDK::Event;

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
 * @return true if successfully handled, false otherwise
 **/
bool SP2::GGeneralEventHandler::HandleStartGameRequest(SDK::GGameEventSPtr in_Event)
{
   // If the Target is not the server, ignore the event
   IF_RETURN(in_Event->m_iTarget != SDK::Event::ESpecialTargets::Server, false);
   IF_RETURN_LOG(!g_SP2Server->GameLoaded(), false, "error : no game data loaded");
   IF_RETURN_LOG(g_SP2Server->GameStarted() == true, false, "error : game laready started");


   // If the event is comming from the server or the admin player
   if(in_Event->m_iSource == SDK::Event::ESpecialTargets::Server || 
      in_Event->m_iSource == g_Joshua.AdminPlayerID())
   {

      // Activate human and ai plauyers
      g_SP2Server->ActivateReadyHumanPlayers();
      g_SP2Server->ActivateAIPlayers();

      g_SP2Server->GameStarted(true);

      g_ServerDCL.UpdateGameObjectives();

      g_SP2Server->SendGameStartEvent();
      g_Joshua.GameSpeed(g_SP2Server->NextGameSpeed() );

      g_Joshua.Log("...started");

      return true;
   }
   
   return false;
}


bool SP2::GGeneralEventHandler::HandleGetRegionCharacteristic(SDK::GGameEventSPtr in_Event)
{   
   GGetRegionsCharacteristic * l_pReceivedEvent = (GGetRegionsCharacteristic *)in_Event.get();

   SDK::GGameEventSPtr l_OutEvent = CREATE_GAME_EVENT(GGetRegionsCharacteristicResult);
   l_OutEvent->m_iTarget = in_Event->m_iSource;
   l_OutEvent->m_iSource = SDK::Event::ESpecialTargets::Server;

   GGetRegionsCharacteristicResult * l_pOutEvent = (GGetRegionsCharacteristicResult *)l_OutEvent.get();   
   l_pOutEvent->Characteristic = l_pReceivedEvent->Characteristic;

   vector<REAL64> l_vData;


   switch (l_pReceivedEvent->Characteristic)
   {
      case ECharacteristicToHighlight::CountryEmpire:
		case ECharacteristicToHighlight::PoliticalControl:
      case ECharacteristicToHighlight::HumanControlledCountries:
      {
         gassert(0,"This event is handled only in the client side");
         return false;
      }
   case ECharacteristicToHighlight::RegionPopulationTotal:
   case ECharacteristicToHighlight::RegionPopulationDensity:      
   case ECharacteristicToHighlight::RegionResourcesProduction:
      l_vData.resize(g_ServerDAL.NbRegion(),0);
      break;
   case ECharacteristicToHighlight::CountryRelations:
      break;
   default:
      if (l_pReceivedEvent->Characteristic >= ECharacteristicToHighlight::RegionResourcesProduction)
         l_vData.resize(g_ServerDAL.NbRegion(),0);
      else
         l_vData.resize(g_ServerDAL.NbCountry(),0);
   }

   switch (l_pReceivedEvent->Characteristic)
   {
   case ECharacteristicToHighlight::CountryRelations:
      {
         REAL64 l_Relation = 0.f;
         l_pOutEvent->Results.resize(g_ServerDAL.NbCountry());
         for (INT32 i = 1; i <= g_ServerDAL.NbCountry(); i++)
         {
            if(l_pReceivedEvent->SelectedCountry != (UINT32)g_ServerDAL.CountryData(i)->CountryID())
            {
               l_Relation = g_ServerDAL.RelationBetweenCountries(l_pReceivedEvent->SelectedCountry, g_ServerDAL.CountryData(i)->CountryID());
            }
            else
            {
               // so we will always be first in diplomatic relations ...
               l_Relation = 101.0f;
            }
            l_pOutEvent->Results[g_ServerDAL.CountryData(i)->CountryID() - 1] = l_Relation / 100;
         }     
         g_Joshua.RaiseEvent(l_OutEvent);
         return true;
      }
   case ECharacteristicToHighlight::RegionPopulationTotal:      
      for (UINT32 i = 1; i <= g_ServerDAL.NbRegion(); i++)
         l_vData[i - 1] = (REAL64)g_ServerDAL.GetGRegion(i)->Population();

      //In the additional results, set the region population total for the country
      l_pOutEvent->m_vAdditionalResults.resize(g_ServerDAL.NbCountry());
      for(UINT32 i = 0 ; i < g_ServerDAL.NbCountry() ; i++)
      {
         SP2::GCountryData* l_pCountry = g_ServerDAL.CountryData(i+1);
         l_pOutEvent->m_vAdditionalResults[i] = (REAL64)l_pCountry->Population();
      }

      break;
   case ECharacteristicToHighlight::RegionPopulationDensity:            
      for (UINT32 i = 1; i <= g_ServerDAL.NbRegion(); i++)
         l_vData[i - 1] = g_ServerDAL.GetGRegion(i)->AreaLand() 
         ? (REAL64)g_ServerDAL.GetGRegion(i)->Population() / g_ServerDAL.GetGRegion(i)->AreaLand()
            : -1;

      //In the additional results, set the average population density for the country
      {
         vector<UINT32> l_vCountryHitCount;
         l_vCountryHitCount.resize(g_ServerDAL.NbCountry(),0);
         l_pOutEvent->m_vAdditionalResults.resize(g_ServerDAL.NbCountry(),0);
         const vector<GRegionControl>& l_vRegionControl = g_ServerDAL.RegionControlArray();
         for(UINT32 i = 1 ; i <= g_ServerDAL.NbRegion() ; i++)
         {
            if(l_vRegionControl[i].m_iPolitical)
            {
               GRegion* l_pRegion = g_ServerDAL.GetGRegion(i);
               l_vCountryHitCount[l_vRegionControl[i].m_iPolitical-1]++;
               l_pOutEvent->m_vAdditionalResults[l_vRegionControl[i].m_iPolitical-1] += (l_pRegion->Population()/l_pRegion->AreaTotal());
            }        
         }
         //Now, divide by the hitcount to get the average
         for(UINT32 i = 0 ; i < l_pOutEvent->m_vAdditionalResults.size() ; i++)
         {
            l_pOutEvent->m_vAdditionalResults[i] /= l_vCountryHitCount[i];
         }
      }
      break;

   case ECharacteristicToHighlight::CountryPopulationGrowth:
      for (INT32 i = 1; i <= g_ServerDAL.NbCountry(); i++)
         l_vData[i - 1] = (REAL64)g_ServerDAL.CountryData(i)->GrowthRate();
      break;
   case ECharacteristicToHighlight::CountryGrossDomesticProduct:
      for (INT32 i = 1; i <= g_ServerDAL.NbCountry(); i++)
         l_vData[g_ServerDAL.CountryData(i)->CountryID() - 1] = (REAL64)g_ServerDAL.CountryData(i)->GDPValue();
      break;
   case ECharacteristicToHighlight::CountryGrossDomesticProductGrowth:
      for (INT32 i = 1; i <= g_ServerDAL.NbCountry(); i++)
         l_vData[g_ServerDAL.CountryData(i)->CountryID() - 1] = (REAL64)g_ServerDAL.CountryData(i)->GDPGrowth();
      break;
   case ECharacteristicToHighlight::CountryPopulationSupport:
      for (INT32 i = 1; i <= g_ServerDAL.NbCountry(); i++)
         l_vData[g_ServerDAL.CountryData(i)->CountryID() - 1] = (REAL64)g_ServerDAL.CountryData(i)->GvtApproval();
      break;
   case ECharacteristicToHighlight::CountryPopulationStabilty:
      for (INT32 i = 1; i <= g_ServerDAL.NbCountry(); i++)
         l_vData[g_ServerDAL.CountryData(i)->CountryID() - 1] = (REAL64)g_ServerDAL.CountryData(i)->GvtStability();
      break;
   case ECharacteristicToHighlight::CountryEconomicalStrength:
      for (INT32 i = 1; i <= g_ServerDAL.NbCountry(); i++)
         l_vData[g_ServerDAL.CountryData(i)->CountryID() - 1] = g_ServerDAL.NbCountry() - (REAL64)g_ServerDAL.CountryData(i)->EconomicRank();
      break;
   case ECharacteristicToHighlight::CountryPoliticalStrength:
      for (INT32 i = 1; i <= g_ServerDAL.NbCountry(); i++)
         l_vData[g_ServerDAL.CountryData(i)->CountryID() - 1] = g_ServerDAL.NbCountry() - (REAL64)g_ServerDAL.CountryData(i)->PoliticRank();
      break;
   case ECharacteristicToHighlight::CountryExceedingResources:
      for (INT32 i = 1; i <= g_ServerDAL.NbCountry(); i++)
      {
         REAL64 l_ResourcesProd = 0;
         for (EResources::Enum r = (EResources::Enum)0; r < (EResources::Enum)EResources::ItemCount; r = (EResources::Enum)(r + 1))
            l_ResourcesProd += g_ServerDAL.CountryData(i)->ResourceProduction(r) - g_ServerDAL.CountryData(i)->ResourceDemand(r);
         l_vData[g_ServerDAL.CountryData(i)->CountryID() - 1] = l_ResourcesProd;
      }
      break;
   case ECharacteristicToHighlight::CountryMilitaryStrength:
      for (INT32 i = 1; i <= g_ServerDAL.NbCountry(); i++)
         l_vData[g_ServerDAL.CountryData(i)->CountryID() - 1] = g_ServerDAL.NbCountry() - (REAL64)g_ServerDAL.CountryData(i)->MilitaryRank();
      break;

   case ECharacteristicToHighlight::CountryNuclearStrength:
      for (INT32 i = 1; i <= g_ServerDAL.NbCountry(); i++)
      {
         REAL64 l_MTons = 0;

         GJoinedVectors<SP2::GNuclearMissile> l_vMissiles = g_ServerDAL.CountryData(i)->NuclearMissiles();
         for(GJoinedVectors<SP2::GNuclearMissile>::GIterator l_It = l_vMissiles.Begin();
            l_It != l_vMissiles.End();
            l_It++)
         {
            l_MTons += l_It->Megatons() * l_It->m_iQty;
         }
         l_vData[g_ServerDAL.CountryData(i)->CountryID() - 1] = l_MTons ? l_MTons : -1;
      }
      break;

   case ECharacteristicToHighlight::CountryHumanDevelopment:
      for (INT32 i = 1; i <= g_ServerDAL.NbCountry(); i++)
         l_vData[g_ServerDAL.CountryData(i)->CountryID() - 1] = (REAL64)g_ServerDAL.CountryData(i)->HumanDevelopment();
      break;

   case ECharacteristicToHighlight::RegionResourcesProduction:      
      for (UINT32 i = 1; i <= g_ServerDAL.NbRegion(); i++)
      {
         REAL64 l_ResourcesProd = 0;
         for (EResources::Enum r = (EResources::Enum)0; r < (EResources::Enum)EResources::ItemCount; r = (EResources::Enum)(r + 1))
            l_ResourcesProd += g_ServerDAL.GetGRegion(i)->ResourceProduction(r);
         l_vData[i - 1] = l_ResourcesProd;
      }
      break;

   default: 
      break;
   }
      
   // all resources
   if (l_pReceivedEvent->Characteristic >= ECharacteristicToHighlight::RegionResourcesProduction)
   {
      EResources::Enum l_eResourceToFetch = (EResources::Enum)(l_pReceivedEvent->Characteristic - ECharacteristicToHighlight::RegionResourcesProduction-1);

      const vector<GRegionControl>& l_vRegionControl = g_ServerDAL.RegionControlArray();
      UINT32 l_iNbRegion = g_ServerDAL.NbRegion();
      for (UINT32 i = 1; i <= l_iNbRegion ; i++)
      {
         if(l_vRegionControl[i].m_iPolitical != 0 && l_vRegionControl[i].m_iPolitical == l_vRegionControl[i].m_iMilitary)
         {
            l_vData[i - 1] = g_ServerDAL.GetGRegion(i)->ResourceProduction(l_eResourceToFetch);          

            //Fetch the country pointer and apply the production modifier
            GCountryData* l_pCountry = g_ServerDAL.CountryData(l_vRegionControl[i].m_iPolitical);
				REAL64 l_fModifier = 0.f;
				if(l_pCountry->GvtType() == EGovernmentType::Anarchy)
					l_fModifier = l_pCountry->GvtApproval() / 2.f;
				else
					l_fModifier = c_pGvtTypeProductionModifier[l_pCountry->GvtType()];
            l_vData[i - 1] *= l_fModifier;
         }
      }

      //In the additional results, set the resource production total for the country
      l_pOutEvent->m_vAdditionalResults.resize(g_ServerDAL.NbCountry());
      for(UINT32 i = 0 ; i < g_ServerDAL.NbCountry() ; i++)
      {
         SP2::GCountryData* l_pCountry = g_ServerDAL.CountryData(i+1);
         l_pOutEvent->m_vAdditionalResults[i] = l_pCountry->ResourceProduction(l_eResourceToFetch);
      }
   }


   vector<UINT32> l_vDataOrder;
   Util::GetSortedVectorIndices(l_vData, l_vDataOrder);

   l_pOutEvent->Results.clear();
   UINT32 l_Size = l_vDataOrder.size();
   UINT32 l_Total = l_Size;

   for (UINT32 i = 0; i < l_Size; i++)
   {
      if (l_vData[l_vDataOrder[i]] == -1) 
         l_Total--;
   }

   l_pOutEvent->Results.resize(l_Size);
   UINT32 l_Rank = 0;
   for (UINT32 i = 0; i < l_Size; i++)
   {      
      if (l_vData[l_vDataOrder[i]] == -1)
         l_pOutEvent->Results[l_vDataOrder[i]] = -1;
      else
      {
         l_Rank++;
         l_pOutEvent->Results[l_vDataOrder[i]] = (REAL64)l_Rank / l_Total;
      }
   }

   g_Joshua.RaiseEvent(l_OutEvent);

   return true;
}

void SP2::GGeneralEventHandler::HandleSetGouvernementType(GSetPlayerInfo *in_pPlayerInfo)
{
   GSetPlayerInfo * l_pSetPlayerInfo = in_pPlayerInfo;
   gassert(l_pSetPlayerInfo,"Invalid Player info pointer");

   SDK::GPlayer * l_player = g_Joshua.HumanPlayer(l_pSetPlayerInfo->m_PlayerInfo.ClientID);
   if(!l_player)
   {
      return;
   }
	gassert(l_player,"GGeneralEventHandler::HandleSetPlayerInfo(): Player should not be NULL");

   // Set PlayerName;
   l_player->Name(l_pSetPlayerInfo->m_PlayerInfo.PlayerName);
   l_player->Client()->PlayerName(l_pSetPlayerInfo->m_PlayerInfo.PlayerName);

   if(l_pSetPlayerInfo->m_PlayerInfo.CountryID > 0)
   {
      // Set Player Selected Country
      l_player->ModID(l_pSetPlayerInfo->m_PlayerInfo.CountryID);

      // Set Player politic party
      GCountryData* l_Data = g_ServerDAL.CountryData( l_pSetPlayerInfo->m_PlayerInfo.CountryID );
      INT32 l_iGvtType = l_Data->GvtType();
      const vector<GPoliticalParty> & l_Parties = l_Data->PoliticalParty();
      INT32 l_iNewPartyID = l_pSetPlayerInfo->m_PlayerInfo.PartyID;

      //If we must set a new party
      if(l_iNewPartyID >= 0)
      {
         //Set the new party
         for(INT32 i=0; i<(INT32)l_Parties.size(); i++)
         {
            if(l_iGvtType == 8 || l_iGvtType == 9)
            {
               if(l_Parties[i].GvtType() == 9)
               {
                  if(l_Parties[i].Id() == l_iNewPartyID)
                  {
                     if(!l_Parties[i].InPower())
                     {
                        l_Data->ChangePoliticalPartyInPower(l_iNewPartyID, true);
                        //l_Parties[i].InPower(true);
                     }
                  }
                  else
                  {
                     if(l_Parties[i].InPower())
                     {
                        l_Data->ChangePoliticalPartyInPower(l_Parties[i].Id(), false);
                        //l_Parties[i].InPower(false);
                     }
                  }
               }
            }
            else
            {
               if(l_Parties[i].GvtType() == l_iGvtType)
               {
                  if(l_Parties[i].Id() == l_iNewPartyID)
                  {
                     if(!l_Parties[i].InPower())
                     {
                        l_Data->ChangePoliticalPartyInPower(l_iNewPartyID, true);
                        //l_Parties[i].InPower(true);
                     }
                  }
                  else
                  {
                     if(l_Parties[i].InPower())
                     {
                        l_Data->ChangePoliticalPartyInPower(l_Parties[i].Id(), false);
                        //l_Parties[i].InPower(false);
                     }
                  }
               }
            }
         }

         // If the player Gvttype is a democracy set the new election date
         if(l_iGvtType == 8 || l_iGvtType == 9)
         {
            //Get the zero date and add the 4 years now

            //Get since when the game is started
            GTimeSpan l_Span( (INT64) ( (g_Joshua.GameTime() + 365.25 * 4) * c_fSP2TimeToTenthOfSecs) );
            GDateTime l_ElectionDate(g_ServerDAL.ZeroDate() + l_Span);

            //Set the election date
            l_Data->NextElection( l_ElectionDate );
         }
      }
   }
}


bool SP2::GGeneralEventHandler::HandleSetPlayerInfo(SDK::GGameEventSPtr in_Event)
{
   GSetPlayerInfo *l_pSetPlayerInfo = (GSetPlayerInfo *)in_Event.get();

   SDK::GPlayer * l_player = g_Joshua.HumanPlayer(l_pSetPlayerInfo->m_PlayerInfo.ClientID);

	gassert(l_player,"GGeneralEventHandler::HandleSetPlayerInfo(): Player should not be NULL");

    // If the player is active, then only set the name if we're not in
    // PlayerNameViaCovert mode (player must change name via Covert window)
    // Always set the player name if the player is not active.
    if(g_SP2Server->CountryNameChangeMode() != ECountryNameChangeMode::PlayerNameViaCovert ||
       l_player->PlayerStatus() != SDK::PLAYER_STATUS_ACTIVE)
    {
        const auto& l_sNewPlayerName = l_pSetPlayerInfo->m_PlayerInfo.PlayerName;

        GDZDebug::Log(L"Player ID " + GString(l_player->Id()) + L", " +
                      l_player->Name() + L", is inactive, or " +
                      L"country name change mode is allowing player name change via Lobby; " +
                      L"changing player name to " + l_sNewPlayerName,
                      EDZDebugLogCategory::Player,
                      __FUNCTION__, __LINE__);

        g_ServerDCL.ChangePlayerName(l_player, l_sNewPlayerName);
    }

    // If we are active, we shouldn't be able to do anything more !
    if(l_player->PlayerStatus() == SDK::PLAYER_STATUS_ACTIVE)
	{
        // GDataControlLayer::ChangePlayerName() will have already sent any new
        // information to all players
        return true;
	}

   // Make absolutly sure that this country is not taken yet
   // and that it is not annexed
   if(l_pSetPlayerInfo->m_PlayerInfo.CountryID > 0 )
   {
      UINT32 l_iPlayerID = g_SP2Server->IsCountryChoosen((UINT16)l_pSetPlayerInfo->m_PlayerInfo.CountryID);
      if( (l_iPlayerID != 0 && l_iPlayerID != (UINT32)l_pSetPlayerInfo->m_PlayerInfo.ClientID) ||
          (!g_ServerDAL.CountryData(l_pSetPlayerInfo->m_PlayerInfo.CountryID)->Activated() ) )
      {
         l_pSetPlayerInfo->m_PlayerInfo.CountryID = 0;
         l_pSetPlayerInfo->m_PlayerInfo.PlayerStatus = SDK::PLAYER_STATUS_IDLE;
      }
   }

   // Player just joined, send server message through chat
   // Detect by observing that the player hasn't selected a country and hasn't changed their name
   if(!g_SP2Server->HasPlayerIDBeenAdded(l_player->Id()))
   {
       const GString l_sMessage = g_SP2Server->Message();
       if(!l_sMessage.empty())
       {
           SDK::GGameEventSPtr l_pChatEvent = CREATE_GAME_EVENT(SDK::Event::GChatEvent);

           // reinterpret_cast, CREATE_GAME_EVENT doesn't "actually" create a GChatEvent object for some reason
           reinterpret_cast<SDK::Event::GChatEvent *>(l_pChatEvent.get())->Message(l_sMessage);

           l_pChatEvent->m_iSource = SDK::Event::ESpecialTargets::Server;
           l_pChatEvent->m_iTarget = l_player->Id();
           g_Joshua.RaiseEvent(l_pChatEvent);
       }
       g_SP2Server->AddPlayerID(l_player->Id());
   }

   // Set Player Selected Country
   l_player->ModID(l_pSetPlayerInfo->m_PlayerInfo.CountryID);

   // Set Player politic party only if player has selected a country
   if(l_player->ModID() > 0)
   {
      HandleSetGouvernementType(l_pSetPlayerInfo);    
   }

   
   // If the status has change, send a country list, cause this player has choosen a country
   if(l_player->PlayerStatus() != l_pSetPlayerInfo->m_PlayerInfo.PlayerStatus)
   {
      // Set Player State (ready or not)
      l_player->PlayerStatus(l_pSetPlayerInfo->m_PlayerInfo.PlayerStatus);

      // Regenerate country list
      g_SP2Server->SendCountryList();      
   }

   // NOTE : NEVER CHANGE THE ORDER OF THOSE TWO NEXT LINE
   // REASON : THE CLIENT GETS ITS NAME AND SELECTED COUNTRY FROM THE PLAYER LIST
   // IT NEVER SET ITSELF A NAME NOR A COUNTRY ID (FOR A SYNCH REASON), THE TRY GAME START CAN POTENTIALLY START THE GAME
   // WE DONT WANT TO PLAYER TO HAVE NO COUNTRY WHEN STARTING THE GAME...

   // Send this new information to all players
   g_SP2Server->SendPlayersList();

   // Try to start the game if its not started
   if(g_SP2Server->GameStarted() == false)
   {
      g_SP2Server->TryStartGame();
   }
   else
   {
      // The game is started, if player is ready add it to the game...
      if(l_player->PlayerStatus() == SDK::PLAYER_STATUS_READY)
      {
         //Synchronize the country data of each human player
         {
            SDK::GGameEventSPtr l_SyncEvt = CREATE_GAME_EVENT(SP2::Event::GSynchronizeCountryData);
            //Check if the country data of that human player must be synchronized, if so, send it
            GCountryData* l_pCountryData = g_ServerDAL.CountryData(l_player->ModID());
            gassert(l_pCountryData,"GServer::Iterate, Country Data does not exist for human player");
            //Make sure everything is sent so set everything dirty
            {
               l_pCountryData->m_bBudgetDataDirty           = true;
               l_pCountryData->m_bCovertActionCellsDirty    = true;
               l_pCountryData->m_bEconomicDataDirty         = true;
               l_pCountryData->m_bNuclearMissilesDataDirty  = true;
               l_pCountryData->m_bResourceDataDirty         = true;
               l_pCountryData->m_bOtherDataDirty            = true;
            }
            l_SyncEvt->m_iSource = SDK::Event::ESpecialTargets::Server;
            l_SyncEvt->m_iTarget = l_player->Id();
            SP2::Event::GSynchronizeCountryData* l_pEvent = (SP2::Event::GSynchronizeCountryData*)l_SyncEvt.get();
            l_pEvent->CountryData(l_pCountryData);
            g_Joshua.RaiseEvent(l_SyncEvt);
            l_pCountryData->ClearModifications();
         }//end of synchronize country data            


         //Set the player objective
         g_ServerDAL.PlayersObjectives(l_player,g_ServerDAL.GameObjectives());

         // Replace current player by this one.
         g_Joshua.ReplaceAIPlayerByHuman(l_player);

         // Send him initial data
         g_SP2Server->SendInitialDataToPlayer(l_player);

         // Send him a start signal
         g_SP2Server->SendGameStartEvent(l_player->Id());
      }

   }

   return true;
}

bool SP2::GGeneralEventHandler::HandleGetPlayersList(SDK::GGameEventSPtr in_Event)
{
   // Return the player list to the source
   GGetPlayersList *l_pEvent = (GGetPlayersList*)in_Event.get();
   g_SP2Server->SendPlayersList(l_pEvent->m_iSource);

   return true;
}

void SP2::GGeneralEventHandler::HandleGetCountryRanks(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GGetCountryRanks* l_pEvent = (SP2::Event::GGetCountryRanks*)in_Event.get();
   l_pEvent->m_iTarget = l_pEvent->m_iSource;
   l_pEvent->m_iSource = SDK::Event::ESpecialTargets::Server;
   l_pEvent->PoliticRank(g_ServerDAL.CountryData(*l_pEvent->CountryId())->PoliticRank());
   l_pEvent->EconomicRank(g_ServerDAL.CountryData(*l_pEvent->CountryId())->EconomicRank());
   g_Joshua.RaiseEvent(in_Event);
}

void SP2::GGeneralEventHandler::HandleGetCountriesRanks(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GGetCountriesRanks* l_pEvent = (SP2::Event::GGetCountriesRanks*)in_Event.get();
  
   l_pEvent->m_viCountryID.clear();
   l_pEvent->m_viEconomicRank.clear();
   l_pEvent->m_viPoliticRank.clear();
   l_pEvent->m_viMilitaryRank.clear();
   
   for(INT16 i=1; i<=g_ServerDAL.NbCountry(); i++)   
   {
      GCountryData* l_pData = g_ServerDAL.CountryData(i);

      if(l_pData)
      {
         l_pEvent->m_viCountryID.push_back( i );
         l_pEvent->m_viEconomicRank.push_back( l_pData->EconomicRank() );
         l_pEvent->m_viPoliticRank.push_back(  l_pData->PoliticRank()  );
         l_pEvent->m_viMilitaryRank.push_back( l_pData->MilitaryRank() );
      }
   }

   l_pEvent->m_iTarget = l_pEvent->m_iSource;
   l_pEvent->m_iSource = SDK::Event::ESpecialTargets::Server;

   g_Joshua.RaiseEvent( in_Event );
}

void SP2::GGeneralEventHandler::HandleRequestAvailableCountries(SDK::GGameEventSPtr in_Event)
{
   assert(in_Event->m_iSource);

   //Send the list of available countries to the client so it can pick a country
   g_Joshua.Log(L"Player :" + GString(in_Event->m_iSource) + L" connected to server. Sending the list of available countries");
   g_SP2Server->SendCountryList(in_Event->m_iSource);
}

void SP2::GGeneralEventHandler::HandleUpdateAdvisorSettings(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GUpdateAdvisorSettings* l_pUpdate = (SP2::Event::GUpdateAdvisorSettings*)in_Event.get();

   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayer(l_pUpdate->m_iSource);
   if(!l_pPlayer)
      return;
   
   GCountryData* l_pData = g_ServerDAL.CountryData(l_pPlayer->ModID());

   if(l_pData->Advisor())
   {
      l_pData->Advisor()->EconomicStatus(l_pUpdate->m_eEconAdvisorStatus);
      l_pData->Advisor()->PoliticStatus(l_pUpdate->m_ePoliAdvisorStatus);
      l_pData->Advisor()->MilitaryStatus(l_pUpdate->m_eMiliAdvisorStatus);

      l_pData->AdvisorEconomicStatus(l_pUpdate->m_eEconAdvisorStatus);
      l_pData->AdvisorPoliticStatus(l_pUpdate->m_ePoliAdvisorStatus);
      l_pData->AdvisorMilitaryStatus(l_pUpdate->m_eMiliAdvisorStatus);

      l_pData->m_bOtherDataDirty = true;
   }
   // this event need an acknowledgement of receipt
   g_ServerDCL.AcknowledgeRequest(l_pUpdate->m_iRequestID, l_pUpdate->m_iSource);
}


/*!
 * Handles the answer from the Admin player that is sent when the game is supposedly over
 * and there is a possibility to continue
 **/
void SP2::GGeneralEventHandler::HandleGameOverAnswer(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GGameOver* l_pGameOverEvt = (SP2::Event::GGameOver*)in_Event.get();

   //Only handle the event if the Admin Player answered
   if(l_pGameOverEvt->m_iSource == g_Joshua.AdminPlayerID())
   {
/*
      //Process the answer
      g_ServerDCL.GameObjectiveEvaluator().ProcessAnswer(l_pGameOverEvt->m_bWillContinue);
*/


      if(l_pGameOverEvt->m_bWillContinue)
      {
         g_Joshua.Log("... Admin player has decided game will be continued... new objectives set");
      }
      else
      {
         g_Joshua.Log("... Admin player has decided game will be over");
         //Game is over, shutdown the server
         SDK::GGameEventSPtr l_Evt = CREATE_GAME_EVENT(SDK::Event::GSystemGameEvent);
         SDK::Event::GSystemGameEvent* l_pEvt = (SDK::Event::GSystemGameEvent*)l_Evt.get();
         l_pEvt->SystemEventType(SDK::Event::GSystemGameEvent::TERMINATE_SERVER);
         l_Evt->m_iSource = SDK::Event::ESpecialTargets::Server;
         l_Evt->m_iTarget = SDK::Event::ESpecialTargets::Server;
         g_Joshua.RaiseEvent(l_Evt);
      }
   }
}

void SP2::GGeneralEventHandler::HandleSetObjectives(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GSetGameObjectives* l_pSetObjEvt = (SP2::Event::GSetGameObjectives*)in_Event.get();
   //Get the default objectives 
   vector<GGameObjective> l_vObjectives = g_ServerDAL.AlwaysPresentObjectives();

   //Add the new objectives
   l_vObjectives.insert(l_vObjectives.begin(),l_pSetObjEvt->m_vObjectives.begin(),l_pSetObjEvt->m_vObjectives.end());
   
   //Set the game objectives
   g_ServerDAL.GameObjectives(l_vObjectives);

   //Updates the objectives for the players with the newly setted objectives
   g_ServerDCL.UpdateGameObjectives();

   //Resend the game objectives to all active players
   SDK::GGameEventSPtr l_EvtObj = CREATE_GAME_EVENT(SP2::Event::GSetGameObjectives);
   SP2::Event::GSetGameObjectives* l_pSetGameObj = (SP2::Event::GSetGameObjectives*)l_EvtObj.get();
   l_pSetGameObj->m_iSource = SDK::Event::ESpecialTargets::Server;
   l_pSetGameObj->m_iTarget = SDK::Event::ESpecialTargets::BroadcastActiveHumanPlayers;
   l_pSetGameObj->m_vObjectives = g_ServerDAL.GameObjectives();
   g_Joshua.RaiseEvent(l_EvtObj);

}

void SP2::GGeneralEventHandler::HandleCountryInfo(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GEventCountryInfo* l_pUpdate = (SP2::Event::GEventCountryInfo*)in_Event.get();

   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayer(l_pUpdate->m_iSource);
   if(!l_pPlayer)
      return;

   l_pUpdate->m_iTarget = l_pUpdate->m_iSource;
   l_pUpdate->m_iSource = SDK::Event::ESpecialTargets::Server;

   GCountryData* l_pData = g_ServerDAL.CountryData(l_pUpdate->m_iCountryID);
   
   l_pUpdate->m_fArableLand      = l_pData->ArableLandLevel();
   l_pUpdate->m_fForestLand      = l_pData->ForestLandLevel();
   l_pUpdate->m_fParksLand       = l_pData->ParksLandLevel();
   l_pUpdate->m_fUnusableLand    = l_pData->NotUsedLandLevel();

   l_pUpdate->m_iPop15           = l_pData->Pop15();
   l_pUpdate->m_iPop1565         = l_pData->Pop1565();
   l_pUpdate->m_iPop65           = l_pData->Pop65();
   
   l_pUpdate->m_fLandArea        = l_pData->AreaLandTotal();
   l_pUpdate->m_fWaterArea       = l_pData->AreaWaterTotal();
   l_pUpdate->m_fTotalArea       = l_pData->AreaTotal();

   l_pUpdate->m_fHumanDev        = l_pData->HumanDevelopment();
   l_pUpdate->m_fHumanDevAverage = g_ServerDCL.AverageHumanDevelopment();
   l_pUpdate->m_iClimateStid     = l_pData->ClimateNameID();

   l_pUpdate->m_fBirthRate       = l_pData->BirthRate();
   l_pUpdate->m_fDeathRate       = l_pData->DeathRate();
   l_pUpdate->m_fInfrastructure  = l_pData->Infrastructure();
   l_pUpdate->m_fTelecommunications = l_pData->TelecomLevel();

   g_Joshua.RaiseEvent(in_Event);

   return;
}

void SP2::GGeneralEventHandler::HandleChangeAIAggressiveness(SDK::GGameEventSPtr in_Event)
{
	//If the event has been received from the admin player
   if(in_Event->m_iSource == g_Joshua.AdminPlayerID())
   {
      SP2::Event::GChangeAIAggressiveness* l_pEvt = (SP2::Event::GChangeAIAggressiveness*)in_Event.get();
		g_SP2Server->AIAggressiveness(l_pEvt->AIAggressiveness());
      g_Joshua.Log("AI aggressiveness changed");
   }
   else
   {
      g_Joshua.Log("Non admin player tried to change AI aggressiveness");
   }
}

void SP2::GGeneralEventHandler::HandleChangeGameSpeed(SDK::GGameEventSPtr in_Event)
{
   //If the event has been received from the admin player
   if(in_Event->m_iSource == g_Joshua.AdminPlayerID())
   {
      SP2::Event::GChangeGameSpeed* l_pEvt = (SP2::Event::GChangeGameSpeed*)in_Event.get();
      g_Joshua.GameSpeed(l_pEvt->NewSpeed());
      g_Joshua.Log("Admin player changed speed");
   }
   else
   {
      g_Joshua.Log("Non admin player tried to change game speed");
   }
}

void SP2::GGeneralEventHandler::HandleRelationHistoryFetch(SDK::GGameEventSPtr in_Event)
{
   //Fetch the relation history array
   SP2::Event::GRelationHistoryFetch* l_pEvt = (SP2::Event::GRelationHistoryFetch*)in_Event.get();

   //If player wants to start fetching relation history data
   if(l_pEvt->m_bStartFetch)
   {
      if(l_pEvt->m_iCountryA != l_pEvt->m_iCountryB)
      {
         //Send a 1st relation history sync.
         g_ServerDCL.SendRelationHistory(l_pEvt->m_iSource,l_pEvt->m_iCountryA,l_pEvt->m_iCountryB);

         //Register this player as want to have relation history information
         g_ServerDAL.RegisterForRelationsHistoryFetch(l_pEvt->m_iSource,l_pEvt->m_iCountryA,l_pEvt->m_iCountryB);
      }
   }
   //Unregister
   else
   {
      g_ServerDAL.UnregisterForRelationsHistoryFetch(l_pEvt->m_iSource);
   }


}

void SP2::GGeneralEventHandler::HandleGetCountryParties(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GGetCountryParties* l_pEvt = (SP2::Event::GGetCountryParties*)in_Event.get();

   if(l_pEvt->m_iCountryId)
   {
      GCountryData* l_pData = g_ServerDAL.CountryData(l_pEvt->m_iCountryId);
      INT32 l_iGvtType = l_pData->GvtType();
      const vector<GPoliticalParty> & l_Parties = l_pData->PoliticalParty();
      
      for(UINT32 i=0; i<l_Parties.size(); i++)
      {
         EGovernmentType::Enum l_eGvtType = l_Parties[i].GvtType();
         
         if(l_iGvtType == 8 || l_iGvtType == 9)
         {
            if( (INT32)l_eGvtType == 9 )
            {
               l_pEvt->m_vPartyList.push_back( l_Parties[i] );
            }
         }
         else if( l_iGvtType == (INT32)l_eGvtType )
         {
            l_pEvt->m_vPartyList.push_back( l_Parties[i] );
         }
      }
   }

   l_pEvt->m_iTarget = l_pEvt->m_iSource;
   l_pEvt->m_iSource = SDK::Event::ESpecialTargets::Server;

   g_Joshua.RaiseEvent(in_Event);
}

void SP2::GGeneralEventHandler::HandleRequestCountryList(SDK::GGameEventSPtr in_Event)
{
   SDK::GGameEventSPtr l_ResponseEvent = CREATE_GAME_EVENT(Event::GReceiveCountryList);
   l_ResponseEvent->m_iSource = SDK::Event::ESpecialTargets::Server;
   l_ResponseEvent->m_iTarget = in_Event->m_iSource;

   Event::GReceiveCountryList* l_pCntrListEvent = (Event::GReceiveCountryList*) l_ResponseEvent.get();
   l_pCntrListEvent->m_vCountries = g_SP2Server->Countries();

   g_Joshua.RaiseEvent(l_ResponseEvent);
}

void SP2::GGeneralEventHandler::HandleFetchCountriesRelations(SDK::GGameEventSPtr in_Event)
{
	// Only send data if game is loaded
   if(g_SP2Server->GameLoaded() )
   {
      SDK::GGameEventSPtr l_ResponseEvent = CREATE_GAME_EVENT(Event::GEventFetchCountriesRelations);
      l_ResponseEvent->m_iSource = SDK::Event::ESpecialTargets::Server;
      l_ResponseEvent->m_iTarget = in_Event->m_iSource;
		SP2::Event::GEventFetchCountriesRelations* l_pSendRelations = (SP2::Event::GEventFetchCountriesRelations*) l_ResponseEvent.get();

      SP2::Event::GEventFetchCountriesRelations* l_pRelationsEvent = (SP2::Event::GEventFetchCountriesRelations*)in_Event.get();
		UINT32 l_iCountry = l_pRelationsEvent->m_iCountryId;
      l_pSendRelations->m_vRelationsValues.clear();
		l_pSendRelations->m_iCountryId = l_iCountry;
		UINT32 l_iNbCountry = g_ServerDAL.NbCountry() + 1;
		l_pSendRelations->m_vRelationsValues.resize(l_iNbCountry);
		for(UINT32 i = 1; i < l_iNbCountry; i++)
		{
			l_pSendRelations->m_vRelationsValues[i] = g_ServerDAL.RelationBetweenCountries(i,l_iCountry);
		}

      g_Joshua.RaiseEvent(l_ResponseEvent); 
   }
}

void SP2::GGeneralEventHandler::HandleRequestCountryStaticData(SDK::GGameEventSPtr in_Event)
{
   // Only send data if game is loaded
   if(g_SP2Server->GameLoaded() )
   {
      SDK::GGameEventSPtr l_ResponseEvent = CREATE_GAME_EVENT(Event::GReceiveCountryStaticData);
      l_ResponseEvent->m_iSource = SDK::Event::ESpecialTargets::Server;
      l_ResponseEvent->m_iTarget = in_Event->m_iSource;

      SP2::Event::GReceiveCountryStaticData* l_pCntrDataEvent = (SP2::Event::GReceiveCountryStaticData*) l_ResponseEvent.get();
      g_ServerDAL.FillReceiveCountryStaticData(l_pCntrDataEvent);

      g_Joshua.RaiseEvent(l_ResponseEvent);

      // Clear modificiation for all current active players to clear modification so that the new player have a "CLEAN" synch.
      g_SP2Server->SyncronizeMilitary(SDK::Event::ESpecialTargets::BroadcastHumanPlayers,g_Joshua.Clock() );
      g_SP2Server->SendCombatInfo(in_Event->m_iSource);

      g_Joshua.Log(L"Sending combat information");
   }
   else
   {
      // No data loaded, return an error
      SDK::GGameEventSPtr l_ErrorEvent = CREATE_GAME_EVENT(Event::GErrorCountryStaticData);
      l_ErrorEvent->m_iSource = SDK::Event::ESpecialTargets::Server;
      l_ErrorEvent->m_iTarget = in_Event->m_iSource;

      g_Joshua.RaiseEvent(l_ErrorEvent);
   }
}

void SP2::GGeneralEventHandler::HandleResetGame(SDK::GGameEventSPtr in_Event)
{
   // Only allo wadmin to reset the game
   if(in_Event->m_iSource == g_Joshua.AdminPlayerID() )
   {
      // Stop the server from iterating
      g_SP2Server->GameStarted(false);

      // Ack reset request from server
      SDK::GGameEventSPtr l_ResetEvent = CREATE_GAME_EVENT(SP2::Event::GResetGame);
      l_ResetEvent->m_iSource = SDK::Event::ESpecialTargets::Server;
      l_ResetEvent->m_iTarget = in_Event->m_iSource;

      g_Joshua.RaiseEvent(l_ResetEvent);
   }
}

void SP2::GGeneralEventHandler::HandleNewGame(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GNewGame* l_pEvent = (SP2::Event::GNewGame*)in_Event.get();
   g_SP2Server->RequestNewGame(l_pEvent->m_sDBFileName, l_pEvent->m_iSource);
   g_SP2Server->DAL().GameOptions() = l_pEvent->m_GameOptions;
}

void SP2::GGeneralEventHandler::HandleLoadGame(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GLoadGame* l_pEvent = (SP2::Event::GLoadGame*)in_Event.get();
   g_SP2Server->RequestLoadGame(l_pEvent->m_sName, l_pEvent->m_iSource);
}

void SP2::GGeneralEventHandler::HandleSaveGame(SDK::GGameEventSPtr in_Event)
{
   SP2::Event::GSaveGame* l_pEvent = (SP2::Event::GSaveGame*)in_Event.get();
   g_SP2Server->RequestSaveGame(l_pEvent->m_sName, l_pEvent->m_iSource);
}
