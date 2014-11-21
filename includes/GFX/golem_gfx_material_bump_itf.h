/**************************************************************
*
* golem_gfx_material_bump_itf.h
*
* Description
* ===========
*	Class CGFX_Material_Bump_Itf definition.
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
#ifndef	_GOLEM_GFX_MATERIAL_BUMP_ITF_
#define	_GOLEM_GFX_MATERIAL_BUMP_ITF_

#include "golem_gfx_material_itf.h"

//! Class to describe a material with a single texture and a bump map modulated with the material color
class CGFX_Material_Bump_Itf : public CGFX_Material_Itf
{
protected:
	CGFX_Material_Bump_Itf( UINT32 in_Type )
		: CGFX_Material_Itf( in_Type )
	{}

public:
	//! Reset the color & alpha shader to its default function
	virtual bool	Reset_Bump_Shader() = 0;

	//! Get the texture shader
	virtual class CGFX_Shader_Texture_Itf*	Get_Texture_Shader() = 0;

	//! Get the bump texture shader
	virtual class CGFX_Shader_Texture_Itf*	Get_Bump_Texture_Shader() = 0;

};

#endif //#ifndef _GOLEM_GFX_MATERIAL_BUMP_ITF_
