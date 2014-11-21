/**************************************************************
*
* golem_gfx_light_itf.h
*
* Description
* ===========
*	Class CGFX_Light_Itf definition.
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
#ifndef _GOLEM_GFX_LIGHT_ITF_
#define _GOLEM_GFX_LIGHT_ITF_

#include "golem_gfx_primitive_itf.h"

//! Class that describes a point light with color
class CGFX_Light_Itf : public CGFX_Primitive_Itf
{
protected:
	CGFX_Light_Itf(UINT32 in_Type)
		: CGFX_Primitive_Itf(in_Type)
	{}

public:
	//! Returns the light color parameter
	virtual CGFX_Color_Parameter&	Color() = 0;

	//! Returns the loght falloff parameter
	virtual CGFX_Parameter&		Falloff() = 0;

	//! Returns the light range parameter
	virtual CGFX_Parameter&		Range() = 0;

	//! Returns the light constant attenuation parameter
	virtual CGFX_Parameter&		Attenuation_Constant() = 0;

   //! Returns the light linear attenuation parameter
	virtual CGFX_Parameter&		Attenuation_Linear() = 0;

   //! Returns the light quadratic attenuation parameter
	virtual CGFX_Parameter&		Attenuation_Squared() = 0;

	//! Returns the light activation parameter
	virtual CGFX_Parameter&		On() = 0;

};

#endif //#ifndef _GOLEM_GFX_LIGHT_ITF_