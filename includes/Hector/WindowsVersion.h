
#pragma once

//********************************
//  WindowsVersion.h - header file
//  Singleton class to retreive info about current windows version
// 
//  Copyright © 2002 Niversoft (Nicolas Hatier)
//  This software is provided 'as-is', without any express or implied warranty. 
//  In no event will the authors be held liable for any damages arising from the
//  use of this software.
//
//  Permission is granted to anyone to use this software for any purpose, including 
//  commercial applications, and to alter it and redistribute it freely, subject to 
//  the following restrictions:
//       1. The origin of this software must not be misrepresented; you must not claim 
//          that you wrote the original software. If you use this software in a product,
//          an acknowledgment in the product documentation would be appreciated but is 
//          not required.
//
//       2. Altered source versions must be plainly marked as such, and must not be
//          misrepresented as being the original software.
//
//       3. This notice may not be removed or altered from any source distribution.
//
//********************************

#ifndef WindowsVersion_h_included
#define WindowsVersion_h_included

#include "shlwapi.h"

typedef HRESULT (CALLBACK* DLLGETVERSIONPROC)(DLLVERSIONINFO *);

class WindowsVersion
{
public:
    enum ShellLevels
    {
        SLBasic         = 0,
        SLActiveDesktop = 1,
        SLME2000        = 2,
        SLXP            = 3,
    };
    enum OsVerLevels
    {
        OVL_UNKNOWN,
        OVL_NT351,
        OVL_NT4,
        OVL_95,
        OVL_98,
        OVL_ME,
        OVL_2000,
        OVL_XP
    };

   static bool is95();
   static bool is98();
   static bool isME();
   static bool isXP();
   static bool is2000();
   static bool isNT4();
   static bool isNT351();
   
   static bool isNTPlatform();
      
   static float ShellVersion();
   static const ShellLevels ShellLevel();
   static const OsVerLevels OsLevel();

private:
   WindowsVersion();
   ~WindowsVersion();

   static void Init();
   static bool init;
   static float m_ShellVersion;
   static DLLVERSIONINFO DllVersionInfo;
   static OSVERSIONINFO OsVersionInfo;
};

#endif