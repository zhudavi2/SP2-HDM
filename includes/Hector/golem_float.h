/**************************************************************
*
* golem_float.h
*
* Description
* ===========
*  Utility functions that can be used to manipulate floating
*  point numbers.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_FLOAT_H_
#define _GOLEM_FLOAT_H_

#include "golem_types.h"

namespace Hector
{
   //! Fast reciprocal square root (taken from Quake 3 game code)
   /*!
    * The result is always at least 99.5% accurate, and the speed
    * about 10x faster than computing 1.f / sqrt(value).
    * @param   in_fNumber  : Value to compute
    * @return  Reciprocal square root of a number
    **/
   __inline REAL32 RSqrt( REAL32 in_fNumber )
   {
      INT32 i;
      REAL32 x2, y;
      const REAL32 threehalfs = 1.5f;
      x2 = in_fNumber * 0.5f;
      y  = in_fNumber;
      i  = * (INT32 *) &y;
      i  = 0x5f3759df - (i >> 1);
      y  = * (REAL32 *) &i;
      y  = y * (threehalfs - (x2 * y * y));
      return y;
   }

   __forceinline INT32 RoundToInt(REAL32 in_Value)
   {
      INT32 l_ReturnValue;

      _asm
      {
         fld      dword ptr [in_Value]
         fistp    dword ptr [l_ReturnValue]
      }

      return l_ReturnValue;
   }

   __forceinline INT32 RoundToInt(REAL64 in_Value)
   {
      INT32 l_ReturnValue;

      _asm
      {
         fld      qword ptr [in_Value]
         fistp    dword ptr [l_ReturnValue]
      }

      return l_ReturnValue;
   }
}

#endif //#ifndef _GOLEM_FLOAT_H_
