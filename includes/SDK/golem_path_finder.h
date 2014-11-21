/**************************************************************
*
* golem_path_finder.h
*
* Description
* ===========
*  Describes the interface of class GPathFinder
*
* Owner
* =====
*  Frederic
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/
#include "golem_sdk_api.h"

#ifndef _GOLEM_PATH_FINDER_H_
#define _GOLEM_PATH_FINDER_H_

namespace SDK
{
   /*!
    * Struct to represent one step in the path
    * A collection of GWayPoint give a complete path
    * Each waypoint points to its father, a NULL parent pointer
    * equals the 1st point of the path
    *
    * F = G + H
    * where:
    *    F is the total estimated cost
    *    G is the cost to go from the start point to the point
    *    H is an estimated distance to go from the point to the target point
    *    
    *  H is calculated with the Heuristic function
    *
    * For the algorithm to be complete, H must never overestimate the remaining
    * distance to the destination
    **/
   struct GWayPoint
   {
      REAL32            F()
      {
         return G + H;
      }

      REAL32            G;
      REAL32            H;
      UINT32            m_iPointID;
      GWayPoint*        m_pParent;
   };

   /*!
    * \brief Class that must be implemented to do a pathfinding in any data structure.
    *
    * The accessors of this class are used to navigate within the data, which
    * allows any kind of structure to be used for the pathfinding algorithm.
    **/
   class GPathFinderDescriptor
   {
   public:
      /*!
       * \brief Set a point to the current node whose neighbors are navigated.
       *
       * Usually, this functions remember which point it has received in parameter
       * and initializes an iterator that will be used later by the NextNeighbor
       * method to iterate through neighbors of this point.
       *
       * @param   in_iPointID  : ID of the point whose neighbors will be traversed
       **/
      virtual void ResetNeighbors(UINT32 in_iPointID) = 0;

      /*!
       * \brief Navigate through neighbors of current point and calculate cost of going to this neighbor.
       *
       * A call to this method is always preceded by at least one call to ResetNeighbors
       * to set which neighbor to navigate. It usually use an iterator initialized by
       * ResetNeighbors to find the next point, and increments the iterator before.
       * This method will be called until it returns 0xFFFFFFFF. which means there is
       * no more neighbors to explore. Do not forget to set the cost of the path to this
       * neighbor before returning.
       *
       * @param   out_fCost   : Cost of the path between current point and returned neighbor.
       *
       * @return  ID of the current neighbor point, or 0xFFFFFFFF if there is no more neighbors.
       **/
      virtual UINT32 NextNeighbor(REAL32& out_fCost) = 0;

      /*!
       * \brief Estimate the cost of a path between the two specified points.
       *
       * Remember that the heuristic must never overestimate the real cost, otherwise
       * the pathfinding algorithm may not be complete.
       *
       * @param   in_iStartID : ID of the first point
       * @param   in_iEndID   : ID of the second point
       *
       * @return  Estimated cost.
       **/
      virtual REAL32 Heuristic(UINT32 in_iStartID, UINT32 in_iEndID) = 0;
   };

   /*!
    * General path finding using the A* [AStar] algorithm
    *
    * Needs an instance implementing the GPathFinderDescriptor interface
    **/
   class GOLEM_SDK_API GPathFinder
   {
   public:
      GPathFinder();
      ~GPathFinder();

      //! Initialize the path finder
      bool Initialize(UINT32 in_iNbPoints,
                      GPathFinderDescriptor* in_pDesc);

      //! Shuts down the path finder
      bool Shutdown();

      //! Resets the path finder
      void Reset();

      //! Finds the path from the given point to the target point
      GWayPoint* Path(UINT32 in_iStart, UINT32 in_iEnd);
    
   protected:
   private:
      enum EPointValue
      {
         // Possible values for the point list
         NOT_PROCESSED = 0,
         CANT_GO       = -1,
         CLOSED        = -2,
         START         = -3,
         END           = -4,
      };

      INT32*   m_pPoints;
      UINT32   m_iNbPoints;

      //! Get the next available way point from the way point pool
      GWayPoint* NextAvailableWayPoint();

      GBinaryHeap<REAL32, GWayPoint*>* m_pOpenList;
      GPathFinderDescriptor*           m_pDesc;

      GWayPoint*                       m_pWayPointsPool;
      UINT32                           m_iNextWayPoint;
   };
}

#endif //_GOLEM_PATH_FINDER_H_
