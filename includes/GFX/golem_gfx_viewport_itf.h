/**************************************************************
*
* golem_gfx_viewport_itf.h
*
* Description
* ===========
*	Class CGFX_Viewport_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	April 15 2003 (foreilly):
*		- Creation
*
**************************************************************/
#ifndef _GOLEM_GFX_VIEWPORT_ITF_
#define _GOLEM_GFX_VIEWPORT_ITF_

#include "golem_gfx_primitive_itf.h"

//! Class used to create a viewport
class CGFX_Viewport_Itf : public CGFX_Primitive_Itf
{
protected:
	CGFX_Viewport_Itf( UINT32 in_Type )
		: CGFX_Primitive_Itf( in_Type )
	{}

public:
	//! Set the scene that will be displayed in the viewport
	virtual bool	Set_Scene( class CGFX_Scene_Itf* in_Scene ) = 0;

	//! Get the scene that is displayed in the viewport
	virtual class CGFX_Scene_Itf*	Get_Scene() = 0;

};

#endif //#ifndef _GOLEM_GFX_VIEWPORT_ITF_