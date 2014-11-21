/******************************************************************

golem_gfx_image_tga.h

Owner : Germain Sauve

******************************************************************/


#ifndef _GOLEM_GFX_IMAGE_TGA_
#define _GOLEM_GFX_IMAGE_TGA_

#include "../../includes/hector/hector.h"

enum e_TGA_ImageDataType
    {
    e_TGA_NoImageData               =   0,
    e_TGA_ColormappedImageData      =   1,
    e_TGA_TruecolorImageData        =   2,
    e_TGA_MonochromeImageData       =   3,
    e_TGA_ColormappedImageDataComp  =   9,
    e_TGA_TruecolorImageDataComp    =   10,
    e_TGA_MonochromeImageDataComp   =   11
    };

class CTGA_Header
    {
public:
    UINT8   IDLength;
    UINT8   ColorMapType;
    UINT8   ImageType;
    UINT16  CMapStart;
    UINT16  CMapLength;
    UINT8   CMapDepth;
    UINT16  XOffset;
    UINT16  YOffset;
    UINT16  Width;
    UINT16  Height;
    UINT8   PixelDepth;
    UINT8   ImageDescriptor;

    bool Load( CEndian_IO& pEIO )
        {
        UINT32 lDummy;
        pEIO.Read( &IDLength, 1, lDummy );
        pEIO.Read( &ColorMapType, 1, lDummy );
        pEIO.Read( &ImageType, 1, lDummy );
        pEIO.Read_LEToHost_16( &CMapStart, 2, lDummy );
        pEIO.Read_LEToHost_16( &CMapLength, 2, lDummy );
        pEIO.Read( &CMapDepth, 1, lDummy );
        pEIO.Read_LEToHost_16( &XOffset, 2, lDummy );
        pEIO.Read_LEToHost_16( &YOffset, 2, lDummy );
        pEIO.Read_LEToHost_16( &Width, 2, lDummy );
        pEIO.Read_LEToHost_16( &Height, 2, lDummy );
        pEIO.Read( &PixelDepth, 1, lDummy );
        pEIO.Read( &ImageDescriptor, 1, lDummy );
        return true;
        }

    };

class CImage2D_TGA : public CImage2D_Itf
    {
    CMemoryBuffer   m_Buffer;
    UINT32          m_Width, m_Height;
    UINT32          m_Channels, m_ChannelType;
    UINT32          m_RowBytes;

    bool    LoadTGA_ColorMap( CIO_Itf& pIO, CTGA_Header& pHeader );
    bool    LoadTGA_TrueColor( CIO_Itf& pIO, CTGA_Header& pHeader );
    bool    LoadTGA_Monochrome( CIO_Itf& pIO, CTGA_Header& pHeader );
    bool    LoadTGA_ColorMapComp( CIO_Itf& pIO, CTGA_Header& pHeader );
    bool    LoadTGA_TrueColorComp( CIO_Itf& pIO, CTGA_Header& pHeader );
    bool    LoadTGA_MonochromeComp( CIO_Itf& pIO, CTGA_Header& pHeader );
public:
    CImage2D_TGA();
    ~CImage2D_TGA();

    bool    Load( CIO_Itf& pIO );
    bool    Save( CIO_Itf& PIO );

    bool Get_Pixel( CVector2Di& pCoord, CRGBColorf& pColor );
    bool Set_pixel( CVector2Di& pCoord, CRGBColorf& pColor );

    bool Get_Pixel( CVector2Di& pCoord, CRGBColori& pColor );
    bool Set_pixel( CVector2Di& pCoord, CRGBColori& pColor );

    bool Get_Row( CVector2Di& pCoord, UINT32 pWidth, CRGBColorf *pRowColor );
    bool Get_Row( CVector2Di& pCoord, UINT32 pWidth, CRGBColori *pRowColor );
    bool Get_Row( CVector2Di& pCoord, UINT32 pWidth, UINT32 pChannels, UINT32 pChannelType, void *pData );

    bool Set_Row( CVector2Di& pCoord, UINT32 pWidth, CRGBColorf *pRowColor );
    bool Set_Row( CVector2Di& pCoord, UINT32 pWidth, CRGBColori *pRowColor );
    bool Set_Row( CVector2Di& pCoord, UINT32 pWidth, UINT32 pChannels, UINT32 pChannelType, void *pData );
    
    UINT32 Channels();
    UINT32 ChannelType();

    UINT32 Palette_Channels();
    UINT32 Palette_ChannelType();

    bool    Get_Palette_Color( UINT32 pIndex, CRGBColori& pColor ); 
    bool    Get_Palette_Color( UINT32 pIndex, CRGBColorf& pColor );
    UINT32  Palette_Size();

    UINT32 Width();
    UINT32 Height();

    UINT32 RowBytes();
    
    bool Reverse_Height();
    void *Buffer();
    void*   BufferAt( UINT32 pX, UINT32 pY );

    bool Blit( CImage2D_Itf *pDestImage, CRectangle& pSourceRect, CVector2Di& pDestPos, UINT32 pChannels );
    };

#endif //#ifndef _GOLEM_GFX_IMAGE_TGA_

//EOF 

