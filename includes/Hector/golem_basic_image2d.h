/*******************************************************

golem_basic_image2d.h

Owner: Germain Sauve

********************************************************/


#ifndef _GOLEM_BASIC_IMAGE2D_
#define _GOLEM_BASIC_IMAGE2D_

#include "golem_image2d_itf.h"
#include "golem_memory_buffer.h"

namespace Hector
{

typedef class CGolem_Basic_Image2D : public CImage2D_Itf
    {
    GMemory_Buffer m_Buffer;
    UINT32 m_Width;
    UINT32 m_Height;
    UINT32 m_RowBytes;

    UINT32 m_Channel;
    UINT32 m_ChannelType;

    UINT32 m_PaletteChannel;
    UINT32 m_PaletteChannelType;

    UINT32 m_PaletteSize;

    UINT32 m_Compression;

public:
    CGolem_Basic_Image2D();
    ~CGolem_Basic_Image2D();

    bool Setup( UINT32 pWidth, UINT32 pHeight, UINT32 pChannels, UINT32 pChannelsFormat, UINT32 pPaletteChannels, UINT32 pPaletteChannelFormat, UINT32 pPaletteSize );
    bool Setup_Other_Buffer( UINT32 pWidth, UINT32 pHeight, UINT32 pChannels, UINT32 pChannelsFormat,
            UINT32 pPaletteChannels, UINT32 pPaletteChannelFormat, UINT32 pPaletteSize,
            UINT32 pRowBytes, const void* pBuffer, const void* pPaletteBuffer );

// overload methods of golem_image2d_itf
    //bool    Load( GIOItf& pIO );
    //bool    Save( GIOItf& PIO );
    
    bool Get_Pixel( GVector_2D<INT32>& pCoord, GColorRGBReal& pColor );
    bool Set_pixel( GVector_2D<INT32>& pCoord, GColorRGBReal& pColor );

    bool Get_Pixel( GVector_2D<INT32>& pCoord, GColorRGBInt& pColor );
    bool Set_pixel( GVector_2D<INT32>& pCoord, GColorRGBInt& pColor );

    bool Get_Row( GVector_2D<INT32>& pCoord, UINT32 pWidth, GColorRGBReal *pRowColor );
    bool Get_Row( GVector_2D<INT32>& pCoord, UINT32 pWidth, GColorRGBInt *pRowColor );
    bool Get_Row( GVector_2D<INT32>& pCoord, UINT32 pWidth, UINT32 pChannels, UINT32 pChannelType, void *pData );

    bool Set_Row( GVector_2D<INT32>& pCoord, UINT32 pWidth, GColorRGBReal *pRowColor );
    bool Set_Row( GVector_2D<INT32>& pCoord, UINT32 pWidth, GColorRGBInt *pRowColor );
    bool Set_Row( GVector_2D<INT32>& pCoord, UINT32 pWidth, UINT32 pChannels, UINT32 pChannelType, UINT32 pWriteChannels, void *pData );
    
    UINT32 Channels();
    UINT32 ChannelType();
    UINT32 Compression();

    UINT32 Palette_Channels();
    UINT32 Palette_ChannelType();

    bool    Get_Palette_Color( UINT32 pIndex, GColorRGBInt& pColor ); 
    bool    Get_Palette_Color( UINT32 pIndex, GColorRGBReal& pColor );
    UINT32  Palette_Size();

    UINT32 Width();
    UINT32 Height();

    UINT32 RowBytes();

    
    bool Reverse_Height();
    void*   Buffer();
    void*   BufferAt( UINT32 pX, UINT32 pY );

    bool Blit( CImage2D_Itf *pDestImage, Geometry::GRectangle<UINT32>& pSourceRect, GVector_2D<INT32>& pDestPos, UINT32 pChannels );

    } *CGolem_Basic_Image2D_Ptr;

}

#endif //#ifndef _GOLEM_BASIC_IMAGE2D_