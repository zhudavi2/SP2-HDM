/**************************************************************
*
* golem_geometry_plane.h
*
* Description
* ===========
*  Implementation of class Geometry::GPlane, which describes a plane in
*  three dimensionnal space.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_GEOMETRY_PLANE_
#define _GOLEM_GEOMETRY_PLANE_

#include "golem_vector.h"

namespace Hector
{
   namespace Geometry
   {
      /*!
       * Describes a 3D plane geometric object. Internally represented
       * using the 0 = Ax + By + Cz + D equation, where A, B & C form
       * the plane normal & D is the plane distance to the world origin.
       */
      template <typename T>
      class GPlane
      {
      public:
         //! Normal of the plane
         GVector3D<T>   m_Normal;

         //! Origin distance of the plane
         T              m_Distance;

         //! Construct an uninitialized plane
         GPlane()
         {
         }

         //! Construct a plane from another
         GPlane(const GPlane<T>& in_Copy)
         {
            *this = in_Copy;
         }

         //! Contruct a plane from its normal & distance
         GPlane(const GVector3D<T> in_PlaneNormal, T in_PlaneDistance)
         {
            m_Normal = in_PlaneNormal;
            m_Distance = in_PlaneDistance;
         }

         //! Construct a plane from three clockwise 3D points
         GPlane(const GVector3D<T>& in_PointA,
                const GVector3D<T>& in_PointB,
                const GVector3D<T>& in_PointC)
         {
            m_Normal = (in_PointB - in_PointA).CrossProduct(in_PointB - in_PointC);
            m_Distance = in_PointB.DotProduct(m_Normal);
         }

         //! Normalize the plane equation
         void Normalize()
         {
            T l_LengthInv = (T) 1 / m_Normal.Length();
            m_Normal *= l_LengthInv;
            m_Distance *= l_LengthInv;
         }
      };
   }
}

#endif // #ifndef _GOLEM_GEOMETRY_PLANE_
