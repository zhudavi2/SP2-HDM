/**************************************************************
*
* golem_engine_info.h
*
* Description
* ===========
*  Describes the content of GEngineInfo class
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_JOSHUA_INFO_
#define _GOLEM_JOSHUA_INFO_

#include "golem_event.h"
#include "golem_console.h"

namespace Hector
{

   class GFileManager;
   class GLogger;

   /*!
   * Class that contains pointers to main engine objects
   **/
   class GEngineInfo
   {
   public:
      HWND                    m_HWnd;           //!< Main window to link everything

      WNDPROC                 m_WndProc;        //!< Wndproc to use when creating window 
      GEventManager*          m_EventManager;   //!< Main event manager
      GConsole*		         m_ConsoleOut;     //!< Main console output
      GFileManager*				m_FileManager;    //!< Main file manager
      GLogger*                m_Logger;         //!< Pointer to Logger

      //! Default constructor
      GEngineInfo();

      //! Destructor
      ~GEngineInfo();
   };

}

#endif //#ifndef _GOLEM_JOSHUA_INFO_