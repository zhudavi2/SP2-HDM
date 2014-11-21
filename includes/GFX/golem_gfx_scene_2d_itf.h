/*******************************************************

golem_gfx_scene_2d_itf.h

Owner: Germain Sauve

********************************************************/

#ifndef _GOLEM_GFX_SCENE_2D_ITF_
#define _GOLEM_GFX_SCENE_2D_ITF_

#include "golem_gfx_obj_itf.h"
#include "golem_gfx_viewport_itf.h"

/*
    Interface de base pour contenir les information d'une scene en 2D
*/
typedef class CGFX_Scene_2D_Itf : public CGFX_Obj_Itf
    {
public:
    virtual bool Set_Viewport( CGFX_Viewport_Itf& pViewport ) = 0;
    } *CGFX_Scene_2D_Itf_Ptr;

#endif //#ifndef _GOLEM_GFX_SCENE_2D_ITF_