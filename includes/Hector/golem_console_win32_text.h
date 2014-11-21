/**************************************************************
*
* golem_console_win32.h
*
* Description
* ===========
*  Description & implementation of class GWin32TextConsole
*
* Owner
* =====
*  Germain Sauve
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_CONSOLE_WIN32_
#define _GOLEM_CONSOLE_WIN32_

#include "golem_console.h"

namespace Hector
{
   /*!
   * Console using a CMD style output (no input)
   **/
   class GWin32TextConsole : public GConsole
   {
      HANDLE m_Handle;

   public:
      GWin32TextConsole();
      ~GWin32TextConsole();

      bool Print( const GString& pString );
   };

}

#endif //#ifndef _GOLEM_WIN32_CONSOLE_