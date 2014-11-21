/**************************************************************
*
* golem_movie_controller.h
*
* Description
* ===========
*  Describes GMovieController interface which allows playback 
*  control of a movie.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_MOVIE_CONTROLLER_H_
#define _GOLEM_MOVIE_CONTROLLER_H_

enum EMovieStatus
{
   MOVSTAT_STOPED = 0,
   MOVSTAT_PLAYING,
   MOVSTAT_PAUSED,
};

/*!
* Describe and control a playing movie
**/ 
class GMovieController
{
protected:
   virtual ~GMovieController() {;}

public:
   virtual bool Play() = 0;
   virtual bool Stop() = 0;
   virtual bool Pause() = 0;

   virtual EMovieStatus Status() = 0;

   virtual bool Loop(bool in_bEnableLoop) = 0;
   virtual bool IsLooping() = 0;

   virtual REAL32 Length() = 0;
   virtual REAL32 Position() = 0;
   virtual bool Seek(REAL32 in_fSeekPosition) = 0;
};

#endif // _GOLEM_MOVIE_CONTROLLER_H_
