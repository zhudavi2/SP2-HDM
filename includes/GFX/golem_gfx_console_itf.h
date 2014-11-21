/**************************************************************
*
* golem_gfx_console_itf.h
*
* Description
* ===========
*	Class CGFX_Console_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	July 16 2003 (foreilly):
*		- Creation
*
**************************************************************/
#ifndef	_GOLEM_GFX_CONSOLE_ITF_
#define	_GOLEM_GFX_CONSOLE_ITF_

#include "golem_gfx_primitive_itf.h"

//! Graphical console class
class CGFX_Console_Itf
   : public CGFX_Primitive_Itf
   , public GConsole
{
protected:
	CGFX_Console_Itf( UINT32 in_Type )
		: CGFX_Primitive_Itf( in_Type )
	{}

public:
	//! Text color parameter
	virtual CGFX_Color_Parameter& Text_Color() = 0;

   //! Carret flashing speed parameter
   virtual CGFX_Parameter& Carret_Visibility() = 0;

   //! Key down event feeder
   virtual bool SendKeyDown(INT32 in_VirtualKey) = 0;

   //! Char event feeder
   virtual bool SendChar(INT32 in_VirtualKey) = 0;

};

#endif //#ifndef _GOLEM_GFX_CONSOLE_ITF_
