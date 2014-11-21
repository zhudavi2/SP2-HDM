/****************************************************************************

SDX_Model.h

Owner : Frederic O'Reilly

History :

	March 7 2003 : foreilly
		- Creation

****************************************************************************/

#ifndef _GDX_MODEL_
#define _GDX_MODEL_

#include "../golem_sfx_context_itf.h"
#include "GingerSDX_Info.h"

class CSDX_Context : public CSFX_Context_Itf
{
	map<UINT32, IDirectSoundBuffer8 *> m_SoundBuffers;

protected:
    ~CSDX_Context();
public:
    CSDX_Context();

// CSFX_Obj_Itf method overload
    bool	Release();

// CSFX_Context_Itf method overload
	bool	Activate();
	bool	Deactivate();

	bool	Add_Sound( class CSFX_Sound_Itf* in_Sound );

	bool	Play_Sound( class CSFX_Sound_Itf* in_Sound, UINT8 in_iVolume = 64 );
	bool	Stop_Sound( class CSFX_Sound_Itf* in_Sound );

   void  Set_Sound_Volume( REAL32 in_fVolume );
   void  Set_Sound_Volume( INT32  in_fVolume );
   INT32 Get_Sound_Volume();

   //! Set panning 1 is total right and 0 is total left.
   void Set_Panning(REAL32 in_fPanning);

   //! Return true if the sound is currently playing
   bool IsSoundPlaying(CSFX_Sound_Itf*    in_Sound);

   IDirectSoundBuffer8* SoundBuffer( UINT32 in_iSoundBufferID );
   map<UINT32, IDirectSoundBuffer8 *> & SoundBuffers();
};

#endif //#ifndef _GDX_MODEL_