#include "golem_sdk_api.h"
/**************************************************************
*
* golem_pathfinder2d.h
*
* Description
* ===========
*  Describes the interface of class GPathFinder2D
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_PATHFINDER2D_H_
#define _GOLEM_PATHFINDER2D_H_

namespace SDK
{

   //Possible values for the 2D Map
   const INT32 NOT_PROCESSED =  0;
   const INT32 CANT_GO       = -1;
   const INT32 CLOSED        = -2;
   const INT32 START         =  1;
   const INT32 END           =  2;
   //If value is different from that, it's a pointer to a GWayPoint2D

   /*!
   * Struct to represent one step in the path
   * A collection of GWayPoint2D give a complete path
   * Each waypoint points to its father, a NULL parent pointer
   * equals the 1st point of the path
   *
   * F = G + H
   * where:
   *    F is the total estimated cost
   *    G is the cost to go from the start point to the end point
   *    H is an estimated distance to go from the point to the target point
   *    
   *  H is calculated with the Heuristic function
   **/
   struct GOLEM_SDK_API GWayPoint2D
   {
      GWayPoint2D();

      //! Updates the F = G + H
      void Update();

      INT32             F;
      INT32             G;
      INT32             H;
      GVector2D<INT32>  m_Coord;
      GWayPoint2D*        m_pParent; //Position of the parent in the closed list


   };

   //! Empty class so we can define pointer to functions
   class GHeuristic{};

   //! Define a pointer to a heuristic function to get the distance between 2 points (estimated or not)
   typedef INT32 (GHeuristic::*HEURISTIC_FUNCTION_POINTER)(const GVector2D<INT32>& in_StartPoint,
      const GVector2D<INT32>& in_EndPoint,
      bool  in_bTrueDistance);


   /*!
   * 2D Path finding using the A* [AStar] algorithm
   *
   * Needs a pointer to a heuristic function that will help evaluate distance between two
   * points.
   **/
   class GOLEM_SDK_API GPathFinder2D
   {
   public:
      GPathFinder2D();
      ~GPathFinder2D();

      //! Initialize the path finder
      bool Initialize(const INT32  in_iWidth,
         const INT32  in_iHeight,
         HEURISTIC_FUNCTION_POINTER in_pFunc,
         GHeuristic* in_pHeuristicInstance);
      //! Shuts down the path finder
      bool Shutdown();

      //! Resets the 2D Map
      void Reset();

      //! Finds the path from the given point to the target point
      GWayPoint2D* Path(const GVector2D<INT32>& in_Start,const GVector2D<INT32>& in_End);


   protected:
   private:
      INT32**  m_pArea;
      INT32    m_iAreaWidth;
      INT32    m_iAreaHeight;

      GVector2D<INT32> m_Start;
      GVector2D<INT32> m_End;


      //! Checks if the point with the given coordinates is valid (within tha area bounds)
      inline bool IsValid(const GVector2D<INT32> &in_Point) const;
      //! Get the estimated weight from the current point to the end point given the heuristic function
      INT32 WeightEstimatedToEnd(const GVector2D<INT32>& in_Point) const;
      //! Get the real distance from between two points
      INT32 WeightReal(const GVector2D<INT32>& in_Start,const GVector2D<INT32>& in_End);


      //! Get the next available way point from the way point pool
      inline GWayPoint2D* NextAvailableWayPoint();

      GBinaryHeap<INT32,GWayPoint2D*>* m_pOpenList;
      HEURISTIC_FUNCTION_POINTER     m_pHeuristicFunc;
      GHeuristic*                    m_pHeuristicInstance;
      //vector<GWayPoint2D*>             m_ClosedList;


      GWayPoint2D*                     m_pWayPointsPool;
      UINT32                         m_iNextWayPoint;

   };

}

#endif //_GOLEM_PATHFINDER2D_H_
