/**************************************************************
*
* golem_gfx_material_simple_itf.h
*
* Description
* ===========
*	Class CGFX_Material_Simple_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	April 23 2003 (foreilly):
*		- Creation
*
**************************************************************/
#ifndef	_GOLEM_GFX_MATERIAL_SIMPLE_ITF_
#define	_GOLEM_GFX_MATERIAL_SIMPLE_ITF_

#include "golem_gfx_material_itf.h"

//! Class to describe a material with a single texture modulated with the material color
typedef	class CGFX_Material_Simple_Itf : public CGFX_Material_Itf
{
protected:
	CGFX_Material_Simple_Itf( UINT32 in_Type )
		: CGFX_Material_Itf( in_Type )
	{}

public:
	//! Reset the color & alpha shader to its default function
	virtual bool	Reset_Simple_Shader() = 0;

	//! Get the texture shader
	virtual class CGFX_Shader_Texture_Itf*	Get_Texture_Shader() = 0;

}	*CGFX_Material_Simple_Itf_Ptr;

#endif //#ifndef _GOLEM_GFX_MATERIAL_SIMPLE_ITF_
