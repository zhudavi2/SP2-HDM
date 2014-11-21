/**************************************************************
*
* golem_movie_controller_fs.h
*
* Description
* ===========
*  Implementation of a movie controller that spawns a 
*  fullscreen window and is stopped by pressing 'ESC' or 
*  'ENTER' or 'SPACE'.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_MOVIE_CONTROLLER_FS_H_
#define _GOLEM_MOVIE_CONTROLLER_FS_H_

#include <dshow.h>

namespace SDK
{

   class GMovieControllerFS : public GMovieController
   {
      friend class GJoshua;

   public:
      GMovieControllerFS(GFile& in_MovieFile);
      ~GMovieControllerFS();

      bool Play();
      bool Stop();
      bool Pause();

      EMovieStatus Status();

      bool Loop(bool in_bEnableLoop);
      bool IsLooping();

      REAL32 Length();
      REAL32 Position();
      bool Seek(REAL32 in_fSeekPosition);

   private:
      static DWORD WINAPI MovieLoopFunc(void* in_pArg);
      static LONG WINAPI WindowProc( HWND hWnd, UINT message,
         WPARAM wParam, LPARAM lParam );

      IGraphBuilder*    m_pGB;
      IMediaControl*    m_pMC;
      IVideoWindow*     m_pVW;
      IMediaEvent*      m_pME;

      HWND              m_hWnd;
      HANDLE            m_hThreadHandle;

      EMovieStatus      m_Status;

      bool              m_bAllOk;
      bool              m_bClose;
      bool              m_bThreadActive;
   };
}

#endif // #ifndef _GOLEM_MOVIE_CONTROLLER_FS_H_
