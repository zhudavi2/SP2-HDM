#ifndef _CLASS_FACE
#define _CLASS_FACE


namespace Map 
{

   // forward declarations
   class GMultiface;
   class GPolygon;


   class GFace
   {
      friend class GMultifaceMap;
      friend class GMultifaceView;

   public:
      // access parent multiface
      const GMultiface* Multiface() const;

      // get number of vertices
      UINT32 NbVertices() const;

      // check if point is contained in face
      bool Contains( const GPoint& in_Point ) const;

      // get the bounding box that encloses the face
      const GBox& BoundingBox() const;

      // access outer polygon
      const GPolygon* Outer() const;

      // get number of inner polygons
      UINT32 NbInners() const;

      // access a particular inner polygon
      const GPolygon* Inner( UINT32 in_iIndex ) const;

      // find all adjacent faces
      void Neighbors( vector<const GFace*>& out_Faces ) const;

   private:
      ID                  m_iId;
      GMultiface*         m_pMultiface;
      GPolygon*           m_pOuterPolygon;
      GArray<GPolygon*>   m_InnerPolygons;

      GFace( ID in_iID );
   };

}

#endif