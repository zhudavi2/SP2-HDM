/**************************************************************
*
* golem_gfx_animation_static_itf.h
*
* Description
* ===========
*	Class CGFX_Animation_Static_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	June 03 2003 (foreilly):
*		- Added more value modification methods
*
**************************************************************/
#ifndef _GOLEM_GFX_ANIMATION_STATIC_ITF_
#define _GOLEM_GFX_ANIMATION_STATIC_ITF_

#include "golem_gfx_animation_itf.h"

class CGFX_Animation_Static_Itf : public CGFX_Animation_Itf
{
protected:
	CGFX_Animation_Static_Itf(UINT32 in_Type)
		: CGFX_Animation_Itf(in_Type)
	{}

public:
	//! Set the static value
	virtual bool	Set_Value(REAL32 in_StaticValue) = 0;

	//! Get the static value
	virtual REAL32	Get_Value() const = 0;

	//! Return a reference to the static value
	virtual REAL32&	Value() = 0;

};

#endif //#ifndef _GOLEM_GFX_ANIMATION_STATIC_ITF_

