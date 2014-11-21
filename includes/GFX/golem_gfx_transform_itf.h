/**************************************************************
*
* golem_gfx_transform_itf.h
*
* Description
* ===========
*  Class CGFX_Transform_Itf definition
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_GFX_TRANSFORM_ITF_
#define _GOLEM_GFX_TRANSFORM_ITF_

#include "golem_gfx_animated_obj_itf.h"

class CGFX_Transform_Itf : public CGFX_Animated_Obj_Itf
{
protected:
	CGFX_Transform_Itf(UINT32 in_Type)
		: CGFX_Animated_Obj_Itf(in_Type)
	{}

public:
	virtual bool Update() = 0;
	virtual bool Update_Matrix(GMatrix4x4<REAL32>& ) = 0;
	virtual GMatrix4x4<REAL32>& Get_Matrix() = 0;
	virtual bool Set_Matrix(const GMatrix4x4<REAL32>& in_Matrix) = 0;
};

#endif //#ifndef _GOLEM_GFX_TRANSFORM_ITF_
