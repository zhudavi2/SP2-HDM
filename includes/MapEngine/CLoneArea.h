#ifndef _CLASS_LONE_AREA
#define _CLASS_LONE_AREA


#include "CLoop.h"


namespace Map 
{

   class GLoneArea
   {
      friend class GLoneAreaMap;
      friend class GLoneAreaView;

   public:
      // return ID number
      ID Id() const;

      // get total number of vertices
      UINT32 NbVertices() const;

      // get the bounding box that encloses the area
      const GBox& BoundingBox() const;

      // check if point is contained in area
      bool Contains( const GPoint& in_Point ) const;

      // access to outer loop
      const GLoop* Outer() const;

      // get number of inner loops
      UINT32 NbInners() const;

      // access a particular inner loop
      const GLoop* Inner( UINT32 in_iIndex ) const;

   private:
      ID              m_iId;
      GLoop           m_OuterLoop;
      GArray<GLoop>   m_InnerLoops;

      // load from file stream
      void Load( GFile& in_File );
   };

}

#endif