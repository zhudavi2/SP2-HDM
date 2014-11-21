/**************************************************************
*
* sp2_client_dll_export.h
*
* Description
* ===========
*	Describes exported DLL functions of client.
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

#ifndef _SP2_SERVER_DLL_EXPORT_H_
#define _SP2_SERVER_DLL_EXPORT_H_

#ifdef SERVER_EXPORTS
#define SP2_SERVER_API extern "C" __declspec(dllexport)
#else
#define SP2_SERVER_API extern "C" __declspec(dllimport)
#endif

SP2_SERVER_API bool GetInterfaceVersion( GGolemVersion& in_Version );
SP2_SERVER_API const char* GetInterfaceName();
SP2_SERVER_API const char* GetInterfaceDescription();
SP2_SERVER_API SP2::GServer* Get_Server();

#endif
