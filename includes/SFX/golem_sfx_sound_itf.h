/*************************************************************

golem_sfx_sound_itf.h

Owner :	Frederic O'Reilly

History	:
	March 27 2003 : foreilly
		- Creation

*************************************************************/

#ifndef	_GOLEM_SFX_SOUND_ITF_
#define	_GOLEM_SFX_SOUND_ITF_

#include "golem_sfx_obj_itf.h"

class CSFX_Sound_Itf : public CSFX_Obj_Itf
{
public:
	virtual	bool	Load_From_File( GFile& in_File ) = 0;
	virtual UINT32	Get_Nb_Channels() = 0;
	virtual UINT32	Get_Bit_Rate() = 0;
	virtual UINT32	Get_Sample_Rate() = 0;
	virtual UINT32	Get_Nb_Samples() = 0;

	virtual bool	Get_Sound_Buffer( UINT8 **in_Buffer ) = 0;
	virtual bool	Get_Sound_Buffer( INT16 **in_Buffer ) = 0;

};

#endif //#ifndef _GOLEM_SFX_SOUND_ITF_
