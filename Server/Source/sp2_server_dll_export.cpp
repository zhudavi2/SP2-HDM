/**************************************************************
*
* sp2_server_dll_export.cpp
*
* Description
* ===========
*	Implements the entry point for the server DLL.
*	Also contains implementation of exported server DLL functions.
*
* Owner
* =====
*	Mathieu Tremblay
*
* History
* =======
*	May 28 2003 (foreilly):
*		- Creation
*
**************************************************************/
#include "golem_pch.hpp"

#include "../Include/sp2_server_dll_export.h"

/**************************************************************
* GLOBALS
**************************************************************/
static char g_Name[] = "SP2_Server";
static char g_Desc[] = "Superpower 2 game server";

BOOL APIENTRY DllMain(	HANDLE hModule, 
						DWORD  ul_reason_for_call, 
						LPVOID lpReserved )
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

SP2_SERVER_API bool GetInterfaceVersion( GGolemVersion& in_Version )
{
	// Fill version information
	in_Version.m_Major = 0;
	in_Version.m_Minor = 2;
	in_Version.m_Sub = 0;
	in_Version.m_Revision = 0;

	return true;
}

SP2_SERVER_API const char* GetInterfaceName()
{
	return g_Name;
}

SP2_SERVER_API const char* GetInterfaceDescription()
{
	return g_Desc;
}

SP2_SERVER_API SP2::GServer* Get_Server()
{
	// Create and return a new server
   SP2::GServer* l_server;

	l_server = new SP2::GServer();
	return l_server;
}
