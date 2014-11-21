#ifndef _CLASS_POLYGON
#define _CLASS_POLYGON


namespace Map 
{

   // forward declarations
   class GFace;
   class GEdge;


   class GPolygon
   {
      friend class GMultifaceMap;
      friend class GMultifaceView;

   public:
      // get number of vertices
      UINT32 NbVertices() const;

      // check if point is contained in polygon
      bool Contains( const GPoint& in_Point ) const;

      // get the bounding box that encloses the polygon
      const GBox& BoundingBox() const;

      // get number of edges
      UINT32 NbEdges() const;

      // access a particular edge
      const GEdge* Edge( UINT32 in_iIndex ) const;

      // access interior and exterior faces
      const GFace* InnerFace() const;
      const GFace* OuterFace() const;

      // circular iterator that provides constant access to vertices
      class GVertexIterator;

      // get an iterator to access vertices
      GVertexIterator FirstVertex() const;

   private:
      ID              m_iId;
      GFace*          m_pInnerFace;
      GFace*          m_pOuterFace;
      GBox            m_BoundingBox;
      GArray<GEdge*>  m_Edges;

      GPolygon( ID in_iID );
   };

   class GPolygon::GVertexIterator
   {
   friend class GPolygon;

   public:
      GVertexIterator();
      GVertexIterator( const GPolygon* in_pPolygon );
      GVertexIterator( const GVertexIterator& in_OtherIterator );

      const GPoint*   Value() const;

      GVertexIterator Next() const;
      GVertexIterator Previous() const;

      GVertexIterator   operator ++    ( );
      GVertexIterator   operator ++    ( INT32 );
      GVertexIterator   operator --    ( );
      GVertexIterator   operator --    ( INT32 );
      const GPoint&     operator *     ( ) const;
      const GPoint*     operator ->    ( ) const;
      bool              operator ==    ( const GVertexIterator& in_Other ) const;
      bool              operator !=    ( const GVertexIterator& in_Other ) const;

   private:
      const GPolygon*   m_pPolygon;
      UINT32            m_iCurrentEdge;
      UINT32            m_iCurrentVertex;
   };
}

#endif