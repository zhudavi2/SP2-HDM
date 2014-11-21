/**************************************************************
*
* golem_gfx_parameter_itf.h
*
* Description
* ===========
*	Class CGFX_Parameter definition.
*
* Owner
* =====
*	Frederic O'Reilly
*
* History
* =======
*	April 09 2003 (foreilly):
*		- Creation
*
**************************************************************/
#ifndef _GOLEM_GFX_PARAMETER_ITF_
#define _GOLEM_GFX_PARAMETER_ITF_

#include "../../includes/hector/hector.h"

//! Describes a single parameter
class CGFX_Parameter
{
	friend class CGFX_Animated_Obj_Itf;
	friend class CGFX_Animation_Itf;

private:
	//! Value of parameter
	REAL32 m_Value;

public:
	//! Get the value of the parameter
	REAL32 Value() { return m_Value; }

	//! Set the value of the parameter
	void Value(REAL32 in_Value) { m_Value = in_Value; }

	//! Get the value using casting operator overload
	operator REAL32 () { return m_Value; }

	//! Set the value using = operator overload
	REAL32 operator = (REAL32 in_NewValue) { return (m_Value = in_NewValue); }

	//! Copy the value of another parameter
	CGFX_Parameter& operator = (const CGFX_Parameter& in_NewParam)
	{
		m_Value = in_NewParam.m_Value;
		return *this;
	}

};

class CGFX_Vector2_Parameter
{
public:
	CGFX_Parameter		X;
	CGFX_Parameter		Y;

	//! Get the value using casting operator overload
   template <typename T>
	operator GVector2D<T> () { return GVector2D<T>( (T) X, (T) Y); }

	//! Set the value using operator overload
   template <typename T>
	GVector2D<T> operator = (const GVector2D<T>& in_NewValue)
	{
		X = (REAL32) in_NewValue.x;
      Y = (REAL32) in_NewValue.y;

		return in_NewValue;
	}

	//! Copy the value of another parameter 2d vector
	CGFX_Vector2_Parameter& operator = (const CGFX_Vector2_Parameter& in_NewParam)
	{
		X = in_NewParam.X;
		Y = in_NewParam.Y;

		return *this;
	}

};

class CGFX_Vector3_Parameter
{
public:
	CGFX_Parameter		X;
	CGFX_Parameter		Y;
	CGFX_Parameter		Z;

	//! Get the value using casting operator overload
   template <typename T>
	operator GVector3D<T> () { return GVector3D<T>( (T) X, (T) Y, (T) Z); }

	//! Set the value using operator overload
   template <typename T>
	GVector3D<T> operator = (const GVector3D<T>& in_NewValue)
	{
		X = (REAL32) in_NewValue.x;
		Y = (REAL32) in_NewValue.y;
		Z = (REAL32) in_NewValue.z;

		return in_NewValue;
	}

	//! Copy the value of another parameter 3d vector
	CGFX_Vector3_Parameter& operator = (const CGFX_Vector3_Parameter& in_NewParam)
	{
		X = in_NewParam.X;
		Y = in_NewParam.Y;
		Z = in_NewParam.Z;

		return *this;
	}

};

class CGFX_Color_Parameter
{
public:
	CGFX_Parameter		R;
	CGFX_Parameter		G;
	CGFX_Parameter		B;
	CGFX_Parameter		A;

	//! Get the value using casting operator overload
	operator GColorRGBReal () { return GColorRGBReal(R, G, B, A); }

	//! Set the value using operator overload
	GColorRGBReal operator = (const GColorRGBReal& in_NewValue)
	{
		R = in_NewValue.R;
		G = in_NewValue.G;
		B = in_NewValue.B;
		A = in_NewValue.A;

		return in_NewValue;
	}

	//! Set the value using operator overload
	GColorRGBInt operator = (const GColorRGBInt& in_NewValue)
	{
		GColorRGBReal l_Color(in_NewValue);

		R = l_Color.R;
		G = l_Color.G;
		B = l_Color.B;
		A = l_Color.A;

		return in_NewValue;
	}

	//! Copy the value of another parameter color vector
	CGFX_Color_Parameter& operator = (const CGFX_Color_Parameter& in_NewParam)
	{
		R = in_NewParam.R;
		G = in_NewParam.G;
		B = in_NewParam.B;
		A = in_NewParam.A;

		return *this;
	}

};
/*
class CGFX_Vector4_Parameter
{
public:
	CGFX_Parameter		X;
	CGFX_Parameter		Y;
	CGFX_Parameter		Z;
	CGFX_Parameter		W;

	//! Copy the value of another parameter 4d vector
	CGFX_Vector4_Parameter& operator = (CGFX_Vector4_Parameter& in_NewParam)
	{
		X = in_NewParam.X;
		Y = in_NewParam.Y;
		Z = in_NewParam.Z;
		W = in_NewParam.W;

		return *this;
	}

};
*/
#endif //#ifndef _GOLEM_GFX_PARAMETER_ITF_

