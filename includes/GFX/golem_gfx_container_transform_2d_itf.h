/****************************************************************************

GDX_2D_Container.cpp

Owner : Germain Sauve

****************************************************************************/

#ifndef _GOLEM_GFX_CONTAINER_TRANSFORM_2D_ITF_
#define _GOLEM_GFX_CONTAINER_TRANSFORM_2D_ITF_

#include "golem_gfx_obj_itf.h"

/*
Container pouvent afficher un CGFX_Drawable_Obj 2D et les transformation

Devrait specifier si les transformation sont absolue ou a etre appliquer a celle courante 
*/
typedef class CGFX_Container_Transform_2D_Itf : public CGFX_Container_Itf
    {
public:
    virtual bool Set_Transform( CMatrix4x4f& pMatrix ) = 0;
    virtual bool Set_Rendered_Obj2D( CGFX_Drawable_Obj_Itf_Ptr pObj2D ) = 0;
    } *CGFX_Container_Transform_2D_Itf_Ptr;


#endif //#ifndef _GOLEM_GFX_CONTAINER_TRANSFORM_2D_ITF_