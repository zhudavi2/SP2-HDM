/**************************************************************
*
* sp2_movie_player.h
*
* Description
* ===========
*  Class that enables a movie to be played full screen using
*  a single window & a texture.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_MOVIE_PLAYER_H_
#define _SP2_MOVIE_PLAYER_H_

namespace SP2
{
   class GMoviePlayer
   {
   public:
      GMoviePlayer();
      ~GMoviePlayer();

      void PlayFile(const GString& in_sFileName);

      void Stop();

      // Return true when movie is over.
      // Stop should be called immediately after
      bool Iterate();

   private:
      void*    m_pYouDontWantToKnowWhatThisIs;
   };
}

#endif // #ifndef _SP2_MOVIE_PLAYER_H_