/**************************************************************
*
* sp2_unit_view_camera.cpp
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

#include "../include/sp2_unit_view_camera.h"

GUnitViewCamera::GUnitViewCamera()
{
   m_pModel = NULL;
   m_pTfx = NULL;
   m_pCamera = NULL;

}

bool GUnitViewCamera::Init(CGFX_Scene_Itf* in_pCameraScene)
{
   // Make sure object is clean before initialize it
   Reset();

   // Create the GFX objects for the camera
   m_pModel    = g_Joshua.Renderer()->Create_Model();
   m_pTfx      = g_Joshua.Renderer()->Create_Transform_SRT();
   m_pCamera   = g_Joshua.Renderer()->Create_Camera();

   // Initialize the camera hierarchy
   in_pCameraScene->Get_Root_Model()->Add_Child(m_pModel);
   m_pModel->Set_Transform(m_pTfx);
   m_pModel->Set_Primitive(m_pCamera);
   m_pCamera->View_Type(GFX::VIEW_PERSPECTIVE);

   SetLimits(GBox<REAL32>(GVector3D<REAL32>(-1.f, -1.f, -1.f), GVector3D<REAL32>(1.f, 1.f, 1.f) ),
             GVector3D<REAL32>(0.f, 0.f, 0.f),
             GVector3D<REAL32>(0.f, 1.f, 0.f),
             GVector3D<REAL32>(1.f, 0.f, 0.f),
             0,
             0,
             0,
             0,
             1.f,
             1000.f);

   SetPerspective( (REAL32) Math::PI * 0.125f, 4.f / 3.f);

   return true;
}

void GUnitViewCamera::Reset()
{
   SAFE_RELEASE(m_pModel);
   SAFE_RELEASE(m_pTfx);
   SAFE_RELEASE(m_pCamera);
}

void GUnitViewCamera::SetLimits(const GBox<REAL32>& in_Bounds, 
                                const GVector3D<REAL32>& in_LookPoint,
                                const GVector3D<REAL32>& in_UpVector,
                                const GVector3D<REAL32>& in_OriginalPosition,
                                REAL32 in_fMinLongitude,
                                REAL32 in_fMaxLongitude,
                                REAL32 in_fMinLatitude,
                                REAL32 in_fMaxLatitude,
                                REAL32 in_fMinDistance,
                                REAL32 in_fMaxDistance)
{
   gassert(m_pModel, "Trying to manipulate an unexisting camera");

   m_Bounds = in_Bounds;
   m_Bounds.Corners(m_pCorners);

   // Make sure up vector is not null
   GVector3D<REAL32> l_UpVectorMod = in_UpVector;
   if( (l_UpVectorMod.x == 0) &&
       (l_UpVectorMod.y == 0) &&
       (l_UpVectorMod.z == 0) )
   {
      l_UpVectorMod.y = 1.f;
   }
   l_UpVectorMod.Normalize();

   // Make sure original position is not exactly at the look point
   GVector3D<REAL32> l_Position = in_OriginalPosition;
   if( (l_Position.x == in_LookPoint.x) &&
       (l_Position.y == in_LookPoint.y) &&
       (l_Position.z == in_LookPoint.z) )
   {
      l_Position.x += 1.f;
   }

   // Set the longitude=0 axis 
   m_Long0Vector = l_Position - in_LookPoint;
   m_Long0Vector.Normalize();

   // Make sure the axis is not colinear with up vector
   if(m_Long0Vector.CrossProduct(in_UpVector).LengthSquared() < 0.000001f)
   {
      m_Long0Vector.x = l_UpVectorMod.y;
      m_Long0Vector.y = l_UpVectorMod.z;
      m_Long0Vector.z = l_UpVectorMod.x;
   }

   m_Long0Vector = m_Long0Vector - (l_UpVectorMod * m_Long0Vector.DotProduct(l_UpVectorMod) );
   m_Long0Vector.Normalize();

   // Update the camera parameters
   m_pCamera->Look_At() = in_LookPoint;
   m_pCamera->Up_Vector() = l_UpVectorMod;

   // Setup the matrices to convert to/from the limts coordinate system
   GVector3D<REAL32> l_Out = m_Long0Vector.CrossProduct(l_UpVectorMod);
   m_BaseCoordSystem.LoadIdentity();

   m_BaseCoordSystem(0,0) = m_Long0Vector.x;
   m_BaseCoordSystem(0,1) = m_Long0Vector.y;
   m_BaseCoordSystem(0,2) = m_Long0Vector.z;
   m_BaseCoordSystem(1,0) = l_UpVectorMod.x;
   m_BaseCoordSystem(1,1) = l_UpVectorMod.y;
   m_BaseCoordSystem(1,2) = l_UpVectorMod.z;
   m_BaseCoordSystem(2,0) = l_Out.x;
   m_BaseCoordSystem(2,1) = l_Out.y;
   m_BaseCoordSystem(2,2) = l_Out.z;

   m_BaseCoordSystemInv = m_BaseCoordSystem.Transpose();

   // Copy longitude limits
   m_fMinLongitude = min(in_fMinLongitude, in_fMaxLongitude);
   m_fMaxLongitude = max(in_fMinLongitude, in_fMaxLongitude);

   // Make sure longitude limits are at a maximum distance of a complete revolution
   while( (m_fMaxLongitude - m_fMinLongitude) > 2.f * (REAL32) Math::PI)
   {
      if(fabs(m_fMinLongitude) > fabs(m_fMaxLongitude) )
      {
         m_fMinLongitude += 2.f * (REAL32) Math::PI;
      }
      else
      {
         m_fMaxLongitude -= 2.f * (REAL32) Math::PI;
      }
   }

   // Copy latitude limits
   m_fMinLatitude = max(min(in_fMinLatitude, in_fMaxLatitude), (REAL32) Math::PI * -0.49f);
   m_fMaxLatitude = min(max(in_fMinLatitude, in_fMaxLatitude), (REAL32) Math::PI * 0.49f);;

   // Copy distance limits
   m_fMinDistance = min(in_fMinDistance, in_fMaxDistance);
   m_fMaxDistance = max(in_fMinDistance, in_fMaxDistance);

   m_pCamera->Near_Plane() = m_fMinDistance;
   m_pCamera->Far_Plane() = m_fMaxDistance + m_Bounds.DiagonalLength() * 2;

   // Set a camera position that respects the current limits
   ChangePosition(l_Position);
}

const GBox<REAL32>& GUnitViewCamera::BoundingBox() const
{
   return m_Bounds;
}

GVector3D<REAL32> GUnitViewCamera::LookPoint() const
{
   return m_pCamera->Look_At();
}

GVector3D<REAL32> GUnitViewCamera::UpVector() const
{
   return m_pCamera->Up_Vector();
}

REAL32 GUnitViewCamera::MinLongitude() const
{
   return m_fMinLongitude;
}

REAL32 GUnitViewCamera::MaxLongitude() const
{
   return m_fMaxLongitude;
}

REAL32 GUnitViewCamera::MinLatitude() const
{
   return m_fMinLatitude;
}

REAL32 GUnitViewCamera::MaxLatitude() const
{
   return m_fMaxLatitude;
}

REAL32 GUnitViewCamera::MinDistance() const
{
   return m_fMinDistance;
}

REAL32 GUnitViewCamera::MaxDistance() const
{
   return m_fMaxDistance;
}

GVector3D<REAL32> GUnitViewCamera::Position() const
{
   return m_pTfx->Translation();
}

void GUnitViewCamera::ChangePosition(const GVector3D<REAL32>& in_NewPosition)
{
   // Fetch spherical camera coordinates
   GVector3D<REAL32> l_SphereCoord = CartesianToSpherical(in_NewPosition - m_pCamera->Look_At() );

   // Make sure the distance is within limits
   l_SphereCoord.z = max(m_fMinDistance, min(m_fMaxDistance, l_SphereCoord.z) );

   // Make sure rotation is within bounds
   while(l_SphereCoord.x > (m_fMinLongitude + 2 * (REAL32) Math::PI) )
   {
      l_SphereCoord.x -= 2 * (REAL32) Math::PI;
   }

   while(l_SphereCoord.x < (m_fMaxLongitude - 2 * (REAL32) Math::PI) )
   {
      l_SphereCoord.x += 2 * (REAL32) Math::PI;
   }

   l_SphereCoord.x = max(m_fMinLongitude, min(m_fMaxLongitude, l_SphereCoord.x) );
   l_SphereCoord.y = max(m_fMinLatitude, min(m_fMaxLatitude, l_SphereCoord.y) );

   // Adjust distance so it is not inside the bounds
   GVector3D<REAL32> l_CamPos = SphericalToCartesian(l_SphereCoord) + m_pCamera->Look_At();
   AdjustCameraPosition(l_CamPos);

   // Change the camera position
   ModifyCameraPosition(l_CamPos);
}

void GUnitViewCamera::OffsetOrientation(REAL32 in_fLongitudeOffset, REAL32 in_fLatitudeOffset)
{
   // Fetch spherical camera coordinates
   GVector3D<REAL32> l_SphereCoord = CartesianToSpherical( (GVector3D<REAL32>) m_pTfx->Translation() - (GVector3D<REAL32>) m_pCamera->Look_At() );

   // Offset rotation
   l_SphereCoord.x += in_fLongitudeOffset;
   l_SphereCoord.y += in_fLatitudeOffset;

   // Make sure rotation is within bounds
   while(l_SphereCoord.x > (m_fMinLongitude + 2 * (REAL32) Math::PI) )
   {
      l_SphereCoord.x -= 2 * (REAL32) Math::PI;
   }

   while(l_SphereCoord.x < (m_fMaxLongitude - 2 * (REAL32) Math::PI) )
   {
      l_SphereCoord.x += 2 * (REAL32) Math::PI;
   }

   l_SphereCoord.x = max(m_fMinLongitude, min(m_fMaxLongitude, l_SphereCoord.x) );
   l_SphereCoord.y = max(m_fMinLatitude, min(m_fMaxLatitude, l_SphereCoord.y) );

   // Adjust distance so it is not inside the bounds
   GVector3D<REAL32> l_CamPos = SphericalToCartesian(l_SphereCoord) + m_pCamera->Look_At();
   AdjustCameraPosition(l_CamPos);

   // Change the camera position
   ModifyCameraPosition(l_CamPos);
}

void GUnitViewCamera::OffsetDistance(REAL32 in_fDistanceOffset)
{
   // Fetch spherical camera coordinates
   GVector3D<REAL32> l_SphereCoord = CartesianToSpherical( (GVector3D<REAL32>) m_pTfx->Translation() - (GVector3D<REAL32>) m_pCamera->Look_At() );

   // Offset the distance
   l_SphereCoord.z += in_fDistanceOffset;

   // Make sure the distance is within limits
   l_SphereCoord.z = max(m_fMinDistance, min(m_fMaxDistance, l_SphereCoord.z) );

   // Adjust distance so it is not inside the bounds
   GVector3D<REAL32> l_CamPos = SphericalToCartesian(l_SphereCoord) + m_pCamera->Look_At();
   AdjustCameraPosition(l_CamPos);

   // Change the camera position
   ModifyCameraPosition(l_CamPos);
}

void GUnitViewCamera::OffsetDistanceRelative(REAL32 in_fDistanceRatio)
{
   OffsetDistance(in_fDistanceRatio * (m_fMaxDistance - m_fMinDistance) );
}

void GUnitViewCamera::SetPerspective(REAL32 in_fFOV, REAL32 in_fAspectRatio)
{
   // Set the camera perspective parameters
   m_pCamera->FOV() = in_fFOV;
   m_pCamera->Aspect_Ratio() = in_fAspectRatio;

   // Update the camera frustrum (in camera space) from its projection matrix
   GMatrix4x4<REAL32> l_Proj = m_pCamera->Get_Projection_Matrix();

   m_Frustrum[0].m_Normal.x   = l_Proj(0, 3) + l_Proj(0, 0);
   m_Frustrum[0].m_Normal.y   = l_Proj(1, 3) + l_Proj(1, 0);
   m_Frustrum[0].m_Normal.z   = l_Proj(2, 3) + l_Proj(2, 0);
   m_Frustrum[0].m_Distance   = l_Proj(3, 3) + l_Proj(3, 0);

   m_Frustrum[1].m_Normal.x   = l_Proj(0, 3) - l_Proj(0, 0);
   m_Frustrum[1].m_Normal.y   = l_Proj(1, 3) - l_Proj(1, 0);
   m_Frustrum[1].m_Normal.z   = l_Proj(2, 3) - l_Proj(2, 0);
   m_Frustrum[1].m_Distance   = l_Proj(3, 3) - l_Proj(3, 0);

   m_Frustrum[2].m_Normal.x   = l_Proj(0, 3) + l_Proj(0, 1);
   m_Frustrum[2].m_Normal.y   = l_Proj(1, 3) + l_Proj(1, 1);
   m_Frustrum[2].m_Normal.z   = l_Proj(2, 3) + l_Proj(2, 1);
   m_Frustrum[2].m_Distance   = l_Proj(3, 3) + l_Proj(3, 1);

   m_Frustrum[3].m_Normal.x   = l_Proj(0, 3) - l_Proj(0, 1);
   m_Frustrum[3].m_Normal.y   = l_Proj(1, 3) - l_Proj(1, 1);
   m_Frustrum[3].m_Normal.z   = l_Proj(2, 3) - l_Proj(2, 1);
   m_Frustrum[3].m_Distance   = l_Proj(3, 3) - l_Proj(3, 1);
}

void GUnitViewCamera::SetPerspectiveFromViewportSize(const GVector2D<REAL32> in_Viewport)
{
   CGFX_Renderer_Init_Mode l_Mode;
   g_Joshua.Renderer()->Get_Current_Mode(&l_Mode);

   SetPerspective( (REAL32) Math::PI * 0.25f * in_Viewport.x / (REAL32) l_Mode.m_Resolution.x, 
                   in_Viewport.x / in_Viewport.y);
}

REAL32 GUnitViewCamera::FOV() const
{
   return m_pCamera->FOV();
}

REAL32 GUnitViewCamera::AspectRatio() const
{
   return m_pCamera->Aspect_Ratio();
}

GVector3D<REAL32> GUnitViewCamera::SphericalToCartesian(const GVector3D<REAL32>& in_SphereCoord) const
{
   GVector3D<REAL32> l_BaseCartesianCoord;
   l_BaseCartesianCoord.x = cosf(in_SphereCoord.x) * cosf(in_SphereCoord.y) * in_SphereCoord.z;
   l_BaseCartesianCoord.y = sinf(in_SphereCoord.y) * in_SphereCoord.z;
   l_BaseCartesianCoord.z = sinf(in_SphereCoord.x) * cosf(in_SphereCoord.y) * in_SphereCoord.z;

   return m_BaseCoordSystem * l_BaseCartesianCoord;
}

GVector3D<REAL32> GUnitViewCamera::CartesianToSpherical(const GVector3D<REAL32>& in_Coord) const
{
   GVector3D<REAL32> l_BaseSphereCoord = m_BaseCoordSystemInv * in_Coord;

   GVector3D<REAL32> l_Result;
   l_Result.z = l_BaseSphereCoord.Length();
   l_BaseSphereCoord /= l_Result.z;
   l_Result.x = atan2f(l_BaseSphereCoord.z, l_BaseSphereCoord.x);
   l_Result.y = asinf(l_BaseSphereCoord.y);

   return l_Result;
}

void GUnitViewCamera::AdjustCameraPosition(GVector3D<REAL32>& io_CamPosition)
{
   // Update the camera position
   bool l_bReadjust = false;
   REAL32 l_fDist = io_CamPosition.Distance(m_pCamera->Look_At() );
   REAL32 l_fDistOffset = 0;

   if(l_fDist == 0)
   {
      io_CamPosition = (m_Long0Vector * 0.001f) + io_CamPosition;
   }

   GMatrix4x4<REAL32> l_PosMat;
   l_PosMat.LoadIdentity();
   l_PosMat(3,0) = io_CamPosition.x;
   l_PosMat(3,1) = io_CamPosition.y;
   l_PosMat(3,2) = io_CamPosition.z;

   GMatrix4x4<REAL32> l_ViewMat = m_pCamera->Get_View_Matrix(l_PosMat);

   // Test if each corner of the box is in front of the current near clipping distance
   for(UINT32 i = 0;i < 8;i ++)
   {
      GVector3D<REAL32> l_ViewCorner = l_ViewMat * m_pCorners[i];
      if( (l_ViewCorner.z) < m_pCamera->Near_Plane() )
      {
         l_bReadjust = true;
         l_fDistOffset = max(l_fDistOffset, m_pCamera->Near_Plane() - l_ViewCorner.z);
      }
   }

   // Adjust the camera position if necessary
   if(l_bReadjust)
   {
      l_fDist += l_fDistOffset;

      io_CamPosition -= m_pCamera->Look_At();

      io_CamPosition.Normalize();

      io_CamPosition *= l_fDist;

      io_CamPosition += m_pCamera->Look_At();
   }
}

void GUnitViewCamera::ModifyCameraPosition(const GVector3D<REAL32>& in_CamPosition)
{
   m_pTfx->Translation() = in_CamPosition;

   m_View = m_pCamera->Get_View_Matrix(m_pTfx->Get_Matrix() );
}
