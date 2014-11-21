/**************************************************************
*
* golem_image_2d_generic.h
*
* Description
* ===========
*  Description & implementation of GImage2DGeneric, a basic 
*  implementation (read : no optimization) of the GImage2D
*  interface.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_IMAGE_2D_GENERIC_
#define _GOLEM_IMAGE_2D_GENERIC_

#include "golem_image_2d.h"

namespace Hector
{

   class GImage2DGeneric : public GImage2D
   {
      UINT8*           m_pPixels;
      UINT32           m_iWidth;
      UINT32           m_iHeight;
      UINT32           m_iPitch;

      EPixelFormat     m_ColorFormat;
      EPixelFormat     m_PaletteFormat;

      UINT8*           m_pPaletteColors;
      UINT32           m_iNbPaletteColors;

      bool             m_bAttachedPixelBuffer;
      bool             m_bAttachedPaletteBuffer;

   public:
      GImage2DGeneric();
      ~GImage2DGeneric();

      // Overloaded methods of class GImage2D
      bool   Create( const GVector2D<UINT32>& in_Size, EPixelFormat in_Format, UINT32 in_iPitch = -1 );

      bool   AttachPixelBuffer( const GVector2D<UINT32>& in_Size, EPixelFormat in_Format, UINT32 in_iPitch, const void* in_pPixels );
      bool   ReleasePixelBuffer();


      bool   CreatePalette( EPixelFormat in_Format, UINT32 in_iNbPaletteColors = 256 );

      bool   AttachPaletteBuffer( EPixelFormat in_Format, const void* in_pPaletteColors, UINT32 in_iNbPaletteColors );
      bool   ReleasePaletteBuffer();


      UINT32 Width() const;
      UINT32 Height() const;
      UINT32 Pitch() const;

      EPixelFormat ColorFormat() const;
      EPixelFormat PaletteFormat() const;


      bool   Resize( const GVector2D<UINT32>& in_Size, EImageResizeType in_ResizeType );
      bool   Resize( GImage2D& io_DestImage, const GVector2D<UINT32>& in_Size, EImageResizeType in_ResizeType ) const;


      void*  Pixels() const;

      void*  PixelAt( const GVector2D<UINT32>& in_Coord ) const;

      bool   ReadRow( const GVector2D<UINT32>& in_Coord, UINT32 in_iWidth, EPixelFormat in_Format, void* out_pRowPixels );
      bool   WriteRow( const GVector2D<UINT32>& in_Coord, UINT32 in_iWidth, EPixelFormat in_Format, const void* in_pRowPixels );


      bool   ReadColor( const GVector2D<UINT32>& in_Coord, GColorRGBReal& out_Color );
      bool   ReadColor( const GVector2D<UINT32>& in_Coord, GColorRGBInt& out_Color );

      bool   WriteColor( const GVector2D<UINT32>& in_Coord, const GColorRGBReal& in_Color );
      bool   WriteColor( const GVector2D<UINT32>& in_Coord, const GColorRGBInt& in_Color );

      bool   ReadRowColor( const GVector2D<UINT32>& in_Coord, UINT32 in_iWidth, GColorRGBReal* out_pRowColors );
      bool   ReadRowColor( const GVector2D<UINT32>& in_Coord, UINT32 in_iWidth, GColorRGBInt* out_pRowColors );

      bool   WriteRowColor( const GVector2D<UINT32>& in_Coord, UINT32 in_iWidth, const GColorRGBReal* in_pRowColors );
      bool   WriteRowColor( const GVector2D<UINT32>& in_Coord, UINT32 in_iWidth, const GColorRGBInt* in_pRowColors );


      UINT32  NbPaletteColors();

      bool   ReadPaletteColor( UINT32 in_iIndex, GColorRGBReal& out_Color );
      bool   ReadPaletteColor( UINT32 in_iIndex, GColorRGBInt& out_Color );

      bool   WritePaletteColor( UINT32 in_iIndex, const GColorRGBReal& in_Color);
      bool   WritePaletteColor( UINT32 in_iIndex, const GColorRGBInt& in_Color);

      void*  PaletteColors() const;

      bool   Clear();
      bool   Clear(const void* in_pPixelValue);

      bool   FlipVertical();
      bool   FlipHorizontal();
      bool   Copy( GImage2D& in_Image );
      bool   Blit( GImage2D& io_DestImage, const Geometry::GRectangle<UINT32>& in_SourceRect, const GVector2D<UINT32>& in_DestPos) const;

   protected:
      void  Clear8(const void* in_pPixelValue);
      void  Clear16(const void* in_pPixelValue);
      void  Clear32(const void* in_pPixelValue);
   };

}

#endif //#ifndef _GOLEM_IMAGE_2D_GENERIC_