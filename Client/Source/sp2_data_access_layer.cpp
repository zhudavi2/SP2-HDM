/****************************************************************
*
* sp2_data_access_layer.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Francois Durand, Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_pch.hpp"

#include "../../includes/common_lib//sp2_event_history_markers.h"
#include "../../includes/common_lib//sp2_game_scenario.h"
#include "../../includes/common_lib//sp2_game_objective.h"



#include "../include/sp2_game_lobby_window.h"
#include "../include/sp2_economic_window.h"
#include "../include/sp2_politic_window.h"
#include "../include/sp2_event_log_window.h"
#include "../include/sp2_mail_window.h"
#include "../include/sp2_fsm_main.h"
#include "../include/sp2_covert_actions_window.h"
#include "../include/sp2_background_earth.h"
#include "../include/sp2_game_scenario_window.h"
#include "../include/sp2_trade_detailed_window.h"


GDataAccessLayerClient::GDataAccessLayerClient()
{
   m_pConfigFile = NULL;

   m_GenericDataHolder.m_bSaveConfirmationAlwaysApply = false;
   m_GenericDataHolder.m_bSaveConfirmationDoNotShow   = false;

   m_iDisplayedOverallCombatID = NULL;
   m_iDisplayedInfoCombatID = NULL;

   m_SelectedUnitsID.clear();

   m_iSelectedCombatID = 0;
   m_iControlledCountryID = -1;

   m_iHistoryMarkerTimeOffset = 0;
   m_fLastHistoryValuesUpdate = 0;
   m_bBeginHistoryIterations = false;
   m_bHistoryIterationsIsRunning = false;

   m_eAdvisorStatusEconomic = EAdvisorStatus::Manual;
   m_eAdvisorStatusPolitic  = EAdvisorStatus::Manual;
   m_eAdvisorStatusMilitary = EAdvisorStatus::Manual;

   m_iRecvCountryDataThreadHandle = 0;
   m_iTotalMarkerReceivedSoFar = 0;

   m_pScenario = GSmartPtr<GGameScenario>(NULL);
   m_bGameIsOver = false;
}

GDataAccessLayerClient::~GDataAccessLayerClient()
{
}

bool GDataAccessLayerClient::Initialize(const GString& in_sXMLFilename)
{
   IF_RETURN(!__super::Initialize(in_sXMLFilename), false);

   // Attach to subject UnitManager as observer
   g_Joshua.UnitManager().Attach(this, Combat::c_notificationUnitGroupRemoved);
   g_Joshua.UnitManager().Attach(this, Combat::c_notificationFirstSynchro);
   return true;
}

bool GDataAccessLayerClient::Shutdown()
{
   // Detach from subject
   g_Joshua.UnitManager().Detach(&m_PathManager);
   g_Joshua.UnitManager().Detach(this);

   Cleanup();

   // Call derivated shutdown
   return __super::Shutdown();
}

/*!
 * Gets a reference to the dialog definition file
 * @param out_File: Updated with the dialog def file
 * @return true in case of success, false otherwise
 **/
bool GDataAccessLayerClient::DialogDefinitionFile(GFile& out_File)
{
   //Get the name of the dialog definition file
   GString l_sDialogDefFilename;
   if(!XMLValue(SP2::DIALOG_DEFINITION_FILE,l_sDialogDefFilename))
      return false;

   if(!File(g_SP2Client->location() + l_sDialogDefFilename,out_File))
      return false;
   else
      return true;
}

bool GDataAccessLayerClient::FetchCountryInfo(INT32 in_iCountryID)
{
   GDZLOG(EDZLogLevel::Entry, L"in_iCountryID = " + GString(in_iCountryID));

   GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GHDMEventCountryInfo);
   SP2::Event::GHDMEventCountryInfo* l_pUpdateEvent = dynamic_cast<SP2::Event::GHDMEventCountryInfo*>(l_pEvent.get());

   l_pUpdateEvent->m_iSource = g_SP2Client->Id();
   GDZLOG(EDZLogLevel::Info1, L"Client ID = " + GString(g_SP2Client->Id()));

   l_pUpdateEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pUpdateEvent->m_iCountryID = (INT16)in_iCountryID;

   g_Joshua.RaiseEvent(l_pEvent);

   GDZLOG(EDZLogLevel::Exit, L"Returning true");
   return true;
}

bool GDataAccessLayerClient::FetchPoliticWindowInfo(INT32 in_iCountryID)
{
   GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GPoliticUpdate);
   SP2::Event::GPoliticUpdate* l_pUpdateEvent = (SP2::Event::GPoliticUpdate*)l_pEvent.get();

   l_pUpdateEvent->m_iSource = g_SP2Client->Id();
   l_pUpdateEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pUpdateEvent->CountryId = (INT16)in_iCountryID;

   g_Joshua.RaiseEvent(l_pEvent);

   return true;
}

bool GDataAccessLayerClient::FetchEconomicWindowInfo(INT32 in_iCountryID)
{
   GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GEconomicUpdate);
   SP2::Event::GEconomicUpdate* l_pUpdateEvent = (SP2::Event::GEconomicUpdate*)l_pEvent.get();

   l_pUpdateEvent->m_iSource = g_SP2Client->Id();
   l_pUpdateEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pUpdateEvent->CountryId = (INT16)in_iCountryID;

   g_Joshua.RaiseEvent(l_pEvent);

   return true;
}

bool GDataAccessLayerClient::FetchEconomicHealthWindowInfo(INT32 in_iCountryID)
{
   GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GEconomicHealthUpdate);
   SP2::Event::GEconomicHealthUpdate* l_pUpdateEvent = (SP2::Event::GEconomicHealthUpdate*)l_pEvent.get();

   l_pUpdateEvent->m_iSource     = g_SP2Client->Id();
   l_pUpdateEvent->m_iTarget     = SDK::Event::ESpecialTargets::Server;
   l_pUpdateEvent->m_iCountryId  = in_iCountryID;
   l_pUpdateEvent->m_bGetData    = true;

   g_Joshua.RaiseEvent(l_pEvent);

   return true;
}

bool GDataAccessLayerClient::FetchResourcesInfo(INT32 in_iCountryID)
{
   GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GResourcesUpdate);
   SP2::Event::GResourcesUpdate* l_pUpdate = (SP2::Event::GResourcesUpdate*)l_pEvent.get();

   l_pUpdate->m_iSource = g_SP2Client->Id();
   l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pUpdate->m_bGetData = true;
   l_pUpdate->m_iCountryID = in_iCountryID;
   l_pUpdate->m_iNbResources = 0;

   g_Joshua.RaiseEvent(l_pEvent);

   return true;
}

bool GDataAccessLayerClient::FetchCountryRanks(INT32 in_iCountryID)
{
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GGetCountryRanks);
   SP2::Event::GGetCountryRanks* l_pRanksEvent = (SP2::Event::GGetCountryRanks*)l_pEvent.get();
   l_pRanksEvent->CountryId((INT16)in_iCountryID);
   l_pRanksEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pRanksEvent->m_iSource   = g_SP2Client->Id();
   g_Joshua.RaiseEvent(l_pEvent);

   return true;
}

bool GDataAccessLayerClient::FetchCountriesRelations()
{
	SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GEventFetchCountriesRelations);
   SP2::Event::GEventFetchCountriesRelations* l_pRelationsEvent = (SP2::Event::GEventFetchCountriesRelations*)l_pEvent.get();
	l_pRelationsEvent->m_iCountryId = g_ClientDAL.ControlledCountryID();
   l_pRelationsEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pRelationsEvent->m_iSource   = g_SP2Client->Id();
   g_Joshua.RaiseEvent(l_pEvent);

   return true;
}

bool GDataAccessLayerClient::FetchConstitutionalFormInfo(INT32 in_iCountryID)
{
   GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GConstitutionalFormUpdate);
   SP2::Event::GConstitutionalFormUpdate* l_pUpdateEvent = (SP2::Event::GConstitutionalFormUpdate*)l_pEvent.get();

   l_pUpdateEvent->m_iSource = g_SP2Client->Id();
   l_pUpdateEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pUpdateEvent->GetData = true;
   l_pUpdateEvent->CountryId = (INT16)in_iCountryID;
   
   g_Joshua.RaiseEvent(l_pEvent);

   return true;
}

bool GDataAccessLayerClient::FetchInternalLawsInfo(INT32 in_iCountryID)
{
   GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GInternalLawsUpdate);
   SP2::Event::GInternalLawsUpdate* l_pUpdateEvent = (SP2::Event::GInternalLawsUpdate*)l_pEvent.get();

   l_pUpdateEvent->m_iSource = g_SP2Client->Id();
   l_pUpdateEvent->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pUpdateEvent->m_iCountryId = (INT16)in_iCountryID;
   l_pUpdateEvent->m_bGetData = true;

   g_Joshua.RaiseEvent(l_pEvent);

   return true;
}

/*!
 * Get the given country
 *   the country id = vector index + 1
 **/
const GCountry& GDataAccessLayerClient::Country(INT32 in_iID) const
{
   //Country id is 1 based
   gassert( (in_iID < (INT32)m_Countries.size() + 1) && (in_iID >= 1), "Invalid Country ID");

   return m_Countries[in_iID - 1];
}

/*!
 * Get the given country
 **/
const GCountry& GDataAccessLayerClient::Country(GString in_sName) const
{
   SP2::GCountries::const_iterator l_It = m_Countries.begin();
   while(l_It != m_Countries.end())
   {
      if(l_It->Name() == in_sName)
      {
         return *l_It;
      }
      l_It ++;
   }

   gassert(0, "Invalid Country Name");

   return *l_It;
}

const GCountry& GDataAccessLayerClient::CountryFromCode(GString in_sCode) const
{
   SP2::GCountries::const_iterator l_It = m_Countries.begin();
   while(l_It != m_Countries.end())
   {
      if(l_It->Code() == in_sCode)
      {
         return *l_It;
      }
      l_It ++;
   }

   gassert(0, "Invalid Country Name");

   return *l_It;
}

INT32 GDataAccessLayerClient::CountryID(GString in_sCountryCode)
{
   SP2::GCountries::iterator l_It = m_Countries.begin();
   while(l_It != m_Countries.end())
   {
      if(l_It->Code() == in_sCountryCode)
      {
         return l_It->Id();
      }
      l_It++;
   }

   return -1;
}

INT16 GDataAccessLayerClient::ControlledCountryID()
{
   return m_iControlledCountryID;
}

void GDataAccessLayerClient::ControlledCountryID(INT16 in_iControlledCountryID)
{
   m_iControlledCountryID = in_iControlledCountryID;
}


vector<GCountry> & GDataAccessLayerClient::Countries()
{
   return m_Countries;
}

SGenericDataHolder & GDataAccessLayerClient::GenericDataHolder()
{
   return m_GenericDataHolder;
}

GString GDataAccessLayerClient::CountryFlagSmallPath(UINT32 in_iID)
{
   //Get the Flag path
   GString m_sSelectedCountryFlagPath;
   if(m_sSelectedCountryFlagPath == L"")
   {
      if(!g_SP2Client->DAL().XMLValue(SP2::FLAG_SMALL_FOLDER, m_sSelectedCountryFlagPath) )
      {
         m_sSelectedCountryFlagPath = SP2::FLAG_SMALL_DEFAULT_FOLDER;      
      }
   }

   const GCountry& l_Country = Country(in_iID);

   return m_sSelectedCountryFlagPath + GString(l_Country.Flag() );
}


/*!
 * Set the combat that is displayed in the overall window
 **/
void GDataAccessLayerClient::DisplayedOverallCombat(const UINT32 in_iArenaID)
{
   m_iDisplayedOverallCombatID = in_iArenaID;
}
/*!
 * Get the combat that is displayed in the overall window
 **/
UINT32 GDataAccessLayerClient::DisplayedOverallCombat() const
{
   return m_iDisplayedOverallCombatID;
}

void GDataAccessLayerClient::DisplayedInfoCombat(const UINT32 in_iArenaID)
{
   m_iDisplayedInfoCombatID = in_iArenaID;
}

UINT32 GDataAccessLayerClient::DisplayedInfoCombat() const
{
   return m_iDisplayedInfoCombatID;
}

vector<SScreenOverlayInfo> & GDataAccessLayerClient::VisibleUnits()
{
   return m_VisibleUnits;
}

vector<SScreenOverlayInfo> & GDataAccessLayerClient::VisibleCombats()
{
   return m_VisibleCombats;
}

vector<SScreenOverlayInfo> & GDataAccessLayerClient::VisibleCities()
{
   return m_VisibleCities;
}

void GDataAccessLayerClient::SelectUnit(UINT32 in_iID, bool in_AddToSelection)
{   
   if (!in_AddToSelection)
      m_SelectedUnitsID.clear();
   m_SelectedUnitsID.insert(in_iID);
}

void GDataAccessLayerClient::UnselectUnit(UINT32 in_iID)
{
   m_SelectedUnitsID.erase(in_iID);
}

void GDataAccessLayerClient::UnselectUnits()
{
   m_SelectedUnitsID.clear();
}

const set<UINT32> & GDataAccessLayerClient::SelectedUnitsID() const
{
   return m_SelectedUnitsID;
}

void GDataAccessLayerClient::SelectCombat(UINT32 in_iID)
{
   m_iSelectedCombatID = in_iID;
}

UINT32 GDataAccessLayerClient::SelectedCombatID() const
{
   return m_iSelectedCombatID;
}

void GDataAccessLayerClient::ReceiveInitialData(SP2::Event::GSendData* in_pEvent)
{
   m_iNbRegion = (UINT16) in_pEvent->m_vRegionControlArray->size()- 1;
   m_vRegionCities.resize(m_iNbRegion + 1);
   g_ClientDCL.ThematicMapColorData.resize(m_iNbRegion + 1);;

   // Copy owner vector
   m_vRegionControl = *(in_pEvent->m_vRegionControlArray);

   // Build country region data
   m_vCountryControl.clear();
   m_vCountryControl.resize(NbCountry() + 1);

   for(UINT32 i = 0;i < m_vRegionControl.size();i ++)
   {
      m_vCountryControl[m_vRegionControl[i].m_iMilitary].first.insert(i);
      m_vCountryControl[m_vRegionControl[i].m_iPolitical].second.insert(i);
   }

   //For every region bombardment point, add them 
   for(multimap<UINT32,vector<GVector2D<REAL32> > >::const_iterator l_BombIt = in_pEvent->m_vBombardedRegionsAndPoints.begin();
                                                                  l_BombIt != in_pEvent->m_vBombardedRegionsAndPoints.end();
                                                                  l_BombIt++)
   {
      g_ClientDCL.OnBombardmentNotification(l_BombIt->first,true,l_BombIt->second);
   }

   m_AnnexedRegions = *(in_pEvent->m_Annexes);

   // Copy city list vector
   m_vCities = *(in_pEvent->m_vCities);
   m_vCityNames.resize(m_vCities.size() );

   // Build city region structure and fetch city names
   for(UINT32 i = 0;i < m_vCities.size();i ++)
   {
      const SCityInfo& l_Info = m_vCities[i];
      if(l_Info.m_iId != 0xFFFFFFFF)
      {
         gassert(i == l_Info.m_iId, "City ID is invalid");
         m_vRegionCities[l_Info.m_iRegionId].push_back(i);

         m_vCityNames[i] = GetString(l_Info.m_iNameStringID);
      }
   }

   m_vRegionNames = in_pEvent->m_vRegionNames;
   m_vRegionContinent = in_pEvent->m_vRegionContinent;
   m_vRegionGeoGroup = in_pEvent->m_vRegionGeoGroup;
	m_vRelations = in_pEvent->m_vRelations;

   // Copy capital list vector
   m_vCapitalsCityID = *(in_pEvent->m_vCapitals);

   // Copy country validity array
   m_pCountryValidityArray = new bool[NbCountry() + 1];
   Memory::Copy(m_pCountryValidityArray, in_pEvent->m_pConqueredCountries, NbCountry() + 1);

   vector<GCountry> & l_Countries = g_ClientDAL.Countries();
   for(INT32 i=1; i<=NbCountry(); i++)
   {
      l_Countries[i - 1].IsActive( m_pCountryValidityArray[i] );
   }

   gassert(m_vCapitalsCityID.size() == (UINT32) NbCountry() + 1, "Number of capitals is different from number of countries");

   // Copy treaty list
   m_Treaties = *(in_pEvent->m_Treaties);

   // Copy war list
   m_Wars = *(in_pEvent->m_Wars);

	m_CurrentWars = *(in_pEvent->m_CurrentWars);

   m_pDiplomaticArray = new UINT8[ (NbCountry() + 1) * (NbCountry() + 1) ];
   m_pTradeEmbargos = new bool[ (NbCountry() + 1) * (NbCountry() + 1) ];
   m_pWeaponTradeEmbargos = new bool[ (NbCountry() + 1) * (NbCountry() + 1) ];

   UpdateDiplomaticStatuses();

	UpdateWeaponTradeEmbargos();

   // Update units in training
   for(list<GUnitInTraining>::const_iterator it = in_pEvent->m_ListOfUnitsInTraining->begin();
       it != in_pEvent->m_ListOfUnitsInTraining->end();++ it)
   {
      SDK::Combat::GUnit* l_pUnit = g_Joshua.UnitManager().Unit(it->m_iUnitId);
      if(l_pUnit)
      {
         if(l_pUnit->Group()->OwnerId() == (UINT32) ControlledCountryID() )
         {
            m_UnitInTraining.insert(make_pair(it->m_iUnitId,*it) );
         }
      }
   }
 
   // Update game options
   g_ClientDAL.GameOptions() = in_pEvent->m_GameOptions;
}

GString GDataAccessLayerClient::RegionName(UINT32 in_iRegionID) 
{
   gassert(in_iRegionID < m_vRegionNames.size(),"region ID is not valid");
   return GetString(m_vRegionNames[in_iRegionID]);
}

/*!
 * Gets the region name string id
 * @param in_iRegionID: Id of the region
 * @return String table entry id for that region name, use GetString with that id to retrive the name or use directly GDataAccessLayerClient::RegionName(UINT32 in_iRegionID)
 **/
UINT32 GDataAccessLayerClient::RegionNameStringId(UINT32 in_iRegionID)
{
   gassert(in_iRegionID < m_vRegionNames.size(),"region ID is not valid");
   return m_vRegionNames[in_iRegionID];
}


void GDataAccessLayerClient::ChangeMilitaryControl(UINT32 in_iRegionID, UINT32 in_iNewControlID)
{
   ModifyMilitaryControl(in_iRegionID, in_iNewControlID);
}

void GDataAccessLayerClient::ChangePoliticalControl(UINT32 in_iRegionID, UINT32 in_iNewControlID)
{
   ModifyPoliticalControl(in_iRegionID, in_iNewControlID);
}

void GDataAccessLayerClient::NotifyMilitaryControlChanged()
{
   NotifyObserver((DesignPattern::GSubjectNotification)ENotifyWindowMsg::ConstitutionalForm, (void*)1);
}

/*
void GDataAccessLayerClient::MissileList(ENuclearMissileType::Enum in_eMissileType, vector<GNuclearMissile>& out_vNuclearMissiles) const
{
  switch(in_eMissileType)
   {
      case ENuclearMissileType::All:
      {
         out_vNuclearMissiles =  m_vMissileList;
         return;
      }
      case ENuclearMissileType::OnSubmarines:
      {
         for(vector<GNuclearMissile>::const_iterator l_It = m_vMissileList.begin();
             l_It != m_vMissileList.end();
             l_It++)
         {
            if(l_It->FromSubmarine())
               out_vNuclearMissiles.push_back(*l_It);
         }
         return;
      }
      case ENuclearMissileType::Standard:
      {
         for(vector<GNuclearMissile>::const_iterator l_It = m_vMissileList.begin();
             l_It != m_vMissileList.end();
             l_It++)
         {
            if(!l_It->FromSubmarine())
               out_vNuclearMissiles.push_back(*l_It);
         }
         return;
      }      
      case ENuclearMissileType::None:
      default:
      {
         return;
      }
   }
}

void GDataAccessLayerClient::MissileList(vector<GNuclearMissile> & in_vMissileList)
{
   m_vMissileList = in_vMissileList;
}
*/
const hash_map<UINT32, GLaunchedNuke>& GDataAccessLayerClient::LaunchedNukes() const
{
   return m_LaunchedNukes;
}

bool GDataAccessLayerClient::RemoveLaunchedNuke(UINT32 in_iNukeID)
{
   return (m_LaunchedNukes.erase(in_iNukeID) != 0);
}

bool GDataAccessLayerClient::AddLaunchedNuke(const GLaunchedNuke& in_NukeInfo)
{
   m_LaunchedNukes[in_NukeInfo.m_Missile.m_iID] = in_NukeInfo;

   return true;
}

bool GDataAccessLayerClient::LoadUnitDesign()
{
   GFile l_File;
   CXML_Util l_XMLUtil;
   //Get the Military node from the Client configuration file
   CTreeXMLNode* l_pMilitaryNode = l_XMLUtil.Search_Node(m_pConfigFile->Root(),MILITARY_NODE,GString(""),NULL,2);

   IF_RETURN(!l_pMilitaryNode,false);        
   CTreeXMLNode* l_pUnitTypesNode= l_XMLUtil.Search_Node(l_pMilitaryNode,UNIT_DESIGN_NODE,L"",NULL,1);   
   IF_RETURN(!l_pUnitTypesNode,false);   
   IF_RETURN(!g_Joshua.FileManager()->File(l_pUnitTypesNode->Data().m_value,l_File),false);

   return DesignVariationHolder.Load(l_File);
}

const GString& GDataAccessLayerClient::CityName(UINT32 in_iCityID) const
{
   gassert(in_iCityID < m_vCityNames.size(), "Invalid city ID for name");

   return m_vCityNames[in_iCityID];
}

void GDataAccessLayerClient::UpdateCity(const SCityInfo& in_ModifiedCity)
{
   if(in_ModifiedCity.m_iId < m_vCities.size() )
   {
      m_vCities[in_ModifiedCity.m_iId] = in_ModifiedCity;
   }
}

void GDataAccessLayerClient::UpdateRegionAnnex(const GRegionAnnex& in_ModifiedAnnex)
{
   m_AnnexedRegions[in_ModifiedAnnex.m_iAnnexedRegion] = in_ModifiedAnnex;
}

void GDataAccessLayerClient::RemoveRegionAnnex(UINT32 in_iRegionID)
{
   m_AnnexedRegions.erase(in_iRegionID);
}

void GDataAccessLayerClient::DestroyCountryEntity(UINT32 in_iCountryID)
{
   gassert(in_iCountryID <= NbCountry(), "Invalid country ID for country destroy");

   m_pCountryValidityArray[in_iCountryID] = false;

   ((SP2::GCountry&)Country(in_iCountryID)).IsActive(false);
}

void GDataAccessLayerClient::AddTreaty(const GTreaty& in_Treaty)
{
   set<UINT32> l_OldEligibles;
   if(m_Treaties.find(in_Treaty.ID()) != m_Treaties.end())
   {
      l_OldEligibles = m_Treaties[in_Treaty.ID() ].EligibleCountries();
   }
   m_Treaties[in_Treaty.ID() ] = in_Treaty;
   
   if(!in_Treaty.m_bEligibleDirty)
      m_Treaties[in_Treaty.ID() ].EligibleCountries(l_OldEligibles);
   
   UpdateDiplomaticStatuses();
   g_ClientDCL.UpdateCurrentCountryUnits();
}

void GDataAccessLayerClient::RemoveTreaty(UINT32 in_iTreatyID)
{
   m_Treaties.erase(in_iTreatyID);
   UpdateDiplomaticStatuses();
   g_ClientDCL.UpdateCurrentCountryUnits();
}

void GDataAccessLayerClient::AddWar(const GWar& in_War)
{
   m_CurrentWars[in_War.ID()] = in_War;
}

void GDataAccessLayerClient::RemoveWar(UINT32 in_iWarID)
{
   m_CurrentWars.erase(in_iWarID);
}

void GDataAccessLayerClient::AddWarPair(const SWarPair& in_NewWar)
{
   m_Wars.insert(in_NewWar);
}

void GDataAccessLayerClient::RemoveWarPair(const SWarPair& in_NewPeace)
{
   m_Wars.erase(in_NewPeace);
}

void GDataAccessLayerClient::UpdateWarStatus()
{
   UpdateDiplomaticStatuses();
   g_ClientDCL.UpdateCurrentCountryUnits();
}

bool GDataAccessLayerClient::RegionsCharacteristic(ECharacteristicToHighlight::Enum in_Charact, INT32 in_iCountryID)
{
   SDK::GGameEventSPtr l_Event;

   //Check if the thematic map painting can be processed locally
   bool l_bProcessedLocally = false;
   {      
      switch (in_Charact)
      {
         //Political Control
         case ECharacteristicToHighlight::PoliticalControl:
         {
            ThematicMapRequestPoliticalControl(in_iCountryID);
            l_bProcessedLocally = true;
            break;
         }
         //Country empire
         case ECharacteristicToHighlight::CountryEmpire:
         {
            ThematicMapRequestCountryEmpire(in_iCountryID);
            l_bProcessedLocally = true;
            break;
         }
         //Thematic map of the human controlled countries
         case ECharacteristicToHighlight::HumanControlledCountries:
         {
            ThematicMapRequestHumanControlledCountries();
            l_bProcessedLocally = true;
            break;
         }
      }
   }

   //If event could not be processed locally, request information from the server
   {
      if(!l_bProcessedLocally)
      {
         l_Event = CREATE_GAME_EVENT(Event::GGetRegionsCharacteristic);
         ((Event::GGetRegionsCharacteristic*)l_Event.get())->Characteristic = in_Charact;
         ((Event::GGetRegionsCharacteristic*)l_Event.get())->SelectedCountry = in_iCountryID;
         
         l_Event->m_iSource = g_Joshua.Client()->Id();
         l_Event->m_iWhen = 0;
         l_Event->m_iTarget = SDK::Event::ESpecialTargets::Server;
   
         g_Joshua.RaiseEvent(l_Event);
      }
   }

   m_iRecvCountryDataThreadHandle = 0;
   return true;
}

void GDataAccessLayerClient::InsertPendingNews(const News::GInfo& in_News)
{
   ESphere::Enum l_Type = ESphere::Demographic;

   switch(in_News.m_eCategory)
   {
   case SP2::News::ECategory::Economical:
      l_Type = ESphere::Economic;
      break;
   case SP2::News::ECategory::Military:
      l_Type = ESphere::Military;
      break;
   case SP2::News::ECategory::Political:
      l_Type = ESphere::Politic;
      break;
   }

   if(in_News.ToString().size())
   {
      if(g_ClientDDL.EventLogWindow() == NULL)
      {
         g_ClientDDL.EventLogWindowSpawn();         
      }
      g_ClientDDL.EventLogWindow()->AddEvent(in_News.ToString(), l_Type, g_ClientDAL.CapitalPosition(in_News.m_iCountryA), in_News.m_iCountryA, in_News.m_iCountryB);
   }

   
   //Check if the client advisor has something to say about that one
   //===============================================================
   switch(in_News.m_eType)
   {

      //Handle War declaration
      //case News::EType::DeclareWar:
      //{
      //   g_ClientDCL.ClientAdvisor().AddMessage(EClientAdvisorMsg::WarDeclaration,in_News.m_iCountryA,in_News.m_iCountryB);
      //   break;
      //}
      //Taken military control on
      case News::EType::TakenMilitaryControl:
      {
         g_ClientDCL.ClientAdvisor().AddMessage(EClientAdvisorMsg::TakenMilitaryControl,in_News.m_iCountryA,in_News.m_iCountryB,in_News.m_vStrings[0]);
         break;
      }
      //Country was nuked
      case News::EType::NuclearAttacks:
      {
         g_ClientDCL.ClientAdvisor().AddMessage(EClientAdvisorMsg::CountryWasNuked,in_News.m_iCountryA);
         break;
      }
      //Country is about to annex
      case News::EType::DeclareIntentionToAnnexRegions:
      {
         g_ClientDCL.ClientAdvisor().AddMessage(EClientAdvisorMsg::CountryToBeAnnexed,in_News.m_iCountryB,in_News.m_iCountryA);
         break;
      }

      default:
         break;
   }//end of check if client advisor has something to say about the news

   if(m_vPendingNews.size() < 3)
   {
      m_vPendingNews.push_back(in_News);
   }
   else
   {
      //Go through the list, and check if the news should be inserted or not.
      //vector<SNewsInfo>::iterator l_OutItr;
      vector<News::GInfo>::iterator l_Itr;
      for(l_Itr = m_vPendingNews.begin(); l_Itr != m_vPendingNews.end(); l_Itr++)
      {
         if( in_News.m_iPriority > (*l_Itr).m_iPriority )
         {
            m_vPendingNews.erase(l_Itr);
            m_vPendingNews.push_back(in_News);
            break;
         }
      }
   }
}

const News::GInfo* GDataAccessLayerClient::GetNextNews() const
{
   if(!m_vPendingNews.size())
      return NULL;
   return (const News::GInfo*)&m_vPendingNews[0];
}

void GDataAccessLayerClient::IterateNews()
{
   if(!m_vPendingNews.size())
      return;
   m_vPendingNews.erase(m_vPendingNews.begin());
}


/*!
 * Get all the arena infos in which we the given country is participating
 * @param in_iCountryID: ID of the country we want its arenas
 **/
vector<GArenaInfo*> GDataAccessLayerClient::ArenaInfos(UINT32 in_iCountryID) const
{
   vector<GArenaInfo*> l_vArenas;
   for(stdext::hash_map<UINT32, SDK::Combat::GArenaInfo*>::const_iterator l_It = g_Joshua.UnitManager().ArenaInfos().begin();
       l_It != g_Joshua.UnitManager().ArenaInfos().end() ;
       l_It++)
   {
      SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)l_It->second;
      SP2::GActorInfo* l_pActor     = l_pArenaInfo->Actor(in_iCountryID);
      if(l_pActor)
         l_vArenas.push_back(l_pArenaInfo);
   }
   return l_vArenas;
}


bool GDataAccessLayerClient::Cleanup()
{
   Countries().clear();
   NbCountry(0);

   m_vCities.clear();

   PathManager().Cleanup();
   m_vRegionNames.clear();
   m_vRegionContinent.clear();
   m_vRegionGeoGroup.clear();
	m_vRelations.clear();
   m_iControlledCountryID = -1;
   memset(&m_GenericDataHolder, 0, sizeof(SGenericDataHolder));
   m_GenericDataHolder.m_bSaveConfirmationAlwaysApply = false;
   m_GenericDataHolder.m_bSaveConfirmationDoNotShow   = false;

   m_iDisplayedOverallCombatID = NULL;
   m_iDisplayedInfoCombatID = NULL;

   m_SelectedUnitsID.clear();

   m_iSelectedCombatID = 0;
   m_iControlledCountryID = -1;

   DataMilitaryWindow.PreviouslyShown = false;
   DataMilitaryWindow.Left = -1;
   DataMilitaryWindow.Top = -1;
   DataEconomicWindow.PreviouslyShown = false;
   DataEconomicWindow.Left = -1;
   DataEconomicWindow.Top = -1;
   DataEconomicHealthWindow.PreviouslyShown = false;
   DataEconomicHealthWindow.Left = -1;
   DataEconomicHealthWindow.Top = -1;
   DataBudgetWindow.PreviouslyShown = false;
   DataBudgetWindow.Left = -1;
   DataBudgetWindow.Top = -1;
   DataResourcesWindow.PreviouslyShown = false;
   DataResourcesWindow.Left = -1;
   DataResourcesWindow.Top = -1;
   DataPoliticWindow.PreviouslyShown = false;
   DataPoliticWindow.Left = -1;
   DataPoliticWindow.Top = -1;
   DataConstitutionalForm.PreviouslyShown = false;
   DataConstitutionalForm.Left = -1;
   DataConstitutionalForm.Top = -1;
   DataInternalLawsWindow[0].PreviouslyShown = false;
   DataInternalLawsWindow[0].Left = -1;
   DataInternalLawsWindow[0].Top = -1;
   DataInternalLawsWindow[1].PreviouslyShown = false;
   DataInternalLawsWindow[1].Left = -1;
   DataInternalLawsWindow[1].Top = -1;
   DataInternalLawsWindow[2].PreviouslyShown = false;
   DataInternalLawsWindow[2].Left = -1;
   DataInternalLawsWindow[2].Top = -1;

   m_VisibleUnits.clear();
   m_VisibleCombats.clear();
   m_BombardedRegions.clear();
   m_VisibleCities.clear();
   //m_vMissileList.clear();
   m_vCityNames.clear();
   m_LaunchedNukes.clear();
   m_vPendingNews.clear();

   CleanScenarios();

   m_vRegionControl.clear();
   
   m_ListOfUnitsForSale.clear();
   m_CellsIdReady.clear();
   m_vCountryControl.clear();
   m_vRegionCities.clear();
   m_vCapitalsCityID.clear();
   m_AnnexedRegions.clear();
   m_Treaties.clear();
   m_Wars.clear();
   m_CurrentWars.clear();

   m_UnitInTraining.clear();
   m_TradesMail.clear();
	m_TreatyRefusalMail.clear();
   m_CombatsOverMail.clear();

   return true;
}

void GDataAccessLayerClient::OnSubjectChange(DesignPattern::GSubject &in_pSubject, const DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg )
{
   if(&in_pSubject == &g_Joshua.UnitManager())
   {
      switch(in_Notification)
      {
         //If a group was removed
         case Combat::c_notificationUnitGroupRemoved:
         {
            ValidateAndUpdateSelection();
            break;
         }
         case Combat::c_notificationFirstSynchro:
         {
            g_ClientDCL.InitialDataReceived();
            break;
         }
      }
   }
}

void GDataAccessLayerClient::ValidateAndUpdateSelection(void)
{
   // Go through current selection and validate ID against Unit Manager deleted list
   const set<UINT32> &l_RemovedUnitGroupSet = g_Joshua.UnitManager().RemovedUnitGroups();
   set<UINT32>::iterator l_itUnit = m_SelectedUnitsID.begin();
   set<UINT32> l_unitToRemove;
   while(l_itUnit != m_SelectedUnitsID.end())
   {

      set<UINT32>::const_iterator l_itRemovedUnit = l_RemovedUnitGroupSet.find(*l_itUnit);

      // If the unit doesnt exist anymore, delete it from the selection list
      if(l_itRemovedUnit != l_RemovedUnitGroupSet.end())
      {
         l_unitToRemove.insert(*l_itUnit);
      }
      l_itUnit++;
   }
   
   // Remove every elements
   l_itUnit = l_unitToRemove.begin();
   while(l_itUnit != l_unitToRemove.end())
   {
      UnselectUnit(*l_itUnit);
      l_itUnit++;
   }
}

GString GDataAccessLayerClient::FindTreatyName(ETreatyType::Enum in_eType)
{
   GString l_sType;

   switch(in_eType)
   {
   case ETreatyType::Alliance:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyAlliance);
      break;
   case ETreatyType::War:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyWar);
      break;
   case ETreatyType::MilitaryAccess:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyMilitaryAccess);
      break;
   case ETreatyType::EconomicPartnership:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyEconomicPartnership);
      break;
   case ETreatyType::CommonMarket:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyCommonMarket);
      break;
   case ETreatyType::EconomicAid:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyEconomicAid);
      break;
   case ETreatyType::AssumeForeignDebt:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyAssumeForeignDebt);
      break;
   case ETreatyType::EconomicEmbargo:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyEconomicEmbargo);
      break;
   case ETreatyType::WeaponTrade:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyWeaponTrade);
      break;
   case ETreatyType::WeaponTradeEmbargo:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyWeaponTradeEmbargo);
      break;
   case ETreatyType::HumanDevelopmentCollaboration:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyHumanDevelopmentCollaboration);
      break;
   case ETreatyType::CulturalExchanges:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyCulturalExchanges);
      break;
   case ETreatyType::ResearchPartnership:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyResearchPartnership);
      break;
   case ETreatyType::RequestMilitaryPresenceRemoval:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyRequestMilitaryPresenceRemoval);
      break;
   case ETreatyType::NobleCause:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyNobleCause);
      break;
   case ETreatyType::Annexation:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyAnnexation);
      break;
   case ETreatyType::FreeRegion:
      l_sType = g_ClientDAL.GetString(EStrId::TreatyFreeRegion);
      break;
   }

   return l_sType;
}

/*!
 * Updates the player country data with the new synchronized data from the network
 **/
void GDataAccessLayerClient::UpdatePlayerCountryData(GCountryDataItf * in_pCtryData)
{
   GCountryDataClient* l_pData                      = &(g_ClientDAL.m_PlayerCountryData);   
   SP2::GCountryDataClient* l_pCntryDataFromNetwork = (SP2::GCountryDataClient*)in_pCtryData;
   l_pData->CopyModifiedData(*l_pCntryDataFromNetwork);


   //If the Actually selected country is Ours
   if(g_ClientDAL.ControlledCountryID() == g_ClientDCL.SelectedCountryID())
   {
      //Notify Economic (does not contain budget) changes
      if(in_pCtryData->m_bEconomicDataDirty)
      {
         DesignPattern::GSubjectNotification l_Notify = (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::Economic;
         NotifyObserver(l_Notify, NULL);
      }

      //Notify Budget Changes
      if(in_pCtryData->m_bBudgetDataDirty)
      {
         DesignPattern::GSubjectNotification l_Notify = (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::Budget;
         NotifyObserver(l_Notify, NULL);
      }

      //Notify other data changes
      if(in_pCtryData->m_bOtherDataDirty)
      {
         DesignPattern::GSubjectNotification l_Notify = (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::Politic;
         NotifyObserver(l_Notify, NULL);

         l_Notify = (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::Military;
         NotifyObserver(l_Notify, NULL);

         l_Notify = (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::Advisor;
         NotifyObserver(l_Notify, NULL);

         l_Notify = (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::CovertActions;
         NotifyObserver(l_Notify, (void*)0xFFFFFFFF);

         l_Notify = (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::ResearchWindow;
         NotifyObserver(l_Notify, NULL);

         //Notify constitutional form changes
         if(l_pData->m_bConstitutionalFormDirty)
         {
            l_Notify = (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::ConstitutionalForm;
            NotifyObserver(l_Notify, NULL);
         }
      }

      //Notify resource data changes
      if(in_pCtryData->m_bResourceDataDirty)
      {
         DesignPattern::GSubjectNotification l_Notify = (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::Resources;
         NotifyObserver(l_Notify, NULL);
      }
   }//end if our country is the actually selected one.

   //Notify covert action cell data change.
   if(in_pCtryData->m_bCovertActionCellsDirty)
   {
      DesignPattern::GSubjectNotification l_Notify = (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::CovertActions;
      NotifyObserver(l_Notify, NULL);

      CheckCellsReadyForMission();
   }
}

void GDataAccessLayerClient::CheckCellsReadyForMission()
{
   if(g_ClientDDL.MailWindow())
   {
      vector<GCovertActionCell> & l_Cells = g_ClientDAL.m_PlayerCountryData.CovertActionCells();
      
      for(UINT32 i=0; i<l_Cells.size(); i++)
      {
         if(l_Cells[i].ActualState() == ECovertActionsCellState::ReadyToExecute) 
         {
            hash_map<UINT32, INT32>::iterator l_It = m_CellsIdReady.find( l_Cells[i].ID() );

            if(l_It != m_CellsIdReady.end())
            {
               continue;
            }

            if( !g_ClientDDL.CovertActionsWindow() || !g_ClientDDL.CovertActionsWindow()->Visible() )
            {
               INT32 l_iMailID = g_ClientDDL.MailWindow()->AddMail(l_Cells[i].Name() + GString(L" ") + g_ClientDAL.GetString(102279), GDataControlLayer::ShowMissionReady, (void*)l_Cells[i].ID());
               m_CellsIdReady.insert( make_pair((UINT32)l_Cells[i].ID(), (INT32)l_iMailID) );
            }

         }
         else if(l_Cells[i].ActualState() == ECovertActionsCellState::Active)
         {
            hash_map<UINT32, INT32>::iterator l_It = m_CellsIdReady.find( l_Cells[i].ID() );
            if(l_It != m_CellsIdReady.end())
            {
               //Remove the mail of the "Ready to execute" (if still present)
               if(g_ClientDDL.MailWindow()->MailExists(l_It->second))
                  g_ClientDDL.MailWindow()->RemoveMail(l_It->second);
               else
                  m_CellsIdReady.erase(l_It);
            }
         }
      }
   }
}

const vector<REAL32>& GDataAccessLayerClient::HistoryValues(EHistoryValues::Enum in_eValueType) const
{
   gassert( (UINT32) in_eValueType < (UINT32) EHistoryValues::TotalCount, "Asking invalid history values" );

   return m_pHistoryGraphValues[in_eValueType];
}

const vector<GHistoryMarker>& GDataAccessLayerClient::HistoryMarkers(EHistoryValues::Enum in_eValueType) const
{
   gassert( (UINT32) in_eValueType < (UINT32) EHistoryValues::TotalCount, "Asking invalid history markers" );

   return m_pHistoryGraphMarkers[in_eValueType];
}

INT32 GDataAccessLayerClient::HistoryMarkerOffset(EHistoryValues::Enum in_eValueType) const
{
   gassert( (UINT32) in_eValueType < (UINT32) EHistoryValues::TotalCount, "Asking invalid history marker offset" );

   return m_iHistoryMarkerTimeOffset;
}

void GDataAccessLayerClient::BeginHistoryValuesIterations()
{
   // Make sure nothing remains in the historical values
   for(UINT32 i = 0;i < EHistoryValues::TotalCount;i ++)
   {
      m_pHistoryGraphValues[i].resize(0);
      m_pHistoryGraphValues[i].reserve(c_iHistoryMaxValues * 2);

      m_pHistoryGraphMarkers[i].resize(0);
      m_pHistoryGraphMarkers[i].reserve(c_iHistoryMaxValues);
   }

   m_bBeginHistoryIterations = true;

   NotifyObserver(ENotifyWindowMsg::HistoryValuesOnly);
}

void GDataAccessLayerClient::StopHistoryValuesIterations()
{
   m_bBeginHistoryIterations = false;
   m_bHistoryIterationsIsRunning = false;
}

void GDataAccessLayerClient::IterateHistoryValues()
{
   // We use the speed to decide if we must start logging historical values
   // since the speed is 0 before the first server time synchronization
   REAL64 l_fTime = g_Joshua.GameTime();
   REAL64 l_fSpeed = g_Joshua.GameSpeed();

   if(l_fSpeed != 0)
   {
      if(m_bBeginHistoryIterations)
      {
         m_iHistoryMarkerTimeOffset = RoundToInt(g_Joshua.GameTime() / c_fHistoryUpdatePeriod);
         m_fLastHistoryValuesUpdate = c_fHistoryUpdatePeriod * (REAL32) m_iHistoryMarkerTimeOffset;
         m_bBeginHistoryIterations = false;
         m_bHistoryIterationsIsRunning = true;
      }

      // Only update when history period is elapsed
      // If we missed some values (because of lag), they will be added later 
      // (and we hope that the values will eventually stop being late...)
      if(m_bHistoryIterationsIsRunning && (l_fTime > m_fLastHistoryValuesUpdate + c_fHistoryUpdatePeriod) )
      {
         // Fetch all data

         // Politic
         m_pHistoryGraphValues[EHistoryValues::Approval].push_back( (REAL32) m_PlayerCountryData.GvtApproval() );
         m_pHistoryGraphValues[EHistoryValues::Stability].push_back( (REAL32) m_PlayerCountryData.GvtStability() );
         m_pHistoryGraphValues[EHistoryValues::Corruption].push_back( (REAL32) m_PlayerCountryData.CorruptionLevel() );
         m_pHistoryGraphValues[EHistoryValues::OverallRelations].push_back( (REAL32) m_PlayerCountryData.OverallDiplomaticRelations() );

         // Military
         m_pHistoryGraphValues[EHistoryValues::ConvForces].push_back( (REAL32) m_PlayerCountryData.ConvertionalForces() );
         m_pHistoryGraphValues[EHistoryValues::StratForces].push_back( (REAL32) m_PlayerCountryData.StrategicForces() );
         m_pHistoryGraphValues[EHistoryValues::Upkeep].push_back( (REAL32) m_PlayerCountryData.BudgetExpenseUnitUpkeep() );
         m_pHistoryGraphValues[EHistoryValues::Research].push_back( (REAL32) m_PlayerCountryData.ResearchInfo()->Total() );

         // Economic
         m_pHistoryGraphValues[EHistoryValues::EconomicHealth].push_back( (REAL32) m_PlayerCountryData.EconomicHealth() );
         m_pHistoryGraphValues[EHistoryValues::GDPCapita].push_back( (REAL32) m_PlayerCountryData.GDPPerCapita() );
         m_pHistoryGraphValues[EHistoryValues::Budget].push_back( (REAL32) (m_PlayerCountryData.BudgetRevenues() - m_PlayerCountryData.BudgetExpenses() ) );
         m_pHistoryGraphValues[EHistoryValues::ResProduction].push_back( (REAL32) m_PlayerCountryData.ResourceProduction() );
         m_pHistoryGraphValues[EHistoryValues::ResBalance].push_back( (REAL32) m_PlayerCountryData.ResourceBalance() );

         // Other
         m_pHistoryGraphValues[EHistoryValues::Population].push_back( (REAL32) m_PlayerCountryData.Population() );
         m_pHistoryGraphValues[EHistoryValues::HumanDevel].push_back( (REAL32) m_PlayerCountryData.HumanDevelopment() );
         m_pHistoryGraphValues[EHistoryValues::Infrastructures].push_back( (REAL32) m_PlayerCountryData.Infrastructure() );
         m_pHistoryGraphValues[EHistoryValues::Telecom].push_back( (REAL32) m_PlayerCountryData.TelecomLevel() );

         m_fLastHistoryValuesUpdate += c_fHistoryUpdatePeriod;

#ifdef GOLEM_DEBUG
         // All values should have been updated, so all vectors should have same count
         for(UINT32 i = 1;i < EHistoryValues::TotalCount;i ++)
         {
            gassert(m_pHistoryGraphValues[i].size() == m_pHistoryGraphValues[0].size(), 
               "History Graph Values should all contain same number of elements");
         }
#endif

         // Make sure there is never more than c_iHistoryMaxValues elements in each array
         // To make sure we do not make a buffer copy each iteration when history 
         // buffers are full, we wait until there is 1.5 x c_iHistoryMaxValues
         // elements before erasing begining of array.
         if(m_pHistoryGraphValues[0].size() > c_iHistoryMaxValues * 3 / 2)
         {
            UINT32 l_iRemoveCount = m_pHistoryGraphValues[0].size() - c_iHistoryMaxValues;
            for(UINT32 i = 0;i < EHistoryValues::TotalCount;i ++)
            {
               m_pHistoryGraphValues[i].assign(
                  m_pHistoryGraphValues[i].begin() + l_iRemoveCount, 
                  m_pHistoryGraphValues[i].end() );
            }

            m_iHistoryMarkerTimeOffset += l_iRemoveCount;

            // Remove all markers associated with the values that references
            // time before current first historical value (m_iHistoryMarkerTimeOffset)
            for(UINT32 i = 0;i < EHistoryValues::TotalCount;i ++)
            {
               // Find the first element with time greater or equal to offset
               vector<GHistoryMarker>::iterator l_FirstValid = m_pHistoryGraphMarkers[i].begin();

               while( (l_FirstValid != m_pHistoryGraphMarkers[i].end() ) &&
                      (l_FirstValid->m_iTime < m_iHistoryMarkerTimeOffset) )
               {
                  ++ l_FirstValid;
               }

               // Erase the elements by assigning the array to the remaining elements
               m_pHistoryGraphMarkers[i].assign(
                  l_FirstValid, 
                  m_pHistoryGraphMarkers[i].end() );
            }
         }

         NotifyObserver(ENotifyWindowMsg::HistoryValuesOnly);
      }
   }
}

void GDataAccessLayerClient::HistoryMarkersUpdate(SP2::Event::GHistoryMarkers* in_pEvent)
{
   m_iTotalMarkerReceivedSoFar++;

   GHistoryMarker l_Marker;
   l_Marker.m_iTime = (UINT32) (in_pEvent->m_fTime / c_fHistoryUpdatePeriod);

   // Add all markers to the values that are influenced
   for(UINT32 m = 0;m < in_pEvent->m_vMarkers.size();m ++)
   {
      const Event::GHistoryMarkers::GMarkerInfo& l_MarkerInfo = in_pEvent->m_vMarkers[m];

      l_Marker.m_eType = l_MarkerInfo.m_eType;
      l_Marker.m_fValue = l_MarkerInfo.m_fValue;

      for(UINT32 v = 0;v < c_HistoryMarkerValueCount[l_Marker.m_eType];v ++)
      {
         vector<GHistoryMarker>& l_vHistory = m_pHistoryGraphMarkers[c_HistoryMarkerValues[l_Marker.m_eType][v] ];

         // Find if a marker already exists for this action at the current time,
         // and merge if needed
         bool l_bInserted = false;
         if(c_HistoryMarkerIsMergeable[l_Marker.m_eType] )
         {
            for(INT32 i = l_vHistory.size() - 1;
                !l_bInserted && (i >= 0) && (l_vHistory[i].m_iTime == l_Marker.m_iTime);
                i ++)
            {
               if(l_vHistory[i].m_eType == l_Marker.m_eType)
               {
                  l_bInserted = true;
                  l_vHistory[i].m_fValue += l_Marker.m_fValue;
               }
            }
         }

         if(!l_bInserted)
         {
            // No merge occured, 
            // so append the marker to the current list for the value
            m_pHistoryGraphMarkers[c_HistoryMarkerValues[l_Marker.m_eType][v] ].push_back(l_Marker);
         }
      }
   }

   NotifyObserver(ENotifyWindowMsg::HistoryMarkersOnly);
}

GString GDataAccessLayerClient::HistoryMarkerToText(UINT32 in_iType, REAL32 in_fValue)
{
   // Convert value to string
   GString l_sValue;
   switch(c_HistoryMarkerValueTypes[in_iType] )
   {
   case EHistoryMarkerValueType::None:
      break;
   case EHistoryMarkerValueType::StringID:
      l_sValue = GetString(RoundToInt(in_fValue) );
      break;
   case EHistoryMarkerValueType::NumberReal:
      l_sValue = GString::FormatNumber( (REAL64) in_fValue, 
                                        " ", ".", "", "", 
                                        3, fabsf(in_fValue) > 100.f ? 0 : 1, true);
      break;
   case EHistoryMarkerValueType::NumberInt:
      l_sValue = GString(RoundToInt(in_fValue) );
      break;
   default:
      gassert(0, "Invalid Marker Value Type");
      break;
   }

   // Combine value & marker info text
   return GetString(EStrId::c_HistoryMarkers[in_iType] ).ReplaceNextPattern(l_sValue, STRING_PATTERN);
}

void GDataAccessLayerClient::NotifyTreatyChanged(const GTreatyNotifyData & in_Data)
{
   DesignPattern::GSubjectNotification l_Notify = (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::Treaties;
   NotifyObserver(l_Notify, (void*)&in_Data);
}

void GDataAccessLayerClient::NotifyWarChanged(UINT32 in_iWarID)
{
   DesignPattern::GSubjectNotification l_Notify = (DesignPattern::GSubjectNotification)SP2::ENotifyWindowMsg::War;
   NotifyObserver(l_Notify, (void*)in_iWarID);
}

list<UINT32> & GDataAccessLayerClient::UnitsForSale(void)
{
   return m_ListOfUnitsForSale;
}

bool GDataAccessLayerClient::IsUnitForSale(UINT32 in_iUnitId)
{
   return find(m_ListOfUnitsForSale.begin(),m_ListOfUnitsForSale.end(),in_iUnitId) != m_ListOfUnitsForSale.end();
}

void GDataAccessLayerClient::ThematicMapRequestPoliticalControl(UINT32 in_iCountryID)
{
   const vector<GRegionControl>& l_vControls = g_ClientDAL.RegionControlArray();

   SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(Event::GGetRegionsCharacteristicResult);
   ((Event::GGetRegionsCharacteristicResult*)l_Event.get())->Characteristic = ECharacteristicToHighlight::PoliticalControl;
   l_Event->m_iTarget = g_Joshua.Client()->Id();
   l_Event->m_iSource = g_Joshua.Client()->Id();
   l_Event->m_iWhen = 0;

   ((Event::GGetRegionsCharacteristicResult*)l_Event.get())->Results.resize(g_ClientDAL.NbRegion());
   //For each region
   UINT32 l_iNbRegion = (UINT32)g_ClientDAL.NbRegion();
   for (UINT32 i = 1; i <= l_iNbRegion; i++)
   {
      ((Event::GGetRegionsCharacteristicResult*)l_Event.get())->Results[i-1] = (int)l_vControls[i].m_iPolitical;
   }

   // Process event immediateley
   g_SP2Client->m_EventHandler.HandleGetRegionsCharacteristicResult(l_Event);
   return;
}

/*!
 * Request the country empire thematic map
 **/
void GDataAccessLayerClient::ThematicMapRequestCountryEmpire(UINT32 in_iCountryID)
{  
   SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(Event::GGetRegionsCharacteristicResult);
   Event::GGetRegionsCharacteristicResult* l_pUpdate = ((Event::GGetRegionsCharacteristicResult*)l_Event.get());
   l_pUpdate->Characteristic = ECharacteristicToHighlight::CountryEmpire;
   l_Event->m_iTarget        = g_Joshua.Client()->Id();
   l_Event->m_iSource        = g_Joshua.Client()->Id();
   l_Event->m_iWhen          = 0;

   l_pUpdate->Results.resize(g_ClientDAL.NbRegion());
   const UINT8 * l_DiplomaticStatuses = CountryDiplomaticStatuses(in_iCountryID);    

   const vector<GRegionControl>& l_vControls = g_ClientDAL.RegionControlArray();
   UINT32 l_iNbRegions = (UINT32)g_ClientDAL.NbRegion();

   //For each region
   for (UINT32 i = 1; i <= l_iNbRegions; i++)
   {
      EThematicMapRegionDescription::Enum l_eRegionFlag = EThematicMapRegionDescription::Unknown;

      //If we have military control and political control over the region
      if ((l_vControls[i].m_iPolitical == in_iCountryID) &&
         (l_vControls[i].m_iMilitary == in_iCountryID))
      {
         // Player Controlled
         l_eRegionFlag = EThematicMapRegionDescription::PlayerControlled; 
      }
      else
      {
         //If only military controlled
         if (l_vControls[i].m_iMilitary == in_iCountryID)
         {
            if (g_ClientDAL.RegionIsAnnexed(i))
               l_eRegionFlag = EThematicMapRegionDescription::TakenBy; 
            else
               l_eRegionFlag = EThematicMapRegionDescription::MilitaryControlled;
         }
         //If only political control
         else if (l_vControls[i].m_iPolitical == in_iCountryID)
         {
            if (g_ClientDAL.RegionIsAnnexed(i))
               l_eRegionFlag = EThematicMapRegionDescription::TakenFrom;
            else
               l_eRegionFlag = EThematicMapRegionDescription::PoliticallyControlled;
         }
         else
         {//No control
            switch ((EDiplomaticStatus::Enum)*(l_DiplomaticStatuses + l_vControls[i].m_iPolitical))
            {
            case EDiplomaticStatus::Allied:            
               l_eRegionFlag = EThematicMapRegionDescription::Allied;
               // Add allied countries into our additional results
               l_pUpdate->m_vAdditionalResults.push_back(l_vControls[i].m_iPolitical);
               break;
            case EDiplomaticStatus::MilitaryAccess:
               l_eRegionFlag = EThematicMapRegionDescription::MilitaryAccess;
               break;
            case EDiplomaticStatus::Hostile:
               l_eRegionFlag = EThematicMapRegionDescription::Hostile;
               break;
            }
         }
      }  
      l_pUpdate->Results[i-1] = (int)l_eRegionFlag;
   }
   // Process event immediateley
   g_SP2Client->m_EventHandler.HandleGetRegionsCharacteristicResult(l_Event);
   return;
}

//! Request the human controlled countries thematic map
void GDataAccessLayerClient::ThematicMapRequestHumanControlledCountries()
{
   //Create the game event
   SDK::GGameEventSPtr l_Event                    = CREATE_GAME_EVENT(Event::GGetRegionsCharacteristicResult);
   Event::GGetRegionsCharacteristicResult* l_pEvt = (Event::GGetRegionsCharacteristicResult*)l_Event.get();
   {
      l_pEvt->Characteristic  = ECharacteristicToHighlight::HumanControlledCountries;
      l_Event->m_iTarget      = g_Joshua.Client()->Id();
      l_Event->m_iSource      = g_Joshua.Client()->Id();
      l_Event->m_iWhen        = 0;
   }
   
   //Set -1 to all countries
   l_pEvt->Results.resize(g_ClientDAL.Countries().size());
   for (UINT32 i = 0; i < g_ClientDAL.Countries().size(); i++)
   {
      l_pEvt->Results[i];
   }

   //Set 1 to all human country
   for (vector<GChat::GChatPeer>::const_iterator itr = g_ClientDDL.GameLobby()->PlayersList().begin();
      itr != g_ClientDDL.GameLobby()->PlayersList().end(); itr++)
   {
      l_pEvt->Results[itr->CountryID - 1] = 1;
   }

   //Set o for the human controlled country
   l_pEvt->Results[g_ClientDAL.ControlledCountryID() - 1] = 1;         

   // Process event immediateley
   g_SP2Client->m_EventHandler.HandleGetRegionsCharacteristicResult(l_Event);
   return;
}

void GDataAccessLayerClient::CleanScenarios(void)
{
   // Clear Scenarios list
   m_vScenarios.clear();

   // No current scenario
   m_pScenario = GSmartPtr<GGameScenario>(NULL);

   if(g_ClientDDL.GameScenarioWindow())
      g_ClientDDL.GameScenarioWindow()->UpdateSelectedScenario();
}

void GDataAccessLayerClient::ReadScenarios(GString in_FileName)
{
   GFile             l_XMLFile;
   GString           l_sTempName;
   GXMLParser        l_XmlParser;
   GTree<GXMLNode>*  l_pScenarioFile;
   CTreeXMLNode*     l_pSceneNode;

   //Request the file to the file manager
   if(!g_Joshua.FileManager()->File(in_FileName,l_XMLFile))
   {
      g_Joshua.Log(SP2::ERROR_CANT_FIND_FILE + in_FileName,MSGTYPE_FATAL_ERROR);
      return;
   } 
  
   //Extract the file to a temp file on the disk so it can be parsed (xml)
   if(!l_XMLFile.Extract(l_sTempName))
   {
      g_Joshua.Log(SP2::ERROR_EXTRACT_FILE + l_XMLFile.Name(),MSGTYPE_FATAL_ERROR);
   }
   l_pScenarioFile = l_XmlParser.Parse(l_sTempName);
   l_XMLFile.Unextract(); //free the temp file
   if(!l_pScenarioFile)
   {
      g_Joshua.Log(SP2::ERROR_PARSING_XML_FILE + l_XMLFile.Name(),MSGTYPE_FATAL_ERROR);
   }

   gassert(l_pScenarioFile,"Config file is not valid or couldn't be read");
   if(l_pScenarioFile == NULL)
      return ;

   //Get the Scene node 
   CXML_Util l_XMLUtil;
   l_pSceneNode = l_XMLUtil.Search_Node(l_pScenarioFile->Root(),"Scenarios",GString(""),NULL,4);
   if(l_pSceneNode == NULL)
      return ;

   CTreeXMLNode* l_pScenarioDescription = l_pSceneNode->FirstChild();
   while(l_pScenarioDescription)
   {
      // We have a Scenario , create it
      GSmartPtr<GGameScenario> l_pScenario(new GGameScenario() );
      l_pScenario->Name(l_pScenarioDescription->Data().m_sName);

      // Get Scenario informations
      // Get Scenario title
      CTreeXMLNode* l_pScenarioTitleNode= l_XMLUtil.Search_Node(l_pScenarioDescription,"Title",L"",NULL,1);   
      if(l_pScenarioTitleNode)
         l_pScenario->Title(l_pScenarioTitleNode->Data().m_value);

      // Get Playable Countries
      CTreeXMLNode* l_pScenarioCountries= l_XMLUtil.Search_Node(l_pScenarioDescription,"Countries",L"",NULL,1);   
      if(l_pScenarioCountries)
      {
         l_pScenario->Country(l_pScenarioCountries->Data().m_value.ToINT32());
      }

      // Get Scenario Description
      CTreeXMLNode* l_pScenarioDescriptionNode= l_XMLUtil.Search_Node(l_pScenarioDescription,"Description",L"",NULL,1);   
      if(l_pScenarioDescriptionNode)
         l_pScenario->Description(l_pScenarioDescriptionNode->Data().m_value);

      // Get Scenario tips
      CTreeXMLNode* l_pScenarioTipsNode= l_XMLUtil.Search_Node(l_pScenarioDescription,"Tips",L"",NULL,1);   
      if(l_pScenarioTipsNode)
         l_pScenario->Tips(l_pScenarioTipsNode->Data().m_value);

      // The read objectives
      CTreeXMLNode* l_pScenarioObjectivesNode= l_XMLUtil.Search_Node(l_pScenarioDescription,"Objectives",L"",NULL,1);   
      if(l_pScenarioObjectivesNode)
         ReadObjectives(l_pScenarioObjectivesNode,l_pScenario);


      // Add it to the scenario list
      m_vScenarios.push_back(l_pScenario);

      // Get Next scenario
      l_pScenarioDescription = l_pSceneNode->NextChild();
   }
}

void GDataAccessLayerClient::Scenario(GSmartPtr<GGameScenario> in_pScenario)
{
   m_pScenario = in_pScenario;
}

GSmartPtr<GGameScenario> GDataAccessLayerClient::Scenario(void)
{
   return m_pScenario;
}


void GDataAccessLayerClient::ReadObjectives(CTreeXMLNode* in_pScenarioObjectivesNode,GGameScenario *in_pScenario)
{
   gassert(in_pScenarioObjectivesNode,"Invalid Objectives node pointer");
   gassert(in_pScenario,"Invalid scenario pointer");
   CXML_Util l_XMLUtil;

   CTreeXMLNode* l_pObjectiveType = in_pScenarioObjectivesNode->FirstChild();
   while(l_pObjectiveType)
   {
      // Create a new objective
      GGameObjective *l_pObjective = new GGameObjective;
      bool l_bObjectiveFound = true;

      // Read Title
      CTreeXMLNode* l_pObjectiveTitleNode= l_XMLUtil.Search_Node(l_pObjectiveType,"Title",L"",NULL,1);   
      if(l_pObjectiveTitleNode)
         l_pObjective->m_Title = l_pObjectiveTitleNode->Data().m_value.Trim();
      else
         l_pObjective->m_Title = "";

      // Read time limit
      CTreeXMLNode* l_pObjectiveTimeLimitNode= l_XMLUtil.Search_Node(l_pObjectiveType,"TimeLimit",L"",NULL,1);   
      if(l_pObjectiveTimeLimitNode)
         l_pObjective->m_fTimeOutTime = l_pObjectiveTimeLimitNode->Data().m_value.ToREAL32();
      else
         l_pObjective->m_fTimeOutTime = 0;

      // Them set objective type, and specific data(ParamX)
      if(l_pObjectiveType->Data().m_sName == "RaiseStability")
      {
         l_pObjective->m_eObjectiveType = EObjectiveType::RaiseStability;

         // Then get raise value
         CTreeXMLNode* l_pParam1= l_XMLUtil.Search_Node(l_pObjectiveType,"RaiseBy",L"",NULL,1);   
         l_pObjective->m_fValueToReach = l_pParam1->Data().m_value.ToREAL32()/100.0f; 
      }
      else
      if(l_pObjectiveType->Data().m_sName == "RaiseEconomicHealth")
      {
         l_pObjective->m_eObjectiveType = EObjectiveType::RaiseEconomicHealth;

         // Then get raise value
         CTreeXMLNode* l_pParam1= l_XMLUtil.Search_Node(l_pObjectiveType,"RaiseBy",L"",NULL,1);   
         l_pObjective->m_fValueToReach = l_pParam1->Data().m_value.ToREAL32()/100.0f; 
      }
      else
      if(l_pObjectiveType->Data().m_sName == "RaiseGDPPerCapita")
      {
         l_pObjective->m_eObjectiveType = EObjectiveType::RaiseGDPPerCapita;

         // Then get raise value
         CTreeXMLNode* l_pParam1= l_XMLUtil.Search_Node(l_pObjectiveType,"RaiseBy",L"",NULL,1);   
         l_pObjective->m_fValueToReach = l_pParam1->Data().m_value.ToREAL32()/100.0f; 
      }
      else
      if(l_pObjectiveType->Data().m_sName == "MeetResourcesNeeds")
      {
         l_pObjective->m_eObjectiveType = EObjectiveType::MeetResourcesNeeds;
      }
      else
      if(l_pObjectiveType->Data().m_sName == "InvadeCountry")
      {
         l_pObjective->m_eObjectiveType = EObjectiveType::InvadeCountry;

         // Then get country to invade
         CTreeXMLNode* l_pParam1= l_XMLUtil.Search_Node(l_pObjectiveType,"Country",L"",NULL,1);   
         l_pObjective->m_Buffer << l_pParam1->Data().m_value.ToINT32(); 
      }
      else
      if(l_pObjectiveType->Data().m_sName == "ChangeTreaty")
      {
         l_pObjective->m_eObjectiveType = EObjectiveType::BeInTreaty;

         // Then the treaty to be in(or out)
         CTreeXMLNode* l_pParam1= l_XMLUtil.Search_Node(l_pObjectiveType,"Treaty",L"",NULL,1);   
         l_pObjective->m_Buffer << l_pParam1->Data().m_value.ToINT32(); 

         // Get the side of the treaty you want to be in 
         CTreeXMLNode* l_pParam2= l_XMLUtil.Search_Node(l_pObjectiveType,"TreatySide",L"",NULL,1);   
         l_pObjective->m_Buffer << l_pParam2->Data().m_value.ToINT32(); 
      }
      else
      if(l_pObjectiveType->Data().m_sName == "AnnexCountry")
      {
         l_pObjective->m_eObjectiveType = EObjectiveType::AnnexCountry;

         // Then get country to invade
         CTreeXMLNode* l_pParam1= l_XMLUtil.Search_Node(l_pObjectiveType,"Country",L"",NULL,1);   
         l_pObjective->m_Buffer << l_pParam1->Data().m_value.ToINT32(); 
      }  
      else
      if(l_pObjectiveType->Data().m_sName == "ResourceControl")
      {
         l_pObjective->m_eObjectiveType = EObjectiveType::ResourceControl;

         // Resource Id to control
         CTreeXMLNode* l_pParam1= l_XMLUtil.Search_Node(l_pObjectiveType,"Resource",L"",NULL,1);   
         l_pObjective->m_Buffer << l_pParam1->Data().m_value.ToINT32(); 

         CTreeXMLNode* l_pParam2= l_XMLUtil.Search_Node(l_pObjectiveType,"TotalControl",L"",NULL,1);   
         l_pObjective->m_fValueToReach = l_pParam2->Data().m_value.ToREAL32()/100.0f; 

      }
      else
      if(l_pObjectiveType->Data().m_sName == "PureCommunistEconomy")
      {
         l_pObjective->m_eObjectiveType = EObjectiveType::PureCommunistEonomy;
      }
      else
      if(l_pObjectiveType->Data().m_sName == "GovernmentType")
      {
         l_pObjective->m_eObjectiveType = EObjectiveType::GovernementType;

         CTreeXMLNode* l_pParam1= l_XMLUtil.Search_Node(l_pObjectiveType,"Type",L"",NULL,1);   
         l_pObjective->m_Buffer << l_pParam1->Data().m_value.ToINT32(); 

      }
      else
      if(l_pObjectiveType->Data().m_sName == "BuildUnitCategory")
      {
         l_pObjective->m_eObjectiveType = EObjectiveType::BuildUnitCategory;

         CTreeXMLNode* l_pParam1= l_XMLUtil.Search_Node(l_pObjectiveType,"Category",L"",NULL,1);   
         l_pObjective->m_Buffer << l_pParam1->Data().m_value.ToINT32(); 

         CTreeXMLNode* l_pParam2= l_XMLUtil.Search_Node(l_pObjectiveType,"Quantity",L"",NULL,1);   
         l_pObjective->m_Buffer << l_pParam2->Data().m_value.ToINT32(); 

      }      
      else
      if(l_pObjectiveType->Data().m_sName == "LaunchNuke")
      {
         l_pObjective->m_eObjectiveType = EObjectiveType::LaunchANuke;

         //0 means any country
         CTreeXMLNode* l_pParam = l_XMLUtil.Search_Node(l_pObjectiveType,"TargetCountry",L"",NULL,1);   
         l_pObjective->m_Buffer << l_pParam->Data().m_value.ToINT32(); 
      }
      else
      {
         gassert(false,"This objective is not supported");
         l_bObjectiveFound = false;
      }

      if(l_bObjectiveFound)
      {
         // Add this objective to this scenario
         in_pScenario->AddObjective(l_pObjective);
      }

      l_pObjectiveType = in_pScenarioObjectivesNode->NextChild();
   }
}

vector<GSmartPtr<GGameScenario> >& GDataAccessLayerClient::Scenarios(void)
{
   return m_vScenarios;
}

void GDataAccessLayerClient::SendAdvisorSettings(EAdvisorStatus::Enum in_eEconomic, EAdvisorStatus::Enum in_ePolitic, EAdvisorStatus::Enum in_eMilitary,
                                                 SP2::GRequestObject* in_pRequestObj)
{  
   SDK::GGameEventSPtr l_pEvent = CREATE_GAME_EVENT(SP2::Event::GUpdateAdvisorSettings);
   SP2::Event::GUpdateAdvisorSettings* l_pUpdate = (SP2::Event::GUpdateAdvisorSettings*)l_pEvent.get();
   
   l_pUpdate->m_iSource = g_SP2Client->Id();
   l_pUpdate->m_iTarget = SDK::Event::ESpecialTargets::Server;
   l_pUpdate->m_eEconAdvisorStatus = in_eEconomic;
   l_pUpdate->m_ePoliAdvisorStatus = in_ePolitic;
   l_pUpdate->m_eMiliAdvisorStatus = in_eMilitary;
   if(in_pRequestObj)
      g_ClientDCL.RequestManager().AddRequest(in_pRequestObj, l_pUpdate);
   g_Joshua.RaiseEvent(l_pEvent);
}

void GDataAccessLayerClient::ApplyOptions()
{
   if( m_DataOptionsWindow.m_bGraphicDetailsDirty )
      ApplyOptionsGraphicDetails();

   if( m_DataOptionsWindow.m_bGraphicModeDirty )
      ApplyOptionsGraphicMode();

   if( m_DataOptionsWindow.m_bPlayerNameDirty )
      ApplyOptionsPlayer();

   if( m_DataOptionsWindow.m_bSoundDirty )
      ApplyOptionsSound();

   if( m_DataOptionsWindow.m_bGeometricDetailDirty )
      ApplyOptionsGeometricDetail();
}

void GDataAccessLayerClient::ApplyOptionsGraphicDetails()
{
   if( m_DataOptionsWindow.m_eTextureQuality != g_ClientDDL.BackgroundEarth()->TxtrQuality() )
   {
      g_ClientDDL.BackgroundEarth()->ChangeTxtrQuality(m_DataOptionsWindow.m_eTextureQuality);
   }

   if( m_DataOptionsWindow.m_bClouds != g_ClientDDL.BackgroundEarth()->CloudsEnabled() )
   {
      if( m_DataOptionsWindow.m_bClouds )
         g_ClientDDL.BackgroundEarth()->CloudsEnable();
      else
         g_ClientDDL.BackgroundEarth()->CloudsDisable();
   }

   if( m_DataOptionsWindow.m_bSun != g_ClientDDL.BackgroundEarth()->SunEnabled() )
   {
      if( m_DataOptionsWindow.m_bSun )
         g_ClientDDL.BackgroundEarth()->SunEnable();
      else
         g_ClientDDL.BackgroundEarth()->SunDisable();
   }

   if( m_DataOptionsWindow.m_bMoon != g_ClientDDL.BackgroundEarth()->MoonEnabled() )
   {
      if( m_DataOptionsWindow.m_bMoon )
         g_ClientDDL.BackgroundEarth()->MoonEnable();
      else
         g_ClientDDL.BackgroundEarth()->MoonDisable();
   }

   if( m_DataOptionsWindow.m_bDayCycle != g_ClientDDL.BackgroundEarth()->DayNightCycleEnabled() )
   {
      if( m_DataOptionsWindow.m_bDayCycle )
         g_ClientDDL.BackgroundEarth()->DayNightCycleEnable();
      else
         g_ClientDDL.BackgroundEarth()->DayNightCycleDisable();
   }
 }

void GDataAccessLayerClient::ApplyOptionsSound()
{

	REAL32 l_fSVolume = m_DataOptionsWindow.m_fSoundVolume;
   if( l_fSVolume )
   {               
      REAL32 l_fSoundVolume = g_Joshua.SoundPlayer()->Get_Sound_Volume( g_SP2Client->SoundBox()->SoundContext(ESoundContext::Left)->Id() );
      l_fSoundVolume = g_Joshua.SoundPlayer()->Get_Sound_Volume( g_SP2Client->SoundBox()->SoundContext(ESoundContext::Center)->Id() );
      l_fSoundVolume = g_Joshua.SoundPlayer()->Get_Sound_Volume( g_SP2Client->SoundBox()->SoundContext(ESoundContext::Right)->Id() );

      if( l_fSoundVolume == 0.f )
      {
         CSFX_Player_Init_Mode initMode;
         initMode.m_BitRate      = 16;
         initMode.m_NbChannels   = 2;
         initMode.m_SamplingFreq = 44100;
         initMode.m_DeviceIdx    = 0;

         // If the Init fails, its fine, it probably means we don't have a sound card here...
         g_Joshua.SoundPlayer()->Init(initMode);
      }
   }
   else
   {
      g_Joshua.SoundPlayer()->Close();
   }

   if( g_SP2Client->SoundBox() && g_SP2Client->SoundBox()->SoundContext(ESoundContext::Center) && g_Joshua.SoundPlayer() )
   {
      g_Joshua.SoundPlayer()->Set_Sound_Volume( g_SP2Client->SoundBox()->SoundContext(ESoundContext::Left)->Id(), l_fSVolume );
      g_Joshua.SoundPlayer()->Set_Sound_Volume( g_SP2Client->SoundBox()->SoundContext(ESoundContext::Center)->Id(), l_fSVolume );
      g_Joshua.SoundPlayer()->Set_Sound_Volume( g_SP2Client->SoundBox()->SoundContext(ESoundContext::Right)->Id(), l_fSVolume );
   }

	REAL32 l_fMVolume = m_DataOptionsWindow.m_fMusicVolume;
	g_SP2Client->AdaptiveMusicPlayer()->SetVolume(l_fMVolume);

}

void GDataAccessLayerClient::ApplyOptionsGraphicMode()
{
	//Set texture detailed
}

void GDataAccessLayerClient::ApplyOptionsGeometricDetail()
{
   g_ClientDDL.BackgroundEarth()->ChangeEarthDetail(m_DataOptionsWindow.m_eGeometricDetail);
	g_ClientDDL.BackgroundEarth()->SetAtlasZoomLevel(m_DataOptionsWindow.m_eZoomLevel);
}

void GDataAccessLayerClient::ApplyOptionsPlayer()
{
	g_ClientDAL.PlayerName(g_ClientDAL.m_DataOptionsWindow.m_sPlayerName);
}

GString GDataAccessLayerClient::StringReplaceNewLineChar(GString in_sText)
{
   UINT32 l_iPos = in_sText.find(L"\\n");
   while (l_iPos != string::npos)
   {
      in_sText.replace(l_iPos, 2, L"\n");
      l_iPos = in_sText.find(L"\\n", l_iPos);
   }

   return in_sText;
}

void GDataAccessLayerClient::PlayerName(const GString& in_sNewPlayerName)
{
   m_sPlayerName = in_sNewPlayerName;
}

void GDataAccessLayerClient::CountryActivated(INT32 in_iCountryID, bool in_bActivated)
{
   m_Countries[in_iCountryID - 1].IsActive(in_bActivated);
}

GString GDataAccessLayerClient::MilitaryStatusToString(EMilitaryStatus::Enum in_eStatus, const SP2::GUnit* in_pUnit)
{	
	if(!in_pUnit)
		return L"";

	const hash_map<UINT32,GUnitInTraining>& l_Training = g_ClientDAL.UnitInTraining();

   switch(in_eStatus)
   {
   case EMilitaryStatus::Parked:
      return g_ClientDAL.GetString( EStrId::MilitaryStatusParked );
   case EMilitaryStatus::Fortified:
      return g_ClientDAL.GetString( EStrId::MilitaryStatusFortified );
   case EMilitaryStatus::Attacking:
      return g_ClientDAL.GetString( EStrId::MilitaryStatusAttacking );
   case EMilitaryStatus::Training:
      return g_ClientDAL.GetString( EStrId::MilitaryStatusTraining );
   case EMilitaryStatus::Upgrading:
      return g_ClientDAL.GetString( EStrId::MilitaryStatusUpgrading );
   case EMilitaryStatus::ForSale:
      return g_ClientDAL.GetString( EStrId::MilitaryStatusForSale );
   case EMilitaryStatus::InTransit:
      return g_ClientDAL.GetString( EStrId::MilitaryStatusInTransit );
   case EMilitaryStatus::ReadyToDeploy:
	{
		if(g_ClientDAL.IsUnitForSale(in_pUnit->Id()))
			return g_ClientDAL.GetString( EStrId::MilitaryStatusForSale );
		if(l_Training.count(in_pUnit->Id()) > 0)
			return g_ClientDAL.GetString( EStrId::MilitaryStatusTraining );
      return g_ClientDAL.GetString( EStrId::MilitaryStatusReadyToDeploy );
	}
   case EMilitaryStatus::Ready:
   default:
      return g_ClientDAL.GetString( EStrId::MilitaryStatusReady );
   }
}

void GDataAccessLayerClient::ReceiveCountryStaticData(SP2::Event::GReceiveCountryStaticData* in_pData)
{
   m_vStringTableIdsReligions = in_pData->m_vStringTableIdsReligions;
   m_vStringTableIdsLanguages = in_pData->m_vStringTableIdsLanguages;
   m_vStringTableIdsPoliticalParties = in_pData->m_vStringTableIdsPoliticalParties;
   m_vStringTableIdsGvtType = in_pData->m_vStringTableIdsGvtType;
   m_vStringTableIdsStatus = in_pData->m_vStringTableIdsStatus;
   m_vStringTableIdsResources = in_pData->m_vStringTableIdsResources;

   __super::LoadCountryData();
}


hash_map<UINT32,GDisplayedBombardedRegion>& GDataAccessLayerClient::BombardedRegions()
{
   return m_BombardedRegions;
}


bool GDataAccessLayerClient::GameIsOver(void)
{

   return m_bGameIsOver;
}

void GDataAccessLayerClient::GameIsOver(bool in_bIsGameOver)
{
   m_bGameIsOver = in_bIsGameOver;
}

/*!
* Add a new Trade Mail and returns its mail id
**/
INT32 GDataAccessLayerClient::AddTreatyRefusalMail(const SP2::GTreatyRefusalDetails & in_TreatyDetails)
{
	// ensure mail window available
   if(!g_ClientDDL.MailWindow())
      g_ClientDDL.MailWindowSpawn();

	// Retrieve our next mail id
   INT32 l_iID = g_ClientDDL.MailWindow()->NextMailID();
   // add into our trade mails array
   m_TreatyRefusalMail[l_iID] = in_TreatyDetails;
   // add the mail in mail window
	GString l_sName = in_TreatyDetails.TreatyName() + g_ClientDAL.GetString(102318);
   g_ClientDDL.MailWindow()->AddMail(l_sName, GDataControlLayer::ShowTreatyRefusal, (void*)l_iID);

	return l_iID;
}

/*!
* Remove a Trade Mail
**/
void GDataAccessLayerClient::RemoveTreatyRefusalMail(INT32 in_MailID)
{
   // erase it from the hashmap
   m_TreatyRefusalMail.erase(in_MailID);
}

/*!
* Add a new Trade Mail and returns its mail id
**/
INT32 GDataAccessLayerClient::AddTradeMail(const SP2::GTradeData & in_Trade)
{
   // ensure mail window available
   if(!g_ClientDDL.MailWindow())
      g_ClientDDL.MailWindowSpawn();
   // Retrieve the trade text string to display
   GString l_strTrade;
   if(in_Trade.Status() == ETradeStatus::Accepted)
   {
      l_strTrade = g_ClientDAL.GetString(SP2::TradeStrings::c_StrIdTradeAccepted);
   }
   else if(in_Trade.Status() == ETradeStatus::Refused)
   {
      l_strTrade = g_ClientDAL.GetString(SP2::TradeStrings::c_StrIdTradeRefused);
   }
   else
   {
      l_strTrade = g_ClientDAL.GetString(SP2::TradeStrings::c_StrIdTradeProposed);
   }
   // Retrieve our next mail id
   INT32 l_iID = g_ClientDDL.MailWindow()->NextMailID();
   // add into our trade mails array
   m_TradesMail[l_iID] = in_Trade;
   // add the mail in mail window
   g_ClientDDL.MailWindow()->AddMail(l_strTrade, GDataControlLayer::ShowTrade, (void*)l_iID);
   // return our associated mail id
   return l_iID;
}

/*!
* Remove a Trade Mail
**/
void GDataAccessLayerClient::RemoveTradeMail(INT32 in_MailID)
{
   // erase it from the hashmap
   m_TradesMail.erase(in_MailID);
}

/*!
* Do we have a trade mail pending with this id ?
**/
bool GDataAccessLayerClient::GotTradeMailWithId(INT32 in_MailID)
{
   return (m_TradesMail.count(in_MailID) > 0);
}

/*!
* Add add new combat over mail and returns its mail id
**/
INT32 GDataAccessLayerClient::AddCombatOverMail(const SP2::GArenaInfo &in_ArenaInfo)
{
   // ensure mail window available
   if(!g_ClientDDL.MailWindow())
      g_ClientDDL.MailWindowSpawn();
   // Retrieve the battle mail text to display
   GString l_sBattleMail;
   // bool to know if we won
   bool l_bWon = false;
   switch(in_ArenaInfo.m_eWinnerSide)
   {
      // attackers won
      case ECombatSide::Attacker:
      {
         // were we in the attackers ?
         if (find(in_ArenaInfo.m_vAttackerCountriesList.begin(), in_ArenaInfo.m_vAttackerCountriesList.end(),
                  g_ClientDAL.ControlledCountryID()) != in_ArenaInfo.m_vAttackerCountriesList.end())
         {
            g_Joshua.PassSteamAchievement(3);
            l_bWon = true;
         }
      }
      break;
      // defenders won
      case ECombatSide::Defender:
      {
         // were we in the defenders ?
         if (find(in_ArenaInfo.m_vDefenderCountriesList.begin(), in_ArenaInfo.m_vDefenderCountriesList.end(),
                  g_ClientDAL.ControlledCountryID()) != in_ArenaInfo.m_vDefenderCountriesList.end())
         {
            g_Joshua.PassSteamAchievement(3);
            l_bWon = true;
         }
      }
      break;
   }
   // append result of the battle
   if(l_bWon)
      l_sBattleMail += g_ClientDAL.GetString(100401);
   else
      l_sBattleMail += g_ClientDAL.GetString(100402);
   // format it to look cool
   l_sBattleMail += " : ";
   // append region in which battle happened
   if(in_ArenaInfo.m_iRegionID == 0)
      l_sBattleMail += g_ClientDAL.GetString(102255);
   else
      l_sBattleMail += g_ClientDAL.GetString(102254) + L" " + g_ClientDAL.RegionName(in_ArenaInfo.m_iRegionID);
   // Retrieve our next mail id
   INT32 l_iID = g_ClientDDL.MailWindow()->NextMailID();
   // add into our combats over mail array
   m_CombatsOverMail[l_iID] = in_ArenaInfo;
   // add the mail in mail window
   g_ClientDDL.MailWindow()->AddMail(l_sBattleMail, GDataControlLayer::ShowCombatOver, (void*)l_iID);
   // return our associated mail id
   return l_iID;
}

/*!
* Remove a combat over Mail
**/
void GDataAccessLayerClient::RemoveCombatOverMail(INT32 in_MailID)
{
   // erase it from the hashmap
   m_CombatsOverMail.erase(in_MailID);
}
 
/*!
* Do we have a combat over mail pending with this id ?
**/
bool GDataAccessLayerClient::GotCombatOverMailWithId(INT32 in_MailID)
{
   return (m_CombatsOverMail.count(in_MailID) > 0);
}

REAL32 GDataAccessLayerClient::RelationWithCountry(UINT32 in_iCountryID)
{
	if(in_iCountryID >= m_vRelations.size() ||
		in_iCountryID < 0)
		return 0.f;

	return m_vRelations[in_iCountryID];
}

void GDataAccessLayerClient::SetRelations(SP2::Event::GEventFetchCountriesRelations* in_pEvent)
{
	m_vRelations = in_pEvent->m_vRelationsValues;
}
