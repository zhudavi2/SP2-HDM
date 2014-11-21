/**************************************************************
*
* golem_gfx_render_list_itf.h
*
* Description
* ===========
*	Class CGFX_Render_List_Itf definition.
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
#ifndef _GOLEM_GFX_RENDER_LIST_ITF_
#define _GOLEM_GFX_RENDER_LIST_ITF_

#error Render List is Deprecated

#include "golem_gfx_obj_itf.h"

//! Manages a sequence of render items
class CGFX_Render_List_Itf : public CGFX_Obj_Itf
{
protected:
	CGFX_Render_List_Itf(UINT32 in_Type)
		: CGFX_Obj_Itf(in_Type)
	{}

};

#endif //#ifndef _GOLEM_GFX_RENDER_LIST_ITF_
