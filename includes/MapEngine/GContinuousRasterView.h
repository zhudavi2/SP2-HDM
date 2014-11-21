/**************************************************************
*
* GContinuousRasterView.h
*
* Description
* ===========
*  Class Map::GContinuousRasterView description & implementation
*
* Owner
* =====
*  Francis Jackson & Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CLASS_CONTINUOUS_RASTER_VIEW
#define _CLASS_CONTINUOUS_RASTER_VIEW

#include "CRasterView.h"

namespace Map
{

   class GContinuousRasterView : public GRasterView
   {
      friend class GContinuousRasterMap;

   public:
      // get interpolated value of raster field at specified location
      REAL32 Value( const GPoint& in_Point ) const;
      REAL32 Value( REAL32 in_X, REAL32 in_Y ) const;

      // fill rectangular grids of interpolated points
      void FillRectangularGrid( REAL32 in_fCenterX,
                                REAL32 in_fCenterY,
                                REAL32 in_fWidth,
                                REAL32 in_fHeight,
                                UINT32 in_iSamplingX,
                                UINT32 in_iSamplingY,
                                REAL32* out_pBuffer ) const;

      // fill rectangular grids of interpolated points
      template <class GBufferType>
      void FillRectangularGrid( REAL32 in_fCenterX,
                                REAL32 in_fCenterY,
                                REAL32 in_fWidth,
                                REAL32 in_fHeight,
                                UINT32 in_iSamplingX,
                                UINT32 in_iSamplingY,
                                GBufferType* out_pBuffer ) const;

      // fill trapezoidal grids of interpolated points
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
      GContinuousRasterView( GContinuousRasterMap* in_pParentMap, const GRasterCoord& in_ULCorner, const GRasterCoord& in_LRCorner );
   };


   // implementation of template methods
   template <class GBufferType>
   void GContinuousRasterView::FillRectangularGrid( REAL32 in_fCenterX,
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
   void GContinuousRasterView::FillTrapezoidalGrid( REAL32 in_fCenterX,
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