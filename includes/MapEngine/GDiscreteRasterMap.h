#ifndef _CLASS_DISCRETE_RASTER_MAP
#define _CLASS_DISCRETE_RASTER_MAP

#include "CRasterMap.h"
#include "GDiscreteRasterView.h"


namespace Map
{

   class GDiscreteRasterMap : public GRasterMap
   {
      friend class GDiscreteRasterView;

   public:
      // load view within specified limits
      GDiscreteRasterView* OpenView( REAL32 in_fCenterX,
                                     REAL32 in_fCenterY,
                                     REAL32 in_fWidth,
                                     REAL32 in_fHeight );
      GDiscreteRasterView* OpenView( const GPoint& in_CenterPoint,
                                     REAL32 in_fWidth,
                                     REAL32 in_fHeight );

   private:
      // create a view
      GDiscreteRasterView* CreateView();
   };
};


#endif