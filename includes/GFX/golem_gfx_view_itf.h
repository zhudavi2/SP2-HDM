/********************************************************************

golem_gfx_view_itf.h

Owner : Frederic O'Reilly

History :

	March 6 2003 : foreilly
		- Creation

********************************************************************/

#ifndef _GOLEM_GFX_VIEW_ITF_
#define _GOLEM_GFX_VIEW_ITF_

#include "golem_gfx_obj_itf.h"

#define k_GFX_View_None			0
#define k_GFX_View_Ortogonal	1
#define k_GFX_View_Perspective 	2

/*
	View interface
	Manages the projection matrix and the viewport
*/
typedef class CGFX_View_Itf : public CGFX_Drawable_Obj_Itf
{
public:
	virtual void	View_Type( UINT32 in_uType) = 0;
	virtual void	View_Type( UINT32 in_uType, REAL32 in_Width, REAL32 in_Height ) = 0;
	virtual UINT32	View_Type() = 0;
} *CGFX_View_Itf_Ptr;

#endif //#ifndef _GOLEM_GFX_VIEW_ITF_