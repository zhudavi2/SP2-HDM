/********************************************************

golem_gfx_color_frame_2d_itf.h

Owner : Germain Sauve

********************************************************/

#ifndef _GOLEM_GFX_COLOR_FRAME_2D_ITF_
#define _GOLEM_GFX_COLOR_FRAME_2D_ITF_

#include "golem_gfx_obj_itf.h"

/*
    
    Devrait servir comme bordure de base et font ( background ) de base

*/
typedef class CGFX_Color_Frame_2D_Itf : public CGFX_Drawable_Obj_Itf
    {
public:
    virtual bool    Position( CRectangle& pRect ) = 0;
    
    virtual UINT32  Blending() = 0;
    virtual void    Blending( UINT32 pBlending ) = 0;

    virtual bool    BorderColors( CRGBColori& pTLColor, CRGBColori& pTRColor, CRGBColori& pBRColor, CRGBColori& pBLColor ) = 0;
    virtual bool    InsideColors( CRGBColori& pTLColor, CRGBColori& pTRColor, CRGBColori& pBRColor, CRGBColori& pBLColor ) = 0;
    } *CGFX_Color_Frame_2D_Itf_Ptr;

#endif //#ifndef _GOLEM_GFX_COLOR_FRAME_2D_ITF_