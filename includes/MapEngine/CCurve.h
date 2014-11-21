#ifndef _CLASS_CURVE
#define _CLASS_CURVE


#include "map_globals.h"


namespace Map 
{

   // forward declarations
   class GConnectingNode;


   class GCurve
   {
      friend class GCurveMap;
      friend class GCurveView;

   public:
      // return ID number
      ID Id() const;

      // get number of vertices
      UINT32 NbVertices() const;

      // access a particular vertex, in forward or reverse order
      const GPoint* Vertex( UINT32 in_iIndex ) const;
      const GPoint* VertexReverse( UINT32 in_iIndex ) const;

      // get the bounding box that encloses the curve
      const GBox& BoundingBox() const;

      // access from and to nodes
      const GConnectingNode* FromNode() const;
      const GConnectingNode* ToNode() const;

      // write vertices in point list container
      void AddToPointList( GPointList& io_PointList, ETraverseDirection in_TraverseDirection = FORWARD ) const;

   protected:
      ID                  m_iId;
      GConnectingNode*    m_pFromNode;
      GConnectingNode*    m_pToNode;
      GBox                m_BoundingBox;
      GArray<GPoint>      m_Vertices;

      GCurve( ID in_iID );
   };

}

#endif