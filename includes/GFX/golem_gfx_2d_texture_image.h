/********************************************************************************

golem_gfx_2d_texture_image.h

Owner : Germain Sauve

********************************************************************************/

#ifndef _GOLEM_GFX_2D_TEXTURE_IMG_ITF_
#define _GOLEM_GFX_2D_TEXTURE_IMG_ITF_

#include "golem_gfx_obj_itf.h"

/*
    Plus un hack qu'autre chose

    Permet d'afficher une texture ( ou une partie ) dans un context 2D
    Ne pertmet pas 
*/
typedef class CGFX_2D_Texture_Image_Itf : public CGFX_Drawable_Obj_Itf
    {

public:
    virtual bool Setup( UINT32 pTxtrID, CVector2Df& pPosition, CRGBColori& pColor,
        CRectangle& pTxtrAbsoluteRect ) = 0;

    virtual bool Set_Texture( UINT32 pID ) = 0;
    virtual bool Set_Texture( const std::string& pName ) = 0;

    virtual UINT32 Blending() = 0;
    virtual void   Blending( UINT32 pBlending ) = 0;

    virtual void Color( CRGBColori& pColor ) = 0;
    virtual CRGBColori Color() = 0;

    virtual CRectangle  Relative_Texture_Rect() = 0;
    virtual void        Relative_Texture_Rect( CRectangle& pRect ) = 0;

    virtual CRectangle  Absolute_Texture_Rect() = 0;
    virtual void        Absolute_Texture_Rect( CRectangle& pRect ) = 0;

    virtual void   Position( CVector2Df pPos ) = 0;
    virtual CVector2Df Position() = 0;
    } *CGFX_2D_Texture_Image_Itf_Ptr;


#endif //#ifndef _GOLEM_GFX_2D_TEXTURE_IMG_ITF_