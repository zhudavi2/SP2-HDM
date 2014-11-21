/*************************************************************

golem_gfx_2d_text_itf.h

Owner : Frederic O'Reilly

History :
  
	March 6 2003 : foreilly
		- Creation

*************************************************************/

#ifndef _GOLEM_GFX_2D_TEXT_ITF_
#define _GOLEM_GFX_2D_TEXT_ITF_

#include "golem_gfx_primitive_itf.h"

typedef class CGFX_2D_Text_Itf : public CGFX_Primitive_Itf
{
public:
	virtual bool Init(GString& in_FontName, UINT32 in_Height, CRGBColori& in_Color) = 0;
	virtual bool Set_Text(GString& in_FontName) = 0;

	virtual UINT32 Text_Height() = 0;

	virtual UINT32 Height() = 0;
	virtual UINT32 Width() = 0;

} *CGFX_2D_Text_Itf_Ptr;

#endif //#ifndef _GOLEM_GFX_2D_TEXT_ITF_
