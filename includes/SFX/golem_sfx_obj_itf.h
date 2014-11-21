/*****************************************

golem_sfx_obj_itf.h

Owner: Frederic	O'Reilly

Description:
	Description	of base	sfx	objects	interface

history:
	- March	27 2003
		Creation of	the	interface

******************************************/

#ifndef	_GOLEM_SFX_OBJ_ITF_
#define	_GOLEM_SFX_OBJ_ITF_

#include "../../includes/hector/hector.h"

// basic type for sfx obj
#define	k_SFX_Type_Unkown			0
#define	k_SFX_Type_Context			100
#define	k_SFX_Type_Sound			1000

// Macro for safe release of SFX_Obj ( and DirectX by the way )
#define	SAFE_RELEASE(p)	{ if(p)	{(p)->Release(); (p) = NULL; } }

/*
Base handle for exported sound objects

Type defines kind of object
ID is unique for each object instance
*/
class CSFX_Obj_Handle
{
public:	
	UINT32	m_Type;
	UINT32	m_ID;

	CSFX_Obj_Handle()
	{
		m_Type = 0;
		m_ID = 0;
	}
	CSFX_Obj_Handle( UINT32	pType, UINT32 pID )
	{
		m_Type = pType;
		m_ID = pID;
	}
	void Set( UINT32 pType,	UINT32 pID )
	{
		m_Type = pType;
		m_ID = pID;
	}
};

/*
Base interface of exported sound objects
Behaves like a smart pointer
*/
class CSFX_Obj_Itf
{
protected:
	CSFX_Obj_Handle	m_Handle;
	UINT32			m_RefCount;	 
public:
	UINT32	Type()
	{
		return m_Handle.m_Type;
	}

	UINT32	Id()
	{
		return m_Handle.m_ID;
	}

	const CSFX_Obj_Handle	Handle()
	{
		return m_Handle;
	}

	UINT32	RefCount()
	{
		return m_RefCount;
	}

	void	Inc_Ref()
	{
		m_RefCount++;
	}

	virtual	bool	Release() =	0;
};

/*
Objet containing information for initializing sound player
*/
class CSFX_Player_Init_Mode
{
public:
	UINT32		m_DeviceIdx;
	UINT32		m_NbChannels;
	UINT32		m_SamplingFreq;
	UINT32		m_BitRate;

	CSFX_Player_Init_Mode()
	{
		m_DeviceIdx		= 0;
		m_NbChannels	= 2;
		m_SamplingFreq	= 22050;
		m_BitRate		= 16;
	}
};

/*
class used by CSFX_Type_ID_Container
contains the reference to an instance of an exported sfx object
*/
class CSFX_Type_ID_Node
{
public:
	CSFX_Obj_Handle m_ObjectHandle;
	CSFX_Obj_Itf*   m_ObjectPtr;

	CSFX_Type_ID_Node()
	{
		m_ObjectHandle.Set( 0, 0 );
		m_ObjectPtr = NULL;
	}

	CSFX_Type_ID_Node(const CSFX_Obj_Handle& in_Handle, CSFX_Obj_Itf* in_ObjPtr )
	{
		m_ObjectHandle = in_Handle;
		m_ObjectPtr = in_ObjPtr;
	}
};

/*
class used by CSFX_Obj_TypeID_Manager
contains CSFX_Type_ID_Node objects linked to a specific type
*/
class CSFX_Type_ID_Container
{
public:
	UINT32 m_New_ID;
	map<UINT32,CSFX_Type_ID_Node> m_MapID_SFX;

	CSFX_Type_ID_Container()
	{
		m_New_ID = 1;
	}
};

/*
Allows managing of exported sfx objects
*/
class CSFX_Obj_TypeID_Manager
{
	map<UINT32, CSFX_Type_ID_Container> m_MapTypeObject;
public:
	CSFX_Obj_TypeID_Manager();
	~CSFX_Obj_TypeID_Manager();

	bool            Insert( CSFX_Obj_Itf *in_Obj );
	bool            Remove( CSFX_Obj_Itf *in_Obj );
	CSFX_Obj_Itf*   SFX_Obj( CSFX_Obj_Handle in_Handle);
	bool            Obj_Exist( CSFX_Obj_Handle in_pHandle);
	UINT32          New_ID( UINT32 in_Type );

	bool            ClearAll();
	bool            DumpList();
};

/*
Returns the description (name) of an exported object type
*/
bool Get_SFX_Type_Name(	GString& in_Str, UINT32 in_SFX_Type );

#endif //#ifndef _GOLEM_SFX_OBJ_ITF_

