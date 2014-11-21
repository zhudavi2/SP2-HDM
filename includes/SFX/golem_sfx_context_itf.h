/*************************************************************

golem_sfx_context_itf.h

Owner : Frederic O'Reilly

History :
	March 27 2003 : foreilly
		- Creation

*************************************************************/

#ifndef _GOLEM_SFX_CONTEXT_ITF_
#define _GOLEM_SFX_CONTEXT_ITF_

#include "golem_sfx_obj_itf.h"

class CSFX_Context_Itf : public CSFX_Obj_Itf
{
public:
	virtual bool	Activate() = 0;
	virtual bool	Deactivate() = 0;

	virtual bool	Add_Sound( class CSFX_Sound_Itf* in_Sound ) = 0;

	virtual bool	Play_Sound( class CSFX_Sound_Itf* in_Sound,UINT8 in_iVolume = 64) = 0;
	virtual bool	Stop_Sound( class CSFX_Sound_Itf* in_Sound ) = 0;
   virtual void   Set_Panning(REAL32 in_fPanning) = 0;
   virtual bool   IsSoundPlaying(CSFX_Sound_Itf*    in_Sound) = 0;

};

#endif //#ifndef _GOLEM_SFX_CONTEXT_ITF_
