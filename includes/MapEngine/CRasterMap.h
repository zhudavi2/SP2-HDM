/**************************************************************
*
* CRasterMap.h
*
* Description
* ===========
*  Class Map::GRasterMap description & implementation
*
* Owner
* =====
*  Francis Jackson & Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CLASS_RASTER_MAP
#define _CLASS_RASTER_MAP


#include "CRasterView.h"
#include "CIndexedInputFileStream.h"


namespace Map 
{

   // XML tags found in config file
   namespace RasterMapXmlTags
   {
      const GString RASTER_MAP_CONFIG = "RASTER_MAP_CONFIG";
      const GString NUM_ROWS          = "NUM_ROWS";
      const GString NUM_COLS          = "NUM_COLS";
      const GString X_PERIODIC        = "X_PERIODIC";
      const GString Y_PERIODIC        = "Y_PERIODIC";
      const GString DATA_FILE_NAME    = "DATA_FILE_NAME";
      const GString INDEX_FILE_NAME   = "INDEX_FILE_NAME";
      const GString ROWS_PER_TILE     = "ROWS_PER_TILE";
      const GString COLS_PER_TILE     = "COLS_PER_TILE";
      const GString GEO_DOMAIN        = "GEO_DOMAIN";
      const GString X_MIN             = "X_MIN";
      const GString X_MAX             = "X_MAX";
      const GString Y_MIN             = "Y_MIN";
      const GString Y_MAX             = "Y_MAX";
   };

   const int c_iDefaultMaxTileCount = 128;

   class GRasterMap
   {
      friend class GRasterView;

   public:
      GRasterMap();
      virtual ~GRasterMap();

      // Initialize map properties from XML configuration file, set message logger
      virtual bool Init( const GString& in_sConfigFileName, 
                         GFileManager* in_pFileManager,
                         UINT32 in_iMaxNbTiles = c_iDefaultMaxTileCount, 
                         GLogger* in_pLogger = NULL );

      // reset the map
      virtual void Clear();

      // close loaded views
      void CloseView( GRasterView* io_pView );
      void CloseAllViews();

      // get domain information
      const GBox& GeoDomain() const;

      // get periodicity attributes
      bool PeriodicX() const;
      bool PeriodicY() const;

      // get pointer to current message logger
      GLogger* Logger() const;

      // set current message logger
      void Logger( GLogger* in_pLogger );

      // write message to log
      void Log( const GString& in_sMessage, EMessageType in_iMessageType = MSGTYPE_MSG );

   protected:
      // message logger
      GLogger* m_pLogger;

      // dimensions of map (in pixels)
      UINT32 m_iNumRows;
      UINT32 m_iNumCols;

      // periodicity in x and y
      bool m_bPeriodicX;
      bool m_bPeriodicY;

      // dimensions of map (in real-world coordinates), 
      // upper-left pixel is at (m_GeoDomain.MinX, m_GeoDomain.MaxY)
      GBox m_GeoDomain;

      // variables for the conversion between real units and pixels
      REAL64 m_fPixelHeight;
      REAL64 m_fPixelWidth;

      // list of tiles referenced by the views
      hash_map<GPixelCoord, GRasterTile*> m_Tiles;
      UINT32 m_iMaxTileCount;

      // list of currently loaded views
      set<GRasterView*> m_Views;

      // tile information
      UINT32  m_iRowsPerTile;
      UINT32  m_iColsPerTile;

      // indexed input file stream to retrieve data from raster file
      GIndexedInputFileStream m_DataFileStream;

      // load view within specified limits
      GRasterView* OpenView(  REAL32 in_fCenterX,
                              REAL32 in_fCenterY,
                              REAL32 in_fWidth,
                              REAL32 in_fHeight );
      GRasterView* OpenView(  const GPoint& in_CenterPoint,
                              REAL32 in_fWidth,
                              REAL32 in_fHeight );

      // coordinate manipulations
      GRasterCoord   PointToRasterCoord( const GPoint& in_Point ) const;
      GPixelCoord    RasterCoordToPixelCoord( const GRasterCoord& in_RasterCoord ) const;
      GPixelCoord    PointToPixelCoord( const GPoint& in_Point ) const;
      GPixelCoord    PixelCoordToTileCoord( const GPixelCoord& in_PixelCoord ) const;
      GPixelCoord    RasterCoordToTileCoord( const GRasterCoord& in_RasterCoord ) const;
      UINT32         TileCoordToTileNumber( const GPixelCoord& in_TileCoord ) const;

      // view management
      virtual GRasterView* CreateView( const GRasterCoord& in_ULCorner, const GRasterCoord& in_LRCorner) = 0;
      void FillView( GRasterView& io_View, 
                     const GPixelCoord& in_UpperLeftTile, 
                     const GPixelCoord& in_LowerRightTile );
      GRasterTile* LocateTile( const GPixelCoord& in_TileCoord );
      void DeleteTile(GRasterTile* in_pTile);

      // tile navigation
      bool LeftTile( const GPixelCoord& in_Start, GPixelCoord& out_Destination ) const;
      bool RightTile( const GPixelCoord& in_Start, GPixelCoord& out_Destination ) const;
      bool UpperTile( const GPixelCoord& in_Start, GPixelCoord& out_Destination ) const;
      bool LowerTile( const GPixelCoord& in_Start, GPixelCoord& out_Destination ) const;

      // pixel navigation
      bool LeftPixel( const GPixelCoord& in_Start, GPixelCoord& out_Destination ) const;
      bool RightPixel( const GPixelCoord& in_Start, GPixelCoord& out_Destination ) const;
      bool UpperPixel( const GPixelCoord& in_Start, GPixelCoord& out_Destination ) const;
      bool LowerPixel( const GPixelCoord& in_Start, GPixelCoord& out_Destination ) const;
   };

}

#endif