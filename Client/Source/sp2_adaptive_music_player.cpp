/**************************************************************
*
* sp2_adaptive_music_player.cpp
*
* Description
* ===========
*  See corresponding .h file
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#include "golem_pch.hpp"

// Includes
#include "../include/sp2_adaptive_music_player.h"
#include "../../includes/sfx/GingerSoundDX/SDX_WaveFile.h"

// Use Namespace
using namespace SP2;
using namespace AdaptiveMusic;

// Constant
GString  g_cThemeName[ESP2MusicalTheme::TotalTheme] = {"Sneaky","Nuclear","Loosing","Winning","Main"};

// GMusic
GMusic::GMusic() :   m_pCurrentVariationFile(NULL),
                     m_iCurrentVaration(0),
                     m_iTotalVariationPlayedSoFar(0),
                     m_iLastVariationPlayed(0), 
                     m_bOutSegment(false),
                     m_pStartCommand(NULL)
{

}

GMusic::~GMusic()
{
   if(m_pCurrentVariationFile)
   {
      m_pCurrentVariationFile->Close();
      SAFE_RELEASE(m_pCurrentVariationFile);
   }

   m_iVariationFileName.clear();
}


void GMusic::OutSegment(bool in_bOutSegment)
{
   m_bOutSegment = in_bOutSegment;
}


void GMusic::Reset(void)
{
   if(m_pCurrentVariationFile)
      m_pCurrentVariationFile->Close();

   m_iCurrentVaration            = 0 ;
   m_iTotalVariationPlayedSoFar  = 0;
   m_iLastVariationPlayed        = 0;
}

bool GMusic::Expired(void)
{
   srand( (unsigned)time( NULL ) );
   return (m_iTotalVariationPlayedSoFar > m_iVariationFileName.size() || m_iTotalVariationPlayedSoFar > (UINT32)(rand() %2) +1) ;
}
 
void GMusic::AddVariation(GString in_VariationFileName) 
{
   m_iVariationFileName.push_back(in_VariationFileName);
}


CSDX_WaveFile*    GMusic::GetNextVariation(ESP2MusicalIntensity::Enum in_eIntensityLevel)
{

   // If there is no variation in this music, just don't play anything..
   if(m_iVariationFileName.size() == 0)
   {
      // But count variation
      m_iTotalVariationPlayedSoFar++;
      return NULL;
   }

   // If we have a current variation file, we will probably change it
   if(m_pCurrentVariationFile)
   {
      m_pCurrentVariationFile->Close();
   }
   else
   {  

      m_pCurrentVariationFile = (CSDX_WaveFile*)g_Joshua.SoundPlayer()->Get_New_WaveFile();
   }

   srand( (unsigned)time( NULL ) );

   // Choose a variation from the intensity level
   INT32 l_iNewlyChoosenVariation = 0;
   switch(in_eIntensityLevel)
   {
      case ESP2MusicalIntensity::Low :
         {
            // Are we starting this music ?
            if(m_iTotalVariationPlayedSoFar == 0)
            {  
               // If so, choose between higer variation
               l_iNewlyChoosenVariation = 0;
            }
            else
            {
               // Otherwise go down, unless its at the bottom and its been played twice.
               if(m_iLastVariationPlayed == 0 && m_iCurrentVaration == 0)
               {
                  l_iNewlyChoosenVariation = m_iCurrentVaration + 1;

               }
               else
               {
                  l_iNewlyChoosenVariation = m_iCurrentVaration - 1;
               }

            }
         }
         break;
      case ESP2MusicalIntensity::Normal :
         {
            // Are we starting this music ?
            if(m_iTotalVariationPlayedSoFar == 0)
            {  
               // If so, choose between two low variation
               l_iNewlyChoosenVariation = rand() %2 + 2;
            }
            else
            {
               // Go Up or down, with a huge preference with up...(To introduce changes in the music)
               if(rand()%6)
                  l_iNewlyChoosenVariation = m_iCurrentVaration+1;
               else
                  l_iNewlyChoosenVariation = m_iCurrentVaration-1;
            }
         }
         break;
      case ESP2MusicalIntensity::High :
         {
            // Are we starting this music ?
            if(m_iTotalVariationPlayedSoFar == 0)
            {  
               // If so, choose between higer variation
               l_iNewlyChoosenVariation = m_iVariationFileName.size() - ((rand()%2) + 1);
            }
            else
            {
               // Otherwise go up, unless its at the top and its been played twice.
               if(m_iLastVariationPlayed == m_iVariationFileName.size()-1 && m_iCurrentVaration == m_iVariationFileName.size()-1)
               {
                  l_iNewlyChoosenVariation = m_iVariationFileName.size()-2;

               }
               else
               {
                  l_iNewlyChoosenVariation = m_iCurrentVaration+1;
               }
            }

         }
         break;
      default:
         gassert(true,"What the hell is this in_eIntensityLevel"); 

   };

   // Make a last check to make sure we didn't choose a variation too high
   if((UINT32)l_iNewlyChoosenVariation >= m_iVariationFileName.size()-1)
         l_iNewlyChoosenVariation = m_iVariationFileName.size()-2;
   if(l_iNewlyChoosenVariation < 0)
      l_iNewlyChoosenVariation = 0;


   // Open the new variation
   m_pCurrentVariationFile->Open(m_iVariationFileName[l_iNewlyChoosenVariation]);

   // Set Last variation to keep track for previous played variation
   m_iLastVariationPlayed = m_iCurrentVaration;

   // Set current variation
   m_iCurrentVaration = l_iNewlyChoosenVariation;

   // Increase total variation played
   m_iTotalVariationPlayedSoFar++;
 
   // return this new variation (and hopefully and interesting one for the player :) )
   return m_pCurrentVariationFile;
}


CSDX_WaveFile* GMusic::GetWaveFile(void)
{
   return m_pCurrentVariationFile;
}

bool GMusic::ReadyToGetOut(void)
{
   if(m_bOutSegment)
   {
      // We are ready if we are playing the out segment.
      return m_iCurrentVaration == m_iVariationFileName.size() -1;
   }

   return true;

}


CSDX_WaveFile*    GMusic::PlayOutSegment(void)
{

   INT32 l_iNewlyChoosenVariation = m_iVariationFileName.size()-1;

  // If there is no variation in this music, just don't play anything..
   if(m_iVariationFileName.size() == 0)
      return NULL;

   // If we have a current variation file, we will probably change it
   if(m_pCurrentVariationFile)
   {
      m_pCurrentVariationFile->Close();
   }
   else
   {  

      m_pCurrentVariationFile = (CSDX_WaveFile*)g_Joshua.SoundPlayer()->Get_New_WaveFile();
   }

   // Open the new variation
   m_pCurrentVariationFile->Open(m_iVariationFileName[l_iNewlyChoosenVariation]);

   // Set Last variation to keep track for previous played variation
   m_iLastVariationPlayed = m_iCurrentVaration;

   // Set current variation
   m_iCurrentVaration = l_iNewlyChoosenVariation;

   // Increase total variation played
   m_iTotalVariationPlayedSoFar++;

   return m_pCurrentVariationFile;
}

GMusicCommand*  GMusic::GetStartCommand(void)
{
   if(m_iTotalVariationPlayedSoFar == 1)
   { 
      return m_pStartCommand;
   }

   return NULL;
}

void  GMusic::SetStartCommand(GMusicCommand* in_pStartCommand)
{
   m_pStartCommand = in_pStartCommand;
}



// GTheme
GTheme::GTheme() : m_iCurrentMusicIndex(0xFFFFFFFF)
{

}

GTheme::~GTheme()
{
   for(UINT32 i=0;i<m_vMusic.size();i++)
   {
      SAFE_DELETE(m_vMusic[i]);
   }

}

void GTheme::AddMusic(GMusic *in_pMusic)
{
   m_vMusic.push_back(in_pMusic);
}

CSDX_WaveFile* GTheme::RequestNewWaveFile(ESP2MusicalIntensity::Enum in_eIntensityLevel)
{

   // Reset music if its the first time
   if(m_iCurrentMusicIndex == 0xFFFFFFFF)
   {
      Reset();
   }

   // Do we have music in this theme ?
   if(m_vMusic.size() == 0)
      return NULL;

   // We are requested a new WaveFile.
   // Lets take this opportunity to change music

   // Is the current music expired ?
   if(m_vMusic[m_iCurrentMusicIndex]->Expired())
   {
      // Were we playing the out segment ?
      if(m_vMusic[m_iCurrentMusicIndex]->ReadyToGetOut())
      {
         // Its expired and we are ready to get out, take a new music
   /*
         // Calculate how many are available.
         UINT32 l_iTotalAvailableMusic = 0;
         for(UINT i=0;i<m_vMusic.size();i++)
         {
            if(m_vMusic[i]->Expired() == false)
            {
               l_iTotalAvailableMusic++;
            }
         }
         // if not zero, choose from pool of avail music
   */
         // Reset Current Theme
         m_vMusic[m_iCurrentMusicIndex]->Reset();
         srand( (unsigned)time( NULL ) );
         if(m_vMusic.size() > 1)
            m_iCurrentMusicIndex+= (rand() % (m_vMusic.size()-1)) + 1;
         m_iCurrentMusicIndex%= m_vMusic.size();
      }
      else
      {
         return  m_vMusic[m_iCurrentMusicIndex]->PlayOutSegment();
      }

   }

   // Request a new variation from the current theme
   return m_vMusic[m_iCurrentMusicIndex]->GetNextVariation(in_eIntensityLevel);
}
        
CSDX_WaveFile* GTheme::GetWaveFile(void)
{
   if(m_iCurrentMusicIndex == 0xFFFFFFFF)
   {
      Reset();
   }
   return m_vMusic[m_iCurrentMusicIndex]->GetWaveFile();
}


GMusicCommand*  GTheme::GetStartCommand(void)
{
   if(m_iCurrentMusicIndex == 0xFFFFFFFF)
   {
      Reset();
   }
   return m_vMusic[m_iCurrentMusicIndex]->GetStartCommand();
}

void GTheme::Reset(void)
{
    if(m_iCurrentMusicIndex == 0xFFFFFFFF)
   {
      m_iCurrentMusicIndex = 0;
   }

   // For each music, reset
   for(UINT32 i=0;i<m_vMusic.size();i++)
   {
      m_vMusic[i]->Reset();
   }
 
   // choose a new one...
   srand( (unsigned)time( NULL ) );
   if(m_vMusic.size() > 1)
      m_iCurrentMusicIndex+= (rand() % (m_vMusic.size()-1)) + 1;
   m_iCurrentMusicIndex%= m_vMusic.size();
}


// GThemeManager

GThemeManager::GThemeManager() : m_eCurrentTheme(ESP2MusicalTheme::None), 
                                 m_eRequestedTheme(ESP2MusicalTheme::None), 
                                 m_eCurrentIntensity(ESP2MusicalIntensity::Normal)
{
   for(UINT32 i=0;i<ESP2MusicalTheme::TotalTheme;i++)
   {
      m_Themes[i] = NULL;
   }

}

GThemeManager::~GThemeManager()
{
   for(UINT32 i=0;i<ESP2MusicalTheme::TotalTheme;i++)
   {
      SAFE_DELETE(m_Themes[i]);
   }
}

void GThemeManager::LoadThemes(void)
{
   GFile          l_XMLFile;
   GString        l_sTempName;
   GXMLParser		l_XmlParser;
   //CTreeXMLNode*	l_pRootNode;
   GTree<GXMLNode>* l_pRootNode;
   

   //Request the file to the file manager
   if(!g_Joshua.FileManager()->File("Data\\AdaptiveMusic\\AdaptiveMusics.xml",l_XMLFile))
   {
      // No music for you!!!
      return;
   } 
   //Extract the file to a temp file on the disk so it can be parsed (xml)
   if(!l_XMLFile.Extract(l_sTempName))
   {
      gassert(NULL,"Couldn't extract xml file");
      return;
   }

   l_pRootNode = l_XmlParser.Parse(l_sTempName);
   l_XMLFile.Unextract(); //free the temp file

   if(!l_pRootNode)
   {
      g_Joshua.Log(SP2::ERROR_PARSING_XML_FILE + l_XMLFile.Name(),MSGTYPE_FATAL_ERROR);
      return;
   }

   gassert(l_pRootNode,"Config file is not valid or couldn't be read");

   for(UINT32 i=0;i<ESP2MusicalTheme::TotalTheme;i++)
   {
      // Read Themes
      // Read loosing theme
      CXML_Util   l_XMLUtil;
      GString     l_WavePath("Data\\AdaptiveMusic\\");
      CTreeXMLNode*  l_pThemeNode;
      l_pThemeNode = l_XMLUtil.Search_Node(l_pRootNode->Root(),g_cThemeName[i],GString(""),NULL,4);
      if(l_pThemeNode)
      {
         // Add theme name to the path
         l_WavePath+=g_cThemeName[i] + "\\"; 

         // Create Theme
         GTheme* l_pTheme = new GTheme;
         m_Themes[i] = l_pTheme;
        
         // Search for all musics for this theme
         CTreeXMLNode* l_pMusicNode = l_pThemeNode->FirstChild();
         while(l_pMusicNode)
         {
            GString l_MusicPath = l_WavePath + l_pMusicNode->Data().m_sName + "\\";
            GMusic*  l_pNewMusic = new GMusic();

            // Read Variation
            CTreeXMLNode* l_pVariationNode = l_pMusicNode->FirstChild();
            while(l_pVariationNode)
            {
               // Is it a command
               if(l_pVariationNode->Data().m_sName == "FadeIn")
               {
                  GMusicCommand* l_pNewCommand  = new GMusicCommand;
                  l_pNewCommand->m_eCommand     = ESP2MusicCommand::FadeIn;
                  l_pNewCommand->m_iTotalBlock  = l_pVariationNode->Data().m_value.ToINT32();
                  l_pNewMusic->SetStartCommand(l_pNewCommand);
               }
               else
               {
                  // Verify if wave file exist
                  GString l_waveFileName = l_MusicPath + l_pVariationNode->Data().m_value;

                  
                  // Find real file location
                  GFile l_MusicFile;
                  g_Joshua.FileManager()->File(l_waveFileName,l_MusicFile);

                  // Extract the file
                  GString l_FilePath;
                  l_MusicFile.Extract(l_FilePath);
                  #ifdef GOLEM_DEBUG
                  {
                     gassert(PathFileExists(l_FilePath.c_str()) == TRUE,"File referenced by AdaptiveMusic doesnt exist");
                  }
                  #endif


                  l_pNewMusic->AddVariation(l_FilePath);
                  l_pNewMusic->OutSegment(l_pVariationNode->Data().m_sName == "Out");
               }
               
               // Get next variation (or command)
               l_pVariationNode = l_pMusicNode->NextChild();
            };
            
            l_pTheme->AddMusic(l_pNewMusic); 
            l_pMusicNode = l_pThemeNode->NextChild();
         };
      }
   }
}

void GThemeManager::SetIntensity(ESP2MusicalIntensity::Enum in_eIntensity)
{
   m_eCurrentIntensity = in_eIntensity;
}

void GThemeManager::RequestTheme(ESP2MusicalTheme::Enum in_eTheme)
{
   m_eRequestedTheme = in_eTheme;
}

CSDX_WaveFile* GThemeManager::GetWaveFile(void)
{
   if(m_eCurrentTheme == ESP2MusicalTheme::None || m_Themes[m_eCurrentTheme] == NULL)
   {
      return NULL;
   }
   
   return m_Themes[m_eCurrentTheme]->GetWaveFile();
}

//! Return next wave file and move on into another segment
CSDX_WaveFile* GThemeManager::GetNextWaveFile(void)
{
   // If its not the same, reset old theme
   if(m_eCurrentTheme != m_eRequestedTheme && m_eCurrentTheme!= ESP2MusicalTheme::None)
   {
      if(m_Themes[m_eCurrentTheme])
         m_Themes[m_eCurrentTheme]->Reset(); 
   }

   // Change Theme
   m_eCurrentTheme = m_eRequestedTheme;

   if(m_eCurrentTheme == ESP2MusicalTheme::None || m_Themes[m_eCurrentTheme] == NULL)
   {
      return NULL;
   }

   return m_Themes[m_eCurrentTheme]->RequestNewWaveFile(m_eCurrentIntensity);
}

GMusicCommand*  GThemeManager::GetStartCommand(void)
{
   if(m_eCurrentTheme == ESP2MusicalTheme::None || m_Themes[m_eCurrentTheme] == NULL)
   {
      return NULL;
   }

   return m_Themes[m_eCurrentTheme]->GetStartCommand();
}
ESP2MusicalTheme::Enum GThemeManager::NowPlayingAtASpeakerCloseToYou(void)
{
   return m_eCurrentTheme;
}



// GThemePlayer

GThemePlayer::GThemePlayer(IDirectSound8* in_pDirectSoundInterface, GThemeManager *in_pThemeManager) :   m_pDSBuffer(NULL), 
                                                                                                         m_pDS(NULL),
                                                                                                         m_pThemeManager(in_pThemeManager),
                                                                                                         m_iNotifyThreadHandle(0),
                                                                                                         m_pActiveCommand(NULL),
                                                                                                         m_bIterateThreadIsDead(true),
                                                                                                         m_bKeepIterateThreadActive(true)
{
   // Validate input
   gassert(in_pThemeManager," Please give me theme manager!!!");
   if(in_pDirectSoundInterface)
   {
      // Get a pointer to important stuff.
      m_pDS             = in_pDirectSoundInterface;
      m_pThemeManager   = in_pThemeManager;

	   // Create thraed to pool streaming position
	   m_iNotifyThreadHandle = CreateThread( NULL, 0, Iterate,  this, 0, &m_iNotifyThreadID );

      // Calculate player buffer and block size for each buffer update
      UINT32 l_iBlockAlign = 4;
      UINT32 l_iSamplesPerSec = 22050;
      UINT32 l_iBufferLength  = 2000;
      m_iTotalBlock = 4;
      m_iBlockSize = l_iSamplesPerSec * (l_iBufferLength / 1000) * l_iBlockAlign / m_iTotalBlock;
      m_iBlockSize -= m_iBlockSize % l_iBlockAlign;  

      WAVEFORMATEX l_bufferFormat;
      l_bufferFormat.nChannels         = 2;
      l_bufferFormat.cbSize            = 0 ;
      l_bufferFormat.nBlockAlign       = (WORD)l_iBlockAlign;
      l_bufferFormat.nSamplesPerSec    = l_iSamplesPerSec;
      l_bufferFormat.nAvgBytesPerSec   = l_iSamplesPerSec * l_iBlockAlign;
      l_bufferFormat.wBitsPerSample    = (WORD)(l_iBlockAlign * 8 / l_bufferFormat.nChannels);
      l_bufferFormat.wFormatTag        = WAVE_FORMAT_PCM;

      // Set up the direct sound buffer.  
      DSBUFFERDESC dsbd;
      ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
      dsbd.dwSize          = sizeof(DSBUFFERDESC);
      dsbd.dwFlags         = DSBCAPS_CTRLVOLUME;
      dsbd.dwBufferBytes   = m_iBlockSize * m_iTotalBlock;
      dsbd.guid3DAlgorithm = GUID_NULL;
      dsbd.lpwfxFormat     = &l_bufferFormat;

      // Create Stream buffer
      HRESULT hr = m_pDS->CreateSoundBuffer( &dsbd, &m_pDSBuffer, NULL );
      gassert(hr == S_OK,"Can't create sound buffer");



      // Set volume to on
      SetVolume(1);
   }
}

GThemePlayer::~GThemePlayer()
{
	// Close Thread
   m_bKeepIterateThreadActive = false;
   while(!m_bIterateThreadIsDead)
   {
      Sleep(10);
   };

   // Close thread handle
   CloseHandle( m_iNotifyThreadHandle );

   // Delete Direct sound buffer
   SAFE_RELEASE(m_pDSBuffer);

   // Safe delete theme manager
   SAFE_DELETE(m_pThemeManager);

   // Assign null pointer (JM NOTE : Mem leak ptr counter ?!?)
   m_pDS = NULL;
}

void GThemePlayer::SetVolume(float in_fMusicVolume)
{
   // assert to be between 1 and 0;
   m_fMusicVolume = in_fMusicVolume;
   SetBufferVolume(in_fMusicVolume);
}
void GThemePlayer::SetBufferVolume(float in_fMusicVolume)
{
   // Set volume between -10000 and 0, non-linear
   long l_iVolume =(long)( - pow(10.0f,(1.0f-in_fMusicVolume) * 4.0f));

   // Set volume to default volume
   if(m_pDSBuffer)
      m_pDSBuffer->SetVolume(l_iVolume);
}


void GThemePlayer::Stop(void)
{
   // Stop buffer
   if(m_pDSBuffer)
      m_pDSBuffer->Stop();

}

void GThemePlayer::Reset(void)
{
   if(m_pDSBuffer)
   {
      // Reset playing buffer
      m_iNextBlockToUpdate = 0;
      m_iNextWriteOffset    = 0;
      m_pDSBuffer->SetCurrentPosition( 0 );  

      // Update Buffer
      UpdateBlock();

      // Update Buffer a second time, to make sure that we have at least one buffer difference.
      UpdateBlock();
   }
}

void GThemePlayer::Play(void)
{
   // Play
   if(m_pDSBuffer)
      m_pDSBuffer->Play(0,0,DSBPLAY_LOOPING);
}

void GThemePlayer::Start(void)
{
   if(m_pDSBuffer)
   {
      Stop();
      Reset();
      Play();
   }
}

DWORD WINAPI GThemePlayer::Iterate( LPVOID lpParameter )
{
   GThemePlayer *l_this = (GThemePlayer *)lpParameter;
   l_this->m_bIterateThreadIsDead = false;

   while(l_this->m_bKeepIterateThreadActive)
   {
      l_this->OnIterate();
      // Give back some control
      Sleep(10);
   };

   l_this->m_bIterateThreadIsDead  = true;

	return 0;
}

bool GThemePlayer::OnIterate(void)
{
   // Get current position
   if(m_pDSBuffer)
   {
      DWORD dwStatus = 0;
      m_pDSBuffer->GetStatus( &dwStatus );
      if(dwStatus & DSBSTATUS_PLAYING)
      {

         // Check for buffer lost
         RestoreBuffer();

         // Get Position
         DWORD dwPlayPos;
         m_pDSBuffer->GetCurrentPosition(&dwPlayPos,NULL);

         // NOTE : should be a while ?
         unsigned int l_iCurrentBlock = dwPlayPos / m_iBlockSize;
         // Update next block 
         l_iCurrentBlock+=2;
         l_iCurrentBlock%=m_iTotalBlock;
         if(l_iCurrentBlock == m_iNextBlockToUpdate)
         {
            // Read one more chunk 
            UpdateBlock();
         }
      }

   }

   return true;
}

void GThemePlayer::RestoreBuffer(void)
{
    HRESULT hr;

    DWORD dwStatus;
    hr = m_pDSBuffer->GetStatus( &dwStatus );
    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        // Since the app could have just been activated, then
        // DirectSound may not be giving us control yet, so 
        // the restoring the buffer may fail.  
        // If it does, sleep until DirectSound gives us control.
        do 
        {
            hr = m_pDSBuffer->Restore();
            if( hr == DSERR_BUFFERLOST )
                Sleep( 10 );
        }
        while( ( hr = m_pDSBuffer->Restore() ) == DSERR_BUFFERLOST );
    }
}

void GThemePlayer::SetCommand(GMusicCommand* in_pCommand)
{
   // If we don't have any queue command... just don't do anything...we surrely don't want to start something and not finish it... hell no!!!
   if(m_pActiveCommand == NULL)
   {
      m_pActiveCommand  = in_pCommand ;
   }
}

void GThemePlayer::UpdateBlock(void)
{
   HRESULT hr;
   VOID*   pDSLockedBuffer = NULL;
   VOID*   pDSLockedBuffer2 = NULL;
   DWORD   dwDSLockedBufferSize;
   DWORD   dwDSLockedBufferSize2;
   UINT32   l_iBytesWrittenToBuffer;

   static unsigned char l_StringOrder[5][32] = {"Data/AdaptiveMusic/A.wav","Data/AdaptiveMusic/B.wav","Data/AdaptiveMusic/C.Wav","Data/AdaptiveMusic/D.Wav","Data/AdaptiveMusic/E.Wav"};
   static INT32 l_iCurrentTheme = 0;

   // Check for buffer lost and restore it if needed
   RestoreBuffer();

  // Lock the DirectSound buffer
  hr = m_pDSBuffer->Lock( m_iNextWriteOffset, m_iBlockSize, &pDSLockedBuffer, &dwDSLockedBufferSize, &pDSLockedBuffer2, &dwDSLockedBufferSize2, 0L );
  gassert(hr == S_OK, "Could't look the buffer to write to");
  // it should be the second buffer, so it should never be valid
  gassert(pDSLockedBuffer2 == NULL,"Apparently it should be null (From DX examle)" );

  // Get the current wave file we want to work on.
  CSDX_WaveFile *l_pCurrentWaveFile = NULL;
  l_pCurrentWaveFile = m_pThemeManager->GetWaveFile();


  if(l_pCurrentWaveFile)
  {
      
      // Fill the DirectSound buffer with wav data
      l_pCurrentWaveFile->Read( (INT8*) pDSLockedBuffer, dwDSLockedBufferSize,  l_iBytesWrittenToBuffer );

      // If the number of bytes written is less than the 
      // amount we requested, we have a short file.
      if( l_iBytesWrittenToBuffer < dwDSLockedBufferSize )
      {
         // We are changing files, so reset the file and fill the buffer with wav data
         DWORD dwReadSoFar = l_iBytesWrittenToBuffer;    // From previous call above.

         while( dwReadSoFar < dwDSLockedBufferSize )
         {  
            
          
            l_pCurrentWaveFile = m_pThemeManager->GetNextWaveFile();

            m_pActiveCommand = m_pThemeManager->GetStartCommand();
            if(m_pActiveCommand)
               m_pActiveCommand->m_iTotalBlockSoFar = 0;


            if(l_pCurrentWaveFile)
            {
               l_pCurrentWaveFile->Read( (INT8*)pDSLockedBuffer + dwReadSoFar, dwDSLockedBufferSize - dwReadSoFar, l_iBytesWrittenToBuffer );
               dwReadSoFar += l_iBytesWrittenToBuffer;
            }
            else
            {
               FillMemory( (INT8*)pDSLockedBuffer + dwReadSoFar, dwDSLockedBufferSize - dwReadSoFar, 0 );
               dwReadSoFar = dwDSLockedBufferSize;
            }
         } 
      } 
   }
   else
   {
      // Fill the DirectSound buffer with silence
      FillMemory( pDSLockedBuffer, dwDSLockedBufferSize, 0 );
      l_iBytesWrittenToBuffer = dwDSLockedBufferSize;

      // ReGquest another wave file...
      m_pThemeManager->GetNextWaveFile();
   }

   // Do we have to command some byte !!
   if(m_pActiveCommand)
   {
      // THIS FUNCTION ASSUMENT 2 CHANNEL 16 BITS!!!
      REAL32 l_fStartRatio;
      REAL32 l_fEndRatio;

      l_fStartRatio = m_pActiveCommand->m_iTotalBlockSoFar / (REAL32)m_pActiveCommand->m_iTotalBlock;
      l_fEndRatio =  (m_pActiveCommand->m_iTotalBlockSoFar + 1)/ (REAL32)m_pActiveCommand->m_iTotalBlock ;

      if(m_pActiveCommand->m_eCommand == ESP2MusicCommand::FadeOutAndChangeTheme)
      {
         l_fEndRatio = 1 - l_fEndRatio;
         l_fStartRatio = 1 - l_fStartRatio;
      }

      REAL32 l_fIncrement = (l_fEndRatio - l_fStartRatio) / (REAL32)(dwDSLockedBufferSize >> 2);
      INT16 *l_pBuffer = (INT16*) pDSLockedBuffer;

      for(UINT32 i=0;i<dwDSLockedBufferSize >> 2;i++)
      {
         // Left Channel
         *l_pBuffer = (UINT16)(*l_pBuffer * l_fStartRatio);
         l_pBuffer++;

         // Right Channel
         *l_pBuffer = (UINT16)(*l_pBuffer * l_fStartRatio);
         l_pBuffer++;

         l_fStartRatio += l_fIncrement;
      }
      m_pActiveCommand->m_iTotalBlockSoFar++;


      // Clear command if its done
      if(m_pActiveCommand->m_iTotalBlock == m_pActiveCommand->m_iTotalBlockSoFar)
      {
         
         if(m_pActiveCommand->m_eCommand == ESP2MusicCommand::FadeOutAndChangeTheme)
         {
            // Provoke a Theme Change
            m_pThemeManager->GetNextWaveFile();

            // Since we know its been created outside...HACK! (New somewhere and delete here...!?!?)
            SAFE_DELETE(m_pActiveCommand);
            m_pActiveCommand = NULL;
         }
         else
         {
            m_pActiveCommand = NULL;
         }
      }
   }

   // Unlock the DirectSound buffer
   m_pDSBuffer->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );

   // Update where the buffer will lock (for next time)
   m_iNextWriteOffset += dwDSLockedBufferSize; 
   m_iNextWriteOffset %= (m_iBlockSize * m_iTotalBlock); // Circular buffer

   // Update Next block
   m_iNextBlockToUpdate++;
   m_iNextBlockToUpdate%= m_iTotalBlock;
}


GThemeManager*    GThemePlayer::ThemeManager(void)
{
   return m_pThemeManager;
}
