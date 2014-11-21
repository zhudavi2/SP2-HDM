/*************************************************************

golem_gfx_transform_2d_itf.h

Owner : Frederic O'Reilly

*************************************************************/

#ifndef _GOLEM_GFX_TRANSFORM_2D_ITF_
#define _GOLEM_GFX_TRANSFORM_2D_ITF_

#include "golem_gfx_transform_itf.h"

class CGFX_Transform_2D_Itf : public CGFX_Transform_Itf
{
protected:
	CGFX_Transform_2D_Itf( UINT32 in_Type )
		: CGFX_Transform_Itf( in_Type )
	{}

public:
	virtual CGFX_Vector2_Parameter&	Position() = 0;
	virtual CGFX_Vector2_Parameter&	Size() = 0;

};

#endif //#ifndef _GOLEM_GFX_TRANSFORM_2D_ITF_
