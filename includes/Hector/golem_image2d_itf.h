/*******************************************************

golem_image2d_itf.h


Owner: Germain Sauve

history:

- 17 dec 2002 : gsauve
Added Blit function

- 12 sept 2002
Creation of the interface

********************************************************/


#ifndef _GOLEM_IMAGE2D_
#define _GOLEM_IMAGE2D_

#include "golem_color.h"
#include "golem_pixel.h"
//#include "golem_io.h"
#include "golem_vector.h"
#include "golem_geometry.h"

namespace Hector
{

typedef class CImage2D_Itf
{
public:
    //virtual bool    Load( GIOItf& pIO ) = 0;
    //virtual bool    Save( GIOItf& PIO ) = 0;
    
    virtual bool Get_Pixel( GVector_2D<INT32>& pCoord, GColorRGBReal& pColor ) = 0;
    virtual bool Set_pixel( GVector_2D<INT32>& pCoord, GColorRGBReal& pColor ) = 0;

    virtual bool Get_Pixel( GVector_2D<INT32>& pCoord, GColorRGBInt& pColor ) = 0;
    virtual bool Set_pixel( GVector_2D<INT32>& pCoord, GColorRGBInt& pColor ) = 0;

    virtual bool Get_Row( GVector_2D<INT32>& pCoord, UINT32 pWidth, GColorRGBReal *pRowColor ) = 0;
    virtual bool Get_Row( GVector_2D<INT32>& pCoord, UINT32 pWidth, GColorRGBInt *pRowColor ) = 0;
    virtual bool Get_Row( GVector_2D<INT32>& pCoord, UINT32 pWidth, UINT32 pChannels, UINT32 pChannelType, void *pData ) = 0;

    virtual bool Set_Row( GVector_2D<INT32>& pCoord, UINT32 pWidth, GColorRGBReal *pRowColor ) = 0;
    virtual bool Set_Row( GVector_2D<INT32>& pCoord, UINT32 pWidth, GColorRGBInt *pRowColor ) = 0;
    virtual bool Set_Row( GVector_2D<INT32>& pCoord, UINT32 pWidth, UINT32 pChannels, UINT32 pChannelType, UINT32 pWriteChannels, void *pData ) = 0;
    
    virtual UINT32 Channels() = 0;
    virtual UINT32 ChannelType() = 0;
    virtual UINT32 Compression() = 0;

    virtual UINT32 Palette_Channels() = 0;
    virtual UINT32 Palette_ChannelType() = 0;

    virtual bool    Get_Palette_Color( UINT32 pIndex, GColorRGBInt& pColor ) = 0; 
    virtual bool    Get_Palette_Color( UINT32 pIndex, GColorRGBReal& pColor ) = 0;
    virtual UINT32  Palette_Size() = 0;

    virtual UINT32 Width() = 0;
    virtual UINT32 Height() = 0;

    virtual UINT32 RowBytes() = 0;

    
    virtual bool    Reverse_Height() = 0;
    virtual void*   Buffer() = 0;
    virtual void*   BufferAt( UINT32 pX, UINT32 pY ) = 0;

    virtual bool Blit( CImage2D_Itf *pDestImage, Geometry::GRectangle<UINT32>& pSourceRect, GVector_2D<INT32>& pDestPos, UINT32 pChannels ) = 0;
} *CImage2D_Itf_Ptr;

}

#endif //#ifndef _GOLEM_IMAGE2D_