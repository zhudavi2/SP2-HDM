/**************************************************************
*
* golem_image_2d.h
*
* Description
* ===========
*  Definition of the interface for the class GImage2D
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_IMAGE_2D_
#define _GOLEM_IMAGE_2D_

#include "golem_color.h"
#include "golem_pixel.h"
#include "golem_vector.h"
#include "golem_geometry.h"

namespace Hector
{

   /*!
   * Lists image resizing operations
   **/
   enum EImageResizeType
   {
      IMRESIZE_NO_STRETCH = 0,   // Resize does not stretch the source image
      IMRESIZE_NEAREST_POINT,    // Resize uses the nearest point for resize interpolation
      IMRESIZE_COUNT,            // Number of constants in this enumerations
   };

   /*!
   * Class that defines an interface to manipulate 2d images
   **/
   class GImage2D
   {
   protected:
      virtual ~GImage2D();

   public:
      // Image class allocation methods
      static GImage2D*   AllocObject();
      static void        FreeObject( GImage2D* in_Object );

      // Image save methods
      void SaveColorBitmap(const GString& in_sFileName);
      void SaveColorBitmap(HANDLE in_hFile, RGBTRIPLE* in_pData, int in_iSizeX, int in_iSizeY);
      void SaveGrayscaleBitmap(HANDLE in_hFile, BYTE* in_pData, int in_iSizeX, int in_iSizeY);

      // Image creation methods
      virtual bool   Create( const GVector2D<UINT32>& in_Size, EPixelFormat in_Format, UINT32 in_iPitch = -1 ) = 0;

      virtual bool   AttachPixelBuffer( const GVector2D<UINT32>& in_Size, EPixelFormat in_Format, UINT32 in_iPitch, const void* in_pPixels ) = 0;
      virtual bool   ReleasePixelBuffer() = 0;

      // Palette creation methods
      virtual bool   CreatePalette( EPixelFormat in_Format, UINT32 in_iNbPaletteColors = 256 ) = 0;

      virtual bool   AttachPaletteBuffer( EPixelFormat in_Format, const void* in_pPaletteColors, UINT32 in_iNbPaletteColors ) = 0;
      virtual bool   ReleasePaletteBuffer() = 0;

      // Image information methods
      virtual UINT32 Width() const = 0;
      virtual UINT32 Height() const = 0;
      virtual UINT32 Pitch() const = 0;

      virtual EPixelFormat ColorFormat() const = 0;
      virtual EPixelFormat PaletteFormat() const = 0;

      // Image size manipulation methods
      virtual bool   Resize( const GVector2D<UINT32>& in_Size, EImageResizeType in_ResizeType ) = 0;
      virtual bool   Resize( GImage2D& io_DestImage, const GVector2D<UINT32>& in_Size, EImageResizeType in_ResizeType ) const = 0;

      // Image pixel manipulation methods
      virtual void*  Pixels() const = 0;
      virtual void*  PixelAt( const GVector2D<UINT32>& in_Coord ) const = 0;

      virtual bool   ReadRow( const GVector2D<UINT32>& in_Coord, UINT32 in_iWidth, EPixelFormat in_Format, void* out_pRowPixels ) = 0;
      virtual bool   WriteRow( const GVector2D<UINT32>& in_Coord, UINT32 in_iWidth, EPixelFormat in_Format, const void* in_pRowPixels ) = 0;

      // Image color manipulation methods
      virtual bool   ReadColor( const GVector2D<UINT32>& in_Coord, GColorRGBReal& out_Color ) = 0;
      virtual bool   ReadColor( const GVector2D<UINT32>& in_Coord, GColorRGBInt& out_Color ) = 0;

      virtual bool   WriteColor( const GVector2D<UINT32>& in_Coord, const GColorRGBReal& in_Color ) = 0;
      virtual bool   WriteColor( const GVector2D<UINT32>& in_Coord, const GColorRGBInt& in_Color ) = 0;

      virtual bool   ReadRowColor( const GVector2D<UINT32>& in_Coord, UINT32 in_iWidth, GColorRGBReal* out_pRowColors ) = 0;
      virtual bool   ReadRowColor( const GVector2D<UINT32>& in_Coord, UINT32 in_iWidth, GColorRGBInt* out_pRowColors ) = 0;

      virtual bool   WriteRowColor( const GVector2D<UINT32>& in_Coord, UINT32 in_iWidth, const GColorRGBReal* in_pRowColors ) = 0;
      virtual bool   WriteRowColor( const GVector2D<UINT32>& in_Coord, UINT32 in_iWidth, const GColorRGBInt* in_pRowColors ) = 0;

      // Image palette manipulation methods
      virtual UINT32  NbPaletteColors() = 0;

      virtual bool   ReadPaletteColor( UINT32 in_iIndex, GColorRGBReal& out_Color ) = 0;
      virtual bool   ReadPaletteColor( UINT32 in_iIndex, GColorRGBInt& out_Color ) = 0;

      virtual bool   WritePaletteColor( UINT32 in_iIndex, const GColorRGBReal& in_Color) = 0;
      virtual bool   WritePaletteColor( UINT32 in_iIndex, const GColorRGBInt& in_Color) = 0;

      virtual void*  PaletteColors() const = 0;

      // Image data manipulation methods
      virtual bool   Clear() = 0;
      virtual bool   Clear(const void* in_pPixelValue) = 0;

      virtual bool   FlipVertical() = 0;
      virtual bool   FlipHorizontal() = 0;

      virtual bool   Copy( GImage2D& in_Image ) = 0;
      virtual bool   Blit( GImage2D& io_DestImage, const Geometry::GRectangle<UINT32>& in_SourceRect, const GVector2D<UINT32>& in_DestPos) const = 0;
   };

}

#endif //#ifndef _GOLEM_IMAGE_2D_