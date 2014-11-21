/**************************************************************
*
* GContinuousRasterMap.h
*
* Description
* ===========
*  Class Map::GContinuousRasterMap description & implementation
*
* Owner
* =====
*  Francis Jackson & Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CLASS_CONTINUOUS_RASTER_MAP
#define _CLASS_CONTINUOUS_RASTER_MAP

#include "CRasterMap.h"
#include "GContinuousRasterView.h"


namespace Map
{

   // XML tags found in config file
   namespace ContinuousRasterMapXmlTags
   {
      const GString ZERO_LEVEL        = "ZERO_LEVEL";
      const GString LEVEL_INCREMENT   = "LEVEL_INCREMENT";
   };


   class GContinuousRasterMap : public GRasterMap
   {
      friend class GContinuousRasterView;

   public:
      GContinuousRasterMap();
      ~GContinuousRasterMap();

      // Initialize map properties from XML configuration file, set message logger
      bool Init( const GString& in_sConfigFileName, 
                  GFileManager* in_pFileManager,
                 UINT32 in_iMaxNbTiles = c_iDefaultMaxTileCount, 
                 GLogger* in_pLogger = NULL );

      // reset the map
      void Clear();

      // load view within specified limits
      GContinuousRasterView* OpenView( REAL32 in_fCenterX,
                                       REAL32 in_fCenterY,
                                       REAL32 in_fWidth,
                                       REAL32 in_fHeight );
      GContinuousRasterView* OpenView( const GPoint& in_CenterPoint,
                                       REAL32 in_fWidth,
                                       REAL32 in_fHeight );

   private:
      // variables used to translate the UINT8 raster data to true REAL32 values
      REAL32 m_pLookupTable[ MAX_UINT8 + 1 ];  // contains the REAL32 value of each level

      // create a view
      GRasterView* CreateView( const GRasterCoord& in_ULCorner, const GRasterCoord& in_LRCorner);
   };
};


#endif