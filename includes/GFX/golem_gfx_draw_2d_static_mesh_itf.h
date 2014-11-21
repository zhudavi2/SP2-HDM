/*******************************************************************************

golem_gfx_draw_2d_static_mesh_itf.h

Owner : Germain Sauve

*******************************************************************************/

#ifndef _GOLEM_GFX_DRAW_2D_STATIC_MESH_ITF_
#define _GOLEM_GFX_DRAW_2D_STATIC_MESH_ITF_

#include "golem_gfx_obj_itf.h"
#include "golem_gfx_2d_static_mesh.h"

/*

Interface de base pour un objet d'affichage d'un static_mesh_2D

*/
typedef class CGFX_Draw_2D_Static_Mesh_Itf : public CGFX_Drawable_Obj_Itf
    {
public:
    virtual bool CreateFrom2DMesh( C2D_Static_Mesh_Ptr pMesh, bool pDynamicAccess ) = 0;
    virtual bool Load( CIO_Itf_Ptr pIO, bool pDynamicAccess ) = 0;
    } *CGFX_Draw_2D_Static_Mesh_Itf_Ptr;



#endif //#ifndef _GOLEM_GFX_DRAW_2D_STATIC_MESH_ITF_