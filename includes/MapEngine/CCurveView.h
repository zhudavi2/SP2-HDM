#ifndef _CLASS_CURVE_VIEW
#define _CLASS_CURVE_VIEW


#include "CVectorView.h"


namespace Map 
{

   // forward declarations
   class GCurve;
   class GConnectingNode;


   class GCurveView: public GVectorView
   {
      friend class GCurveMap;

   public:
      // access particular curve
      const GCurve* Curve( ID in_iCurve ) const;

      // produce geometric constructs for graphical representation of curves
      void BuildMultiFacetList( ID in_iCurve, 
                                Geometry::GShape& out_Shape, 
                                Geometry::GMultiFacetList& out_MultiFacetList ) const;
      void BuildMultiFacetList( const vector<ID>& in_Curves,
                                Geometry::GShape& out_Shape,
                                Geometry::GMultiFacetList& out_MultiFacetList ) const;
      void BuildMultiFacetList( Geometry::GShape& shape,
                                Geometry::GMultiFacetList& out_MultiFacetList ) const;
      void ClipCurvesInRectangle( REAL32 in_fCenterX,  
                                  REAL32 in_fCenterY,
                                  REAL32 in_fWidth,
                                  REAL32 in_fHeight,
                                  Geometry::GShape& out_Shape,
                                  Geometry::GMultiFacetList& out_MultiFacetList ) const;
      void ClipCurvesInRectangle( const GPoint& in_Center,
                                  REAL32 in_fWidth,
                                  REAL32 in_fHeight,
                                  Geometry::GShape& out_Shape,
                                  Geometry::GMultiFacetList& out_MultiFacetList ) const;

      // write curves in text file using given separator
      void WriteCurvePoints( const GString& in_sFileName, const GString& in_sSeparator ) const;

   private:
      GCurveView( GCurveMap* in_pParentMap );
      ~GCurveView();

      // maps of features contained in the view
      hash_map<ID, GCurve*>            m_Curves;
      hash_map<ID, GConnectingNode*>   m_ConnectingNodes;

      // load features from data streams in parent map
      void Load();                                    // load all curves
      void Load( const vector<ID>& in_Curves );       // load curves in curve_list

      void CreateCurves();                                // create all curves
      void CreateCurves( const vector<ID>& curve_list );  // create curves in curve list
      void ReadCurves();                                  // read curves in stream
      void ReadConnectingNodes();                         // read connecting nodes in stream

      // helper functions to build geometry objects
      void InitGeometryMultiFacetList( Geometry::GShape& out_Shape,
                                       Geometry::GMultiFacetList& out_MultiFacetList ) const;
      void InitGeometryMultiFacetList( Geometry::GShape& out_Shape,
                                       Geometry::GMultiFacetList& out_MultiFacetList,
                                       const vector<ID>& in_Curves ) const;
      static void InitGeometryMultiFacetList( Geometry::GShape& out_Shape,
                                              Geometry::GMultiFacetList& out_MultiFacetList,
                                              const vector<GPointList*>& in_Curves );
      static void CurveToGeometryFacetList( const GCurve& in_Curve,
                                            Geometry::GFacetList& out_FacetList,
                                            Geometry::GShape& out_Shape,
                                            UINT32& io_iCurrentShapePosition );
      void GatherVisibleCurves( const GBox& in_ViewPort, vector<GCurve*>& out_VisibleCurves ) const;
   };

}

#endif