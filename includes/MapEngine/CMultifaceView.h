#ifndef _CLASS_MULTIFACE_VIEW
#define _CLASS_MULTIFACE_VIEW


#include "CVectorView.h"


namespace Map 
{
   // forward declarations
   class GMultiface;
   class GFace;
   class GPolygon;
   class GEdge;
   class GConnectingNode;

   class GMultifaceView: public GVectorView
   {
      friend class GMultifaceMap;

   public:
      // iterate through all multifaces
      const GMultiface* FirstMultiface();
      const GMultiface* NextMultiface();

      // access particular multiface
      const GMultiface* Multiface( ID in_iMultiface ) const;

      // find which multiface, if any, contains the given point
      const GMultiface* Locate( const GPoint& in_Point ) const;
      const GMultiface* Locate( REAL32 in_fX, REAL32 in_fY ) const;

      // get the bounding box that encloses the specified multifaces
      GBox ComputeBoundingBox(ID in_iMultifaceId) const;
      GBox ComputeBoundingBox(const vector<ID>& in_MultifaceIdList) const;
      GBox ComputeBoundingBox() const;

      // find all adjacent neighbors of given multiface among a list of potential neighbors
      void GroupNeighboringMultifaces( ID in_iMultiface,
                                       const vector<ID>& in_PotentialNeighbors,
                                       vector<const GMultiface*>& out_Neighbors ) const;

      // produce geometric constructs for graphical representation of multifaces
      void BuildFaceList( ID in_iMultifaceId,
                          Geometry::GShape& out_Shape,
                          Geometry::GFaceList<Geometry::GPolygon>& out_Faces ) const;

      void BuildFaceList( const vector<ID>& in_MultifaceIdList,
                          Geometry::GShape& out_Shape,
                          Geometry::GFaceList<Geometry::GPolygon>& out_Faces ) const;

      void BuildFaceList( Geometry::GShape& out_Shape,
                          Geometry::GFaceList<Geometry::GPolygon>& out_Faces ) const;

      void BuildContours( ID in_iMultifaceId,
                          Geometry::GShape& out_Shape,
                          Geometry::GMultiFacetList& out_ExternalContours,
                          Geometry::GMultiFacetList& out_InternalContours ) const;

      void BuildContours( const vector<ID>& in_MultifaceIdList,
                          Geometry::GShape& out_Shape,
                          Geometry::GMultiFacetList& out_ExternalContours,
                          Geometry::GMultiFacetList& out_InternalContours ) const;

      void BuildContours( Geometry::GShape& out_Shape,
                          Geometry::GMultiFacetList& out_ExternalContours,
                          Geometry::GMultiFacetList& out_InternalContours ) const;

      void ClipOuterLimitsInRectangle( REAL32 in_fCenterX,
                                       REAL32 in_fCenterY,
                                       REAL32 in_fWidth,
                                       REAL32 in_fHeight,
                                       Geometry::GShape& out_Shape,
                                       Geometry::GFaceList<Geometry::GPolygon>& out_Outers,
                                       Geometry::GFaceList<Geometry::GPolygon>& out_Inners ) const;

      void ClipOuterLimitsInRectangle( const GPoint& in_Center,
                                       REAL32 in_fWidth,
                                       REAL32 in_fHeight,
                                       Geometry::GShape& out_Shape,
                                       Geometry::GFaceList<Geometry::GPolygon>& out_Outers,
                                       Geometry::GFaceList<Geometry::GPolygon>& out_Inners ) const;

      // write polygon in text file using given separator
      void WritePolygonPoints( const GString& in_sFileName, const GString& in_sSeparator ) const;

   private:
      GMultifaceView( GMultifaceMap* in_pParentMap );
      ~GMultifaceView();

      // hash_maps of features contained in the view
      hash_map<ID, GMultiface*>        m_Multifaces;
      hash_map<ID, GFace*>             m_Faces;
      hash_map<ID, GPolygon*>          m_Polygons;
      hash_map<ID, GEdge*>             m_Edges;
      hash_map<ID, GConnectingNode*>   m_ConnectingNodes;

      hash_map<ID, GMultiface*>::iterator m_MultifaceIt;

      // load features from data streams in parent map
      void Load();                                    // load all multifaces
      void Load( const vector<ID>& in_Multifaces );   // load multifaces in multiface list

      void CreateMultifaces();                                    // create all multifaces
      void CreateMultifaces( const vector<ID>& in_Multifaces );   // create multifaces in multiface list
      void ReadMultifaces();                      // read multifaces in stream
      void ReadFaces();                           // read faces in stream
      void ReadPolygons();                        // read polygons in stream
      void ReadEdges();                           // read edges in stream
      void ReadConnectingNodes();                 // read connecting nodes in stream

      // helper functions to build geometry objects
      struct GEdgeUsage
      {
         UINT32 Frequency;
         UINT32 Position;
      };

      static void MultifacesToGeometryFaceList( const vector<GMultiface*>& in_Multifaces,
                                                Geometry::GShape& out_Shape,
                                                Geometry::GFaceList<Geometry::GPolygon>& out_Faces );

      static UINT32 CountFaces( const vector<GMultiface*>& in_Multifaces );

      static void GatherEdgeInfo( const vector<GMultiface*>& in_Multifaces,
                                  hash_map<GEdge*, GEdgeUsage>& out_EdgeUsageMap,
                                  UINT32& out_iNbVertices,
                                  bool in_bRepeatEdges );

      static void SizeGeometryContours( const hash_map<GEdge*, GEdgeUsage>& in_EdgeUsageMap,
                                        Geometry::GMultiFacetList& out_ExternalContours,
                                        Geometry::GMultiFacetList& out_InternalContours );

      static void AddMultifaceToGeometryFaceList( const GMultiface& in_Multiface, 
                                                  Geometry::GShape& out_Shape,
                                                  Geometry::GFaceList<Geometry::GPolygon>& out_Faces,
                                                  UINT32& io_iCurrentShapePosition,
                                                  UINT32& io_iCurrentFace,
                                                  hash_map<GEdge*, GEdgeUsage>& io_EdgeUsageMap );

      static void FillGeometryPolygon( const GPolygon& in_MapPolygon,
                                       Geometry::GPolygon& out_GeometryPolygon,
                                       Geometry::GShape& out_Shape,
                                       UINT32& io_iCurrentShapePosition,
                                       hash_map<GEdge*, GEdgeUsage>& io_EdgeUsageMap );

      static void WriteEdgeInShape( const GEdge& in_Edge,
                                    Geometry::GShape& out_Shape,
                                    UINT32& io_iCurrentShapePosition,
                                    ETraverseDirection in_Direction );

      static void SetGeometryContours( const hash_map<GEdge*, GEdgeUsage>& in_EdgeUsageMap, 
                                       Geometry::GMultiFacetList& out_ExternalContours,
                                       Geometry::GMultiFacetList& out_InternalContours );

      static void MultifacesToGeometryContours( const vector<GMultiface*>& in_Multifaces,
                                                Geometry::GShape& out_Shape,
                                                Geometry::GMultiFacetList& out_ExternalContours,
                                                Geometry::GMultiFacetList& out_InternalContours );

      void BuildVisiblePolygons( const GBox& in_ViewPort,
                                 vector<GPointList*>& out_Outers,
                                 vector<GPointList*>& out_Inners ) const;

      void GatherVisibleFaces( const GBox& in_ViewPort, hash_set<const GFace*>& out_VisibleFaces ) const;

      static void GatherOutsideEdges( const hash_set<const GFace*>& in_Faces,
                                      hash_map<const GEdge*, ETraverseDirection>& out_Edges );

      static bool FindNextEdge( const GEdge*& io_pCurrentEdge,
                                ETraverseDirection& io_CurrentDirection,
                                const hash_map<const GEdge*, ETraverseDirection>& in_Edges );
   };
}

#endif