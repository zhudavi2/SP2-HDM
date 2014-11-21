#ifndef _CLASS_VECTOR_VIEW
#define _CLASS_VECTOR_VIEW


namespace Map 
{

   // forward declarations
   class GVectorMap;


   class GVectorView
   {
      friend class GVectorMap;

   public:
      // method used to clip polygons or curves (expressed as simple point lists) inside convex
      // and counterclockwise clipping polygons
      static void Clip( const GPointList& in_FeatureToClip,
                        EFeatureType in_FeatureType,
                        const GPointList& in_ClippingPolygon,
                        vector<GPointList*>& out_ClippedFeatures );

      // write point lists in text file using given separator
      static void PrintPointLists( const vector<GPointList*>& in_PointLists, 
                                   const GString& in_sFileName,
                                   const GString& in_sSeparator );

   protected:
      // pointer to parent GVectorMap that spawned the view
      GVectorMap* m_pParentMap;

      GVectorView( GVectorMap* m_pParentMap );
      virtual ~GVectorView();

      // load features from data streams in m_pParentMap
      virtual void Load( const vector<ID>& feature_list ) = 0;    // load features in feature_list
      virtual void Load() = 0;                                    // load all features

      // various functions used in the construction of geometry objects
      static void PointListToGeometryPolygon( const GPointList& in_PointList,
                                              Geometry::GPolygon& out_Polygon,
                                              Geometry::GShape& out_Shape,
                                              UINT32& io_iShapePosition,
                                              ETraverseDirection in_TraverseDirection = FORWARD );
      static void PointListToGeometryFacetList( const GPointList& in_PointList,
                                                Geometry::GFacetList& out_FacetList,
                                                Geometry::GShape& out_Shape,
                                                UINT32& io_iShapePosition,
                                                ETraverseDirection in_TraverseDirection = FORWARD );

      // helper functions for the Clip() method
      static void ClipPolygon( const GPointList& in_PolygonToClip,
                               const GLineSegment& in_ClippingSegment,
                               vector <GPointList*>& out_ClippedPolygons );
      static void ClipCurve( const GPointList& in_CurveToClip,
                             const GLineSegment& in_ClippingSegment,
                             vector <GPointList*>& out_ClippedCurves );
      static EOrientation RotationDirection( const GPointList& in_PointList );
      static void SortPointLists( vector<GPointList*>& io_PointLists, const GLineSegment& in_ClippingSegment );
      static REAL32 ProjectionOnClippingSegment( const GLineSegment& in_ClippingSegment, const GPoint& in_Point );
      static bool PointListsOverlap( const GPointList* in_pWidePointList, const GPointList* in_pNarrowPointList );
      static GPointList* MergeOverlappingPointLists( GPointList* io_pWidePointList,
                                                     GPointList* io_pNarrowPointList,
                                                     const GLineSegment& in_ClippingSegment );
   };

}

#endif