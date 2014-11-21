/**************************************************************
*
* golem_pixel.h
*
* Description
* ===========
*  Definition & implementation of GPixel & GPixelManager
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_PIXEL_H_
#define _GOLEM_PIXEL_H_

#include "golem_color.h"
#include "golem_types.h"
#include "golem_memory_util.h"

namespace Hector
{

//! Types of available pixel formats
enum EPixelFormat
{
   PIXELFMT_UNKNOWN = 0,   // Unknown pixel format
   PIXELFMT_R8G8B8,        // 24-bit RGB; 8 bits per channel
   PIXELFMT_A8R8G8B8,      // 32-bit ARGB; 8 bits per channel
   PIXELFMT_X8R8G8B8,      // 32-bit RGB; 8 bits per channel
   PIXELFMT_R5G6B5,        // 16-bit RGB; 5 bits for red, 6 bits for green, 5 bits for blue
   PIXELFMT_X1R5G5B5,      // 16-bit RGB; 5 bits per channel
   PIXELFMT_A1R5G5B5,      // 16-bit ARGB; 5 bits per color, 1 bit for alpha
   PIXELFMT_A4R4G4B4,      // 16-bit ARGB, 4 bits per channel
   PIXELFMT_R3G3B2,        // 8-bit RGB pixel format using 3 bits for red, 3 bits for green, and 2 bits for blue
   PIXELFMT_A8,            // 8-bit alpha only
   PIXELFMT_A8R3G3B2,      // 16-bit ARGB pixel format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue
   PIXELFMT_X4R4G4B4,      // 16-bit RGB pixel format using 4 bits for each color
   PIXELFMT_A2B10G10R10,   // 32-bit pixel format using 10 bits for each color and 2 bits for alpha
   PIXELFMT_A8B8G8R8,      // 32-bit ARGB pixel format with alpha, using 8 bits per channel
   PIXELFMT_X8B8G8R8,      // 32-bit RGB pixel format, where 8 bits are reserved for each color
   PIXELFMT_G16R16,        // 32-bit pixel format using 16 bits each for green and red
   PIXELFMT_A2R10G10B10,   // 32-bit pixel format using 10 bits each for red, green, and blue, and 2 bits for alpha
   PIXELFMT_A16B16G16R16,  // 64-bit pixel format using 16 bits for each component
   PIXELFMT_A8P8,          // 8-bit color indexed with 8 bits of alpha
   PIXELFMT_P8,            // 8-bit color indexed
   PIXELFMT_L8,            // 8-bit luminance only
   PIXELFMT_L16,           // 16-bit luminance only
   PIXELFMT_A8L8,          // 16-bit using 8 bits each for alpha and luminance
   PIXELFMT_A4L4,          // 8-bit using 4 bits each for alpha and luminance

   PIXELFMT_V8U8,          // 16-bit bump-map format using 8 bits each for u and v data. 
   PIXELFMT_Q8W8V8U8,      // 32-bit bump-map format using 8 bits for each channel. 
   PIXELFMT_V16U16,        // 32-bit bump-map format using 16 bits for each channel. 
   PIXELFMT_Q16W16V16U16,  // 64-bit bump-map format using 16 bits for each component. 
   PIXELFMT_CxV8U8,        // 16-bit normal compression format. The texture sampler computes the C channel from: C = sqrt(1 - U2 - V2). 

   PIXELFMT_L6V5U5,        // 16-bit bump-map format with luminance using 6 bits for luminance, and 5 bits each for v and u. 
   PIXELFMT_X8L8V8U8,      // 32-bit bump-map format with luminance using 8 bits for each channel. 
   PIXELFMT_A2W10V10U10,   // 32-bit bump-map format using 2 bits for alpha and 10 bits each for w, v, and u. 

   PIXELFMT_R16F,          // 16-bit float format using 16 bits for the red channel. 
   PIXELFMT_G16R16F,       // 32-bit float format using 16 bits for the red channel and 16 bits for the green channel. 
   PIXELFMT_A16B16G16R16F, // 64-bit float format using 16 bits for the each channel (alpha, blue, green, red). 

   PIXELFMT_R32F,          // 32-bit float format using 32 bits for the red channel. 
   PIXELFMT_G32R32F,       // 64-bit float format using 32 bits for the red channel and 32 bits for the green channel. 
   PIXELFMT_A32B32G32R32F, // 128-bit float format using 32 bits for the each channel (alpha, blue, green, red). 
};

/*!
 * Structure that can be used with PIXELFMT_R8G8B8 pixel format
 **/
struct GPixelR8G8B8
{
   UINT8 B;
   UINT8 G;
   UINT8 R;

};

/*!
 * Structure that can be used with PIXELFMT_A8R8G8B8 pixel format
 **/
struct GPixelA8R8G8B8
{
   UINT8 B;
   UINT8 G;
   UINT8 R;
   UINT8 A;
};

/*!
 * Structure that can be used with PIXELFMT_X8R8G8B8 pixel format
 **/
struct GPixelX8R8G8B8
{
   UINT8 B;
   UINT8 G;
   UINT8 R;
   UINT8 X;
};

/*!
 * Structure that can be used with PIXELFMT_A8B8G8R8 pixel format.
 * Can be casted directly into a GColorRGBInt class.
 **/
struct GPixelA8B8G8R8
{
   UINT8 R;
   UINT8 G;
   UINT8 B;
   UINT8 A;
};

/*!
 * Structure that can be used with PIXELFMT_R5G6B5 pixel format
 **/
struct GPixelR5G6B5
{
   UINT16 B:5;
   UINT16 G:6;
   UINT16 R:5;
};

/*!
 * Structure that can be used with PIXELFMT_X1R5G5B5 pixel format
 **/
struct GPixelX1R5G5B5
{
   UINT16 B:5;
   UINT16 G:5;
   UINT16 R:5;
   UINT16 X:1;
};

/*!
 * Structure that can be used with PIXELFMT_A1R5G5B5 pixel format
 **/
struct GPixelA1R5G5B5
{
   UINT16 B:5;
   UINT16 G:5;
   UINT16 R:5;
   UINT16 A:1;
};

/*!
 * Structure that can be used with PIXELFMT_A4R4G4B4 pixel format
 **/
struct GPixelA4R4G4B4
{
   UINT16 B:4;
   UINT16 G:4;
   UINT16 R:4;
   UINT16 A:4;
};

/*!
 * Structure that can be used with PIXELFMT_R3G3B2 pixel format
 **/
struct GPixelR3G3B2
{
   UINT8 B:3;
   UINT8 G:3;
   UINT8 R:2;
};

/*!
 * Structure that can be used with PIXELFMT_A8 pixel format
 **/
struct GPixelA8
{
   UINT8 A;
};

/*!
 * Structure that can be used with PIXELFMT_A8R3G3B2 pixel format
 **/
struct GPixelA8R3G3B2
{
   UINT8 B:3;
   UINT8 G:3;
   UINT8 R:2;
   UINT8 A;
};

/*!
 * Structure that can be used with PIXELFMT_X4R4G4B4 pixel format
 **/
struct GPixelX4R4G4B4
{
   UINT16 B:4;
   UINT16 G:4;
   UINT16 R:4;
   UINT16 X:4;
};

/*!
 * Structure that can be used with PIXELFMT_A2B10G10R10 pixel format
 **/
struct GPixelA2B10G10R108
{
   UINT32 R:10;
   UINT32 G:10;
   UINT32 B:10;
   UINT32 A:2;
};

/*!
 * Structure that can be used with PIXELFMT_G16R16 pixel format
 **/
struct GPixelG16R16
{
   UINT16 R;
   UINT16 G;
};

/*!
 * Structure that can be used with PIXELFMT_A2R10G10B10 pixel format
 **/
struct GPixelA2R10G10B10
{
   UINT32 B:10;
   UINT32 G:10;
   UINT32 R:10;
   UINT32 A:2;
};

/*!
 * Structure that can be used with PIXELFMT_A16B16G16R16 pixel format
 **/
struct GPixelA16B16G16R16
{
   UINT16 R;
   UINT16 G;
   UINT16 B;
   UINT16 A;
};

/*!
 * Structure that can be used with PIXELFMT_L8 pixel format
 **/
struct GPixelL8
{
   UINT8 L;
};

/*!
 * Structure that can be used with PIXELFMT_L16 pixel format
 **/
struct GPixelL16
{
   UINT16 L;
};

/*!
 * Structure that can be used with PIXELFMT_A8L8 pixel format
 **/
struct GPixelA8L8
{
   UINT8 L;
   UINT8 A;
};

/*!
 * Structure that can be used with PIXELFMT_A4L4 pixel format
 **/
struct GPixelA4L4
{
   UINT8 L:4;
   UINT8 A:4;
};


/*!
 * Structure that can be used with PIXELFMT_X8B8G8R8 pixel format.
 * Can be casted directly into a GColorRGBInt class 
 * (except that A component means nothing).
 **/
struct GPixelX8B8G8R8
{
   UINT8 R;
   UINT8 G;
   UINT8 B;
   UINT8 X;
};

/*!
 * Structure that can be used with PIXELFMT_V8U8 pixel format.
 **/
struct GPixelV8U8
{
   INT8 U;
   INT8 V;
};

/*!
 * Structure that can be used with PIXELFMT_Q8W8V8U8 pixel format.
 **/
struct GPixelQ8W8V8U8
{
   INT8 U;
   INT8 V;
   INT8 W;
   INT8 Q;
};

/*!
 * Structure that can be used with PIXELFMT_V16U16 pixel format.
 **/
struct GPixelV16U16
{
   INT16 U;
   INT16 V;
};

/*!
 * Structure that can be used with PIXELFMT_Q16W16V16U16 pixel format.
 **/
struct GPixelQ16W16V16U16
{
   INT16 U;
   INT16 V;
   INT16 W;
   INT16 Q;
};

/*!
 * Structure that can be used with PIXELFMT_CxV8U8 pixel format.
 **/
struct GPixelCxV8U8
{
   INT8 U;
   INT8 V;
};

/*!
 * Structure that can be used with PIXELFMT_L6V5U5 pixel format.
 **/
struct GPixelL6V5U5
{
   INT16 U:5;
   INT16 V:5;
   UINT16 L:6;
};

/*!
 * Structure that can be used with PIXELFMT_X8L8V8U8 pixel format.
 **/
struct GPixelX8L8V8U8
{
   INT8 U;
   INT8 V;
   UINT8 L;
   UINT8 X;
};

/*!
 * Structure that can be used with PIXELFMT_A2W10V10U10 pixel format.
 **/
struct GPixelA2W10V10U10
{
   INT32 U:10;
   INT32 V:10;
   INT32 W:10;
   UINT32 A:2;
};

/*!
 * Structure that can be used with PIXELFMT_R16F pixel format.
 **/
struct GPixelR16F
{
   REAL16 R;
};

/*!
 * Structure that can be used with PIXELFMT_G16R16F pixel format.
 **/
struct GPixelG16R16F
{
   REAL16 R;
   REAL16 G;
};

/*!
 * Structure that can be used with PIXELFMT_A16B16G16R16F pixel format.
 **/
struct GPixelA16B16G16R16F
{
   REAL16 R;
   REAL16 G;
   REAL16 B;
   REAL16 A;
};

/*!
 * Structure that can be used with PIXELFMT_R32F pixel format.
 **/
struct GPixelR32F
{
   REAL32 R;
};

/*!
 * Structure that can be used with PIXELFMT_G32R32F pixel format.
 **/
struct GPixelG32R32F
{
   REAL32 R;
   REAL32 G;
};

/*!
 * Structure that can be used with PIXELFMT_A32B32G32R32F pixel format.
 * Can be casted directly into a GColorRGBReal class.
 **/
struct GPixelA32B32G32R32F
{
   REAL32 R;
   REAL32 G;
   REAL32 B;
   REAL32 A;
};

/*!
 * Class that is used to make conversion between different types of pixels
 **/
class GPixelManager
{
private:
   // This makes sure the class cannot be instanciated
   GPixelManager() {;}

public:
   //! Returns the size (in bytes) of the specified pixel format
   static UINT32  GetPixelSize(EPixelFormat in_Format);

   //! Converts a pixel of the specified format into another pixel format
   /*!
    * This function does not work with paletted pixel formats
    */
   static bool    ConvertToPixelFormat(EPixelFormat in_SourceFormat, void* in_pSourcePixel, EPixelFormat in_DestFormat, void* out_pDestPixel);

   //! Converts a pixel of the specified format into another pixel format
   static bool    ConvertToPixelFormat(EPixelFormat in_SourceFormat, void* in_pSourcePixel, EPixelFormat in_PaletteFormat, void* in_pPaletteColors, UINT32 in_iNbPaletteColors, EPixelFormat in_DestFormat, void* out_pDestPixel);

   //! Converts a pixel of the specified format into a floating point RGB color
   /*!
    * This function does not work with paletted pixel formats
    */
   static bool    ConvertToRGBReal(EPixelFormat in_ColorFormat, void* in_pPixel, GColorRGBReal& out_Color);

   //! Converts a pixel of the specified format into a floating point RGB color
   static bool    ConvertToRGBReal(EPixelFormat in_ColorFormat, void* in_pPixel, EPixelFormat in_PaletteFormat, void* in_pPaletteColors, UINT32 in_iNbPaletteColors, GColorRGBReal& out_Color);

   //! Converts a pixel of the specified format into an integer RGB color
   /*!
    * This function does not work with paletted pixel formats
    */
   static bool    ConvertToRGBInt(EPixelFormat in_ColorFormat, void* in_pPixel, GColorRGBInt& out_Color);

   //! Converts a pixel of the specified format into an integer RGB color
   static bool    ConvertToRGBInt(EPixelFormat in_ColorFormat, void* in_pPixel, EPixelFormat in_PaletteFormat, void* in_pPaletteColors, UINT32 in_iNbPaletteColors, GColorRGBInt& out_Color);

   //! Converts a floating point RGB color into a pixel of the specified color format
   /*!
    * This function does not work with paletted pixel formats
    */
   static bool    ConvertFromRGBReal(const GColorRGBReal& in_Color, EPixelFormat in_ColorFormat, void* out_pPixel);

   //! Converts a floating point RGB color into a pixel of the specified color format
   static bool    ConvertFromRGBReal(const GColorRGBReal& in_Color, EPixelFormat in_PaletteFormat, void* in_pPaletteColors, UINT32 in_iNbPaletteColors, EPixelFormat in_ColorFormat, void* out_pPixel);

   //! Converts an integer RGB color into a pixel of the specified color format
   /*!
    * This function does not work with paletted pixel formats
    */
   static bool    ConvertFromRGBInt(const GColorRGBInt& in_Color, EPixelFormat in_ColorFormat, void* out_pPixel);

   //! Converts an integer RGB color into a pixel of the specified color format
   static bool    ConvertFromRGBInt(const GColorRGBInt& in_Color, EPixelFormat in_PaletteFormat, void* in_pPaletteColors, UINT32 in_iNbPaletteColors, EPixelFormat in_ColorFormat, void* out_pPixel);
};
}

#endif //#ifndef _GOLEM_PIXEL_H_