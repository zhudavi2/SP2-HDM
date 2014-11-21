/**************************************************************
*
* golem_gfx_primitive_itf.h
*
* Description
* ===========
*	Class CGFX_Primitive_Itf definition.
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
#ifndef _GOLEM_GFX_CAMERA_ITF_
#define _GOLEM_GFX_CAMERA_ITF_

#include "golem_gfx_primitive_itf.h"

namespace GFX
{
	// Enumerates possible view types
	enum EView_Type
	{
		VIEW_NONE = 0,
		VIEW_ORTHOGONAL,
		VIEW_PERSPECTIVE,
	};
};

//! Describes a camera and its associated view
class CGFX_Camera_Itf : public CGFX_Primitive_Itf
{
protected:
	CGFX_Camera_Itf( UINT32 in_Type )
		: CGFX_Primitive_Itf( in_Type )
	{}

public:
	//! Set the view type of the camera
	virtual bool	View_Type( GFX::EView_Type in_Type) = 0;

	//! Get the view type of the camera
	virtual GFX::EView_Type	View_Type() = 0;

	//! Get the FOV parameter
	virtual CGFX_Parameter&	FOV() = 0;

	//! Get the aspect ratio parameter
	virtual CGFX_Parameter&	Aspect_Ratio() = 0;

	//! Get the near plane parameter
	virtual CGFX_Parameter&	Near_Plane() = 0;

	//! Get the far plane parameter
	virtual CGFX_Parameter&	Far_Plane() = 0;

	//! Get the left parameter (only used for ortho projection)
	virtual CGFX_Parameter&	Left() = 0;

	//! Get the top parameter (only used for ortho projection)
	virtual CGFX_Parameter&	Top() = 0;

	//! Get the right parameter (only used for ortho projection)
	virtual CGFX_Parameter&	Right() = 0;

	//! Get the bottom parameter (only used for ortho projection)
	virtual CGFX_Parameter&	Bottom() = 0;

	//! Get the look at parameter
	//! \todo TBR : Use contraint system instead
	virtual CGFX_Vector3_Parameter& Look_At() = 0;

	//! Get the up vector parameter
	//! \todo TBR : Use contraint system instead
	virtual CGFX_Vector3_Parameter& Up_Vector() = 0;

	//! Get the view matrix (rotation only)
	//! \todo TBR : Use transform instead
	virtual GMatrix4x4<REAL32> Get_View_Matrix(GMatrix4x4<REAL32>& in_Transform) = 0;

	//! Get the projection matrix
	virtual GMatrix4x4<REAL32> Get_Projection_Matrix() = 0;

};

#endif //#ifndef _GOLEM_GFX_CAMERA_ITF_