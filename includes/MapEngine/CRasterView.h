/**************************************************************
*
* CRasterView.h
*
* Description
* ===========
*  Description & implementation of class Map::GRasterView
*
* Owner
* =====
*  Francis Jackson & Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CLASS_RASTER_VIEW
#define _CLASS_RASTER_VIEW


#include "CRasterTile.h"


namespace Map 
{

   class GRasterView
   {
      friend class GRasterMap;

   protected:
      GRasterMap*                         m_pParentMap;  // raster map that is the context of the view

      GRasterView( GRasterMap* in_pParentMap, 
                   const GRasterCoord& in_ULCorner, 
                   const GRasterCoord& in_LRCorner );
      virtual ~GRasterView();

      // attach & copy decompressed tile to view
      void AddTile( GRasterTile* io_pTile );

      void CopyTile( GRasterTile& in_Tile, const GPixelCoord& in_ULCopy, const GPixelCoord& in_LRCopy, const GPixelCoord& in_ULDest);

      vector<GRasterTile*> m_vRefTiles;

      UINT8*         m_pRaster;
      UINT32         m_iPitch;
      GPixelCoord    m_ULCorner;
      GPixelCoord    m_LRCorner;
   };


   // template function to load data from a view into a rectangular grid
   template <class GBufferType, class GViewType>
   void LoadRectangularGrid( const GPoint& in_Center,
                             REAL32 in_fWidth,
                             REAL32 in_fHeight,
                             UINT32 in_iSamplingX,
                             UINT32 in_iSamplingY,
                             GViewType* in_pView,
                             GBufferType* out_pBuffer )
   {
      REAL32 fSpacingX = 0;
      REAL32 fSpacingY = 0;
      GPoint UpperLeftPoint( in_Center.X - in_fWidth / 2, in_Center.Y + in_fHeight / 2 );
      GPoint CurrentPoint;

      if ( in_iSamplingX != 1 )
      {
         fSpacingX = in_fWidth / ( in_iSamplingX - 1 );
      }

      if ( in_iSamplingY != 1 )
      {
         fSpacingY = in_fHeight / ( in_iSamplingY - 1 );
      }

      // grid is populated in the upper-left to lower-right order

      for ( UINT32 iRow = 0; iRow < in_iSamplingY; iRow++ )
      {
         CurrentPoint.Y = UpperLeftPoint.Y - fSpacingY * iRow;

         for ( UINT32 iCol = 0; iCol < in_iSamplingX; iCol++ )
         {
            CurrentPoint.X = UpperLeftPoint.X + fSpacingX * iCol;

            out_pBuffer[ iRow * in_iSamplingX + iCol ] = (GBufferType) in_pView->Value( CurrentPoint );
         }
      }
   }

   // template function to load data from a view into a trapezoidal grid
   template <class GBufferType, class GViewType>
   void LoadTrapezoidalGrid( const GPoint& in_Center,
                             REAL32 in_fUpperWidth,
                             REAL32 in_fLowerWidth,
                             REAL32 in_fHeight,
                             UINT32 in_iSamplingX,
                             UINT32 in_iSamplingY,
                             GViewType* in_pView,
                             GBufferType* out_pBuffer )
   {
      REAL32 fSpacingY        = 0;
      REAL32 fSpacingX        = 0;
      REAL32 fWidthVariation  = 0;
      REAL32 fCurrentWidth    = 0;
      GPoint CurrentPoint;

      if ( in_iSamplingY != 1 )
      {
         fSpacingY         = in_fHeight / ( in_iSamplingY - 1 );
         fWidthVariation   = ( in_fLowerWidth - in_fUpperWidth ) / ( in_iSamplingY - 1 );
      }

      // grid is populated in the upper-left to lower-right order

      for ( UINT32 row = 0; row < in_iSamplingY; row++ )
      {
         fCurrentWidth = in_fUpperWidth + fWidthVariation * row;

         if ( in_iSamplingX != 1 )
         {
            fSpacingX = fCurrentWidth / ( in_iSamplingX - 1 );
         }

         CurrentPoint.Y = ( in_Center.Y + in_fHeight / 2 ) - fSpacingY * row;

         for ( UINT32 col = 0; col < in_iSamplingX; col++ )
         {
            CurrentPoint.X = ( in_Center.X - fCurrentWidth / 2 ) + fSpacingX * col;

            out_pBuffer[ row * in_iSamplingX + col ] = (GBufferType) in_pView->Value( CurrentPoint );
         }
      }
   }

}

#endif