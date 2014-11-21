//SWIG-EXPORT 2
/**************************************************************

golem_gfx_overlay_itf.h

Owner : Germain Sauve

**************************************************************/

#ifndef _GOLEM_GFX_OVERLAY_ITF_
#define _GOLEM_GFX_OVERLAY_ITF_

#include "golem_gfx_obj_itf.h"

/*
Hack en atendant d'avoir les container

*/
typedef class COverlay_Itf : public CGFX_Obj_Itf
    {
    
public:
    virtual bool    LinkDrawObj( CGFX_Drawable_Obj_Itf* pObj ) = 0;
    virtual bool    UnlinkDrawObj() = 0;
    
    virtual bool MoveBack() = 0;
    virtual bool MoveForward() = 0;
    virtual bool BringToFront() = 0;
    virtual bool SendToBack() = 0;

    virtual UINT32  ZOrder() = 0;
    virtual void    ZOrder( UINT32 pOrder ) = 0;
    } *COverlay_Itf_Ptr;


#endif //#ifndef _GOLEM_GFX_OVERLAY_ITF_

