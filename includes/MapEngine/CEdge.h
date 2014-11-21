#ifndef _CLASS_EDGE
#define _CLASS_EDGE


#include "CCurve.h"
#include "CPolygon.h"


namespace Map 
{

   // forward declarations
   class GConnectingNode;
   class GPolygon;
   class GPolygon::GVertexIterator;


   class GEdge : public GCurve
   {
      friend class GMultifaceMap;
      friend class GMultifaceView;
      friend class GPolygon;
      friend class GPolygon::GVertexIterator;

   public:
      // access left and right polygons
      const GPolygon* RightPolygon() const;
      const GPolygon* LeftPolygon() const;

      // access left and right faces
      const GFace* RightFace() const;
      const GFace* LeftFace() const;

   private:
      GPolygon*   m_pRightPolygon;
      GPolygon*   m_pLeftPolygon;

      GEdge( ID in_iID );
   };

}

#endif