/**************************************************************
*
* golem_gfx_primitive_itf.h
*
* Description
* ===========
*	Class CGFX_Primitive_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	April 09 2003 (foreilly):
*		- Creation
*
**************************************************************/
#ifndef _GOLEM_GFX_PRIMITIVE_ITF_
#define _GOLEM_GFX_PRIMITIVE_ITF_

#include "golem_gfx_render_obj_itf.h"

//! Class representing a Model primitive
class CGFX_Primitive_Itf : public CGFX_Render_Obj_Itf
{
protected:
	CGFX_Primitive_Itf( UINT32 in_Type )
		: CGFX_Render_Obj_Itf( in_Type )
	{}

};

#endif //#ifndef _GOLEM_GFX_PRIMITIVE_ITF_
