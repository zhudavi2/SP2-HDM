/********************************************************

GingerSDX_Info.h

Owner : Frederic O'Reilly

Description:
	Contains an information class used to contain global data
	for the sound fx player

********************************************************/
#ifndef _GINGERSDX_INFO_
#define _GINGERSDX_INFO_

#include <dsound.h>

#include "../golem_sfx_obj_itf.h"

/*---------------------------------
class CSDX_Info
global object for sound fx player
----------------------------------*/
class CSDX_Info
{
	REAL32				m_TimerResolution;
	INT64				m_TimerStart;
	bool				m_Init;

public:
	IDirectSound8		*m_DXSound;

	DSCAPS				m_DXSoundCaps;

	GEngineInfo*    g_JoshuaInfo;

	CSFX_Obj_TypeID_Manager	m_TypeIDMan;

	CSFX_Player_Init_Mode	m_CurrentInitMode;

	CSDX_Info();
	~CSDX_Info();

	bool Is_Init();
	bool Init( GEngineInfo* in_EngineInfo );

	bool ResetCapsFromDevice();

	bool Shutdown();

	void Debug_Log_Engine( GString in_Str );
	void Debug_Log_Win32LastError( GString in_Str );
	void Debug_Log_DSError( GString in_Str, HRESULT in_Error );

	bool GetWin32LastError( GString& in_StrError );
	bool GetDSResultErrorString( HRESULT in_Error, GString& in_StrError );

	REAL32 GetTime();
};

extern CSDX_Info g_SDX_Info;

#endif //#ifndef _GINGERSDX_INFO_