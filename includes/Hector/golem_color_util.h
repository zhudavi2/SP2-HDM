//SWIG-EXPORT  1
/***************************************************

golem_color_util.h

Owner : Germain Sauve


***************************************************/


#ifndef _GFX_COLOR_UTIL_
#define _GFX_COLOR_UTIL_

BEGIN_NAMESPACE(Hector)

// ---- image channels defines
#define IMAGE_CHANNEL_RED                1
#define IMAGE_CHANNEL_GREEN              2
#define IMAGE_CHANNEL_BLUE               3
#define IMAGE_CHANNEL_ALPHA              4
#define IMAGE_CHANNEL_LUMINANCE          5
#define IMAGE_CHANNEL_ALPHA_LUMINANCE    6
#define IMAGE_CHANNEL_RGB                8
#define IMAGE_CHANNEL_RGBA               9
#define IMAGE_CHANNEL_BGR                10
#define IMAGE_CHANNEL_ABGR               11
#define IMAGE_CHANNEL_ARGB               12

// packed pixel type
#define IMAGE_CHANNEL_R5G6B5             101
#define IMAGE_CHANNEL_R5G5B5             102
#define IMAGE_CHANNEL_R5G5B5A1           103
#define IMAGE_CHANNEL_A4L4               104
#define IMAGE_CHANNEL_A1R5G5B5           105
#define IMAGE_CHANNEL_A4R4G4B4           106

#define IMAGE_CHANNEL_UNKNOWN            200
// palette
#define IMAGE_CHANNEL_Index              1000

// ---- image type defines
#define IMAGE_CHANNEL_TYPE_UINT8                 1
#define IMAGE_CHANNEL_TYPE_UINT16                2
#define IMAGE_CHANNEL_TYPE_UINT32                3
#define IMAGE_CHANNEL_TYPE_REAL32                4
#define IMAGE_CHANNEL_TYPE_INT8                  10
#define IMAGE_CHANNEL_TYPE_INT16                 11
#define IMAGE_CHANNEL_TYPE_INT32                 12

// ---- image compression defines
#define IMAGE_COMP_UNCOMPRESS                    0
#define IMAGE_COMP_DXT1                          1
#define IMAGE_COMP_DXT2                          2
#define IMAGE_COMP_DXT3                          3
#define IMAGE_COMP_DXT4                          4
#define IMAGE_COMP_DXT5                          5



// ---- define color blending mode
#define k_COLORBLAND_COPY                           0
#define k_COLORBLEND_ADD                            1
#define k_COLORBLEND_SUBTRACT                       2
#define k_COLORBLEND_MODULATE                       3


class GColorInt
    {
public:
    union
        {
        UINT8   r;
        UINT8   lum;
        };
    UINT8 g, b, a;
    GColorInt();
    GColorInt( UINT8 pR, UINT8 pG, UINT8 pB, UINT8 pA = 0 );
    GColorInt( const GColorInt& pOther ) : r(pOther.r), g(pOther.g), b(pOther.b), a(pOther.a) {}

    void Set( UINT8 pR, UINT8 pG, UINT8 pB, UINT8 pA );

    GColorInt& operator += ( const GColorInt& pOther );
    GColorInt& operator -= ( const GColorInt& pOther );
    GColorInt& operator *= ( const GColorInt& pOther );
    GColorInt& operator /= ( const GColorInt& pOther );

    bool operator == ( const GColorInt& pOther ) const;
    };

class GColorReal
    {
public:
    union
        {
        REAL32   r;
        REAL32   lum;
        };
    REAL32 g, b, a;
    GColorReal();
    GColorReal( REAL32 pColor );
    GColorReal( const GColorInt& pColor );
    GColorReal( const GColorReal& pOther ) : r(pOther.r), g(pOther.g), b(pOther.b), a(pOther.a) {}
    GColorReal( REAL32 pR, REAL32 pG, REAL32 pB, REAL32 pA = 0.0f );

    void Set( REAL32 pR, REAL32 pG, REAL32 pB, REAL32 pA ); 

    GColorReal operator + ( const GColorReal& pOther ) const;
    GColorReal operator - ( const GColorReal& pOther ) const;
    GColorReal operator * ( const GColorReal& pOther ) const;
    GColorReal operator / ( const GColorReal& pOther ) const;

    GColorReal& operator += ( const GColorReal& pOther );
    GColorReal& operator -= ( const GColorReal& pOther );
    GColorReal& operator *= ( const GColorReal& pOther );
    GColorReal& operator /= ( const GColorReal& pOther );

    GColorReal& operator = ( const GColorReal& pOther );
    GColorReal& operator = ( const GColorInt& pOther );

    bool operator == ( const GColorReal& pOther ) const;
    bool operator != ( const GColorReal& pOther ) const;

    const GColorInt ToUINT8() const;
    void  Clamp();

	bool ToFormat(UINT32 in_Format, void* out_Color) const;
	bool FromFormat(UINT32 in_Format);

	};


/*!
 * Hue Saturation Brillance color format
 **/
class CHSBColorf
{
public:
	CHSBColorf();
	CHSBColorf(REAL32 h,REAL32 s,REAL32 b);
	~CHSBColorf();
	REAL32 h;
	REAL32 s;
	REAL32 b;

	void	  Set(REAL32 h,REAL32 s, REAL32 b);
	GColorInt To_RGBColori();

private:
	REAL32 Hue_To_RGB(REAL32 n1,REAL32 n2 ,REAL32 hue);

};


/*!
 * Hue saturation color format
 **/
class CHSColorf
{
public:
	CHSColorf();
	CHSColorf(REAL32 h,REAL32 s);
	REAL32 h;
	REAL32 s;

	GColorInt To_RGBColori();
	
};



/*!
 * CColor
 *		To Unify all the color types under one single type
 *		Only one color is being held at the time, so only
 *	    one of the members is valid at all times.
 **/
class CColor
{
	
public:

	CColor();
	CColor(GColorReal colorf);
	CColor(CHSBColorf colorf);
	CColor(CHSColorf  colorf);
enum COLOR_TYPE
	{
		UNKNOWN_TYPE =0,
		RGB_COLOR,
		HSB_COLOR,
		HS_COLOR
	};

	GColorReal m_RGBColorf;
	CHSBColorf m_HSBColorf;
	CHSColorf  m_HSColorf;

	COLOR_TYPE Type();


	GColorInt To_RGBColori();

private:
	COLOR_TYPE m_type;

};






typedef class GPixel
    {
public:
    virtual UINT32 Get_Channels() = 0;
    virtual UINT32 Get_Channel_Type() = 0;

    virtual void Write( GColorReal& pColor, UINT32 pChannels ) = 0;
    virtual void Write( GColorInt& pColor, UINT32 pChannels ) = 0;

    virtual void To_Color( GColorReal& pColor ) = 0;
    virtual void To_Color( GColorInt& pColor ) = 0;

    virtual UINT32 Size() = 0;

    virtual bool Set_Pixel_Source( void* pSource ) = 0;
    virtual bool Offset( INT32 pNumPixels ) = 0;
    virtual bool Inc() = 0;
    virtual bool Dec() = 0;
    } *GPixel*;


class CPixel_Manager
    {
public:
    CPixel_Manager();
    ~CPixel_Manager();
    
    GPixel* Get_Pixel_Itf( UINT32 pChannels, UINT32 pChannelType ); 
    };

// extern visible function
//! TODO: Remove this global symbol
UINT32 Get_ChannelType_Pixel_Size( UINT32 pChannels, UINT32 pChannelType );
bool Convert_Row( UINT32 pWidth, void *pSource, UINT32 pSouceChannels, UINT32 pSourceType, void *pDest, UINT32 pDestChannels, UINT32 pDestType );


//! TODO: Remove this global symbol
extern CPixel_Manager g_PixelManager;

END_NAMESPACE(Hector)

#endif //#ifndef _GFX_COLOR_UTIL_