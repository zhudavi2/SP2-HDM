/**************************************************************
*
* sp2_region_connect_graph.h
*
* Description
* ===========
*  Describes & implements class SP2::GRegionConnectGraph, 
*  which represents in a graph the connection between each 
*  region of the world, and allows queries concerning 
*  connectivity of regions to be answered.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_SP2_REGION_CONNECT_GRAPH_H_
#define _GOLEM_SP2_REGION_CONNECT_GRAPH_H_

namespace SP2
{
   struct GRegionGraphNode
   {
      GVector2D<REAL32> m_Position;
      UINT32            m_iNbNeighbors;
      UINT32*           m_pNeighborIDs;
   };

   /*!
    * Represent in a graph the connection between each 
    * region of the world which allows queries concerning 
    * connectivity of regions to be answered.
    **/
   class GRegionConnectGraph
   {
      //! Number of entries in the graph
      UINT32               m_iNbRegions;

      //! Array that indicates for each region what other region is connected
      GRegionGraphNode*    m_pGraph;

      //! Array that contains all neighbors
      UINT32*              m_pNeighbors;

      GVector2D<REAL32> FindRegionCenter(const Map::GMultiface* in_pMultiface) const;

      REAL32 FaceCentroidAndArea(const Map::GFace* in_pFace, GVector2D<REAL32>& out_Centroid) const;

   public:
      GRegionConnectGraph();
      ~GRegionConnectGraph();

      /*!
       * Initialize the object by using the current country view to 
       * construct the region connectivity graph
       *
       * @return  Boolean indicating if initialization was sucessful
       **/
      bool Init();

      /*!
       * \brief Return the number of entries in the graph.
       *
       * @return  Number of elements in the graph
       **/
      UINT32 NbRegions() const { return m_iNbRegions; }

      /*!
       * \brief Return the graph indicating the connectivity of each region.
       *
       * @return  Constant pointer to the graph data
       **/
      const GRegionGraphNode* Graph() const { return m_pGraph; }
   };
}

#endif // #ifndef _GOLEM_SP2_REGION_CONNECT_GRAPH_H_
