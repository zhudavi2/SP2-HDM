/*************************************************************
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

#include "../Include/sp2_country_data_holder.h"

// Uncomment following line to log duplicates found during lad of military units
//#define LOG_DUPLICATES

GDataAccessLayerServer::GDataAccessLayerServer()
{
	m_pCountryData                       = NULL;
	m_pHumanDevelopmentBonus             = NULL;
	m_pEconomicPartnershipBonus          = NULL;
   m_pConfigFile                        = NULL;
	m_Regions                            = NULL;
   m_CombatEfficiencyTableGun           = NULL;
   m_CombatEfficiencyTableMissile       = NULL;
   m_fAMDSResearchLastCheckClock        = 0;
   m_fAMDSShortRangeInterceptionSuccess = 0;
   m_fAMDSLongRangeInterceptionSuccess  = 0;
   m_fNuclearMissileSpeed               = 0;
	m_iNextTreatyID                      = 0;
	m_iNextWarID								 = 0;
   m_fNextRelationHistoryUpdateGameTime = 0;
	m_iFirstCountryThatNuked				 = 0;
   m_pRelationTable                     = NULL;
	m_pRelationTableExpected				 = NULL;
	m_pRelationTableModifier				 = NULL;
   m_pTradeEmbargos                     = NULL;
   m_pDiplomaticArray                   = NULL;
   m_pWeaponTradeEmbargos               = NULL;
   m_pCountryValidityArray              = NULL;

   m_fMilitaryResearchStandardBudget    = -1.f;
   Memory::Set(m_MilitaryResearchTimeInDays,0,c_iMaxResearchLevel);
	m_DecisionHistory.clear();
}

GDataAccessLayerServer::~GDataAccessLayerServer()
{   
	SAFE_DELETE_ARRAY(m_pCountryData);
	SAFE_DELETE_ARRAY(m_pHumanDevelopmentBonus);
	SAFE_DELETE_ARRAY(m_pEconomicPartnershipBonus);
	SAFE_DELETE_ARRAY(m_Regions);
}

GDatabase* GDataAccessLayerServer::LoadDatabase(const GString &in_sDBFilename)
{
   g_Joshua.Database()->Shutdown();

   GFile l_DatabaseFile;
   g_Joshua.FileManager()->File(in_sDBFilename,l_DatabaseFile);

   // Extract the file
   GString l_FilePath;
   if(!l_DatabaseFile.Extract(l_FilePath) )
      return NULL;


   if(g_Joshua.Database()->Init(l_FilePath) != DB_NO_ERROR)
   {
      g_Joshua.Log(L"Database initialization failed",MSGTYPE_CRITICAL_ERROR);
      return NULL;
   }

   return g_Joshua.Database();
}

void GDataAccessLayerServer::CalculateCountryInfo()
{

   multimap<REAL32,INT32> l_mPoliticRank;
   multimap<REAL32,INT32> l_mEconomicRank;
	multimap<REAL32,INT32> l_mMilitaryRank;	

   //For each country 
   for(UINT32 i = 0 ; i < (UINT32)NbCountry() ; i++)
   {      
      GCountryData* l_pCountryData = CountryData(i+1);      			

		if(!l_pCountryData->Activated())
		{
			multimap<REAL32,INT32>::value_type l_Value(0.f, i + 1);
			l_mPoliticRank.insert(l_Value);
			l_mEconomicRank.insert(l_Value);
			l_mMilitaryRank.insert(l_Value);
			continue;
		}
			
      
      //Calculate its politic score
      {
			REAL32 l_fCountryScore = ((l_pCountryData->GvtStability()+l_pCountryData->GvtApproval()) / 2.f) * (1.f - l_pCountryData->CorruptionLevel()) * l_pCountryData->GvtType();
			l_pCountryData->PoliticStrength(l_fCountryScore);

         multimap<REAL32,INT32>::value_type l_Value(l_fCountryScore, i + 1);
         l_mPoliticRank.insert(l_Value);
      }

      //Compute its economic score
      {         
         REAL32 l_fCountryScore = (REAL32)l_pCountryData->GDPValue() * l_pCountryData->EconomicHealth();
			multimap<REAL32,INT32>::value_type l_Value2(l_fCountryScore,i + 1);				
			l_pCountryData->EconomicStrength(l_fCountryScore);		
         l_mEconomicRank.insert(l_Value2);
      }
		//Calculate its military score
		{
			REAL32 l_fTotalUnitsValue = TotalUnitsValue(i+1) + TotalMissileValue(i+1);			
			l_pCountryData->MilitaryStrength(l_fTotalUnitsValue);	
         multimap<REAL32,INT32>::value_type l_Value3(l_fTotalUnitsValue, i + 1);
         l_mMilitaryRank.insert(l_Value3);
		}
   }


   //Write the ranks
   {
      multimap<REAL32,INT32>::iterator it = l_mPoliticRank.begin();
      multimap<REAL32,INT32>::iterator it2 = l_mEconomicRank.begin();
		multimap<REAL32,INT32>::iterator it3 = l_mMilitaryRank.begin();
      INT16 l_iRank = NbCountry();
      while(it != l_mPoliticRank.end())
      {
         CountryData(it->second)->PoliticRank(l_iRank);			
         it++;
         l_iRank--;
      }

      l_iRank = NbCountry();
      while(it2 != l_mEconomicRank.end())
      {
         CountryData(it2->second)->EconomicRank(l_iRank);
         it2++;
         l_iRank--;
      }

		l_iRank = NbCountry();
      while(it3 != l_mMilitaryRank.end())
      {
			CountryData(it3->second)->MilitaryRank(l_iRank);         
         it3++;
         l_iRank--;
      }
   }
}


/*!
 * Shutdown
 * @return true if everything successfully freed, false otherwise
 **/
bool GDataAccessLayerServer::Shutdown()
{
   GDALInterface::Shutdown();

   g_Joshua.Database()->Shutdown();

   return true;
}

DB::EError GDataAccessLayerServer::DBGetCountryList(GTable &out_RSet)
{
	return g_Joshua.Database()->ExecuteQuery("SELECT id,country.name_stid,country.code,country.flag FROM country", out_RSet);
}

set<INT32> GDataAccessLayerServer::GetAffectedCountries(const GVector2D<REAL32> & in_Center, REAL32 radius)
{
   GTable l_Table;
   set<INT32> l_AffectedCountries;   

   g_Joshua.QueryDB("SELECT region.country_id, longitude, latitude FROM region, cities WHERE region.region_id = cities.region_id", l_Table);
   
   for (UINT32 i = 0; i < l_Table.RowCount(); i++)
   {
      GRow & l_Row = *l_Table.Row(i);
      INT32 l_CountryId;
      l_Row.Cell(0)->GetData(l_CountryId);
      if (l_AffectedCountries.find(l_CountryId) == l_AffectedCountries.end())
      {         
         GVector2D<REAL32> l_CountryPos;
         l_Row.Cell(1)->GetData(l_CountryPos.x);
         l_Row.Cell(2)->GetData(l_CountryPos.y);
         
         if (l_CountryPos.Distance(in_Center) <= radius)
            l_AffectedCountries.insert(l_CountryId);
      }
   }   
   return l_AffectedCountries;
}

/*
set<pair<INT32,INT32> > GDataAccessLayerServer::GetAffectedRegionsAndCities(const GVector2D<REAL32> & in_Center, REAL32 radius)
{
   DB::GTable l_Table;
   set<pair<INT32,INT32> > l_AffectedCities;   
   g_Joshua.QueryDB("SELECT id, longitude, latitude, region_id FROM cities", l_Table);
   
   for (UINT32 i = 0; i < l_Table.RowCount(); i++)
   {
      GRow & l_Row = *l_Table.Row(i);
      INT32 l_CityId, l_Region;
      GVector2D<REAL32> l_CityPos;
      l_Row.Cell(0)->GetData(l_CityId);      
      l_Row.Cell(1)->GetData(l_CityPos.x);
      l_Row.Cell(2)->GetData(l_CityPos.y);
      l_Row.Cell(4)->GetData(l_Region);
      
      if (l_CityPos.Distance(in_Center) <= radius)
         l_AffectedCities.insert(pair<INT32,INT32>(l_CityId,l_Region));
   }
   
   return l_AffectedCities;
}
*/

/*!
 * Load the unit designs of the game
 **/
bool GDataAccessLayerServer::LoadUnitDesigns()
{
   GTable l_Results;
   GSString l_sSelectQuery("select * from design");
   DB::EError e_Error = g_Joshua.QueryDB(l_sSelectQuery,l_Results);
   if(e_Error != DB_NO_ERROR)
   {
      g_Joshua.Log(L"Query error when loading the unit designs");
      return false;
   }

   UINT32 l_iID;
   UINT32 l_iDesignerID;
   INT32 l_iNameID;
	GString l_sName;
   UINT8 l_iSpeedAndManeuverability;
   UINT8 l_iSensors;
   UINT8 l_iGunRange;
   UINT8 l_iGunPrecision;
   UINT8 l_iGunDamage;
   UINT8 l_iMissilePayload;
   UINT8 l_iMissileRange;
   UINT8 l_iMissilePrecision;
   UINT8 l_iMissileDamage;
   UINT8 l_iStealth;
   UINT8 l_iCounterMesures;
   UINT8 l_iArmor;
   INT32 l_iUnitType;
   INT16 l_iPiece[c_iMaxDesignPieceCount];
   INT16 l_iTexture;

   INT32             l_iTempValue;
   INT16             l_iTempValue16;
   DB::GRow*         l_pRow;
   //For each design, fill its fields with the correct values
   UINT32 l_iRowCount = l_Results.RowCount();
   for(UINT32 i = 0 ; i < l_iRowCount  ;i++)
   {
      l_pRow = l_Results.Row(i);
      l_pRow->Cell(1)->GetData(l_iTempValue);
      l_iID = l_iTempValue;
      l_pRow->Cell(2)->GetData(l_iTempValue);
      l_iDesignerID = l_iTempValue;
      l_pRow->Cell(3)->GetData(l_iTempValue);
      l_iUnitType = l_iTempValue;
      l_pRow->Cell(4)->GetData(l_iTempValue);
		l_iNameID = l_iTempValue;      		
   	l_sName = GetString(l_iNameID);

      l_pRow->Cell(5)->GetData(l_iTempValue);
      l_iSpeedAndManeuverability = (UINT8)l_iTempValue;
      l_pRow->Cell(6)->GetData(l_iTempValue);
      l_iSensors = (UINT8)l_iTempValue;
      l_pRow->Cell(7)->GetData(l_iTempValue);
      l_iGunRange = (UINT8)l_iTempValue;
      l_pRow->Cell(8)->GetData(l_iTempValue);
      l_iGunPrecision = (UINT8)l_iTempValue;
      l_pRow->Cell(9)->GetData(l_iTempValue);
      l_iGunDamage = (UINT8)l_iTempValue;
      l_pRow->Cell(10)->GetData(l_iTempValue);
      l_iMissilePayload = (UINT8)l_iTempValue;
      l_pRow->Cell(11)->GetData(l_iTempValue);
      l_iMissileRange = (UINT8)l_iTempValue;
      l_pRow->Cell(12)->GetData(l_iTempValue);
      l_iMissilePrecision = (UINT8)l_iTempValue;
      l_pRow->Cell(13)->GetData(l_iTempValue);
      l_iMissileDamage = (UINT8)l_iTempValue;
      l_pRow->Cell(14)->GetData(l_iTempValue);
      l_iStealth = (UINT8)l_iTempValue;
      l_pRow->Cell(15)->GetData(l_iTempValue);
      l_iCounterMesures = (UINT8)l_iTempValue;
      l_pRow->Cell(16)->GetData(l_iTempValue);
      l_iArmor = (UINT8)l_iTempValue;
      l_pRow->Cell(17)->GetData(l_iTempValue16);
      l_iPiece[0] = l_iTempValue16;
      l_pRow->Cell(18)->GetData(l_iTempValue16);
      l_iPiece[1] = l_iTempValue16;
      l_pRow->Cell(19)->GetData(l_iTempValue16);
      l_iPiece[2] = l_iTempValue16;
      l_pRow->Cell(20)->GetData(l_iTempValue16);
      l_iTexture  = l_iTempValue16;

      //Set the unit type, we assume the unit types are already loaded and ordered
      GUnitType* l_pType = m_vUnitTypes[l_iUnitType-1];

      g_ServerDCL.CreateUnitDesign(l_iID,
                                   l_iDesignerID,
                                   l_pType,
                                   l_sName,
											  l_iNameID,
                                   l_iSpeedAndManeuverability,
                                   l_iSensors,
                                   l_iGunRange,
                                   l_iGunPrecision,
                                   l_iGunDamage,
                                   l_iMissilePayload,
                                   l_iMissileRange,
                                   l_iMissilePrecision,
                                   l_iMissileDamage,
                                   l_iStealth,
                                   l_iCounterMesures,
                                   l_iArmor,
                                   l_iPiece,
                                   l_iTexture);
   }

   return true;
}

/*!
 * Load the units of the game.
 **/
bool GDataAccessLayerServer::LoadUnits()
{
	//Load and Create the unit groups
   {
      GTable l_Results;
      GSString l_sSelectQuery("select * from unit_groups");
      DB::EError e_Error = g_Joshua.QueryDB(l_sSelectQuery,l_Results);
      if(e_Error != DB_NO_ERROR)
      {
         g_Joshua.Log(L"Query error when loading the unit designs");
         return false;
      }

      for(UINT32 i = 0 ; i < l_Results.RowCount()  ;i++)
      {
         INT32 l_iGroupID;
			INT32 l_iCountryID;
         INT32 l_iGroupStatus;
         REAL64 l_fLongitude;
         REAL64 l_fLatitude;

         l_Results.Row(i)->Cell(1)->GetData(l_iGroupID);
			l_Results.Row(i)->Cell(2)->GetData(l_iCountryID);
         l_Results.Row(i)->Cell(3)->GetData(l_fLongitude);
         l_Results.Row(i)->Cell(4)->GetData(l_fLatitude);
         l_Results.Row(i)->Cell(5)->GetData(l_iGroupStatus);

         SDK::Combat::GUnitGroup* l_pUnitGroup = g_Joshua.UnitManager().CreateUnitGroup();

         l_pUnitGroup->Id(l_iGroupID);
			l_pUnitGroup->OwnerId(l_iCountryID);

         g_Joshua.UnitManager().AddUnitGroup(l_pUnitGroup);
         g_Joshua.UnitManager().SetGroupPosition(l_pUnitGroup, GVector2D<REAL32>( (REAL32) l_fLongitude, (REAL32) l_fLatitude) );

         SP2::GUnitGroup* l_pSP2Group = (SP2::GUnitGroup*) l_pUnitGroup;
         l_pSP2Group->ChangeStatus( (EMilitaryStatus::Enum) l_iGroupStatus);
         g_ServerDCL.UnitMover().AddGroup(l_pUnitGroup);
      }
   }

   //Load and create the units
   {
      GTable l_Results;
      GSString l_sSelectQuery("select * from units");
      DB::EError e_Error = g_Joshua.QueryDB(l_sSelectQuery,l_Results);
      if(e_Error != DB_NO_ERROR)
      {
         g_Joshua.Log(L"Query error when loading the unit designs");
         return false;
      }

      for(UINT32 i = 0 ; i < l_Results.RowCount()  ;i++)
      {
         INT32 l_iTemp;
         SP2::GUnit* l_pUnit = (SP2::GUnit*)g_Joshua.UnitManager().CreateUnit();
         //Id
         l_Results.Row(i)->Cell(1)->GetData(l_iTemp);
         l_pUnit->Id(l_iTemp);
         //Owner Id
         l_Results.Row(i)->Cell(2)->GetData(l_iTemp);
         //Design Id
         l_Results.Row(i)->Cell(3)->GetData(l_iTemp);
         SDK::Combat::Design::GUnit* l_pDesign = g_Joshua.UnitManager().UnitDesign(l_iTemp);
         gassert(l_pDesign,"Cannot create a unit with an unknown design");
         l_pUnit->Design(l_pDesign);
         //Quantity -- same as HP
         l_Results.Row(i)->Cell(4)->GetData(l_iTemp);
         //!TODO: Fix this, this is a hack, should never happen that qty == 0 in database
         if(l_iTemp == 0)
         {
            l_iTemp = 1;
#ifdef GOLEM_DEBUG
            g_Joshua.Log(GString(L"Unit has HP of 0 : ") + GString(l_pUnit->Id() ) );
#endif
         }
         l_pUnit->HP(l_iTemp*((SP2::GUnitDesign*)l_pDesign)->Type()->HP());
         //Unit Training
			REAL32 l_fTraining = 0.f;
         l_Results.Row(i)->Cell(5)->GetData(l_fTraining);
         l_pUnit->Training((SP2::ETrainingLevel::Enum)(UINT32)l_fTraining);

         if(l_pUnit->Training() > ETrainingLevel::Elite)
         {
             GDZLOG(EDZLogLevel::Warning, L"Unit " + GString(l_pUnit->Id()) + L" has invalid training level " + GString(l_pUnit->Training()) + L"; setting to Elite");
             l_pUnit->Training(ETrainingLevel::Elite);
         }

         g_Joshua.UnitManager().AddNewUnit(l_pUnit);


      }
   }

   //Load the unit groups and add the units to the groups
   {
      GTable l_Results;
      GSString l_sSelectQuery("select * from group_unit");
      DB::EError e_Error = g_Joshua.QueryDB(l_sSelectQuery,l_Results);
      if(e_Error != DB_NO_ERROR)
      {
         g_Joshua.Log(L"Query error when loading the unit designs");
         return false;
      }

      for(UINT32 i = 0 ; i < l_Results.RowCount()  ;i++)
      {
         INT32 l_iGroupID;
         INT32 l_iUnitID;

         l_Results.Row(i)->Cell(1)->GetData(l_iGroupID);
         l_Results.Row(i)->Cell(2)->GetData(l_iUnitID);
         //Get the group ptr
         SDK::Combat::GUnitGroup* l_pGroup = g_Joshua.UnitManager().UnitGroup(l_iGroupID);
         SDK::Combat::GUnit*      l_pUnit  = g_Joshua.UnitManager().Unit(l_iUnitID);
         gassert(l_pGroup && l_pUnit,"ca chie");

         // TODO: REMOVE -> Log all units that have duplicate designs into the same group
#ifdef LOG_DUPLICATES
         for(UINT32 i = 0;i < l_pGroup->Units().size();i ++)
         {
            if( ( (SP2::GUnit*) l_pGroup->Units() [i] )->CanMergeWith( (SP2::GUnit*) l_pUnit) )
            {
               g_Joshua.Log(GString("Found duplicate in group ") + 
                            GString(l_pGroup->Id() ) + 
                            GString(" of units ") +
                            GString(l_pGroup->Units() [i]->Id() ) + 
                            GString(" and ") +
                            GString(l_pUnit->Id() ) + 
                            GString(" with design ") +
                            GString(l_pUnit->Design()->Id() ) );
            }
         }
#endif

         g_Joshua.UnitManager().AddUnit2Group(l_pUnit,l_pGroup);
      }
   
   }
   return true;
}

/*!
 * Load the Regions in the game
 *  Fetch the regions from the DB and load them into the DAL
 * @return true if loaded successfully, false otherwise
 **/
bool GDataAccessLayerServer::LoadRegions()
{
   GDZLOG(EDZLogLevel::Entry, L"");

	// Fetch data from DB
   GSString l_sQuery = "SELECT * FROM region";
   GTable l_Results;
	DB::EError l_eError = g_Joshua.QueryDB(l_sQuery, l_Results);
	if(l_eError != DB_NO_ERROR) 
	{
		g_Joshua.Log(L"QUERY ERROR - SELECT query on region table");
		return false;
	}

   // Determine number of regions by taking maximum region ID
   m_iNbRegion = 0;
   for(UINT32 i = 0;i < l_Results.RowCount();i ++)
   {
      UINT32 l_iRegionID;
      l_Results.Row(i)->Cell(1)->GetData(l_iRegionID);
      m_iNbRegion = max( (UINT16) l_iRegionID, m_iNbRegion);
   }

   gassert(m_iNbRegion <= Admin().NbFeatures(), "Inconsistency between map engine and database");

	// Create and fill the array of regions
	SAFE_DELETE_ARRAY(m_Regions)
	m_Regions = new GRegion[m_iNbRegion + 1];	
	m_vRegionGeoGroup.resize(m_iNbRegion + 1);
	m_vRegionContinent.resize(m_iNbRegion + 1);
	UINT32 l_iRegionID;
	GTable l_ReligionResults;
	GTable l_LanguageResults;
	GSString l_sReligionQuery;
	GSString l_sLanguageQuery;
	for(UINT32 i=0; i < l_Results.RowCount(); i++)
	{
		l_iRegionID = (UINT32)(*((INT32*)l_Results.Row(i)->Cell(1)->Data()));
		m_Regions[l_iRegionID].Id(l_iRegionID);
		m_Regions[l_iRegionID].OwnerId(*((INT32*)l_Results.Row(i)->Cell(3)->Data()));
		m_Regions[l_iRegionID].OwnerMilitaryId(*((INT32*)l_Results.Row(i)->Cell(4)->Data()));
		m_Regions[l_iRegionID].NameId(*((INT32*)l_Results.Row(i)->Cell(2)->Data()));
		m_vRegionGeoGroup[l_iRegionID] = (EGeoGroups::Enum)(*((INT32*)l_Results.Row(i)->Cell(5)->Data()));
		m_vRegionContinent[l_iRegionID] = (EContinent::Enum)(*((INT32*)l_Results.Row(i)->Cell(6)->Data()));		
		m_Regions[l_iRegionID].HistoricalClaim((UINT32)(*((INT32*)l_Results.Row(i)->Cell(7)->Data())));
		m_Regions[l_iRegionID].Population15((INT64)(*((INT32*)l_Results.Row(i)->Cell(8)->Data())));
		m_Regions[l_iRegionID].Population1565((INT64)(*((INT32*)l_Results.Row(i)->Cell(9)->Data())));
		m_Regions[l_iRegionID].Population65((INT64)(*((INT32*)l_Results.Row(i)->Cell(10)->Data())));
		m_Regions[l_iRegionID].Infrastructure(*((REAL32*)l_Results.Row(i)->Cell(11)->Data()));
		m_Regions[l_iRegionID].TelecomLevel(*((REAL32*)l_Results.Row(i)->Cell(12)->Data()));
		m_Regions[l_iRegionID].TourismIncome(*((REAL64*)l_Results.Row(i)->Cell(13)->Data()));
		m_Regions[l_iRegionID].AreaWater(*((REAL32*)l_Results.Row(i)->Cell(14)->Data()));
		m_Regions[l_iRegionID].ArableLand(*((REAL32*)l_Results.Row(i)->Cell(15)->Data()));
		m_Regions[l_iRegionID].ForestLand(*((REAL32*)l_Results.Row(i)->Cell(16)->Data()));
		m_Regions[l_iRegionID].ParksLand(*((REAL32*)l_Results.Row(i)->Cell(17)->Data()));
		m_Regions[l_iRegionID].NotUsedLand(*((REAL32*)l_Results.Row(i)->Cell(18)->Data()));
		m_Regions[l_iRegionID].AreaLand(m_Regions[l_iRegionID].ArableLand() + 
												  m_Regions[l_iRegionID].ForestLand() + 
												  m_Regions[l_iRegionID].ParksLand() + 
												  m_Regions[l_iRegionID].NotUsedLand());		
		m_Regions[l_iRegionID].AreaTotal(m_Regions[l_iRegionID].AreaWater() + 
													m_Regions[l_iRegionID].AreaLand() );		
		m_Regions[l_iRegionID].HighestPoint((*((INT32*)l_Results.Row(i)->Cell(19)->Data())));
		m_Regions[l_iRegionID].LowestPoint(*((INT32*)l_Results.Row(i)->Cell(20)->Data()));
		m_Regions[l_iRegionID].StandardElevation(*((INT32*)l_Results.Row(i)->Cell(21)->Data()));
		m_Regions[l_iRegionID].Coastlines(*((INT32*)l_Results.Row(i)->Cell(22)->Data()));

		for(UINT32 j=0; j<EResources::ItemCount; j++)
		{			
			//m_Regions[l_iRegionID].ResourceDemand((SP2::EResources::Enum)j,(REAL64)(*((REAL32*)l_Results.Row(i)->Cell((j*2)+22)->Data())));
			m_Regions[l_iRegionID].ResourceProduction((SP2::EResources::Enum)j,(REAL64)(*((REAL32*)l_Results.Row(i)->Cell(23+j)->Data())));
		}
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::Avalanche,*((REAL32*)l_Results.Row(i)->Cell(50)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::Blizzard,*((REAL32*)l_Results.Row(i)->Cell(51)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::ColdWave,*((REAL32*)l_Results.Row(i)->Cell(52)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::Cyclone,*((REAL32*)l_Results.Row(i)->Cell(53)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::Drought,*((REAL32*)l_Results.Row(i)->Cell(54)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::Earthquake,*((REAL32*)l_Results.Row(i)->Cell(55)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::Flood,*((REAL32*)l_Results.Row(i)->Cell(56)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::HeathWave,*((REAL32*)l_Results.Row(i)->Cell(57)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::Hurricane,*((REAL32*)l_Results.Row(i)->Cell(58)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::InsectInfestation,*((REAL32*)l_Results.Row(i)->Cell(59)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::Landslide,*((REAL32*)l_Results.Row(i)->Cell(60)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::Storm,*((REAL32*)l_Results.Row(i)->Cell(61)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::TidalWave,*((REAL32*)l_Results.Row(i)->Cell(62)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::Tornado,*((REAL32*)l_Results.Row(i)->Cell(63)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::Tsunami,*((REAL32*)l_Results.Row(i)->Cell(64)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::Typhoon,*((REAL32*)l_Results.Row(i)->Cell(65)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::VolcanicEruption,*((REAL32*)l_Results.Row(i)->Cell(66)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::WildFire,*((REAL32*)l_Results.Row(i)->Cell(67)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::StockMarketCrash,*((REAL32*)l_Results.Row(i)->Cell(68)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::EconomicDepression,*((REAL32*)l_Results.Row(i)->Cell(69)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::EconomicBoom,*((REAL32*)l_Results.Row(i)->Cell(70)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::PoorHarvest,*((REAL32*)l_Results.Row(i)->Cell(71)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::BountifulHarvest,*((REAL32*)l_Results.Row(i)->Cell(72)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::LivestockEpidemic,*((REAL32*)l_Results.Row(i)->Cell(73)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::ResourceDiscoveryOre,*((REAL32*)l_Results.Row(i)->Cell(74)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::ResourceDiscoveryEnergy,*((REAL32*)l_Results.Row(i)->Cell(75)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::ResourceDiscoveryWealth,*((REAL32*)l_Results.Row(i)->Cell(76)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::Epidemic,*((REAL32*)l_Results.Row(i)->Cell(77)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::PopulationBoom,*((REAL32*)l_Results.Row(i)->Cell(78)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::Famine,*((REAL32*)l_Results.Row(i)->Cell(79)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::NuclearMeltdown,*((REAL32*)l_Results.Row(i)->Cell(80)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::ChemicalSpill,*((REAL32*)l_Results.Row(i)->Cell(81)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::IndustrialAccident,*((REAL32*)l_Results.Row(i)->Cell(82)->Data()));
		m_Regions[l_iRegionID].RandomEvent(ERandomEvents::AntiGlobalizationProtest,*((REAL32*)l_Results.Row(i)->Cell(83)->Data()));
	}

   // Construct list of regions by country
   m_vRegionControl.clear();
   m_vRegionControl.resize(m_iNbRegion + 1);
   m_vCountryControl.clear();
   m_vCountryControl.resize(NbCountry() + 1);
   SAFE_DELETE_ARRAY(m_pCountryValidityArray);
   m_pCountryValidityArray = new bool[NbCountry() + 1];
   Memory::Set(m_pCountryValidityArray, true, NbCountry() + 1);

   // Copy data
   for(UINT32 i = 0;i <= m_iNbRegion;i ++)
   {
      GRegionControl& l_Control = m_vRegionControl[i];
      l_Control.m_iMilitary = (UINT16) m_Regions[i].OwnerMilitaryId();
      l_Control.m_iPolitical = (UINT16) m_Regions[i].OwnerId();

      m_vCountryControl[l_Control.m_iMilitary].first.insert(i);
      m_vCountryControl[l_Control.m_iPolitical].second.insert(i);
   }

   // Fetch religion data from DB
	l_sReligionQuery = "SELECT region_id, religion_id, population FROM religions";		
	l_eError = g_Joshua.QueryDB(l_sReligionQuery, l_ReligionResults);
	if(l_eError != DB_NO_ERROR) 
	{
		g_Joshua.Log(L"QUERY ERROR - SELECT query on religions table");
		return false;
	}
	for(UINT32 j=0; j < l_ReligionResults.RowCount(); j++)
	{
		INT32 l_iRegionID = *((INT32*)l_ReligionResults.Row(j)->Cell(0)->Data());
		m_Regions[l_iRegionID].InsertReligion( *((INT32*)l_ReligionResults.Row(j)->Cell(1)->Data()),
															*((INT32*)l_ReligionResults.Row(j)->Cell(2)->Data())
															);
	}

    // Check database religion population consistency in database if it's a new game
    if(!g_SP2Server->UpdateStaticOnly())
    {
        for(UINT32 i = 1; i <= m_iNbRegion; i++)
        {
            if(!g_ServerDCL.VerifyRegionPopulationConsistency(true, m_Regions[i]))
            {
                GDZLOG(EDZLogLevel::Error, L"Region " + RegionNameAndIDForLog(i) + L" has inconsistent population");
                GDZLOG(EDZLogLevel::Exit, L"Returning false");
                return false;
            }
        }
    }

	// Fetch language data from DB
	l_sLanguageQuery = "SELECT region_id, language_id, population FROM languages";		
	l_eError = g_Joshua.QueryDB(l_sLanguageQuery, l_LanguageResults);
	if(l_eError != DB_NO_ERROR) 
	{
		g_Joshua.Log(L"QUERY ERROR - SELECT query on languages table");
		return false;
	}
	for(UINT32 j=0; j < l_LanguageResults.RowCount(); j++)
	{
		INT32 l_iRegionID = *((INT32*)l_LanguageResults.Row(j)->Cell(0)->Data());
		m_Regions[l_iRegionID].InsertLanguage( *((INT32*)l_LanguageResults.Row(j)->Cell(1)->Data()),
															*((INT32*)l_LanguageResults.Row(j)->Cell(2)->Data())
															);
	}

    // Check language population consistency in database if it's a new game
    if(!g_SP2Server->UpdateStaticOnly())
    {
        for(UINT32 i = 1; i <= m_iNbRegion; i++)
        {
            if(!g_ServerDCL.VerifyRegionPopulationConsistency(false, m_Regions[i]))
            {
                GDZLOG(EDZLogLevel::Error, L"Region " + RegionNameAndIDForLog(i) + L" has inconsistent population");
                GDZLOG(EDZLogLevel::Exit, L"Returning false");
                return false;
            }
        }
    }

    GDZLOG(EDZLogLevel::Exit, L"Returning true");
	return true;
}

/*!
 * Load the Data for every Country in the game
 *  Fetch the data from the DB and load them into the DAL
 * @return true if loaded successfully, false otherwise
 **/
bool GDataAccessLayerServer::LoadCountryData()
{
	SAFE_DELETE_ARRAY(m_pCountryData)
	INT16 l_iNumberCountry = NbCountry();
	m_pCountryData = new GCountryData[l_iNumberCountry+1];

	Memory::Fill<REAL64>(m_pWorldAvailability,0.0,EResources::ItemCount);
	Memory::Fill<REAL64>(m_pWorldProduction,0.0,EResources::ItemCount);
	Memory::Fill<REAL64>(m_pWorldDemand,0.0,EResources::ItemCount);

	for(UINT32 i=1; (INT16)i <= l_iNumberCountry; i++)
	{
		m_pCountryData[i].FetchCountryData(i);		
		for(UINT32 k=0; k<EResources::ItemCount; k++)
		{
			m_pWorldProduction[k] += m_pCountryData[i].ResourceProduction((EResources::Enum)k);
			m_pWorldDemand[k] += m_pCountryData[i].ResourceDemand((EResources::Enum)k);
			m_pWorldAvailability[k] += m_pCountryData[i].ResourceProduction((EResources::Enum)k)
										  - m_pCountryData[i].ResourceDemand((EResources::Enum)k);
		}

		//Sets the activated array
		m_pCountryValidityArray[i] = m_pCountryData[i].Activated();

		m_pCountryData[i].NationalSecurity( g_ServerDCL.FindNationalSecurity(i) );
		m_pCountryData[i].Pressure(0.f);
	}

   //Now that we have fetched all countries, we can fill up the percentage value for every regions.
	for(UINT32 i=1; i <= m_iNbRegion; i++)
	{
		if(m_Regions[i].OwnerId() < 1)
			continue;
		
		if(m_pCountryData[m_Regions[i].OwnerId()].Population() != 0)						
			m_Regions[i].PercentageValue((REAL32)m_Regions[i].Population() / (REAL32)m_pCountryData[m_Regions[i].OwnerId()].Population());
		else
			m_Regions[i].PercentageValue(0.0f);		
	}

	//We can also calculate the country overall relations for every country
	for(UINT32 i=1; (INT16)i <= l_iNumberCountry; i++)
	{
		m_pCountryData[i].OverallDiplomaticRelations( 
			g_ServerDAL.OverallRelations(i) );		
	}


	UpdateHumanDevelopmentBonus();
	UpdateEconomicPartnershipBonus();
	g_ServerDCL.CalculateMarketAvailability();	
	CalculateExpectedRelations();

	return true;
}

/*!
 * Load the Relation Table of the game
 *  Fetch the relations from the DB and load them into the DAL
 * @return true if loaded successfully, false otherwise
 **/
bool GDataAccessLayerServer::LoadRelationTable()
{
	//Loads the relations from the relations table in the database. It takes a long time.
	//Relations are between -100 and 100.
	//100 means the 2 countries absolutely love each other and want to have babies
	//-100 means they hate each other
	//Relations between country A vs B are the same than B vs A


   //Resize the relation table to make sure it has enough place
	INT16 l_iNumberCountry = NbCountry();
	SAFE_DELETE_ARRAY(m_pRelationTable)
	SAFE_DELETE_ARRAY(m_pRelationTableModifier)
	SAFE_DELETE_ARRAY(m_pRelationTableExpected)
	m_pRelationTable = new REAL32[l_iNumberCountry*l_iNumberCountry];
	m_pRelationTableExpected = new REAL32[l_iNumberCountry*l_iNumberCountry];
	m_pRelationTableModifier = new REAL32[l_iNumberCountry*l_iNumberCountry];


   //Reset the relation history table
   for(UINT32 i = 0 ; i < c_iRelationHistoryNumberOfValuesToKeep ; i++)
   {
      m_pRelationHistory[i].Resize(l_iNumberCountry);
      m_pRelationHistory[i].ClearTo(0.f);
   }

	
	GTable      l_TableRelations;
	DB::EError  e_Error;

	INT32 l_iSourceID;
	REAL32 l_fRelations = 0;
	

	GSString l_sSelectQuery("SELECT * FROM relations");

	e_Error = g_Joshua.QueryDB(l_sSelectQuery, l_TableRelations);

	if ((e_Error != DB_NO_ERROR) || (l_TableRelations.RowCount() == 0))
	{
		GString l_sError = L"Error during SELECT query: ";
		l_sError += l_sSelectQuery;
		g_Joshua.Log(l_sError,MSGTYPE_ERROR);
		return false;
	}	

	for(UINT32 i=0; i<l_TableRelations.RowCount(); i++)
	{
		l_iSourceID = 	*(INT32*)l_TableRelations.Row(i)->Cell(0)->Data();
		for(INT32 j=1; j< l_iSourceID ; j++)
		{
			if (l_iSourceID != j)
			{            
				l_fRelations = *(REAL32*)l_TableRelations.Row(i)->Cell(j)->Data();
            m_pRelationTableModifier[((l_iSourceID-1)*l_iNumberCountry) + (j-1)] = l_fRelations;
				m_pRelationTable[((l_iSourceID-1)*l_iNumberCountry) + (j-1)] = l_fRelations;
			}				
		}		
	}	

	return true;
}

//! Return the relation between the source and the target. It tells how the sources likes/dislikes the target.
REAL32 GDataAccessLayerServer::RelationBetweenCountries(ENTITY_ID in_iCountryIDSource, 
																		  ENTITY_ID in_iCountryIDTarget)
{
	//If one of the two countries isn't active, we return 0
	if(!m_pCountryValidityArray[in_iCountryIDSource] || !m_pCountryValidityArray[in_iCountryIDTarget])
		return 0.f;
	
	if (in_iCountryIDSource == in_iCountryIDTarget)
		return 100.f;

	UINT32 i = max(in_iCountryIDSource,in_iCountryIDTarget);
	UINT32 j = min(in_iCountryIDSource,in_iCountryIDTarget);
	return m_pRelationTable[((i-1)*NbCountry()) + (j-1)];
}

//! Return the relation between the source and the target. It tells how the sources likes/dislikes the target.
REAL32 GDataAccessLayerServer::RelationBetweenCountriesExpected(ENTITY_ID in_iCountryIDSource, 
																					 ENTITY_ID in_iCountryIDTarget)
{
	//If one of the two countries isn't active, we return 0
	if(!m_pCountryValidityArray[in_iCountryIDSource] || !m_pCountryValidityArray[in_iCountryIDTarget])
		return 0.f;
	
	if (in_iCountryIDSource == in_iCountryIDTarget)
		return 100.f;

	UINT32 i = max(in_iCountryIDSource,in_iCountryIDTarget);
	UINT32 j = min(in_iCountryIDSource,in_iCountryIDTarget);
	return m_pRelationTableExpected[((i-1)*NbCountry()) + (j-1)];
}

//! Sets the relation between the source and the target. It tells how the sources likes/dislikes the target.
bool GDataAccessLayerServer::RelationBetweenCountries(ENTITY_ID in_iCountryIDSource, 
																		ENTITY_ID in_iCountryIDTarget,
																		REAL32 in_fRelation)
{

   if(in_iCountryIDSource ==  in_iCountryIDTarget)
      return false;

	//If one of the two countries isn't active, we don't set the relations
	if(!m_pCountryValidityArray[in_iCountryIDSource] || !m_pCountryValidityArray[in_iCountryIDTarget])
		return false;

	//Bound values between 100 and -100
	if (in_fRelation > 100.f)
		in_fRelation = 100.f;
	else if (in_fRelation < -100.f)
		in_fRelation = -100.f;


	UINT32 i = max(in_iCountryIDSource,in_iCountryIDTarget);
	UINT32 j = min(in_iCountryIDSource,in_iCountryIDTarget);
	m_pRelationTable[((i-1)*NbCountry()) + (j-1)] = in_fRelation;
	return true;
}

/*!
 * Get the country Id associated with that region
 **/
UINT32 GDataAccessLayerServer::CountryIDFromRegionID(const UINT32 in_iRegionID)
{
   return m_Regions[in_iRegionID].OwnerId();
}


/*!Return pointer to a region
 * @param	in_iRegionID: A region Id
 * @return	pointer
 **/
GRegion* GDataAccessLayerServer::GetGRegion(UINT32 in_iRegionID) const
{
	return m_Regions + in_iRegionID;
}

bool GDataAccessLayerServer::AddWar(GWar& in_NewWar)
{
	in_NewWar.ID(++m_iNextWarID);
	// Make sure ID will never be invalid
   if(in_NewWar.ID() == 0xFFFFFFFF)
   {
   	in_NewWar.ID(++m_iNextWarID);
   }

	m_CurrentWars[in_NewWar.ID()] = in_NewWar;

	ModifyWar(in_NewWar.ID() );	

	return true;
}

bool GDataAccessLayerServer::AddTreaty(GTreaty& in_NewTreaty)
{
	in_NewTreaty.ID(++m_iNextTreatyID);
   // Make sure ID will never be invalid
   if(in_NewTreaty.ID() == 0xFFFFFFFF)
   {
   	in_NewTreaty.ID(++m_iNextTreatyID);
   }

	m_Treaties[in_NewTreaty.ID() ] = in_NewTreaty;

   ModifyTreaty(in_NewTreaty.ID() );	

	return true;
}

GTreaty* GDataAccessLayerServer::Treaty(UINT32 in_iTreatyID)
{
   return m_Treaties.count(in_iTreatyID) ? &(m_Treaties[in_iTreatyID] ) : NULL;
}

GWar* GDataAccessLayerServer::War(UINT32 in_iWarID)
{
	return m_CurrentWars.count(in_iWarID) ? &(m_CurrentWars[in_iWarID] ) : NULL;
}

void GDataAccessLayerServer::SyncronizeTreaties()
{
	bool l_bUpdateEconomicEmbargo     = false;
	bool l_bUpdateWeaponTradeEmbargo  = false;
   bool l_bUpdateWeaponTrade         = false;
   bool l_bUpdateAlliance            = false;
	bool l_bUpdateHumanDevelopment    = false;
	bool l_bUpdateEconomicPartnership = false;
   bool l_bUpdateWorldPeace          = false;
   
	bool l_bRemovingTreaty = false;   

	for(set<UINT32>::iterator it = m_TreatiesToSynchronize.begin();
		 it != m_TreatiesToSynchronize.end(); it++)
	{
		UINT32 l_iTreatyID = *it;

		if(m_TreatiesToDelete.count(l_iTreatyID))
			l_bRemovingTreaty = true;
		else
			l_bRemovingTreaty = false;

		//Calculate the change of cost for countries inside that treaty
		if(!l_bRemovingTreaty)
			g_ServerDCL.UpdateCostByCountryInsideTreaty(l_iTreatyID);

		// Prepare a game event to inform all clients of the new treaty
		SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GUpdateTreaty);
		SP2::Event::GUpdateTreaty* l_UpdateEvent = (SP2::Event::GUpdateTreaty*) (l_Event.get() );
		l_UpdateEvent->m_ModifiedTreaty = m_Treaties[l_iTreatyID];		
		l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
		l_Event->m_iTarget = SDK::Event::ESpecialTargets::BroadcastActiveHumanPlayers;
		g_Joshua.RaiseEvent(l_Event);

		if(l_bRemovingTreaty)
		{
			// Prepare a game event to inform all clients of the deletion of the treaty
			SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GUpdateTreaty);
			SP2::Event::GUpdateTreaty* l_UpdateEvent = (SP2::Event::GUpdateTreaty*) (l_Event.get() );			
			l_UpdateEvent->m_iRemovedTreaty = l_iTreatyID;			
			l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
			l_Event->m_iTarget = SDK::Event::ESpecialTargets::BroadcastActiveHumanPlayers;
			g_Joshua.RaiseEvent(l_Event);

			//Send mail to human player to explain why the treaty was removed
			SDK::GPlayer* l_pPlayerCreator = g_Joshua.ActivePlayerByModID( m_Treaties[l_iTreatyID].Creator() );		
			if(l_pPlayerCreator && !l_pPlayerCreator->AIControlled())
			{
				GTreaty& l_Treaty = m_Treaties[l_iTreatyID];
				SDK::GGameEventSPtr l_Event        = CREATE_GAME_EVENT(SP2::Event::GEventTreatyRefusalDetails);
				l_Event->m_iSource                 = SDK::Event::ESpecialTargets::Server;
				l_Event->m_iTarget                 = l_pPlayerCreator->Id();
				SP2::Event::GEventTreatyRefusalDetails* l_pTreatyRefusal = (SP2::Event::GEventTreatyRefusalDetails*)l_Event.get();      
				l_pTreatyRefusal->m_TreatyRefusalDetails.TreatyName(l_Treaty.Name());
				
				vector<pair<UINT32,ETreatyRefusal::Enum> > l_CountriesThatRefused;
				hash_set<UINT32>									 l_CountriesThatAccepted;

				//Add the countries that refused or accepted
				const set<UINT32>& l_OriginalsSideA = l_Treaty.MembersOriginalsA();
				for(set<UINT32>::const_iterator it = l_OriginalsSideA.begin();
					 it != l_OriginalsSideA.end(); it++)
				{
					if(l_Treaty.MembersSideA(true).count(*it) > 0)
						l_CountriesThatAccepted.insert(*it);
					else
					{
						//Fetch the reason
						ETreatyRefusal::Enum l_eDecision = ETreatyRefusal::Unknown;
						if(m_DecisionHistory.count(l_iTreatyID) > 0)
						{
							if(m_DecisionHistory[l_iTreatyID].count(*it) > 0)
							{
								l_eDecision = m_DecisionHistory[l_iTreatyID][*it];
							}	
						}

						l_CountriesThatRefused.push_back(make_pair<UINT32,ETreatyRefusal::Enum>(*it,l_eDecision));
					}
				}
				//If the Side B can leave, we also have to fetch the reasons why
				{
					if(l_Treaty.SideBCanLeave())
					{
						const set<UINT32>& l_OriginalsSideB = l_Treaty.MembersOriginalsB();
						for(set<UINT32>::const_iterator it = l_OriginalsSideB.begin();
							it != l_OriginalsSideB.end(); it++)
						{
							if(l_Treaty.MembersSideB(true).count(*it) > 0)
								l_CountriesThatAccepted.insert(*it);
							else
							{
								//Fetch the reason
								ETreatyRefusal::Enum l_eDecision = ETreatyRefusal::Unknown;
								if(m_DecisionHistory.count(l_iTreatyID) > 0)
								{
									if(m_DecisionHistory[l_iTreatyID].count(*it) > 0)
									{
										l_eDecision = m_DecisionHistory[l_iTreatyID][*it];
									}	
								}

								l_CountriesThatRefused.push_back(make_pair<UINT32,ETreatyRefusal::Enum>(*it,l_eDecision));
							}
						}
					}
				}
				l_pTreatyRefusal->m_TreatyRefusalDetails.CountriesThatAccepted(l_CountriesThatAccepted);
				l_pTreatyRefusal->m_TreatyRefusalDetails.CountriesThatRefused(l_CountriesThatRefused);

				if(l_pTreatyRefusal->m_TreatyRefusalDetails.CountriesThatRefused().size() > 0 &&
					l_pTreatyRefusal->m_TreatyRefusalDetails.CountriesThatAccepted().size() > 0)
				{
					if(Treaty(l_iTreatyID) && !Treaty(l_iTreatyID)->Active())
						g_Joshua.RaiseEvent(l_Event); 
				}
				m_DecisionHistory.erase(l_iTreatyID);
			}
		}

		UpdateDiplomaticStatuses();

		m_Treaties[l_iTreatyID].m_bEligibleDirty = false;
		ETreatyType::Enum l_TreatyType = m_Treaties[l_iTreatyID].Type();

		switch(l_TreatyType)
		{
		case ETreatyType::EconomicEmbargo:
			l_bUpdateEconomicEmbargo = true;  break;
		case ETreatyType::WeaponTradeEmbargo:
			l_bUpdateWeaponTradeEmbargo = true;  break;
		case ETreatyType::HumanDevelopmentCollaboration:
			l_bUpdateHumanDevelopment = true;  break;
		case ETreatyType::EconomicPartnership:
			l_bUpdateEconomicPartnership = true;  break;
      case ETreatyType::Alliance:
         l_bUpdateAlliance = true; 
         // world peace alliance ? (alliance treaty with every countries in it)
         if(m_Treaties[l_iTreatyID].MembersSideA(false).size() == NbCountry())
         {
            m_vPossibleWorldPeaceTreatiesID.push_back(l_iTreatyID);
            l_bUpdateWorldPeace = true;
         }
         break;
      case ETreatyType::WeaponTrade: 
         l_bUpdateWeaponTrade = true; break;
		}
		if(l_bRemovingTreaty)
			m_Treaties.erase(l_iTreatyID);
	}

	if(l_bUpdateEconomicEmbargo)
		UpdateTradeEmbargos();
	if(l_bUpdateWeaponTradeEmbargo)
		UpdateWeaponTradeEmbargos();
	if(l_bUpdateHumanDevelopment)
		UpdateHumanDevelopmentBonus();
	if(l_bUpdateEconomicPartnership)
		UpdateEconomicPartnershipBonus();	
   if(l_bUpdateWeaponTradeEmbargo || l_bUpdateAlliance || l_bUpdateWeaponTrade)
      UpdateBuildingCountries();
   if(l_bUpdateWorldPeace)
      CheckWorldPeace();

	//Send game events
	for(list<pair<UINT32,UINT32> >::iterator it = m_vCreateNewTreatyEvents.begin();
		 it != m_vCreateNewTreatyEvents.end(); it++)	
	{
		UINT32 l_iCountryID = it->first;
		UINT32 l_iTreatyID = it->second;
		SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GTreatyOffer);
		l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
		l_Event->m_iTarget = g_Joshua.ActivePlayerByModID(l_iCountryID)->Id();
		SP2::Event::GTreatyOffer* l_pTreatyOffer = (SP2::Event::GTreatyOffer*)l_Event.get();
		l_pTreatyOffer->TreatyID = l_iTreatyID;				
		g_Joshua.RaiseEvent(l_Event);
	}
		
	m_TreatiesToSynchronize.clear();
	m_vCreateNewTreatyEvents.clear();
	m_TreatiesToDelete.clear();
}

void GDataAccessLayerServer::ModifyWar(UINT32 in_iWarID)
{   
	m_pUpdateEvent->m_vWarChanges.push_back(m_CurrentWars[in_iWarID]);
	/*
	// Prepare a game event to inform all clients of the modified war
	SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GUpdateWar);
	SP2::Event::GUpdateWar* l_UpdateEvent = (SP2::Event::GUpdateWar*) (l_Event.get() );
	l_UpdateEvent->m_ModifiedWar = m_CurrentWars[in_iWarID];		
	l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
	l_Event->m_iTarget = SDK::Event::ESpecialTargets::BroadcastActiveHumanPlayers;
	g_Joshua.RaiseEvent(l_Event);
	*/
}

void GDataAccessLayerServer::RemoveWar(UINT32 in_iWarID)
{   
	m_pUpdateEvent->m_WarsToRemove.insert(in_iWarID);
	/*
	// Prepare a game event to inform all clients of the deletion of the treaty
	SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GUpdateWar);
	SP2::Event::GUpdateWar* l_UpdateEvent = (SP2::Event::GUpdateWar*) (l_Event.get() );			
	l_UpdateEvent->m_iRemovedWar = in_iWarID;			
	l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
	l_Event->m_iTarget = SDK::Event::ESpecialTargets::BroadcastActiveHumanPlayers;
	g_Joshua.RaiseEvent(l_Event);
	*/
	m_CurrentWars.erase(in_iWarID);
	
}

void GDataAccessLayerServer::ModifyTreaty(UINT32 in_iTreatyID)
{   
	m_TreatiesToSynchronize.insert(in_iTreatyID);
}

void GDataAccessLayerServer::RemoveTreaty(UINT32 in_iTreatyID)
{

	list<pair<UINT32,UINT32> >::iterator it = m_vCreateNewTreatyEvents.begin();
   while(it != m_vCreateNewTreatyEvents.end())
   {
		list<pair<UINT32,UINT32> >::iterator l_ItemToRemove = it++;
      if(l_ItemToRemove->second == in_iTreatyID)
      {
			m_vCreateNewTreatyEvents.erase(l_ItemToRemove);
      }
   }


	m_TreatiesToSynchronize.insert(in_iTreatyID);
	m_TreatiesToDelete.insert(in_iTreatyID);		
}

/*!
 * Put the status of 2 countries to hostile, which means they are at war.
 **/
void GDataAccessLayerServer::AddWarStatus(ENTITY_ID in_iFirstCountryID, ENTITY_ID in_iSecondCountryID)
{
   SWarPair l_War(in_iFirstCountryID,in_iSecondCountryID);
	m_Wars.insert(l_War);
   m_pUpdateEvent->m_vWarList.push_back(l_War);
   UpdateDiplomaticStatuses();
}

/*!
 * Remove war status of 2 countries.
 **/
void GDataAccessLayerServer::RemoveWarStatus(ENTITY_ID in_iFirstCountryID, ENTITY_ID in_iSecondCountryID)
{
   SWarPair l_War(in_iFirstCountryID,in_iSecondCountryID);
	m_Wars.erase(l_War);	
   m_pUpdateEvent->m_vPeaceList.push_back(l_War);
	UpdateDiplomaticStatuses();
}

/*!
 * Load the Treaties of the game
 *  Fetch the treaties from the DB and load them into the DAL
 * @return true if loaded successfully, false otherwise
 **/
bool GDataAccessLayerServer::LoadTreaties()
{

	SAFE_DELETE_ARRAY(m_pHumanDevelopmentBonus)
	SAFE_DELETE_ARRAY(m_pEconomicPartnershipBonus)
	INT16 l_iNumberCountry = NbCountry();
	m_pDiplomaticArray = new UINT8[ (l_iNumberCountry + 1) * (l_iNumberCountry + 1) ];
	m_pTradeEmbargos	 = new bool[ (l_iNumberCountry + 1) * (l_iNumberCountry + 1) ];	
	m_pWeaponTradeEmbargos = new bool[ (l_iNumberCountry + 1) * (l_iNumberCountry + 1) ];	
	m_pEconomicPartnershipBonus = new REAL64[ (l_iNumberCountry + 1) * EResources::ItemCount ];
	m_pHumanDevelopmentBonus = new REAL32[l_iNumberCountry + 1];	

	GSString l_sSelectQuery("SELECT treaty_id, name, type_treaty, private, activated FROM treaty");
	GTable l_TreatyTable;
	DB::EError e_Error = g_Joshua.QueryDB(l_sSelectQuery, l_TreatyTable);
	if(e_Error != DB_NO_ERROR)
	{
		g_Joshua.Log(L"Error during SELECT query on the treaty table. SP2::data_access_layer",MSGTYPE_ERROR);
		return false;
	}  
	UINT32 l_iTreatyID = 0;
	INT32 l_iNameID = 0;
	GString l_sName = L"";
	for(UINT32 i=0; i<l_TreatyTable.RowCount(); i++)
	{
		l_iTreatyID = (UINT32)(*(INT32*)l_TreatyTable.Row(i)->Cell(0)->Data());
		m_iNextTreatyID = max(l_iTreatyID,m_iNextTreatyID);
		m_Treaties[l_iTreatyID].ID(l_iTreatyID);
		l_iNameID = *(INT32*)l_TreatyTable.Row(i)->Cell(1)->Data();
		l_sName = g_ServerDAL.GetString(l_iNameID);
		m_Treaties[l_iTreatyID].Name( l_sName );
		m_Treaties[l_iTreatyID].NameID(l_iNameID);
		m_Treaties[l_iTreatyID].Type( (ETreatyType::Enum)(*(INT32*)l_TreatyTable.Row(i)->Cell(2)->Data()));
		if( *(char*)l_TreatyTable.Row(i)->Cell(3)->Data() == 'T' )
			m_Treaties[l_iTreatyID].Private(true);
		else
			m_Treaties[l_iTreatyID].Private(false);
		if( *(char*)l_TreatyTable.Row(i)->Cell(4)->Data() == 'T' )
			m_Treaties[l_iTreatyID].Active(true);
		else
			m_Treaties[l_iTreatyID].Active(false);		
		m_Treaties[l_iTreatyID].Creator(0);

        GDZLOG(EDZLogLevel::Info2, L"Loading treaty ID " + GString(l_iTreatyID) + L": " + l_sName);
	}

	GSString l_sSelectQueryMember("SELECT treaty_id, country_id, side, activated, original, suspended FROM treaty_member");
	GTable l_TreatyMemberTable;
	e_Error = g_Joshua.QueryDB(l_sSelectQueryMember, l_TreatyMemberTable);
	if(e_Error != DB_NO_ERROR)
	{
		g_Joshua.Log(L"Error during SELECT query on the treaty_member table. SP2::data_access_layer",MSGTYPE_ERROR);
		return false;
	} 
	INT32 l_iSide = 0;
	UINT32 l_iCountryID = 0;
	bool l_bActive = false;
	bool l_bOriginal = false;
	bool l_bSuspended = false;
	for(UINT32 i=0; i<l_TreatyMemberTable.RowCount(); i++)
	{
		l_iTreatyID = (UINT32)(*(INT32*)l_TreatyMemberTable.Row(i)->Cell(0)->Data());
		l_iCountryID = (UINT32)(*(INT32*)l_TreatyMemberTable.Row(i)->Cell(1)->Data());
		l_iSide = *(INT32*)l_TreatyMemberTable.Row(i)->Cell(2)->Data();
		if( *(char*)l_TreatyMemberTable.Row(i)->Cell(3)->Data() == 'T' )
			l_bActive = true;
		else
			l_bActive = false;
		if( *(char*)l_TreatyMemberTable.Row(i)->Cell(4)->Data() == 'T' )
			l_bOriginal = true;
		else
			l_bOriginal = false;
		if( *(char*)l_TreatyMemberTable.Row(i)->Cell(5)->Data() == 'T' )
			l_bSuspended = true;
		else
			l_bSuspended = false;
		switch(l_iSide)
		{
			case 1:
			m_Treaties[l_iTreatyID].AddMemberSideA(l_iCountryID,l_bActive,l_bOriginal); break;
			case 2:
			m_Treaties[l_iTreatyID].AddMemberSideB(l_iCountryID,l_bActive,l_bOriginal); break;
			case 3:
			m_Treaties[l_iTreatyID].AddMemberPressure(l_iCountryID); break;
		}		
		if(l_bSuspended)
			m_Treaties[l_iTreatyID].SuspendCountry(l_iCountryID);
	}	

    for(auto it = m_Treaties.cbegin(); it != m_Treaties.cend(); ++it)
    {
        if(it->second.Type() == ETreatyType::MilitaryAccess &&
           it->second.Name().find(GDataControlLayer::c_sClientStateTreatyPrefix) == 0)
            g_ServerDCL.MakeClientState(*it->second.MembersSideA(true).cbegin(),
                                        *it->second.MembersSideB(true).cbegin(),
                                        it->first,
                                        true);
    }

	//Add the treaty conditions
	GSString l_sSelectQueryCondition("SELECT treaty_id, condition_id, condition_status FROM treaty_condition");
	GTable l_TreatyConditionTable;
	e_Error = g_Joshua.QueryDB(l_sSelectQueryCondition, l_TreatyConditionTable);
	if(e_Error != DB_NO_ERROR)
	{
		g_Joshua.Log(L"Error during SELECT query on the treaty_condition table. SP2::data_access_layer",MSGTYPE_ERROR);
		return false;
	} 

	ETreatyConditions::Enum l_eConditions;
	UINT32 l_iConditionStatus = 0;
	for(UINT32 i=0; i<l_TreatyConditionTable.RowCount(); i++)
	{
		l_iTreatyID = (UINT32)(*(INT32*)l_TreatyConditionTable.Row(i)->Cell(0)->Data());
		l_eConditions = (ETreatyConditions::Enum)(*(INT32*)l_TreatyConditionTable.Row(i)->Cell(1)->Data());
		l_iConditionStatus = (UINT32)(*(INT32*)l_TreatyConditionTable.Row(i)->Cell(2)->Data());
		m_Treaties[l_iTreatyID].AddCondition(l_eConditions,l_iConditionStatus);
	}

	UpdateDiplomaticStatuses();
	UpdateTradeEmbargos();		
	UpdateWeaponTradeEmbargos();		

   return true;
}

/*!
 * Loads all military data for the game
 **/
bool GDataAccessLayerServer::LoadMilitaryData()
{
   IF_RETURN(!LoadUnitDesigns(),false);
   IF_RETURN(!LoadUnits(),false);	

   //Log the military data incoherences
#ifdef GOLEM_DEBUG
   g_ServerDCL.LogMilitaryDataIncoherences(true);
#endif
   g_ServerDAL.UpdateCountryUnitValues();	

   return true;
}

/*!
 * Loads all politic data for the game
 **/
bool GDataAccessLayerServer::LoadPoliticData()
{	
	g_Joshua.Log("Loading Treaties...");
   IF_RETURN(!LoadTreaties(),false);
   g_Joshua.Log("Loading Relations...");
   IF_RETURN(!LoadRelationTable(),false);
   g_Joshua.Log("Loading Country data...");
	IF_RETURN(!LoadCountryData(),false);		

	return true;
}



/*!
 * @return a description of that region
 **/
GString GDataAccessLayerServer::RegionDescription(INT32 in_iRegionID)
{
	GString l_sDescription;
	GEntity* l_pEntity = SDK::EHE::GManager::Entity( CountryIDFromRegionID(in_iRegionID) );
	if ( ! l_pEntity ) return l_sDescription;	
	
	l_sDescription += L"(ID: " + GString(in_iRegionID) + L", NAME:  " + GetString(StringIdRegion(in_iRegionID)) + L", COUNTRY: " + l_pEntity->Name() + L")";

	return l_sDescription;
}

/*!
 * Debug function that prints relation table of a country
 **/
void GDataAccessLayerServer::PrintCountryRelations(ENTITY_ID in_iCountryID)
{
	g_Joshua.Log(L"### Relation table of country " + GString(in_iCountryID) + L" ###",MSGTYPE_MSG);

	for(UINT32 i=1; (INT16)i<=NbCountry(); i++)
	{
		if ( i!= in_iCountryID )
		{			
			g_Joshua.Log(L" vs " + GString(i) + L": " + GString(RelationBetweenCountries(in_iCountryID,i)),MSGTYPE_MSG);
		}

		
	}
}

/*!
 * @return a string id for a specific region
 **/
INT32 GDataAccessLayerServer::StringIdRegion(INT32 in_iRegionID)
{
	return m_Regions[in_iRegionID].NameId();
}

bool GDataAccessLayerServer::ChangeMilitaryControl(UINT32 in_iRegionID, UINT32 in_iNewControlID)
{
	GRegion* l_pRegion = GetGRegion(in_iRegionID);
	IF_RETURN(!l_pRegion, false);
	

   gassert(l_pRegion->OwnerMilitaryId() == m_vRegionControl[in_iRegionID].m_iMilitary,
           "Incoherent military control cached data");

	UINT32 l_iOldControl = l_pRegion->OwnerMilitaryId();
	IF_RETURN(l_iOldControl == in_iNewControlID, true);

#ifdef GOLEM_DEBUG
   g_Joshua.Log(GString(in_iNewControlID) + GString(L" has taken military control of region ") + GString(in_iRegionID) + L" (" + g_ServerDAL.GetString(l_pRegion->NameId()) + L")." );	
#endif

	GCountryData* l_pCountryData = CountryData(in_iNewControlID);
	IF_RETURN(!l_pCountryData, false);

	GCountryData* l_pPrevCountryData = CountryData(l_iOldControl);
	IF_RETURN(!l_pPrevCountryData, false);

	// Update cached data
   ModifyMilitaryControl(in_iRegionID, in_iNewControlID);

	l_pRegion->OwnerMilitaryId(in_iNewControlID);	
	g_ServerDAL.AddCountryToSynchronize(in_iNewControlID);
	g_ServerDAL.AddCountryToSynchronize(l_iOldControl);

   // Remember to update data to the active clients
   m_pUpdateEvent->m_vRegionMilitaryChanges.push_back(make_pair(in_iRegionID, in_iNewControlID) );

   return true;
}

bool GDataAccessLayerServer::ChangePoliticalControl(UINT32 in_iRegionID, UINT32 in_iNewControlID)
{
   GRegion* l_pRegion = GetGRegion(in_iRegionID);
	IF_RETURN(!l_pRegion, false);

#ifdef GOLEM_DEBUG
   g_Joshua.Log(GString(in_iNewControlID) + GString(L" has taken political control of region ") + GString(in_iRegionID) + L" (" + g_ServerDAL.GetString(l_pRegion->NameId()) + L")." );
#endif

   gassert(l_pRegion->OwnerId() == m_vRegionControl[in_iRegionID].m_iPolitical,
           "Incoherent political control cached data");

	UINT32 l_iOldControl = l_pRegion->OwnerId();
	if(l_iOldControl == in_iNewControlID)
		return true;

	GCountryData* l_pCountryData = CountryData(in_iNewControlID);
	IF_RETURN(!l_pCountryData, false);

	GCountryData* l_pPrevCountryData = CountryData(l_iOldControl);
	IF_RETURN(!l_pPrevCountryData, false);

	// Update cached data
   ModifyPoliticalControl(in_iRegionID, in_iNewControlID);

	//Change the available man power available. Give the available man from old owner to new owner
	INT64 l_iManPower = (INT64)((REAL32)l_pRegion->Population1565() * SP2::c_fPourcPopulationAbleToEnroll);
	l_pPrevCountryData->MiliManpowerAvailable(l_pPrevCountryData->MiliManpowerAvailable() - l_iManPower);
	l_pPrevCountryData->MiliManpowerAvailable(l_pCountryData->MiliManpowerAvailable() + l_iManPower);

	l_pRegion->OwnerId(in_iNewControlID);
	g_ServerDAL.AddCountryToSynchronize(in_iNewControlID);
	g_ServerDAL.AddCountryToSynchronize(l_iOldControl);

   // Remember to update data to the active clients
   m_pUpdateEvent->m_vRegionPoliticalChanges.push_back(make_pair(in_iRegionID, in_iNewControlID) );

   return true;
}

void GDataAccessLayerServer::DebugPrintCountryInfo(UINT32 in_iCountryID)
{
	GEntity* l_pEntity = SDK::EHE::GManager::Entity( in_iCountryID );
	if ( ! l_pEntity ) return;
	g_Joshua.Log(L"-- " + l_pEntity->Description() + L" --");
	g_Joshua.Log(L"Number of military regions: " + GString(CountryMilitaryControl(in_iCountryID).size()));
	g_Joshua.Log(L"Number of political regions: " + GString(CountryPoliticalControl(in_iCountryID).size()));
	g_Joshua.Log(L"Population: " + GString(m_pCountryData[in_iCountryID].Population()));
	const GLanguageList& l_Languages = m_pCountryData[in_iCountryID].GetLanguages();
	const GReligionList& l_Religions = m_pCountryData[in_iCountryID].GetReligions();
	g_Joshua.Log(L"Religion list: ");
	for(GReligionList::const_iterator l_Itr = l_Religions.begin();
			l_Itr != l_Religions.end();
			l_Itr++)
	{
		g_Joshua.Log(L"religion: " + GetString(StringIdReligion(l_Itr->first)) + L" " + GString(m_pCountryData[in_iCountryID].ReligionGetPourcentage(l_Itr->first)) + L", " + GString(m_pCountryData[in_iCountryID].ReligionGetStatus(l_Itr->first)));
	}
	g_Joshua.Log(L"Language list: ");
	for(GLanguageList::const_iterator l_Itr = l_Languages.begin();
			l_Itr != l_Languages.end();
			l_Itr++)
	{
		g_Joshua.Log(L"language: " + GetString(StringIdLanguage(l_Itr->first)) + L" " + GString(m_pCountryData[in_iCountryID].LanguageGetPourcentage(l_Itr->first)) + L", " + GString(m_pCountryData[in_iCountryID].LanguageGetStatus(l_Itr->first)));
	}	
	g_Joshua.Log(L"");
}

/*!
 * Load the server config file gameplay constants
 **/
bool GDataAccessLayerServer::LoadServerConfigGameplayConstants()
{
   CXML_Util l_XMLUtil;
   
   //Get the gameplay node
   CTreeXMLNode* l_pGameplayNode = NULL;
   {
      l_pGameplayNode = l_XMLUtil.Search_Node(m_pConfigFile->Root(),SERVER_GAMEPLAY_CONSTANTS_NODE,GString(""),NULL,2);
      IF_RETURN_LOG(!l_pGameplayNode,false,L"Couldnt get the gameplay constants node in the server config file");
   }
   //AMDS Interception Success chances
   {
      CTreeXMLNode* l_pAMDSLongRangeNode = l_XMLUtil.Search_Node(l_pGameplayNode,AMDS_LONG_RANGE_SUCCESS_NODE,GString(""),NULL,1);
      IF_RETURN_LOG(!l_pAMDSLongRangeNode,false,L"No AMDS Long range interception success node present");
      m_fAMDSLongRangeInterceptionSuccess = l_pAMDSLongRangeNode->Data().m_value.ToREAL32()/100;

      CTreeXMLNode* l_pAMDSShortRangeNode = l_XMLUtil.Search_Node(l_pGameplayNode,AMDS_SHORT_RANGE_SUCCESS_NODE,GString(""),NULL,1);
      IF_RETURN_LOG(!l_pAMDSShortRangeNode,false,L"No AMDS Short range interception success node present");
      m_fAMDSShortRangeInterceptionSuccess = l_pAMDSShortRangeNode->Data().m_value.ToREAL32()/100;   
   }
   // Nuclear Missile Speed
   {
      CTreeXMLNode* l_pMissileSpeedNode = l_XMLUtil.Search_Node(l_pGameplayNode,NUCLEAR_MISSILE_SPEED_NODE,GString(""),NULL,1);
      IF_RETURN_LOG(!l_pMissileSpeedNode,false,L"No nuclear missile speed node present");
      m_fNuclearMissileSpeed = l_pMissileSpeedNode->Data().m_value.ToREAL32();   
   }
   // Game startup date
   {
      UINT32 l_iYear, l_iMonth, l_iDay, l_iHour;

      CTreeXMLNode* l_pDateNode = l_XMLUtil.Search_Node(l_pGameplayNode,c_sStartingYearCfg,GString(""),NULL,1);
      IF_RETURN_LOG(!l_pDateNode,false,L"No Year node present");
      l_iYear = l_pDateNode->Data().m_value.ToINT32();

      l_pDateNode = l_XMLUtil.Search_Node(l_pGameplayNode,c_sStartingMonthCfg,GString(""),NULL,1);
      IF_RETURN_LOG(!l_pDateNode,false,L"No Month node present");
      l_iMonth = l_pDateNode->Data().m_value.ToINT32();

      l_pDateNode = l_XMLUtil.Search_Node(l_pGameplayNode,c_sStartingDayCfg,GString(""),NULL,1);
      IF_RETURN_LOG(!l_pDateNode,false,L"No Month node present");
      l_iDay = l_pDateNode->Data().m_value.ToINT32();

      l_pDateNode = l_XMLUtil.Search_Node(l_pGameplayNode,c_sStartingHourCfg,GString(""),NULL,1);
      IF_RETURN_LOG(!l_pDateNode,false,L"No Month node present");
      l_iHour = l_pDateNode->Data().m_value.ToINT32();

      m_ConfigZeroDate = GDateTime(l_iYear,
                                   l_iMonth,
                                   l_iDay,
                                   l_iHour,
                                   0,
                                   0,
                                   0);
   }
   return true;
}

/*!
 * Updates the cities information on the server
 **/
bool GDataAccessLayerServer::UpdateCitiesInfo()
{
   m_vCities.clear();

	GTable l_Results;	
	GSString l_sSelectQuery("SELECT cities.id, cities.longitude, cities.latitude, cities.pop, cities.region_id, cities.name_stid FROM cities, region WHERE region.region_id = cities.region_id");

	DB::EError e_Error = g_Joshua.QueryDB(l_sSelectQuery, l_Results);

	if(e_Error != DB_NO_ERROR)
	{
		g_Joshua.Log(L"QUERY ERROR - Invalid query when trying to access all cities.");
		return false;
	}

   m_vCities.reserve(l_Results.RowCount() + 1);
   m_vRegionCities.resize(m_iNbRegion + 1);

	SCityInfo l_Info;
	REAL64 l_fLongitude = 0.f;
	REAL64 l_fLatitude  = 0.f;
   for(UINT32 i = 0 ; i < l_Results.RowCount() ; i++)
   {
      // Fetch city data
      l_Results.Row(i)->Cell(4)->GetData(l_Info.m_iRegionId);
      l_Results.Row(i)->Cell(0)->GetData(l_Info.m_iId);
      l_Results.Row(i)->Cell(1)->GetData(l_fLongitude);
      l_Results.Row(i)->Cell(2)->GetData(l_fLatitude);
		l_Info.m_Position.x = (REAL32) l_fLongitude;
      l_Info.m_Position.y = (REAL32) l_fLatitude;
      l_Results.Row(i)->Cell(3)->GetData(l_Info.m_iPopulation);
		l_Results.Row(i)->Cell(5)->GetData(l_Info.m_iNameStringID);

      // Add city to data structures
      if(m_vCities.size() <= l_Info.m_iId)
      {
         m_vCities.resize(l_Info.m_iId + 1);
      }
      m_vCities[l_Info.m_iId] = l_Info;
      m_vRegionCities[l_Info.m_iRegionId].push_back(l_Info.m_iId);
   }
	return true;
}

bool GDataAccessLayerServer::UpdateCapitalsInfo()
{
   m_vCapitalsCityID.resize(NbCountry() + 1, 0x80000000);

   DB::GTable l_Results;   

   DB::EError l_QueryResult = g_Joshua.QueryDB("SELECT country.id, cities.id  FROM cities, country WHERE country.capital_id = cities.id", l_Results);
   IF_RETURN_LOG(l_QueryResult != DB::DB_NO_ERROR,false,L"Couldnt fetch capitals");

   for(UINT32 i = 0 ; i < l_Results.RowCount(); i++)
   {
      INT32 l_iID;
      INT32 l_iCountryId;
      l_Results.Row(i)->Cell(0)->GetData(l_iCountryId);
      l_Results.Row(i)->Cell(1)->GetData(l_iID);

      m_vCapitalsCityID[l_iCountryId] = l_iID;
   }

   return true; 
}

/*!
 * @param	in_iCasualties: Number of people to remove/kill
 *	@param	in_iCityID: City ID from which to remove people
 *	@param	in_iRegionID: The region of the city
 **/

UINT32 GDataAccessLayerServer::RemovePopulationFromCity(UINT32 in_iCasualties, UINT32 in_iCityID, UINT32 in_iRegionID)
{
	SCityInfo* l_pCity = (SCityInfo*) City(in_iCityID);
	if( (!l_pCity) || (l_pCity->m_iRegionId != in_iRegionID) )
	{
		g_Joshua.Log(L"City not found in the desired region.",MSGTYPE_WARNING);
		return 0;
	}

	UINT32 l_iCityPopulation = l_pCity->m_iPopulation;
	if(in_iCasualties >  l_iCityPopulation)
		in_iCasualties = l_iCityPopulation;
	
	GRegion* l_pRegion = GetGRegion(in_iRegionID);
	if(!l_pRegion) return 0;

	INT64 l_iPop = l_pRegion->Population();

	if(l_iPop == 0)
		return 0;

	if((INT64)in_iCasualties > l_iPop)
		in_iCasualties = (UINT32)l_iPop;
	
	l_iCityPopulation -= in_iCasualties;
	l_pCity->m_iPopulation = l_iCityPopulation;

   m_pUpdateEvent->m_vCityChanges.push_back(*l_pCity);

	g_ServerDCL.RemovePopulationFromRegion(in_iRegionID,in_iCasualties,false);	

	return in_iCasualties;

}

void GDataAccessLayerServer::UpdateCapitalID(UINT32 in_iCountryID, UINT32 in_iCapitalID)
{
   ChangeCapitalID(in_iCountryID, in_iCapitalID);
   m_pUpdateEvent->m_vCapitalChanges.push_back(make_pair(in_iCountryID, in_iCapitalID) );
}

void GDataAccessLayerServer::AddAnnexToRegion(UINT32 in_iCountryID, UINT32 in_iRegionID, REAL64 in_fDaysBeforeCompletion)
{
   // Create a new entry for this region annex
   GRegionAnnex& l_Annex = m_AnnexedRegions[in_iRegionID];
   l_Annex.m_iAnnexedRegion = in_iRegionID;
   l_Annex.m_iAnnexingCountry = in_iCountryID;
   l_Annex.m_iCompletionTime = g_Joshua.GameTime() + in_fDaysBeforeCompletion;

   m_pUpdateEvent->m_vAnnexChanges.push_back(l_Annex);
}

void GDataAccessLayerServer::RemoveAnnexOfRegion(UINT32 in_iRegionID)
{
   // Remove region annex
   m_AnnexedRegions.erase(in_iRegionID);
   m_pUpdateEvent->m_vAnnexRemoved.push_back(in_iRegionID);
}

void GDataAccessLayerServer::DestroyCountryEntity(UINT32 in_iCountryID, UINT32 in_iConquerorID)
{
   gassert(in_iCountryID <= NbCountry(), "Invalid country ID to destroy");
   gassert(m_vCountryControl[in_iCountryID].second.empty(), "Trying to destroy a country with political control");


	//Clear Unit production for that country
	g_ServerDCL.ClearUnitProductionForCountry(in_iCountryID);

   //For every existing arena, check if we are in a fight, 
   //if so retreat the units so they can be destroyed cleanly
   {
      const vector<SDK::Combat::GArena*>& l_vArenas = g_CombatManager.Arenas();
      for(UINT32 i = 0 ; i < l_vArenas.size() ; i++)
      {
         SP2::GArena*     l_pArena      = (SP2::GArena*)l_vArenas[i];
         SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)l_pArena->Info();
         //Check if we are in the fight
         if(l_pArenaInfo->Actor(in_iCountryID))
         {
            // Move the unit just beside

            //retreat the units
            g_ServerDCL.RetreatCountryFromCombat(l_pArena,in_iCountryID);
         }
      }
   }

	//Clear the units in deployment
	g_ServerDCL.UnitMover().ClearDeploymentListForCountry(in_iCountryID);

   //Removing all the units for that country
   {
      const set<UINT32>& l_CountryGroups = g_Joshua.UnitManager().CountryUnitGroups(in_iCountryID);
      while(l_CountryGroups.size())
      {        
         //Remove all the units of the group
         SP2::GUnitGroupEx* l_pGroup = (SP2::GUnitGroupEx*)g_Joshua.UnitManager().UnitGroup(*l_CountryGroups.begin());			
			gassert(l_pGroup,"GDataAccessLayerServer::DestroyCountryEntity(): Trying to remove an invalid unit group");
         while(l_pGroup->Units().size())
         {
				UINT32 l_iUnitID = (*l_pGroup->Units().begin())->Id();
				if(g_ServerDCL.IsUnitForSale(l_iUnitID))
					g_ServerDCL.SellUnit(l_iUnitID);
				if(g_ServerDCL.UnitMover().IsUnitInTraining(l_iUnitID))
					g_ServerDCL.UnitMover().CancelTraining(l_iUnitID);				
            g_Joshua.UnitManager().KillUnit((*l_pGroup->Units().begin())->Id(),false);
         }         
         g_Joshua.UnitManager().RemoveUnitGroup(*l_CountryGroups.begin());
      }
   }//end of removing all the units for that country

   // remove nuclear missiles of our country
   vector<GNuclearMissile> l_vEmptyList; l_vEmptyList.clear();
   m_pCountryData[in_iCountryID].NuclearMissilesSet(ENuclearMissileType::Standard,     l_vEmptyList);
   m_pCountryData[in_iCountryID].NuclearMissilesSet(ENuclearMissileType::OnSubmarines, l_vEmptyList);

   m_pCountryValidityArray[in_iCountryID] = false;
	
	m_pCountryData[in_iCountryID].Activated(false);

	//Remove this country from every existing treaty
	set<UINT32> l_TreatiesToRemove;
	for(hash_map<UINT32,GTreaty>::iterator l_Itr = m_Treaties.begin();
		 l_Itr != m_Treaties.end();
		 l_Itr++)
	{
		l_TreatiesToRemove.insert(l_Itr->first);		
	}	

	for(set<UINT32>::iterator it = l_TreatiesToRemove.begin();
		 it != l_TreatiesToRemove.end(); it++)
	{
		if(!m_Treaties[(*it)].SideBCanLeave() && m_Treaties[(*it)].CountrySide(in_iCountryID) == 2)
		{
			RemoveTreaty(*it);      
			m_DecisionHistory.erase(*it);
		}
		g_ServerDCL.LeaveTreaty(in_iCountryID,*it,true);
	}

	//Remove war status of that country against other countries
	set<UINT32> l_vCountriesAtWar;
	IsAtWarWith(in_iCountryID,l_vCountriesAtWar);
	for(set<UINT32>::const_iterator l_Itr = l_vCountriesAtWar.begin();
		 l_Itr != l_vCountriesAtWar.end();
		 l_Itr++)
	{
		RemoveWarStatus(in_iCountryID,*l_Itr);	
	}

	//Remove the country from current wars
	set<UINT32> l_WarsToLeave;
	set<UINT32> l_WarsToChangeOpinion;
	const hash_map<UINT32,GWar>& l_Wars = g_ServerDAL.CurrentWars();
	for(hash_map<UINT32,GWar>::const_iterator it = l_Wars.begin();
		 it != l_Wars.end(); it++)
	{
		const GWar& l_CurWar = it->second;
		if(l_CurWar.MasterAttacking() == in_iCountryID || 
			l_CurWar.MasterDefending() == in_iCountryID)
		{
			l_WarsToChangeOpinion.insert(it->first);			
		}
		else if(l_CurWar.AttackingSide().count(in_iCountryID) == 1 ||
				  l_CurWar.DefendingSide().count(in_iCountryID) == 1)
		{
			l_WarsToLeave.insert(it->first);			
		}
	}

	for(set<UINT32>::iterator it = l_WarsToChangeOpinion.begin();
		 it != l_WarsToChangeOpinion.end(); it++)
	{
		UINT32 l_iMasterDefending = m_CurrentWars[*it].MasterDefending();
		UINT32 l_iMasterAttacking = m_CurrentWars[*it].MasterAttacking();
		g_ServerDCL.ChangeOpinionOnWar(l_iMasterDefending,*it,true);
		g_ServerDCL.ChangeOpinionOnWar(l_iMasterAttacking,*it,true);
	}	

	for(set<UINT32>::iterator it = l_WarsToLeave.begin();
		 it != l_WarsToLeave.end(); it++)
	{
		g_ServerDCL.LeaveWar(in_iCountryID, *it);
	}
	
	//Calculate the economic, politic, and militar ranks, without this country
	CalculateCountryInfo();

   //Make sure that no covert action cell is found in a deactivated country
   {
      //For every country, check its cells and if the cell is in a country that is 
      //annexed, set its assigned country to the conqueror country id
      if(in_iConquerorID == 0)
		{
			vector<GCovertActionCell>& l_vCells = CountryData(in_iCountryID)->CovertActionCells();
			while(!l_vCells.empty())
			{
				GCovertActionCell& l_Cell = l_vCells.back();
				CountryData(in_iCountryID)->RemoveCovertActionCell(l_Cell);
			}
		}
		else
		{
			UINT32 l_iNbCountry = NbCountry();
			for(UINT32 i = 1 ; i <= l_iNbCountry ; i++)
			{
                GCountryData& l_CountryData = m_pCountryData[i];
				vector<GCovertActionCell>& l_vCells = l_CountryData.CovertActionCells();
				for(UINT32 j = 0 ; j < l_vCells.size() ; j++)
				{
                    const UINT32 l_iCellID = l_vCells[j].ID();
                    const GCountryData& l_ConquerorData = m_pCountryData[in_iConquerorID];

					if(l_vCells[j].AssignedCountry() == in_iCountryID &&
                       l_vCells[j].ActualState() != ECovertActionsCellState::InTransit)
					{	
                        //Cancel missions against conquered country
                        switch(l_vCells[j].ActualState())
                        {
                        case ECovertActionsCellState::PreparingMission:
                        case ECovertActionsCellState::ReadyToExecute:
                            GDZLOG(EDZLogLevel::Info1, l_CountryData.NameAndIDForLog() + L" is cancelling a mission by cell " + CovertCellInfoForLog(i, l_iCellID) + L" against " + GString(in_iCountryID) + L" due to target inactivity");
                            l_vCells[j].CancelAction();
                            break;

                        default:
                            break;
                        }

                        GDZLOG(EDZLogLevel::Info1, l_CountryData.NameAndIDForLog() + L"'s cell " + CovertCellInfoForLog(i, l_iCellID) + L" will be reassigned from " + GString(in_iCountryID) + L" to " + l_ConquerorData.NameAndIDForLog() + L" due to target inactivity");

						l_vCells[j].AssignedCountry(in_iConquerorID);
                        l_CountryData.FlagCovertActionsCellsAsDirty();
					}
                    else if(l_vCells[j].NextAssignedCountry() == in_iCountryID)
                    {
                        GDZLOG(EDZLogLevel::Info1, l_CountryData.NameAndIDForLog() + L"'s cell " + CovertCellInfoForLog(i, l_iCellID) + L" will be redirected from " + GString(in_iCountryID) + L" to " + l_ConquerorData.NameAndIDForLog() + L" due to target inactivity");

                        l_vCells[j].NextAssignedCountry(in_iConquerorID);
                        l_CountryData.FlagCovertActionsCellsAsDirty();
                    }
				}	

                //Recalculate national security if conqueror's cells were in the conquered country
                if(i == in_iConquerorID)
                    l_CountryData.NationalSecurity(g_ServerDCL.FindNationalSecurity(i));
			}
		}
   }

   // Give back military control to their current political control
   {
      const set<UINT32>& l_MilitaryControl = CountryMilitaryControl(in_iCountryID); 
      list<UINT32> l_RegionsToGiveBack(l_MilitaryControl.begin(), l_MilitaryControl.end() );

      for(list<UINT32>::const_iterator it = l_RegionsToGiveBack.begin();
          it != l_RegionsToGiveBack.end();++ it)
      {
         g_ServerDCL.ChangeRegionMilitaryControl(*it, RegionControl(*it).m_iPolitical, false);
      }
   }

   //No longer a client of its master
   {
       gassert(m_pCountryData[in_iCountryID].Clients().size() == 0,
               m_pCountryData[in_iCountryID].NameAndIDForLog() + L" " +
               L"should've had its clients removed when it left its treaties");

       const auto l_Master = m_pCountryData[in_iCountryID].Master();
       if(l_Master.first != 0)
           g_ServerDCL.LeaveTreaty(l_Master.first, l_Master.second, true);
   }

   // Prepare a game event to inform all clients of the country "Disapearance"
   {
      SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GConquerCountry);
      SP2::Event::GConquerCountry* l_ConquerEvent = (SP2::Event::GConquerCountry*) (l_Event.get() );
      l_ConquerEvent->m_iConqeredID = in_iCountryID;
      l_ConquerEvent->m_iConqueringID = in_iConquerorID;
      l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
      l_Event->m_iTarget = SDK::Event::ESpecialTargets::BroadcastActiveHumanPlayers;
      g_Joshua.RaiseEvent(l_Event);	
   }

   //test achievement
   {
      SDK::GPlayer *lPlayer = g_Joshua.ActivePlayerByModID(in_iConquerorID);
      if(lPlayer)
      {
         //send achievement 5 to client :  annex a country
         SDK::GGameEventSPtr l_Event = CREATE_GAME_EVENT(SP2::Event::GObjectiveCompleted);
         l_Event->m_iSource = SDK::Event::ESpecialTargets::Server;
         l_Event->m_iTarget = lPlayer->Id();
         SP2::Event::GObjectiveCompleted* l_pObjectiveCompleted = (SP2::Event::GObjectiveCompleted*)l_Event.get();
         l_pObjectiveCompleted->m_iCompletedObjectivesID = 10000 + 5;
         l_pObjectiveCompleted->m_eStatus = EObjectiveStatus::Reached;
         g_Joshua.RaiseEvent(l_Event);
      }
   }
	
}

bool GDataAccessLayerServer::CountryIsValid(UINT32 in_iCountryID) const
{
	return m_pCountryValidityArray[in_iCountryID];
}

void GDataAccessLayerServer::FillInitialData(SP2::Event::GSendData* in_pSendEvent,
															UINT32 in_iPlayerModId)
{
   in_pSendEvent->m_vRegionControlArray = &m_vRegionControl;
   in_pSendEvent->m_vCities = &m_vCities;
   in_pSendEvent->m_vCapitals = &m_vCapitalsCityID;
   in_pSendEvent->m_Annexes = &m_AnnexedRegions;
   in_pSendEvent->m_pConqueredCountries = m_pCountryValidityArray;
   in_pSendEvent->m_Treaties = &m_Treaties;
   in_pSendEvent->m_Wars = &m_Wars;
	in_pSendEvent->m_CurrentWars = &m_CurrentWars;
   in_pSendEvent->m_vRegionContinent   = m_vRegionContinent;
   in_pSendEvent->m_vRegionGeoGroup    = m_vRegionGeoGroup;
   in_pSendEvent->m_ListOfUnitsInTraining = (list<GUnitInTraining> *) &(g_ServerDCL.UnitMover().ListOfUnitsInTraining() );
   in_pSendEvent->m_GameOptions = g_ServerDAL.GameOptions();
	in_pSendEvent->m_vRelations.clear();
	//Fill with relations of all countries
	in_pSendEvent->m_vRelations.resize(NbCountry() + 1);
	for(UINT32 i=1; i <= NbCountry(); i++)
	{
		in_pSendEvent->m_vRelations[i] = RelationBetweenCountries(in_iPlayerModId,i);
	}

   //For all the bombardment points
   for(hash_set<UINT32>::const_iterator l_It = g_ServerDCL.UnitMover().BombardingGroupList().begin();
                                        l_It != g_ServerDCL.UnitMover().BombardingGroupList().end();
                                        l_It++)
   {
      SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_It);
      gassert(l_pGroup, "Unit group should exist");
      if(!l_pGroup)
      	continue;
      vector<GVector2D<REAL32 > > l_vBombardmentPoints ;
      g_ServerDCL.BombardmentExplosionsPointsInRegion(l_pGroup->RegionToBombard(),l_vBombardmentPoints);
      in_pSendEvent->m_vBombardedRegionsAndPoints.insert(make_pair(l_pGroup->RegionToBombard(),l_vBombardmentPoints));
   }


   in_pSendEvent->m_vRegionNames.clear();   
   for (int i = 0; i <= m_iNbRegion; i++)
      in_pSendEvent->m_vRegionNames.push_back(GetGRegion(i)->NameId());
}

void GDataAccessLayerServer::ResetUpdateData()
{
   m_pUpdateEventSmartPtr = CREATE_GAME_EVENT(SP2::Event::GUpdateData);
   m_pUpdateEventSmartPtr->m_iSource = SDK::Event::ESpecialTargets::Server;
   m_pUpdateEventSmartPtr->m_iTarget = SDK::Event::ESpecialTargets::BroadcastActiveHumanPlayers;
   m_pUpdateEvent = (SP2::Event::GUpdateData*) (m_pUpdateEventSmartPtr.get() );
}

void GDataAccessLayerServer::SendUpdateData()
{
   // Test if update game event must be sent
   if(!m_pUpdateEvent->m_vWarChanges.empty() ||
		!m_pUpdateEvent->m_WarsToRemove.empty() ||
		!m_pUpdateEvent->m_vRegionMilitaryChanges.empty() ||
      !m_pUpdateEvent->m_vRegionPoliticalChanges.empty() ||
      !m_pUpdateEvent->m_vCityChanges.empty() ||
      !m_pUpdateEvent->m_vCapitalChanges.empty() ||
      !m_pUpdateEvent->m_vAnnexChanges.empty() ||
      !m_pUpdateEvent->m_vAnnexRemoved.empty() ||
      !m_pUpdateEvent->m_vWarList.empty() ||
      !m_pUpdateEvent->m_vPeaceList.empty())
   {
      // Raise event
      g_Joshua.RaiseEvent(m_pUpdateEventSmartPtr);

      // Create a new event for next iteration
      ResetUpdateData();
   }
}

void GDataAccessLayerServer::ResetUpdateUnitValue()
{
   m_pUpdateUnitEventSmartPtr = CREATE_GAME_EVENT(SP2::Event::GUpdateUnitValue);
   m_pUpdateUnitEventSmartPtr->m_iSource = SDK::Event::ESpecialTargets::Server;
   m_pUpdateUnitEventSmartPtr->m_iTarget = SDK::Event::ESpecialTargets::BroadcastActiveHumanPlayers;
   m_pUpdateUnitEvent = (SP2::Event::GUpdateUnitValue*) (m_pUpdateUnitEventSmartPtr.get() );
}

void GDataAccessLayerServer::SendUpdateUnitValue()
{
   // Test if update game event must be sent
   if(!m_pUpdateUnitEvent->m_DirtyCountryUnitValue.empty() )
   {
      // Raise event
      g_Joshua.RaiseEvent(m_pUpdateUnitEventSmartPtr);

      // Create a new event for next iteration
      ResetUpdateUnitValue();
   }
}

void GDataAccessLayerServer::InitCountriesExportsImports()
{
	INT16 l_iNumberCountry = NbCountry();
	for(UINT32 i=1; (INT16)i <= l_iNumberCountry; i++)
	{
		m_pCountryData[i].InitExportsImports();
	}
}

//Return market Production for a specified resource
REAL64 GDataAccessLayerServer::MarketProduction(EResources::Enum in_iResource) const
{			
	return m_pWorldProduction[in_iResource];
}

const REAL64* GDataAccessLayerServer::MarketProduction() const
{			
	return m_pWorldProduction;
}

//Sets market Production for a specified resource
void GDataAccessLayerServer::MarketProduction(EResources::Enum in_iResource, REAL64 in_fAmount)
{		
	m_pWorldProduction[in_iResource] = in_fAmount;
}

//Return market Demand for a specified resource
REAL64 GDataAccessLayerServer::MarketDemand(EResources::Enum in_iResource) const
{			
	return m_pWorldDemand[in_iResource];
}

const REAL64* GDataAccessLayerServer::MarketDemand() const
{			
	return m_pWorldDemand;
}

//Sets market Demand for a specified resource
void GDataAccessLayerServer::MarketDemand(EResources::Enum in_iResource, REAL64 in_fAmount)
{		
	m_pWorldDemand[in_iResource] = in_fAmount;
}

bool GDataAccessLayerServer::InsideTheSameCountryGroup(UINT32 in_iCountryA, UINT32 in_iCountryB) const
{
	if(in_iCountryA < 1 || in_iCountryB < 1)
		return false;
	if( m_pCountryData[in_iCountryA].CountryGroup() == m_pCountryData[in_iCountryB].CountryGroup() )
		return true;
	return false;
}

void GDataAccessLayerServer::CalculateWorldProductionDemand()
{
	Memory::Fill<REAL64>(m_pWorldAvailability,0.0,EResources::ItemCount);
	Memory::Fill<REAL64>(m_pWorldProduction,1.0,EResources::ItemCount);
	Memory::Fill<REAL64>(m_pWorldDemand,1.0,EResources::ItemCount);

	for(INT16 i=1; i<= g_ServerDAL.NbCountry(); i++)
	{
		if(!m_pCountryData[i].Activated())
			continue;
		for(UINT32 k=0; k<EResources::ItemCount; k++)
		{
			m_pWorldProduction[k] += m_pCountryData[i].ResourceProduction((EResources::Enum)k);
			m_pWorldDemand[k] += m_pCountryData[i].ResourceDemand((EResources::Enum)k);			
		}		
	}
	for(UINT32 k=0; k<EResources::ItemCount; k++)
	{
		m_pWorldAvailability[k] = m_pWorldProduction[k] - m_pWorldDemand[k];
	}			
}

void GDataAccessLayerServer::UpdateHumanDevelopmentBonus()
{
	Memory::Fill<REAL32>(m_pHumanDevelopmentBonus,0,NbCountry()+1);
	REAL32 l_fHumanDevA = 0.f;
	REAL32 l_fHumanDevB = 0.f;

	for(hash_map<UINT32, GTreaty>::const_iterator it = m_Treaties.begin();
       it != m_Treaties.end();++ it)
   {
      const GTreaty& l_Treaty = it->second;

		if(!l_Treaty.Active()  || l_Treaty.Type() != ETreatyType::HumanDevelopmentCollaboration)
			continue;
		
		//The treaty is a Human Developement Collaboration
		const set<UINT32>& l_SideA = l_Treaty.MembersSideA(true);	//Active members only
		for(set<UINT32>::const_iterator l_ItrA = l_SideA.begin();
			 l_ItrA != l_SideA.end();
			 l_ItrA++)
		{
			l_fHumanDevA = m_pCountryData[*l_ItrA].HumanDevelopment();
			for(set<UINT32>::const_iterator l_ItrB = l_SideA.begin();
				 l_ItrB != l_SideA.end();
				 l_ItrB++)
			{
				if(*l_ItrA == *l_ItrB)
					continue;

				l_fHumanDevB = m_pCountryData[*l_ItrB].HumanDevelopment();
				if(l_fHumanDevA > l_fHumanDevB)
				{
					//A can help B
					m_pHumanDevelopmentBonus[*l_ItrB] += SP2::c_fHumanDevelopmentCollaborationTreatyBonus * (l_fHumanDevA-l_fHumanDevB);
				}
			}
		}
	}

}

void GDataAccessLayerServer::UpdateEconomicPartnershipBonus()
{
	Memory::Fill<REAL64>(m_pEconomicPartnershipBonus,0, (NbCountry() + 1) * EResources::ItemCount);
	REAL64 l_pMaxProduction[EResources::ItemCount];
	Memory::Fill<REAL64>(l_pMaxProduction, 0, EResources::ItemCount);
	REAL64 l_fRatio = 0.f;

	for(hash_map<UINT32, GTreaty>::const_iterator it = m_Treaties.begin();
       it != m_Treaties.end();++ it)
   {
      const GTreaty& l_Treaty = it->second;

		if(!l_Treaty.Active()  || l_Treaty.Type() != ETreatyType::EconomicPartnership)
			continue;
		
		//The treaty is a Economic Partnership
		const set<UINT32>& l_SideA = l_Treaty.MembersSideA(true);	//Active members only
		
		//Cycle once to find the best country, for every resources		
		for(set<UINT32>::const_iterator l_ItrA = l_SideA.begin();
			 l_ItrA != l_SideA.end();
			 l_ItrA++)
		{
			GCountryData* l_pCountryData = &m_pCountryData[*l_ItrA];
			for(UINT32 i=0; i<EResources::ItemCount; i++)
			{
				l_pMaxProduction[i] = max(l_pMaxProduction[i],l_pCountryData->ResourceProduction((EResources::Enum)i));
			}
		}

		//Cycle twice to adjust the bonuses
		for(set<UINT32>::const_iterator l_ItrA = l_SideA.begin();
			 l_ItrA != l_SideA.end();
			 l_ItrA++)
		{
			GCountryData* l_pCountryData = &m_pCountryData[*l_ItrA];
			for(UINT32 i=0; i<EResources::ItemCount; i++)
			{
				l_fRatio = min( (l_pMaxProduction[i]+1.f) / (l_pCountryData->ResourceProduction((EResources::Enum)i) + 1.f),
									SP2::c_fMaximumLargerSizeEconomicPartnership);
				if(l_fRatio > 1.f)
					m_pEconomicPartnershipBonus[ (*l_ItrA*EResources::ItemCount) + i ] += SP2::c_fGainInPourcentageForMaximumSizeEP * l_fRatio / SP2::c_fMaximumLargerSizeEconomicPartnership;
			}
		}
	}

}

const REAL64* GDataAccessLayerServer::CountryEconomicPartnership(UINT32 in_iCountryID) const
{   
	return m_pEconomicPartnershipBonus + (in_iCountryID * EResources::ItemCount);
}

REAL32 GDataAccessLayerServer::HumanDevelopmentBonus(UINT32 in_iCountryID)
{
	return m_pHumanDevelopmentBonus[in_iCountryID];
}

/*!
 * Sets the player objective list
 **/
void GDataAccessLayerServer::PlayersObjectives(SDK::GPlayer* in_pPlayer,
                                               const vector<GGameObjective>& in_vObjectives)
{
   gassert(in_pPlayer,"GDataAccessLayerServer::PlayersObjectives Null player pointer, incoherence");


   vector<GGameObjective> l_vAdaptedObjectives;
   //Adapt the objectives to the player's situation
   for(UINT32 i = 0 ; i < in_vObjectives.size() ; i++)   
   {
      switch(in_vObjectives[i].m_eObjectiveType)
      {
         //----------------------
         //Raise GDPPerCapita
         //----------------------
         case EObjectiveType::RaiseGDPPerCapita:
         {
            GGameObjective l_Objective;
            l_Objective = in_vObjectives[i];
            
            //Initial GDP Per capita
            SP2::GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_pPlayer->ModID());
            gassert(l_pCountryData,"GDataAccessLayerServer::PlayersObjectives Null country data, incoherence");
            l_Objective.m_Buffer << l_pCountryData->GDPPerCapita();
            l_vAdaptedObjectives.push_back(l_Objective);
            break;
         }
         case EObjectiveType::RaiseEconomicHealth:
         {
            GGameObjective l_Objective;
            l_Objective = in_vObjectives[i];
            
            SP2::GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_pPlayer->ModID());
            gassert(l_pCountryData,"GDataAccessLayerServer::PlayersObjectives Null country data, incoherence");
            l_Objective.m_Buffer << l_pCountryData->EconomicHealth();
            l_vAdaptedObjectives.push_back(l_Objective);            
            break;
         }
         case EObjectiveType::RaiseStability:
         {
            GGameObjective l_Objective;
            l_Objective = in_vObjectives[i];            
            SP2::GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_pPlayer->ModID());
            gassert(l_pCountryData,"GDataAccessLayerServer::PlayersObjectives Null country data, incoherence");
            l_Objective.m_Buffer << l_pCountryData->GvtStability();
            l_vAdaptedObjectives.push_back(l_Objective);
            break;
         }
         case EObjectiveType::AnnexCountry:
         {
            GGameObjective l_Objective;
            l_Objective = in_vObjectives[i];

            // Get out country ID
            UINT32 l_iCountryId;
            l_Objective.m_Buffer >> l_iCountryId;
            
            //Send all the region for that country 
            const set<UINT32> l_RegionList = g_ServerDAL.CountryPoliticalControl(l_iCountryId);
            set<UINT32>::const_iterator l_RegionIterator = l_RegionList.begin();
            l_Objective.m_Buffer << l_RegionList.size();
            while(l_RegionIterator != l_RegionList.end())
            {
               l_Objective.m_Buffer << (*l_RegionIterator);
               l_RegionIterator++;
            }

            l_vAdaptedObjectives.push_back(l_Objective);
            break;
         }
         case EObjectiveType::InvadeCountry:
         {
            GGameObjective l_Objective;
            l_Objective = in_vObjectives[i];

            // Get out country ID
            UINT32 l_iCountryId;
            l_Objective.m_Buffer >> l_iCountryId;
            
            //Send all the region for that country 
            const set<UINT32> l_RegionList = g_ServerDAL.CountryPoliticalControl(l_iCountryId);
            set<UINT32>::const_iterator l_RegionIterator = l_RegionList.begin();
            l_Objective.m_Buffer << l_RegionList.size();
            while(l_RegionIterator != l_RegionList.end())
            {
               l_Objective.m_Buffer << (*l_RegionIterator);
               l_RegionIterator++;
            }

            l_vAdaptedObjectives.push_back(l_Objective);
            break;
         }

         //----------------------
         //Default handler
         //----------------------
         default:        
         {
            l_vAdaptedObjectives.push_back(in_vObjectives[i]);
         }
      }
   }
   m_PlayerObjectives[in_pPlayer->Id()] = l_vAdaptedObjectives;
}

void  GDataAccessLayerServer::GameObjectives(const vector<GGameObjective>& in_vObjectives)
{
   m_vGameObjectives = in_vObjectives;
}
const vector<GGameObjective>& GDataAccessLayerServer::GameObjectives() const
{
   return m_vGameObjectives;
}


//! Creates the game objectives for the player
vector<GGameObjective> GDataAccessLayerServer::AlwaysPresentObjectives()
{
   vector<GGameObjective> l_vObjectives;
   //Actually always create the same objectives
   for(UINT32 i = EObjectiveType::FirstNegativeObjective ; i < EObjectiveType::LastNegativeObjective ; i++)
   {
      if(i == EObjectiveType::FirstNegativeObjective)
      {
         continue;
      }

      GGameObjective l_Objective;
      l_Objective.m_eObjectiveType = (EObjectiveType::Enum)i;
      l_vObjectives.push_back(l_Objective);
   }
   return l_vObjectives;
}

void GDataAccessLayerServer::AddHumanPlayer(SDK::GPlayer* in_pPlayer)
{
   // Create game event for the player & set its source/target so we don't have to do it each time
   SDK::GGameEventSPtr l_MarkerEvent = CREATE_GAME_EVENT(SP2::Event::GHistoryMarkers);
   l_MarkerEvent->m_iSource = SDK::Event::ESpecialTargets::Server;
   l_MarkerEvent->m_iTarget = in_pPlayer->Id();

   m_HistoricMarkersEvents[in_pPlayer->Id() ] = l_MarkerEvent;
}

void GDataAccessLayerServer::RemoveHumanPlayer(SDK::GPlayer* in_pPlayer)
{
   m_HistoricMarkersEvents.erase(in_pPlayer->Id() );
}

void GDataAccessLayerServer::AddHistoricalMarker(UINT32 in_iModID, 
                                                 EHistoryMarkerType::Enum in_eType,
                                                 REAL32 in_fValue)
{
   SDK::GPlayer* l_pPlayer = g_Joshua.ActivePlayerByModID(in_iModID);
   gassert(l_pPlayer, "Invalid player for history marker");

   // Make sure country is played by a human
   hash_map<UINT32, SDK::GGameEventSPtr>::iterator l_It = m_HistoricMarkersEvents.find(l_pPlayer->Id() );
   if(l_It != m_HistoricMarkersEvents.end() )
   {
      SP2::Event::GHistoryMarkers* l_pEvent = (SP2::Event::GHistoryMarkers*) (l_It->second.get() );

      // Find if a marker already exists for this action, and merge if needed
      bool l_bInserted = false;
      if(c_HistoryMarkerIsMergeable[in_eType] )
      {
         for(INT32 i = 0;(i < (INT32) l_pEvent->m_vMarkers.size() ) && !l_bInserted;i ++)
         {
            if(l_pEvent->m_vMarkers[i].m_eType == in_eType)
            {
               l_bInserted = true;
               l_pEvent->m_vMarkers[i].m_fValue += in_fValue;
            }
         }
      }

      if(!l_bInserted)
      {
         // No merge occured, 
         // so append the marker to the current list for the player
         Event::GHistoryMarkers::GMarkerInfo l_Info;
         l_Info.m_eType = in_eType;
         l_Info.m_fValue = in_fValue;

         l_pEvent->m_vMarkers.push_back(l_Info);
      }
   }
}

void GDataAccessLayerServer::SendHistory()
{
   // Send all non-empty history events
   hash_map<UINT32, SDK::GGameEventSPtr>::iterator l_It = m_HistoricMarkersEvents.begin();
   while(l_It != m_HistoricMarkersEvents.end() )
   {
      SP2::Event::GHistoryMarkers* l_pEvent = (SP2::Event::GHistoryMarkers*) (l_It->second.get() );

      if(!l_pEvent->m_vMarkers.empty() )
      {
         l_pEvent->m_fTime = g_Joshua.GameTime();
         g_Joshua.RaiseEvent(l_It->second);

         // Recreate a clean game event
         l_It->second = CREATE_GAME_EVENT(SP2::Event::GHistoryMarkers);
         l_It->second->m_iSource = SDK::Event::ESpecialTargets::Server;
         l_It->second->m_iTarget = l_It->first;
      }

      ++ l_It;
   }
}

REAL32 GDataAccessLayerServer::PopulationLevel(UINT32 in_iCountryID) const
{
    REAL32 l_fTempPopulation = 4000000.f;
	REAL32 l_fPopulation= static_cast<REAL32>(m_pCountryData[in_iCountryID].Population());
	
	if(l_fPopulation < l_fTempPopulation)
    {
        return 1.f + l_fPopulation/l_fTempPopulation;
    }
    else
    {
        return (logf(l_fPopulation/l_fTempPopulation)/logf(2.f)) + 2.f;
    }
}

list<UINT16>& GDataAccessLayerServer::BuildingCountryList(void)
{
   return m_BuildingCountryList;
}

REAL32 GDataAccessLayerServer::TotalMissileValue(UINT32 in_iCountryID) const
{
   gassert(in_iCountryID <= NbCountry(),"Country ID is invalid");
   return m_vCountryUnitValues[in_iCountryID].m_fTotalMissileValue;
}

REAL32 GDataAccessLayerServer::OverallRelations(UINT32 in_iCountryID) const
{
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	REAL32 l_fNbCountries = 0.f;
	REAL32 l_fTotalRelations = 0.f;
	for(UINT32 i=1; i<=l_iNbCountry; i++)
	{
		if(!(m_pCountryValidityArray[i] && i != in_iCountryID))
			continue;
		l_fNbCountries += 1.f;
		l_fTotalRelations += g_ServerDAL.RelationBetweenCountries(in_iCountryID,i);
	}
	if(l_fNbCountries > 0.f)
		l_fTotalRelations /= l_fNbCountries;
	else
		l_fTotalRelations = 100.f;
	
	return l_fTotalRelations;
}

UINT8 GDataAccessLayerServer::CountryResearchStatus(UINT32 in_iCountryID, 
																	 EUnitCategory::Enum in_UnitCategory, 
																	 EUnitDesignCharacteristics::Enum in_ResearchCategory) const
{
	GResearchInformationItf* l_pResearchInfo = g_ServerDAL.CountryData(in_iCountryID)->ResearchInfo();
	gassert(in_UnitCategory < EUnitCategory::ItemCount && in_UnitCategory >= 0 && in_ResearchCategory >= 0 && in_ResearchCategory <= EUnitDesignCharacteristics::ItemCount,"Invalid data to index arrays"); 
   return (UINT8)l_pResearchInfo->m_fMaxValues[in_UnitCategory][in_ResearchCategory];
}


void GDataAccessLayerServer::UpdateAccuracyOfInformation(UINT32 in_iCountryID) 
{
	const REAL32 c_fImportanceTelecom	= 0.2f;
	const REAL32 c_fImportanceCells		= 0.8f;
	const REAL32 c_fCellsTotalNeeded		= 20.f;

	GCountryData* l_pCountryData = g_ServerDAL.CountryData(in_iCountryID);
	hash_map<UINT32,REAL32> l_Accuracy;

	const UINT8* l_pStatuses = g_ServerDAL.CountryDiplomaticStatuses(in_iCountryID);

	REAL32 l_fTelecomLevel = l_pCountryData->TelecomLevel();
	REAL32 l_fTotalCells = 0.f;
	const vector<GCovertActionCell>& l_vCells = l_pCountryData->CovertActionCells();
	for(UINT32 i=0; i<l_vCells.size(); i++)
	{
		UINT32 l_iAssignedCountry = l_vCells[i].AssignedCountry();
		REAL32 l_fStrength = 0.f;
		switch(l_vCells[i].ExperienceLevelType())
		{
		case ECovertActionCellTraining::Recruit:
			l_fStrength = c_fCovertActionsCellsRecruitSuccessRate;			
			break;
		case ECovertActionCellTraining::Regular:
			l_fStrength = c_fCovertActionsCellsRegularSuccessRate;
			break;
		case ECovertActionCellTraining::Veteran:
			l_fStrength = c_fCovertActionsCellsVeteranSuccessRate;
			break;
		case ECovertActionCellTraining::Elite:
			l_fStrength = c_fCovertActionsCellsEliteSuccessRate;
			break;
		}
		l_fTotalCells += l_fStrength;
		if(l_vCells[i].ActualState() == ECovertActionsCellState::Active && 
			l_iAssignedCountry != in_iCountryID && 
			l_iAssignedCountry > 0)
		{
			if(l_Accuracy.count(l_iAssignedCountry) > 0)
				l_Accuracy[l_iAssignedCountry] += l_fStrength * SP2::c_fAccuracyOfInformationBonusByActiveCell;
			else
			l_Accuracy[l_iAssignedCountry] = l_fStrength * SP2::c_fAccuracyOfInformationBonusByActiveCell;
		}		
	}

	if(l_fTotalCells >= c_fCellsTotalNeeded)
		l_fTotalCells = 1.f;
	else
		l_fTotalCells = l_fTotalCells / c_fCellsTotalNeeded;

	REAL32 l_fBaseResults = (c_fImportanceTelecom * l_fTelecomLevel) + (c_fImportanceCells * l_fTotalCells);

	//Iterate for all countries, adding base results + bonus for active cells inside country
	set<UINT32> l_Allies;
	g_ServerDAL.CountryAllies(in_iCountryID,l_Allies);
	UINT32 l_iNbCountry = (UINT32)g_ServerDAL.NbCountry();
	for(UINT32 i=1; i<= l_iNbCountry; i++)
	{		
		REAL32 l_fBonus = 0.f;
		if(l_Accuracy.count(i) > 0)
			l_fBonus = l_Accuracy[i];
		REAL32 l_fAccuracyOfInformation = min(1.f,l_fBaseResults + l_fBonus);
		if(i == in_iCountryID || 
		  ((EDiplomaticStatus::Enum)l_pStatuses[i]) == EDiplomaticStatus::Allied)
			l_fAccuracyOfInformation = 1.f;

		//Iterate through allies
		for(set<UINT32>::iterator it = l_Allies.begin();
			 it != l_Allies.end(); it++)
		{
			REAL32 l_fAllyAccuracy = g_ServerDAL.CountryData(*it)->AccuracyInformation(i);
			if(l_fAllyAccuracy > l_fAccuracyOfInformation)
				l_fAccuracyOfInformation = l_fAllyAccuracy;
			else if (l_fAccuracyOfInformation > l_fAllyAccuracy)
				g_ServerDAL.CountryData(*it)->AccuracyInformation(i,l_fAccuracyOfInformation);
		}
		l_pCountryData->AccuracyInformation(i,l_fAccuracyOfInformation);
	}
}

/*!
 * Updates the relation history
 **/
void GDataAccessLayerServer::UpdateRelationsHistory()
{
   //Check if we must perform the history update
   bool l_bMustUpdate = false;
   if(m_fNextRelationHistoryUpdateGameTime <= g_Joshua.GameTime() || m_fNextRelationHistoryUpdateGameTime <= 0.f)
   {
      //set the time for the next update
      l_bMustUpdate = true;
      m_fNextRelationHistoryUpdateGameTime += c_fRelationHistoryUpdatePeriod;
   }

   if(l_bMustUpdate)
   {
      //Move all the remaining relation history one toward the oldest, 
      //insert the new one at the beginning of the history
      for(UINT32 i = c_iRelationHistoryNumberOfValuesToKeep-1 ; i > 0  ; i--)
      {
         m_pRelationHistory[i] = m_pRelationHistory[i-1];
      }

      //Copy the actual relation table into the 1st relation history log
      for(UINT32 i = 1 ; i < g_ServerDAL.NbCountry() ; i++)
      {
         for(UINT32 j = 1 ; j < i ; j++)
         {
            m_pRelationHistory[0].Set(i-1,j-1,RelationBetweenCountries(i,j));
         }
      }

      //Relations history was updated, notify everybody that wishes to be notified      
      for(hash_map<UINT32, pair<UINT32,UINT32> >::iterator l_It = m_RelationHistoryPlayerWishUpdates.begin() ;
         l_It != m_RelationHistoryPlayerWishUpdates.end();
         l_It++)
      {
         g_ServerDCL.SendRelationHistory(l_It->first,l_It->second.first,l_It->second.second);         
      }
   }
   return;
}

/*!
 * Fetch the relation history
 **/
GSymmetricMatrix<REAL32>* GDataAccessLayerServer::RelationHistory()
{
   return m_pRelationHistory;
}

void GDataAccessLayerServer::RegisterForRelationsHistoryFetch(UINT32 in_iPlayerID,UINT32 in_iCountry1,UINT32 in_iCountry2)
{
   m_RelationHistoryPlayerWishUpdates[in_iPlayerID] = make_pair(in_iCountry1,in_iCountry2);
}
void GDataAccessLayerServer::UnregisterForRelationsHistoryFetch(UINT32 in_iPlayerID)
{
   hash_map<UINT32, pair<UINT32,UINT32> >::iterator l_It = m_RelationHistoryPlayerWishUpdates.find(in_iPlayerID);
   if(l_It != m_RelationHistoryPlayerWishUpdates.end())
   {
      m_RelationHistoryPlayerWishUpdates.erase(l_It);
   }
}

REAL64 GDataAccessLayerServer::ResourceProductionModifier(UINT32 in_iCountryID, EGovernmentType::Enum in_Type) const
{
	if(in_Type == EGovernmentType::Anarchy)
		return g_ServerDAL.CountryData(in_iCountryID)->GvtApproval() / 2.f;
	else
		return SP2::c_pGvtTypeProductionModifier[in_Type];
}

void GDataAccessLayerServer::AddFutureGameEventCreateNewTreaty(UINT32 in_iCountryID, UINT32 in_iTreatyID)
{
	m_vCreateNewTreatyEvents.push_back(make_pair<UINT32,UINT32>(in_iCountryID,in_iTreatyID));
}

void GDataAccessLayerServer::AddCountryToSynchronize(UINT32 in_iCountryID)
{
	m_CountriesToSynchronize.insert(in_iCountryID);	
}

void GDataAccessLayerServer::IterateRegionSynchronization()
{
	for(set<UINT32>::iterator it = m_CountriesToSynchronize.begin();
		 it != m_CountriesToSynchronize.end(); it++)
	{
		if(m_pCountryValidityArray[*it])
			m_pCountryData[*it].SynchronizeWithRegions();
	}
	m_CountriesToSynchronize.clear();	
}

void GDataAccessLayerServer::DirtyCountryUnitsServer(UINT32 in_iCountryID)
{
   DirtyCountryUnits(in_iCountryID);
   m_pUpdateUnitEvent->m_DirtyCountryUnitValue.insert(in_iCountryID);
}

GString GDataAccessLayerServer::DesignName(UINT32 in_iCountryID, EUnitType::Enum in_eUnitType)
{	
   vector<GDesignFormat>& l_vFormats = g_ServerDAL.CountryData(in_iCountryID)->DesignFormat(in_eUnitType);
   GString l_sNewName = L"";
   if(l_vFormats.size() == 0)
      return L"Military Vehicle " + GString(in_eUnitType);

   //Find the format we want
   Random::GQuick l_Rand;
	l_Rand.Seed( (UINT32) (g_Joshua.GameTime() * (REAL64)in_iCountryID * (REAL64)in_eUnitType) );
   l_Rand.RandomReal();
   UINT32 l_iIndex = (UINT32)(l_Rand.RandomReal() * (REAL32) l_vFormats.size());
   GDesignFormat* l_pChosenFormat = &l_vFormats[l_iIndex];
   

   //Now that we have the format, fill the new string
   GString l_sFormatString = l_pChosenFormat->m_sFormat;
   //Find the x inside the [x]
   UINT32 l_iFirstPosition = l_sFormatString.find(L"[");
   UINT32 l_iLastPosition = l_sFormatString.find(L"]");
   if((l_iFirstPosition != GString::npos) && (l_iLastPosition != GString::npos))
   {
      // [ ] characters found
      GString l_sNumber = l_sFormatString.substr(l_iFirstPosition + 1,max(0,l_iLastPosition-l_iFirstPosition)-1);
      INT32 l_iNumber = l_sNumber.ToINT32();
      l_sNewName = l_sFormatString.substr(0,l_iFirstPosition) + GString(l_pChosenFormat->m_iNextID) + l_sFormatString.substr(l_iLastPosition+1,l_sFormatString.size()); 
      l_pChosenFormat->m_iNextID = l_pChosenFormat->m_iNextID + l_iNumber;
   }   

   return l_sNewName;
}

void GDataAccessLayerServer::LastCountryThatNuked(UINT32 in_iCountryID)
{
	m_iFirstCountryThatNuked = in_iCountryID;
}

UINT32 GDataAccessLayerServer::LastCountryThatNuked()
{
	return m_iFirstCountryThatNuked;
}

void GDataAccessLayerServer::DateOfLastNuke(const GDateTime& in_Date)
{
   m_DateOfFirstNuke = in_Date;
}
const GDateTime& GDataAccessLayerServer::DateOfLastNuke()
{
   return m_DateOfFirstNuke;
}

bool GDataAccessLayerServer::OnSave(GIBuffer& io_Buffer)
{
   io_Buffer << ZeroDate().Serialize();

   // Save designs
   {
	   g_Joshua.Log(L"Saving Unit Designs...",MSGTYPE_MSG);	
      const hash_map<UINT32, SDK::Combat::Design::GUnit*>& l_UnitDesigns = g_Joshua.UnitManager().UnitDesigns();

      // Remember unit design count
      io_Buffer << (UINT32) l_UnitDesigns.size();

	   for(hash_map<UINT32, SDK::Combat::Design::GUnit*>::const_iterator it = l_UnitDesigns.begin();
		   it != l_UnitDesigns.end(); it++)
      {
         // Serialize each unit design
         SP2::GUnitDesign* l_pUnitDesign = (SP2::GUnitDesign*) (it->second);
         l_pUnitDesign->Serialize(io_Buffer);
      }
   }

   // Save units
   {
	   g_Joshua.Log(L"Saving Unit Groups...",MSGTYPE_MSG);	

	   const hash_map<UINT32, SDK::Combat::GUnitGroup*>& l_UnitGroups =  g_Joshua.UnitManager().UnitGroups();

      // Remember unit group count
      io_Buffer << (UINT32) l_UnitGroups.size();

	   for(hash_map<UINT32, SDK::Combat::GUnitGroup*>::const_iterator it = l_UnitGroups.begin();
		   it != l_UnitGroups.end(); it++)
	   {
         // Serialize each unit group
         it->second->Serialize(io_Buffer);

         // Save "ex"tra group info
		   SP2::GUnitGroupEx* l_pGroup = (SP2::GUnitGroupEx*) (it->second);
         io_Buffer << l_pGroup->m_iRegionID
                   << l_pGroup->m_ActionAtDest
                   << l_pGroup->m_iMoveRegionID
                   << l_pGroup->m_iPrevMoveRegionID
                   << l_pGroup->m_fLastMoveValue;

         // Serialize each unit of the group
		   const vector<SDK::Combat::GUnit*>& l_vUnits = it->second->Units();
         io_Buffer << l_vUnits.size();

         for(UINT32 i = 0;i < l_vUnits.size();i ++)
		   {
            l_vUnits[i]->Serialize(io_Buffer);

            // Serialize fight info if present
		      SP2::GUnit* l_pUnit = (SP2::GUnit*) (l_vUnits[i] );
            if(l_pUnit->FightInfo() )
            {
               io_Buffer << (UINT8) 1;
               l_pUnit->SerializeFightInfo(io_Buffer);
            }
            else
            {
               io_Buffer << (UINT8) 0;
            }

            // Serialize unit design
            io_Buffer << l_pUnit->Design()->Id();
		   }
	   }
   }

   // Save cities
   {
      io_Buffer << (UINT32) m_vCities.size();
      for(UINT32 i = 0;i < m_vCities.size();i ++)
      {
         // Only population can change in cities
         io_Buffer << m_vCities[i].m_iId << m_vCities[i].m_iPopulation;
      }
   }

   // Save regions
   {
      io_Buffer << m_iNbRegion;
      for(UINT32 i = 0;i <= m_iNbRegion;i ++)
      {
         m_Regions[i].Serialize(io_Buffer);
      }
   }

   // Save country data
   {
	   g_Joshua.Log(L"Saving Country Data...",MSGTYPE_MSG);	

      io_Buffer << (UINT32) NbCountry();
	   for(UINT32 i = 1;i <= (UINT32) NbCountry();i ++)
	   {
		   if(!m_pCountryData[i].OnSave(io_Buffer) )
         {
            return false;
         }
	   }
   }

   // Save capitals
   io_Buffer << m_vCapitalsCityID;

   // Save relation table & history
   {
      io_Buffer.push( (INT8*) m_pRelationTable, sizeof(REAL32) * NbCountry() * NbCountry() );

      io_Buffer << m_fNextRelationHistoryUpdateGameTime;

      for(UINT32 h = 0;h < c_iRelationHistoryNumberOfValuesToKeep;h ++)
      {
         m_pRelationHistory[h].Serialize(io_Buffer);
      }
   }

   // Save treaties
   {
      io_Buffer << m_iNextTreatyID;

      io_Buffer << (UINT32) m_Treaties.size();
      for(hash_map<UINT32, GTreaty>::iterator it = m_Treaties.begin();
          it != Treaties().end();++ it)
      {
         // Must dirty treaty otherwise it wont save completely
         it->second.m_bEligibleDirty = true;
         io_Buffer << it->second.ID();
         it->second.Serialize(io_Buffer);
      }

      io_Buffer << m_vCreateNewTreatyEvents;

      io_Buffer.push( (INT8*) m_pDiplomaticArray, (NbCountry() + 1) * (NbCountry() + 1) );
	   io_Buffer.push( (INT8*) m_pTradeEmbargos, (NbCountry() + 1) * (NbCountry() + 1) * sizeof(bool) );
	   io_Buffer.push( (INT8*) m_pWeaponTradeEmbargos, (NbCountry() + 1) * (NbCountry() + 1) * sizeof(bool) );
	   io_Buffer.push( (INT8*) m_pEconomicPartnershipBonus, (NbCountry() + 1) * EResources::ItemCount * sizeof(REAL64) );
	   io_Buffer.push( (INT8*) m_pHumanDevelopmentBonus, (NbCountry() + 1) * sizeof(REAL32) );	
   }

   // Save war lists
   {
      io_Buffer << m_iNextWarID;
      io_Buffer << m_Wars;

      io_Buffer << (UINT32) m_CurrentWars.size();
      for(hash_map<UINT32, GWar>::const_iterator it = m_CurrentWars.begin();
          it != m_CurrentWars.end();++ it)
      {
         io_Buffer << it->first;
         it->second.Serialize(io_Buffer);
      }
   }

   // Save country validity
   io_Buffer.push( (INT8*) m_pCountryValidityArray, (NbCountry() + 1) * sizeof(bool) );

   // Save annexations
   io_Buffer << m_AnnexedRegions;

   // Save objectives
   {
      io_Buffer << (UINT32) m_PlayerObjectives.size();
      for(hash_map<UINT32,vector<GGameObjective> >::const_iterator it = m_PlayerObjectives.begin();
          it != m_PlayerObjectives.end();++ it)
      {
         io_Buffer << it->first;

         const vector<GGameObjective>& l_ObjectiveList = it->second;
         io_Buffer << l_ObjectiveList.size();

         for(UINT32 j = 0;j < l_ObjectiveList.size();j ++)
         {
            l_ObjectiveList[j].Serialize(io_Buffer);
         }
      }

      io_Buffer << (UINT32) m_vGameObjectives.size();
      for(UINT32 j = 0;j < m_vGameObjectives.size();j ++)
      {
         m_vGameObjectives[j].Serialize(io_Buffer);
      }
   }

   // Save launched nukes
   {
      io_Buffer << m_LaunchedNukes.size();
      for(multimap<REAL64,GLaunchedNuke>::const_iterator it = m_LaunchedNukes.begin();
          it != m_LaunchedNukes.end();++ it)
      {
         io_Buffer << it->first;
         it->second.Serialize(io_Buffer);
      }

		io_Buffer << m_iFirstCountryThatNuked;
      io_Buffer << m_DateOfFirstNuke.Serialize();
   }

   // Save global resource info
   for(UINT32 i = 0;i < EResources::ItemCount;i ++)
   {
      io_Buffer << m_pWorldAvailability[i]
                << m_pWorldProduction[i]
                << m_pWorldDemand[i];
   }

   // Save remaining information
   io_Buffer << m_BuildingCountryList
             << m_fAMDSResearchLastCheckClock;

   //Save our game options
   GameOptions().Serialize(io_Buffer);

	//Save the treaty decision history
	io_Buffer << m_DecisionHistory.size();
	for(hash_map<UINT32, hash_map<UINT32,ETreatyRefusal::Enum> >::iterator it = m_DecisionHistory.begin();
		 it != m_DecisionHistory.end(); it++)
	{
		io_Buffer << it->first;
		io_Buffer << it->second.size();
		for(hash_map<UINT32,ETreatyRefusal::Enum>::iterator it2 = it->second.begin();
			 it2 != it->second.end(); it2++)
		{
			io_Buffer << it2->first << it2->second;
		}
	}	

   return true;
}

bool GDataAccessLayerServer::OnLoad(GOBuffer& io_Buffer)
{
   GDZLOG(EDZLogLevel::Entry, L"");

   INT64 l_iZeroDate;
   io_Buffer >> l_iZeroDate;
   ZeroDate(GDateTime(l_iZeroDate) );

   g_Joshua.UnitManager().NbCountries(NbCountry() );

   // Load designs
   {
	   g_Joshua.Log(L"Loading Unit Designs...",MSGTYPE_MSG);
      UINT32 l_iCount;
      io_Buffer >> l_iCount;

	   for(UINT32 i = 0;i < l_iCount;i ++)
      {
         SP2::GUnitDesign* l_pUnitDesign = (SP2::GUnitDesign*) g_Joshua.UnitManager().CreateUnitDesign();
         l_pUnitDesign->Unserialize(io_Buffer);

         g_Joshua.UnitManager().RegisterUnitDesign(l_pUnitDesign);
      }
   }

   // Load units
   {
	   g_Joshua.Log(L"Loading Unit Groups...",MSGTYPE_MSG);	

      UINT32 l_iCount;
      io_Buffer >> l_iCount;

      for(UINT32 i = 0;i < l_iCount;i ++)
	   {
		   SP2::GUnitGroupEx* l_pGroup = (SP2::GUnitGroupEx*) g_Joshua.UnitManager().CreateUnitGroup();
         l_pGroup->Unserialize(io_Buffer);

         // Load "ex"tra group info
         io_Buffer >> l_pGroup->m_iRegionID
                   >> l_pGroup->m_ActionAtDest
                   >> l_pGroup->m_iMoveRegionID
                   >> l_pGroup->m_iPrevMoveRegionID
                   >> l_pGroup->m_fLastMoveValue;

         // Unserialize each unit of the group
         UINT32 l_iUnitCount;
         io_Buffer >> l_iUnitCount;

         for(UINT32 j = 0;j < l_iUnitCount;j ++)
		   {
            SP2::GUnit* l_pUnit = (SP2::GUnit*) g_Joshua.UnitManager().CreateUnit();
            l_pUnit->Unserialize(io_Buffer);

            // Unserialize fight info if present
            UINT8 l_iFightInfoPresent;
            io_Buffer >> l_iFightInfoPresent;
            if(l_iFightInfoPresent)
            {
               l_pUnit->UnserializeFightInfo(io_Buffer);
            }

            // Unserialize unit design
            UINT32 l_iDesignID;
            io_Buffer >> l_iDesignID;
            l_pUnit->Design(g_Joshua.UnitManager().UnitDesign(l_iDesignID) );

            g_Joshua.UnitManager().AddNewUnit(l_pUnit);

            // Add loaded unit to group
            g_Joshua.UnitManager().AddUnit2Group(l_pUnit, l_pGroup);
		   }

         g_Joshua.UnitManager().AddUnitGroup(l_pGroup);
	   }
   }

   // Load cities
   {
      UINT32 l_iCount;
      io_Buffer >> l_iCount;
      IF_RETURN_LOG(l_iCount != m_vCities.size(), false, "Loaded city count not equal to game city count");

      for(UINT32 i = 0;i < l_iCount;i ++)
      {
         // Only population can change in cities
         io_Buffer >> m_vCities[i].m_iId >> m_vCities[i].m_iPopulation;
      }
   }

   // Load regions
   {
      UINT16 l_iCount;
      io_Buffer >> l_iCount;
      IF_RETURN_LOG(l_iCount != m_iNbRegion, false, "Loaded region count not equal to game region count");

      for(UINT16 i = 0;i <= l_iCount;i ++)
      {
         m_Regions[i].Unserialize(io_Buffer);

         // Check religion population consistency
         if(!g_ServerDCL.VerifyRegionPopulationConsistency(true, m_Regions[i]))
         {
             GDZLOG(EDZLogLevel::Error, L"Region " + RegionNameAndIDForLog(i) + L" has inconsistent population");
             GDZLOG(EDZLogLevel::Exit, L"Returning false");
             return false;
         }

         // Check language population consistency
         if(!g_ServerDCL.VerifyRegionPopulationConsistency(false, m_Regions[i]))
         {
             GDZLOG(EDZLogLevel::Error, L"Region " + RegionNameAndIDForLog(i) + L" has inconsistent population");
             GDZLOG(EDZLogLevel::Exit, L"Returning false");
             return false;
         }
      }
   }

   // Load country data
   {
	   g_Joshua.Log(L"Loading Country Data...",MSGTYPE_MSG);	

      UINT32 l_iCount;
      io_Buffer >> l_iCount;
      IF_RETURN_LOG(l_iCount != NbCountry(), false, "Loaded country count not equal to game country count");

	   for(UINT32 i = 1;i <= l_iCount;i ++)
	   {
		   IF_RETURN(!m_pCountryData[i].OnLoad(io_Buffer), false);

           // Load country names; any custom names will propagate to the clients
           if(m_pCountryData[i].Activated())
           {
               // GServer::Countries() is 0-based
               g_SP2Server->Countries().at(i - 1).Name(m_pCountryData[i].Name());
           }

           GDZLOG(EDZLogLevel::Info2, L"After loading " + m_pCountryData[i].NameAndIDForLog() + L": " +
                  L"Next covert cell ID " + GString(GCovertActionCell::m_iNextId));
	   }

       // Issue #10, covert cell duplicate ID fix
       for(ENTITY_ID i = 1; i <= l_iCount; i++)
       {
           GCountryData& l_Data = m_pCountryData[i];
           if(l_Data.Activated())
           {
               vector<GCovertActionCell>& l_vCells = l_Data.CovertActionCells();
               set<UINT32> l_vCellIds;

               for(auto it = l_vCells.begin(); it < l_vCells.end(); ++it)
               {
                   gassert(it->ID() < GCovertActionCell::m_iNextId,
                           l_Data.NameAndIDForLog() + L": Covert cell ID " + GString(it->ID()) + L" vs. " +
                           L"next ID" + GString(GCovertActionCell::m_iNextId));

                   if(l_vCellIds.count(it->ID()) == 1)
                   {
                       GDZLOG(EDZLogLevel::Warning, l_Data.NameAndIDForLog() + L": " +
                              L"Duplicate covert cell ID " + GString(it->ID()) + L"; " +
                              L"Setting ID " + GString(GCovertActionCell::m_iNextId));
                       it->ID(GCovertActionCell::m_iNextId);
                       GCovertActionCell::m_iNextId++;
                   }

                   l_vCellIds.insert(it->ID());
               }

               gassert(l_vCells.size() == l_vCellIds.size(),
                       l_Data.NameAndIDForLog() + L": " +
                       L"Unique cells " + GString(l_vCells.size()) + L" vs. " +
                       L"Unique cell IDs " + GString(l_vCellIds.size()));

               GDZLOG(EDZLogLevel::Info2, L"After fixing " + l_Data.NameAndIDForLog() + L": " +
                      L"Next covert cell ID " + GString(GCovertActionCell::m_iNextId));
           }
       }

   }

   // Load capitals
   io_Buffer >> m_vCapitalsCityID;

   // Load relation table & history
   {
      io_Buffer.pop( (INT8*) m_pRelationTable, sizeof(REAL32) * NbCountry() * NbCountry() );

      io_Buffer >> m_fNextRelationHistoryUpdateGameTime;

      for(UINT32 h = 0;h < c_iRelationHistoryNumberOfValuesToKeep;h ++)
      {
         m_pRelationHistory[h].Unserialize(io_Buffer);
      }
   }

   // Load treaties
   {
      io_Buffer >> m_iNextTreatyID;

      UINT32 l_iCount;
      io_Buffer >> l_iCount;
      for(UINT32 i = 0;i < l_iCount;i ++)
      {
         UINT32 l_iTreatyID;
         io_Buffer >> l_iTreatyID;
         m_Treaties[l_iTreatyID].Unserialize(io_Buffer);

         GDZLOG(EDZLogLevel::Info2, L"Loading treaty ID " + GString(l_iTreatyID) + L": " +
                m_Treaties[l_iTreatyID].Name());

         if(m_Treaties[l_iTreatyID].Type() == ETreatyType::MilitaryAccess &&
            m_Treaties[l_iTreatyID].Name().find(GDataControlLayer::c_sClientStateTreatyPrefix) == 0)
            g_ServerDCL.MakeClientState(*m_Treaties[l_iTreatyID].MembersSideA(true).cbegin(),
                                        *m_Treaties[l_iTreatyID].MembersSideB(true).cbegin(),
                                        l_iTreatyID,
                                        true);
      }

      io_Buffer >> m_vCreateNewTreatyEvents;

      io_Buffer.pop( (INT8*) m_pDiplomaticArray, (NbCountry() + 1) * (NbCountry() + 1) );
	   io_Buffer.pop( (INT8*) m_pTradeEmbargos, (NbCountry() + 1) * (NbCountry() + 1) * sizeof(bool) );
	   io_Buffer.pop( (INT8*) m_pWeaponTradeEmbargos, (NbCountry() + 1) * (NbCountry() + 1) * sizeof(bool) );
	   io_Buffer.pop( (INT8*) m_pEconomicPartnershipBonus, (NbCountry() + 1) * EResources::ItemCount * sizeof(REAL64) );
	   io_Buffer.pop( (INT8*) m_pHumanDevelopmentBonus, (NbCountry() + 1) * sizeof(REAL32) );	
   }

   // Load war lists
   {
      io_Buffer >> m_iNextWarID;
      io_Buffer >> m_Wars;

      UINT32 l_iCount;
      io_Buffer >> l_iCount;
      for(UINT32 i = 0;i < l_iCount;i ++)
      {
         UINT32 l_iKey;
         io_Buffer >> l_iKey;

         m_CurrentWars[l_iKey].Unserialize(io_Buffer);
      }
   }

   // Load country validity
   io_Buffer.pop( (INT8*) m_pCountryValidityArray, (NbCountry() + 1) * sizeof(bool) );

   // Update region & country control
   m_vRegionControl.clear();
   m_vCountryControl.clear();
   m_vRegionControl.resize(m_iNbRegion + 1);
   m_vCountryControl.resize(NbCountry() + 1);

   for(UINT32 i = 0;i <= m_iNbRegion;i ++)
   {
      GRegionControl& l_Control = m_vRegionControl[i];
      l_Control.m_iMilitary     = (UINT16) m_Regions[i].OwnerMilitaryId();
      l_Control.m_iPolitical    = (UINT16) m_Regions[i].OwnerId();

      m_vCountryControl[l_Control.m_iMilitary].first.insert(i);
      m_vCountryControl[l_Control.m_iPolitical].second.insert(i);
   }


   // Load annexations
   io_Buffer >> m_AnnexedRegions;

   // Load objectives
   {
      UINT32 l_iCount;
      io_Buffer >> l_iCount;

      for(UINT32 i = 0;i < l_iCount;i ++)
      {
         UINT32 l_iPlayerID;
         io_Buffer >> l_iPlayerID;
         vector<GGameObjective>& l_vObjectives = m_PlayerObjectives[l_iPlayerID];

         UINT32 l_iObjectiveCount;
         io_Buffer >> l_iObjectiveCount;
         l_vObjectives.resize(l_iObjectiveCount);

         for(UINT32 j = 0;j < l_iObjectiveCount;j ++)
         {
            l_vObjectives[j].Unserialize(io_Buffer);
         }
      }

      io_Buffer >> l_iCount;
      m_vGameObjectives.resize(l_iCount);

      for(UINT32 j = 0;j < l_iCount;j ++)
      {
         m_vGameObjectives[j].Unserialize(io_Buffer);
      }
   }

   // Load launched nukes
   {
      UINT32 l_iCount;
      io_Buffer >> l_iCount;

      for(UINT32 i = 0;i < l_iCount;i ++)
      {
         REAL64 l_fNukeHitTime;
         io_Buffer >> l_fNukeHitTime;
         GLaunchedNuke l_LaunchedNuke;
         l_LaunchedNuke.Unserialize(io_Buffer);
         m_LaunchedNukes.insert(make_pair(l_fNukeHitTime, l_LaunchedNuke) );
      }

		io_Buffer >> m_iFirstCountryThatNuked;
      INT64 l_iDateVal;
      io_Buffer >> l_iDateVal;
		m_DateOfFirstNuke = GDateTime(l_iDateVal);
   }

   // Load global resource info
   for(UINT32 i = 0;i < EResources::ItemCount;i ++)
   {
      io_Buffer >> m_pWorldAvailability[i]
                >> m_pWorldProduction[i]
                >> m_pWorldDemand[i];
   }

   // Load remaining information
   io_Buffer >> m_BuildingCountryList
             >> m_fAMDSResearchLastCheckClock;

   //Load our game options
   GameOptions().Unserialize(io_Buffer);

	//Save the treaty decision history
	{
		m_DecisionHistory.clear();
		UINT32 l_iDecisionHistorySize = 0;
		io_Buffer >> l_iDecisionHistorySize;
		for(UINT32 i=0; i < l_iDecisionHistorySize; i++)
		{
			UINT32 l_iTreatyID = 0;
			io_Buffer >> l_iTreatyID;
			UINT32 l_iTreatySize = 0;
			io_Buffer >> l_iTreatySize;
			for(UINT32 i=0; i < l_iTreatySize; i++)
			{
				UINT32 l_iCountryID = 0;
				ETreatyRefusal::Enum l_eDecision;
				io_Buffer >> l_iCountryID >> l_eDecision;
				m_DecisionHistory[l_iTreatyID][l_iCountryID] = l_eDecision;				
			}
		}
	}

   // reload total units values
	for(UINT32 i=1; i<=g_ServerDAL.NbCountry(); i++)
		m_vCountryUnitValues[i].m_iDirtyFlag = 1;
	g_ServerDAL.UpdateCountryUnitAndMissilesValues();

	//Calculate the new relations expected
	g_ServerDAL.CalculateExpectedRelations();

   GDZLOG(EDZLogLevel::Exit, L"Returning true");
   return true;
}

void GDataAccessLayerServer::CalculateExpectedRelations()
{				
	REAL32 l_fExpectedRelations = 0.f;	
	UINT32 l_iNbCoutries = (UINT32)NbCountry(); 
	for(UINT32 i = 1; i <= l_iNbCoutries; i++ )
	{
		UINT32 l_iIllegalLanguageSize = m_pCountryData[i].LanguageIllegalStatus().size();
		set<INT32>& l_IllegalLanguages = m_pCountryData[i].LanguageIllegalStatus();
		UINT32 l_iIllegalReligionSize = m_pCountryData[i].ReligionIllegalStatus().size();
		set<INT32>& l_IllegalReligions = m_pCountryData[i].ReligionIllegalStatus();
		for(UINT32 j = 1; j < i; j++ )
		{
			if(j == i)
				continue;			
			l_fExpectedRelations = 0.f;

			UINT32 l_iIllegalLanguageSizeSecond = m_pCountryData[j].LanguageIllegalStatus().size();			
			UINT32 l_iIllegalReligionSizeSecond = m_pCountryData[j].ReligionIllegalStatus().size();			

			//At War?
			if(DiplomaticStatus(i,j) == EDiplomaticStatus::Hostile)
				l_fExpectedRelations = -100.f;
			else
			{
				//History modifier, as written in the database
				l_fExpectedRelations += m_pRelationTableModifier[((i-1)*l_iNbCoutries) + (j-1)];
				//Governmenty type modifier
				l_fExpectedRelations += RelationBetweenGovernmentType((EGovernmentType::Enum) m_pCountryData[i].GvtType(), (EGovernmentType::Enum) m_pCountryData[j].GvtType());
				//Internal Laws modifier
				for(UINT32 l=0; l<EInternalLaws::ItemCount; l++)
				{
					if(m_pCountryData[i].InternalLaw((EInternalLaws::Enum)l) == m_pCountryData[j].InternalLaw((EInternalLaws::Enum)l))
						l_fExpectedRelations += SP2::c_fRelationsDifferenceInLaw;
					else
						l_fExpectedRelations -= SP2::c_fRelationsDifferenceInLaw;
				}
				//Economic Ideology modifier
				l_fExpectedRelations -= abs(m_pCountryData[i].CountryEconomicModel() - 
					m_pCountryData[j].CountryEconomicModel()) * SP2::c_fRelationsDifferenceEconomyIdeology;
				
				//Illegal Religion
				{
					if(l_iIllegalReligionSize)
					{
						const GReligionList& l_Religions = m_pCountryData[j].GetReligions();
						//Check how many compares, lose 10 for every one that are the same
						for(set<INT32>::iterator it = l_IllegalReligions.begin();
							it != l_IllegalReligions.end(); it++)
						{						
							for(GReligionList::const_iterator itr = l_Religions.begin();
								itr != l_Religions.end(); itr++)
							{
								if(*it == itr->first)
									l_fExpectedRelations -= 10.f;
							}							
						}					
					}
					if(l_iIllegalReligionSizeSecond)
					{
						const GReligionList& l_Religions = m_pCountryData[i].GetReligions();
						set<INT32>& l_IllegalReligionsSecond = m_pCountryData[j].ReligionIllegalStatus();
						//Check how many compares, lose 10 for every one that are the same
						for(set<INT32>::iterator it = l_IllegalReligionsSecond.begin();
							it != l_IllegalReligionsSecond.end(); it++)
						{						
							for(GReligionList::const_iterator itr = l_Religions.begin();
								itr != l_Religions.end(); itr++)
							{
								if(*it == itr->first)
									l_fExpectedRelations -= 10.f;
							}							
						}					
					}
				}
				//Illegal Language
				{
					if(l_iIllegalLanguageSize)
					{
						const GLanguageList& l_Languages = m_pCountryData[j].GetLanguages();
						//Check how many compares, lose 10 for every one that are the same
						for(set<INT32>::iterator it = l_IllegalLanguages.begin();
							it != l_IllegalLanguages.end(); it++)
						{						
							for(GLanguageList::const_iterator itr = l_Languages.begin();
								itr != l_Languages.end(); itr++)
							{
								if(*it == itr->first)
									l_fExpectedRelations -= 10.f;
							}							
						}					
					}
					if(l_iIllegalLanguageSizeSecond)
					{
						set<INT32>& l_IllegalLanguagesSecond = m_pCountryData[j].LanguageIllegalStatus();
						const GLanguageList& l_Languages = m_pCountryData[i].GetLanguages();
						//Check how many compares, lose 10 for every one that are the same
						for(set<INT32>::iterator it = l_IllegalLanguagesSecond.begin();
							it != l_IllegalLanguagesSecond.end(); it++)
						{						
							for(GLanguageList::const_iterator itr = l_Languages.begin();
								itr != l_Languages.end(); itr++)
							{
								if(*it == itr->first)
									l_fExpectedRelations -= 10.f;
							}							
						}					
					}
				}
				//Emigration borders different?
				if(m_pCountryData[i].EmigrationClosed() != m_pCountryData[j].EmigrationClosed())
					l_fExpectedRelations -= 5.f;
			}
			
			l_fExpectedRelations = min(100.f,l_fExpectedRelations);
			l_fExpectedRelations = max(-100.f,l_fExpectedRelations);

			m_pRelationTableExpected[((i-1)*l_iNbCoutries) + (j-1)] = l_fExpectedRelations;
		}
	}
	
	//Treaties
	{
		REAL32 l_fGain = 0.f;
		REAL32 l_fLoss = 0.f;
		const hash_map<UINT32,SP2::GTreaty>& l_hTreaties =  g_ServerDAL.Treaties();
		for(hash_map<UINT32,SP2::GTreaty>::const_iterator l_Itr = l_hTreaties.begin();
			l_Itr != l_hTreaties.end();
			l_Itr++)
		{
			const GTreaty& l_Treaty = l_Itr->second;
			if(!l_Treaty.Active())
				continue;
			switch(l_Treaty.Type())
			{
			case ETreatyType::CulturalExchanges:
				{
					l_fGain = SP2::c_fYearlyRelationsGainCulturalExchange;
					//Cycle through the members on side A, and each of them has a bonus of relations
					const set<UINT32>& l_vSideA = l_Treaty.MembersSideA(true);
					for(set<UINT32>::const_iterator l_FirstCountry = l_vSideA.begin();
						l_FirstCountry != l_vSideA.end();
						l_FirstCountry++)
					{
						for(set<UINT32>::const_iterator l_SecondCountry = l_FirstCountry;
							l_SecondCountry != l_vSideA.end();
							l_SecondCountry++)
						{
							if(*l_FirstCountry == *l_SecondCountry)
								continue;
							UINT32 l_iFirst = max(*l_FirstCountry,*l_SecondCountry);
							UINT32 l_iSecond = min(*l_FirstCountry,*l_SecondCountry);
							m_pRelationTableExpected[((l_iFirst-1)*l_iNbCoutries) + (l_iSecond-1)] = 
								min(100.f,m_pRelationTableExpected[((l_iFirst-1)*l_iNbCoutries) + (l_iSecond-1)] + l_fGain);
						}
					}
				}
				break;
			case ETreatyType::NobleCause:			
				{
					l_fGain = SP2::c_fYearlyRelationsGainNobleCause;
					l_fLoss = SP2::c_fYearlyRelationsLossNobleCause;
					const set<UINT32>& l_vSideA = l_Treaty.MembersSideA(true);
					for(set<UINT32>::const_iterator l_FirstCountry = l_vSideA.begin();
						l_FirstCountry != l_vSideA.end();
						l_FirstCountry++)
					{
						for(UINT32 i=1; i<=l_iNbCoutries; i++)
						{
							if(*l_FirstCountry == i)
								continue;
							if(l_vSideA.find(i) == l_vSideA.end())
							{
								UINT32 l_iFirst = max(*l_FirstCountry,i);
								UINT32 l_iSecond = min(*l_FirstCountry,i);
								m_pRelationTableExpected[((l_iFirst-1)*l_iNbCoutries) + (l_iSecond-1)] = 
									max(-100.f,m_pRelationTableExpected[((l_iFirst-1)*l_iNbCoutries) + (l_iSecond-1)] + l_fLoss);
							}
							else
							{
								UINT32 l_iFirst = max(*l_FirstCountry,i);
								UINT32 l_iSecond = min(*l_FirstCountry,i);
								m_pRelationTableExpected[((l_iFirst-1)*l_iNbCoutries) + (l_iSecond-1)] = 
									min(100.f,m_pRelationTableExpected[((l_iFirst-1)*l_iNbCoutries) + (l_iSecond-1)] + l_fGain);
							}
						}
					}
				}
				break;
			}
		}
	}
}

bool GDataAccessLayerServer::OnNew(GDatabase* in_pDatabase)
{
   IF_RETURN(!UpdateCitiesInfo(), false);
	IF_RETURN(!LoadRegions(), false);

	vector<GSString> l_sSelectQuery;
	vector<GTable*> l_StringIds;

   GTable l_Table1;
   GTable l_Table2;
   GTable l_Table3;
   GTable l_Table4;
   GTable l_Table5;
	GTable l_Table6;

   l_sSelectQuery.push_back("SELECT religion_name FROM religion ORDER BY id");
   l_sSelectQuery.push_back("SELECT language_name FROM language ORDER BY id");
   l_sSelectQuery.push_back("SELECT name_stid FROM parties ORDER BY id");
   l_sSelectQuery.push_back("SELECT name_stid FROM gvt_type ORDER BY id");
   l_sSelectQuery.push_back("SELECT name_stid FROM status ORDER BY id");
	l_sSelectQuery.push_back("SELECT name_stid FROM resource_name ORDER BY id");

   l_StringIds.push_back(&l_Table1);
   l_StringIds.push_back(&l_Table2);
   l_StringIds.push_back(&l_Table3);
   l_StringIds.push_back(&l_Table4);
   l_StringIds.push_back(&l_Table5);
	l_StringIds.push_back(&l_Table6);

	DB::EError e_Error = g_Joshua.QueryDB(l_sSelectQuery, l_StringIds);

	if(e_Error != DB_NO_ERROR || 
      l_StringIds[0]->RowCount() == 0 ||
      l_StringIds[1]->RowCount() == 0 ||
      l_StringIds[2]->RowCount() == 0 ||
      l_StringIds[3]->RowCount() == 0 ||
      l_StringIds[4]->RowCount() == 0 ||
      l_StringIds[5]->RowCount() == 0)
	{
		g_Joshua.Log(L"QUERY ERROR - Invalid query when trying to access religion_name from Religion Table.");
		return false;
	}
	
   //RELIGION
	m_vStringTableIdsReligions.resize(l_StringIds[0]->RowCount() + 1);
   m_vStringTableIdsReligions[0] = 0;
	for(UINT32 i=1; i <= l_StringIds[0]->RowCount(); i++)
	{
		m_vStringTableIdsReligions[i] = *((INT32*)l_StringIds[0]->Row((i-1))->Cell(0)->Data());
	}

   //LANGUAGES
	m_vStringTableIdsLanguages.resize(l_StringIds[1]->RowCount() + 1);
   m_vStringTableIdsLanguages[0] = 0;
	for(UINT32 i=1; i <= l_StringIds[1]->RowCount(); i++)
	{
		m_vStringTableIdsLanguages[i] = *((INT32*)l_StringIds[1]->Row((i-1))->Cell(0)->Data());
	}

   //PARTIES
	m_vStringTableIdsPoliticalParties.resize(l_StringIds[2]->RowCount() + 1);
   m_vStringTableIdsPoliticalParties[0] = 0;
	for(UINT32 i=1; i <= l_StringIds[2]->RowCount(); i++)
	{		
		m_vStringTableIdsPoliticalParties[i] = *((INT32*)l_StringIds[2]->Row((i-1))->Cell(0)->Data());
	}

   //GVT TYPE
	m_vStringTableIdsGvtType.resize(l_StringIds[3]->RowCount() + 1);
   m_vStringTableIdsGvtType[0] = 0;
	for(UINT32 i=1; i <= l_StringIds[3]->RowCount(); i++)
	{
		m_vStringTableIdsGvtType[i] = *((INT32*)l_StringIds[3]->Row((i-1))->Cell(0)->Data());
	}

   //STATUS
   m_vStringTableIdsStatus.resize(l_StringIds[4]->RowCount() + 1);
   m_vStringTableIdsStatus[0] = 0;
	for(UINT32 i=1; i <= l_StringIds[4]->RowCount(); i++)
	{
		m_vStringTableIdsStatus[i] = *((INT32*)l_StringIds[4]->Row((i-1))->Cell(0)->Data());
	}

	//RESSOURCES
	m_vStringTableIdsResources.resize(l_StringIds[5]->RowCount() + 1);
   m_vStringTableIdsResources[0] = 0;
	for(UINT32 i=0; i < l_StringIds[5]->RowCount(); i++)
	{
		m_vStringTableIdsResources[i] = *((INT32*)l_StringIds[5]->Row((i))->Cell(0)->Data());
	}

   IF_RETURN(!__super::LoadCountryData(), false);

   g_Joshua.UnitManager().ResetNextIDs();

   m_fNextRelationHistoryUpdateGameTime = 0;
   m_fAMDSResearchLastCheckClock = 0;
	m_iNextTreatyID = 0;
	m_iNextWarID = 0;
   m_iFirstCountryThatNuked = 0;

   // This section will only be called when creating a new game
   if(!g_SP2Server->UpdateStaticOnly() )
   {
      IF_RETURN(!UpdateCapitalsInfo(), false);
      IF_RETURN(!LoadMilitaryData(), false);

      IF_RETURN(!LoadPoliticData(), false);
      CalculateCountryInfo();

      for(UINT32 i = 1;i <= NbCountry();i ++)
      {
         UpdateAccuracyOfInformation(i);
      }

      GameObjectives(AlwaysPresentObjectives() );
   }
   else
   {
      m_pCountryData = new GCountryData[NbCountry() + 1];
      for(UINT32 i = 1;i <= NbCountry();i ++)
      {
         m_pCountryData[i].OnNew(in_pDatabase);
      }

   	m_pRelationTable = new REAL32[NbCountry() * NbCountry()];
		m_pRelationTableExpected = new REAL32[NbCountry() * NbCountry()];
		m_pRelationTableModifier = new REAL32[NbCountry() * NbCountry()];

	   m_pDiplomaticArray = new UINT8[ (NbCountry() + 1) * (NbCountry() + 1) ];
	   m_pTradeEmbargos	 = new bool[ (NbCountry() + 1) * (NbCountry() + 1) ];	
	   m_pWeaponTradeEmbargos = new bool[ (NbCountry() + 1) * (NbCountry() + 1) ];	
	   m_pEconomicPartnershipBonus = new REAL64[ (NbCountry() + 1) * EResources::ItemCount ];
	   m_pHumanDevelopmentBonus = new REAL32[NbCountry() + 1];	
   }

	for(UINT32 i=1; i<=g_ServerDAL.NbCountry(); i++)
		m_vCountryUnitValues[i].m_iDirtyFlag = 1;
	g_ServerDAL.UpdateCountryUnitAndMissilesValues();

   return true;
}

void GDataAccessLayerServer::OnClean()
{
   g_Joshua.UnitManager().Shutdown();

   if(g_SP2Server->CleanVariableOnly() )
   {
      for(UINT32 i = 1;i <= (UINT32) NbCountry();i ++)
      {
         m_pCountryData[i].OnClean();
      }
   }
   else
   {
      SAFE_DELETE_ARRAY(m_pCountryData);

      SAFE_DELETE_ARRAY(m_pRelationTable);
		SAFE_DELETE_ARRAY(m_pRelationTableExpected);
		SAFE_DELETE_ARRAY(m_pRelationTableModifier);
      SAFE_DELETE_ARRAY(m_pHumanDevelopmentBonus);
      SAFE_DELETE_ARRAY(m_pEconomicPartnershipBonus);
      SAFE_DELETE_ARRAY(m_Regions);

      // Common data
      m_vRegionControl.clear();
      m_vCountryControl.clear();
      SAFE_DELETE_ARRAY(m_pCountryValidityArray);
      m_vRegionCities.clear();
      m_vCities.clear();
      m_vCapitalsCityID.clear();
      SAFE_DELETE_ARRAY(m_pDiplomaticArray);
      SAFE_DELETE_ARRAY(m_pTradeEmbargos);
      SAFE_DELETE_ARRAY(m_pWeaponTradeEmbargos);

	   SAFE_DELETE_ARRAY(m_StringTableIdsObjectiveResults);

      m_vRegionContinent.clear();
      m_vRegionGeoGroup.clear();
      m_vCountryUnitValues.clear();
   }

   m_RelationHistoryPlayerWishUpdates.clear();
   m_Treaties.clear();
   m_Wars.clear();
   m_CurrentWars.clear();
   m_AnnexedRegions.clear();
   m_PlayerObjectives.clear();
   m_vGameObjectives.clear();
   m_LaunchedNukes.clear();
   m_vPossibleWorldPeaceTreatiesID.clear();

   m_TreatiesToSynchronize.clear();
   m_TreatiesToDelete.clear();
   m_CountriesToSynchronize.clear();
   m_vCreateNewTreatyEvents.clear();
   m_BuildingCountryList.clear();

   ResetUpdateData();
   ResetUpdateUnitValue();

   // Clean all history events
   hash_map<UINT32, SDK::GGameEventSPtr>::iterator l_It = m_HistoricMarkersEvents.begin();
   while(l_It != m_HistoricMarkersEvents.end() )
   {
      // Recreate a clean game event
      l_It->second = CREATE_GAME_EVENT(SP2::Event::GHistoryMarkers);
      l_It->second->m_iSource = SDK::Event::ESpecialTargets::Server;
      l_It->second->m_iTarget = l_It->first;

      ++ l_It;
   }
}

bool GDataAccessLayerServer::Initialize(const GString& in_sConfigFile)
{
   DataManager()->RegisterNode(this);

   return __super::Initialize(in_sConfigFile);
}



void GDataAccessLayerServer::UpdateCountryUnitAndMissilesValues(void)
{

   for(UINT32 i = 1;i <= NbCountry();i ++)
   {		
      if(m_vCountryUnitValues[i].m_iDirtyFlag)
      {
			m_vCountryUnitValues[i].m_fTotalMissileValue = g_ServerDAL.CountryData(i)->StrategicForces();
      }
   }

   // 
   UpdateCountryUnitValues();
}

void GDataAccessLayerServer::FillReceiveCountryStaticData(SP2::Event::GReceiveCountryStaticData* in_pData)
{
   in_pData->m_vStringTableIdsReligions = m_vStringTableIdsReligions;
   in_pData->m_vStringTableIdsResources = m_vStringTableIdsResources;
   in_pData->m_vStringTableIdsLanguages = m_vStringTableIdsLanguages;
   in_pData->m_vStringTableIdsPoliticalParties = m_vStringTableIdsPoliticalParties;
   in_pData->m_vStringTableIdsGvtType = m_vStringTableIdsGvtType;
   in_pData->m_vStringTableIdsStatus = m_vStringTableIdsStatus;
}

REAL64 GDataAccessLayerServer::RegionValue(UINT32 in_iRegionID) const
{
	GRegion* l_pRegion = g_ServerDAL.GetGRegion(in_iRegionID);
	if(!l_pRegion)
		return 0.f;

	REAL64 l_fTotal = 0.f;
	for(UINT32 i=0; i < EResources::ItemCount; i++)
		l_fTotal += l_pRegion->ResourceProduction((EResources::Enum)i);

	return l_fTotal * SP2::c_fRegionValueModifier;
}

void GDataAccessLayerServer::UpdateBuildingCountries()
{
   // loop into the active player
   for(SDK::GPlayers::const_iterator l_It = g_Joshua.HumanPlayers().begin();
         l_It != g_Joshua.HumanPlayers().end();
         l_It++)
   {
      // send the appropriate building countries list
      g_ServerDCL.SendBuildingCountriesList(l_It->second->ModID());
   } 
}

void GDataAccessLayerServer::CheckWorldPeace()
{
   // determines if the world is at peace
   bool l_bWorldAtPeace = false;
   // our iterator into our global alliances
   vector<UINT32>::iterator l_It = m_vPossibleWorldPeaceTreatiesID.begin();
   // loop into our treaties id to check
   while(l_It != m_vPossibleWorldPeaceTreatiesID.end())
   {
      // retrieve our associated treaty and of a good type
      GTreaty* l_pTreaty = Treaty(*l_It);
      // treaty still exists ?
      if(l_pTreaty && l_pTreaty->Type() == ETreatyType::Alliance)
      {
         // did we have found one world peace treaty ? (alliance with every countries in it)
         if(l_pTreaty->MembersSideA(true).size() == NbCountry())
         {
            // we got one, get out
            l_bWorldAtPeace = true;
            break;
         }
         ++l_It;
      }
      // we dont need to check that treaty anymore, remove it
      else
      {
         // remove useless treaty
         vector<UINT32>::iterator l_ItToRemove = l_It; ++l_It;
         m_vPossibleWorldPeaceTreatiesID.erase(l_ItToRemove);
      }
   }

   // update our world peace status after our in depth analysis ?
   g_ServerDCL.UpdateWorldPeaceStatus(l_bWorldAtPeace);
}

bool GDataAccessLayerServer::CountryCanAssignCovertCellToTarget(UINT32 in_iSource, UINT32 in_iTarget)
{
    const INT32 l_iMaximumCellsInForeignCountry = g_SP2Server->MaximumCellsInForeignCountry();

    bool l_bCanMove = true;

    if(in_iTarget != in_iSource && l_iMaximumCellsInForeignCountry > 0)
    {
        INT32 l_iNumberCellsAlreadyInIntendedCountry = 0;

        const vector<GCovertActionCell>& l_vExistingCells = m_pCountryData[in_iSource].CovertActionCells();
        for(vector<GCovertActionCell>::const_iterator l_It = l_vExistingCells.cbegin();
            l_It < l_vExistingCells.cend();
            ++l_It)
        {
            if(l_It->AssignedCountry() == in_iTarget ||
               l_It->NextAssignedCountry() == in_iTarget)
                l_iNumberCellsAlreadyInIntendedCountry++;

            if(l_iNumberCellsAlreadyInIntendedCountry >= l_iMaximumCellsInForeignCountry)
            {
                l_bCanMove = false;
                break;
            }
        }

        GDZLOG(EDZLogLevel::Info2, g_ServerDAL.CountryData(in_iSource)->NameAndIDForLog() + L", trying to assign a cell to " +
               g_ServerDAL.CountryData(in_iTarget)->NameAndIDForLog() + "; " +
               L"already has " + GString(l_iNumberCellsAlreadyInIntendedCountry) + L" cells in that country. " +
               L"CanMove " + GString(l_bCanMove));
    }

    return l_bCanMove;
}

bool GDataAccessLayerServer::BattleOccurringInRegion(UINT32 in_iRegionId) const
{
    const vector<SDK::Combat::GArena*> l_vArenas = g_CombatManager.Arenas();
    bool l_bBattleOccurring = false;

    for(auto it = l_vArenas.cbegin(); it < l_vArenas.cend(); ++it)
    {
        const GArenaInfo* const l_pArenaInfo = dynamic_cast<GArenaInfo*>((*it)->Info());
        if(l_pArenaInfo->m_iRegionID == in_iRegionId)
        {
            l_bBattleOccurring = true;
            break;
        }
    }

    return l_bBattleOccurring;
}

GString GDataAccessLayerServer::RegionNameAndIDForLog(UINT32 in_iRegionID)
{
    return GetString(StringIdRegion(in_iRegionID)) +
           L" (" + GString(in_iRegionID) + L")";
}

GString GDataAccessLayerServer::CovertCellInfoForLog(const ENTITY_ID in_iCountryID, const UINT32 in_iCellID) const
{
    GString l_sCellInfo;

    if(m_pCountryValidityArray[in_iCountryID])
    {
        GCountryData& l_CountryData = m_pCountryData[in_iCountryID];
        const GCovertActionCell& l_Cell = l_CountryData.CovertActionCell(in_iCellID);
        l_sCellInfo = l_Cell.Name() + L" (" + GString(l_Cell.ID()) + L", " + l_CountryData.NameAndIDForLog() + L")";
    }

    return l_sCellInfo;
}

GString GDataAccessLayerServer::WarInfoForLog(const UINT32 in_iWarID, const bool in_bLogAllParticipants) const
{
    GString l_sWarInfo;

    do
    {
        if(m_CurrentWars.count(in_iWarID) == 0)
        {
            GDZLOG(EDZLogLevel::Warning, L"War ID " + GString(in_iWarID) + L" not found");
            break;
        }

        const GWar& l_War = m_CurrentWars.at(in_iWarID);

        const ENTITY_ID l_iMasterAttacking = l_War.MasterAttacking();
        const GCountryData& l_MasterAttacking = m_pCountryData[l_iMasterAttacking];
        if(!l_MasterAttacking.Activated())
        {
            GDZLOG(EDZLogLevel::Warning, L"War ID " + GString(in_iWarID) + L" has inactive attacking master " + GString(l_iMasterAttacking));
            break;
        }

        const ENTITY_ID l_iMasterDefending = l_War.MasterDefending();
        const GCountryData& l_MasterDefending = m_pCountryData[l_War.MasterDefending()];
        if(!l_MasterDefending.Activated())
        {
            GDZLOG(EDZLogLevel::Warning, L"War ID " + GString(in_iWarID) + L" has inactive defending master " + GString(l_iMasterDefending));
            break;
        }

        l_sWarInfo += l_MasterAttacking.NameAndIDForLog() + L" (" + GString(l_War.AttackingSide().size()) + L" total" + L")";
        if(in_bLogAllParticipants)
        {
            l_sWarInfo += L": ";
            for(auto it = l_War.AttackingSide().cbegin(); it != l_War.AttackingSide().cend(); ++it)
                l_sWarInfo += m_pCountryData[*it].NameAndIDForLog() + L", ";
        }

        l_sWarInfo += L" vs. ";

        l_sWarInfo += l_MasterDefending.NameAndIDForLog() + L" (" + GString(l_War.DefendingSide().size()) + L" total" + L")";
        if(in_bLogAllParticipants)
        {
            l_sWarInfo += L": ";
            for(auto it = l_War.DefendingSide().cbegin(); it != l_War.DefendingSide().cend(); ++it)
                l_sWarInfo += m_pCountryData[*it].NameAndIDForLog() + L", ";
        }

        l_sWarInfo += L"(ID " + GString(in_iWarID) + L")";

    } while(false);

    return l_sWarInfo;
}