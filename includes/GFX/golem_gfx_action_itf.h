/**************************************************************
*
* golem_gfx_action_itf.h
*
* Description
* ===========
*	Class CGFX_Action_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	April 11 2003 (foreilly):
*		- Creation
*
**************************************************************/
#ifndef _GOLEM_GFX_ACTION_ITF_
#define _GOLEM_GFX_ACTION_ITF_

#include "golem_gfx_obj_itf.h"

//! Class that contains a collection of animations
class CGFX_Action_Itf : public CGFX_Obj_Itf
{
protected:
	CGFX_Action_Itf( UINT32 in_Type )
		: CGFX_Obj_Itf( in_Type )
	{}

public:
	//! Set the length of the action
	virtual bool	Length(REAL32 in_Length) = 0;

	//! Return the length of the action
	virtual REAL32	Length() = 0;

	//! Update all animations using specified time
	virtual bool	Update_Animations(REAL32 in_Time) = 0;

	//! Add an animation to the action
	virtual bool	Add_Animation(class CGFX_Animation_Itf* in_Animation) = 0;

	//! Remove the specified animation
	virtual bool	Remove_Animation(class CGFX_Animation_Itf* in_Animation) = 0;

	//! Remove the animation at the specified index
	virtual bool	Remove_Animation_At(UINT32 in_Index) = 0;

	//! Remove all animations from the action
	virtual bool	Clear_Animations() = 0;

	//! Return the number of animations in the action
	virtual UINT32	Get_Nb_Animations() = 0;

	//! Get the animation at the specified index
	virtual class CGFX_Animation_Itf*	Get_Animation(UINT32 in_Index) = 0;

	//! Get the index of the requested animation
	virtual UINT32	Get_Animation_Index( class CGFX_Animation_Itf* in_Animation ) = 0;

};

#endif //#ifndef _GOLEM_GFX_ACTION_ITF_

