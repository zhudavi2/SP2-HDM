/*****************************************************************

golem_rendering_buffer_itf.h

Owner : Germain Sauve

******************************************************************/



#ifndef _RENDERING_BUFFER_ITF_
#define _RENDERING_BUFFER_ITF_


#include "golem_gfx_obj_itf.h"

/*
    Utile pour offscreen rendering ( multiple render target )
*/
class CRendering_Buffer_Itf : public CGFX_Obj_Itf
    {
public:
    virtual bool On_Screen() = 0;
    virtual bool Used_As_Texture() = 0;

    virtual bool Activate() = 0;
    virtual bool DeActivate() = 0;

    virtual bool Get_Image( CGolem_Basic_Image2D_Ptr pImage ) = 0;
	virtual bool Get_Image( CGolem_Basic_Image2D_Ptr pImage, CRectangle& pCoord ) = 0;
	virtual bool Blit( CImage2D_Itf *pDestImage, CRectangle& pSourceRect, CVector2Di& pDestPos ) = 0;

    virtual bool Set_As_Texture() = 0;
    virtual bool Release_Texture() = 0;

    virtual bool Get_Mode( CRenderer_Init_Mode& pMode ) = 0;

	virtual bool Lock() = 0;
	virtual bool Unlock() = 0;
    };




#endif //#ifndef _RENDERING_BUFFER_ITF_