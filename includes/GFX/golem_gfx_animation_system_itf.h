/**************************************************************
*
* golem_gfx_animation_system_itf.h
*
* Description
* ===========
*	Class CGFX_Animation_System_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	June 04 2003 (foreilly):
*		- Added track support
*
**************************************************************/
#ifndef _GOLEM_GFX_ANIMATION_SYSTEM_ITF_
#define _GOLEM_GFX_ANIMATION_SYSTEM_ITF_

#include "golem_gfx_obj_itf.h"

//! Manage scene animations
class CGFX_Animation_System_Itf : public CGFX_Obj_Itf
{
protected:
	CGFX_Animation_System_Itf( UINT32 in_Type )
		: CGFX_Obj_Itf( in_Type )
	{}

public:
	//! Updates all animations in the scene
	virtual bool	Update_Animations(REAL32 in_Time) = 0;

	//! Play all animations
	virtual bool	Play_Animation() = 0;

	//! Stop all animations
	virtual bool	Stop_Animation() = 0;

	//! Get current animation time using internal timer
	virtual REAL32	Get_Animation_Time() const = 0;

	//! Get current animation time using specified time
	virtual REAL32	Get_Animation_Time(REAL32 in_RefTime) const = 0;

	//! Set the current animation time
	virtual bool	Set_Animation_Time(REAL32 in_Time) = 0;

	//! Play the action at the specified index
	virtual bool	Play_Action(UINT32 in_Index) = 0;

	//! Stop playing the action at the specified index
	virtual bool	Stop_Action(UINT32 in_Index) = 0;

	//! Stop all playing actions
	virtual bool	Stop_All_Actions() = 0;

	//! Get current action playing time
	virtual REAL32	Get_Action_Time(UINT32 in_Index) const = 0;

	//! Set current action playing time
	virtual bool	Set_Action_Time(UINT32 in_Index, REAL32 in_Time) = 0;

	//! Get the number of actions
	virtual UINT32	Get_Nb_Actions() const = 0;

	// Return the Id (index) of the specified action pointer
	virtual UINT32	Get_Action_Index(class CGFX_Action_Itf* in_Action) const = 0;

	//! Return the specified action by Id (index)
	virtual class CGFX_Action_Itf*	Get_Action( UINT32 in_Index ) = 0;

	//! Remove all actions
	virtual bool	Clear_Actions() = 0;

	//! Add an action
	virtual UINT32	Add_Action( class CGFX_Action_Itf* in_Action ) = 0;

	//! Remove an action by pointer
	virtual bool	Remove_Action( class CGFX_Action_Itf* in_Action ) = 0;

	//! Remove an action by Id (index)
	virtual bool	Remove_Action_At( UINT32 in_Index ) = 0;

	//! Get the number of tracks
	virtual UINT32	Get_Nb_Tracks() const = 0;

	// Return the Id (index) of the specified track pointer
	virtual UINT32	Get_Track_Index(class CGFX_Track_Itf* in_Track) const = 0;

	//! Return the specified track by Id (index)
	virtual class CGFX_Track_Itf*	Get_Track( UINT32 in_Index ) = 0;

	//! Remove all tracks
	virtual bool	Clear_Tracks() = 0;

	//! Add a track
	virtual UINT32	Add_Track( class CGFX_Track_Itf* in_Track ) = 0;

	//! Remove a track by pointer
	virtual bool	Remove_Track( class CGFX_Track_Itf* in_Track ) = 0;

	//! Remove a track by Id (index)
	virtual bool	Remove_Track_At( UINT32 in_Index ) = 0;

};

#endif //#ifndef _GOLEM_GFX_ANIMATION_SYSTEM_ITF_

