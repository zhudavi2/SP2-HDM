#ifndef _CLASS_MULTIFACE_MAP
#define _CLASS_MULTIFACE_MAP


#include "CVectorMap.h"
#include "CMultifaceView.h"
#include "CIndexedInputFileStream.h"


namespace Map 
{

   // XML tags found in config file
   namespace MultifaceMapXmlTags
   {
      const GString MULTIFACE             = "MULTIFACE";
      const GString FACE                  = "FACE";
      const GString POLYGON               = "POLYGON";
      const GString EDGE                  = "EDGE";
      const GString CONNECTING_NODE       = "CONNECTING_NODE";
   };


   class GMultifaceMap: public GVectorMap
   {
      friend class GMultifaceView;

   public:
      GMultifaceMap();
      ~GMultifaceMap();

      // Initialize map properties from XML configuration file, set message logger
      bool Init( const GString& in_sConfigFileName, GFileManager* in_pFileManager, GLogger* in_pLogger = NULL );

      // reset the map
      void Clear();

      // get number of multifaces
      UINT32 NbFeatures() const;

      // open view within specified limits (or no limits at all if no parameters)
      GMultifaceView* OpenView();
      GMultifaceView* OpenView( REAL32 in_fCenterX, REAL32 in_fCenterY, REAL32 in_fWidth, REAL32 in_fHeight );
      GMultifaceView* OpenView( const GPoint& in_CenterPoint, REAL32 in_fWidth, REAL32 in_fHeight );

      // open view containing specific features
      GMultifaceView* OpenView( ID in_iMultiface );
      GMultifaceView* OpenView( const vector<ID>& in_Multifaces );

   private:
      // create GMultifaceView object
      GMultifaceView* CreateView();

      // indexed input streams to retrieve the map features
      GIndexedInputFileStream m_MultifaceStream;
      GIndexedInputFileStream m_FaceStream;
      GIndexedInputFileStream m_PolygonStream;
      GIndexedInputFileStream m_EdgeStream;
      GIndexedInputFileStream m_ConnectingNodeStream;
   };

}

#endif