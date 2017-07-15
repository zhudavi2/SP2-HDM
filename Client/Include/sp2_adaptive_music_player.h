/**************************************************************
*
* sp2_adaptive_music_player.h
*
* Description
* ===========
*  Description & implementation of the music_player for an adaptive score
*
* Creator
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _sp2_adaptive_music_player_h
#define _sp2_adaptive_music_player_h

// Includes
#include <Dsound.h>

// Foward Declarations
class CSDX_WaveFile;

namespace SP2
{ 

   namespace AdaptiveMusic 
   {
      namespace ESP2MusicalTheme
      {
         enum Enum
         {
            Sneaky = 0,
            Nuclear,
            Loosing,
            Winning,
            Main,
            None,
            TotalTheme = None
         };
      };

      namespace   ESP2MusicalIntensity
      {
         enum Enum 
         {
            Low = 0,
            Normal,
            High,
         };
      };

      namespace ESP2MusicCommand
      {
         enum Enum
         {
            FadeIn,
            FadeOutAndChangeTheme
         };
      }

      struct GMusicCommand
      {
         ESP2MusicCommand::Enum  m_eCommand;
         UINT32                  m_iTotalBlock;
         UINT32                  m_iTotalBlockSoFar;
      };

      class GMusic
      {
      private:
         GMusicCommand*    m_pStartCommand;
         unsigned int      m_iCurrentVaration;
         unsigned int      m_iLastVariationPlayed;
         unsigned int      m_iTotalVariationPlayedSoFar;
         vector<GString>   m_iVariationFileName;
         CSDX_WaveFile*    m_pCurrentVariationFile;
         bool              m_bOutSegment;
      public:

         // Constructor and Destructor
         GMusic();
         ~GMusic();

         //! Add a variation
         void AddVariation(GString in_VariationFileName);

         //! Get current wave file
         CSDX_WaveFile*    GetWaveFile(void);

         //! Get Next variation given a tension level
         CSDX_WaveFile*    GetNextVariation(ESP2MusicalIntensity::Enum in_eIntensity);

         //! Reset a Music so it forget what happen
         void Reset(void);

         //! Return true if the music has expired, (its been playing for too long)
         bool Expired(void);

         void OutSegment(bool in_bOutSegment);

         // Are we ready to get out ?
         bool ReadyToGetOut(void);

         //! Play the last segment, if there is one.
         CSDX_WaveFile*    PlayOutSegment(void);

         //! Return the start command of the current music
         GMusicCommand*    GetStartCommand(void);
         void              SetStartCommand(GMusicCommand* in_pStartCommand);

      };

      class GTheme
      {
      private:
         vector<GMusic*>         m_vMusic;
         UINT32                  m_iCurrentMusicIndex;

         // Add Music
      public:
         GTheme();
         ~GTheme();

         void AddMusic(GMusic* in_pMusic);

         CSDX_WaveFile* RequestNewWaveFile(ESP2MusicalIntensity::Enum in_eIntensityLevel);

         // Return the start command of the current music
         GMusicCommand*  GetStartCommand(void);

         CSDX_WaveFile* GetWaveFile(void);

         void Reset(void);

      };

      class GThemeManager
      {
         GTheme*                    m_Themes[ESP2MusicalTheme::TotalTheme];
         ESP2MusicalIntensity::Enum m_eCurrentIntensity;
         ESP2MusicalTheme::Enum     m_eCurrentTheme;
         ESP2MusicalTheme::Enum     m_eRequestedTheme;
         
      public :
         GThemeManager();
         ~GThemeManager();

         //! Prepare themes (for SP2 Adaptive Music)
         void LoadThemes(void);

         //! Set the Theme that will next be playing
         void RequestTheme(ESP2MusicalTheme::Enum in_eTheme);

         //! Return the theme currently playing
         ESP2MusicalTheme::Enum NowPlayingAtASpeakerCloseToYou(void);

         // Set the current Intensity level
         void SetIntensity(ESP2MusicalIntensity::Enum in_eIntensity);

         //! Return current wave file to play
         CSDX_WaveFile* GetWaveFile(void);

         //! Return next wave file and move on into another segment
         CSDX_WaveFile* GetNextWaveFile(void);

         // Return the start command of the current music
         GMusicCommand*  GetStartCommand(void);

      };

     
      class GThemePlayer
      {
      private:

         unsigned int         m_currentThemeIndex;
         GMusicCommand*       m_pActiveCommand;
         REAL32               m_fMusicVolume;
         
         
         LPDIRECTSOUNDBUFFER  m_pDSBuffer;
	      DWORD                m_iNotifyThreadID;
	      HANDLE               m_iNotifyThreadHandle;

         unsigned int         m_iNextBlockToUpdate;
         unsigned int         m_iBlockSize;
         unsigned int         m_iTotalBlock;
         DWORD                m_iNextWriteOffset;

         IDirectSound8*       m_pDS;   
         GThemeManager*       m_pThemeManager;

         bool                 m_bKeepIterateThreadActive;
         bool                 m_bIterateThreadIsDead;
         

         void UpdateBlock(void);

         // void Restore the buffer if lost
         void RestoreBuffer(void);

         static DWORD WINAPI Iterate( LPVOID lpParameter );

      public:
         GThemePlayer(IDirectSound8* in_pDirectSoundInterface,GThemeManager *in_pThemeManager );
	      ~GThemePlayer();

         //Play a Specific theme
         void Play(void);
         void Stop(void);
         void Reset(void);

         GThemeManager*    ThemeManager(void);

         // Start the player
         void Start(void);

         void SetVolume(float in_fMusicVolume);
         void SetBufferVolume(float in_fMusicVolume);

         void SetCommand(GMusicCommand* in_pCommand);
         
         // Iterate Procedure;
         bool OnIterate(void);

      };
   } // end of namespace AdaptiveMusic
}  // end of namespace SP2
#endif //_sp2_adaptive_music_player_h