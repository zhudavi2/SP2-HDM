#ifndef _CLASS_MULTIFACE
#define _CLASS_MULTIFACE


namespace Map 
{

   // forward declarations
   class GFace;

   class GMultiface
   {
      friend class GMultifaceMap;
      friend class GMultifaceView;

   public:
      // return ID number
      ID Id() const;

      // get total number of vertices
      UINT32 NbVertices() const;

      // get the bounding box that encloses the multiface
      const GBox& BoundingBox() const;

      // check if point is contained in multiface
      bool Contains( const GPoint& in_Point ) const;

      // get number of faces
      UINT32 NbFaces() const;

      // access a particular face
      const GFace* Face( UINT32 in_iIndex ) const;

      // find all adjacent multifaces
      void Neighbors( vector<const GMultiface*>& out_Multifaces ) const;

   private:
      ID              m_iId;
      GBox            m_BoundingBox;
      GArray<GFace*>  m_Faces;

      GMultiface( ID in_iID );
   };

}

#endif