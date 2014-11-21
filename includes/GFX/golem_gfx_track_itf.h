/**************************************************************
*
* golem_gfx_track_itf.h
*
* Description
* ===========
*	Class CGFX_Track_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	June 04 2003 (foreilly):
*		- Creation
*
**************************************************************/
#ifndef	_GOLEM_GFX_TRACK_ITF_
#define	_GOLEM_GFX_TRACK_ITF_

#include "golem_gfx_render_obj_itf.h"

namespace GFX
{
	enum EPlayback_Type
	{
		PLAYBACK_FORWARD = 0,
		PLAYBACK_REVERSE,
		PLAYBACK_PING_PONG,
		PLAYBACK_PING_PONG_REVERSE,
	};

	enum ETrack_State
	{
		TRACK_NORMAL = 0,
		TRACK_SOLO,
		TRACK_MUTE,
	};

	enum ETrack_Mixing_Type
	{
		MIX_NONE = 0,
		MIX_QUICK,
		MIX_NORMALIZE,
	};
};

//! Track describing how an action is played over time and blended with other actions
class CGFX_Track_Itf : public CGFX_Animated_Obj_Itf
{
protected:
	CGFX_Track_Itf( UINT32 in_Type )
		: CGFX_Animated_Obj_Itf( in_Type )
	{}

public:
	//! Reset all parameters in the action of the track to 0
	virtual bool	Clear_Animated_Data() = 0;

	//! Update track animations
	virtual bool	Update_Animations(REAL32 in_Time) = 0;

	//! Get the track state parameter
	virtual CGFX_Parameter&	Track_State() = 0;

	//! Set the action of the track
	virtual bool	Set_Action(class CGFX_Action_Itf* in_Action) = 0;

	//! Get the action of the track
	virtual class CGFX_Action_Itf*	Get_Action() = 0;

	//! Set the playback type of the track
	virtual bool	Playback(GFX::EPlayback_Type in_PlaybackType) = 0;

	//! Get the playback type of the track
	virtual GFX::EPlayback_Type	Playback() const = 0;

	//! Set the number of repetitions
	//! Use 0 or lower for infinite repetition
	virtual bool	Playback_Count(REAL32 in_NbRepeats) = 0;

	//! Get the number of repetitions
	//! 0 means infinite repetition
	virtual REAL32	Playback_Count() const = 0;

	//! Set the start time of the track
	virtual bool	Set_Start_Time(REAL32 in_Time) = 0;

	//! Get the start time of the track
	virtual REAL32	Get_Start_Time() const = 0;

	//! Get the time when the track will end
	//! NOTE: If -1, it means this track will never end (infinite playback count)
	virtual REAL32	Get_Stop_Time() const = 0;

	//! (Re)Set the length to the action length
	virtual bool	Reset_Length() = 0;

	//! Set the length relative to the action length (length must be greater than or equal to 0)
	//! NOTE : this sets the action length for a single repeat
	virtual bool	Set_Length_Relative(REAL32 in_Time) = 0;

	//! Get the length relative to the action length
	virtual REAL32	Get_Length_Relative() const = 0;

	//! Set the length of the track (must be greater than or equal to 0)
	//! NOTE : this sets the action length for a single repeat
	virtual bool	Set_Length(REAL32 in_Time) = 0;

	//! Get the length of the track
	virtual REAL32	Get_Length() const = 0;

	//! Set the total length of the track
	//! NOTE : this sets the action length for all repeats
	//! Does not work for infinite repeat count
	virtual bool	Set_Length_Total(REAL32 in_Time) = 0;

	//! Get the total length of the track
	//! NOTE: If -1, it means this track will never end (infinite playback count)
	virtual REAL32	Get_Length_Total() const = 0;

	//! (Re)Set the offset to 0
	virtual bool	Reset_Offset() = 0;

	//! Set the offset relative to the action length
	virtual bool	Set_Offset_Relative(REAL32 in_Time) = 0;

	//! Get the offset relative to the action length
	virtual REAL32	Get_Offset_Relative() const = 0;

	//! Set the offset of the track 
	virtual bool	Set_Offset(REAL32 in_Time) = 0;

	//! Get the offset of the track
	virtual REAL32	Get_Offset() const = 0;

	//! Set a time warping animation
	//! The animation describes how the time evolves between [0, ActionLength]
	//! Default time warp goes from [0,0] to [1,1]
	virtual bool	Set_Time_Warp(class CGFX_Animation_Itf* in_TimeAnimation) = 0;

	//! Get the time warping animation
	virtual class CGFX_Animation_Itf*	Get_Time_Warp() = 0;

	//! Set the type of track mixing
	virtual bool	Set_Mixing_Type(GFX::ETrack_Mixing_Type in_MixType) = 0;

	//! Get the type of track mixing
	virtual GFX::ETrack_Mixing_Type	Get_Mixing_Type() = 0;

	//! Return the mixing factor parameter
	virtual CGFX_Parameter&	Mixing_Factor() = 0;

   //! Get if track must be extrapolated before its start time
   virtual bool   ExtrapolationBefore() const = 0;

   //! Set if track must be extrapolated before its start time
   virtual void   ExtrapolationBefore(bool in_bUseExtrapolationBefore) = 0;

   //! Get if track must be extrapolated after its start time
   virtual bool   ExtrapolationAfter() const = 0;

   //! Set if track must be extrapolated after its start time
   virtual void   ExtrapolationAfter(bool in_bUseExtrapolationAfter) = 0;
};

#endif //#ifndef _GOLEM_GFX_TRACK_ITF_
