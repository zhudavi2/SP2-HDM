#ifndef _CLASS_LOOP
#define _CLASS_LOOP


namespace Map 
{

   class GLoop
   {
      friend class GLoneArea;
      friend class GLoneAreaMap;
      friend class GLoneAreaView;

   public:
      // get total number of vertices
      UINT32 NbVertices() const;

      // check if point is on loop border or inside it
      bool Contains( const GPoint& in_Point ) const;

      // write vertices in point list container
      void ToPointList( GPointList& out_PointList ) const;

   private:
      GBox            m_BoundingBox;
      GArray<GPoint>  m_Vertices;

      // load from file stream
      void Load( GFile &in_File );
   };

}

#endif