//SWIG-EXPORT 2
/************************************************************

golem_gfx_2dframe_itf.h

Owner : Germain Sauve

************************************************************/


#ifndef _GOLEM_GFX_2DFRAME_ITF_
#define _GOLEM_GFX_2DFRAME_ITF_

#include "golem_gfx_obj_itf.h"

/*
To be flushed

Used in OGL to test 
*/
typedef class CGFX_2D_Frame_Itf : public CGFX_Drawable_Obj_Itf
    {
    
public:
    virtual bool Set( CRectangle& pRect, CRGBColorf& pColor ) = 0;

    virtual void Inset( REAL32 pX, REAL32 pY ) = 0;
    virtual void Inset( CVector2Df& pInset ) = 0;
    
    virtual void Offset( REAL32 pX, REAL32 pY ) = 0;
    virtual void Offset( CVector2Df& pOffset ) = 0;

    virtual void Color( CRGBColorf& pColor ) = 0;
    virtual void Get_Color( CRGBColorf& pColor ) = 0;

    virtual void Colors( CRGBColorf& pCol1, CRGBColorf& pCol2, CRGBColorf& pCol3, CRGBColorf& pCol4 ) = 0;
    
    virtual bool Use_Texture() = 0;
    virtual void Use_Texture( bool pValue ) = 0;
    
    virtual bool Set_Texture( std::string& pTextureName ) = 0;
    virtual UINT32 Get_TextureID() = 0;

    virtual bool Set_Texture_Coord_Relative( CVector2Df& pTC0, CVector2Df& pTC1, CVector2Df& pTC2, CVector2Df& pTC3 ) = 0;
    virtual bool Set_Texture_Coord_Absolute( CVector2Di& pTC0, CVector2Di& pTC1, CVector2Di& pTC2, CVector2Di& pTC3 ) = 0;
    } *CGFX_2D_Frame_Itf_Ptr;


#endif //#ifndef _GOLEM_GFX_2DFRAME_ITF_