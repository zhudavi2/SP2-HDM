/**************************************************************
*
* sp2_unit_mover.h
*
* Description
* ===========
*  Describes & implements class GUnitMover, which calculates
*  the path of a unit.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_SP2_UNIT_MOVER_H_
#define _GOLEM_SP2_UNIT_MOVER_H_

namespace SP2
{
   namespace ERegionType
   {
      enum Enum
      {
         Water,
         Owned,
         Allied,
         Enemy,
         Blocked,
         Count,
      };
   }

   /*!
    * Creates movement for a single or multiple units.
    * It ensures units are ready and start at the same
    * location and a path exists between their position
    * and their destination.
    */
   struct GUnitGroupInDeployement
   {
      UINT32               m_pGroupIDToDeploy;
      REAL64               m_fDeployementTime;
   };

   class GUnitMover
      : public SDK::GPathFinderDescriptor
      , public Hector::DesignPattern::GSubject
      , public SDK::GGameDataNode
   {
      friend class GDataControlLayer;
      set<UINT32>*               m_pRegionGroups;

      struct GCountryMovingGroups
      {
         GCountryMovingGroups() : m_fTotalValue(0) {};
         set<UINT32>             m_Groups;
         REAL64                  m_fTotalValue;
      };

      GCountryMovingGroups*      m_pCountryGroupsWithPath;

      hash_set<UINT32>           m_GroupsToFortify;
      list<UINT32>               m_GroupsChangingStatus;

      vector<SP2::GUnitGroupEx*> m_vProductionQueueGroups;  
      vector< list<GUnitGroupInDeployement> > m_vGroupsInDeployement;
      list<GUnitInTraining>      m_ListOfUnitsInTraining;
      
      hash_set<UINT32>           m_BombardingGroups;
      REAL64                     m_fLastBombardmentClock;

      UINT8                      m_pStatusCanBeMerged[EMilitaryStatus::CountTotal];

      struct GGroupMove
      {
         EMilitaryAction::Flags  m_Action;
         UINT32                  m_iDestRegion;
      };

      hash_map<UINT32, GGroupMove> m_GroupsToMove;


      //! Describes a region node interconnection
      struct SRegionEdge
      {
         UINT32            m_iDestID;
         REAL32            m_fDistance;
      };

      //! Describes a region waypoint in the graph
      struct SRegionVertex
      {
         UINT32            m_iRegionId;
         GVector2D<REAL32> m_Center;
         UINT32            m_iNbEdges;
         SRegionEdge*      m_pEdges;
      };

      //! Describes a region node interconnection in the graph file
      struct GRegionEdgeExtended : SRegionEdge
      {
         REAL32   m_fUpDelta;
         REAL32   m_fDownDelta;
      };

      //! Describes a vertex in a path with distance affected by elevation
      struct GVertexElevated
      {
         SRegionVertex*    m_pVertex;
         REAL32            m_fDistanceWithElevation;
      };

      //! Specifies if a naval unit is actually moving or not
      bool                          m_bMovingNavalUnit;

      //! Contains all region vertices arranged in a graph
      vector<SRegionVertex>         m_vRegionGraph;

      //! Associates each region to all vertices it contains
      vector<vector<UINT32> >       m_vRegionVertices;

      //! Control waypoint (like panama and suez)
      hash_map<UINT32,UINT32>       m_vControlledWayPoint;

      //! Path finder used for global movement
      SDK::GPathFinder              m_PathFinder;

      // Used to remember current vertex neighborhood iteration
      UINT32                        m_iCurrentID;
      SRegionVertex*                m_pCurrentVertex;

      // Used to remember what can be visited when finding a path
      UINT8*                        m_pCountryTypes;
      UINT8*                        m_pWaypointTypes;

      // Used to remember all currently refused movements
      SDK::GGameEventSPtr                 m_pRefusedMoveEventSPtr;
      SP2::Event::GRefuseMilitaryAction*  m_pRefusedMoveEvent;

      SDK::Combat::GArena*          m_pArena;

      //! Returns the ID of the graph region at the specified location
      bool LocateRegionVertex(const GVector2D<REAL32>& in_Dest,
                              UINT32& out_iVertexID);

      // GPathFinderDescriptor implementation
      void ResetNeighbors(UINT32 in_iPointID);
      UINT32 NextNeighbor(REAL32& out_fCost);
      REAL32 Heuristic(UINT32 in_iStartID, UINT32 in_iEndID);

      /*!
       * \brief Update the country visitability (does this word exist??) using
       *        the current diplomatic statues.
       *
       * All allies of the specified country are marked as visitable or not
       * (depending on the in_bUpdateValue parameter).
       *
       * @param   in_iCountryID     : Country to find allies
       * @param   in_eUpdateValue   : Value used to update the allies visitability (I love that word!)
       **/
      void UpdateAlliedCountry(UINT32 in_iCountryID,
                               ERegionType::Enum in_eUpdateValue,
                               bool in_bCheckTresspassing);

      /*!
       * \brief Update the vertices of the specified country with the specified connectivity.
       *
       * @param   in_iRegionID      : ID of the country to update
       * @param   in_eType          : Type of region connection to establish
       **/
      void ConnectCountry(UINT32 in_iCountryID, ERegionType::Enum in_eType);

      //! Change the status of a ready group to moving
      void ChangeToMovingStatus(SP2::GUnitGroupEx* in_pGroup, REAL32 in_fTime);

      /*!
       * \brief Update the path of the specified group.
       *
       * @param   in_pGroup         : Group that must update its path
       * @param   in_eType          : Type of region connection to establish
       * @param   in_fTime          : Time at whitch the path is changed
       **/
      void ChangeGroupPath(SP2::GUnitGroupEx* in_pGroup, const GGroupMove& in_Path, REAL32 in_fTime);

      /*!
       * \brief Update the speed of all currently moving units of a coutnry.
       *
       * @param   in_iCountryID     : ID of the country to update
       * @param   in_fTime          : Time at whitch to update the speed
       **/
      void UpdateCountryMoveSpeeds(UINT32 in_iCountryID, REAL32 in_fTime);

      /*!
       * \brief Handles when a group is automatically destroyed by the unit manager.
       *
       * @param   in_pGroup         : Group that is about to be destroyed
       */
      void OnDestroyGroup(SDK::Combat::GUnitGroup* in_pGroup);

      /*!
       * Create a combat between two unit groups. If combat is not possible
       * between units, delete the combat.
       *
       * @param in_pGroupA          : Unit that creates the combat arena.
       * @param in_pGroupB          : Unit that creates the combat arena.
       */
      void CreateTwoSidedCombat(SP2::GUnitGroupEx* in_pGroupA, SP2::GUnitGroupEx* in_pGroupB);

      /*!
       * Add the specified unit to an existing combat arena.
       *
       * @param in_pGroup           : Unit that joins the combat arena.
       * @param in_iOwner           : ID of the country where hostility must be checked.
       */
      void JoinCombat(SP2::GUnitGroupEx* in_pGroup);

      /*!
       * Cancel any pending or ongoin movement for the specified unit.
       *
       * @param in_pGroup           : Unit that must stop moving.
       */
      void CancelMovement(SP2::GUnitGroupEx* in_pGroup);

      // GGameDataNode implementation
      virtual bool OnSave(GIBuffer& io_Buffer);
      virtual bool OnLoad(GOBuffer& io_Buffer);
      virtual bool OnNew(GDatabase* in_pDatabase);
      virtual void OnClean();

   public:
      /*!
       * Initialize the class by loading the appropriate data for the
       * region path finding
       *
       * @return  Boolean indicating if initialization was sucessful
       **/
      bool Init();
      void Shutdown();   

      /*!
       * Cancels the status change for the given unit group
       * @param in_pGroup           : Unit to cancel the status change on
       **/
      void CancelStatusChange(SP2::GUnitGroupEx* in_pGroup);


      /*! 
       * Add a group to be maintainable for movement.
       *
       * @param   in_pGroup   : Group that is added.
       **/
      void AddGroup(SDK::Combat::GUnitGroup* in_pGroup);

      //! Return the city location of the closest city from the initial location, for that region only.
      bool FindClosestCityLocation(const GVector2D<REAL32>& in_GroupLocation, 
                                   UINT32 in_iRegionID,
                                   GVector2D<REAL32>& out_CityLocation);      


      /*!
       * Perform offshore bombardment with the units in the list
       * (make the units bombard)
       **/
      void PerformOffShoreBombardment();


      //! Returns the closest naval waypoint for that region
      bool FindNavalWaypointAdjacentToRegion(UINT32 in_iRegionID, Hector::GVector2D<REAL32>& out_Position) const;

      /*!
       * Update unit movement when a region military control is changed.
       *
       * @param in_iRegionID        : ID of region that changed owner.
       * @param in_iNewControl      : ID of the new military control for this region.
       * @param in_iPreviousControl : ID of the previous military control for this region.
       */
      void UpdateRegionMilitaryControl(UINT32 in_iRegionID, UINT32 in_iNewControl, UINT32 in_iPreviousControl);

      /*!
       * Used to enable multi-step unit orders (ex. when
       * a unit is asked to move but it is not ready, it must
       * first switch to ready, then it can move).
       *
       * @return Boolean that indicating if no error were detected.
       */
      bool Iterate();

      void IterateGroupsInDeployement(void);
      void IterateUnitsInTraining(void);

      bool FinishDeployingGroup(SP2::GUnitGroup* in_pGroup);
      void CreateDeployementList(void);
      void DestroyDeployementList(void);

      /*!
       * Change the status of a single unit.
       *
       * @param in_pUnitGroup    : Pointer to the group that must change status.
       * @param in_eNewStatus    : New status of the unit.
       *
       * @return Boolean that is true only if no error occured.
       */
      bool ChangeUnitState(SDK::Combat::GUnitGroup* in_pUnitGroup,
                           EMilitaryStatus::Enum in_eNewStatus,
                           bool in_bMoveAfter = false);

      /*!
       * Move multiples unit to the specified destination
       *
       * @param   in_vUnitGroups    : Units to move
       * @param   in_Dest           : Destination of the units
       *
       * @return  A boolean indicating if movement is allowed
       **/
      bool MoveUnits(const vector<SDK::Combat::GUnitGroup* >& in_vUnitGroups,
                     const GVector2D<REAL32>& in_Dest,
                     EMilitaryAction::Flags in_Action = EMilitaryAction::Default);

      /*!
       * Return a location inside a region. Useful to get a point inside a region without cities.
       *
       * @param        in_iRegionID    : The region from which we want a location
       *
       * @return   A GVector2D that includes the longitude/latitude of that location
       **/
      const GVector2D<REAL32>& RegionLocation(UINT32 in_iRegionID);

      /*!
       * Create the unit groups where the created units will have be placed when built.
       **/
      void   CreateProductionQueueUnitGroups();

      /*!
       * Get the UnitProductionQueue vector
       **/
      const vector<SP2::GUnitGroupEx*>& ProductionQueueUnitGroups() const {return m_vProductionQueueGroups;}

      /*!
       * Update the location of the unit building queue with a new capital.
       * @param      in_iCountryID :   The country that has changed capital
       */
      void UpdateCapitalLocation(UINT32 in_iCountryID);

      //! Train a list of units
      void TrainUnits(list<UINT32>& in_UnitListToTrain);

      //! Cancel a unit in training
      void CancelTraining(UINT32 in_UnitId);

      //! Return true if unit is in training
      bool IsUnitInTraining(UINT32 in_iUnitId);


        /*!
         * Fills the outgoing vector with ID's of unit groups inside that region.
         * @param		in_iRegionID:	The region where we want to get the units
         * @param		out_vUnitGroups: The outgoing vector containing the unit group ids.
         * @param    in_iCountryID: We can specify unit groups of a certain country. By default returns every unit groups.
         **/
        void UnitGroupsInsideRegion(UINT32 in_iRegionID, vector<UINT32>& out_vUnitGroups, ENTITY_ID in_iCountryID = 0) const;

        /*!
         * Return the list of all group IDs in the specified region.
         * @param		in_iRegionID   : The region where we want to get the unit group IDs
         * @return   A set of unit group IDs
         **/
        const set<UINT32>& UnitGroupsInsideRegion(UINT32 in_iRegionID) const;


        const hash_set<UINT32>& BombardingGroupList() const;
        void               AddBombardingGroup(UINT32 in_iId);
        bool               RemoveBombardingGroup(UINT32 in_iId);

		

		  /*!
         * Return true if a unit was deploy.
         * @param		in_pGroup            : The group in which the unit must be removed (deployed)
			* @param		in_UnitListToDeploy  : A list of unitId to deploy
			* @param		in_Dest              : The destination (long-lat) to where should the unit be deployed.
         * @return   True if was  deployed.
         **/
		  void DeployUnit(SP2::GUnitGroup* in_pGroup,list<UINT32>& in_UnitListToDeploy, const GVector2D<REAL32>& in_Dest);

        //! Cancel the deployement of a specific group
        void CancelDeployement(SP2::GUnitGroup* in_pGroup);

			/*!
			*	Removes automatically units from the leaving country to the closest allied region
			*	@param in_iCountryLeaving : The country that needs to move its troops
			*  @param in_iCountryAsking  : The country that asked to move troops away from its territory
            *  @return True if at least one unit actually moved
			**/
		  bool MilitaryRemoval(UINT32 in_iCountryLeaving, UINT32 in_iCountryAsking);

			/*!
			*	Moves a unit group towards that region cities, or middle if no cities exist
			*	@param in_iUnitGroupID : Unit to move
			*  @param in_iDestRegion: Destination
			*	@param in_eAction: Type of movement
			**/
		  void MoveUnitToRegion(UINT32 in_iUnitGroupID, UINT32 in_iDestRegion, SP2::EMilitaryAction::Flags in_eAction = SP2::EMilitaryAction::Default);

  		/*!
		* Clear the deployment list for that country
		**/
		void ClearDeploymentListForCountry(UINT32 in_iCountry);

      /*!
       * Try to merge to specified unit with any unit of the same group that share
       * characteristics (only for units that are in a deployment group).
       **/
      void TryDeployGroupMerge(SP2::GUnit* in_pUnit);

      const list<GUnitInTraining>& ListOfUnitsInTraining() const
      {
         return m_ListOfUnitsInTraining;
      }

      bool IsUnitGroupChangingStatus(UINT32 in_iUnitGroupID) const;
   };
}

#endif // #ifndef _GOLEM_SP2_UNIT_MOVER_H_
