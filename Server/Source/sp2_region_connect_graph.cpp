/**************************************************************
*
* sp2_region_connect_graph.cpp
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

GRegionConnectGraph::GRegionConnectGraph()
{
   Memory::Clear(this);
}

GRegionConnectGraph::~GRegionConnectGraph()
{
   // Delete allocated memory
   SAFE_DELETE_ARRAY(m_pGraph);
   SAFE_DELETE_ARRAY(m_pNeighbors);
}

bool GRegionConnectGraph::Init()
{
   // Fetch number of regions (including water)
   m_iNbRegions = g_ServerDAL.Admin().NbFeatures() + 1;

   // Allocate graph memory
   m_pGraph = new GRegionNeighbors[m_iNbRegions];

   // Count number of neighbors for each region 
   // (assume each region connects to the water)
   UINT32 l_iTotalNeighbors;

   m_pGraph[0].m_iNbNeighbors = m_iNbRegions;
   l_iTotalNeighbors = m_iNbRegions;

   vector<const Map::GMultiface*> l_vNeighbors;
   l_vNeighbors.reserve(m_iNbRegions);

   for(UINT32 i = 1;i < m_iNbRegions;i ++)
   {
      g_ServerDAL.CountryView()->Multiface(i)->Neighbors(l_vNeighbors);
      m_pGraph[i].m_iNbNeighbors = l_vNeighbors.size() + 1;
      l_iTotalNeighbors += m_pGraph[i].m_iNbNeighbors;
   }

   // Allocate neighbor array
   m_pNeighbors = new UINT32[l_iTotalNeighbors];

   // Fill neighbor array
   m_pGraph[0].m_iNbNeighbors = 0;
   UINT32 l_iCurIndex = m_iNbRegions;

   for(UINT32 i = 1;i < m_iNbRegions;i ++)
   {
      g_ServerDAL.CountryView()->Multiface(i)->Neighbors(l_vNeighbors);
      m_pGraph[i].m_pNeighborIDs = m_pNeighbors + l_iCurIndex;
      l_iCurIndex += m_pGraph[i].m_iNbNeighbors;

      for(UINT32 n = 0;n < l_vNeighbors.size();n ++)
      {
         m_pGraph[i].m_pNeighborIDs[n] = l_vNeighbors[i]->Id();
      }

      // Verify if region has access to water
      if(g_ServerDAL.FindWaterAccess(g_ServerDAL.CountryView()->Multiface(i) ) )
      {
         m_pGraph[i].m_pNeighborIDs[m_pGraph[i].m_iNbNeighbors - 1] = 0;
         m_pGraph[0].m_pNeighborIDs[m_pGraph[0].m_iNbNeighbors] = i;
         m_pGraph[0].m_iNbNeighbors ++;
      }
      else
      {
         m_pGraph[i].m_iNbNeighbors --;
      }
   }

   return true;
}
