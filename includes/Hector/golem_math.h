/**************************************************************
*
* golem_math.h
*
* Description
* ===========
*  Mathematical constants.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_MATH_
#define _GOLEM_MATH_

namespace Hector
{
   namespace Math
   {

      //! Constant with pi value
         const REAL64 PI = 3.141592653589793238462643383279502884197169399375105820974944\
5923078164062862089986280348253421170679821f;
//      const REAL32 PI   = 3.14159265359f;

      //! Value considered negligible
      const REAL32 EPSILON = 0.000001f;

   }
}

#endif //#ifndef _GOLEM_MATH_