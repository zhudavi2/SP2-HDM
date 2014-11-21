/**************************************************************
*
* golem_gfx_material_Window_itf.h
*
* Description
* ===========
*	Class CGFX_Material_Window_Itf definition.
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
#ifndef	_GOLEM_GFX_MATERIAL_WINDOW_ITF_
#define	_GOLEM_GFX_MATERIAL_WINDOW_ITF_

#include "golem_gfx_material_itf.h"

namespace GFX
{
	//! Enumerates possible alpha map source
	enum EAlphaSource
	{
		ALPHASOURCE_NONE = 0,
		ALPHASOURCE_PICURE,
		ALPHASOURCE_ALPHAMAP,
	};
}

//! Class to describe a window material
typedef	class CGFX_Material_Window_Itf : public CGFX_Material_Itf
{
protected:
	CGFX_Material_Window_Itf( UINT32 in_Type )
		: CGFX_Material_Itf( in_Type )
	{}

public:
	//! Reset the color & alpha shader to its default function
	virtual bool	Reset_Window_Shader() = 0;

	//! Get the image texture shader
	virtual class CGFX_Shader_Texture_Itf*	Get_Image_Txtr_Shader() = 0;

	//! Get the alpha map texture shader
	virtual class CGFX_Shader_Texture_Itf*	Get_Alpha_Map_Txtr_Shader() = 0;

	//! Set an image to the window
	virtual bool	Set_Image( class GFile& in_File ) = 0;

	//! Set an alpha map to the window
	virtual bool	Set_Alpha_Map( class GFile& in_File ) = 0;

	//! Remove the image of the window
	virtual bool	Clear_Image() = 0;

	//! Remove the alpha map of the window
	virtual bool	Clear_Alpha_Map() = 0;

	//! Returns the window color parameter
	virtual CGFX_Color_Parameter& Color() = 0;

	//! Returns the alpha map source parameter
	virtual CGFX_Parameter&	Alpha_Source() = 0;

	//! Returns the master color enable flag
	virtual CGFX_Parameter&	Master_Enable() = 0;

}	*CGFX_Material_Window_Itf_Ptr;

#endif //#ifndef _GOLEM_GFX_MATERIAL_WINDOW_ITF_
