/*****************************************

golem_gfx_conainer_trans2d_itf.h.h

Owner: Germain Sauve

******************************************/


#ifndef _GOLEM_GFX_CONTAINER_TRANS2D_ITF_
#define _GOLEM_GFX_CONTAINER_TRANS2D_ITF_


#include "golem_gfx_obj_itf.h"

typedef class CGFX_Container_Trans2D_Itf : public CGFX_Obj_Itf
    {
public:
    virtual bool Set_Translation( CVector2Df& pTrans ) = 0;
    virtual bool Get_Translation( CVector2Df& pTrans ) = 0;

    virtual bool Set_Scale( CVector2Df& pScale ) = 0;
    virtual bool Get_Scale( CVector2Df& pScale ) = 0;


    } *CGFX_Container_Trans2D_Itf_Ptr;



#endif //#ifndef _GOLEM_GFX_CONTAINER_TRANS2D_ITF_