/**************************************************************
*
* golem_gfx_animation_itf.h
*
* Description
* ===========
*	Class CGFX_Animation_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	May 30 2003 (foreilly):
*		- New design
*
**************************************************************/
#ifndef _GOLEM_GFX_ANIMATION_ITF_
#define _GOLEM_GFX_ANIMATION_ITF_

#include "golem_gfx_animated_obj_itf.h"

class CGFX_Animation_Itf : public CGFX_Obj_Itf
{
	CGFX_Animated_Obj_Itf*		m_ConnectedObject;
	GFX::EParameter_Type		m_ParamType;
	UINT32						m_ParamIndex;
	CGFX_Parameter*				m_Parameter;

protected:
	CGFX_Animation_Itf(UINT32 in_Type);

public:

	//! Connect the animation to an object parameter by type
	virtual bool	Connect(CGFX_Animated_Obj_Itf* in_Object, GFX::EParameter_Type in_ParamType);

	//! Connect the animation to an object parameter by name
	virtual bool	Connect(CGFX_Animated_Obj_Itf* in_Object, const GString& in_ParamName);

	//! Connect the animation to an object custom parameter by index
	virtual bool	Connect_Custom(CGFX_Animated_Obj_Itf* in_Object, UINT32 in_Index);

	//! Return the currently connected parameter
	virtual CGFX_Parameter&	Connected_Parameter();

	//! Return the object currently connected to the animation
	virtual CGFX_Animated_Obj_Itf*	Get_Connected_Object();

	//! Return the type of the parameter currently connected to the parameter
	virtual GFX::EParameter_Type	Get_Connected_Parameter_Type() const;

	//! Return the name of the parameter currently connected to the parameter
	virtual const GString&	Get_Connected_Parameter_Name() const;

	//! Return the size in bytes of a key entry in the data buffer
	virtual UINT32	Get_Data_Pitch() const;

	//! Return the offset in bytes where the value of the key is stored in each key structure
	virtual UINT32	Get_Data_Value_Offset() const;

	//! Return the size in bytes of the key data
	virtual UINT32	Get_Data_Size() const;

	//! Return the number of keys in data
	virtual UINT32	Get_Nb_Data_Keys() const;

	//! Fill a byte array of all key data
	virtual bool	Fill_Data(UINT8* out_DataArray) const;

	//! Setup all key data with content of byte array
	virtual bool	Setup_Data(UINT8* in_DataArray, UINT32 in_DataSize);

	//! Setup the animation. Must be called before interpolation can be calculated.
	virtual bool	Setup() = 0;

	//! Make the animation update its connected parameter
	virtual bool	Push_Animation(REAL32 in_Time);

	//! Make the animation update into the specified value reference
	virtual bool	Push_Animation(REAL32 in_Time, REAL32& out_Value) = 0;

};

#endif //#ifndef _GOLEM_GFX_ANIMATION_ITF_

