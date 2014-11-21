/**************************************************************
*
* sp2_unit_view_camera.h
*
* Description
* ===========
*  [A quick overview of the content in both files (.h and .cpp)]
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef __SP2_UNIT_VIEW_CAMERA_H__
#define __SP2_UNIT_VIEW_CAMERA_H__

namespace SP2
{
   /*!
    * This class is used to look at three dimensionnal objects
    * that has a limited bounding box and to be able to move the camera
    * aroundthose objects. The class controls the camera GFX objects
    * allocation & utilisations.
    **/
   class GUnitViewCamera
   {
   public:
      GUnitViewCamera();

      //! Initialize the object
      /*!
       * The object uses the input scene to create & attach itself. It
       * will create a model that will be parented at the root of the scene.
       * @param in_pCameraScene :   Scene where to create the camera GFX objects.
       * @return A boolean indicating if initialization was a success.
       **/
      bool Init(CGFX_Scene_Itf* in_pCameraScene);

      //! Reset the object
      /*!
       * Destroy all objects used by this object.
       **/
      void Reset();

      //! Change the limits in which the camera is allowed to move.
      /*!
       * The defaults limits are a camera unbale to be anywhere else
       * than unit away from 0,0,0 on the x axis.
       * @param in_Bounds           : Box that must always be seen from the camera.
       * @param in_LookPoint        : Point that the camera always look within the box.
       * @param in_UpVector         : Orientation of the camera
       * @param in_OriginalPosition : Position that will be used as reference for longitude 0 (and where camera will be located first)
       * @param in_fMinLongitude    : Lower bound of valid longitude
       * @param in_fMaxLongitude    : Upper bound of valid longitude
       * @param in_fMinLatitude     : Lower bound of valid latitude
       * @param in_fMaxLatitude     : Upper bound of valid latitude
       * @param in_fMinDistance     : Lower bound of valid camera distance
       * @param in_fMaxDistance     : Upper bound of valid camera distance
       **/
      void SetLimits(const GBox<REAL32>& in_Bounds, 
                     const GVector3D<REAL32>& in_LookPoint,
                     const GVector3D<REAL32>& in_UpVector,
                     const GVector3D<REAL32>& in_OriginalPosition,
                     REAL32 in_fMinLongitude,
                     REAL32 in_fMaxLongitude,
                     REAL32 in_fMinLatitude,
                     REAL32 in_fMaxLatitude,
                     REAL32 in_fMinDistance,
                     REAL32 in_fMaxDistance);

      //! Fetch the current camera bounding box
      const GBox<REAL32>& BoundingBox() const;

      //! Fetch the current camera look point
      GVector3D<REAL32> LookPoint() const;

      //! Fetch the current camera up vector (orientation)
      GVector3D<REAL32> UpVector() const;

      //! Fetch the current camera minimum longitude
      REAL32 MinLongitude() const;

      //! Fetch the current camera maximum longitude
      REAL32 MaxLongitude() const;

      //! Fetch the current camera minimum latitude
      REAL32 MinLatitude() const;

      //! Fetch the current camera maximum latitude
      REAL32 MaxLatitude() const;

      //! Fetch the current camera minimum longitude
      REAL32 MinDistance() const;

      //! Fetch the current camera maximum longitude
      REAL32 MaxDistance() const;

      //! Fetch the current camera position
      GVector3D<REAL32> Position() const;

      //! Change the position of the camera.
      /*!
       * If this position is out of range, it modified
       * to be in the specified valid range.
       * @param in_NewPosition   : New position for the camera
       **/
      void ChangePosition(const GVector3D<REAL32>& in_NewPosition);

      //! Offset the current latitude & longitude of the camera
      /*!
       * If offset goes beyond current limits, the value is clipped to the limit.
       * @param in_fLongitudeOffset : Offset to the current longitude.
       * @param in_fLongitudeOffset : Offset to the current latitude.
       **/
      void OffsetOrientation(REAL32 in_fLongitudeOffset, REAL32 in_fLatitudeOffset);

      //! Offset the current distance of the camera fromt the look point
      /*!
       * If offset goes beyond current limits, the value is clipped to the limit.
       * @param in_fDistanceOffset  : Offset to the current distance.
       **/
      void OffsetDistance(REAL32 in_fDistanceOffset);

      //! Offset the current distance of the camera fromt the look point using a ration of the distance limits
      /*!
       * If offset goes beyond current limits, the value is clipped to the limit.
       * @param in_fDistanceRatio   : Offset to the current distance, between 0 and 1.
       **/
      void OffsetDistanceRelative(REAL32 in_fDistanceRatio);

      //! Change the camera field of view & aspect ratio
      /*!
       * @param in_fFOV          : Field of view for the camera, in radians
       * @param in_fAspectRatio  : Aspect ratio of the camera
       **/
      void SetPerspective(REAL32 in_fFOV, REAL32 in_fAspectRatio);

      //! Change the camera field of view & aspect ratio using the specified viewport
      /*!
       * @param in_Viewport      : Viewport size used to comput FOV & apsect ratio. 
       **/
      void SetPerspectiveFromViewportSize(const GVector2D<REAL32> in_Viewport);

      //! Fetch the current camera field of view
      REAL32 FOV() const;

      //! Fetch the current camera aspect ratio
      REAL32 AspectRatio() const;

   private:
      // Get the current camera position using its spherical coordinates
      GVector3D<REAL32> SphericalToCartesian(const GVector3D<REAL32>& in_SphereCoord) const;

      // Get the current camera spherical coordinates using its position
      GVector3D<REAL32> CartesianToSpherical(const GVector3D<REAL32>& in_Coord) const;

      // Adjust the specified camera position so the camera sees all the bounds
      void AdjustCameraPosition(GVector3D<REAL32>& io_CamPosition);

      // Update the position of the camera
      void ModifyCameraPosition(const GVector3D<REAL32>& in_CamPosition);

      CGFX_Model_Itf*         m_pModel;
      CGFX_Transform_SRT_Itf* m_pTfx;
      CGFX_Camera_Itf*        m_pCamera;

      GBox<REAL32>            m_Bounds;
      GVector3D<REAL32>       m_pCorners[8];
      GVector3D<REAL32>       m_Long0Vector;

      GMatrix4x4<REAL32>      m_BaseCoordSystem;
      GMatrix4x4<REAL32>      m_BaseCoordSystemInv;

      REAL32                  m_fMinLongitude;
      REAL32                  m_fMaxLongitude;
      REAL32                  m_fMinLatitude;
      REAL32                  m_fMaxLatitude;
      REAL32                  m_fMinDistance;
      REAL32                  m_fMaxDistance;

      GPlane<REAL32>          m_Frustrum[4];
      GMatrix4x4<REAL32>      m_View;
   };
}

#endif // #ifndef __SP2_UNIT_VIEW_CAMERA_H__
