#ifndef _CLASS_VECTOR_MAP
#define _CLASS_VECTOR_MAP


namespace Map 
{

   // XML tags found in config file
   namespace VectorMapXmlTags
   {
      const GString VECTOR_MAP_CONFIG     = "VECTOR_MAP_CONFIG";
      const GString GEO_DOMAIN            = "GEO_DOMAIN";
      const GString X_MIN                 = "X_MIN";
      const GString X_MAX                 = "X_MAX";
      const GString Y_MIN                 = "Y_MIN";
      const GString Y_MAX                 = "Y_MAX";
      const GString X_PERIODIC            = "X_PERIODIC";
      const GString Y_PERIODIC            = "Y_PERIODIC";
      const GString DATA_FILES            = "DATA_FILES";
      const GString OFFSET_INDEX_FILES    = "OFFSET_INDEX_FILES";
      const GString NUM_CELL_ROWS         = "NUM_CELL_ROWS";
      const GString NUM_CELL_COLS         = "NUM_CELL_COLS";
      const GString SPATIAL_INDEX_FILE    = "SPATIAL_INDEX_FILE";
   };


   // forward declarations
   class GVectorView;


   class GVectorMap
   {
   public:
      GVectorMap();
      virtual ~GVectorMap();

      // Initialize map properties from XML configuration file, set message logger
      virtual bool Init( const GString& in_sConfigFileName,GFileManager* in_pFileManager, GLogger* in_pLogger = NULL);

      // reset the map
      virtual void Clear();

      // get number of features in map
      virtual UINT32 NbFeatures() const = 0;

      // open view within specified limits (or no limits at all if no parameters)
      virtual GVectorView* OpenView();
      virtual GVectorView* OpenView( REAL32 in_fCenterX, REAL32 in_fCenterY, REAL32 in_fWidth, REAL32 in_fHeight );
      virtual GVectorView* OpenView( const GPoint& in_CenterPoint, REAL32 in_fWidth, REAL32 in_fHeight );

      // open view containing specific features
      virtual GVectorView* OpenView( ID in_iFeatureId );
      virtual GVectorView* OpenView( const vector<ID>& in_iFeatureIdList );

      // close views
      void CloseView( GVectorView* io_pView );
      void CloseAllViews();

      // get domain information
      const GBox& GeoDomain() const;

      // get periodicity attributes
      bool PeriodicX() const;
      bool PeriodicY() const;

      // bring any point into map domain
      GPoint BringPointInMap( const GPoint& in_Point ) const;

      // clip given rectangle to limits of map
      void ClipRectangleInGeoDomain( const GPoint& in_Center, REAL32 in_fWidth, REAL32 in_fHeight, vector<GBox>& out_Boxes ) const;

      // get pointer to current message logger
      GLogger* Logger() const;

      // set current message logger
      void Logger( GLogger* in_pLogger );

      // write message to log
      void Log( const GString& in_sMessage, EMessageType in_iMessageType = MSGTYPE_MSG );

   protected:
      // message logger
      GLogger* m_pLogger;

      // File Manager to support multimod
      GFileManager* m_pFileManager;

      // extent of map
      GBox m_GeoDomain;

      // periodicity of x and y coordinates
      bool m_bPeriodicX;
      bool m_bPeriodicY;

      // spatial index that lists which features are visible in each rectangular cell
      GSpatialIndex   m_SpatialIndex;
      REAL32          m_fCellWidth;
      REAL32          m_fCellHeight;

      // list of currently loaded views
      set<GVectorView*> m_Views;

      // find the cell that contains specified point
      GCellPosition PointToCellPosition( const GPoint& in_Point ) const;

      // get contents of cell
      const GSpatialIndexCell& Cell( const GCellPosition& in_CellPosition ) const;
      const GSpatialIndexCell& Cell( const GPoint& in_Point ) const;

      // load the spatial index
      bool LoadSpatialIndex( const GString& in_sFileName, UINT32 in_iNbCellRows, UINT32 in_iNbCellCols );

      // select the features visible in the box defined by center_point, width and height
      void BuildFeatureList( const GPoint& in_CenterPoint,
         REAL32 in_fWidth,
         REAL32 in_fHeight,
         vector<ID>& out_FeatureList );

      // return a container (derived from GVectorView) that will contain the selected features
      virtual GVectorView* CreateView() = 0;

      // cell navigation
      bool LeftCell( const GCellPosition& in_Start, GCellPosition& out_Destination ) const;
      bool RightCell( const GCellPosition& in_Start, GCellPosition& out_Destination ) const;
      bool UpperCell( const GCellPosition& in_Start, GCellPosition& out_Destination ) const;
      bool LowerCell( const GCellPosition& in_Start, GCellPosition& out_Destination ) const;
   };
};

#endif