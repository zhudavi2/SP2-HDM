/*******************************************************

golem_sfx_player_itf.h

Owner: Frederic	O'Reilly

history:
	- March	27 2003
		Creation of	the	interface

********************************************************/

#ifndef	_GOLEM_SFX_PLAYER_ITF_
#define	_GOLEM_SFX_PLAYER_ITF_

#include "golem_sfx_context_itf.h"
#include "golem_sfx_sound_itf.h"

// Foward Declarations
class CSFX_WaveFile_Itf;
class CSDX_Info;

class CSFX_Player_Itf
{
public:
	// link	the	player to the engine
	virtual	bool	Startup( GEngineInfo *in_Engine_Info) = 0;

	// return the last error 
	virtual	bool	GetLastError( GString&	in_ErrorString ) = 0;

	// init	the	player for a specific playing mode and sound device
	virtual	bool	Init( CSFX_Player_Init_Mode& in_InitMode ) = 0;

	// close the init sound device
	virtual	bool	Close() = 0;

	// release the player from the engine
	virtual	bool	Release() = 0;

   //! \todo TBR : Not a clean way to do this
   virtual bool   PlayMusicFile(GFile& in_File) = 0;

   // Stop the music (See above comment)
   virtual bool   StopMusic(void ) = 0;

	// TBA : device detection & selection 

	//------------------ Get Object	Method

	// Return a newly created context
	virtual CSFX_Context_Itf*   	Get_New_Context() = 0;

	// Return a previously created context
	virtual CSFX_Context_Itf*   	Get_Context( UINT32 in_ID ) = 0;

	// Return a newly created sound
	virtual CSFX_Sound_Itf*   	Get_New_Sound() = 0;

   // Return a newly created wavefile 
   virtual CSFX_WaveFile_Itf*   Get_New_WaveFile() = 0;

	// Return a previously created sound
	virtual CSFX_Sound_Itf*   	Get_Sound( UINT32 in_ID ) = 0;

   // Return the SDX info
   virtual CSDX_Info&                 Get_SDX_Info() = 0;

   //------------------- Get/Set volume Method

   // Get sound volume
   virtual REAL32 Get_Sound_Volume( UINT32 in_iContextID ) = 0;
   // Set sound volume
   virtual void  Set_Sound_Volume( UINT32 in_iContextID, REAL32 in_fVolume ) = 0;

   // Iterate the sound player for delayed sound and other process
   virtual void Iterate(void) = 0;

   //! Ask the player to play a sound with after a certain delay
   virtual void PlayDelayedSound(CSFX_Context_Itf* in_pSoundContext, CSFX_Sound_Itf* in_pSoundFile, REAL32 in_fDelayInSecond) = 0;

   //! Return in how long a particular sound will be playing. return -1 if sounds is playing or have played or will not be playing.
   virtual REAL32 GetTimeForDelayedSound(CSFX_Context_Itf* in_pSoundContext, CSFX_Sound_Itf* in_pSoundFile) = 0;


   //! Clear the list of delayed sound...
   virtual void ClearDeleayedSound(void) = 0;

};

#endif //#ifndef _GOLEM_SFX_PLAYER_ITF_
