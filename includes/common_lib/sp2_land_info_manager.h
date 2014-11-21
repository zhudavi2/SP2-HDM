/**************************************************************
*
* sp2_land_info_manager.h
*
* Description
* ===========
*  Describes & implements the SP2::GLandInfoManager class
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef __COMMON_LIB_SP2_LAND_INFO_MANAGER_H__
#define __COMMON_LIB_SP2_LAND_INFO_MANAGER_H__

namespace SP2
{
   struct GLandInfo 
   {
      INT16    m_iElev;
      UINT8    m_iCountry;
      UINT8    m_iType:7;
      UINT8    m_iRoad:1;

      static const OCEAN = 0;
      static const RIVER = 0x7F;
      static const WATER = 0x7E;
   };

   /*!
    * Class that manages the data related with the land information
    **/
   class GLandInfoData
   {
      friend class GLandInfoManager;
   private:
      // Private constructor/destructor so it can only be created/destroyed by the land info manager
      GLandInfoData();
      ~GLandInfoData();

      GLandInfo*           m_pData;
      GRectangle<REAL32>   m_Location;
      GVector2D<REAL32>    m_Resolution;
      GVector2D<INT32>     m_ValidRange;
      UINT32               m_iPitch;

   public:
      /*!
       * \brief Return the data at the specifed integer offset in the buffer
       *
       * \param in_iOffsetX      X offset in the buffer.
       * \param in_iOffsetY      Y offset in the buffer.
       *
       * \return A pointer to the desired land information
       */
      const GLandInfo*     DataAt(INT32 in_iOffsetX = 0, INT32 in_iOffsetY = 0) const;

      /*!
       * \brief Return the data at the specifed coordinate
       *
       * \param in_Coordinate    Desired coordinate in latitude/longitude.
       *
       * \return A pointer to the desired land information
       */
      const GLandInfo*     DataAt(GVector2D<REAL32> in_Coordinate) const;

      /*!
       * \brief Return the pitch of the buffer
       */
      UINT32               Pitch() const;

      /*!
       * \brief Return the location area described by this object
       *
       * \return A rectangle that limits this object location area
       */
      const GRectangle<REAL32>&  Location() const;

      /*!
       * \brief Return the range of values that can be accessed in the buffer
       *
       * \return A 2D vector of the integer range in the buffer
       */
      const GVector2D<INT32>&    Range() const;

      /*!
       * \brief Return the resolution of a buffer increment
       *
       * \return A 2D vector containing the resolution
       */
      const GVector2D<REAL32>&   Resolution() const;

      /*!
       * \brief Return the offset X and Y in the buffer for the given Coordinates (Lat Long)
       *       
       * \param GVector2D<REAL32> Coordinates where we need the offset
       * \return A GVector2D<UINT32> containing the offsets
       **/
      GVector2D<UINT32> OffsetAt(const GVector2D<REAL32>& in_Coordinates);

      /*!
       * \brief Return the Coordinates (Latitude/Longitude) at the given offset
       *       
       * \param UINT32 Offset X
       * \param UINT32 Offset Y
       * \return A GVector2D<UINT32> containing the offsets
       **/
      GVector2D<REAL32> CoordinateAt(const UINT32 in_iOffsetX,const UINT32 in_iOffsetY);


   };

   class GLandInfoManager
   {
   public:
      //! Constructor
      GLandInfoManager();

      //! Destructor
      ~GLandInfoManager();

      //! Initialize the land information manager.
      bool Init(class GDALInterface* in_pDAL, REAL32 in_fAspectRatio);

      //! Shutdown the land information manager.
      bool Shutdown();

      /*!
       * \brief Load and return land information situated at the specified coordinate.
       *
       * If the center is near a pole, it will be offset so the pole will 
       * be the maximum/minimum point in the buffer.
       *
       * \param in_CoordCenter      Coordinate of the center of the desired area.
       * \param in_Size             Size of the desired area.
       *
       * \return A land information data object or NULL if the function failed.
       */
      GLandInfoData* LandInformationData(GVector2D<REAL32> in_CoordCenter,
                                         GVector2D<REAL32> in_Size);

      /*!
       * \brief Free a previously allocated land information data object.
       *
       * \param in_pLandData        Land info Object to free.
       *
       * \return A boolean indicating the success of the operation.
       */
      bool LandInformationDataFree(GLandInfoData* in_pLandData);

      /*!
       * \brief Get the resolution (nb of pixels if the whole planet was included)
       *
       * \param void
       * \return GVector2D<UINT32> The resolution
       **/
      const GVector2D<UINT32>& Resolution();

   private:
      static void SpanLand(void* in_pBuf, INT32 y, INT32 xl, INT32 xr);
      static void SpanWater(void* in_pBuf, INT32 y, INT32 xl, INT32 xr);

      static void DrawRiver(GLandInfoData* in_pInfo, INT32 x, INT32 y);
      static void DrawRoad(GLandInfoData* in_pInfo, INT32 x, INT32 y);

      class GDALInterface*          m_pDAL;
      REAL32                        m_fAspectRatio;

      GVector2D<UINT32>             m_Resolution;

      GVector2D<REAL32>             m_ResolutionScale;
      GVector2D<REAL32>             m_ResolutionOffset;

      GVector2D<REAL32>             m_CoordScale;
      GVector2D<REAL32>             m_CoordOffset;
   };
};

#endif //__COMMON_LIB_SP2_LAND_INFO_MANAGER_H__