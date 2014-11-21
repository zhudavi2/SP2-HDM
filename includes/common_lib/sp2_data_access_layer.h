/**************************************************************
*
* sp2_data_access_layer.h
*
* Description
* ===========
* Hold the implemantation of DAL Interface for basics function
* that are the same on client and server)
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef __COMMON_LIB_SP2_DATA_ACCESS_LAYER_H__
#define __COMMON_LIB_SP2_DATA_ACCESS_LAYER_H__

#include "sp2_unit_type.h"
#include "sp2_covert_actions_cell.h"

namespace SP2
{

   /*!
    * Predefs
    **/
   class GUnit;
   class GUnitType;
	class GRegion;
	class GCountryData;
   class GDALInterface;

   class GUnitListInformation
   {
   public:
      GVector2D<REAL32>       m_Position;
      EMilitaryStatus::Enum   m_Status;
      SP2::GUnit*             m_pUnit;
   };

   typedef vector<pair<UINT32,vector<GUnitListInformation> > >  GUnitListByType; //Key = design id, value = GUnitListInformation
	typedef vector<pair<UINT8, pair<EUnitCategory::Enum, EUnitDesignCharacteristics::Enum> > > GResearchOffer;

   // Production information of one unit type
   struct GProductionInformation
   {
      REAL64 m_fInitialTimeInDays;
      REAL64 m_fUnitsPerDays;
   };

   /*!
    * Information about a city
    **/
   struct SCityInfo
   {
      UINT32				m_iId;
      UINT32				m_iRegionId;
		GVector2D<REAL32>	m_Position;
		UINT32				m_iPopulation;
      UINT32            m_iNameStringID;

      SCityInfo()
      {
         m_iId          = 0xFFFFFFFF;
      }
   };

   struct GRegionControl
   {
      UINT16   m_iMilitary;
      UINT16   m_iPolitical;

      inline bool operator == (const GRegionControl& in_Compare)
      {
         return *( (UINT32*) this) == *( (UINT32*) &in_Compare);
      }
   };

   /*!
    * Describes a unit in training
    **/
   struct GUnitInTraining
   {
      UINT32                  m_iUnitId;
      REAL32                  m_fTrainingCompletionTime;
      REAL32                  m_fTrainingStartTime;
      ETrainingLevel::Enum    m_ePreviousTrainingLevel;
   };

   /*!
    * Information about a country annex
    **/
   struct GRegionAnnex
   {
      UINT32         m_iAnnexingCountry;
      UINT32         m_iAnnexedRegion;
      REAL64         m_iCompletionTime;
   };

	namespace EDestinationStatus
	{
		enum Enum
		{
			Home,
			Allied,
			Enemy
		};
	};

   /*!
    * Game options
    **/
   class GGameOptions
   {
   public:
      // construction and destruction
      GGameOptions(){Init();};
      virtual ~GGameOptions(){};

      //! Nuclear flags getter and setter
      bool NuclearAllowed() const 
         {return m_bCanNuke;};
      void NuclearAllowed(bool in_bAllowed)
         {m_bCanNuke = in_bAllowed;};

      // Network serialization functions
      virtual bool Serialize(GIBuffer& io_Buffer) const 
         {io_Buffer << m_bCanNuke; return true;};
      virtual bool Unserialize(GOBuffer& io_Buffer) 
         {io_Buffer >> m_bCanNuke; return true;};

      //Init our game options
      void Init(){m_bCanNuke = true;};

   protected:
      //! members
      bool m_bCanNuke;
   };

   /*!
    * Data access layer
    **/
   class GDALInterface
   {
   public:
      GDALInterface();
      virtual ~GDALInterface();

      Map::GMultifaceView* CountryView();

      //! Get a XML tree node
      const GTreeNode<GXMLNode>* XMLNode(const GString& in_sNodename) const;

      //! Get the value of an xml node of the file that has been parsed when initialized
      bool XMLValue(const GString& in_sNodename,GString& out_sValue);

		//! Save a node to the xml
		void SaveNodeToXML(GString& in_NodeName, GString& in_sValue, GString in_sPath);

      //! Get the requested file
      bool File(const GString& in_sFilename,GFile& out_File);
      
      //! Initializes the map engine
      bool MapEngineInit(bool in_bCompleteLoad);

      //! Shutdown the map engine
      bool MapEngineShutdown();

		//! Return true if a multiface has access to water
		bool FindWaterAccess(const Map::GMultiface* in_pMultiface);

		//! Return true if the region, given by the region id, is touching the specified country
		bool RegionTouchesCountry(UINT32 in_iRegionID, UINT32 in_iAttackingCountry) const;	

		//! Return the relation variation between 2 governement type
		REAL32 RelationBetweenGovernmentType(EGovernmentType::Enum in_eFirstType, EGovernmentType::Enum in_eSecondType) const;

      //! Get the Elevation Map
      bool MapEngineElevationMap(const REAL32& in_fCenterX,
                                 const REAL32& in_fCenterY,
                                 const REAL32& in_fWidth,
                                 const REAL32& in_fHeight,
                                 const INT32&  in_iSamplingX,
                                 const INT32&  in_iSamplingY,
                                 REAL32*       out_pMap);

		GString TreatyRefusalEnumToString(ETreatyRefusal::Enum);

		//Returns a %
		REAL32 ProportionGainedByCountryFromCountry(UINT32 in_iCountryHavingMilitaryControl,
																  UINT32 in_iCountryHavingPoliticalRegion);

      //! Get a view for the elevation data
      Map::GContinuousRasterView* MapEngineElevationView(REAL32 in_fCenterX,
                                                         REAL32 in_fCenterY,
                                                         REAL32 in_fWidth,
                                                         REAL32 in_fHeight);
      // Close an opened view
      bool  MapEngineElevationCloseView(Map::GContinuousRasterView* in_pView);

		//! Method to update the trade embargo array
		void UpdateTradeEmbargos();

		//! Method to update the weapon trade embargo array
		void UpdateWeaponTradeEmbargos();

      bool MapEngineWaterData(const REAL32& in_fCenterX,
                              const REAL32& in_fCenterY,
                              const REAL32& in_fWidth,
                              const REAL32& in_fHeight,
                              GShape& out_WaterShape,
                              GFaceList<GPolygon>& out_Water,
                              GFaceList<GPolygon>& out_Islands);

      bool MapEngineCoastlinesClipOuterLimitsRectangle(REAL32 in_fCenterX,
                                                       REAL32 in_fCenterY,
                                                       REAL32 in_fWidth,
                                                       REAL32 in_fHeight,
                                                       Geometry::GShape& out_Shape,
                                                       Geometry::GFaceList<Geometry::GPolygon>& out_Outers,
                                                       Geometry::GFaceList<Geometry::GPolygon>& out_Inners);

      //! Initializes the Data Access Layer
      bool Initialize(const GString& in_sXMLFilename);

      bool LoadCommonConfig();


      UINT32* MilitaryResearchTimeInDays() const;
      REAL64  MilitaryResearchStdBudget() const;


      //Covert action cell cost methods
      static REAL64 CovertActionCellSetupCost(ECovertActionCellTraining::Enum in_eTraining);
      
      //! Shutdown
      virtual bool Shutdown();

      //! Get the distance in kilometers between two points of the earth
      static REAL32 DistanceBetween2PointsLatLong(const GVector2D<REAL32>& in_Start,
                                                  const GVector2D<REAL32>& in_End);

      //! Set the game beginning date
      void ZeroDate(const GDateTime& in_BeginningDate);
      const GDateTime& ZeroDate() const;  //!<Initial Date

      GDateTime        ActualDate() const;   //!<Actual game date based on zero date.
      GDateTime        DateFromTime(REAL64 in_fTime) const;
      
		//! Returns the string table ID for a specific resource.
		INT32 StringIdResource(EResources::Enum in_iResourceID);

		//! Returns the string table ID for a random event.
		INT32 StringIdRandomEvent(ERandomEvents::Enum in_RandomEvent);

		//! Returns the string table ID for a specific religion.
      INT32 StringIdReligion(INT32 in_iReligionID);

      //! Returns the string table ID for a specific language.
      INT32 StringIdLanguage(INT32 in_iLanguageID);

      //! Returns the string table ID for a specific religion.
      INT32 StringIdPoliticalParty(INT32 in_iPoliticalPartyID);

      //! Returns the string table ID for a specific status.
      INT32 StringIdStatus(INT32 in_iStatusID);

      //! Returns the string table ID for a specific gvt type.
      INT32 StringIdGvtType(INT32 in_iGvtTypeID);

		INT32 StringIdObjectiveResult(EAdvisorObjectives::Enum l_Objective, bool in_bImprovement) const;

		//! Returns the string table ID for the specified Advisor action
		INT32 StringIdAdvisorAction(EAdvisorActions::Enum) const;

		//! Returns the string table ID for the specified Advisor objective
		INT32 StringIdAdvisorObjective(EAdvisorObjectives::Enum) const;

      //Enum to string and vice-versa conversion functions
      GString ConvertCellStateToString(ECovertActionsCellState::Enum in_State);
      ECovertActionsCellState::Enum ConvertCellStateToEnum(GString in_sState);

      UINT32 ConvertMissionTypeStrID(ECovertActionsMissionType::Enum in_Type);
      GString ConvertMissionTypeToString(ECovertActionsMissionType::Enum in_Type);
      ECovertActionsMissionType::Enum ConvertMissionTypeToEnum(GString in_sType);

      GString ConvertTargetSectorToString(ECovertActionsTargetSector::Enum in_Target);
      ECovertActionsTargetSector::Enum ConvertTargetSectorToEnum(GString in_sTarget);

      GString ConvertMissionComplexityToString(ECovertActionsMissionComplexity::Enum in_Complexity);
      ECovertActionsMissionComplexity::Enum ConvertMissionComplexityToEnum(GString in_sComplexity);  

      GString ConvertCellTrainingToString(ECovertActionCellTraining::Enum in_Training);
      ECovertActionCellTraining::Enum ConvertCellTrainingToEnum(GString in_sTraining);

      SP2::EResources::Enum ConvertResourcesToEnum(const GString & in_sResourceName);
      GString               ConvertResourcesToString(const SP2::EResources::Enum & in_eResource);

      SP2::EUnitCategory::Enum ConvertUnitCategoryToEnum(const GString & in_sUnitCategoryName);
      GString                  ConvertUnitCategoryToString(const SP2::EUnitCategory::Enum & in_eUnitCategory);


		//! Total of units value (in $) for a country
		REAL32 TotalUnitsValue(UINT32 in_iCountryID, SP2::EUnitCategory::Enum in_eCategory = SP2::EUnitCategory::ItemCount) const;

      void DirtyCountryUnits(UINT32 in_iCountryID);

		//! Returns the total amount of the specified unit type for a country
		UINT32 TotalUnitType(UINT32 in_iCountryID, EUnitType::Enum in_eUnitType) const;		

		//! Total of units value (in $) for that country, and its allies
		REAL32 TotalUnitsValueWithAllies(UINT32 in_iCountryID) const;

		//! Returns the total of unit troups (planes, infantry, tanks, etc...) from a certain type
		//! If no type is given, returns the total number of troups
		INT32 TotalUnitsRaw(UINT32 in_iCountryID, UINT32 in_iUnitType = 0) const;		

		//! Find the ranks of a country in each of the 4 categories
		void FindUnitTypeRanks(UINT32 in_iCountryID, 
										INT32& out_iInfantryRank,
										INT32& out_iGroundRank,
										INT32& out_iAirRank,
										INT32& out_iNavalRank) const;

      inline UINT16 NbCountry() const {return m_iNbCountry;}

      void NbCountry(UINT16 in_iNbCountry);

		UINT16 NbRegion();

      UINT32 InfantryValue() const{return m_iInfantryCost;}

      
      GProductionInformation&  ProductionInformation(EUnitType::Enum in_eUnitType);

      /*!
       * \brief Return the list of military control for each region.
       *
       * The returned vector is 1 based, 0 is water. In the array, an owner
       * of 0 means the regions is not and can't be controlled by any country.
       *
       * @return Vector of military control for each region
       */
      const vector<GRegionControl>& RegionControlArray() const;

      /*!
       * \brief Return the counry that has military control of the 
       * specified region.
       *
       * The region ID is 1 based (0 is water). An owner of 0 means 
       * the regions is not and can't be controlled by any country.
       *
       * @param in_iRegionID  : ID of the region
       *
       * @return Vector of military control for each region
       */
      GRegionControl RegionControl(UINT32 in_iRegionID) const;

      /*!
       * \brief Return the regions with military control for the specified country.
       *
       * @param in_iCountry   : ID of the country
       *
       * @return Set of region IDs controled by the country military
       */
      const set<UINT32>& CountryMilitaryControl(UINT32 in_iCountry) const;

      /*!
       * \brief Return the regions with political control for the specified country.
       *
       * @param in_iCountry   : ID of the country
       *
       * @return Set of region IDs controled by the country politic
       */
      const set<UINT32>& CountryPoliticalControl(UINT32 in_iCountry) const;

      /*!
       * \brief Return an array indicating the validity of a country (i.e. if
       * a country has been conquered (false) or not (true).
       *
       * @return Array of NbCountry() + 1 booleans
       */
      const bool* CountryValidityArray() const;

      /*!
       * \brief Return if the country ID is valid (not conquered and within country count bounds).
       *
       * @param in_iCountry   : ID of the country
       *
       * @return Boolean
       */
      bool CountryValidityArray(UINT32 in_iCountry) const;

		GString GetString(INT32 in_iStringId);

      bool LoadUnitTypes();
      const vector<GUnitType*>& GDALInterface::UnitTypes() const;

      const REAL32** MissileCombatEfficiencyTable();
      const REAL32** GunCombatEfficiencyTable();


      //! Get the unit quantity and value by country and category 
      void UnitQtyByCountryAndCategory(UINT32 in_iCountryID,
                                       EUnitCategory::Enum in_Category,
                                       UINT32& out_iQty) const;


      bool FetchUnitListInformation(UINT32 in_iCountryID,
                                    GUnitListByType*   out_pUnitListByType,
                                    const set<UINT32>* in_pUnitGroupList = NULL);

      bool LoadCountryData();

      //! Return the required city information.
      /*!
       * A null pointer means the requested city does not exist.
       * @param in_iCityID    : ID of the city to retrieve.
       * @return A pointer to a structure describing the city.
       **/
      const SCityInfo*           City(UINT32 in_iCityID) const;

      //! Return the information about all cities.
      /*!
       * An element that has ID 0xFFFFFFFF is not valid.
       * @return A vector of structures describing each city.
       **/
      const vector<SCityInfo>&   Cities() const;

      //! Return the ID of all cities in the specified region.
      /*!
       * All IDs in the list should be valid
       * @param in_iRegionID  : ID of the region to fetch city IDs.
       * @return A vector of city ID.
       **/
      const vector<UINT32>&      CitiesInRegion(UINT32 in_iRegionID) const;

      //! Indicate if the specified city is a capital.
      /*!
       * If the city is a capital, the country can easily be identified by,
       * taking the military control of the city region.
       * @param in_City       : City to test.
       * @return Boolean that is true only if the city is the capital of a country.
       **/
      bool                       CityIsCapital(const SCityInfo& in_City) const;

      //! Return the capital ID.
      /*!
       * If the ID has the last bit set (masked by value 0x80000000), the
       * capital is a region (remove mask by using value 0x7FFFFFFF), else the
       * capital is a city. If the ID equals 0x80000000, the country has no capital.
       * @param in_iCountryID : ID of the country to retrieve the capital ID.
       * @return The ID of the capital.
       **/
      UINT32                     CapitalID(UINT32 in_iCountryID) const;

      //! Return the capital city of the specified country.
      /*!
       * A null pointer means the requested country does not have a city as its capital.
       * @param in_iCountryID : ID of the country to retrieve the capital.
       * @return A pointer to a structure describing the city.
       **/
      const SCityInfo*           CapitalCity(UINT32 in_iCountryID) const;

      //! Return the capital region of the specified country.
      /*!
       * If the CapitalCity method returns null, the region acts as the 
       * capital for the country. If this method also returns 0, there is 
       * no capital for this country.
       * @param in_iCountryID : ID of the country to retrieve the capital.
       * @return The ID of the region containing the capital.
       **/
      UINT32                     CapitalRegion(UINT32 in_iCountryID) const;

      //! Return if the specified country has a capital.
      /*!
       * @param in_iCountryID : ID of the country to retrieve the capital.
       * @return A boolean that is true only if the country has a capital.
       **/
      bool                       CapitalExists(UINT32 in_iCountryID) const;

      //! Return the location of the capital of a coountry.
      /*!
       * The position is invalid if the country does not have a capital.
       * @param in_iCountryID : ID of the country to retrieve the capital.
       * @return The position of the capital.
       **/
      const GVector2D<REAL32>&   CapitalPosition(UINT32 in_iCountryID) const;

      //! Change the capital of the specified country.
      /*!
       * If the capital ID has the last bit set (masked by value 0x80000000), the
       * capital is a region (remove mask by using value 0x7FFFFFFF), else the
       * capital is a city. If the ID equals 0x80000000, the country has no capital.
       * @param in_iCountryID : ID of the country to change the capital.
       * @param in_iCapitalID  : ID of the region to set as capital.
       **/
      void ChangeCapitalID(UINT32 in_iCountryID, UINT32 in_iCapitalID);

      //! Return the list of currently ongoing annexes
      const hash_map<UINT32, GRegionAnnex>& Annexes() const;

      //! Return if the specified region is currently under annexion.
      /*!
       * @param in_iRegionID  : ID of the region to test.
       * @return A boolean that is true if the region is annexed.
       **/
      bool RegionIsAnnexed(UINT32 in_iRegionID) const;

      //! Return the annex structure of the specified region.
      /*!
       * The returned pointer will be NULL if the region is not annexed.
       * @param in_iRegionID  : ID of the region to test.
       * @return The annex struture for the region.
       **/
      const GRegionAnnex* RegionAnnex(UINT32 in_iRegionID) const;

		//! Return the continent where that region is
		EContinent::Enum RegionContinent(UINT32 in_iRegionID) const;

		//! Return the geogroup where that region is
		EGeoGroups::Enum RegionGeoGroup(UINT32 in_iRegionID) const;

		//! Return list of treaties
      const hash_map<UINT32, GTreaty>& Treaties() const;

		//! Return the list of current wars
		const hash_map<UINT32, GWar>& GDALInterface::CurrentWars() const;

      //! Returns an array of treaty types of the specified country with each other country
      /*!
       * This is a fast way to test the treaty status of two countries (at war, neutral, or allied)
       * @param in_iCountryID    : ID of a country on which we want treaty info
       * @return Array of bytes whose values can be 0 (at war), 1 (neutral) or 2 (allied)
       **/
      const UINT8* CountryDiplomaticStatuses(UINT32 in_iCountryID) const;

      //! Return the current diplomatic status between two countries
      /*!
       * This is a fast way to test the treaty status of two countries (at war, neutral, or allied)
       * @param in_iFirstCountryID  : ID of a country on which we want treaty info
       * @param in_iSecondCountryID : ID of a country on which we want treaty info
       * @return Diplomatic relation of the two countries
       **/
      EDiplomaticStatus::Enum DiplomaticStatus(UINT32 in_iFirstCountryID, UINT32 in_iSecondCountryID) const;

      //! Return the list of trade embargo status with each other country
		const bool* CountryTradeEmbargos(UINT32 in_iCountryID) const;

		//! Return true if the first country has a trade embargo towards the second country
		bool TradeEmbargo(UINT32 in_iFirstCountryID, UINT32 in_iSecondCountryID) const;

		//! Returns the whole array of trade embargos
		const bool* TradeEmbargos() const;

		//! Return the list of weapon trade embargo status with each other country
		const bool* CountryWeaponTradeEmbargos(UINT32 in_iCountryID) const;

		//! Return true if the first country has a weapon trade embargo towards the second country
		bool WeaponTradeEmbargo(UINT32 in_iFirstCountryID, UINT32 in_iSecondCountryID) const;

		//! Returns the whole array of weapon trade embargos
		const bool* WeaponTradeEmbargos() const;

		//! Return the complete list of wars
		const hash_set<SWarPair>& Wars() const;

		//! Return true if the specified country is at war with one or more other nations
      /*!
       * @ return true if this country is at war with 1 or more countries
       **/
		bool IsAtWar(UINT32 in_iCountryID) const;

		/*! Return the list of allies of a country
		*	@param in_iCountryID: The country we want the allies from
		*  @param out_vListOfAllies: The list of allies
		**/
		void CountryAllies(UINT32 in_iCountryID, set<UINT32>& out_vListOfAllies) const;

		//! Fills the vector of all the countries ID at war with that country
		void IsAtWarWith(UINT32 in_iCountryID, set<UINT32>& out_vListOfCountries) const;

      static GDALInterface* Instance;

      Map::GMultifaceMap& Coastlines() { return m_CoastlinesMap; }
      Map::GMultifaceMap& Admin();
      Map::GLoneAreaMap& Water() { return m_WaterMap; }
      Map::GContinuousRasterMap& Elevation() { return m_ElevationMap; }
      Map::GCurveMap& River() { return m_RiverMap; }
      Map::GCurveMap& Road() { return m_RoadMap; }

      //! Return a graph that contains interconnect between regions
      const GRegionConnectGraph& RegionGraph() const { return m_RegionGraph; }

      //! Return the speed (in km/day) at which the specified military value will move
      /*!
       * @param in_fMilitaryValue   : Military value, in dollars
       * @return Real number representing the speed
       **/
      REAL32 MilitarySpeed(REAL64 in_fMilitaryValue) const;

      //! Return the delay (in days) before the specified military value begins to move
      /*!
       * @param in_Status: Higher status of target/source
		 * @param in_fInfrastructureLevel : Infrastructure level of country
       * @return Real number representing the delay
       **/
		REAL32 MilitaryMoveDelay(EDestinationStatus::Enum in_Status, 
										 REAL32 in_fInfrastructureLevel) const;

		//! Return the correct string ID for that news sentence
		INT32 FindNewsStringID(News::EType::Enum) const;

      //! Gets the category of the news according to the type
      News::ECategory::Enum NewsCategory(News::EType::Enum in_eType) const;
      //! Gets the priority of the news
      UINT8 NewsPriority(News::EType::Enum in_eType) const;

		EEHECategory::Enum EHEObjectiveCategory(UINT32 in_iObjectiveID) const;
		EEHECategory::Enum EHEActionCategory(UINT32 in_iActionID) const;

		//! Returns a string for a generic name of a treaty. 
		GString TreatyName(ETreatyType::Enum in_iTreatyType, const GString & in_iConstructorName);

      //! Return a cost modificator in function of a production speed
      REAL32   ProductionSpeedCostModificator(UINT16 in_iSpeedLevel);

      //! Return a ratio for the unit cost when selling unit to other country
      REAL32   SellingUnitCostRatio(void)const;

      //! Return the training cost for a given training level
      REAL32 TrainingUnitCost(ETrainingLevel::Enum in_eTrainingLevel) const;

      //! Return the training time for a given training level
      REAL32 TrainingUnitTime(ETrainingLevel::Enum in_eTrainingLevel) const;

		//! Return the value of a tech level. The first parameter is the tech level of the country offering a tech. The second parameter is the tech level of the other country
		REAL64 GetTechValues(UINT8 in_iTechLevelOffering, UINT8 in_iTechLevelWant) const;

      REAL64 AMDSResearchCost() const;
      REAL32 AMDSResearchTimeDays() const;

      inline SDK::GGameDataManager* DataManager() { return &m_DataManager; }

      //             BOMBARDMENT METHODS
      //------------------------------------------------

      const vector<GVector2D<REAL32> >& RegionBombardmentPoints(UINT32 in_iRegionId) const;
      //! Finds the closest bombardment point, false if no point found
      bool                              ClosestBombardmentPoint(UINT32 in_iRegionID,
                                                                const GVector2D<REAL32>& in_ActualPosition,
                                                                GVector2D<REAL32>& out_ClosestPoint);

      bool                              RandomBombardmentPoint(UINT32 in_iRegionID,GVector2D<REAL32>&       out_ClosestPoint);

      //! Lists the regions that can be bombarded
      vector<UINT32>                    BombardableRegions(UINT32 in_iCountryID);

      //! Lists all the unit groups of a country that can perform bombardment
      vector<SDK::Combat::GUnitGroup*>          UnitGroupsThatCanBombard(UINT32 in_iCountryId);


      //---------- end bombardment methods -------------------
      
      GGameOptions& GameOptions() {return m_GameOptions;};      

   protected:
      GUnitListByType::iterator FindUnitListInformationDesign(GUnitListByType* in_pList,
                                                              UINT32 in_iDesignID);

      //! Configuration XML Tree
	   GTree<GXMLNode>*	    m_pConfigFile;

      //! Time in days to research military at the normal budget
      UINT32 m_MilitaryResearchTimeInDays[c_iMaxResearchLevel];
      REAL64 m_fMilitaryResearchStandardBudget;

      //Map engine
      Map::GMultifaceMap            m_AdminMap;
      Map::GMultifaceView*          m_pAdminView;
      Map::GLoneAreaMap             m_WaterMap;
      Map::GContinuousRasterMap     m_ElevationMap;
      Map::GMultifaceMap            m_CoastlinesMap;
      Map::GCurveMap                m_RiverMap;
      Map::GCurveMap                m_RoadMap;

      vector<GUnitType*> m_vUnitTypes;   

      REAL32**   m_CombatEfficiencyTableGun;
      REAL32**   m_CombatEfficiencyTableMissile;		

      // Cached region military control
      void ModifyMilitaryControl(UINT32 in_iRegionID, UINT32 in_iNewControl);

      // Cached region political control
      void ModifyPoliticalControl(UINT32 in_iRegionID, UINT32 in_iNewControl);

      // Array is 1 based (0 is water)
      vector<GRegionControl>           m_vRegionControl;

      // Array is 1 based (0 is "neutral lands", like water and antartica).
      // The pair content : fist is military control, second is political control.
      vector<pair<set<UINT32>, set<UINT32> > >  m_vCountryControl;

      //! Array with an element for each 
      // Array is 1 based (0 is "neutral lands", and are never valid)
      bool*                            m_pCountryValidityArray;
      GProductionInformation            m_ProductionTable[EUnitType::TotalCount];

      //! Cities
      vector<SCityInfo>                m_vCities;

      //! Cities by region
      vector<vector<UINT32> >          m_vRegionCities;

      //! Capitals ID by country
      // An ID whith the last bit set refers to a region
      vector<UINT32>                   m_vCapitalsCityID;

      //! List of current annexion, mapped by region ID
      hash_map<UINT32, GRegionAnnex>   m_AnnexedRegions;

      //! List of all treaties
      hash_map<UINT32, GTreaty>        m_Treaties;

		//! List of all current wars
      hash_map<UINT32, GWar>				m_CurrentWars;

      //! List of all countries in war
		hash_set<SWarPair>               m_Wars;

      //! Method to update the diplomatic status array
      void UpdateDiplomaticStatuses();		

      //! Loads the bombardment point list
      void LoadBombardmentPointList();


      //! 2D array which indicates the diplomatic relation between two country
      UINT8*            m_pDiplomaticArray;
		//! 2D array which indicates the trade embargos between countries
		bool*					m_pTradeEmbargos;
		//! 2D array which indicates the weapon trade embargos between countries
		bool*					m_pWeaponTradeEmbargos;

      //String table environement
      vector<INT32>     m_vStringTableIdsReligions;
		vector<INT32>     m_vStringTableIdsResources;		
      vector<INT32>     m_vStringTableIdsLanguages;
      vector<INT32>     m_vStringTableIdsPoliticalParties;
      vector<INT32>     m_vStringTableIdsGvtType;
      vector<INT32>     m_vStringTableIdsStatus;

		INT32*            m_StringTableIdsObjectiveResults;
      INT32             m_NewsString[News::EType::Count];
		INT32             m_StringTableIdsRandomEvents[ERandomEvents::ItemCount];
		INT32             m_AdvisorActionsString[EAdvisorActions::Undefined];
		INT32             m_AdvisorObjectivesString[EAdvisorObjectives::Undefined];

		//Contains the continent and geogroup of regions
		vector<EContinent::Enum> m_vRegionContinent;
		vector<EGeoGroups::Enum> m_vRegionGeoGroup;

      UINT32 m_iInfantryCost;


      GRegionConnectGraph           m_RegionGraph;
		UINT16								m_iNbRegion;

      struct GCountryUnitValue
      {
         REAL32               m_pValues[EUnitCategory::ItemCount + 1];
         REAL32               m_fTotalMissileValue;
         UINT32               m_iDirtyFlag;
      };

      void UpdateCountryUnitValues();
      void ComputeTotalUnitsValue(UINT32 in_iCountryID);

      vector<GCountryUnitValue>     m_vCountryUnitValues;


      //! Bombardment point list by region
      vector<vector<GVector2D<REAL32> > > m_vBombardmentPointsByRegion;

      // our game options data
      GGameOptions m_GameOptions;

   private:
      UINT16                        m_iNbCountry;
      GDateTime                     m_ZeroDate;

      //! Selling unit cost ratio, when selling units to another country
      REAL32                        m_fSellingUnitCostRatio;
      REAL32                        m_fTrainingUnitTime[ETrainingLevel::ItemCount];
      REAL32                        m_fTrainingUnitCost[ETrainingLevel::ItemCount];

      //! Loaded from the common config file - Research cost for the AMDS system
      REAL64 m_fAMDSResearchCost;
      //! Loaded from the common config file - Research time for the AMDS system
      REAL32 m_fAMDSResearchTimeInDays;

      SDK::GGameDataManager        m_DataManager;
   };

}

#endif //__COMMON_LIB_SP2_DATA_ACCESS_LAYER_H__
