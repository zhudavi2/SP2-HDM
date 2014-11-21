#ifndef _CLASS_CURVE_MAP
#define _CLASS_CURVE_MAP


#include "CVectorMap.h"
#include "CCurveView.h"
#include "CIndexedInputFileStream.h"


namespace Map
{

   // XML tags found in config file
   namespace CurveMapXmlTags
   {
      const GString CURVE             = "CURVE";
      const GString CONNECTING_NODE   = "CONNECTING_NODE";
   };


   class GCurveMap: public GVectorMap
   {
      friend class GCurveView;

   public:
      GCurveMap();
      ~GCurveMap();

      // Initialize map properties from XML configuration file, set message logger
      bool Init( const GString& in_sConfigFileName, GFileManager* in_pFileManager, GLogger* in_pLogger = NULL );

      // reset the map
      void Clear();

      // get number of curves
      UINT32 NbFeatures() const;

      // open view within specified limits (or no limits at all if no parameters)
      GCurveView* OpenView();
      GCurveView* OpenView( REAL32 in_fCenterX, REAL32 in_fCenterY, REAL32 in_fWidth, REAL32 in_fHeight );
      GCurveView* OpenView( const GPoint& in_Center, REAL32 in_fWidth, REAL32 in_fHeight );

      // open view containing specific features
      GCurveView* OpenView( ID in_iCurve );
      GCurveView* OpenView( const vector<ID>& in_Curves );

   private:
      // create GCurveView object
      GCurveView* CreateView();

      // indexed input streams to retrieve the map features
      GIndexedInputFileStream m_CurveStream;
      GIndexedInputFileStream m_ConnectingNodeStream;
   };

}

#endif