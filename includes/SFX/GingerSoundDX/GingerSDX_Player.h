/*************************************************************************

GingerSDX_Player.h

Owner : Frederic O'Reilly

Descrition :

	Interface CSDX_Player implementation
	Uses DirectX 9

*************************************************************************/

#include "GingerSDX_Info.h"
#include "../golem_sfx_player_itf.h"
#include "../../../includes/hector/golem_time.h"
#include <dshow.h>

// Foward declaration
class  CSFX_WaveFile_Itf;

class CSDX_Player : public CSFX_Player_Itf
{
   IGraphBuilder*       m_pGraphBuilder;
   IMediaControl*       m_pMediaControl;
   IMediaSeeking*       m_pMediaSeeking;
   IMediaEvent*         m_pMediaEvent;
   HANDLE               m_hMusicThread;
   INT32                m_iSoundVolume;
   bool                 m_bTerminateMusicThread;
   bool                 m_bMusicThreadTerminated;
   GTimeCounter         m_TimeCounter;

   static DWORD WINAPI MusicLoopFunc(void* in_pArg);

   struct DelayedSound
   {
      CSFX_Context_Itf*    m_pSoundContext;
      CSFX_Sound_Itf*      m_pSoundToPlay;
   };

   multimap<REAL32,DelayedSound> m_DelayedSound;

public:
	CSDX_Player();
	~CSDX_Player();

	// CSFX_Player_Itf methods overload
	bool	Startup( GEngineInfo *in_Engine_Info);
	bool	GetLastError( GString&	in_ErrorString );
	bool	Init( CSFX_Player_Init_Mode& in_InitMode );
	bool	Close();
	bool	Release();

   bool  PlayMusicFile(GFile& in_File);
   bool  StopMusic(void);

	CSFX_Context_Itf*   	Get_New_Context();
	CSFX_Context_Itf*   	Get_Context( UINT32 in_ID );

	CSFX_Sound_Itf*   	Get_New_Sound();
	CSFX_Sound_Itf*   	Get_Sound( UINT32 in_ID );

   CSFX_WaveFile_Itf*   Get_New_WaveFile();
   CSDX_Info&           Get_SDX_Info();

   // Iterate the sound player for delayed sound and other process
   void Iterate(void);

   // Ask the player to play a sound with after a certain delay
   void PlayDelayedSound(CSFX_Context_Itf* in_pSoundContext, CSFX_Sound_Itf* in_pSoundFile, REAL32 in_fDelayInSecond);



   // Get sound volume
   REAL32 Get_Sound_Volume( UINT32 in_iContextID );
   // Set sound volume in hundredths of a decibel (dB).
   void  Set_Sound_Volume( UINT32 in_iContextID, INT32 in_iVolume );
   // Set sound volume in percentage 
   void  Set_Sound_Volume( UINT32 in_iContextID, REAL32 in_fVolume );
   //! Return in how long a particular sound will be playing. return -1 if sounds is playing or have played or will not be playing.
   REAL32 GetTimeForDelayedSound(CSFX_Context_Itf* in_pSoundContext, CSFX_Sound_Itf* in_pSoundFile);

   // Clear delayed sound
   void ClearDeleayedSound(void);

};