/**************************************************************
*
* sp2_client_dll_export.cpp
*
* Description
* ===========
*	Implements the entry point for the client DLL.
*	Also contains implementation of exported client DLL functions.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	May 06 2003 (foreilly):
*		- Creation
*
**************************************************************/
#include "golem_pch.hpp"

#include "../Include/sp2_client_dll_export.h"

/**************************************************************
* GLOBALS
**************************************************************/
static char g_Name[] = "SP2_Client";
static char g_Desc[] = "Superpower 2 game client";

BOOL APIENTRY DllMain(	HANDLE hModule,  DWORD  ul_reason_for_call, LPVOID lpReserved )
{
   switch (ul_reason_for_call)
   {
   case DLL_PROCESS_ATTACH:
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
   case DLL_PROCESS_DETACH:
      break;
   }
   return TRUE;
}

SP2_CLIENT_API bool GetInterfaceVersion( GGolemVersion& in_Version )
{
   // Fill version information
   in_Version.m_Major = 0;
   in_Version.m_Minor = 2;
   in_Version.m_Sub = 0;
   in_Version.m_Revision = 0;

   return true;
}

SP2_CLIENT_API const char* GetInterfaceName()
{
   return g_Name;
}

SP2_CLIENT_API const char* GetInterfaceDescription()
{
   return g_Desc;
}

SP2_CLIENT_API SP2::GClient* Get_Client()
{
   // Create and return a new client
   SP2::GClient* l_Client;

   l_Client = new SP2::GClient;
   return l_Client;
}
