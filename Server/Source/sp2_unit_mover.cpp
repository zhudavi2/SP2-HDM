/**************************************************************
*
* sp2_unit_mover.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"


using namespace SDK::Combat;

//#define __LOG_GROUP_MODIFICATIONS__
//#define __LOG_PATH_MODIFICATIONS__

#ifdef __LOG_GROUP_MODIFICATIONS__
   #define LOG_MODIF(text) (g_Joshua.Log(text) );
#else //#ifdef __LOG_GROUP_MODIFICATIONS__
   #define LOG_MODIF(text) ;
#endif //#ifdef __LOG_GROUP_MODIFICATIONS__

#ifdef __LOG_PATH_MODIFICATIONS__
   #define LOG_PATH(text) (g_Joshua.Log(text) );
#else //#ifdef __LOG_PATH_MODIFICATIONS__
   #define LOG_PATH(text) ;
#endif //#ifdef __LOG_PATH_MODIFICATIONS__

const REAL32 c_fWaterCostModifier = 1.0f;
const REAL32 c_fOwnedCostModifier = 1.0f;
const REAL32 c_fAllyCostModifier = 2.0f;
const REAL32 c_fEnemyCostModifier = 10.0f;

const REAL32 c_pCostTable[ERegionType::Count] = 
{
   c_fWaterCostModifier,
   c_fOwnedCostModifier,
   c_fAllyCostModifier,
   c_fEnemyCostModifier,
   0,
};

const REAL32 c_fDelayStatusParkedToReady = 4.f;
const REAL32 c_fDelayStatusAnyToParked = 2.f;
const REAL32 c_fDelayStatusReadyToFortified = 2.f;

const REAL32 c_fMergeThresSquared = 0.0001f;


const REAL64 c_fOffShoreBombardmentsFrequencyDays = 1.f;
const REAL32 c_fOffshoreBombardmentImpactOnResourcePercent = 0.01f;
const REAL32 c_fOffshoreBombardmentImpactOnInfrastructure  = 0.01f;

const REAL32 c_fHeightToDistanceFactor = 0.025f;

bool GUnitMover::LocateRegionVertex(const GVector2D<REAL32>& in_Dest, 
                                    UINT32& out_iVertexID)
{
   // Locate region of the starting point
   UINT32 l_iRegionID = 0;
   const Map::GMultiface* l_pMF = g_ServerDAL.CountryView()->Locate(in_Dest.x, in_Dest.y);
   if(l_pMF)
   {
      l_iRegionID = l_pMF->Id();
   }

   // Load possible vertices
   assert(l_iRegionID < m_vRegionVertices.size() );
   vector<UINT32>& l_vVertices = m_vRegionVertices[l_iRegionID];

   if(l_vVertices.size() == 0)
      return false;

   // Find closest vertex to desired location
   out_iVertexID = l_vVertices[0];
   REAL32 l_fDist = m_vRegionGraph[out_iVertexID].m_Center.Distance(in_Dest);
   for(UINT32 v = 1;v < l_vVertices.size();v ++)
   {
      UINT32 l_iCompareID = l_vVertices[v];
      REAL32 l_fCompareDist = m_vRegionGraph[l_iCompareID].m_Center.Distance(in_Dest);
      if(l_fCompareDist < l_fDist)
      {
         out_iVertexID = l_iCompareID;
         l_fDist = l_fCompareDist;
      }
   }

   return true;
}

void GUnitMover::ResetNeighbors(UINT32 in_iPointID)
{
   m_pCurrentVertex = &(m_vRegionGraph[in_iPointID] );
   m_iCurrentID = 0;
}

UINT32 GUnitMover::NextNeighbor(REAL32& out_fCost)
{
   // Skip neighbors until a visitable waypoint is found
   while(m_iCurrentID < m_pCurrentVertex->m_iNbEdges)
   {
      SRegionEdge& l_Edge = m_pCurrentVertex->m_pEdges[m_iCurrentID];
      m_iCurrentID ++;


      //Moving a standard unit
      if(!m_bMovingNavalUnit)
      {
         if(m_pWaypointTypes[l_Edge.m_iDestID] != ERegionType::Blocked)
         {
            out_fCost = l_Edge.m_fDistance * c_pCostTable[m_pWaypointTypes[l_Edge.m_iDestID] ];
            return l_Edge.m_iDestID;
         }
      }
      //Moving a naval unit (only has access to water)
      else
      {
         if(m_pWaypointTypes[l_Edge.m_iDestID] == ERegionType::Water)
         {
            out_fCost = l_Edge.m_fDistance * c_pCostTable[ERegionType::Water];
            return l_Edge.m_iDestID;
         }
      }
   }

   return 0xFFFFFFFF;
}

REAL32 GUnitMover::Heuristic(UINT32 in_iStartID, UINT32 in_iEndID)
{
   SRegionVertex& l_StartVertex = m_vRegionGraph[in_iStartID];
   SRegionVertex& l_EndVertex = m_vRegionGraph[in_iEndID];

   return g_ServerDAL.DistanceBetween2PointsLatLong(l_EndVertex.m_Center, l_StartVertex.m_Center);
}

void GUnitMover::UpdateAlliedCountry(UINT32 in_iCountryID, 
                                     ERegionType::Enum in_eUpdateValue,
                                     bool in_bCheckTresspassing)
{
   // Search treaties for alliances, which updates the connectivity of the country
   const UINT8* l_pDiplomacies = g_ServerDAL.CountryDiplomaticStatuses(in_iCountryID);
   for(UINT32 i = 1;i <= g_ServerDAL.NbCountry();i ++)
   {
      // Make countries are allied
      if(l_pDiplomacies[i] == EDiplomaticStatus::Allied)
      {
         // Remember that this allied country accept or reject
         // that the specified country use their territory to move troops
         m_pCountryTypes[i] = (UINT8) in_eUpdateValue;
      }
      // Also had countries with tresspassing right
      else if( in_bCheckTresspassing && l_pDiplomacies[i] == EDiplomaticStatus::MilitaryAccess)
      {
         m_pCountryTypes[i] = (UINT8) in_eUpdateValue;
      }
   }
}

void GUnitMover::ConnectCountry(UINT32 in_iCountryID, ERegionType::Enum in_eType)
{
   // Update all regions of the country
   const set<UINT32>& l_vRegions = g_ServerDAL.CountryMilitaryControl(in_iCountryID);
	for(set<UINT32>::const_iterator l_Itr = l_vRegions.begin();
		 l_Itr != l_vRegions.end();
		 l_Itr++)
   {
      assert(*l_Itr < m_vRegionVertices.size() );

      // Update all vertices of the region
      const vector<UINT32>& l_vVertices = m_vRegionVertices[ *l_Itr ];
      for(UINT32 i = 0;i < l_vVertices.size();i ++)
      {
         m_pWaypointTypes[l_vVertices[i] ] = (UINT8) in_eType;
      }
   }
}

void GUnitMover::ChangeToMovingStatus(SP2::GUnitGroupEx* in_pGroup, REAL32 in_fTime)
{
   gassert(in_pGroup->Status() == EMilitaryStatus::Ready, "Unit must not be already moving");

	const UINT8* l_pStatuses = g_ServerDAL.CountryDiplomaticStatuses(in_pGroup->OwnerId());
	const vector<GRegionControl>& l_vControls = g_ServerDAL.RegionControlArray();
	EDestinationStatus::Enum l_DestStatus = EDestinationStatus::Home;
	REAL32 l_fInfrastructure = g_ServerDAL.CountryData(in_pGroup->OwnerId())->Infrastructure();
	if(in_pGroup->m_iRegionID)
	{
		if(l_vControls[in_pGroup->m_iRegionID].m_iPolitical == in_pGroup->OwnerId())
			l_DestStatus = EDestinationStatus::Home;
		else if((EDiplomaticStatus::Enum)l_pStatuses[l_vControls[in_pGroup->m_iRegionID].m_iPolitical] == EDiplomaticStatus::Allied)
			l_DestStatus = EDestinationStatus::Allied;
		else
			l_DestStatus = EDestinationStatus::Enemy;
	}
	const vector<SP2::GUnitPathPoint>& l_vPoints = in_pGroup->Path().Points();
	UINT32 l_iRegionDestination = l_vPoints.back().m_iRegionID;
	if(l_iRegionDestination)
	{
		if(l_vControls[l_iRegionDestination].m_iPolitical == in_pGroup->OwnerId())
			l_DestStatus = max(EDestinationStatus::Home,l_DestStatus);
		else if((EDiplomaticStatus::Enum)l_pStatuses[l_vControls[l_iRegionDestination].m_iPolitical] == EDiplomaticStatus::Allied)
			l_DestStatus = max(EDestinationStatus::Allied,l_DestStatus);
		else
			l_DestStatus = max(EDestinationStatus::Enemy,l_DestStatus);
	}

   REAL32 l_fMoveDelay = g_ServerDAL.MilitaryMoveDelay(l_DestStatus, l_fInfrastructure);
   if(in_pGroup->NextStatus() != EMilitaryStatus::Moving)
   {
      // Status change is not started yet, start the status change now
      in_pGroup->ChangeStatus(EMilitaryStatus::Moving, in_fTime, l_fMoveDelay);
      m_GroupsChangingStatus.push_back(in_pGroup->Id() );
   }
   else
   {
      gassert(find(m_GroupsChangingStatus.begin(), m_GroupsChangingStatus.end(), in_pGroup->Id() ) == m_GroupsChangingStatus.end(),
              "A group is changing to moving, but it is not in the status change list");

      // Status change already started, update the delay by using the previous change time.
      in_pGroup->ChangeStatus(EMilitaryStatus::Moving, 
                              in_pGroup->StatusChangeStartTime(), 
                              l_fMoveDelay);
   }

	in_pGroup->Path().Speed( (in_pGroup->IsNaval() ? 0.25f : 1.f) * g_ServerDAL.MilitarySpeed(in_pGroup->Value() ), in_pGroup->StatusChangeEndTime() );

   g_Joshua.UnitManager().ModifyUnitGroup(in_pGroup);
}

void GUnitMover::ChangeGroupPath(SP2::GUnitGroupEx* in_pGroup, const GGroupMove& in_Move, REAL32 in_fTime)
{
   LOG_PATH( GString(L"ChangeGroupPath : ") + GString(in_pGroup->Id() ) );

   gassert(!in_pGroup->Path().Points().empty(), "A path should be present on this group");

   in_pGroup->m_iMoveRegionID = in_pGroup->m_iRegionID;
   in_pGroup->m_iPrevMoveRegionID = in_pGroup->m_iRegionID;
   in_pGroup->m_iRegionID = in_Move.m_iDestRegion;
   in_pGroup->m_ActionAtDest = in_Move.m_Action;

   m_pCountryGroupsWithPath[in_pGroup->OwnerId() ].m_Groups.insert(in_pGroup->Id() );
   in_pGroup->m_fLastMoveValue = in_pGroup->Value();
   m_pCountryGroupsWithPath[in_pGroup->OwnerId() ].m_fTotalValue += in_pGroup->m_fLastMoveValue;
   g_Joshua.UnitManager().ModifyUnitGroup(in_pGroup);

   UpdateCountryMoveSpeeds(in_pGroup->OwnerId(), in_fTime);
}

void GUnitMover::UpdateCountryMoveSpeeds(UINT32 in_iCountryID, REAL32 in_fTime)
{
   //! \todo TBR : This serves as reference for code that update unit speed depending on total moving military value
   /*
   GCountryMovingGroups& l_CountryMoves = m_pCountryGroupsWithPath[in_iCountryID];
#ifdef GOLEM_DEBUG
   REAL64 l_fRealValue = 0;
   for(set<UINT32>::iterator it = l_CountryMoves.m_Groups.begin();
       it != l_CountryMoves.m_Groups.end();++ it)
   {
      SP2::GUnitGroupEx* l_pGroup = (SP2::GUnitGroupEx*) g_Joshua.UnitManager().UnitGroup(*it);
      gassert(l_pGroup, "Invlid group with path in update of speed");
      l_fRealValue += l_pGroup->Value();
   }
   gassert(l_fRealValue == l_CountryMoves.m_fTotalValue, "Incohrent country moving unit values");
#endif

   // could be replaced by 1 / x^0.25 OR 1.f / RSqrt(RSqrt(x) )
   REAL32 l_fSpeed = g_ServerDAL.MilitarySpeed(l_CountryMoves.m_fTotalValue);
   for(set<UINT32>::iterator it = l_CountryMoves.m_Groups.begin();
       it != l_CountryMoves.m_Groups.end();++ it)
   {
      SP2::GUnitGroupEx* l_pGroup = (SP2::GUnitGroupEx*) g_Joshua.UnitManager().UnitGroup(*it);
      gassert(l_pGroup, "Invalid group ptr for speed update");
      l_pGroup->Path().Speed(l_fSpeed, in_fTime);
      g_Joshua.UnitManager().ModifyUnitGroup(l_pGroup);
   }
   */
}

void GUnitMover::OnDestroyGroup(SDK::Combat::GUnitGroup* in_pGroup)
{
   LOG_MODIF( GString(L"OnDestroyGroup : ") + GString(in_pGroup->Id() ) );
   LOG_MODIF( GString(L"Move region : ") + GString( ( (GUnitGroupEx*) in_pGroup )->m_iMoveRegionID) );
   LOG_MODIF( GString(L"Dest region : ") + GString( ( (GUnitGroupEx*) in_pGroup )->m_iRegionID) );

   SP2::GUnitGroupEx* l_pGroupEx = (SP2::GUnitGroupEx*)in_pGroup;

	UINT32 l_iID = in_pGroup->Id();


   CancelMovement( (SP2::GUnitGroupEx*) in_pGroup);
   CancelStatusChange( (SP2::GUnitGroupEx*) in_pGroup);

   m_GroupsToFortify.erase(l_iID);
   list<UINT32>::iterator l_It = 
      find(m_GroupsChangingStatus.begin(), m_GroupsChangingStatus.end(), l_iID);
   if(l_It != m_GroupsChangingStatus.end() )
   {
      m_GroupsChangingStatus.erase(l_It);
   }
   m_pRegionGroups[ ( (GUnitGroupEx*) in_pGroup )->m_iMoveRegionID].erase(l_iID);
   m_pRegionGroups[ ( (GUnitGroupEx*) in_pGroup )->m_iRegionID].erase(l_iID);	

   //Remove the group from the bombardment list<
   if(l_pGroupEx->m_ActionAtDest & EMilitaryAction::BombardRegion && l_pGroupEx->RegionToBombard())
      RemoveBombardingGroup(in_pGroup->Id());
}

void GUnitMover::CreateTwoSidedCombat(SP2::GUnitGroupEx* in_pGroupA, SP2::GUnitGroupEx* in_pGroupB)
{
   // Create the combat arena
   m_pArena = g_CombatManager.CreateArena(
      g_Joshua.UnitManager().NewArenaInfoID(), (in_pGroupA->Position() + in_pGroupB->Position() ) * 0.5f);

   //Set the region ID
   {
      SP2::GArenaInfo* l_pArenaInfo = (SP2::GArenaInfo*)m_pArena->Info();
      l_pArenaInfo->m_iRegionID = g_ServerDCL.EarthLocateRegion(l_pArenaInfo->m_Coordinate.x,l_pArenaInfo->m_Coordinate.y);

      //Drop stability for the country that suffers the fight
		if(l_pArenaInfo->m_iRegionID != 0 && l_pArenaInfo->m_iRegionID != 111) ///111 = antartica, belongs to nobody
		{
			g_ServerDCL.ChangeCountryStability( g_ServerDAL.GetGRegion(l_pArenaInfo->m_iRegionID)->OwnerId(),
				SP2::c_fStabilityCombatStarted,true);
		}
   }


   // Join each group to the combat
   bool l_bAddSuccess = true;
   LOG_MODIF( GString(L"CreateCombat : ") + 
              GString(in_pGroupA->Id() ) + GString(L" & ") + 
              GString(in_pGroupB->Id() ) + GString(L" -> ") + 
              GString(m_pArena->Info()->m_iId) );
	bool l_bASuccessfullyAdded = m_pArena->AddGroup(in_pGroupA);
   l_bAddSuccess &= l_bASuccessfullyAdded;
	bool l_bBSuccessfullyAdded = m_pArena->AddGroup(in_pGroupB);
   l_bAddSuccess &= l_bBSuccessfullyAdded;

   // Make sure the combat is valid (both sides are inside the combat)
   if(!l_bAddSuccess)
   {
		if(l_bASuccessfullyAdded)
			m_pArena->RemoveGroup(in_pGroupA);
		if(l_bBSuccessfullyAdded)
			m_pArena->RemoveGroup(in_pGroupB);
      g_CombatManager.DeleteArena(m_pArena);
      m_pArena = NULL;
   }
   else
   {
      // Remove the path if present
      CancelMovement(in_pGroupA);
      CancelStatusChange(in_pGroupA);

      CancelMovement(in_pGroupB);
      CancelStatusChange(in_pGroupB);

      g_Joshua.UnitManager().ModifyUnitGroup(in_pGroupA);
      g_Joshua.UnitManager().ModifyUnitGroup(in_pGroupB);

      g_ServerDAL.AddHistoricalMarker(in_pGroupA->OwnerId(), 
                                      EHistoryMarkerType::CombatStarted, 
                                      1);

      g_ServerDAL.AddHistoricalMarker(in_pGroupB->OwnerId(), 
                                      EHistoryMarkerType::CombatStarted, 
                                      1);
   }
}

void GUnitMover::JoinCombat(SP2::GUnitGroupEx* in_pGroup)
{
   LOG_MODIF( GString(L"JoinCombat : ") + GString(in_pGroup->Id() ) + GString(L" -> ") + GString(m_pArena->Info()->m_iId) );

   bool l_bAlreadyInCombat = ( ( (SP2::GArenaInfo*) (m_pArena->Info() ) )->Actor(in_pGroup->OwnerId() ) != NULL);

   // Add the group to the combat
   if(m_pArena->AddGroup(in_pGroup) )
   {
      // Remove the path if present
      CancelMovement(in_pGroup);
      CancelStatusChange(in_pGroup);
      g_Joshua.UnitManager().ModifyUnitGroup(in_pGroup);

      if(!l_bAlreadyInCombat)
      {
         g_ServerDAL.AddHistoricalMarker(in_pGroup->OwnerId(), 
                                         EHistoryMarkerType::CombatStarted, 
                                         1);
      }
   }
}

void GUnitMover::CancelMovement(SP2::GUnitGroupEx* in_pGroup)
{
   LOG_PATH( GString(L"CancelMovement : ") + GString(in_pGroup->Id() ) );

   if(in_pGroup->Status() == EMilitaryStatus::Moving)
   {
      in_pGroup->ChangeStatus(EMilitaryStatus::Ready);
   }

   UINT32 l_iID = in_pGroup->Id();

   // Remove any reference to that group ID
   if(m_pCountryGroupsWithPath[in_pGroup->OwnerId() ].m_Groups.count(l_iID) )
   {
      m_pCountryGroupsWithPath[in_pGroup->OwnerId() ].m_Groups.erase(l_iID);
      m_pCountryGroupsWithPath[in_pGroup->OwnerId() ].m_fTotalValue -= in_pGroup->m_fLastMoveValue;
   }

   m_GroupsToMove.erase(l_iID);

   in_pGroup->Path().Reset();

   m_pRegionGroups[in_pGroup->m_iMoveRegionID].erase(l_iID);
   m_pRegionGroups[in_pGroup->m_iRegionID].erase(l_iID);

   const Map::GMultiface* l_pMF = g_ServerDAL.CountryView()->Locate(in_pGroup->Position() );
   if(l_pMF)
   {
      in_pGroup->m_iRegionID = l_pMF->Id();
   }
   else
   {
      in_pGroup->m_iRegionID = 0;
   }
   in_pGroup->m_iMoveRegionID = 0;

   // Add the group to the region array
   m_pRegionGroups[in_pGroup->m_iRegionID].insert(in_pGroup->Id() );
}

bool GUnitMover::Init()
{
   m_bMovingNavalUnit      = false;
   m_pRefusedMoveEventSPtr = CREATE_GAME_EVENT(SP2::Event::GRefuseMilitaryAction);
   m_pRefusedMoveEvent = (SP2::Event::GRefuseMilitaryAction*) m_pRefusedMoveEventSPtr.get();


   m_fLastBombardmentClock = 0;

   // Fetch region graph path
   GString l_GraphPath;
   if(!g_ServerDAL.XMLValue(REGION_GRAPH, l_GraphPath) )
   {
      g_Joshua.Log(L"No region graph configured", MSGTYPE_ERROR);
      return false;
   }

   // Open region graph data file
   GFile l_GraphFile;
   if(!g_Joshua.FileManager()->File(l_GraphPath, l_GraphFile) )
   {
      g_Joshua.Log(L"Region graph does not exist", MSGTYPE_ERROR);
      return false;
   }

   // Read file
   if(!l_GraphFile.Open() )
   {
      g_Joshua.Log(L"Unable to open region graph", MSGTYPE_ERROR);
      return false;
   }

   // Read number of regions
   UINT32 l_iRegionCount;
   if(!l_GraphFile.Read(&l_iRegionCount, sizeof(UINT32) ) )
   {
      g_Joshua.Log("Problem reading graph header", MSGTYPE_ERROR);
      return false;
   }

   m_vRegionVertices.resize(l_iRegionCount);

   // Read number of vertices
   UINT32 l_iVertexCount;
   if(!l_GraphFile.Read(&l_iVertexCount, sizeof(UINT32) ) )
   {
      g_Joshua.Log("Problem reading graph header", MSGTYPE_ERROR);
      return false;
   }

   // Read each vertex data
   m_vRegionGraph.resize(l_iVertexCount);

   vector<GRegionEdgeExtended> l_vEdges;

   for(UINT32 r = 0;r < l_iVertexCount;r ++)
   {
      SRegionVertex& l_Vertex = m_vRegionGraph[r];

      if(!l_GraphFile.Read(&l_Vertex, sizeof(SRegionVertex) - sizeof(SRegionEdge*) ) )
      {
         g_Joshua.Log("Problem reading graph vertices", MSGTYPE_ERROR);
         return false;
      }

      // Read edge data
      if(l_Vertex.m_iNbEdges)
      {
         l_Vertex.m_pEdges = new SRegionEdge[l_Vertex.m_iNbEdges];
         l_vEdges.resize(l_Vertex.m_iNbEdges);
         if(!l_GraphFile.Read(& (l_vEdges[0] ), sizeof(GRegionEdgeExtended) * l_Vertex.m_iNbEdges) )
         {
            g_Joshua.Log("Problem reading graph edges", MSGTYPE_ERROR);
            return false;
         }

         for(UINT32 e = 0;e < l_Vertex.m_iNbEdges;e ++)
         {
            l_Vertex.m_pEdges[e].m_iDestID = l_vEdges[e].m_iDestID;

            if(l_vEdges[e].m_fUpDelta > l_vEdges[e].m_fDownDelta * 0.2)
            {
               // convert elevation meters to comparable distance kilometers
               // and adjust elevation by an arbitrary factor
               l_vEdges[e].m_fUpDelta *= c_fHeightToDistanceFactor;
               l_vEdges[e].m_fDownDelta *= c_fHeightToDistanceFactor;

               // Remove some elevation effect when there is a down slope
               REAL32 l_fHeightValue = l_vEdges[e].m_fUpDelta - l_vEdges[e].m_fDownDelta * 0.2f;

               REAL32 l_fHeightHypothenuseInv = RSqrt(l_vEdges[e].m_fDistance * l_vEdges[e].m_fDistance + l_fHeightValue * l_fHeightValue);
               REAL32 l_fHeightSlopePenalty = 4 * l_fHeightValue * l_fHeightHypothenuseInv;

               l_Vertex.m_pEdges[e].m_fDistance = l_vEdges[e].m_fDistance * 
                                                  (1 + l_fHeightSlopePenalty);
            }
            else
            {
               l_Vertex.m_pEdges[e].m_fDistance = l_vEdges[e].m_fDistance;
            }
         }
      }
      else
      {
         l_Vertex.m_pEdges = NULL;
      }

      // Insert this vertex into the region array
      m_vRegionVertices[l_Vertex.m_iRegionId].push_back(r);
   }

   REAL32 l_fMaxPenalty = 0.f;
   REAL32 l_fAveragePenalty = 0.f;
   UINT32 l_iEdgeCount = 0;

   REAL32 l_fMaxPenaltyNoWater = 0.f;
   REAL32 l_fAveragePenaltyNoWater = 0.f;
   UINT32 l_iEdgeCountNoWater = 0;

   for(UINT32 r = 0;r < l_iVertexCount;r ++)
   {
      SRegionVertex& l_Vertex = m_vRegionGraph[r];

      for(UINT32 e = 0;e < l_Vertex.m_iNbEdges;e ++)
      {
         REAL32 l_fPenalty = l_Vertex.m_pEdges[e].m_fDistance / g_ServerDAL.DistanceBetween2PointsLatLong(l_Vertex.m_Center, m_vRegionGraph[l_Vertex.m_pEdges[e].m_iDestID].m_Center);
         l_fMaxPenalty = max(l_fMaxPenalty, l_fPenalty);
         l_fAveragePenalty += l_fPenalty;
         l_iEdgeCount ++;

         if( (l_Vertex.m_iRegionId != 0) && (m_vRegionGraph[l_Vertex.m_pEdges[e].m_iDestID].m_iRegionId != 0) )
         {
//            g_Joshua.Log(GString("Edge from ") + GString(l_Vertex.m_iRegionId) + GString(" to ") + GString(l_Vertex.m_pEdges[e].m_iDestID) + GString(" has a penalty of : ") + GString(l_fPenalty) );

            l_fMaxPenaltyNoWater = max(l_fMaxPenaltyNoWater, l_fPenalty);
            l_fAveragePenaltyNoWater += l_fPenalty;
            l_iEdgeCountNoWater ++;
         }
      }
   }

   REAL32 l_fVariance = 0;
   REAL32 l_fVarianceNoWater = 0;

   for(UINT32 r = 0;r < l_iVertexCount;r ++)
   {
      SRegionVertex& l_Vertex = m_vRegionGraph[r];

      for(UINT32 e = 0;e < l_Vertex.m_iNbEdges;e ++)
      {
         REAL32 l_fPenalty = l_Vertex.m_pEdges[e].m_fDistance / g_ServerDAL.DistanceBetween2PointsLatLong(l_Vertex.m_Center, m_vRegionGraph[l_Vertex.m_pEdges[e].m_iDestID].m_Center);
         l_fVariance += (l_fPenalty - l_fAveragePenalty / (REAL32) l_iEdgeCount) * (l_fPenalty - l_fAveragePenalty / (REAL32) l_iEdgeCount);

         if( (l_Vertex.m_iRegionId != 0) && (m_vRegionGraph[l_Vertex.m_pEdges[e].m_iDestID].m_iRegionId != 0) )
         {
            l_fVarianceNoWater += (l_fPenalty - l_fAveragePenaltyNoWater / (REAL32) l_iEdgeCountNoWater) * (l_fPenalty - l_fAveragePenaltyNoWater / (REAL32) l_iEdgeCountNoWater);
         }
      }
   }

   l_fVariance = sqrtf(l_fVariance / (REAL32) l_iEdgeCount);
   l_fVarianceNoWater = sqrtf(l_fVarianceNoWater / (REAL32) l_iEdgeCountNoWater);

   g_Joshua.Log(GString("Maximum height penalty computed : ") + GString(l_fMaxPenalty) );
   g_Joshua.Log(GString("Average height penalty computed : ") + GString(l_fAveragePenalty / (REAL32) l_iEdgeCount) );
   g_Joshua.Log(GString("Standard deviation height penalty computed : ") + GString(l_fVariance) );

   g_Joshua.Log(GString("Maximum height penalty computed (no water) : ") + GString(l_fMaxPenaltyNoWater) );
   g_Joshua.Log(GString("Average height penalty computed (no water) : ") + GString(l_fAveragePenaltyNoWater / (REAL32) l_iEdgeCountNoWater) );
   g_Joshua.Log(GString("Standard deviation height penalty computed (no water) : ") + GString(l_fVarianceNoWater) );

   m_PathFinder.Initialize(m_vRegionGraph.size(), this);

   // Allocate memory for group localisation & movement
   m_pRegionGroups = new set<UINT32> [g_ServerDAL.Admin().NbFeatures() + 1];
   m_pCountryGroupsWithPath = NULL;

   // Allocate memory for visit arrays
   m_pWaypointTypes = new UINT8[m_vRegionGraph.size() ];
   m_pCountryTypes = NULL;

   // Register group deletion callback on the unit manager
   g_Joshua.UnitManager().RegistedDestroyGroupCallback( (CALLBACK_HANDLER_pGroup) &GUnitMover::OnDestroyGroup, this);

   // Init array that indicates which military statuses can be merged into
   Memory::Clear(m_pStatusCanBeMerged, EMilitaryStatus::CountTotal);
   m_pStatusCanBeMerged[EMilitaryStatus::Parked] = 1;
   m_pStatusCanBeMerged[EMilitaryStatus::Ready] = 1;
   m_pStatusCanBeMerged[EMilitaryStatus::Fortified] = 1;

   g_ServerDAL.DataManager()->RegisterNode(this);


   // Then read controlled waypoint (for suez and panama)
   GFile l_ControlledWaypointFile;
   if(g_Joshua.FileManager()->File(L"Data/Map/region.ctr", l_ControlledWaypointFile) )
   {

      // Read file
      if(l_ControlledWaypointFile.Open())
      {

         UINT32 l_iWaypointCount;
         l_ControlledWaypointFile.Read(&l_iWaypointCount, sizeof(UINT32) );

         for(UINT32 y=0;y<l_iWaypointCount;y++)
         {
            UINT32 l_iWaypointID;
            UINT32 l_iRegionControlID;
            l_ControlledWaypointFile.Read(&l_iWaypointID, sizeof(UINT32) );
            l_ControlledWaypointFile.Read(&l_iRegionControlID, sizeof(UINT32) );

            m_vControlledWayPoint.insert(make_pair(l_iWaypointID,l_iRegionControlID));
         }
      }
   }

   return true;
}


void GUnitMover::Shutdown()
{
   m_vRegionVertices.clear();

   for (unsigned int i = 0; i < m_vRegionGraph.size(); i++)
      delete m_vRegionGraph[i].m_pEdges;

   m_vRegionGraph.clear();
   m_vControlledWayPoint.clear();
   m_PathFinder.Shutdown();
   
   SAFE_DELETE_ARRAY(m_pRegionGroups);
   SAFE_DELETE_ARRAY(m_pCountryGroupsWithPath);
   SAFE_DELETE_ARRAY(m_pCountryTypes);
   SAFE_DELETE_ARRAY(m_pWaypointTypes);
}

//Create the production Queue Groups
void   GUnitMover::CreateProductionQueueUnitGroups()
{
   m_vProductionQueueGroups.resize(g_ServerDAL.NbCountry());
   for(UINT32 i = 0 ; i < m_vProductionQueueGroups.size() ; i++)
   {
      UINT32 l_iCountryID = i + 1;
      SP2::GUnitGroupEx* l_pUnitGroup = (SP2::GUnitGroupEx*)g_Joshua.UnitManager().CreateUnitGroup();

      l_pUnitGroup->Id(g_Joshua.UnitManager().NewUnitGroupID() );
		l_pUnitGroup->OwnerId(l_iCountryID);

      g_Joshua.UnitManager().AddUnitGroup(l_pUnitGroup);

      //Get the capital of the country
      g_Joshua.UnitManager().SetGroupPosition(l_pUnitGroup, g_ServerDAL.CapitalPosition(l_iCountryID) );
      l_pUnitGroup->ChangeStatus(EMilitaryStatus::ReadyToDeploy);

      m_vProductionQueueGroups[i] = l_pUnitGroup;
   }
}

void GUnitMover::CreateDeployementList(void)
{
   gassert(m_vGroupsInDeployement.size() == 0, "GUnitMover::CreateDeployementList, Memory leak");
   m_vGroupsInDeployement.resize(g_ServerDAL.NbCountry());  
}

void GUnitMover::DestroyDeployementList(void)
{
   m_vGroupsInDeployement.clear();
}

void GUnitMover::UpdateCapitalLocation(UINT32 in_iCountryID)
{
   if(m_vProductionQueueGroups[in_iCountryID - 1] )
   {
      g_Joshua.UnitManager().SetGroupPosition(m_vProductionQueueGroups[in_iCountryID - 1], 
                                             g_ServerDAL.CapitalPosition(in_iCountryID) );
   }
}

void GUnitMover::AddGroup(SDK::Combat::GUnitGroup* in_pGroup)
{
   LOG_MODIF( GString(L"AddGroup : ") + GString(in_pGroup->Id() ) );

   // Set the group region ID
   SP2::GUnitGroupEx* l_pGroupEx = (SP2::GUnitGroupEx*) in_pGroup;

   const Map::GMultiface* l_pMF = g_ServerDAL.CountryView()->Locate(in_pGroup->Position() );
   if(l_pMF)
   {
      l_pGroupEx->m_iRegionID = l_pMF->Id();
   }
   else
   {
      l_pGroupEx->m_iRegionID = 0;
   }
   l_pGroupEx->m_iMoveRegionID = 0;

   // Add the group to the region array
   m_pRegionGroups[l_pGroupEx->m_iRegionID].insert(in_pGroup->Id() );

}

void GUnitMover::UnitGroupsInsideRegion(UINT32 in_iRegionID, vector<UINT32>& out_vUnitGroups, ENTITY_ID in_iCountryID) const
{
	for(set<UINT32>::const_iterator l_Itr = m_pRegionGroups[in_iRegionID].begin();
		 l_Itr != m_pRegionGroups[in_iRegionID].end();
		 l_Itr++)
	{
		SDK::Combat::GUnitGroup* l_pUnitGroup = g_Joshua.UnitManager().UnitGroup(*l_Itr);
		if(!l_pUnitGroup)
			continue;
		EMilitaryStatus::Enum l_Status = ((SP2::GUnitGroup*)l_pUnitGroup)->Status();
		if(((in_iCountryID == 0 || l_pUnitGroup->OwnerId() == in_iCountryID)) &&
			(l_Status == EMilitaryStatus::Fortified ||
			 l_Status == EMilitaryStatus::Ready ||
			 l_Status == EMilitaryStatus::Moving ||
			 l_Status == EMilitaryStatus::Attacking ||
			 l_Status == EMilitaryStatus::Parked))
		{
			out_vUnitGroups.push_back(*l_Itr);
		}
	}
}

const set<UINT32>& GUnitMover::UnitGroupsInsideRegion(UINT32 in_iRegionID) const
{
   gassert(in_iRegionID <= g_ServerDAL.NbRegion(), "Invalid region ID for unit group listing");

   return m_pRegionGroups[in_iRegionID];
}

void GUnitMover::UpdateRegionMilitaryControl(UINT32 in_iRegionID, UINT32 in_iNewControl, UINT32 in_iPreviousControl)
{
   LOG_MODIF( GString(L"UpdateRegionMilitaryControl : ") + GString(in_iRegionID ) 
            + GString(L" ; ") + GString(in_iPreviousControl)
            + GString(L" -> ") + GString(in_iNewControl) );

   REAL32 l_fTime = (REAL32) g_Joshua.GameTime();
   //! \todo TBM : An optimization could be to associate each region with 
   //! groups with their destination

   // Test all groups that move
   for(UINT32 i = 1;i <= (UINT32) g_ServerDAL.NbCountry();i ++)
   {
      set<UINT32>& l_GroupWithPaths = m_pCountryGroupsWithPath[i].m_Groups;
      set<UINT32>::iterator l_GroupPathIt = l_GroupWithPaths.begin();
      while(l_GroupPathIt != l_GroupWithPaths.end() )
      {
         SP2::GUnitGroupEx* l_pGroup = (SP2::GUnitGroupEx*) g_Joshua.UnitManager().UnitGroup(*l_GroupPathIt);
         gassert(l_pGroup, "Problem synchronizing group paths");

         EDiplomaticStatus::Enum l_PrevRegionRelation = g_ServerDAL.DiplomaticStatus(l_pGroup->OwnerId(), in_iPreviousControl);
         EDiplomaticStatus::Enum l_CurRegionRelation = g_ServerDAL.DiplomaticStatus(l_pGroup->OwnerId(), in_iNewControl);

         // Test if this region control change modified the relations
         if(l_PrevRegionRelation != l_CurRegionRelation)
         {
            // Stop unit protection when unit is inside this region and it has become hostile
            if( (l_CurRegionRelation == EDiplomaticStatus::Hostile) && 
                (l_pGroup->m_iMoveRegionID == in_iRegionID) )
            {
               if(l_pGroup->Path().IsProtected(l_fTime) )
               {
                  l_pGroup->Path().UpdateProtection(l_fTime);
               }
            }
            else if( (l_CurRegionRelation == EDiplomaticStatus::Hostile) && 
                     (l_pGroup->m_iRegionID == in_iRegionID) )
            {
               //! \todo TBM : Place unit in "LIMBO", 
               // where it cannot be used until a decision is made 
               // for its destination
            }
            // Update unit protection when unit is not destinated to the region
            else if(l_pGroup->m_iRegionID != in_iRegionID)
            {
               //! \todo : update unit protection start point (if applicable)
            }
            // Cancel attack of region if move is destinated to a region and 
            // relation is no more hostile
            else if(l_CurRegionRelation != EDiplomaticStatus::Hostile)
            {
               if(l_CurRegionRelation == EDiplomaticStatus::Allied)
               {
                  if(l_pGroup->m_iMoveRegionID == in_iRegionID)
                  {
                     l_pGroup->Path().UpdateProtection(l_pGroup->Path().TimeEnd() );
                  }
                  else
                  {
                     //! \todo TBM : update unit protection start point (if applicable)
                     l_pGroup->Path().UpdateProtection(l_pGroup->Path().TimeEnd() );
                  }
               }
               else
               {
                  //! \todo TBM : Place unit in "LIMBO", 
                  // where it cannot be used until a decision is made 
                  // for its destination
                  l_pGroup->Path().UpdateProtection(l_pGroup->Path().TimeEnd() );
               }
            }
         }
         ++ l_GroupPathIt;
      }
   }
}

bool GUnitMover::Iterate()
{
   REAL32 l_fTime = (REAL32) g_Joshua.GameTime();

   vector<SP2::GUnitGroupEx*> l_vUnprotectedMoves;
   vector<SP2::GUnitGroupEx*> l_vProtectedMoves;
   hash_set<UINT32> l_vGroupsToMerge;

   // Update all groups that move
   for(UINT32 i = 1;i <= (UINT32) g_ServerDAL.NbCountry();i ++)
   {
      set<UINT32>& l_GroupWithPaths = m_pCountryGroupsWithPath[i].m_Groups;
      set<UINT32>::iterator l_GroupPathIt = l_GroupWithPaths.begin();
      while(l_GroupPathIt != l_GroupWithPaths.end() )
      {
         SP2::GUnitGroupEx* l_pGroup = (SP2::GUnitGroupEx*) g_Joshua.UnitManager().UnitGroup(*l_GroupPathIt);
         gassert(l_pGroup, "Problem synchronizing group paths");

         // Remove group from its current region
         m_pRegionGroups[l_pGroup->m_iMoveRegionID].erase(*l_GroupPathIt);
         ++ l_GroupPathIt;

         // Update prevlocation
         l_pGroup->m_iPrevMoveRegionID = l_pGroup->m_iMoveRegionID;

         // Update the current group path, if updatePath returns true, movement is over
         if(l_pGroup->UpdatePath(l_fTime) )
         {
            //If movement is complete.
            CancelMovement(l_pGroup);
            g_Joshua.UnitManager().ModifyUnitGroup(l_pGroup);

            // Merge with other group[s] at the final location
            if(l_pGroup->m_ActionAtDest & EMilitaryAction::MergeWith)
            {
               l_vGroupsToMerge.insert(l_pGroup->Id() );
            }
            //If when reach the destination, group must perform offshore bombardments
            else if(l_pGroup->m_ActionAtDest & EMilitaryAction::BombardRegion)
            {
               AddBombardingGroup(l_pGroup->Id());
            }
         }

         // Locate current location of unit
         UINT32 l_iRegionID = g_ServerDCL.EarthLocateRegion(l_pGroup->Position().x, l_pGroup->Position().y);
         l_pGroup->m_iMoveRegionID = l_iRegionID;

         // Add group from its current region
         m_pRegionGroups[l_pGroup->m_iMoveRegionID].insert(l_pGroup->Id() );

         if(!l_pGroup->Path().IsProtected(l_fTime) )
         {
            // Remember to test if unit must enter combat
            l_vUnprotectedMoves.push_back(l_pGroup);
         }
         else
         {
            l_vProtectedMoves.push_back(l_pGroup);
         }
      }
   }

   // Check each unprotected move for combat opportunities & region ownership change
   const REAL32 l_fCombatThresholdSquare = g_SP2Server->CombatThresholdSquare();

   for(UINT32 i = 0;i < l_vUnprotectedMoves.size();i ++)
   {
      m_pArena = NULL;
      SP2::GUnitGroupEx* l_pGroup = l_vUnprotectedMoves[i];
      UINT32 l_iControl = g_ServerDAL.RegionControl(l_pGroup->m_iMoveRegionID).m_iMilitary;
      const GRegionControl&  l_Prevcontrol = g_ServerDAL.RegionControl(l_pGroup->m_iPrevMoveRegionID);
      const GVector2D<REAL32>& l_GroupPos = l_pGroup->Position();

      // Compare current unit with all other moving units until a combat is created
      for(UINT32 j = i + 1;(j < l_vUnprotectedMoves.size() ) && (l_pGroup->Status() != EMilitaryStatus::Attacking);j ++)
      {
         SP2::GUnitGroupEx* l_pCompareGroup = l_vUnprotectedMoves[j];

         // Make sure group is not already in combat
         if(l_pCompareGroup->Status() != EMilitaryStatus::Attacking)
         {
            UINT32 l_iCompareControl = g_ServerDAL.RegionControl(l_pCompareGroup->m_iMoveRegionID).m_iMilitary;

            // Make sure units are in the same country
            if(l_iCompareControl == l_iControl)
            {
               // Make sure units are close enough to create a combat
               const GVector2D<REAL32>& l_CompareGroupPos = l_pCompareGroup->Position();
               if(l_GroupPos.DistanceSquared(l_CompareGroupPos) <= l_fCombatThresholdSquare)
               {
                  // Make sure groups are hostile each other
                  if(g_ServerDAL.DiplomaticStatus(l_pGroup->OwnerId(), l_pCompareGroup->OwnerId() ) == EDiplomaticStatus::Hostile)
                  {
                     CreateTwoSidedCombat(l_pGroup, l_pCompareGroup);
                  }
               }
            }
         }
      }

      // Compare current unit with all units in the same country until a combat is created
      const set<UINT32>& l_vRegions  = g_ServerDAL.CountryMilitaryControl(l_iControl);

      // Test all regions of the country
		for(set<UINT32>::const_iterator l_Itr = l_vRegions.begin();
			 (l_Itr != l_vRegions.end() ) && (l_pGroup->Status() != EMilitaryStatus::Attacking);
			 l_Itr++)
      {
         UINT32 l_iRegionID = *l_Itr;
         set<UINT32>& l_RegionGroups = m_pRegionGroups[l_iRegionID];

         // Test all units of the region
         set<UINT32>::iterator l_It = l_RegionGroups.begin();
         while((l_It != l_RegionGroups.end() ) && (l_pGroup->Status() != EMilitaryStatus::Attacking))
         {
            SP2::GUnitGroupEx* l_pCompareGroup = (SP2::GUnitGroupEx*) g_Joshua.UnitManager().UnitGroup(*l_It);
            gassert(l_pCompareGroup, "Sync problem in unit mover regions");

            // group is really valid ?
            if(l_pCompareGroup)
            {
               // Make sure units are close enough to creatpe a combat
               const GVector2D<REAL32>& l_CompareGroupPos = l_pCompareGroup->Position();
               if( (l_pCompareGroup != l_pGroup) && 
                  (l_GroupPos.DistanceSquared(l_CompareGroupPos) <= l_fCombatThresholdSquare) )
               {
                  // Make sure groups are hostile each other
                  if(g_ServerDAL.DiplomaticStatus(l_pGroup->OwnerId(), l_pCompareGroup->OwnerId() ) == EDiplomaticStatus::Hostile)
                  {
                     // Test if the test group is already in combat
                     if(l_pCompareGroup->Status() == EMilitaryStatus::Attacking)
                     {
                        // A combat already exist, join it
                        m_pArena = g_CombatManager.Arena(l_pCompareGroup->ArenaInfo()->m_iId);
                        JoinCombat(l_pGroup);
                        m_pArena = NULL;
                     }
                     else if(l_pCompareGroup->Status() < EMilitaryStatus::CanJoinCombat )
                     {
                        // Create the combat arena
                        CreateTwoSidedCombat(l_pGroup, l_pCompareGroup);
                     }
                  }
               }
               if(l_pGroup->Status() != EMilitaryStatus::Attacking)
               {
                  ++ l_It;
               }
            }
            // group referenced in the region isnt valid, remove it
            else
            {       
               // log error ...
               g_Joshua.Log(GString("Invalid unit in region [") + GString(l_iRegionID) + 
                            GString("] with id [")              + GString(*l_It)       + 
                            GString("]. Removing ..."), MSGTYPE_WARNING);
               // remove it
               set<UINT32>::iterator l_ItToRemove = l_It++;
               l_RegionGroups.erase(l_ItToRemove);
            }
         }
      }

      // Test if group movement generates a region owner change
      if( (l_pGroup->Status() != EMilitaryStatus::Attacking) && (l_iControl != 0) && (l_iControl != l_pGroup->OwnerId() ) )
      {
         g_ServerDCL.UpdateMilitaryControl(l_pGroup->m_iMoveRegionID, l_pGroup->OwnerId() );
      }

      // Test if leaving a region generates a region owner change
      if(l_pGroup->m_iMoveRegionID != l_pGroup->m_iPrevMoveRegionID)
      {
         if(l_Prevcontrol.m_iPolitical != l_Prevcontrol.m_iMilitary)
         {
            g_ServerDCL.UpdateMilitaryControl(l_pGroup->m_iPrevMoveRegionID, l_Prevcontrol.m_iPolitical);
         }
      }

      // When a combat is created, join all nearby units into it
      if(m_pArena)
      {
         const GVector2D<REAL32>& l_ArenaPos = ( (SP2::GArenaInfo*)m_pArena->Info() )->m_Coordinate;

         gassert(l_pGroup->Status() == EMilitaryStatus::Attacking, "Unit group should be attacking");

         // Compare current combat with all other moving units and join units to the combat if needed
         for(UINT32 j = i + 1;j < l_vUnprotectedMoves.size();j ++)
         {
            SP2::GUnitGroupEx* l_pCompareGroup = l_vUnprotectedMoves[j];

            // Make sure group is not already in combat
            if(l_pCompareGroup->Status() != EMilitaryStatus::Attacking)
            {
               UINT32 l_iCompareControl = g_ServerDAL.RegionControl(l_pCompareGroup->m_iMoveRegionID).m_iMilitary;

               // Make sure units are in the same country as the combat
               if(l_iCompareControl == l_iControl)
               {
                  // Make sure units are close enough to create a combat
                  const GVector2D<REAL32>& l_CompareGroupPos = l_pCompareGroup->Position();
                  if(l_ArenaPos.DistanceSquared(l_CompareGroupPos) <= l_fCombatThresholdSquare)
                  {
                     // Make sure groups are hostile each other
                     if(g_ServerDAL.DiplomaticStatus(l_pGroup->OwnerId(), l_pCompareGroup->OwnerId() ) == EDiplomaticStatus::Hostile)
                     {
                        JoinCombat(l_pCompareGroup);
                     }
                  }
               }
            }
         }

         // Test all regions of the country
         vector<SP2::GUnitGroupEx*> l_vUnitsThatHaveToJoinTheCombat;
		   for(set<UINT32>::const_iterator l_Itr = l_vRegions.begin();
			    l_Itr != l_vRegions.end();
			    l_Itr++)
         {
            UINT32 l_iRegionID = *l_Itr;
            set<UINT32>& l_RegionGroups = m_pRegionGroups[l_iRegionID];

            // Test all units of the region
            set<UINT32>::iterator l_It = l_RegionGroups.begin();
            while(l_It != l_RegionGroups.end() )
            {
               SP2::GUnitGroupEx* l_pCompareGroup = (SP2::GUnitGroupEx*) g_Joshua.UnitManager().UnitGroup(*l_It);
               gassert(l_pCompareGroup, "Sync problem in unit mover regions");

               // Make sure units are close enough to join a combat
               const GVector2D<REAL32>& l_CompareGroupPos = l_pCompareGroup->Position();
               if( (l_pCompareGroup->Status() < EMilitaryStatus::CanJoinCombat) && 
                   (l_ArenaPos.DistanceSquared(l_CompareGroupPos) <= l_fCombatThresholdSquare) )
               {
                  // Make sure groups are hostile each other
                  if(g_ServerDAL.DiplomaticStatus(l_pGroup->OwnerId(), l_pCompareGroup->OwnerId() ) == EDiplomaticStatus::Hostile)
                  {
                     l_vUnitsThatHaveToJoinTheCombat.push_back(l_pCompareGroup);
                  }
               }
               ++ l_It;
            }
         }

         for(UINT32 i = 0;i < l_vUnitsThatHaveToJoinTheCombat.size();i ++)
         {
            JoinCombat(l_vUnitsThatHaveToJoinTheCombat[i] );
         }
      }
   }

   // Check each protected move for region ownership change
   for(UINT32 i = 0;i < l_vProtectedMoves.size();i ++)
   {
      SP2::GUnitGroupEx* l_pGroup = l_vProtectedMoves[i];
      const GRegionControl&  l_Prevcontrol = g_ServerDAL.RegionControl(l_pGroup->m_iPrevMoveRegionID);

      // Test if leaving a region generates a region owner change
      if(l_pGroup->m_iMoveRegionID != l_pGroup->m_iPrevMoveRegionID)
      {
         if(l_Prevcontrol.m_iPolitical != l_Prevcontrol.m_iMilitary)
         {
            g_ServerDCL.UpdateMilitaryControl(l_pGroup->m_iPrevMoveRegionID, l_Prevcontrol.m_iPolitical);
         }
      }
   }

   // Test all groups that currently change status
   list<UINT32>::iterator l_It = m_GroupsChangingStatus.begin();
   while(l_It != m_GroupsChangingStatus.end() )
   {
      UINT32 l_iGroupID = *l_It;
      list<UINT32>::iterator l_CurrentIt = l_It;
      ++ l_It;
      SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*) g_Joshua.UnitManager().UnitGroup(l_iGroupID);
      gassert(l_pGroup, "Iterating a non existing group");

		if(l_pGroup->Status() != EMilitaryStatus::Attacking)
		{
			hash_set<UINT32>::iterator l_FortifyIt = m_GroupsToFortify.find(l_iGroupID);
			hash_map<UINT32, GGroupMove>::iterator l_MoveIt = m_GroupsToMove.find(l_iGroupID);

			// Check if status change is over
			if(l_pGroup->StatusChangeEndTime() < l_fTime)
			{
				LOG_MODIF("Unit " +
							GString(l_iGroupID) +
							" completed its status change to " + 
							GString( (INT32) l_pGroup->NextStatus() ) );

				// Complete status change by making current status equal to next status
				l_pGroup->ChangeStatus(l_pGroup->NextStatus() );


				// Remove this element from the list
				m_GroupsChangingStatus.erase(l_CurrentIt);

				// Test if another action must be taken for that group

				// Check if group must also change status to fortified
				if(l_FortifyIt != m_GroupsToFortify.end() )
				{
					gassert(l_pGroup->Status() == EMilitaryStatus::Ready, "Incoherence between unit mover & unit group #0");
					gassert(l_MoveIt == m_GroupsToMove.end(), "Unit wants to fortify & move at the same time!");

					m_GroupsToFortify.erase(l_FortifyIt);

					l_pGroup->ChangeStatus(EMilitaryStatus::Fortified, l_fTime, c_fDelayStatusReadyToFortified);
					m_GroupsChangingStatus.push_back(l_iGroupID);
					g_Joshua.UnitManager().ModifyUnitGroup(l_pGroup);
				}
				// Check if group must begin a movement
				else if(l_MoveIt != m_GroupsToMove.end() )
				{
					// Unit must be in status "moving" to begin its movement
					if(l_pGroup->Status() == EMilitaryStatus::Moving)
					{
						ChangeGroupPath( (SP2::GUnitGroupEx*) l_pGroup, l_MoveIt->second, l_fTime);
						m_GroupsToMove.erase(l_MoveIt);
					}
					else
					{
//						gassert(l_pGroup->Status() == EMilitaryStatus::Ready, "Unit must be ready before it can change to moving status");
						ChangeToMovingStatus( (SP2::GUnitGroupEx*) l_pGroup, l_fTime);
					}
				}
			}
      }
		else
		{
			// Cancel status change
			m_GroupsChangingStatus.erase(l_CurrentIt);
			m_GroupsToFortify.erase(l_iGroupID);
			m_GroupsToMove.erase(l_iGroupID);
		}
   }

   // Merge groups
   hash_set<UINT32>::iterator l_GroupItr;
   for(l_GroupItr = l_vGroupsToMerge.begin(); l_GroupItr != l_vGroupsToMerge.end(); l_GroupItr++)
   {
      SP2::GUnitGroupEx* l_pGroup = (SP2::GUnitGroupEx*) g_Joshua.UnitManager().UnitGroup(*l_GroupItr);
      if( (l_pGroup) && (m_pStatusCanBeMerged[l_pGroup->Status() ] ) )
      {
         const set<UINT32>& l_CountryUnitGroups = g_Joshua.UnitManager().CountryUnitGroups(l_pGroup->OwnerId() );
         set<UINT32>::const_iterator itr;
         for (itr = l_CountryUnitGroups.begin(); itr != l_CountryUnitGroups.end(); ++ itr)
         {
            SP2::GUnitGroupEx* l_DestGroup = (SP2::GUnitGroupEx*) g_Joshua.UnitManager().UnitGroup(*itr);
            if ( (l_DestGroup != l_pGroup) && (m_pStatusCanBeMerged[l_DestGroup->Status() ] ) )
            {
               if (l_DestGroup->Position().DistanceSquared(l_pGroup->Position() ) < c_fMergeThresSquared)
               {
                  if(l_DestGroup->IsNaval() == l_pGroup->IsNaval())
                  {
                     UINT32 l_iGroupId = l_pGroup->Id();

                     // Remember each unit design in the destination group
                     multimap<UINT32, UINT32> l_DesignMap;

                     for(UINT32 i = 0;i < l_DestGroup->Units().size();i ++)
                     {
                        l_DesignMap.insert(make_pair(l_DestGroup->Units() [i]->Design()->Id(), i) );
                     }

                     // Merge each unit of the source group into the destination group
                     while (!l_pGroup->Units().empty() )
                     {
                        SP2::GUnit* l_pUnit = (SP2::GUnit*) l_pGroup->Units().front();
                        gassert(l_pUnit, "Invalid unit in a group while merging");

                        // Add the group, but it could be destroyed later
                        g_Joshua.UnitManager().AddUnit2Group(l_pUnit, l_DestGroup);

                        // try to find the unit design and training level in the destination group
                        UINT32 l_iDesignID = l_pUnit->Design()->Id();
                        multimap<UINT32, UINT32>::iterator l_It = l_DesignMap.find(l_iDesignID);
                        if(l_It != l_DesignMap.end() )
                        {
                           // at least one same design is present, 
                           // find a unit that can merge with it in the destination
                           // group
                           while( (l_It != l_DesignMap.end() ) && (l_It->first == l_iDesignID) && (l_pUnit != NULL) )
                           {
                              SP2::GUnit* l_pDestUnit = (SP2::GUnit*) l_DestGroup->Units() [l_It->second];
                              if(l_pDestUnit->CanMergeWith(l_pUnit) )
                              {
                                 l_pDestUnit->MergeWithUnit(l_pUnit);
                                 g_Joshua.UnitManager().KillUnit(l_pUnit->Id() );
                                 l_pUnit = NULL;
                              }
                              ++ l_It;
                           }
                        }
                     }
                     LOG_MODIF("Merged group : " + GString(l_iGroupId) + L" into group: " + GString(l_DestGroup->Id()));

                     g_Joshua.UnitManager().RemoveUnitGroup(l_iGroupId);
                     break;
                  }
               }
            }
         }
      }
   }

   // Iterate over units that are in a deployement status
   IterateGroupsInDeployement();

   // Iterate over units that are in training
   IterateUnitsInTraining();

   return true;
}


void GUnitMover::IterateUnitsInTraining(void)
{
   list<GUnitInTraining>::iterator l_UnitIterator = m_ListOfUnitsInTraining.begin();
   while(l_UnitIterator != m_ListOfUnitsInTraining.end())
   {
      // Are we done with training
      if((*l_UnitIterator).m_fTrainingCompletionTime < g_Joshua.GameTime())
      {
         // Updagre training level
         SP2::GUnit *l_pUnit = (SP2::GUnit *)g_Joshua.UnitManager().Unit((*l_UnitIterator).m_iUnitId);

         // Tag the prodsuction group as modified so the client update the training status of the units.
         g_Joshua.UnitManager().ModifyUnitGroup(l_pUnit->Group() );

         //Already too much trained
         if(l_pUnit->Training() >= ETrainingLevel::Elite)
         {
         }
         else
         {
            l_pUnit->Training((ETrainingLevel::Enum)(l_pUnit->Training()+1));
            g_Joshua.UnitManager().ModifyUnit(l_pUnit);

            TryDeployGroupMerge(l_pUnit);
         }

         // Remove it from the list
        l_UnitIterator =  m_ListOfUnitsInTraining.erase(l_UnitIterator);
      }
      else
      {
         l_UnitIterator++;
      }
   }
}

void GUnitMover::ClearDeploymentListForCountry(UINT32 in_iCountry)
{
	list<GUnitGroupInDeployement>* l_pListOfGroupInDeployement = &(m_vGroupsInDeployement[in_iCountry-1] );
	set<UINT32> l_GroupIDToCancel;
	list<GUnitGroupInDeployement>::iterator l_groupIterator = l_pListOfGroupInDeployement->begin();
   while(l_groupIterator != l_pListOfGroupInDeployement->end())
   {   
      GUnitGroupInDeployement  l_GroupInDepoloyement = *l_groupIterator;
		l_GroupIDToCancel.insert(l_GroupInDepoloyement.m_pGroupIDToDeploy);		
		l_groupIterator++;
	}
	for(set<UINT32>::iterator it = l_GroupIDToCancel.begin(); 
		 it != l_GroupIDToCancel.end(); it++)
	{
		CancelDeployement( (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup( *it ));
	}

   m_vProductionQueueGroups[in_iCountry - 1] = NULL;
}

void GUnitMover::IterateGroupsInDeployement()
{
   // Update all groups that are currently deploying.
   for(UINT32 i = 0;i < m_vGroupsInDeployement.size();i ++)
   {
      list<GUnitGroupInDeployement>* l_pListOfGroupInDeployement = &(m_vGroupsInDeployement[i] );
      list<GUnitGroupInDeployement>::iterator l_groupIterator = l_pListOfGroupInDeployement->begin();
      while(l_groupIterator != l_pListOfGroupInDeployement->end())
      {   
         GUnitGroupInDeployement  l_GroupInDepoloyement= *l_groupIterator;
      
         // When deployement time is smaller than current game time, its time to deploy.
         if(l_GroupInDepoloyement.m_fDeployementTime < g_Joshua.GameTime())
         {
            // Deploy units.
            bool l_bGroupDeployed;
            SP2::GUnitGroup *l_pGroupToDeploy = (SP2::GUnitGroup *)g_Joshua.UnitManager().UnitGroup(l_GroupInDepoloyement.m_pGroupIDToDeploy);
            if(l_pGroupToDeploy == NULL)
            {
               l_bGroupDeployed = true; 
               gassert(false,"Somehow this group doesnt exist anymore");
            }
            else
               l_bGroupDeployed = FinishDeployingGroup(l_pGroupToDeploy);

				if(l_bGroupDeployed)
				{
	            // Remove it from this list
					l_pListOfGroupInDeployement->erase(l_groupIterator);
				}


            // JMercier TODO !
            // keep iteratig
            break;
         }
         
         l_groupIterator++;
      }
   }
}

bool GUnitMover::ChangeUnitState(SDK::Combat::GUnitGroup* in_pUnitGroup,
                                 EMilitaryStatus::Enum in_eNewStatus,
                                 bool in_bMoveAfter)
{
   UINT32 l_iGroupID = in_pUnitGroup->Id();
   SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*) in_pUnitGroup;

   // A unit cannot change status while in movement
   if(m_pCountryGroupsWithPath[in_pUnitGroup->OwnerId() ].m_Groups.count(in_pUnitGroup->Id() ) )
   {
      LOG_MODIF("Unit cannot change status while moving : " + GString(l_iGroupID) );
      return false;
   }

   LOG_MODIF("Changing unit " +
             GString(l_iGroupID) +
             " status from " + 
             GString( (INT32) l_pGroup->Status() ) + 
             " to " +
             GString( (INT32) in_eNewStatus) );

   if(l_pGroup->Status() > EMilitaryStatus::Moving)
   {
      return false;
   }

   if(!in_bMoveAfter)
   {
      // If unit was asked to move after status change, cancel it
      CancelMovement( (SP2::GUnitGroupEx*) in_pUnitGroup);
      g_Joshua.UnitManager().ModifyUnitGroup(in_pUnitGroup);
   }

   //If unit group is actually bombarding and going to a new status 
   // that is incompatible with bombardments, cancel any actual bombardment
   SP2::GUnitGroupEx* l_pUnitGroupEx = (SP2::GUnitGroupEx*)in_pUnitGroup;
   if(l_pUnitGroupEx->m_ActionAtDest & EMilitaryAction::BombardRegion)
   {
      switch(in_eNewStatus)
      {
         case EMilitaryStatus::Parked:
         case EMilitaryStatus::Fortified:
         case EMilitaryStatus::ForSale:
         case EMilitaryStatus::Training:
         case EMilitaryStatus::ReadyToDeploy:
         case EMilitaryStatus::InDeployement:
            RemoveBombardingGroup(l_pGroup->Id());
            l_pUnitGroupEx->RegionToBombard(0);
            l_pUnitGroupEx->m_ActionAtDest &= ~EMilitaryAction::BombardRegion;
            g_Joshua.UnitManager().ModifyUnitGroup(l_pUnitGroupEx);
            break;
         default:
            break;

      }
   }



   // Find if unit is currently changing status
   list<UINT32>::iterator l_It = 
      find(m_GroupsChangingStatus.begin(), m_GroupsChangingStatus.end(), l_iGroupID);

   if(l_It != m_GroupsChangingStatus.end() )
   {
      // The unit has a status change in progress
      gassert(l_pGroup->NextStatus() != l_pGroup->Status(), "Incoherence between unit mover & unit group #1");

      bool l_bAlreadyOk = false;

      // Stop unit status change if unit is asked to go to previous status
      if(in_eNewStatus == l_pGroup->Status() )
      {
         l_pGroup->ChangeStatus(in_eNewStatus);
         m_GroupsChangingStatus.erase(l_It);
         g_Joshua.UnitManager().ModifyUnitGroup(l_pGroup);

         l_bAlreadyOk  = true;
      }

      // Just make sure unit is not updated if status change is same as current
      if(in_eNewStatus == l_pGroup->NextStatus() )
      {
         l_bAlreadyOk = true;
      }


      if(l_bAlreadyOk)
      {
         // Test if unit was asked to change to fortified status
         hash_set<UINT32>::iterator l_FortifyIt = m_GroupsToFortify.find(l_iGroupID);
         if(l_FortifyIt != m_GroupsToFortify.end() )
         {
            // Just remove the need to change to fortify at current status change completion
            m_GroupsToFortify.erase(l_FortifyIt);
         }

         return true;
      }
   }
   else
   {
      // The unit does not currently change status
      gassert(l_pGroup->NextStatus() == l_pGroup->Status(), "Incoherence between unit mover & unit group #2");

      // Just make sure unit is not asked to go to its present status
      if(in_eNewStatus == l_pGroup->Status() )
      {
         return true;
      }
   }

   static EHistoryMarkerType::Enum s_eStatusToMarker[] = 
   {
      EHistoryMarkerType::ParkUnit,
      EHistoryMarkerType::ReadyUnit,
      EHistoryMarkerType::FortifyUnit,
   };

   EMilitaryStatus::Enum l_eNextStatus = in_eNewStatus;

   // If the unit goes from parked to fortified, it will be first readied, 
   // then it will be fortified
   if( (l_pGroup->Status() == EMilitaryStatus::Parked) && (in_eNewStatus == EMilitaryStatus::Fortified) )
   {
      // Test if unit was already asked to change to fortified status
      if(m_GroupsToFortify.find(l_iGroupID) != m_GroupsToFortify.end() )
      {
         gassert(l_pGroup->NextStatus() == EMilitaryStatus::Ready, "Incoherence between unit mover & unit group #3");
         return true;
      }

      l_eNextStatus = EMilitaryStatus::Ready;

      m_GroupsToFortify.insert(l_iGroupID);

      // Test if unit is already changing to ready
      if(l_pGroup->NextStatus() == EMilitaryStatus::Ready)
      {
         return true;
      }
   }

   // Compute the time needed to change from the current status
   REAL32 l_fTimeForStatusChange = 0.f;
   if(l_eNextStatus == EMilitaryStatus::Parked)
   {
      l_fTimeForStatusChange = c_fDelayStatusAnyToParked;
   }
   else if(l_eNextStatus == EMilitaryStatus::Fortified)
   {
      // The current status is necessarily Ready, since a parked unit
      l_fTimeForStatusChange = c_fDelayStatusReadyToFortified;
   }
   else
   {
      if(l_pGroup->Status() == EMilitaryStatus::Parked)
      {
			if(g_ServerDAL.CountryData(l_pGroup->OwnerId())->GvtType() == EGovernmentType::MilitaryDictatorship)
				l_fTimeForStatusChange = c_fDelayStatusParkedToReady * SP2::c_fGvtTypeMilitaryChangeStatusBonus;
			else
				l_fTimeForStatusChange = c_fDelayStatusParkedToReady;
      }
   }

   // Update the group status
   if(l_fTimeForStatusChange > 0)
   {
      REAL32 l_fCurrentTime = (REAL32) g_Joshua.GameTime();
      l_pGroup->ChangeStatus(l_eNextStatus, l_fCurrentTime, l_fTimeForStatusChange);
      g_Joshua.UnitManager().ModifyUnitGroup(l_pGroup);

      if(l_It == m_GroupsChangingStatus.end() )
      {
         m_GroupsChangingStatus.push_back(in_pUnitGroup->Id() );
      }
   }
   else
   {
      l_pGroup->ChangeStatus(l_eNextStatus);
      g_Joshua.UnitManager().ModifyUnitGroup(l_pGroup);

      if(l_It != m_GroupsChangingStatus.end() )
      {
         m_GroupsChangingStatus.erase(l_It);
      }
   }

   g_ServerDAL.AddHistoricalMarker(in_pUnitGroup->OwnerId(), 
                                   s_eStatusToMarker[in_eNewStatus], 
                                   (REAL32) l_pGroup->Value() );

   return true;
}

bool GUnitMover::MoveUnits(const vector<SDK::Combat::GUnitGroup* >& in_vUnitGroups,
                           const GVector2D<REAL32>& in_Dest,
                           EMilitaryAction::Flags in_Action)
{
   UINT32 l_iOwner = in_vUnitGroups[0]->OwnerId();

#ifdef GOLEM_DEBUG
   // Verify that owning country is same for all unit groups in the list
   for(UINT32 i = 1;i < in_vUnitGroups.size();i ++)
   {
      gassert(in_vUnitGroups[i]->OwnerId() == l_iOwner, 
              "Trying to move a group of units not of the same owner");
   }
#endif

   // Update graph connectivity with treaties information
   memset(m_pCountryTypes, ERegionType::Blocked, (g_ServerDAL.NbCountry() + 1) * sizeof(UINT8) );
   memset(m_pWaypointTypes, ERegionType::Blocked, m_vRegionGraph.size() * sizeof(UINT8) );

   // Search treaties for the list of current unit owner allies
   UpdateAlliedCountry(l_iOwner, ERegionType::Allied, true);
   m_pRefusedMoveEventSPtr->m_iSource   = SDK::Event::ESpecialTargets::Server;
   m_pRefusedMoveEventSPtr->m_iTarget = g_Joshua.ActivePlayerByModID(l_iOwner)->Id();

   // Find targeted country
   UINT32 l_iTargetID = 0;
   UINT32 l_iTargetRegionID = 0;
   const Map::GMultiface* l_pMF = g_ServerDAL.CountryView()->Locate(in_Dest.x, in_Dest.y);
   if(l_pMF)
   {
      l_iTargetRegionID = l_pMF->Id();
      l_iTargetID = g_ServerDAL.RegionControl(l_iTargetRegionID).m_iMilitary;
   }

   // A unit cannot attack in its own territory
   if( (l_iTargetID == l_iOwner) && (in_Action & EMilitaryAction::Attack) )
   {
      in_Action &= ~EMilitaryAction::Attack;
   }

   // If unit is attacking, declare war to the target country
   if(in_Action & EMilitaryAction::Attack)
   {
      if(l_iTargetID != 0)
      {
			set<UINT32> l_Attackers;
			l_Attackers.insert(l_iOwner);
			g_ServerDCL.DeclareNewWar(l_Attackers, l_iOwner, l_iTargetID);
      }
      else
      {
         //Movement is in the water, check for close units and declare war to all
         //Neutral units that are close to that.
         g_ServerDCL.DeclareWarToNeutralUnitsWithinRange(l_iOwner,in_Dest,g_SP2Server->CombatThresholdSquare());
         
      }
   }


   if(  (l_iTargetID != 0) &&
		  (g_ServerDAL.DiplomaticStatus(l_iOwner, l_iTargetID) == EDiplomaticStatus::Neutral) &&
       !(in_Action & EMilitaryAction::Attack) &&
        (l_iTargetID != l_iOwner) )
   {
      // Unit is not attacking, but the detination cannot accept the unit in their territory
      LOG_PATH(L"Unit cant move to neutral territory");

      // Fill a game event to indicate why the unit cannot move
      for(UINT32 i = 0;i < in_vUnitGroups.size();i ++)
      {
         m_pRefusedMoveEvent->m_vRefusedMoves.push_back(make_pair(in_vUnitGroups[i]->Id(), ERefusedMoveReason::NoMilitarySupport) );
      }
   }
   else
   {
      if(l_iTargetID != l_iOwner && l_iTargetID != 0)
      {
         // If unit is attacking, remove all allies to the target from the possible
         // path
         if(g_ServerDAL.DiplomaticStatus(l_iOwner, l_iTargetID) == EDiplomaticStatus::Hostile)
         {
            // Make sure it exist
            if(l_iTargetID > 0)
            {
               UpdateAlliedCountry(l_iTargetID, ERegionType::Blocked, false);
            }
         }
      }

      // Connect water waypoints
      const bool c_bNavalRuleEnabled = g_SP2Server->NavalRuleEnabled();

      if(!c_bNavalRuleEnabled)
      {
            m_pCountryTypes[0] = ERegionType::Water;
      }
      else
      {
          //MultiMOD
          //first, if country is NOT at war, allow movement to allies/own/tresspassing regions
          if (!g_ServerDAL.IsAtWar(l_iOwner))
          {
              if (l_iOwner == l_iTargetID)
              {
                  m_pCountryTypes[0] = ERegionType::Water;
              }

              if (g_ServerDAL.DiplomaticStatus(l_iOwner, l_iTargetID) == EDiplomaticStatus::Allied ||
                  g_ServerDAL.DiplomaticStatus(l_iOwner, l_iTargetID) == EDiplomaticStatus::MilitaryAccess)
              {
                  m_pCountryTypes[0] = ERegionType::Water;
              }
          }


          if (g_ServerDAL.IsAtWar(l_iOwner))
          {
              bool NoNavalGroups = true;
              const set<UINT32>& l_UnitGroupsId = g_Joshua.UnitManager().CountryUnitGroups(l_iOwner);
              for (set<UINT32>::const_iterator  l_UnitGroupIt = l_UnitGroupsId.begin();
                  l_UnitGroupIt != l_UnitGroupsId.end();
                  l_UnitGroupIt++)
              {

                  //check if country has naval units that can bombard 
                  SP2::GUnitGroup* l_pGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_UnitGroupIt);


                  if(l_pGroup->CanPerformOffShoreBombardment())
                  {	
                      //Calculate distance between navalgroup & enemy navalgroups. If closest is further than 750km, allow
                      UINT32 l_iNbCountries = (UINT32)g_ServerDAL.NbCountry();
                      for(UINT32 i9=1; i9 <= l_iNbCountries; i9++)
                      {
                          if(g_ServerDAL.DiplomaticStatus(i9,l_iOwner) == EDiplomaticStatus::Hostile)
                          {


                              const set<UINT32>& l_EnemyId = g_Joshua.UnitManager().CountryUnitGroups(i9);
                              for (set<UINT32>::const_iterator  l_EnemyNavalGroupIt = l_EnemyId.begin();
                                  l_EnemyNavalGroupIt != l_EnemyId.end();
                                  l_EnemyNavalGroupIt++)
                              {
                                  //check if enemy country has naval units
                                  SP2::GUnitGroup* l_pEnemyNavalGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().UnitGroup(*l_EnemyNavalGroupIt);

                                  if(l_pEnemyNavalGroup->IsNaval())
                                  {
                                      NoNavalGroups = false;
                                      //REAL32 l_fEnemyDistance = 0;
                                      REAL32 l_fEnemyDistance = g_ServerDAL.DistanceBetween2PointsLatLong(l_pGroup->Position(),l_pEnemyNavalGroup->Position());
                                      if (l_fEnemyDistance > 1500) 
                                      {

                                          //Calculate distance between navalgroup & targetregion. If less than 750km & not in battle, allow
                                          //REAL32 l_fDistance = 0;
                                          REAL32 l_fDistance = g_ServerDAL.DistanceBetween2PointsLatLong(l_pGroup->Position(),RegionLocation(l_iTargetRegionID));
                                          if (l_fDistance <= 750)
                                          { 
                                              //Calculate the worth of the units here that can bombard

                                              REAL64 CapitalWorth = 0;
                                              for(vector<SDK::Combat::GUnit*>::const_iterator l_It2 = l_pGroup->Units().begin();
                                                  l_It2 != l_pGroup->Units().end();
                                                  l_It2++)
                                              {
                                                  SDK::Combat::GUnit* UnitToCheck = *l_It2;
                                                  GUnitDesign* UnitDesignToCheck = (SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign((*l_It2)->Design()->Id());
                                                  if (UnitDesignToCheck->Type()->Id() == EUnitType::AircraftCarrier ||
                                                      UnitDesignToCheck->Type()->Id() == EUnitType::Destroyer)
                                                  {	
                                                      CapitalWorth += UnitToCheck->Design()->Cost();
                                                  }
                                              }



                                              //Now we'll go through the units being moved one by one, calculate their cost
                                              //add it to the total cost thus far. If it's less than CapitalWorth times three
                                              //allow movement.
                                              REAL64 UnitsToMoveCost = 0;
                                              for(UINT32 g = 0;g < in_vUnitGroups.size();g ++)
                                              {
                                                  SP2::GUnitGroupEx* l_pUnitGroupToMove = (SP2::GUnitGroupEx*)in_vUnitGroups[g];
                                                  UnitsToMoveCost += l_pUnitGroupToMove->Value();

                                                  if (UnitsToMoveCost <= CapitalWorth)
                                                  {
                                                      m_pCountryTypes[0] = ERegionType::Water;
                                                  }
                                              }
                                          }
                                      }
                                  }
                              }
                              if (NoNavalGroups)
                              {
                                  REAL32 Distance = g_ServerDAL.DistanceBetween2PointsLatLong(l_pGroup->Position(),RegionLocation(l_iTargetRegionID));
                                  if (Distance <= 750)
                                  { 
                                      m_pCountryTypes[0] = ERegionType::Water;
                                  }
                              }						
                          }
                      }
                  }
              }
          }
          //MultiMOD
          //allow the naval units access to water in all cases
          for(UINT32 g = 0;g < in_vUnitGroups.size();g ++)
          {
              SP2::GUnitGroup* l_pGroupToMove = (SP2::GUnitGroup*)in_vUnitGroups[g];
              if (l_pGroupToMove->IsNaval())
              {
                  m_pCountryTypes[0] = ERegionType::Water;
              }
          }	
      }


      // Make sure unit will be able to move through its owner country
      m_pCountryTypes[l_iOwner] = ERegionType::Owned;

      // Make sure unit will be able to move to its destination
      // But don't allow moving to Antarctica, which would be target ID 0 and region ID 111, if Naval Rule is enabled
      if((l_iTargetID != 0 || l_iTargetRegionID != 111 || !c_bNavalRuleEnabled) &&
         m_pCountryTypes[l_iTargetID] == ERegionType::Blocked)
      {
         m_pCountryTypes[l_iTargetID] = ERegionType::Enemy;
      }

      // Update waypoint connectivity
      for(INT32 c = 0;c <= g_ServerDAL.NbCountry();c ++)
      {
         // Only connect country we have permission to move over
         if(m_pCountryTypes[c] != ERegionType::Blocked)
         {
            // Update all regions of the country
            ConnectCountry(c, (ERegionType::Enum) m_pCountryTypes[c] );
         }
      }

      // Verify the list of controlled waypoint and block them if needed.
      for(hash_map<UINT32,UINT32>::iterator l_iControlledWaypointIterator = m_vControlledWayPoint.begin();
         l_iControlledWaypointIterator!= m_vControlledWayPoint.end();l_iControlledWaypointIterator++)
      {
         UINT32 l_iWaypointIndex = l_iControlledWaypointIterator->first;
         UINT32 l_iRegionId = l_iControlledWaypointIterator->second;

         const vector<GRegionControl>& l_vRegions = g_ServerDAL.RegionControlArray();

         GRegionControl l_control = l_vRegions[l_iRegionId];
         bool l_bAtWarWith = g_ServerDAL.DiplomaticStatus(l_iOwner, l_control.m_iMilitary) == EDiplomaticStatus::Hostile;
         if(l_bAtWarWith) 
            m_pWaypointTypes[l_iWaypointIndex] =ERegionType::Blocked;
         
      }

   
      REAL32 l_fStartTime = (REAL32) g_Joshua.GameTime();
      vector<GVertexElevated> l_vVertexPath;

      // Move each unit group
      for(UINT32 g = 0;g < in_vUnitGroups.size();g ++)
      {
         SP2::GUnitGroupEx* l_pUnitGroup = (SP2::GUnitGroupEx*)in_vUnitGroups[g];
         if( (!l_pUnitGroup) || ( ( (SP2::GUnitGroup*) l_pUnitGroup)->Status() > EMilitaryStatus::Moving) )
         {
            continue;
         }

         //If the bombard flag is not set and the unit group is actually bombarding, cancel any bombardment.
         //We must cancel any actual bombardment
         if(!(in_Action & EMilitaryAction::BombardRegion) && (l_pUnitGroup->m_ActionAtDest &  EMilitaryAction::BombardRegion))
         {
            RemoveBombardingGroup(l_pUnitGroup->Id());
            l_pUnitGroup->RegionToBombard(0);
            l_pUnitGroup->m_ActionAtDest &= ~EMilitaryAction::BombardRegion;
            g_Joshua.UnitManager().ModifyUnitGroup(l_pUnitGroup);
         }


         // Locate starting region (unit location)
         UINT32 l_iStartRegion;
         if(!LocateRegionVertex(l_pUnitGroup->Position(), l_iStartRegion) )
         {
            LOG_PATH(GString("Unable to locate unit source") );
            continue;
         }

         // Locate destination region
         UINT32 l_iDestRegion;
         if(!LocateRegionVertex(in_Dest, l_iDestRegion) )
         {
            LOG_PATH(GString("Unable to locate unit destination") );
            continue;
         }

         // Make sure the current country is connected (at least as enemy)
         bool l_bChangingSource = false;
         UINT32 l_iSourceID = g_ServerDAL.RegionControl(m_vRegionGraph[l_iStartRegion].m_iRegionId).m_iMilitary;
         if(m_pWaypointTypes[l_iStartRegion] == ERegionType::Blocked)
         {
            l_bChangingSource = true;
            ConnectCountry(l_iSourceID, ERegionType::Enemy);
         }

         // Make sure the current country is connected (at least as enemy)
         bool l_bChangingSourcePolitical = false;
         UINT32 l_iSourcePoliticalID = g_ServerDAL.RegionControl(m_vRegionGraph[l_iStartRegion].m_iRegionId).m_iPolitical;
			if(m_pCountryTypes[l_iSourcePoliticalID] == ERegionType::Blocked)
         {
            l_bChangingSourcePolitical = true;
            ConnectCountry(l_iSourcePoliticalID, ERegionType::Enemy);
         }

         // Initialize path finder
         m_PathFinder.Reset();


         //Specify if the moving unit is a naval unit or a standard unit
         bool l_bImpossible = false;
         {
            if(l_pUnitGroup->IsNaval() )
            {
               // Naval units can only move on water
               if(m_vRegionGraph[l_iDestRegion].m_iRegionId != 0)
               {
                  l_bImpossible = true;
                  m_pRefusedMoveEvent->m_vRefusedMoves.push_back(make_pair(l_pUnitGroup->Id(), ERefusedMoveReason::NoWaterSupport) );
               }
               m_bMovingNavalUnit = true;
            }
            else
            {
               // Ground units can only move on ground
               if(m_vRegionGraph[l_iDestRegion].m_iRegionId == 0)
               {
                  l_bImpossible = true;
                  m_pRefusedMoveEvent->m_vRefusedMoves.push_back(make_pair(l_pUnitGroup->Id(), ERefusedMoveReason::NoLandSupport) );
               }
               m_bMovingNavalUnit = false;
            }
         }

         const GCountryData* const l_pOwnerCountry = g_ServerDAL.CountryData(l_iOwner);

         // These could change if the path gets shortened
         const GRegion* l_pDestRegion = g_ServerDAL.GetGRegion(m_vRegionGraph[l_iDestRegion].m_iRegionId);
         ENTITY_ID l_iDestOwner = l_pDestRegion->OwnerId();
         const GCountryData* l_pDestOwnerCountry = g_ServerDAL.CountryData(l_iDestOwner);
         ENTITY_ID l_iDestMilitaryOwner = l_pDestRegion->OwnerMilitaryId();

         //For gassert()
         const ENTITY_ID l_iOriginalDestMilitaryOwner = l_iDestMilitaryOwner;

         SDK::GWayPoint* l_pPath = NULL;
         if(!l_bImpossible)
         {
            const UINT8* const l_pStatuses = g_ServerDAL.CountryDiplomaticStatuses(l_iOwner);

            if(!m_bMovingNavalUnit)
            {
                const GRegion* const l_pStartRegion = g_ServerDAL.GetGRegion(m_vRegionGraph[l_iStartRegion].m_iRegionId);

                if(l_iOwner != l_iDestMilitaryOwner)
                {
                    GDZDebug::Log(l_pOwnerCountry->NameAndIDForLog() + L" is trying to move ground units " +
                                  L"from region " + g_ServerDAL.GetString(l_pStartRegion->NameId()) + L" " +
                                  L"of " + g_ServerDAL.CountryData(l_iSourcePoliticalID)->NameAndIDForLog() + L", " +
                                  L"controlled militarily by " + g_ServerDAL.CountryData(l_iSourceID)->NameAndIDForLog() + L", " +
                                  L"to region " + g_ServerDAL.GetString(l_pDestRegion->NameId()) + L" " +
                                  L"of " + l_pDestOwnerCountry->NameAndIDForLog() + L", " +
                                  L"controlled militarily by " + g_ServerDAL.CountryData(l_iDestMilitaryOwner)->NameAndIDForLog(),
                                  EDZLogCat::UnitMovement,
                                  __FUNCTION__, __LINE__);
                }
            }

            // Find requested path
            l_pPath = m_PathFinder.Path(l_iStartRegion, l_iDestRegion);

            if(l_pPath)
            {
               GGroupMove l_UnitMove;

               // Count number of points in path
               UINT32 l_iNbPoints = 1;
               SDK::GWayPoint* l_pCur = l_pPath;
               while(l_pCur->m_iPointID != l_iStartRegion)
               {
                  ++ l_iNbPoints;
                  l_pCur = l_pCur->m_pParent;
               }

               // Copy vertices to a vector
               {
                  l_vVertexPath.resize(l_iNbPoints);
                  UINT32 l_iIndex = l_iNbPoints - 1;

                  l_pCur = l_pPath;
                  while(l_pCur->m_iPointID != l_iStartRegion)
                  {
                     l_vVertexPath[l_iIndex].m_pVertex = &(m_vRegionGraph[l_pCur->m_iPointID] );
                     // Temporarily fill distance with distance computed during path-finding
                     l_vVertexPath[l_iIndex].m_fDistanceWithElevation = l_pCur->G;

                     l_pCur = l_pCur->m_pParent;
                     -- l_iIndex;
                  }
                  l_vVertexPath[l_iIndex].m_pVertex = &(m_vRegionGraph[l_pCur->m_iPointID] );
               }

               // Update each edge elevated distance using info found in the path
               {
                  REAL32 l_fLastDistance = 0.f;
                  for(UINT32 i = 0;i < l_vVertexPath.size() - 1;i ++)
                  {
                     l_vVertexPath[i].m_fDistanceWithElevation = 
                        l_vVertexPath[i + 1].m_fDistanceWithElevation - l_fLastDistance;
                     l_fLastDistance = l_vVertexPath[i + 1].m_fDistanceWithElevation;
                  }
               }

               const vector<GRegionControl>& l_vRegions = g_ServerDAL.RegionControlArray();
               bool l_bPathShortened = false;

               // If our start and dest point dont have same owner and destination is in hostile territory
               //  we'll check all waypoints to stop at first that is hostile
			      if( ( (l_vRegions[m_vRegionGraph[l_iStartRegion].m_iRegionId].m_iPolitical !=
                      l_vRegions[m_vRegionGraph[l_iDestRegion].m_iRegionId].m_iPolitical) || 
                     (l_vRegions[m_vRegionGraph[l_iStartRegion].m_iRegionId].m_iMilitary !=
                      l_vRegions[m_vRegionGraph[l_iDestRegion].m_iRegionId].m_iMilitary) ) &&
                     (l_pStatuses[l_vRegions[m_vRegionGraph[l_iDestRegion].m_iRegionId].m_iMilitary] ==
                      EDiplomaticStatus::Hostile) )
               {
                  UINT32 l_iOriginalPOwner = l_vRegions[m_vRegionGraph[l_iStartRegion].m_iRegionId].m_iPolitical;

                  for(UINT32 i = 1;i < l_vVertexPath.size() - 1;i ++)
                  {
                     // Check if this waypoint is hostile.  If it is we'll resize the vertex path,
                     //  fix l_iNbPoints and raise a flag to indicate path has been shortened
                     const UINT32 l_iVertexRegionId = l_vVertexPath[i].m_pVertex->m_iRegionId;
                     const GRegionControl& l_RegionControl = l_vRegions[l_iVertexRegionId];
                     const auto l_iTempDestMilitaryOwner = l_RegionControl.m_iMilitary;
                     if( (l_iTempDestMilitaryOwner != 0) &&
                         (l_iTempDestMilitaryOwner != l_iOriginalPOwner) &&
                         (l_pStatuses[l_iTempDestMilitaryOwner] == EDiplomaticStatus::Hostile) )
                     {
                        l_vVertexPath.resize(i + 1);
                        l_iNbPoints = i + 1;
                        l_bPathShortened = true;

                        l_pDestRegion = g_ServerDAL.GetGRegion(l_iVertexRegionId);
                        l_iDestOwner = l_RegionControl.m_iPolitical;
                        l_pDestOwnerCountry = g_ServerDAL.CountryData(l_iDestOwner);
                        l_iDestMilitaryOwner = l_iTempDestMilitaryOwner;
                        
                        GDZDebug::Log(L"Region " + g_ServerDAL.GetString(l_pDestRegion->NameId()) + L" " +
                                      L"of " + l_pDestOwnerCountry->NameAndIDForLog() + L", " +
                                      L"controlled militarily by " + g_ServerDAL.CountryData(l_iTempDestMilitaryOwner)->NameAndIDForLog() + L", " +
                                      L"is in the way",
                                      EDZLogCat::UnitMovement,
                                      __FUNCTION__, __LINE__);

                        break;
                     }
                  }
              }

              gassert(l_bPathShortened || l_iOriginalDestMilitaryOwner == l_iDestMilitaryOwner,
                      "Path wasn't shortened, but the destination region is different");

              // If the setting dictates it, never allow movement into an attacking country,
              // unless there's a war in which we're attacking them
              bool l_bAllowMovementBasedOnDefenderAttackingAttacker = true;
              if(!g_SP2Server->AllowDefenderAttackAttackerTerritory() &&
                 l_pStatuses[l_iDestOwner] == EDiplomaticStatus::Hostile &&
                 l_iDestOwner == l_iDestMilitaryOwner)
              {
                  l_bAllowMovementBasedOnDefenderAttackingAttacker = false;

                  const hash_map<UINT32,GWar>& l_mWars = g_ServerDAL.CurrentWars();
                  for(hash_map<UINT32,GWar>::const_iterator l_It = l_mWars.cbegin();
                      l_It != l_mWars.cend();
                      ++l_It)
                  {
                      const GWar& l_War = l_It->second;
                      const set<ENTITY_ID>& l_vAttackingSide = l_War.AttackingSide();
                      const set<ENTITY_ID>& l_vDefendingSide = l_War.DefendingSide();
                      if(l_vAttackingSide.find(l_iOwner) != l_vAttackingSide.cend() &&
                         l_vDefendingSide.find(l_iDestOwner) != l_vDefendingSide.cend())
                      {
                          GDZDebug::Log(l_pOwnerCountry->NameAndIDForLog() + L" is on the attacking side and " +
                                        l_pDestOwnerCountry->NameAndIDForLog() + L" is on the defending side of " +
                                        L"a war between " + g_ServerDAL.CountryData(l_War.MasterAttacking())->NameAndIDForLog() + L" and " +
                                        g_ServerDAL.CountryData(l_War.MasterDefending())->NameAndIDForLog(),
                                        EDZLogCat::UnitMovement | EDZLogCat::War,
                                        __FUNCTION__, __LINE__);
                          l_bAllowMovementBasedOnDefenderAttackingAttacker = true;
                          break;
                      }
                  }
              }

              if(l_bAllowMovementBasedOnDefenderAttackingAttacker)
              {
                  REAL32 l_fStopProtection = 0;
                  REAL32 l_fStartProtection = 0;

                  // Locate ending move protection point
                  UINT32 l_iEndProtectionID = 0xFFFFFFFF;

                  if(m_pCountryTypes[l_iTargetID] == ERegionType::Enemy )
                  {
                      for(INT32 p = l_iNbPoints - 1;p >= 0;p --)
                      {
                          if(g_ServerDAL.RegionControl(l_vVertexPath[p].m_pVertex->m_iRegionId).m_iMilitary != l_iTargetID)
                          {
                              break;
                          }
                          l_iEndProtectionID = p;
                      }
                  }


                  // Locate starting move protection point
                  UINT32 l_iStartProtectionID = 0xFFFFFFFF;

                  if(l_iEndProtectionID > 0)
                  {
                      if(m_pCountryTypes[l_iSourceID] == ERegionType::Enemy)
                      {
                          UINT32 l_iMaxID = min(l_iNbPoints - 1, l_iEndProtectionID);
                          for(UINT32 p = 1;p <= l_iMaxID;p ++)
                          {
                              l_iStartProtectionID = p;
                              if(g_ServerDAL.RegionControl(l_vVertexPath[p].m_pVertex->m_iRegionId).m_iMilitary != l_iSourceID)
                              {
                                  break;
                              }
                          }
                      }
                  }
                  else
                  {
                      l_iEndProtectionID = 0xFFFFFFFE;
                      l_fStopProtection = 0;
                  }

                  if(l_iStartProtectionID == 0xFFFFFFFF)
                  {
                      l_fStartProtection = 0;
                  }

                  // Create unit path (3rd dimension is distance to the next point!!!)
                  vector<pair<GVector3D<REAL32>, UINT32> > l_PathPoints;
                  UINT32 l_iTotalNbPoints = max(2, l_iNbPoints);
                  l_PathPoints.resize(l_iTotalNbPoints);

                  // Update first and last point
                  l_PathPoints.front().first = l_pUnitGroup->Position();
                  l_PathPoints.front().second = m_vRegionGraph[l_iStartRegion].m_iRegionId;

                  // If the path was shortened to prevent exploring deep territory, we must not set the
                  //  last point to our destination but to the last vertex of our vertex path
                  if(l_bPathShortened)
                  {
                      l_PathPoints.back().first = l_vVertexPath[l_vVertexPath.size() - 1].m_pVertex->m_Center;
                      l_PathPoints.back().second = l_vVertexPath[l_vVertexPath.size() - 1].m_pVertex->m_iRegionId;
                  }
                  else
                  {
                      // If it wasn't shortened we set our last pt to the destination
                      l_PathPoints.back().first = in_Dest;
                      l_PathPoints.back().second = l_iTargetRegionID;
                  }

                  // Set distances of first point
                  if(l_vVertexPath.size() > 1)
                  {
                      // Set first point distance, modulated by penalty between first waypoints
                      REAL32 l_fRealDistance = 
                          (REAL32) g_ServerDAL.DistanceBetween2PointsLatLong(
                          l_vVertexPath[0].m_pVertex->m_Center, 
                          l_vVertexPath[1].m_pVertex->m_Center);
                      REAL32 l_fElevatedDistance = l_vVertexPath[0].m_fDistanceWithElevation;
                      REAL32 l_fElevationPenalty = l_fElevatedDistance / l_fRealDistance;
                      gassert(_isnan(l_fElevationPenalty) == false,"Elevation penalty will be too much");

                      l_PathPoints.front().first.z = 
                          (REAL32) g_ServerDAL.DistanceBetween2PointsLatLong(
                          l_PathPoints.front().first, 
                          l_vVertexPath[1].m_pVertex->m_Center) * 
                          l_fElevationPenalty;
                  }
                  else
                  {
                      // Path connects directly to end point, no penalty
                      l_PathPoints.front().first.z = 
                          (REAL32) g_ServerDAL.DistanceBetween2PointsLatLong(
                          l_PathPoints.front().first, 
                          l_PathPoints.back().first);
                  }

                  // Fill remaining points
                  for(UINT32 p = 1;p < l_iNbPoints - 1;p ++)
                  {
                      l_PathPoints[p].first = l_vVertexPath[p].m_pVertex->m_Center;
                      l_PathPoints[p].second = l_vVertexPath[p].m_pVertex->m_iRegionId;

                      l_PathPoints[p].first.z = l_vVertexPath[p].m_fDistanceWithElevation;
                  }

                  // Compute penalty for last edge in the path (edge to the destination point)
                  if(l_vVertexPath.size() > 1)
                  {
                      // Set last distance, modulated by penalty between last waypoints
                      REAL32 l_fRealDistance = 
                          (REAL32) g_ServerDAL.DistanceBetween2PointsLatLong(
                          l_vVertexPath[l_iNbPoints - 1].m_pVertex->m_Center, 
                          l_vVertexPath[l_iNbPoints - 2].m_pVertex->m_Center);
                      REAL32 l_fElevatedDistance = l_vVertexPath[l_iNbPoints - 2].m_fDistanceWithElevation;
                      REAL32 l_fElevationPenalty = l_fElevatedDistance / l_fRealDistance;

                      l_PathPoints[l_iNbPoints - 2].first.z = 
                          (REAL32) g_ServerDAL.DistanceBetween2PointsLatLong(
                          l_PathPoints.back().first, 
                          l_vVertexPath[l_iNbPoints - 2].m_pVertex->m_Center) * 
                          l_fElevationPenalty;
                  }
                  else
                  {
                      // Path connects directly to end point, its already computed
                  }

                  // Now, convert each path element that crosses the -180/180 longitude boundary
                  // in 3 path elemnts:
                  //   - One from start point to the boundary
                  //   - One from boundary to inverse sign of boundary
                  //   - One from last boundary to end point
                  // Ex.: A path from -170 to 160 is converted to:
                  //   - -170 to -180
                  //   - -180 to 180
                  //   - 180 to 160
                  vector<pair<GVector3D<REAL32>, UINT32> >::iterator l_It = l_PathPoints.begin();
                  GVector3D<REAL32>* l_pLastPos = &(l_It->first);
                  ++ l_It;
                  while(l_It != l_PathPoints.end() )
                  {
                      GVector3D<REAL32>* l_pCurPos = &(l_It->first);

                      // Test current path segment
                      if(fabsf(l_pCurPos->x - l_pLastPos->x) > 180.f)
                      {
                          // Split path in three
                          l_It = l_PathPoints.insert(l_It, *l_It);
                          l_It = l_PathPoints.insert(l_It, *l_It);
                          vector<pair<GVector3D<REAL32>, UINT32> >::iterator l_PrevIt = l_It - 1;
                          vector<pair<GVector3D<REAL32>, UINT32> >::iterator l_NextIt = l_It + 1;
                          vector<pair<GVector3D<REAL32>, UINT32> >::iterator l_LastIt = l_NextIt + 1;

                          l_pLastPos = &(l_PrevIt->first);
                          l_pCurPos = &(l_LastIt->first);

                          REAL32 l_fDeltaX;

                          // Update middle point longitude
                          if(l_pLastPos->x > l_pCurPos->x)
                          {
                              l_It->first.x = 180.f;
                              l_NextIt->first.x = -180.f;
                              l_fDeltaX = 360.f - (l_pLastPos->x - l_pCurPos->x);
                          }
                          else
                          {
                              l_It->first.x = -180.f;
                              l_NextIt->first.x = 180.f;
                              l_fDeltaX = -(360.f - (l_pCurPos->x - l_pLastPos->x) );
                          }

                          // Compute middle point latitude
                          l_It->first.y = l_pLastPos->y +
                              (l_pLastPos->x - l_It->first.x) * 
                              (l_pLastPos->y - l_pCurPos->y) / l_fDeltaX;
                          l_NextIt->first.y = l_It->first.y;

                          // Update distances
                          REAL32 l_fOriginalEdgeDistance = sqrtf(l_fDeltaX * l_fDeltaX + 
                              (l_pLastPos->y - l_pCurPos->y) * (l_pLastPos->y - l_pCurPos->y) );
                          REAL32 l_fFirstEdgeDistance = sqrtf(
                              (l_pLastPos->x - l_It->first.x) * (l_pLastPos->x - l_It->first.x) + 
                              (l_pLastPos->y - l_It->first.y) * (l_pLastPos->y - l_It->first.y) );
                          REAL32 l_fDistancesRatio = l_fFirstEdgeDistance / l_fOriginalEdgeDistance;

                          REAL32 l_fOriginalElevatedDistance = l_PrevIt->first.z;

                          l_PrevIt->first.z = l_fOriginalElevatedDistance * l_fDistancesRatio;
                          l_It->first.z = 0.f;
                          l_NextIt->first.z = l_fOriginalElevatedDistance - l_PrevIt->first.z;

                          l_It = l_LastIt;
                      }

                      // Go to next path segment
                      l_pLastPos = l_pCurPos;
                      ++ l_It;
                  }

                  // Add distance to the path
                  SP2::GUnitGroupEx* l_pSP2Group = (SP2::GUnitGroupEx*) l_pUnitGroup;
                  CancelMovement(l_pSP2Group);
                  l_pSP2Group->Path().Reset(l_PathPoints.size() );
                  REAL32 l_fLastEdgeDist = l_PathPoints.front().first.z;
                  REAL32 l_fCurDist = 0;
                  l_pSP2Group->Path().AddPoint(0, l_PathPoints.front().first, l_PathPoints.front().second);
                  for(UINT32 i = 1;i < l_PathPoints.size();i ++)
                  {
                      REAL32 l_fDistance = l_fLastEdgeDist;
                      l_pSP2Group->Path().AddPoint(l_fCurDist + l_fDistance, l_PathPoints[i].first, l_PathPoints[i].second);
                      if(i == l_iStartProtectionID + 1)
                      {
                          l_fStartProtection = l_fCurDist + l_fDistance * 0.5f;
                      }
                      if(i == l_iEndProtectionID + 1)
                      {
                          l_fStopProtection = l_fCurDist + l_fDistance * 0.5f;
                      }
                      l_fCurDist += l_fDistance;
                      l_fLastEdgeDist = l_PathPoints[i].first.z;
                  }

                  if(l_iEndProtectionID == 0xFFFFFFFF)
                  {
                      l_fStopProtection = l_fCurDist;
                  }

                  l_pSP2Group->Path().SetupProtection(l_fStartProtection, l_fStopProtection);

                  l_UnitMove.m_Action = in_Action;
                  l_UnitMove.m_iDestRegion = l_iTargetRegionID;

                  // If unit is already moving, immediately update its path
                  if(l_pSP2Group->Status() == EMilitaryStatus::Moving)
                  {
                      ChangeGroupPath(l_pSP2Group, l_UnitMove, l_fStartTime);
                  }
                  else
                  {
                      // Change status of unit to ready
                      ChangeUnitState(l_pUnitGroup, EMilitaryStatus::Ready, true);

                      if(l_pSP2Group->Status() == EMilitaryStatus::Ready)
                      {
                          ChangeToMovingStatus(l_pSP2Group, l_fStartTime);
                      }

                      // Remember to move unit after unit status changed to moving
                      m_GroupsToMove[l_pUnitGroup->Id() ] = l_UnitMove;
                  }
              }
              else
              {
                  GDZDebug::Log(l_pOwnerCountry->NameAndIDForLog() + L" cannot move to territory of " +
                                l_pDestOwnerCountry->NameAndIDForLog() + L" due to lack of sufficient war status",
                                EDZLogCat::UnitMovement,
                                __FUNCTION__, __LINE__);

                  m_pRefusedMoveEvent->m_vRefusedMoves.push_back(make_pair(l_pUnitGroup->Id(), ERefusedMoveReason::NoMilitarySupport) );
              }
            }
            else
            {
               LOG_PATH(GString(L"Unit cant move : from ") + GString(l_iOwner) + GString(" to ") + GString(l_iTargetID) );

               // Fill a game event to indicate why the unit cannot move
               m_pRefusedMoveEvent->m_vRefusedMoves.push_back(make_pair(l_pUnitGroup->Id(), ERefusedMoveReason::NoMilitarySupport) );
            }
         }

         // Reset previous connectivity of source region (if needed)
         if(l_bChangingSource)
         {
            ConnectCountry(l_iSourceID, ERegionType::Blocked);
         }
			if( (l_bChangingSourcePolitical) && (l_iSourcePoliticalID != l_iSourceID) )
			{
            ConnectCountry(l_iSourcePoliticalID, ERegionType::Blocked);
			}
      }
   }

   // Indicate any refused movement
   if(!m_pRefusedMoveEvent->m_vRefusedMoves.empty() )
   {
      g_Joshua.RaiseEvent(m_pRefusedMoveEventSPtr);

      m_pRefusedMoveEventSPtr = CREATE_GAME_EVENT(SP2::Event::GRefuseMilitaryAction);
      m_pRefusedMoveEvent = (SP2::Event::GRefuseMilitaryAction*) m_pRefusedMoveEventSPtr.get();
   }

   return true;
}

const GVector2D<REAL32>& GUnitMover::RegionLocation(UINT32 in_iRegionID)
{
	// Returns the center of the first vertex of that region
	return m_vRegionGraph[(*m_vRegionVertices[in_iRegionID].begin())].m_Center;
}




/*!
* Deploy Units
* @param in_pGroup            : Group in wich the units are taken from 
* @param in_iUnitIdToDeploy   : Id of the unit to be deployed
* @param in_Dest              : Drop point destination
**/
void GUnitMover::DeployUnit(SP2::GUnitGroup* in_pGroup,list<UINT32>& in_UnitListToDeploy, const GVector2D<REAL32>& in_Dest)
{
   //Validate few things
   gassert(in_pGroup,"Invalid null pointer");
   gassert(in_UnitListToDeploy.size() > 0,"Empty group to deploy");
	gassert(in_pGroup->Status() == EMilitaryStatus::ReadyToDeploy,"Group is not a production queue group");	

   if( in_UnitListToDeploy.size() == 0)
      return;

   // Create a new group
   SP2::GUnitGroup*  l_pNewGroup = (SP2::GUnitGroup*)g_Joshua.UnitManager().CreateUnitGroup();
   gassert(l_pNewGroup,"New group couldn't be created. Invalid null pointer");

   // Give new Id to group.
   l_pNewGroup->Id(g_Joshua.UnitManager().NewUnitGroupID());

   // Give ownership to a country.
   l_pNewGroup->OwnerId(in_pGroup->OwnerId());

   // Set Group position
   g_Joshua.UnitManager().SetGroupPosition(l_pNewGroup,in_Dest);

   // Go through all IDs
   list<UINT32>::iterator l_UnitIdIterator = in_UnitListToDeploy.begin();
   while(l_UnitIdIterator != in_UnitListToDeploy.end())
   {
      // Get Unit from unit manager
      GUnit* l_pUnitToDeploy = (GUnit*)g_Joshua.UnitManager().Unit(*l_UnitIdIterator); 

		if(g_ServerDCL.IsUnitForSale(l_pUnitToDeploy->Id()))
			g_ServerDCL.SellUnit(l_pUnitToDeploy->Id());

      // If unit is in training cancel its training
      if(IsUnitInTraining(l_pUnitToDeploy->Id()))
      {
         CancelTraining(l_pUnitToDeploy->Id());
      }

      // Add this unit to the the new group
      g_Joshua.UnitManager().AddUnit2Group(l_pUnitToDeploy, l_pNewGroup);

      // Iterate over IDs
      l_UnitIdIterator++;
   }


   // Set group status to be in deployement
   l_pNewGroup->ChangeStatus(EMilitaryStatus::InDeployement);

   // Add this group to unit manager
   g_Joshua.UnitManager().AddUnitGroup(l_pNewGroup);
   
   // Add this group to the unit mover
   AddGroup(l_pNewGroup);
   
   // Add this new group to the deployement list
   GUnitGroupInDeployement l_UnitGroupInDeployement ;
   l_UnitGroupInDeployement.m_pGroupIDToDeploy           = l_pNewGroup->Id();

   // Calculate time to deploy
   // Linear distance / 2000km a day + 1 day
   const GVector2D<REAL32>& l_CapitalPosition = g_ServerDAL.CapitalPosition(l_pNewGroup->OwnerId());
   REAL32 l_fDistanceFromCapital = 0.f;

   // Only use the built-in distance calculation if the deployment location is far enough from the capital
   // This prevents units from getting stuck in deployment due to the calculation sometimes giving NaN for close distances
   if(fabs(l_CapitalPosition.x - in_Dest.x) > 0.01f || fabs(l_CapitalPosition.y - in_Dest.y) > 0.01f)
      l_fDistanceFromCapital = g_ServerDAL.DistanceBetween2PointsLatLong(l_CapitalPosition,in_Dest);

   REAL32 l_totalDayToDeploy = l_fDistanceFromCapital / 2000 + 1 ;
   l_UnitGroupInDeployement.m_fDeployementTime         = g_Joshua.GameTime()+ l_totalDayToDeploy;

   // Get the list for the current country.
   list<GUnitGroupInDeployement>* l_pListOfGroupInDeployement = &(m_vGroupsInDeployement[in_pGroup->OwnerId()-1] );
   l_pListOfGroupInDeployement->push_back(l_UnitGroupInDeployement);

	NotifyObserver(SDK::Combat::c_notificationUnitGroupModified,(void*)l_pNewGroup->OwnerId());

}


void GUnitMover::CancelDeployement(SP2::GUnitGroup* in_pGroup)
{
   gassert(in_pGroup != NULL,"Invalid null pointer");
   gassert(in_pGroup->Status() == EMilitaryStatus::InDeployement,"Group wansn't in deployement");

   // For each units in the group, thransfert them back in the production group.
   SP2::GUnitGroupEx* l_pProductionGroup = ProductionQueueUnitGroups()[in_pGroup->OwnerId()-1];
   gassert(l_pProductionGroup,"There should be a unit production queue");
   
   for(UINT32 i = 0 ;i < in_pGroup->Units().size() ; i++)
   {
      SDK::Combat::GUnit* l_pUnit  = (SP2::GUnit*)in_pGroup->Units()[0];

      // Add this unit back to the production group
      g_Joshua.UnitManager().AddUnit2Group(l_pUnit,l_pProductionGroup);
   }
 
   // Remove this group from the deployement list
   list<GUnitGroupInDeployement>* l_pListOfGroupInDeployement = &(m_vGroupsInDeployement[in_pGroup->OwnerId()-1] );
   list<GUnitGroupInDeployement>::iterator l_GroupInDeployementIterator = l_pListOfGroupInDeployement->begin();

   // TODO : put in ifdef.
   bool l_bSucess = false;
   while(l_GroupInDeployementIterator != l_pListOfGroupInDeployement->end())
   {
      GUnitGroupInDeployement l_GroupInDeployement = *l_GroupInDeployementIterator;
      if(l_GroupInDeployement.m_pGroupIDToDeploy == in_pGroup->Id())
      {
         l_pListOfGroupInDeployement->erase(l_GroupInDeployementIterator);
         l_bSucess = true;
         break;
      }
      l_GroupInDeployementIterator++;
   }

   gassert(l_bSucess,"Couldn't find group to remove from deployement list");


   // Kill this group
   g_Joshua.UnitManager().RemoveUnitGroup(in_pGroup->Id());
}

bool  GUnitMover::FinishDeployingGroup(SP2::GUnitGroup* in_pGroup)
{
   // Validate input
   gassert(in_pGroup,"Invalid null pointer");
   gassert(in_pGroup->Status() == EMilitaryStatus::InDeployement,"Group wasn't in deployement");

   g_ServerDAL.DirtyCountryUnitsServer(in_pGroup->OwnerId() );

   // JMercier TODO : Validate that we can Still deploy here.
   bool l_bIsNaval = in_pGroup->IsNaval();
   if(g_ServerDCL.IsDeployGroupAtPositionPossible((UINT16)in_pGroup->OwnerId(),in_pGroup->Position(),l_bIsNaval))
   {

      //If there is nuclear submarine, add the missiles to the nuclear missile list
      vector<SDK::Combat::GUnit*>::const_iterator l_UnitIterator = in_pGroup->Units().begin();
      while(l_UnitIterator !=  in_pGroup->Units().end())
      {

         GUnitDesign*  l_pUnitDesign= (SP2::GUnitDesign*)g_Joshua.UnitManager().UnitDesign((*l_UnitIterator)->Design()->Id());
		   if(l_pUnitDesign->Type()->Id() == EUnitType::BallisticMissileSubmarine)
            g_ServerDAL.CountryData((*l_UnitIterator)->Group()->OwnerId())->CreateMissilesForNuclearSubmarine((*l_UnitIterator)->Id());

         l_UnitIterator++;
      }

      //If a unit group is present at that exact location, merge the two
		bool l_bFoundUnitGroup = false;
		const set<UINT32>& l_UnitGroups = g_Joshua.UnitManager().CountryUnitGroups( in_pGroup->OwnerId() );		
		for(set<UINT32>::const_iterator it = l_UnitGroups.begin();
			 ((it != l_UnitGroups.end()) && (l_bFoundUnitGroup == false)); it++)
		{
			SDK::Combat::GUnitGroup* l_pUnitGroup = g_Joshua.UnitManager().UnitGroup(*it);
			if(l_pUnitGroup->Id() != in_pGroup->Id() &&
				l_pUnitGroup->Position() == in_pGroup->Position())
			{
				in_pGroup->ChangeStatus(EMilitaryStatus::Ready);
				vector<SDK::Combat::GUnitGroup*> l_vUnitsToMove;				
				l_vUnitsToMove.push_back(in_pGroup);
				MoveUnits(l_vUnitsToMove,l_pUnitGroup->Position(),EMilitaryAction::MergeWith);
				l_bFoundUnitGroup = true;
			}
		}

		if(!l_bFoundUnitGroup)	
		{
			// Make it permanent.
			// Set group status to ready.
			in_pGroup->ChangeStatus(EMilitaryStatus::Ready);
		}

      // Notify Client that this units was modified.
      g_Joshua.UnitManager().ModifyUnitGroup(in_pGroup);

		// Unit was deployed
		return true;
   }
   else
   {
		// Unit couldnt be deployed
      CancelDeployement(in_pGroup);
		return false;
   }
}

void GUnitMover::MilitaryRemoval(UINT32 in_iCountryLeaving, UINT32 in_iCountryAsking)
{
	set<UINT32> l_vRegionPossibilities;	
	set<UINT32> l_vAllies;
	const set<UINT32>& l_vRegionsToLeave = g_ServerDAL.CountryMilitaryControl(in_iCountryAsking);
	g_ServerDAL.CountryAllies(in_iCountryLeaving,l_vAllies);
	l_vAllies.erase(in_iCountryAsking);
	l_vAllies.insert(in_iCountryLeaving);
	
	//Fill the list of possible retreat regions
	for(set<UINT32>::const_iterator l_Itr = l_vAllies.begin();
		 l_Itr != l_vAllies.end();
		 l_Itr++)
	{
		l_vRegionPossibilities.insert(g_ServerDAL.CountryMilitaryControl(*l_Itr).begin(),
											   g_ServerDAL.CountryMilitaryControl(*l_Itr).end());
	}
	
	//Now cycle through units in asking country territory, and move them away
	const set<UINT32>& l_vUnitGroups = g_Joshua.UnitManager().CountryUnitGroups(in_iCountryLeaving);
	for(set<UINT32>::const_iterator l_Itr = l_vUnitGroups.begin();
		 l_Itr != l_vUnitGroups.end();
		 l_Itr++)
	{
		SDK::Combat::GUnitGroup* l_pGroup = g_Joshua.UnitManager().UnitGroup(*l_Itr);
		gassert(l_pGroup,"Group does not exist");
		UINT32 l_iStartRegion = ((SP2::GUnitGroupEx*)l_pGroup)->m_iRegionID;
		if( l_vRegionsToLeave.find( l_iStartRegion ) == l_vRegionsToLeave.end() )
		{
			//Country is not in an ennemy territory. But is it going there?
			if(((SP2::GUnitGroup*)l_pGroup)->Path().Points().size() > 0)
			{
				if( l_vRegionsToLeave.find( ((SP2::GUnitGroup*)l_pGroup)->Path().Points().back().m_iRegionID ) == l_vRegionsToLeave.end() )
				{
					//No, it's not going to an ennemy territory. Skip that unit. 
					continue;
				}					
			}
			
		}
			

		REAL32 l_fMinDistance = FLT_MAX;
		UINT32 l_iMinRegion = 0;

		//Find a destination for the unit group, cycle throuh region possibilites
		for(set<UINT32>::const_iterator l_ItrRegion = l_vRegionPossibilities.begin();
			 l_ItrRegion != l_vRegionPossibilities.end();
			 l_ItrRegion++)
		{
			REAL32 l_fDistance = GDALInterface::DistanceBetween2PointsLatLong(RegionLocation(*l_ItrRegion),l_pGroup->Position());
			if((l_fDistance < l_fMinDistance) && g_ServerDCL.MovePossible(in_iCountryLeaving,l_iStartRegion,*l_ItrRegion))
			{
				l_fMinDistance = l_fDistance;
				l_iMinRegion = *l_ItrRegion;
			}
		}
		if(l_iMinRegion)
		{
			//Move unit group towards the minimum region
			MoveUnitToRegion(*l_Itr,l_iMinRegion, EMilitaryAction::MergeWith);
		}
	}

}

void GUnitMover::MoveUnitToRegion(UINT32 in_iUnitGroupID, UINT32 in_iDestRegion, SP2::EMilitaryAction::Flags in_eAction)
{
	SDK::Combat::GUnitGroup* l_pUnitGroup = g_Joshua.UnitManager().UnitGroup(in_iUnitGroupID);

	const vector<UINT32>& l_vRegionCities = g_ServerDAL.CitiesInRegion(in_iDestRegion);

	vector<SDK::Combat::GUnitGroup*> l_vMovingGroups;
	l_vMovingGroups.push_back(l_pUnitGroup);

	//If there are no cities, let the unit move towards one of the vertex center
	if(!l_vRegionCities.size())
	{
		MoveUnits(l_vMovingGroups,RegionLocation(in_iDestRegion),in_eAction);
	}
	else
	{
		//Find the closest city
		GVector2D<REAL32> l_Destination;
		FindClosestCityLocation(l_pUnitGroup->Position(),in_iDestRegion,l_Destination);
		MoveUnits(l_vMovingGroups,l_Destination,in_eAction);
	}
	
}

/*!
 * @param	in_GroupLocation:	Compare which city is closest to that location
 *	@param	in_iRegionID:		The region into which we are looking for cities
 *	@param	out_CityLocation:	The city location will be stored there
 *
 *	@return	true if a city is found
 **/
bool GUnitMover::FindClosestCityLocation(const GVector2D<REAL32>& in_GroupLocation, 
															UINT32 in_iRegionID,
															GVector2D<REAL32>& out_CityLocation)
{
   const vector<UINT32>& l_vRegionCities = g_ServerDAL.CitiesInRegion(in_iRegionID);
   if(in_iRegionID < 1 || in_iRegionID > g_ServerDAL.NbRegion() )
		return false;

	UINT32 l_iMinPosition = 0;
	REAL32 l_fMinDistance = -1.f;
	REAL32 l_fDistance = 0.;

	for(UINT32 i=0; i < l_vRegionCities.size(); i++)
	{
		l_fDistance = GDALInterface::DistanceBetween2PointsLatLong(in_GroupLocation,
																				  g_ServerDAL.City(l_vRegionCities[i] )->m_Position);
		if(l_fDistance < l_fMinDistance || l_fMinDistance == -1.f)
		{
			l_fMinDistance = l_fDistance;
			l_iMinPosition = i;
		}			
	}
	if(l_fMinDistance != -1.f)
	{
		out_CityLocation = g_ServerDAL.City(l_vRegionCities[l_iMinPosition] )->m_Position;
		return true;
	}

	//If no destination is found, default value is current location
	out_CityLocation = in_GroupLocation;
	return false;
}


bool GUnitMover::FindNavalWaypointAdjacentToRegion(UINT32 in_iRegionID, Hector::GVector2D<REAL32>& out_Position) const
{
	if(in_iRegionID == 0)		
		return false;
	
	UINT32 l_iBestRegionVertexID = UINT_MAX;
	REAL32 l_fMinimumDistance = FLT_MAX;

   gassert(in_iRegionID < m_vRegionVertices.size(),"GUnitMover::FindNavalWaypointAdjacentToRegion invalid region id");

	for(UINT32 i=0; i < m_vRegionVertices[in_iRegionID].size(); i++)
	{
		const SRegionVertex* l_pRegionVertex = &m_vRegionGraph[m_vRegionVertices[in_iRegionID][i]];
		for(UINT32 j=0; j < l_pRegionVertex->m_iNbEdges; j++)
		{
			if( (m_vRegionGraph[l_pRegionVertex->m_pEdges[j].m_iDestID].m_iRegionId == 0) &&
				 (l_pRegionVertex->m_pEdges[j].m_fDistance < l_fMinimumDistance) )
			{
				 l_iBestRegionVertexID = l_pRegionVertex->m_pEdges[j].m_iDestID;
				 l_fMinimumDistance = l_pRegionVertex->m_pEdges[j].m_fDistance;
			}			
		}
	}
	if(l_iBestRegionVertexID != UINT_MAX)
	{
		out_Position = Hector::GVector2D<REAL32>(m_vRegionGraph[l_iBestRegionVertexID].m_Center);
		return true;
	}
	return false;
}

/*!
 * Cancels the status change for the given unit group
 * @param in_pGroup           : Unit to cancel the status change on
 **/
void GUnitMover::CancelStatusChange(SP2::GUnitGroupEx* in_pGroup)
{
   UINT32 l_iID = in_pGroup->Id();
   m_GroupsToFortify.erase(l_iID);

   list<UINT32>::iterator l_It = 
      find(m_GroupsChangingStatus.begin(), m_GroupsChangingStatus.end(), l_iID);
   if(l_It != m_GroupsChangingStatus.end() )
   {
      m_GroupsChangingStatus.erase(l_It);
   }

}

void GUnitMover::CancelTraining(UINT32 in_UnitId)
{
   list<GUnitInTraining>::iterator l_UnitInTrainingIt = m_ListOfUnitsInTraining.begin();
   while(l_UnitInTrainingIt!= m_ListOfUnitsInTraining.end())
   {
      if((*l_UnitInTrainingIt).m_iUnitId == in_UnitId)
      {
			m_ListOfUnitsInTraining.erase(l_UnitInTrainingIt);
         return;
      }
      l_UnitInTrainingIt++;
   }

   gassert(false,"Trying to cancel a  unit that is not in the training queue");   
}

bool GUnitMover::IsUnitInTraining(UINT32 in_UnitId)
{
   list<GUnitInTraining>::iterator l_UnitInTrainingIt = m_ListOfUnitsInTraining.begin();
   while(l_UnitInTrainingIt!= m_ListOfUnitsInTraining.end())
   {
      if((*l_UnitInTrainingIt).m_iUnitId == in_UnitId)
         return true;

      l_UnitInTrainingIt++;
   }

   return false;
}


void GUnitMover::TrainUnits(list<UINT32>& in_UnitListToTrain)
{
   gassert(in_UnitListToTrain.size() > 0 ,"We should have at least one unit to train");

   UINT32 l_iCountry = 0;
   REAL32 l_fTotalVal = 0;
	REAL32 l_fTotalCost = 0.f;
   // Iteratate over units
   list<UINT32>::iterator l_UnitIterator = in_UnitListToTrain.begin();
   while(l_UnitIterator != in_UnitListToTrain.end())
   {
      // Get Units pointer
      SP2::GUnit* l_pUnit = (SP2::GUnit*)g_Joshua.UnitManager().Unit((*l_UnitIterator));

      //If unit is already in training, ignore it
      if(!IsUnitInTraining(*l_UnitIterator))
      {
         gassert(l_pUnit,"Unit should stay in the training list");
         if(l_pUnit == NULL)
         {
            in_UnitListToTrain.erase(l_UnitIterator);
            break;
         }
         
	      SP2::GUnitDesign* l_pDesign = (SP2::GUnitDesign*)l_pUnit->Design();
         l_iCountry = l_pUnit->Group()->OwnerId();

		   // Remove nuclear missile from ballistic subs because we'll assign them at redeployment
		   if(l_pDesign->Type()->Id() == EUnitType::BallisticMissileSubmarine)
		   {
			   g_ServerDAL.CountryData(l_iCountry)->RemoveMissilesForNuclearSubmarine(l_pUnit->Id());
		   }

         // Get Deployement group
         SP2::GUnitGroupEx* l_pGroup = ProductionQueueUnitGroups()[l_iCountry - 1];
         gassert(l_pGroup,"There should be a unit production queue group where the units are to be placed when built");

         SP2::GUnitGroup *l_pGroupToRemoveUnitFrom = (SP2::GUnitGroup *)l_pUnit->Group();

         // If group is in transit, this unit can't be train.
         if(l_pGroupToRemoveUnitFrom->Status() == EMilitaryStatus::InDeployement)
         {
            continue;
         }

		   l_fTotalVal += (l_pUnit->Qty()*((SP2::GUnitDesign*)l_pUnit->Design())->Cost());		

         // Move it from its groups to the deployement group
         g_Joshua.UnitManager().AddUnit2Group(l_pUnit, l_pGroup);

         if(l_pGroupToRemoveUnitFrom->Units().size() == 0)
            g_Joshua.UnitManager().RemoveUnitGroup(l_pGroupToRemoveUnitFrom->Id());


         // Tag it as in Training

         // Add it to the training list
         GUnitInTraining l_UnitToTrain;
         l_UnitToTrain.m_iUnitId = *l_UnitIterator;
         l_UnitToTrain.m_ePreviousTrainingLevel = l_pUnit->Training();
         l_UnitToTrain.m_fTrainingStartTime = (REAL32) g_Joshua.GameTime();

         if(l_pUnit->Training() == ETrainingLevel::Elite)
         {
            //ignore the unit
         }
         else
         {
            ETrainingLevel::Enum l_eNextTrainingLevel = (ETrainingLevel::Enum)(l_pUnit->Training() + 1);
            l_UnitToTrain.m_fTrainingCompletionTime = (REAL32) (g_Joshua.GameTime() +  g_ServerDAL.TrainingUnitTime(l_eNextTrainingLevel) );
		      l_fTotalCost += g_ServerDAL.TrainingUnitCost(l_eNextTrainingLevel) * l_pUnit->Qty();
            m_ListOfUnitsInTraining.push_back(l_UnitToTrain);
         }

         // Tag the prodsuction group as modified so the client update the training status of the units.
         g_Joshua.UnitManager().ModifyUnitGroup(l_pGroup);
      }

      // Next unit
      l_UnitIterator++;
   }

   if(l_fTotalVal > 0)
   {
      g_ServerDAL.AddHistoricalMarker(l_iCountry, 
                                      EHistoryMarkerType::TrainUnit, 
                                      l_fTotalVal);
   }

	if(l_iCountry > 0)
		g_ServerDCL.TakeMoney(l_iCountry,l_fTotalCost);
}


/*!
 * Get the list of naval groups that are performing offshore bombardments
 **/
const hash_set<UINT32>& SP2::GUnitMover::BombardingGroupList() const
{
      return m_BombardingGroups;
}

/*!
 * Remove a group from the Offshore bombardment group list
 **/
bool SP2::GUnitMover::RemoveBombardingGroup(UINT32 in_iId)
{
   hash_set<UINT32>::iterator l_It = m_BombardingGroups.find(in_iId);
   if(l_It != m_BombardingGroups.end())
   {
       SP2::GUnitGroupEx* l_pGroup = (SP2::GUnitGroupEx*)g_Joshua.UnitManager().UnitGroup(in_iId);
      //Notify a bombardment as ended
      SDK::GGameEventSPtr l_Evt                    = CREATE_GAME_EVENT(SP2::Event::GBombardmentNotification);
      SP2::Event::GBombardmentNotification* l_pEvt = (SP2::Event::GBombardmentNotification*)l_Evt.get();
      l_pEvt->m_bStarting                          = false;
      l_pEvt->m_iSource                            = SDK::Event::ESpecialTargets::Server;
      l_pEvt->m_iTargetRegion                      = l_pGroup->RegionToBombard();
      l_pEvt->m_iTarget                            = SDK::Event::ESpecialTargets::BroadcastActiveHumanPlayers;
      g_Joshua.RaiseEvent(l_Evt);
      m_BombardingGroups.erase(l_It);
      return true;
   }
   else
      return false;
}

/*!
 * Add a group into the offshore bombardment group list
 **/
void SP2::GUnitMover::AddBombardingGroup(UINT32 in_iId)
{
   SP2::GUnitGroupEx* l_pGroup = (SP2::GUnitGroupEx*)g_Joshua.UnitManager().UnitGroup(in_iId);
   gassert(l_pGroup,"Group should exist");
   gassert(l_pGroup->RegionToBombard(),"Region to bombard should be > 0");
   m_BombardingGroups.insert(in_iId);

   //Notify a bombardment is starting in the given region, providing points where explosions should appear
   {
      SDK::GGameEventSPtr l_Evt                    = CREATE_GAME_EVENT(SP2::Event::GBombardmentNotification);
      SP2::Event::GBombardmentNotification* l_pEvt = (SP2::Event::GBombardmentNotification*)l_Evt.get();
      g_ServerDCL.BombardmentExplosionsPointsInRegion(l_pGroup->RegionToBombard(),l_pEvt->m_vBombardedPoints);
      l_pEvt->m_bStarting                          = true;
      l_pEvt->m_iSource                            = SDK::Event::ESpecialTargets::Server;
      l_pEvt->m_iTargetRegion                      = l_pGroup->RegionToBombard();
      l_pEvt->m_iTarget                            = SDK::Event::ESpecialTargets::BroadcastActiveHumanPlayers;
      g_Joshua.RaiseEvent(l_Evt);
   }

}

bool GUnitMover::OnSave(GIBuffer& io_Buffer)
{
   g_Joshua.Log(L"Saving unit status & movement...");

   // Save each region unit information
   for(UINT32 i = 0;i < g_ServerDAL.NbRegion();i ++)
   {
      io_Buffer << m_pRegionGroups[i];
   }

   // Save each country unit information
   for(UINT32 i = 1;i < g_ServerDAL.NbCountry();i ++)
   {
      io_Buffer << m_pCountryGroupsWithPath[i].m_fTotalValue
                << m_pCountryGroupsWithPath[i].m_Groups;
   }

   // Save unit transitory information
   io_Buffer << m_GroupsToFortify
             << m_GroupsChangingStatus
             << m_GroupsToMove;

   // Save unit production information
   io_Buffer << (UINT32) m_vProductionQueueGroups.size();
   for(UINT32 i = 0;i < m_vProductionQueueGroups.size();i ++)
   {
      io_Buffer << (m_vProductionQueueGroups[i] ? m_vProductionQueueGroups[i]->Id() : (UINT32) 0);
   }

   // Save unit deployment information
   io_Buffer << m_vGroupsInDeployement;

   // Save unit training information
   io_Buffer << m_ListOfUnitsInTraining;

   io_Buffer << m_BombardingGroups;


   g_Joshua.Log(L"Saving combats...");

   // Save all arenas
   const vector<SDK::Combat::GArena*>& l_Arenas = g_CombatManager.Arenas();

   io_Buffer << (UINT32) l_Arenas.size();
   for(vector<SDK::Combat::GArena*>::const_iterator it = l_Arenas.begin();
       it != l_Arenas.end();++ it)
   {
      (*it)->Serialize(io_Buffer);
   }
   return true;
}

bool GUnitMover::OnLoad(GOBuffer& io_Buffer)
{
   g_Joshua.Log(L"Loading unit status & movement...");

   // Load each region unit information
   for(UINT32 i = 0;i < g_ServerDAL.NbRegion();i ++)
   {
      io_Buffer >> m_pRegionGroups[i];
   }

   // Load each country unit information
   for(UINT32 i = 1;i < g_ServerDAL.NbCountry();i ++)
   {
      io_Buffer >> m_pCountryGroupsWithPath[i].m_fTotalValue
                >> m_pCountryGroupsWithPath[i].m_Groups;
   }

   // Load unit transitory information
   io_Buffer >> m_GroupsToFortify
             >> m_GroupsChangingStatus
             >> m_GroupsToMove;

   // Load unit production information
   UINT32 l_iCount;
   io_Buffer >> l_iCount;
   m_vProductionQueueGroups.resize(l_iCount);
   for(UINT32 i = 0;i < l_iCount;i ++)
   {
      UINT32 l_iID;
      io_Buffer >> l_iID;
      m_vProductionQueueGroups[i] = l_iID ? (SP2::GUnitGroupEx*) g_Joshua.UnitManager().UnitGroup(l_iID) : NULL;
   }

   // Load unit deployment information
   io_Buffer >> m_vGroupsInDeployement;

   // Load unit training information
   io_Buffer >> m_ListOfUnitsInTraining;

   io_Buffer >> m_BombardingGroups;

   g_Joshua.Log(L"Loading combats...");

   // Load all arenas
   io_Buffer >> l_iCount;
   for(UINT32 i = 0;i < l_iCount;i ++)
   {
      g_CombatManager.CreateArena(io_Buffer);
   }

   return true;
}

bool GUnitMover::OnNew(GDatabase* in_pDatabase)
{
   m_pCountryGroupsWithPath = new GCountryMovingGroups[g_ServerDAL.NbCountry() + 1];
   m_pCountryTypes = new UINT8[g_ServerDAL.NbCountry() + 1];

   CreateDeployementList();

   if(!g_SP2Server->UpdateStaticOnly() )
   {
      CreateProductionQueueUnitGroups();

	   GTable l_Results;
      GSString l_sSelectQuery("select * from selling_units");
      DB::EError e_Error = g_Joshua.QueryDB(l_sSelectQuery,l_Results);
      if(e_Error != DB_NO_ERROR)
      {
         g_Joshua.Log(L"Query error when loading the unit for sells table");
         return false;
      }


	   for(UINT32 i = 0 ; i < l_Results.RowCount()  ;i++)
      {
         INT32 l_iGroupID;

         l_Results.Row(i)->Cell(1)->GetData(l_iGroupID);
		   g_ServerDCL.SellUnit(l_iGroupID);
      }
   }

   return true;
}

void GUnitMover::OnClean()
{
   if(g_SP2Server->CleanVariableOnly() )
   {
      for(UINT32 i = 1;i <= g_ServerDAL.NbCountry();i ++)
      {
         m_pCountryGroupsWithPath[i].m_Groups.clear();
      }
   }
   else
   {
      SAFE_DELETE_ARRAY(m_pCountryGroupsWithPath);
      SAFE_DELETE_ARRAY(m_pCountryTypes);
   }

   for(UINT32 i = 0;i < g_ServerDAL.NbRegion();i ++)
   {
      m_pRegionGroups[i].clear();
   }

   m_GroupsToFortify.clear();
   m_GroupsChangingStatus.clear();
   m_vProductionQueueGroups.clear();
   m_vGroupsInDeployement.clear();
   m_ListOfUnitsInTraining.clear();
   m_GroupsToMove.clear();

   m_pRefusedMoveEventSPtr = CREATE_GAME_EVENT(SP2::Event::GRefuseMilitaryAction);
   m_pRefusedMoveEvent = (SP2::Event::GRefuseMilitaryAction*) m_pRefusedMoveEventSPtr.get();
   m_pArena = NULL;

   m_BombardingGroups.clear();

   g_CombatManager.Shutdown();
}


/*!
   * Perform offshore bombardment with the units in the list
   * (make the units bombard)
   **/
void GUnitMover::PerformOffShoreBombardment()
{
   REAL64 l_fTimeSinceLastCheck = g_Joshua.GameTime() - m_fLastBombardmentClock;
   if( l_fTimeSinceLastCheck < c_fOffShoreBombardmentsFrequencyDays)
   {
      return;
   }
   else
   {
      m_fLastBombardmentClock = g_Joshua.GameTime();
   }

   //Compute a global modifier based on various factors
   REAL32 l_fGlobalModifier;
   {
      REAL32 l_fTimeRatioModifier = (REAL32)(l_fTimeSinceLastCheck / c_fOffShoreBombardmentsFrequencyDays);
      gassert(l_fTimeRatioModifier >= 1,"Should always be => 1");
      REAL32 l_fRandomModifier    = g_CombatManager.RandomReal()+ 0.2f;

      l_fGlobalModifier = l_fTimeRatioModifier*l_fRandomModifier;
   }



   //List of modified countries
   set<UINT32> l_ModifiedCountryList;

   set<UINT32> l_GroupsToRemove;

   //For every unit group that is performing offshore bombardments
   for(hash_set<UINT32>::const_iterator l_GroupIt = m_BombardingGroups.begin();
                                   l_GroupIt != m_BombardingGroups.end() ;
                                   l_GroupIt++)
   {
      SP2::GUnitGroupEx* l_pAttackerGroup = (SP2::GUnitGroupEx*)g_Joshua.UnitManager().UnitGroup(*l_GroupIt);
      gassert(l_pAttackerGroup                                                 ,"Asking to perform bombardmets on a inexisting group");
      gassert(l_pAttackerGroup->m_ActionAtDest & EMilitaryAction::BombardRegion,"Unit group should be in bombard action mode");
      gassert(l_pAttackerGroup->RegionToBombard() > 0 && l_pAttackerGroup->RegionToBombard() <= g_ServerDAL.NbRegion(),"Invalid region to bombard");


      //If the unit group can perform offshore bombardments
      if(l_pAttackerGroup->CanPerformOffShoreBombardment())
      {
         //Offshore bombarding is separated in 2 parts
         // 1 - Military targets
         //    Killing enemy units in the target region
         // 2 - Civilian targets
         //    Reduction of infrastructure and resource development
         //Bombarding military targets
         {
            //List the enemy unit groups in the region
            vector<SP2::GUnitGroupEx*> l_EnemyUnitGroupsInRegion;
            {
               set<UINT32>& l_GroupsInRegion = m_pRegionGroups[l_pAttackerGroup->RegionToBombard()];
               for(set<UINT32>::const_iterator l_GroupsInRegionIt = l_GroupsInRegion.begin();
                                             l_GroupsInRegionIt != l_GroupsInRegion.end();
                                             l_GroupsInRegionIt++)
               {
                  SP2::GUnitGroupEx* l_pTargetGroup = (SP2::GUnitGroupEx*)g_Joshua.UnitManager().UnitGroup(*l_GroupsInRegionIt);
                  gassert(l_pTargetGroup,"Unit mover refers to an unexisting group");
                  EDiplomaticStatus::Enum l_DiplStat = g_ServerDAL.DiplomaticStatus(l_pAttackerGroup->OwnerId(),l_pTargetGroup->OwnerId());
                  if(l_DiplStat == EDiplomaticStatus::Hostile)
                  {
                     l_EnemyUnitGroupsInRegion.push_back(l_pTargetGroup);
                  }
               }
            }

            //Fetch the attack score (will be used to compute damage on enemy units and civilian damage)
            UINT32 l_iAttackScore    = g_ServerDCL.BombardmentAttackSumCharacteristics(l_pAttackerGroup);

            //If there are enemy groups
            if(l_EnemyUnitGroupsInRegion.size())
            {
               //A random group will be attacked
               UINT32 l_iTargetGroupIdx                        = (UINT32)(g_CombatManager.RandomReal()*l_EnemyUnitGroupsInRegion.size());
               SP2::GUnitGroupEx* l_pFinalTargetGroup          = l_EnemyUnitGroupsInRegion[l_iTargetGroupIdx];
               gassert(l_pFinalTargetGroup,"Group should not be NULL");

               //Fetch the defense score, to be used versus the attack score
               UINT32 l_iDefenderScore  = g_ServerDCL.BombardmentDefendSumCharacteristics(l_pFinalTargetGroup);
               UINT32 l_iNbHpDamage     = max((INT32)l_iAttackScore - (INT32)l_iDefenderScore,1);

               //Apply the global modifier
               l_iNbHpDamage = max((UINT32)(l_iNbHpDamage * l_fGlobalModifier),1);

               //Apply the damage
               for(UINT32 i = 0 ; i < l_pFinalTargetGroup->Units().size() ; i++)
               {
                  SP2::GUnit* l_pTargetUnit = (SP2::GUnit*)l_pFinalTargetGroup->Units()[i];
                  UINT32 l_iDamageToDo = min(l_iNbHpDamage,l_pTargetUnit->HP());

                  gassert(l_iDamageToDo,"Damage should be > 0");

                  SP2::GUnitGroupEx* l_pGroup     = (SP2::GUnitGroupEx*)l_pTargetUnit->Group();
                  gassert(l_pGroup,"Should be a group");
                  //If the unit is in a combat
                  if(l_pTargetUnit->m_pFightInfo)
                  {
                     SP2::GArenaInfo*   l_pArenaInfo = (SP2::GArenaInfo*)l_pGroup->ArenaInfo();
                     gassert(l_pArenaInfo,"Should be an arena info");
                     SP2::GArena*       l_pArena     = (SP2::GArena*)g_CombatManager.Arena(l_pArenaInfo->m_iId);
                     gassert(l_pArena,"There should be an arena");

                     l_pArena->OnUnitsHit(l_pTargetUnit,l_iDamageToDo,NULL);
                     //If we are going to kill that unit with our attack, ask her to leave the combat before
                     if(l_pTargetUnit->HP() <= l_iDamageToDo)
                     {
                        //If the unit is the last unit of the group, remove the group from the combat
                        if(l_pTargetUnit->Group()->Units().size() == 1)
                        {

                           gassert(l_pArena, "Should be an arena");
                           l_pArena->RemoveGroup(l_pGroup);
                        }
                        else
                        {
                           l_pTargetUnit->LeaveCombat();
                        }
                     }

                  }

                  g_Joshua.UnitManager().SetUnitHP(l_pTargetUnit, l_pTargetUnit->HP() - l_iDamageToDo, true);
                  l_iNbHpDamage -= l_iDamageToDo;

                  //No more damage to report.
                  if(!l_iNbHpDamage)
                     break;
               }
            } 
         }
         
         //Civilian Damage
         {
            GRegion* l_pRegionData = g_ServerDAL.GetGRegion(l_pAttackerGroup->RegionToBombard());
            //Affect the resource production
            {
               //A Random resource production will be affected
               EResources::Enum l_eAffectedResourceId = (EResources::Enum)(UINT32)(g_CombatManager.RandomReal() * EResources::ItemCount);
               REAL64 l_fRegionResourceProduction = l_pRegionData->ResourceProduction(l_eAffectedResourceId);
               REAL64 l_fRegionResourceProductionLoss = c_fOffshoreBombardmentImpactOnResourcePercent * l_fGlobalModifier * l_fRegionResourceProduction;
               l_pRegionData->ResourceProduction(l_eAffectedResourceId,max(0.f,l_pRegionData->ResourceProduction(l_eAffectedResourceId)-l_fRegionResourceProductionLoss));
            }
            //Affect infrastructure
            {
               REAL32 l_fInfrastructure = l_pRegionData->Infrastructure();
               REAL32 l_fInfrastructureLoss = l_fInfrastructure*c_fOffshoreBombardmentImpactOnInfrastructure*l_fGlobalModifier;
               l_pRegionData->Infrastructure(max(0.f,l_pRegionData->Infrastructure()-l_fInfrastructureLoss));
            }

            //Set the owner of the region to be modified
            l_ModifiedCountryList.insert(l_pRegionData->OwnerId());
         }
      }
      else
      {
         //Group cannot perform offshore bombardments.
         l_GroupsToRemove.insert(*l_GroupIt);
      }
   }


   //Remove the groups that cannot bombard anymore
   for(set<UINT32>::const_iterator l_It = l_GroupsToRemove.begin();
                                   l_It != l_GroupsToRemove.end();
                                   l_It++)
   {
      SP2::GUnitGroupEx* l_pGroup = (SP2::GUnitGroupEx*)g_Joshua.UnitManager().UnitGroup(*l_It);
      l_pGroup->RegionToBombard(0);
      l_pGroup->m_ActionAtDest &= ~EMilitaryAction::BombardRegion;
      g_Joshua.UnitManager().ModifyUnitGroup(l_pGroup);
      RemoveBombardingGroup(*l_It);
   }



   //For every modified country
   for(set<UINT32>::const_iterator l_It = l_ModifiedCountryList.begin(); 
                                   l_It != l_ModifiedCountryList.end();
                                   l_It++)
   {
      GCountryData* l_pCountryData = g_ServerDAL.CountryData(*l_It);
      l_pCountryData->SynchronizeWithRegions();
   }
}

void GUnitMover::TryDeployGroupMerge(SP2::GUnit* in_pUnit)
{
   SDK::Combat::GUnitGroup* l_pGroup = in_pUnit->Group();

   if(!ProductionQueueUnitGroups() [l_pGroup->OwnerId() - 1] )
      return;

   gassert(ProductionQueueUnitGroups() [l_pGroup->OwnerId() - 1]->Id() == l_pGroup->Id(),
           "Trying to merge a unit inside a group that is not the production group");

   // Try to merge the unit if it gains the same characteristic of an other unit
   for(UINT32 i = 0;(i < l_pGroup->Units().size() ) && in_pUnit;i ++)
   {
      SP2::GUnit* l_pDestUnit = (SP2::GUnit*) l_pGroup->Units() [i];
      if(l_pDestUnit->CanMergeWith(in_pUnit) &&
         (!g_ServerDCL.UnitMover().IsUnitInTraining(l_pDestUnit->Id() ) ) && 
         (!g_ServerDCL.IsUnitForSale(l_pDestUnit->Id() ) ) )
      {
         l_pDestUnit->MergeWithUnit(in_pUnit);
         g_Joshua.UnitManager().KillUnit(in_pUnit->Id() );
         in_pUnit = NULL;
      }
   }
}
