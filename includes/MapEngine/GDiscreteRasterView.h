#ifndef _CLASS_DISCRETE_RASTER_VIEW
#define _CLASS_DISCRETE_RASTER_VIEW

namespace Map
{

   class GDiscreteRasterView : public GRasterView
   {
      friend class GDiscreteRasterMap;

   public:
      // get value of raster field at specified location
      UINT8 Value( const GPoint& in_Point ) const;
      UINT8 Value( REAL32 in_X, REAL32 in_Y ) const;

      // fill rectangular grids of points
      template <class GBufferType>
      void FillRectangularGrid( REAL32 in_fCenterX,
                                REAL32 in_fCenterY,
                                REAL32 in_fWidth,
                                REAL32 in_fHeight,
                                UINT32 in_iSamplingX,
                                UINT32 in_iSamplingY,
                                GBufferType* out_pBuffer ) const;

      // fill trapezoidal grids of points
      template <class GBufferType>
      void FillTrapezoidalGrid( REAL32 in_fCenterX,
                                REAL32 in_fCenterY,
                                REAL32 in_fUpperWidth,
                                REAL32 in_fLowerWidth,
                                REAL32 in_fHeight,
                                UINT32 in_iSamplingX,
                                UINT32 in_iSamplingY,
                                GBufferType* out_pBuffer ) const;

   private:
      GDiscreteRasterView( GDiscreteRasterMap* in_pParentMap );
   };


   // implementation of template methods
   template <class GBufferType>
   void GDiscreteRasterView::FillRectangularGrid( REAL32 in_fCenterX,
                                                  REAL32 in_fCenterY,
                                                  REAL32 in_fWidth,
                                                  REAL32 in_fHeight,
                                                  UINT32 in_iSamplingX,
                                                  UINT32 in_iSamplingY,
                                                  GBufferType* out_pBuffer ) const
   {
      LoadRectangularGrid( GPoint( in_fCenterX, in_fCenterY ),
                           in_fWidth,
                           in_fHeight,
                           in_iSamplingX,
                           in_iSamplingY,
                           this,
                           out_pBuffer );
   }

   template <class GBufferType>
   void GDiscreteRasterView::FillTrapezoidalGrid( REAL32 in_fCenterX,
                                                  REAL32 in_fCenterY,
                                                  REAL32 in_fUpperWidth,
                                                  REAL32 in_fLowerWidth,
                                                  REAL32 in_fHeight,
                                                  UINT32 in_iSamplingX,
                                                  UINT32 in_iSamplingY,
                                                  GBufferType* out_pBuffer ) const
   {
      LoadTrapezoidalGrid( GPoint( in_fCenterX, in_fCenterY ),
                           in_fUpperWidth,
                           in_fLowerWidth,
                           in_fHeight,
                           in_iSamplingX,
                           in_iSamplingY,
                           this,
                           out_pBuffer );
   }

};

#endif