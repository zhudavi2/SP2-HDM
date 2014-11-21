/*******************************************************

golem_gfx_container_scene_2d_itf.h

Owner: Germain Sauve

********************************************************/

#ifndef _GOLEM_GFX_CONTAINER_SCENE_2D_ITF_
#define _GOLEM_GFX_CONTAINER_SCENE_2D_ITF_

#include "golem_gfx_obj_itf.h"
#include "golem_gfx_scene_2d_itf.h"


/*
    Container specifiant un viewport, projection et state de base 
*/
typedef class CGFX_Container_Scene_2D_Itf : public CGFX_Container_Itf
    {
public:
    virtual bool Set_Scene_2D( CGFX_Scene_2D_Itf_Ptr pScene2D ) = 0;
    } *CGFX_Container_Scene_2D_Itf_Ptr;


#endif //#ifndef _GOLEM_GFX_CONTAINER_SCENE_2D_ITF_