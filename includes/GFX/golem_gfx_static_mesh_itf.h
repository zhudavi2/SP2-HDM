/******************************************************************

golem_gfx_static_mesh_itf.h

Owner : Germain Sauve

******************************************************************/

#ifndef _GOLEM_GFX_STATIC_MESH_ITF_
#define _GOLEM_GFX_STATIC_MESH_ITF_

#include "golem_gfx_obj_itf.h"
#include "golem_gfx_material_itf.h"

/*
Devrait etre flusher pour les 2D et 3D static Mesh
*/
typedef class CGFX_Static_Mesh : public CGFX_Drawable_Obj_Itf
    {
public:
    virtual bool Init( UINT32 pNumVertice, UINT32 pNumTriangles, 
        UINT32 pNumTxtrCoord, UINT32 pNumNormal, UINT32 pNumVColor, UINT23 pNumMaterial ) = 0;

    virtual bool Set_Vertice( UINT32 pIdx, CVector3Df& pVertice ) = 0;
    virtual bool Set_Normal( UINT32 pIdx, CVector3Df& pNormal ) = 0;
    virtual bool Set_TxtrCoord( UINT32 pIdx, CVector2Df& pTxtrCoord ) = 0;
    virtual bool Set_VColor( UINT32 pIdx, CRGBColorf& pVColor ) = 0;
    virtual bool Set_Material( UINT32 pIdx, CGFX_Material *pMaterial ) = 0;

    virtual bool Set_Triangle(  )

    } *CGFX_Static_Mesh_Ptr;

#endif //#ifndef _GOLEM_GFX_STATIC_MESH_ITF_