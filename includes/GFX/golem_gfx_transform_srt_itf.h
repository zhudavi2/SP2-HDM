/*************************************************************

golem_gfx_transform_srt_itf.h

Owner : Frederic O'Reilly

*************************************************************/

#ifndef _GOLEM_GFX_TRANSFORM_SRT_ITF_
#define _GOLEM_GFX_TRANSFORM_SRT_ITF_

#include "golem_gfx_transform_itf.h"

class CGFX_Transform_SRT_Itf : public CGFX_Transform_Itf
{
protected:
	CGFX_Transform_SRT_Itf( UINT32 in_Type )
		: CGFX_Transform_Itf( in_Type )
	{}

public:
	virtual CGFX_Vector3_Parameter& Translation() = 0;
	virtual CGFX_Vector3_Parameter& Rotation() = 0;
	virtual CGFX_Vector3_Parameter& Scale() = 0;

};

#endif //#ifndef _GOLEM_GFX_TRANSFORM_SRT_ITF_
