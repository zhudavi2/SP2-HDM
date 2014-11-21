/**************************************************************
*
* golem_types.h
*
* Description
* ===========
*  Various type definitions
*
* Owner
* =====
*  Mathieu Tremblay, Germain Sauve
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_TYPES_
#define _GOLEM_TYPES_

/**************************
Windows typedefs
***************************/
typedef signed char           INT8;
typedef signed short          INT16;
typedef signed int            INT32;
typedef signed __int64        INT64;
typedef unsigned char         UINT8;
typedef unsigned short        UINT16;
typedef unsigned int          UINT32;
typedef unsigned __int64      UINT64;
typedef float                 REAL32;
typedef double                REAL64;
typedef wchar_t               WCHAR;
typedef char*                 STR;
typedef wchar_t*              WSTR;
typedef const wchar_t* const  CWSTR;

struct REAL16
{
   union REAL32Part
   {
      struct Parts
      {
         UINT32   m_Mantissa:23;
         UINT32   m_Exponent:8;
         UINT32   m_Sign:1;
      } m_Parts;
      float m_Value;
   };

   UINT16   m_Mantissa:10;
   UINT16   m_Exponent:5;
   UINT16   m_Sign:1;

   REAL16& operator = (float in_Value)
   {
      REAL32Part l_ValuePart;
      l_ValuePart.m_Value = in_Value;

      m_Sign = l_ValuePart.m_Parts.m_Sign;
      m_Exponent = max(min(l_ValuePart.m_Parts.m_Exponent, 127 + 16), 127 - 15) - 127 + 15;
      m_Mantissa = l_ValuePart.m_Parts.m_Mantissa >> 13;

      return *this;
   }

   operator float () const
   {
      REAL32Part l_ValuePart;

      l_ValuePart.m_Parts.m_Sign = m_Sign;
      l_ValuePart.m_Parts.m_Exponent = m_Exponent - 15 + 127;
      l_ValuePart.m_Parts.m_Mantissa = m_Mantissa << 13;

      return l_ValuePart.m_Value;
   }
};


#endif // #ifndef _GOLEM_TYPES_
