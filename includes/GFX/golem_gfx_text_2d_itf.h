#ifndef _GOLEM_GFX_TEXT_2D_ITF_
#define _GOLEM_GFX_TEXT_2D_ITF_

#include "golem_gfx_primitive_itf.h"

class CGFX_Text_2D_Itf : public CGFX_Primitive_Itf
{
protected:
	CGFX_Text_2D_Itf( UINT32 in_Type )
		: CGFX_Primitive_Itf( in_Type )
	{}

public:
	virtual CGFX_Color_Parameter& Color() = 0;

	virtual bool Init(const GString& in_FontName, UINT32 in_Height, bool in_Multiline) = 0;
	virtual bool Text(const GString& in_Text, UINT32 in_Width = 0) = 0;

	virtual UINT32 Text_Height() = 0;

	virtual UINT32 Height() = 0;
	virtual UINT32 Width() = 0;
	virtual UINT32 WidthMax() = 0;

};

#endif //#ifndef _GOLEM_GFX_TEXT_2D_ITF_
