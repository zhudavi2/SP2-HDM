/**************************************************************
*
* golem_geometry.h
*
* Description
* ===========
*  Description & implementation of a classes specifying 
*  different geometric elements.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#pragma once

#include "golem_vector.h"
#include "golem_facet_list.h"

namespace Hector
{
   namespace Geometry
   {

      //! \todo TBM : Move these functions on the appropriate class

      //! Test if a point lies inside a polygon
      template<typename TPoly, typename T>
         bool PointIsInsideContour(const GShape& in_ContourShape, 
                                   const TPoly& in_Contour, 
                                   const GVector2D<T>& in_P)
      {
         UINT32 l_iIntersectCount = 0;
         GVector2D<T> l_Previous = in_ContourShape.Position(in_Contour.Index(0) );
         for(UINT32 c = in_Contour.NbIndexes();c > 0;c --)
         {
            GVector2D<T> l_Current = in_ContourShape.Position(in_Contour.Index(c - 1) );
            if( (l_Current.y < in_P.y) && (l_Previous.y < in_P.y) )
            {
               ;
            }
            else if( (l_Current.y >= in_P.y) && (l_Previous.y >= in_P.y) )
            {
               ;
            }
            else
            {
               if( (l_Current.x < in_P.x) && (l_Previous.x < in_P.x) )
               {
                  ;
               }
               else if( (l_Current.x >= in_P.x) && (l_Previous.x >= in_P.x) )
               {
                  l_iIntersectCount ++;
               }
               else
               {
                  if(l_Current.y > l_Previous.y)
                  {
                     if(SignedArea(l_Previous, l_Current, in_P) >= 0)
                     {
                        l_iIntersectCount ++;
                     }
                  }
                  else
                  {
                     if(SignedArea(l_Current, l_Previous, in_P) >= 0)
                     {
                        l_iIntersectCount ++;
                     }
                  }
               }
            }
            l_Previous = l_Current;
         }

         return l_iIntersectCount & 0x01;
      }

      //! Test if a point lies inside a face
      template<typename TPoly, typename T>
         bool PointIsInsideFace(const GShape& in_ContourShape, 
                                const GFace<TPoly>& in_Face, 
                                const GVector2D<T>& in_P)
      {
         const TPoly& l_Outer = in_Face.Outer();

         // Check if point is inside outer contour
         if(PointIsInsideContour(in_ContourShape, l_Outer, in_P) )
         {
            // Check if point is inside any inner contour
            for(UINT32 i = 0;i < in_Face.NbInners();i ++)
            {
               const TPoly& l_Inner = in_Face.Inner(i);

               if(PointIsInsideContour(in_ContourShape, l_Inner, in_P) )
               {
                  return false;
               }
            }
            return true;
         }

         return false;
      }

      //! Computes 2x the signed area of tree points.
      /*!
      * This can be used to test if point P is to the left or to the right or colinear 
      * with segment AB.
      * If P is to the right of AB, result will be negative.
      * If P is to the left of AB, result will be positive.
      * If P is colinear with AB, result will be 0.
      **/
      template <typename T>
         T SignedArea(const GVector2D<T>& in_A, 
                      const GVector2D<T>& in_B, 
                      const GVector2D<T>& in_P)
      {
         // This is equivalent to the cross product (B - A) x (P - A)
         return   in_A.x * in_B.y - in_B.x * in_A.y + 
                  in_P.x * in_A.y - in_A.x * in_P.y +
                  in_B.x * in_P.y - in_P.x * in_B.y;
      }

      //! Test if a point lies inside a CW triangle
      template <typename T>
         bool PointIsInsideTriangleCW(const GVector2D<T> in_TrianglePoints[3], 
                                      const GVector2D<T>& in_P)
      {
         // To be inside a clockwise triangle, a point must be to the right of each segment
         if( (SignedArea(in_TrianglePoints[0], in_TrianglePoints[1], in_P) <= 0.f) && 
             (SignedArea(in_TrianglePoints[1], in_TrianglePoints[2], in_P) <= 0.f) && 
             (SignedArea(in_TrianglePoints[2], in_TrianglePoints[0], in_P) <= 0.f) )
         {
            return true;
         }

         return false;
      }

      //! Test if a point lies inside a CW triangle but not on its contour
      template <typename T>
         bool PointIsInsideTriangleCWStrict(const GVector2D<T> in_TrianglePoints[3], 
                                            const GVector2D<T>& in_P)
      {
         // To be inside a clockwise triangle, a point must be to the right of each segment
         if( (SignedArea(in_TrianglePoints[0], in_TrianglePoints[1], in_P) < 0.f) && 
             (SignedArea(in_TrianglePoints[1], in_TrianglePoints[2], in_P) < 0.f) && 
             (SignedArea(in_TrianglePoints[2], in_TrianglePoints[0], in_P) < 0.f) )
         {
            return true;
         }

         return false;
      }

      //! Test if a point lies inside a CCW triangle
      template <typename T>
         bool PointIsInsideTriangleCCW(const GVector2D<T> in_TrianglePoints[3], 
                                       const GVector2D<T>& in_P)
      {
         // To be inside a counterclockwise triangle, a point must be to the left of each segment
         if( (SignedArea(in_TrianglePoints[0], in_TrianglePoints[1], in_P) >= 0.f) && 
             (SignedArea(in_TrianglePoints[1], in_TrianglePoints[2], in_P) >= 0.f) && 
             (SignedArea(in_TrianglePoints[2], in_TrianglePoints[0], in_P) >= 0.f) )
         {
            return true;
         }

         return false;
      }

      //! Test if a point lies inside a CCW triangle but not on its contour
      template <typename T>
         bool PointIsInsideTriangleCCWStrict(const GVector2D<T> in_TrianglePoints[3], 
                                             const GVector2D<T>& in_P)
      {
         // To be inside a counterclockwise triangle, a point must be to the left of each segment
         if( (SignedArea(in_TrianglePoints[0], in_TrianglePoints[1], in_P) > 0.f) && 
             (SignedArea(in_TrianglePoints[1], in_TrianglePoints[2], in_P) > 0.f) && 
             (SignedArea(in_TrianglePoints[2], in_TrianglePoints[0], in_P) > 0.f) )
         {
            return true;
         }

         return false;
      }

      //! Test if two segments (P1Q1 & P2Q2) intersects
      template <typename T>
         bool SegmentIsIntersecting(const GVector2D<T>& in_P1, 
                                    const GVector2D<T>& in_Q1, 
                                    const GVector2D<T>& in_P2, 
                                    const GVector2D<T>& in_Q2)
      {
         // Two segments intersection algorithm found p.614 of 
         // "Data Structures & algorithms in java"

         // Find orientation of each point with each segment
         T l_OrientationP1Q1P2 = in_P1.x * in_Q1.y - in_Q1.x * in_P1.y + 
            in_P2.x * in_P1.y - in_P1.x * in_P2.y +
            in_Q1.x * in_P2.y - in_P2.x * in_Q1.y;

         T l_OrientationP1Q1Q2 = in_P1.x * in_Q1.y - in_Q1.x * in_P1.y + 
            in_Q2.x * in_P1.y - in_P1.x * in_Q2.y +
            in_Q1.x * in_Q2.y - in_Q2.x * in_Q1.y;

         T l_OrientationP2Q2P1 = in_P2.x * in_Q2.y - in_Q2.x * in_P2.y + 
            in_P1.x * in_P2.y - in_P2.x * in_P1.y +
            in_Q2.x * in_P1.y - in_P1.x * in_Q2.y;

         T l_OrientationP2Q2Q1 = in_P2.x * in_Q2.y - in_Q2.x * in_P2.y + 
            in_Q1.x * in_P2.y - in_P2.x * in_Q1.y +
            in_Q2.x * in_Q1.y - in_Q1.x * in_Q2.y;

         // Treat special case when segments are colinear
         if( (l_OrientationP1Q1P2 == 0) && 
            (l_OrientationP1Q1Q2 == 0) && 
            (l_OrientationP2Q2P1 == 0) &&
            (l_OrientationP2Q2Q1 == 0) )
         {
            GVector2D<T> l_Origin(0,0);
            T l_DistP1 = l_Origin.DistanceSquared(in_P1);
            T l_DistQ1 = l_Origin.DistanceSquared(in_Q1);
            T l_DistP2 = l_Origin.DistanceSquared(in_P2);
            T l_DistQ2 = l_Origin.DistanceSquared(in_Q2);

            T l_Dist1 = min(l_DistP1, l_DistQ1);
            T l_Length1 = abs(l_DistP1 - l_DistQ1);
            if( (l_DistP2 - l_Dist1 < l_Length1) || 
                (l_DistQ2 - l_Dist1 < l_Length1) )
            {
               return true;
            }
         }
         else
         {
            if( (l_OrientationP1Q1P2 * l_OrientationP1Q1Q2 <= 0) && 
                (l_OrientationP2Q2P1 * l_OrientationP2Q2Q1 <= 0) )
            {
               return true;
            }
         }

         return false;
      }

      //! Test if two segments (P1Q1 & P2Q2) cross
      template <typename T>
         bool SegmentIsCrossing(const GVector2D<T>& in_P1, 
                                const GVector2D<T>& in_Q1, 
                                const GVector2D<T>& in_P2, 
                                const GVector2D<T>& in_Q2)
      {
         //REAL32 l_OrientationAABBCC = in_AA.x * in_BB.y - in_BB.x * in_AA.y + 
         //                             in_CC.x * in_AA.y - in_AA.x * in_CC.y +
         //                             in_BB.x * in_CC.y - in_CC.x * in_BB.y;

         // Two segments intersection algorithm found p.614 in "Data Structures & algorithms in java"
         T l_OrientationP1Q1P2 = in_P1.x * in_Q1.y - in_Q1.x * in_P1.y + 
            in_P2.x * in_P1.y - in_P1.x * in_P2.y +
            in_Q1.x * in_P2.y - in_P2.x * in_Q1.y;

         T l_OrientationP1Q1Q2 = in_P1.x * in_Q1.y - in_Q1.x * in_P1.y + 
            in_Q2.x * in_P1.y - in_P1.x * in_Q2.y +
            in_Q1.x * in_Q2.y - in_Q2.x * in_Q1.y;

         T l_OrientationP2Q2P1 = in_P2.x * in_Q2.y - in_Q2.x * in_P2.y + 
            in_P1.x * in_P2.y - in_P2.x * in_P1.y +
            in_Q2.x * in_P1.y - in_P1.x * in_Q2.y;

         T l_OrientationP2Q2Q1 = in_P2.x * in_Q2.y - in_Q2.x * in_P2.y + 
            in_Q1.x * in_P2.y - in_P2.x * in_Q1.y +
            in_Q2.x * in_Q1.y - in_Q1.x * in_Q2.y;

         if( (l_OrientationP1Q1P2 == 0) && 
            (l_OrientationP1Q1Q2 == 0) && 
            (l_OrientationP2Q2P1 == 0) &&
            (l_OrientationP2Q2Q1 == 0) )
         {
            return false;
         }
         else
         {
            if( (l_OrientationP1Q1P2 * l_OrientationP1Q1Q2 < 0) && (l_OrientationP2Q2P1 * l_OrientationP2Q2Q1 < 0) )
            {
               return true;
            }
         }

         return false;
      }

      //! Calculate the point where two segments intersect
      template <typename T>
         GVector2D<T> SegmentIntersection(const GVector2D<T>& in_P1, 
                                          const GVector2D<T>& in_Q1, 
                                          const GVector2D<T>& in_P2, 
                                          const GVector2D<T>& in_Q2)
      {
         // Algorithm found here : http://www.exaflop.org/docs/cgafaq/cga1.html

         T l_fDenominator = (in_Q1.x - in_P1.x) * (in_Q2.y - in_P2.y) - (in_Q1.y - in_P1.y) * (in_Q2.x - in_P2.x);
         T l_fNumerator = (in_P1.y - in_P2.y) * (in_Q2.x - in_P2.x) - (in_P1.x - in_P2.x) * (in_Q2.y - in_P2.y);

         if(l_fDenominator == 0)
         {
            return in_P1;
         }

         return in_P1 + ( (in_Q1 - in_P1) * (l_fNumerator / l_fDenominator) );
      }

      //! Clip a line in the specified window
      /*!
       * @return  0 if line is completely clipped, 1 if start point is 
       *          clipped, 2 if end point is clipped, 3 is both endpoint
       *          are clipped, 4 if no endpoint are clipped (line is entirely
       *          contained in rectangle)
       **/
      template <typename T>
      UINT32 ClipLine( GVector2D<T>& io_StartPoint, 
                       GVector2D<T>& io_EndPoint,
                       const class GRectangle<T>& in_Window)
      {
         UINT32 l_pPixels[2];
         Memory::Clear( (UINT32*) l_pPixels, 2);

         // Determine where the endpoints are located
         if(io_StartPoint.y > in_Window.Bottom() )
            l_pPixels[0] = 1;
         if(io_StartPoint.y < in_Window.Top() )
            l_pPixels[0] |= 2;
         if(io_StartPoint.x > in_Window.Right() )
            l_pPixels[0] |= 4;
         if(io_StartPoint.x < in_Window.Left() )
            l_pPixels[0] |= 8;
         if(io_EndPoint.y > in_Window.Bottom() )
            l_pPixels[1] = 1;
         if(io_EndPoint.y < in_Window.Top() )
            l_pPixels[1] |= 2;
         if(io_EndPoint.x > in_Window.Right() )
            l_pPixels[1] |= 4;
         if(io_EndPoint.x < in_Window.Left() )
            l_pPixels[1] |= 8;

         // Test if line is completely visible
         if( !(l_pPixels[0] | l_pPixels[1]) )
            return 4;

         // Test if line is completely invisible
         if(l_pPixels[0] & l_pPixels[1])
            return 0;

         UINT32 l_iReturnValue = (l_pPixels[0] ? 1 : 0) | (l_pPixels[1] ? 2 : 0);

         // Line is partially visible, clip its coordinate
         GVector2D<T>* l_pStart;
         GVector2D<T>* l_pEnd;
         if(l_pPixels[0] == 0)
         {
            l_pPixels[0] = l_pPixels[1];
            l_pPixels[1] = 0;
            l_pStart = &io_EndPoint;
            l_pEnd = &io_StartPoint;
         }
         else
         {
            l_pStart = &io_StartPoint;
            l_pEnd = &io_EndPoint;
         }

         T dx = l_pEnd->x - l_pStart->x;
         T dy = l_pEnd->y - l_pStart->y;
         if(l_pPixels[0] & 1)
         {
            l_pStart->x = l_pStart->x + (in_Window.Bottom() - l_pStart->y) * dx / dy;
            l_pStart->y = in_Window.Bottom();
         }
         if(l_pPixels[0] & 2)
         {
            l_pStart->x = l_pStart->x +(in_Window.Top() - l_pStart->y) * dx / dy;
            l_pStart->y = in_Window.Top();
         }
         if(l_pPixels[0] & 4)
         {
            l_pStart->y = l_pStart->y + (in_Window.Right() - l_pStart->x) * dy / dx;
            l_pStart->x = in_Window.Right();
         }
         if(l_pPixels[0] & 8)
         {
            l_pStart->y = l_pStart->y + (in_Window.Left() - l_pStart->x) * dy / dx;
            l_pStart->x = in_Window.Left();
         }

         if(l_pPixels[1] != 0)
         {
            ClipLine(*l_pEnd, *l_pStart, in_Window);
         }

         return l_iReturnValue;
      }

      //! Scan convert a line with integer endpoints
      template<typename TLineList, 
               typename TDrawParam, 
               void (*DrawProc) (TDrawParam in_Param, INT32 x, INT32 y), 
               bool TDrawEndpoint>
      bool ScanConvertIntegerLines(const GShape& in_LineShape, 
                                   const TLineList& in_LineList, 
                                   TDrawParam in_DrawParam)
      {
         for(UINT32 l = 0;l < in_LineList.NbLines();l ++)
         {
            GLineIndex l_Line = in_LineList.Line(l);

            const GVector3D<REAL32>& l_StartPointReal = in_LineShape.Position(l_Line.m_pVertices[0] );
            const GVector3D<REAL32>& l_EndPointReal = in_LineShape.Position(l_Line.m_pVertices[1] );

            GVector2D<INT32> l_StartPoint;
            GVector2D<INT32> l_EndPoint;

            l_StartPoint.x = RoundToInt(l_StartPointReal.x);
            l_StartPoint.y = RoundToInt(l_StartPointReal.y);

            l_EndPoint.x = RoundToInt(l_EndPointReal.x);
            l_EndPoint.y = RoundToInt(l_EndPointReal.y);

            INT32 dx = l_EndPoint.x - l_StartPoint.x;
            INT32 ax = abs(dx * 2);
            INT32 sx = dx < 0 ? -1 : 1;

            INT32 dy = l_EndPoint.y - l_StartPoint.y;
            INT32 ay = abs(dy * 2);
            INT32 sy = dy < 0 ? -1 : 1;

            INT32 x = l_StartPoint.x;
            INT32 y = l_StartPoint.y;

            if(ax > ay)
            {
               // x dominant
               INT32 d = 0 - (ax >> 1);
	            for(;x != l_EndPoint.x;)
               {
                  DrawProc(in_DrawParam, x, y);

                  d += ay;

                  if(d > 0)
                  {
                     y += sy;
                     d -= ax;
                  }

                  x += sx;
   	         }
            }
            else
            {
               // y dominant
               INT32 d = 0 - (ay >> 1);
	            for(;y != l_EndPoint.y;)
               {
                  DrawProc(in_DrawParam, x, y);

                  d += ax;

                  if(d > 0)
                  {
                     x += sx;
                     d -= ay;
                  }

                  y += sy;
   	         }
            }

            if(TDrawEndpoint)
            {
               DrawProc(in_DrawParam, x, y);
            }
         }
         return true;
      }

      //! Scan convert a line
      template<typename TLineList, 
               typename TDrawParam, 
               void (*DrawProc) (TDrawParam in_Param, INT32 x, INT32 y), 
               INT32 TSubPixelFactor>
      bool ScanConvertLines(const GShape& in_LineShape, 
                            const TLineList& in_LineList, 
                            TDrawParam in_DrawParam)
      {
         // Scan convert each line in the facet list
         for(UINT32 l = 0;l < in_LineList.NbLines();l ++)
         {
            // Fetch current line endpoints
            GLineIndex l_Line = in_LineList.Line(l);

            const GVector3D<REAL32>& l_StartPointReal = in_LineShape.Position(l_Line.m_pVertices[0] );
            const GVector3D<REAL32>& l_EndPointReal = in_LineShape.Position(l_Line.m_pVertices[1] );

            INT32 dx = RoundToInt( (l_EndPointReal.x - l_StartPointReal.x) * TSubPixelFactor);
            INT32 ax = abs(dx * 2);
            INT32 sx = dx < 0 ? -1 : 1;

            INT32 dy = RoundToInt( (l_EndPointReal.y - l_StartPointReal.y) * TSubPixelFactor);
            INT32 ay = abs(dy * 2);
            INT32 sy = dy < 0 ? -1 : 1;

            if(ax > ay)
            {
               INT32 x = (INT32) ceilf(l_StartPointReal.x * sx) * sx;
               INT32 lastx = (INT32) ceilf(l_EndPointReal.x * sx) * sx;

               if(x == lastx)
                  continue;

               INT32 y = (INT32) ceilf( (l_StartPointReal.y * sy - 0.499999f) ) * sy;

               INT32 dinit = RoundToInt( (l_StartPointReal.y - (REAL32) y) * ax) * sy - (ax >> 1);
               INT32 dinc = RoundToInt(ay * ( (REAL32) x - l_StartPointReal.x) )* sx;
               INT32 d = dinit + dinc;
               if(d > 0)
               {
                  y += sy;
                  d -= ax;
               }

               // x dominant
	            for(;x != lastx;)
               {
                  DrawProc(in_DrawParam, x, y);

                  d += ay;

                  if(d > 0)
                  {
                     y += sy;
                     d -= ax;
                  }

                  x += sx;
   	         }
            }
            else
            {
               INT32 y = (INT32) ceilf(l_StartPointReal.y * sy) * sy;
               INT32 lasty = (INT32) ceilf(l_EndPointReal.y * sy) * sy;

               if(y == lasty)
                  continue;

               INT32 x = (INT32) ceilf( (l_StartPointReal.x * sx - 0.499999f) ) * sx;

               INT32 dinit = RoundToInt( (l_StartPointReal.x - (REAL32) x) * ay) * sx - (ay >> 1);
               INT32 dinc = RoundToInt(ax * ( (REAL32) y - l_StartPointReal.y) )* sy;
               INT32 d = dinit + dinc;
               if(d > 0)
               {
                  x += sx;
                  d -= ay;
               }

               // y dominant
	            for(;y != lasty;)
               {
                  DrawProc(in_DrawParam, x, y);

                  d += ax;

                  if(d > 0)
                  {
                     x += sx;
                     d -= ay;
                  }

                  y += sy;
   	         }
            }
         }
         return true;
      }

      template<typename TDrawParam, 
               void (*DrawProc) (TDrawParam in_Param, INT32 x, INT32 y), 
               INT32 TSubPixelFactor>
      void ScanConvertLine(const GVector2D<REAL32>& in_StartPoint, 
                           GVector2D<REAL32>& in_EndPoint, 
                           TDrawParam in_DrawParam)
      {
         INT32 dx = RoundToInt( (in_EndPoint.x - in_StartPoint.x) * TSubPixelFactor);
         INT32 ax = abs(dx * 2);
         INT32 sx = dx < 0 ? -1 : 1;

         INT32 dy = RoundToInt( (in_EndPoint.y - in_StartPoint.y) * TSubPixelFactor);
         INT32 ay = abs(dy * 2);
         INT32 sy = dy < 0 ? -1 : 1;

         if(ax > ay)
         {
            INT32 x = (INT32) ceilf(in_StartPoint.x * sx) * sx;
            INT32 lastx = (INT32) ceilf(in_EndPoint.x * sx) * sx;

            if(x == lastx)
               return;

            INT32 y = (INT32) ceilf( (in_StartPoint.y * sy - 0.5f) ) * sy;

            INT32 dinit = RoundToInt( (in_StartPoint.y - (REAL32) y) * ax) * sy - (ax >> 1);
            INT32 dinc = RoundToInt(ay * ( (REAL32) x - in_StartPoint.x) )* sx;
            INT32 d = dinit + dinc;
            if(d > 0)
            {
               y += sy;
               d -= ax;
            }

            // x dominant
	         for(;x != lastx;)
            {
               DrawProc(in_DrawParam, x, y);

               d += ay;

               if(d > 0)
               {
                  y += sy;
                  d -= ax;
               }

               x += sx;
   	      }
         }
         else
         {
            INT32 y = (INT32) ceilf(in_StartPoint.y * sy) * sy;
            INT32 lasty = (INT32) ceilf(in_EndPoint.y * sy) * sy;

            if(y == lasty)
               return;

            INT32 x = (INT32) ceilf( (in_StartPoint.x * sx - 0.5f) ) * sx;

            INT32 dinit = RoundToInt( (in_StartPoint.x - (REAL32) x) * ay) * sx - (ay >> 1);
            INT32 dinc = RoundToInt(ax * ( (REAL32) y - in_StartPoint.y) )* sy;
            INT32 d = dinit + dinc;
            if(d > 0)
            {
               x += sx;
               d -= ay;
            }

            // y dominant
	         for(;y != lasty;)
            {
               DrawProc(in_DrawParam, x, y);

               d += ax;

               if(d > 0)
               {
                  x += sx;
                  d -= ay;
               }

               y += sy;
   	      }
         }
      }

      template<typename T>
      class GRectangle
      {
      public:
         T   m_Left;
         T   m_Top;
         T   m_Width;
         T   m_Height;

         GRectangle<T>()
         {
            m_Left = 0;
            m_Top = 0;
            m_Width = 0;
            m_Height = 0;
         }

         GRectangle<T>( T in_Left, T in_Top, T in_Width, T in_Height )
            : m_Left(in_Left), m_Top(in_Top)
            , m_Width(in_Width), m_Height(in_Height)
         {
         }

         GRectangle<T>( GVector2D<T>& in_TopLeft, GVector2D<T>& in_WidthHeight )
         {
            m_Left     = in_TopLeft.x;
            m_Top      = in_TopLeft.y;
            m_Width    = in_WidthHeight.x;
            m_Height   = in_WidthHeight.y;
         }

         GRectangle<T>( const GRectangle<T>& in_Other )
         {
            m_Left     = in_Other.m_Left;
            m_Top      = in_Other.m_Top;
            m_Width    = in_Other.m_Width;
            m_Height   = in_Other.m_Height;
         }

         void Set( T in_Left, T in_Top, T in_Width, T in_Height )
         {
            m_Left     = in_Left;
            m_Top      = in_Top;
            m_Width    = in_Width;
            m_Height   = in_Height;
         }

         void Set( const GVector2D<T>& in_TopLeft, const GVector2D<T> in_WidthHeight )
         {
            m_Left     = in_TopLeft.x;
            m_Top      = in_TopLeft.y;
            m_Width    = in_WidthHeight.x;
            m_Height   = in_WidthHeight.y;
         }

         T Left()   const
         {
            return m_Left;
         }

         T Top()    const
         {
            return m_Top;
         }

         T Width()  const
         {
            return m_Width;
         }

         T Height() const
         {
            return m_Height;
         }

         T Right()  const
         {
            return m_Left + m_Width;
         }

         T Bottom() const
         {
            return m_Top + m_Height;
         }

         void Left( T in_Value )
         {
            m_Left = in_Value;
         }

         void Top( T in_Value )
         {
            m_Top = in_Value;
         }

         void Width( T in_Value )
         {
            m_Width = in_Value;
         }

         void Height( T in_Value )
         {
            m_Height = in_Value;
         }

         void Right( T in_Value )
         {
            m_Width = in_Value - m_Left;
         }

         void Bottom( T in_Value )
         {
            m_Height = in_Value - m_Top;
         }

         void Inset( T in_X, T in_Y )
         {
            m_Left     += in_X;
            m_Width    -= in_X;
            m_Top      += in_Y;
            m_Height   -= in_Y;
         }

         void Inset( const GVector2D<T>& in_Inset )
         {
            m_Left     += in_Inset.x;
            m_Width    -= in_Inset.x;
            m_Top      += in_Inset.y;
            m_Height   -= in_Inset.y;
         }

         void Offset( T in_X, T in_Y )
         {
            m_Left     += in_X;
            m_Top      += in_Y;
         }

         void Offset( const GVector2D<T>& in_Offset )
         {
            m_Left     += in_Offset.x;
            m_Top      += in_Offset.y;
         }

         bool PointInRect( const GVector2D<T>& in_Point ) const
         {
            if ( in_Point.x >= Left() )
            {
               if ( in_Point.x <= Right() )
               {
                  if ( in_Point.y >= Top() )
                  {
                     if ( in_Point.y <= Bottom() )
                     {
                        return true;
                     }
                  }
               }
            }
            return false;
         }

         void Union( const GRectangle<T>& in_Other )
         {
            // not optimise at all stie!
            T l1, l2, r1, r2, t1, t2, b1, b2;

            l1 = Left();
            l2 = in_Other.Left();

            t1 = Top();
            t2 = in_Other.Top();

            r1 = Right();
            r2 = in_Other.Right();

            b1 = Bottom();
            b2 = in_Other.Bottom();

            if ( l2 < l1 ) l1 = l2;
            if ( t2 < t1 ) t1 = t2;
            if ( r2 > r1 ) r1 = r2;
            if ( b2 > b1 ) b1 = b2; 

            Set( l1, t1, r1 - l1, b1 - t1 );
         }

         GVector2D<T> Center() const
         {
            return GVector2D<T>(m_Left + m_Width / 2, m_Top + m_Height / 2);
         }

         bool Contains(const GVector2D<T>& in_Point) const
         {
            return (in_Point.x >= m_Left) && (in_Point.x <= m_Left + m_Width) &&
                   (in_Point.y >= m_Top) && (in_Point.y <= m_Top + m_Height);
         }
      };

      // Box in 3D
      template<typename T>
      class GBox
      {
      public:
         GBox<T>()
            : m_LeftTopFront(0, 0, 0)
            , m_RightBottomBack(0, 0, 0)
         {
         }

         GBox<T>( const GVector3D<T>& in_Point )
            : m_LeftTopFront(in_Point)
            , m_RightBottomBack(in_Point)
         {
         }

         GBox<T>( const GVector3D<T>& in_PointA, const GVector3D<T>& in_PointB )
         {
            m_LeftTopFront.x = min(in_PointA.x, in_PointB.x);
            m_LeftTopFront.y = min(in_PointA.y, in_PointB.y);
            m_LeftTopFront.z = min(in_PointA.z, in_PointB.z);

            m_RightBottomBack.x = max(in_PointA.x, in_PointB.x);
            m_RightBottomBack.y = max(in_PointA.y, in_PointB.y);
            m_RightBottomBack.z = max(in_PointA.z, in_PointB.z);
         } 

         GBox<T>( const GBox<T>& in_Other )
            : m_LeftTopFront(in_Other.m_LeftTopFront)
            , m_RightBottomBack(in_Other.m_RightBottomBack)
         {
         }

         T Left()   const
         {
            return m_LeftTopFront.x;
         }

         T Top()    const
         {
            return m_LeftTopFront.y;
         }

         T Front()  const
         {
            return m_LeftTopFront.z;
         }

         T Width()  const
         {
            return m_RightBottomBack.x - m_LeftTopFront.x;
         }

         T Height() const
         {
            return m_RightBottomBack.y - m_LeftTopFront.y;
         }

         T Depth()  const
         {
            return m_RightBottomBack.z - m_LeftTopFront.z;
         }

         T Right()  const
         {
            return m_RightBottomBack.x;
         }

         T Bottom() const 
         {
            return m_RightBottomBack.y;
         }

         T Back()   const
         {
            return m_RightBottomBack.z;
         }

         void Left( T in_Value )
         {
            m_LeftTopFront.x = in_Value;
         }

         void Top( T in_Value )
         {
            m_LeftTopFront.y = in_Value;
         }

         void Front( T in_Value )
         {
            m_LeftTopFront.z = in_Value;
         }

         void Width( T in_Value )
         {
            m_RightBottomBack.x = m_LeftTopFront.x + in_Value;
         }

         void Height( T in_Value )
         {
            m_RightBottomBack.y = m_LeftTopFront.y + in_Value;
         }

         void Depth( T in_Value )
         {
            m_RightBottomBack.z = m_LeftTopFront.z + in_Value;
         }

         void Right( T in_Value )
         {
            m_RightBottomBack.x = in_Value;
         }

         void Bottom( T in_Value )
         {
            m_RightBottomBack.y = in_Value;
         }

         void Back( T in_Value )
         {
            m_RightBottomBack.z = in_Value;
         }

         void Inset( const GVector3D<T>& in_Inset )
         {
            m_LeftTopFront += in_Inset;
         }

         void Offset( const GVector3D<T>& in_Offset )
         {
            m_LeftTopFront += in_Offset;
            m_RightBottomBack += in_Offset;
         }

         void Union( const GVector3D<T>& in_Point )
         {
            m_LeftTopFront.x = min(m_LeftTopFront.x, in_Point.x);
            m_LeftTopFront.y = min(m_LeftTopFront.y, in_Point.y);
            m_LeftTopFront.z = min(m_LeftTopFront.z, in_Point.z);

            m_RightBottomBack.x = max(m_RightBottomBack.x, in_Point.x);
            m_RightBottomBack.y = max(m_RightBottomBack.y, in_Point.y);
            m_RightBottomBack.z = max(m_RightBottomBack.z, in_Point.z);
         }

         void Union( const GBox<T>& in_Other )
         {
            m_LeftTopFront.x = min(m_LeftTopFront.x, in_Other.m_LeftTopFront.x);
            m_LeftTopFront.y = min(m_LeftTopFront.y, in_Other.m_LeftTopFront.y);
            m_LeftTopFront.z = min(m_LeftTopFront.z, in_Other.m_LeftTopFront.z);

            m_RightBottomBack.x = max(m_RightBottomBack.x, in_Other.m_RightBottomBack.x);
            m_RightBottomBack.y = max(m_RightBottomBack.y, in_Other.m_RightBottomBack.y);
            m_RightBottomBack.z = max(m_RightBottomBack.z, in_Other.m_RightBottomBack.z);
         }

         void Corners( GVector3D<T> out_Corners[8] ) const
         {
            out_Corners[0].x = m_LeftTopFront.x;
            out_Corners[0].y = m_LeftTopFront.y;
            out_Corners[0].z = m_LeftTopFront.z;

            out_Corners[1].x = m_LeftTopFront.x;
            out_Corners[1].y = m_LeftTopFront.y;
            out_Corners[1].z = m_RightBottomBack.z;

            out_Corners[2].x = m_LeftTopFront.x;
            out_Corners[2].y = m_RightBottomBack.y;
            out_Corners[2].z = m_LeftTopFront.z;

            out_Corners[3].x = m_LeftTopFront.x;
            out_Corners[3].y = m_RightBottomBack.y;
            out_Corners[3].z = m_RightBottomBack.z;

            out_Corners[4].x = m_RightBottomBack.x;
            out_Corners[4].y = m_LeftTopFront.y;
            out_Corners[4].z = m_LeftTopFront.z;

            out_Corners[5].x = m_RightBottomBack.x;
            out_Corners[5].y = m_LeftTopFront.y;
            out_Corners[5].z = m_RightBottomBack.z;

            out_Corners[6].x = m_RightBottomBack.x;
            out_Corners[6].y = m_RightBottomBack.y;
            out_Corners[6].z = m_LeftTopFront.z;

            out_Corners[7].x = m_RightBottomBack.x;
            out_Corners[7].y = m_RightBottomBack.y;
            out_Corners[7].z = m_RightBottomBack.z;
         }

         GVector3D<T> Center() const
         {
            return (m_LeftTopFront + m_RightBottomBack) * 0.5;
         }

         T DiagonalLength()const
         {
            return m_LeftTopFront.Distance(m_RightBottomBack);
         }

      private:
         GVector3D<T>  m_LeftTopFront;
         GVector3D<T>  m_RightBottomBack;
      };

      template <typename T>
      class GSphere
      {
      public:
         GSphere<T>( const GVector3D<T>& in_Center, T in_Radius )
         {
            m_Center = in_Center;
            m_Radius = in_Radius;
         }

         GSphere<T>( const GVector3D<T>& in_Center, const GVector3D<T>& in_Point )
         {
            m_Center = in_Center;
            m_Radius = m_Center.Distance( in_Point ); // distance from center to point
         }

         GSphere<T> ( const GSphere<T>& in_Other )
         {
            m_Center = in_Other.m_Center;
            m_Radius = in_Other.m_Radius;
         }

         void Offset( const GVector3D<T>& in_Offset )
         {
            m_Center += in_Offset;
         }

         T Radius()
         {
            return m_Radius;
         }

         GVector3D<T>  Center() const
         {
            return m_Center;
         }

         void Radius( T in_Value )
         {
            m_Radius = in_Value;
         }

         void Center( const GVector3D<T>& in_Value )
         {
            m_Center = in_Value;
         }

      private:
         GVector3D<T>  m_Center;
         T              m_Radius;
      };

      template <typename T>
      class GRay
      {
      public:
         GVector3D<T>  m_Center;
         GVector3D<T>  m_Direction;

         GRay<T>()
         {
            m_Center.Set( 0.0f, 0.0f, 0.0f );
            m_Direction.Set( 0.0f, 0.0f, 0.0f );
         }

         GRay<T>( const GVector3D<T>& in_Center, const GVector3D<T>& in_Dir ) 
            : m_Center(in_Center)
            , m_Direction(in_Dir)
         {
         }

         GRay<T>( const GRay<T>& in_Other )
         {
            m_Center = in_Other.m_Center;
            m_Direction = in_Other.m_Direction;
         }

         //! Test if this ray intersects with a plane & return the intersection point
         /*!
         * @param in_PlaneNormal    Normal of the plane
         * @param in_PlaneDistance  Distance of the plane from the origin
         * @param out_Intersection  Point where this ray intersect the plane
         * @return bool         True if the triangle intersects, false otherwise
         */
         bool IntersectPlane(const GVector3D<T>& in_PlaneNormal,
                             const T& in_PlaneDistance,
                             GVector3D<T>& out_Intersection) const
         {
            T l_Denom = in_PlaneNormal.DotProduct(m_Direction);

            // Test if ray is parallel to plane
            if(l_Denom == 0)
               return false;

            T l_Num = - (in_PlaneNormal.DotProduct(m_Center) + in_PlaneDistance);
            T l_Factor = l_Num / l_Denom;

            out_Intersection = m_Center + (m_Direction * l_Factor);

            return true;
         }

         //! Test if this ray intersects with a triangle & return the intersection parameters
         /*!
         * @param in_TriPoints  Vertices of the triangle
         * @param out_Distance  Distance of the ray origin & the triangle intersection
         * @param out_U         First barycentric coordinate of triangle
         * @param out_V         Second barycentric coordinate of triangle
         * @return bool         True if the triangle intersects, false otherwise
         */
         bool IntersectTriangle(const GVector3D<T> in_TriPoints[3],
                                T& out_Distance,
                                T& out_U,
                                T& out_V) const
         {
            // Algorithm used:
            //    Fast, minimum storage ray-triangle intersection.
            //    Tomas Möller and Ben Trumbore. 

            // Extract edges of triangle
            GVector3D<T> l_Edge1 = in_TriPoints[1] - in_TriPoints[0];
            GVector3D<T> l_Edge2 = in_TriPoints[2] - in_TriPoints[0];

            // Begin calculating determinant
            GVector3D<T> l_PVec = m_Direction.CrossProduct(l_Edge2);

            // If determinant is zero, ray lies in plane of triangle
            T l_Det = l_Edge1.DotProduct(l_PVec);

            if(l_Det == 0)
               return false;

            T l_InvDet = (T) 1 / l_Det;

            // Calculate distance from first triangle vertex ray origin
            GVector3D<T> l_TVec = m_Center - in_TriPoints[0];

            // Calculate U parameter and test bounds
            out_U = l_TVec.DotProduct(l_PVec) * l_InvDet;
            if( (out_U < (T) 0) || (out_U > (T) 1) )
               return false;

            // Prepare to test V parameter
            GVector3D<T> l_QVec = l_TVec.CrossProduct(l_Edge1);

            // Calculate V parameter and test bounds
            out_V = m_Direction.DotProduct(l_QVec) * l_InvDet;
            if( (out_V < (T) 0) || (out_U + out_V > (T) 1) )
               return false;

            // Calculate distance
            out_Distance = l_Edge2.DotProduct(l_QVec) * l_InvDet;

            return true;
         }

         //! Test if this ray intersects with a sphere & return the intersection point
         /*!
         * @param in_Radius         Radius of the sphere
         * @param out_Intersection  Point where this ray intersect the sphere
         * @return bool             True if the sphere intersects, false otherwise
         */
         bool IntersectSphere(T in_Radius,
                              GVector3D<T>& out_Intersection) const
         {
            T a = m_Direction.x * m_Direction.x +
                  m_Direction.y * m_Direction.y +
                  m_Direction.z * m_Direction.z;

            T b = 2 * m_Direction.x * m_Center.x + 
                  2 * m_Direction.y * m_Center.y + 
                  2 * m_Direction.z * m_Center.z;

            T c = m_Center.x * m_Center.x + 
                  m_Center.y * m_Center.y + 
                  m_Center.z * m_Center.z -
                  in_Radius * in_Radius;

            if( (b * b - 4 * a * c) < 0)
            {
               return false;
            }

            T t0 = (-b + (T) sqrt(b * b - 4 * a * c) ) / (2 * a);
            T t1 = (-b - (T) sqrt(b * b - 4 * a * c) ) / (2 * a);
            T t;
            if(fabs(t1) < fabs(t0) )
            {
               t = t1;
            }
            else
            {
               t = t0;
            }

            out_Intersection = m_Center + (m_Direction * t);

            return true;
         }

      };
   }
}
