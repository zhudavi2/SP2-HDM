/****************************************************************************

SDX_Sound.h

Owner : Frederic O'Reilly

History :

	March 27 2003 : foreilly
		- Creation

****************************************************************************/

#ifndef _SDX_SOUND_
#define _SDX_SOUND_

#include "../../include/golem_sfx_sound_itf.h"
#include "GingerSDX_Info.h"

class CSDX_Sound : public CSFX_Sound_Itf
{
	// TBM : Create class to manage format
	UINT32	m_NbChannels;
	UINT32	m_BitRate;
	UINT32	m_SampleRate;
	UINT32	m_NbSamples;

	UINT8	*m_SoundBuffer;

protected:
    ~CSDX_Sound();
public:
    CSDX_Sound();

// CSFX_Obj_Itf method overload
    bool	Release();

// CSFX_Sound_Itf method overload
	bool	Load_From_File( GFile& in_File );

	UINT32	Get_Nb_Channels();
	UINT32	Get_Bit_Rate();
	UINT32	Get_Sample_Rate();
	UINT32	Get_Nb_Samples();

	bool	Get_Sound_Buffer( UINT8 **in_Buffer );
	bool	Get_Sound_Buffer( INT16 **in_Buffer );

};

#endif //#ifndef _SDX_SOUND_
