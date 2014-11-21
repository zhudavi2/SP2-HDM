/**************************************************************
*
* golem_gfx_2d_multiple_line_list_itf.h
*
* Description
* ===========
*	Class CGFX_2D_Multiple_Line_List_Itf definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	April 15 2003 (foreilly):
*		- Creation
*
**************************************************************/
#ifndef _GOLEM_GFX_2D_MULTIPLE_LINE_LIST_ITF_
#define _GOLEM_GFX_2D_MULTIPLE_LINE_LIST_ITF_

#include "golem_gfx_primitive_itf.h"

//! Class that describes a set of multiple 2D lines
class CGFX_2D_Multiple_Line_List_Itf : public CGFX_Primitive_Itf
{
protected:
	CGFX_2D_Multiple_Line_List_Itf( UINT32 in_Type )
		: CGFX_Primitive_Itf( in_Type )
	{}

public:
	//! Remove all information
	virtual bool	ClearAll() = 0;

	//! Get the number of points
	virtual UINT32	Get_Nb_Points() = 0;

	//! Remove all points
	virtual bool	Clear_Points() = 0;

	//! Set all points to the content of the array
	virtual bool	Setup_Points( GVector2D<REAL32> *in_Points, UINT32 in_NbPoints ) = 0;

	//! Add a single point
	virtual bool	Add_Point( const GVector2D<REAL32>& in_Position ) = 0;

	//! Fill the specified array with all point data
	virtual bool	Fill_Point_Array( GVector2D<REAL32> *out_V3DArray ) = 0;

	//! Get a single point
	virtual bool	Get_Point( UINT32 in_Index, GVector2D<REAL32>& out_Point ) = 0;

	//! Set a single point of the mesh
	virtual bool	Set_Point( UINT32 in_Index, const GVector2D<REAL32>& in_Point ) = 0;


	//! Get the number of colors
	virtual UINT32	Get_Nb_Colors() = 0;

	//! Remove all colors
	virtual bool	Clear_Colors() = 0;

	//! Set all colors to the content of the float array
	virtual bool	SetupColors( GColorRGBReal *in_Colors, UINT32 in_NbColor ) = 0;

	//! Set all colors to the content of the integer array
	virtual bool	SetupColors( GColorRGBInt *in_Colors, UINT32 in_NbColor ) = 0;

	//! Add a single float color
	virtual bool	AddColor( const GColorRGBReal& in_Color ) = 0;

	//! Add a single integer color
	virtual bool	AddColor( const GColorRGBInt& in_Color ) = 0;

	//! Fill the specified array with all float color data
	virtual bool	FillColorArray( GColorRGBReal *out_ColorArray ) = 0;

	//! Fill the specified array with all integer color data
	virtual bool	FillColorArray( GColorRGBInt *out_ColorArray ) = 0;

	//! Get a single float color
	virtual bool	Get_Color( UINT32 in_Index, GColorRGBReal& out_Color ) = 0;

	//! Get a single integer color
	virtual bool	Get_Color( UINT32 in_Index, GColorRGBInt& out_Color ) = 0;

	//! Set a single float color
	virtual bool	Set_Color( UINT32 in_Index, const GColorRGBReal& in_Color ) = 0;

	//! Set a single integer color
	virtual bool	Set_Color( UINT32 in_Index, const GColorRGBInt& in_Color ) = 0;


	//! Get the number of lines
	virtual UINT32	Get_Nb_Lines() = 0;

	//! Set the number of lines
	virtual bool	Set_Nb_Lines( UINT32 in_NbLines ) = 0;

	//! Remove all lines
	virtual bool	Clear_Lines() = 0;

	//! Set all lines to the content of the array
	virtual bool	Setup_Lines( UINT32 *in_StartIndexes, UINT32 *in_NbPoints, UINT32 in_NbLines ) = 0;

	//! Add a single line
	virtual bool	Add_Line( UINT32 in_StartIndex, UINT32 in_NbPoints ) = 0;

	//! Get a single line information
	virtual bool	Get_Line( UINT32 in_Line, UINT32& out_Start, UINT32& out_NbPoints ) = 0;

	//! Set a single line information
	virtual bool	Set_Line( UINT32 in_Line, UINT32 in_Start, UINT32 in_NbPoints ) = 0;

};

#endif //#ifndef _GOLEM_GFX_2D_MULTIPLE_LINE_LIST_ITF_