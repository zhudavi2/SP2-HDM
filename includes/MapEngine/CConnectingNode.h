#ifndef _CLASS_CONNECTING_NODE
#define _CLASS_CONNECTING_NODE


namespace Map 
{

   // forward declarations
   class GCurve;


   class GConnectingNode
   {
      friend class GMultifaceMap;
      friend class GMultifaceView;
      friend class GCurveMap;
      friend class GCurveView;

   public:
      // access node's position
      const GPoint& Position() const;
      
      // get number of connected curves
      UINT32 NbConnectedCurves() const;

      // access a particular connected curve
      const GCurve* ConnectedCurve( UINT32 in_iIndex ) const;

   private:
      ID                  m_iId;
      GPoint              m_Position;
      GArray<GCurve*>     ConnectedCurves;

      GConnectingNode( ID in_iID );
   };

}

#endif