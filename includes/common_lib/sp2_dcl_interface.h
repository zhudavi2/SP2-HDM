/**************************************************************
*
* golem_dclinterface.h
*
* Description
* ===========
*  Describes the interface of class GDCLInterface
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_DCLINTERFACE_H_
#define _GOLEM_DCLINTERFACE_H_

namespace SP2
{
   class GCountryDataItf;

   typedef GCountryDataItf* (*CREATE_COUNTRY_DATA_FUNC)();

/*!
 * Data Control Layer interface
 **/
class GDCLInterface 
{
public:
   GDCLInterface(GDALInterface* in_pDAL);
   virtual ~GDCLInterface();

   //! Allocate memory for fast search in move possible method.
   void InitCacheMemory(void);

   //! Calculate the best path to go from the start to the end
/*   GFullPath CalculatePath(const GVector2D<REAL32>& l_Start,
                           const GVector2D<REAL32>& l_End);*/

   //! Translates a coordinates in earth degree into meters from the origin
   static void CoordDegreesToKM(GVector2D<REAL32>& io_Coord);
   //! Translates a coordinates in meters from the origin into degrees
   static void CoordKMToDegrees(GVector2D<REAL32>& io_Coord);

	//! Converts a length in KMs at the given position into an estimation of a degree, latitude, longitude
	static REAL32 KMLengthInDegrees(const GVector2D<REAL32>& in_Coordinate,REAL32 in_fLength);	

   static REAL32 AreaOf2IntersectingCircles(const  GVector2D<REAL32>& in_Circle1Center,
                                            REAL32 in_fCircle1Radius,
                                            const  GVector2D<REAL32>& in_Circle2Center,
                                            REAL32 in_fCircle2Radius);

   UINT32 EarthLocateRegion(REAL32 in_fLongitude,REAL32 in_fLatitude);
   static REAL32 CalculateUnitCost( EUnitType::Enum            in_eCategory,
                                    UINT8 in_iSpeed,           UINT8 in_iSensors,
                                    UINT8 in_iGunRange,        UINT8 in_iGunPrecision,
                                    UINT8 in_iGunDamage,       UINT8 in_iMissilePayload,
                                    UINT8 in_iMissileRange,    UINT8 in_iMissilePrecision,
                                    UINT8 in_iMissileDamage,   UINT8 in_iStealth,
                                    UINT8 in_iCountermeasures, UINT8 in_iArmor );

   //=========================================================================
   //          NUKE HELPERS FUNCTIONS
   //=========================================================================   


   //! Select nuclear targets and assign the missiles to them
   vector<pair<GNukeTarget,vector<GNuclearMissile> > > 
      SelectNuclearTargetsAndAssignMissiles(const vector<GNuclearMissile>& in_vMissileList,
                                     UINT32                         in_iTargetCountry,
                                     ENuclearTargetType::Enum       in_eTargetType,
                                     UINT32                         in_iMaxQtyOfMissilesToAssign,
                                     bool                           in_bOptimizeAssignment = true);

   //! List all the possible nuclear targets for the given country, according to the type specified
   void ListAllNuclearTargets(UINT32 in_iTargetCountry,
                              ENuclearTargetType::Enum in_eTargetType,
                              multimap<UINT32,GNukeTarget>& out_Targets);

   //! Estimates nuclear casualties
   UINT32 EstimateNuclearCasualties(vector<pair<GNukeTarget,vector<GNuclearMissile> > >& in_TargetsAndMissiles);
         

   UINT32 EstimateNuclearCasualties(const GNukeTarget& in_Target,
                                    const GNuclearMissile& in_Missile);

   //! Returns the ID of the missiles that are in range of the target
   multimap<UINT32,GNuclearMissile> MissilesInRange(const vector<GNuclearMissile>& in_vMissileList,
                                           const GNukeTarget& in_Target);

   static REAL32 PopulationDensityBySqKM(UINT32 in_iPopulation);	

   //=========================================================================
   //          END NUKE HELPERS
   //=========================================================================   


   //! return if its possible to deploy a group at a certain position.
   bool IsDeployGroupAtPositionPossible(UINT16 in_iDeployingCountry,GVector2D<REAL32> in_DeployPosition,bool in_bNavalDeployment);

   /*!
   * Return true if a unit group from that country can move from region A to region B
   * @param		in_iCountryMoving   : The country trying to move
	* @param		in_iStartRegion	  : Region where the unit group would be starting
	* @param		in_iDestinationRegion  : Region where the unit group is going
   * @return   True if move is possible
   **/
   bool MovePossible(UINT32 in_iCountryMoving, UINT32 in_iStartRegion, UINT32 in_iDestinationRegion) const;

  /*!
   * Return true if a unit group from that country can move from region A and attack region B without going over any ennemies regions (except the destination of course)
   * @param		in_iCountryMoving   : The country trying to move
	* @param		in_iStartRegion	  : Region where the unit group would be starting
	* @param		in_iDestinationRegion  : Region where the unit group is going
   * @return   True if move is possible
   **/
	bool MoveHostilePossible(UINT32 in_iCountryMoving, UINT32 in_iStartRegion, UINT32 in_iDestinationRegion) const;


	/*!
   * Return true if a unit group from that country will be able to move from region A and attack region B without going over any ennemies regions (except the destination of course)
   * @param		in_iCountryMoving   : The country trying to move
	* @param		in_iStartRegion	  : Region where the unit group would be starting
	* @param		in_iDestinationRegion  : Region where the unit group is going
   * @return   True if move is possible
   **/
	bool MoveHostileFuturePossible(UINT32 in_iCountryMoving, UINT32 in_iStartRegion, UINT32 in_iDestinationRegion) const;

	//! Returns a country ID necessary for that country to move through to the other country
	UINT32 CountryToPassThroughTo(UINT32 in_iCountryID, UINT32 in_iSourceRegion, UINT32 in_iDestinationRegion) const;

   REAL64 IncreaseProductionApproxCost(REAL64 in_fPourcOfIncrease, REAL64 in_fCurrentProd) const;

   //Create Country Data
   GCountryDataItf* CreateCountryData();
   void             RegisterCreateCountryDataMethod(CREATE_COUNTRY_DATA_FUNC in_pFunction);	

   //! Create the directory that contains all save games
   void CreateSaveDirectory();

   static GDCLInterface* Instance;

protected:
   GDALInterface* const       m_pDAL;
   

   
private:


   //! Heuristic path finding function       
   INT32 PathFindingHeuristic(const GVector2D<INT32>&in_Start,
                              const GVector2D<INT32>&in_End,
                              bool  in_bTrueDistance);

   SDK::GPathFinder2D         m_PathFinder2D;

	//Used by MoveHostilePossible and MovePossible
   bool*					         m_pVisitedRegion;
   CREATE_COUNTRY_DATA_FUNC   m_pCreateCountryDataFunction;
};

};//end of namespace sp2

#endif //_GOLEM_DCLINTERFACE_H_
