/*********************************************************************************

golem_gfx_directx_util.h

Owner : Germain Sauve

*********************************************************************************/

#ifndef _GOLEM_GFX_DIRECTX_UTIL_
#define _GOLEM_GFX_DIRECTX_UTIL_

#ifndef GOLEMFOURCC
    #ifdef HOST_IS_LE
        #define GOLEMFOURCC(ch0, ch1, ch2, ch3)                              \
                    ((UINT32)(UINT8)(ch0) | ((UINT32)(UINT8)(ch1) << 8) |   \
                    ((UINT32)(UINT8)(ch2) << 16) | ((UINT32)(UINT8)(ch3) << 24 ))
    #else
        #define GOLEMFOURCC(ch0, ch1, ch2, ch3)                              \
                    ((UINT32)(UINT8)(ch0)<<24 | ((UINT32)(UINT8)(ch1) << 16) |   \
                    ((UINT32)(UINT8)(ch2) << 8) | ((UINT32)(UINT8)(ch3)))
    #endif //#ifdef HOST_IS_LE
#endif //defined(MAKEFOURCC)


/*-------------------------------------

Constants used by DirectX defined if DX isn't use

--------------------------------------*/
#ifndef DIRECT3D_VERSION

// define all constants

//The dwFlags member of the original DDSURFACEDESC2structure can be set to one or more of the following values.
#define DDSD_CAPS 0x00000001 
#define DDSD_HEIGHT 0x00000002 
#define DDSD_WIDTH 0x00000004 
#define DDSD_PITCH 0x00000008 
#define DDSD_PIXELFORMAT 0x00001000 
#define DDSD_MIPMAPCOUNT 0x00020000 
#define DDSD_LINEARSIZE 0x00080000 
#define DDSD_DEPTH 0x00800000 

#define DDPF_ALPHAPIXELS 0x00000001 
#define DDPF_FOURCC 0x00000004 
#define DDPF_RGB 0x00000040 

//The dwCaps1 member of the DDSCAPS2 structure can be set to one or more of the following values.


#define DDSCAPS_COMPLEX 0x00000008 
#define DDSCAPS_TEXTURE 0x00001000 
#define DDSCAPS_MIPMAP 0x00400000 

//The dwCaps2 member of the DDSCAPS2 structure can be set to one or more of the following values.

#define DDSCAPS2_CUBEMAP 0x00000200 
#define DDSCAPS2_CUBEMAP_POSITIVEX 0x00000400 
#define DDSCAPS2_CUBEMAP_NEGATIVEX 0x00000800 
#define DDSCAPS2_CUBEMAP_POSITIVEY 0x00001000 
#define DDSCAPS2_CUBEMAP_NEGATIVEY 0x00002000 
#define DDSCAPS2_CUBEMAP_POSITIVEZ 0x00004000 
#define DDSCAPS2_CUBEMAP_NEGATIVEZ 0x00008000 
#define DDSCAPS2_VOLUME 0x00200000 

#define FCC_DXT1 GOLEMFOURCC( 'D' ,'X', 'T', '1' )
#define FCC_DXT2 GOLEMFOURCC( 'D' ,'X', 'T', '2' )
#define FCC_DXT3 GOLEMFOURCC( 'D' ,'X', 'T', '3' )
#define FCC_DXT4 GOLEMFOURCC( 'D' ,'X', 'T', '4' )
#define FCC_DXT5 GOLEMFOURCC( 'D' ,'X', 'T', '5' )

#endif //#ifndef DIRECT3D_VERSION



// define structure for DX wrapper
typedef struct golem_DDPIXELFORMAT
    {
    UINT32  dwSize;
    UINT32  dwFlags;
    UINT32  dwFourCC;
    UINT32  dwRGBBitCount;
    UINT32  dwRBitMask;
    UINT32  dwGBitMask;
    UINT32  dwBBitMask;
    UINT32  dwRGBAlphaBitMask;
    UINT32  dwCaps1;
    UINT32  dwCaps2;
    UINT32  Reserved[2];

    golem_DDPIXELFORMAT()
        {
        dwSize = 0;
        dwFlags = 0;
        dwFourCC = 0;
        dwRGBBitCount = 0;
        dwRBitMask = 0;
        dwGBitMask = 0;
        dwBBitMask = 0;
        dwRGBAlphaBitMask = 0;
        dwCaps1 = 0;
        dwCaps2 = 0;
        Reserved[0] = Reserved[1] = Reserved[2] = Reserved[3] = 0;
        }
/*
    bool Load( CEndian_IO& pEIO )
        {
        UINT32 lDummy;
        pEIO.Read_LEToHost_32( &dwSize, 4, lDummy );
        pEIO.Read_LEToHost_32( &dwFlags, 4, lDummy );
        pEIO.Read_LEToHost_32( &dwFourCC, 4, lDummy );
        pEIO.Read_LEToHost_32( &dwRGBBitCount, 4, lDummy );
        pEIO.Read_LEToHost_32( &dwRBitMask, 4, lDummy );
        pEIO.Read_LEToHost_32( &dwGBitMask, 4, lDummy );
        pEIO.Read_LEToHost_32( &dwBBitMask, 4, lDummy );
        pEIO.Read_LEToHost_32( &dwRGBAlphaBitMask, 4, lDummy );
        pEIO.Read_LEToHost_32( &dwCaps1, 4, lDummy );
        pEIO.Read_LEToHost_32( &dwCaps2, 4, lDummy );
        pEIO.Read_LEToHost_32( Reserved, 4*2, lDummy );
        return true;
        }
*/
    bool Get_ChannelAndType( UINT32& pChannels, UINT32& pChannelType, UINT32 pCompression )
        {
        /*
        if ( dwSize != 32 )
            return false; //error
        
        if ( dwFlags & DDPF_RGB )
            {
            switch ( dwRGBBitCount )
                {
                case 16:
                    pChannels = IMAGE_CHANNEL_R5G6B5;
                    pChannelType = IMAGE_CHANNEL_TYPE_UINT16;
                    pCompression = IMAGE_COMP_UNCOMPRESS;
                    break;
                case 24:
                    pChannels = IMAGE_CHANNEL_BGR;
                    pChannelType = IMAGE_CHANNEL_TYPE_UINT8;
                    pCompression = IMAGE_COMP_UNCOMPRESS;
                    break;
                case 32:
                    pChannels = IMAGE_CHANNEL_ABGR;
                    pChannelType = IMAGE_CHANNEL_TYPE_UINT8;
                    pCompression = IMAGE_COMP_UNCOMPRESS;
                    break;
                default:
                    return false;
                    break;
                }
            }
        else if ( dwFlags & DDPF_FOURCC )
            {
            //char lStr[10];
            //::memset( lStr, 0, 10 );
            //*((UINT32*)lStr) = dwFourCC;
            switch ( dwFourCC )
                {
                case FCC_DXT1:
                    pChannels = IMAGE_CHANNEL_ABGR;
                    pChannelType = IMAGE_CHANNEL_TYPE_UINT8;
                    pCompression = IMAGE_COMP_DXT1;
                    break;
                case FCC_DXT2:
                    pChannels = IMAGE_CHANNEL_ABGR;
                    pChannelType = IMAGE_CHANNEL_TYPE_UINT8;
                    pCompression = IMAGE_COMP_DXT2;
                    break;
                case FCC_DXT3:
                    pChannels = IMAGE_CHANNEL_ABGR;
                    pChannelType = IMAGE_CHANNEL_TYPE_UINT8;
                    pCompression = IMAGE_COMP_DXT3;
                    break;
                case FCC_DXT4:
                    pChannels = IMAGE_CHANNEL_ABGR;
                    pChannelType = IMAGE_CHANNEL_TYPE_UINT8;
                    pCompression = IMAGE_COMP_DXT4;
                    break;
                case FCC_DXT5:
                    pChannels = IMAGE_CHANNEL_ABGR;
                    pChannelType = IMAGE_CHANNEL_TYPE_UINT8;
                    pCompression = IMAGE_COMP_DXT5;
                    break;
                default:
                    return false; //unknown FOURCC type
                    break;
                }
            }
        */
        return true;
        }
    } *golem_DDPIXELFORMAT_Ptr;

typedef struct golem_DDSCAPS2
    {
    UINT32  lCaps;
    UINT32  lCaps1;
    UINT32  lCaps2;
    UINT32  lCaps3;

    golem_DDSCAPS2()
        {
        lCaps = 0;
        lCaps1 = 0;
        lCaps2 = 0;
        lCaps3 = 0;
        }
    /*
    bool Load( CEndian_IO& pEIO )
        {
        UINT32 lDummy;
        pEIO.Read_LEToHost_32( &lCaps, 4, lDummy );
        pEIO.Read_LEToHost_32( &lCaps1, 4, lDummy );
        pEIO.Read_LEToHost_32( &lCaps2, 4, lDummy );
        pEIO.Read_LEToHost_32( &lCaps3, 4, lDummy );
        return true;
        }
    */
    } *golem_DDSCAPS2_Ptr;

typedef struct golem_DDSURFACEDESC2
    {
    UINT32  dwSize;
    UINT32  dwFlags;
    UINT32  dwHeight;
    UINT32  dwWidth;
    UINT32  dwPitchOrLinearSize;
    UINT32  dwDepth;
    UINT32  dwMipMapCount;
    UINT32  dwReserved1[11];
    golem_DDPIXELFORMAT     ddpfPixelFormat;
    golem_DDSCAPS2          ddsCaps;
    UINT32  dwReserved2;

    golem_DDSURFACEDESC2()
        {
        dwSize = 0;
        dwFlags = 0;
        dwHeight = 0;
        dwWidth = 0;
        dwPitchOrLinearSize = 0;
        dwDepth = 0;
        dwMipMapCount = 0;
        for ( UINT32 lc = 0; lc < 11; lc++ )
            {
            dwReserved1[lc] = 0;
            }
        dwReserved2 = 0;
        }
/*
    bool Load( CEndian_IO& pEIO )
        {
        UINT32 lDummy;

        pEIO.Read_LEToHost_32( &dwSize, 4, lDummy );
        if ( dwSize != 124 )
            return false;

        pEIO.Read_LEToHost_32( &dwFlags, 4, lDummy );
        pEIO.Read_LEToHost_32( &dwHeight, 4, lDummy );
        pEIO.Read_LEToHost_32( &dwWidth, 4, lDummy );
        pEIO.Read_LEToHost_32( &dwPitchOrLinearSize, 4, lDummy );
        pEIO.Read_LEToHost_32( &dwDepth, 4, lDummy );
        pEIO.Read_LEToHost_32( &dwMipMapCount, 4, lDummy );
        pEIO.Read_LEToHost_32( dwReserved1, 4 * 11, lDummy );
        if ( ddpfPixelFormat.Load( pEIO ) == false )
            return false;
        if ( ddsCaps.Load( pEIO ) == false )
            return false;
        pEIO.Read_LEToHost_32( &dwReserved2, 4 , lDummy );
        return true;
        }
        */
    } *Golem_DDSURFACEDESC2_Ptr;





/*-------------------------------------

Define functions to bind golem_wrapper_struct to real DX struct

--------------------------------------*/
#ifdef DIRECT3D_VERSION

// inlude stuff to link to real D3D structure


#endif //#ifddef DIRECT3D_VERSION


#endif //#ifndef _GOLEM_GFX_DIRECTX_UTIL_