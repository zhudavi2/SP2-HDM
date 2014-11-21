#ifndef _CLASS_LONE_AREA_MAP
#define _CLASS_LONE_AREA_MAP


#include "CVectorMap.h"
#include "CLoneAreaView.h"
#include "CIndexedInputFileStream.h"


namespace Map 
{

   // XML tags found in config file
   namespace LoneAreaMapXmlTags
   {
      const GString LONE_AREA = "LONE_AREA";
   };


   class GLoneAreaMap: public GVectorMap
   {
      friend class GLoneAreaView;

   public:
      GLoneAreaMap();
      ~GLoneAreaMap();

      // Initialize map properties from XML configuration file, set message logger
      bool Init( const GString& in_sConfigFileName,GFileManager* in_pFileManager, GLogger* in_pLogger = NULL );

      // reset the map
      void Clear();

      // get number of lone areas
      UINT32 NbFeatures() const;

      // open view within specified limits (or no limits at all if no parameters)
      GLoneAreaView* OpenView();
      GLoneAreaView* OpenView( REAL32 in_fCenterX, REAL32 in_fCenterY, REAL32 in_fWidth, REAL32 in_fHeight );
      GLoneAreaView* OpenView( const GPoint& in_CenterPoint, REAL32 in_fWidth, REAL32 in_fHeight );

      // open view containing specific features
      GLoneAreaView* OpenView( ID in_iLoneArea );
      GLoneAreaView* OpenView( const vector<ID>& in_LoneAreas );

   private:
      // create GLoneAreaView object
      GLoneAreaView* CreateView();

      // indexed input streams to retrieve the map features
      GIndexedInputFileStream m_LoneAreaStream;
   };

}

#endif