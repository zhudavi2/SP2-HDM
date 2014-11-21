/**************************************************************
*
* golem_gfx_animation_cubic_itf.h
*
* Description
* ===========
*	Classes CGFX_Key_Cubic & CGFX_Animation_Cubic_Itf 
*	definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	June 03 2003 (foreilly):
*		- Added more key modification methods
*
**************************************************************/
#ifndef _GOLEM_GFX_ANIMATION_CUBIC_ITF_
#define _GOLEM_GFX_ANIMATION_CUBIC_ITF_

#include "golem_gfx_animation_itf.h"

struct CGFX_Key_Cubic
{
	REAL32	m_Time;
	REAL32	m_Value;
	REAL32	m_InTan;
	REAL32	m_OutTan;
	REAL32	m_InLength;
	REAL32	m_OutLength;
};

//! Animation using cubic interpolation
class CGFX_Animation_Cubic_Itf : public CGFX_Animation_Itf
{
protected:
	CGFX_Animation_Cubic_Itf(UINT32 in_Type)
		: CGFX_Animation_Itf(in_Type)
	{}

public:
	//! Setup the animation with an already filled data
	virtual bool	Setup_Keys(CGFX_Key_Cubic* in_Keys, UINT32 in_NbKeys) = 0;

	//! Add a key to the animation.
	virtual UINT32	Add_Key(CGFX_Key_Cubic in_Key) = 0;

	//! Remove the key at the specified index
	virtual bool	Remove_Key(UINT32 in_Index) = 0;

	//! Return the number of keys in the animation
	virtual UINT32	Get_Nb_Keys() const = 0;

	//! Remove all keys from the animation
	virtual bool	Clear_Keys() = 0;

	//! Fill an array with the keys in the animation
	virtual bool	Fill_Array_Keys(CGFX_Key_Cubic* out_KeyArray) const = 0;

	//! Get the key at the specified index
	virtual bool	Get_Key(UINT32 in_Index, CGFX_Key_Cubic& out_Key) const = 0;

	//! Set the key at the specified index
	virtual bool	Set_Key(UINT32 in_Index, CGFX_Key_Cubic& in_Key) = 0;

	//! Get a key reference to the spceified index
	virtual CGFX_Key_Cubic&	Key(UINT32 in_Index) = 0;

};

#endif //#ifndef _GOLEM_GFX_ANIMATION_CUBIC_ITF_

