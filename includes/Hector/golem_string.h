/**************************************************************
*
* golem_string.h
*
* Description
* ===========
*  The GString class is a container that enables the use of 
*  strings as normal types, such as using comparison and 
*  concatenation operations, iterators, and STL algorithms 
*  and copying and assigning with class allocator managed memory.
*  The GString, in combination with the GSString, automates the 
*  wchar-to-char and inverse conversions.
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#pragma once

#ifndef _GOLEMLAB_STRING___H
#define _GOLEMLAB_STRING___H

#include "golem_types.h"

#ifdef _MANAGED   
#pragma managed
#include <vcclr.h>   
#endif

namespace Hector
{
   class GString;
   /**
   * \ingroup hector
   * Casting class for GString. Represents a single-byte-character string
   *
   * \par requirements
   * Win2000 or later (UNICODE support) 
   *
   * \version 2.0
   * \date 03-24-2003
   *
   * \author nhatier
   *
   */   
   class GSString : public string
   {
      static char s_sCBuffer[]; 

   public:
      GSString();
      GSString(const string &);
      GSString(const wstring &);
      GSString(const char *);
      explicit GSString(INT32);
      explicit GSString(INT64);
      explicit GSString(REAL32);
      explicit GSString(REAL64);
      explicit GSString(UINT32);


      GSString operator + (const GSString&) const;      
      GSString operator + (const string&) const;
      GSString operator + (const char*) const;      
      void     operator +=(const GSString&);
      void     operator +=(const string&);
      void     operator +=(const char*);
      bool     operator ==(const GSString&) const;      
      bool     operator ==(const string&) const;      
      bool     operator ==(const char*) const;
      bool     operator !=(const GSString&) const;                
      bool     operator !=(const string&) const;
      bool     operator !=(const char*) const;

      operator const char *() const;
      operator const signed char *() const;
      operator const unsigned char *() const;
      operator GString() const;

      GSString ToUpper() const;
      GSString ToLower() const;

#ifdef _DEBUG // This is provided only for Intellisense
      size_t length() const;
      size_t size() const;
      const char * c_str() const;
      const_reference at(int) const;
      reference at(int);
      const_iterator begin() const;
      iterator begin();
      const_iterator end() const;
      iterator end();
      bool empty( ) const;
      GSString substr(int _Off = 0, int _Count = npos) const;
#endif
   };

   /**
   * Main String class for Ginger
   * Represents a Unicode (double-byte) character string, with many 
   * support functions and operators
   *
   * \par requirements
   * Win2000 or later (UNICODE support) 
   *
   * \version 2.0
   * \date 03-24-2003
   *
   * \author nhatier 
   */
   class GString : public wstring
   { 
      static wchar_t s_sWBuffer[]; 
      static  char   s_sCBuffer[]; 

   public: 
      GString();      

      GString(const wstring&);
      GString(const wchar_t*);
      GString(const string&);
      GString(const char*);
      explicit GString(INT32);
      explicit GString(INT64);
      explicit GString(REAL32);
      explicit GString(REAL64);
      explicit GString(UINT32);

      INT32  ToINT32()  const;
      INT64  ToINT64()  const;
      REAL32 ToREAL32() const;
      REAL64 ToREAL64() const;

      GString operator +(const GString&) const;      
      GString operator +(const string&) const;
      GString operator +(const char*) const;
      GString operator +(const wchar_t*) const;

      void operator +=(const GString&);
      void operator +=(const string&);
      void operator +=(const char*);      
      void operator +=(const wchar_t*);

      bool operator ==(const GString&) const;      
      bool operator ==(const string&) const;
      bool operator ==(const wchar_t*) const;
      bool operator ==(const char*) const;

      bool operator !=(const GString&) const;      
      bool operator !=(const wchar_t*) const;    
      bool operator !=(const string&) const;
      bool operator !=(const char*) const;

      operator GSString() const;

      GString ToUpper() const;
      GString ToLower() const;
      GString RTrim(wchar_t blankchar = ' ') const;
      GString LTrim(wchar_t blankchar = ' ') const;
      GString Trim(wchar_t blankchar = ' ') const;

      GString ReplaceNextPattern(const GString& in_ReplacingString, const GString& in_sPattern) const;

      //! This method is used to format a String
      static GString FormatString(const GString& in_sPattern, const GString& in_sFirst, ...);


      static GString FormatNumber(REAL64  in_fNumber,
                                  GString in_sNumberDelimiter = L" ",
                                  GString in_sDecimalDelimiter = L".",
                                  GString in_sPrefix = L"",
                                  GString in_sSuffix = L"",
                                  INT8    in_iGrouping = 3,
                                  INT8    in_iPrecision = 2,
                                  bool    in_iAbbreviate = false);
      
      static GString FormatNumber(REAL64  in_fNumber,
                                  INT8    in_iPrecision);

#ifdef _MANAGED   

      GString(System::String * in_systemString)
      {
         wchar_t __pin * pinned = PtrToStringChars(in_systemString);
         *this = pinned;
      }
      operator System::String *() const { return new System::String(this->c_str()); }   

#endif

#ifdef _DEBUG // This is provided only for Intellisense
      size_t length() const;
      size_t size() const;
      const wchar_t * c_str() const;
      const_reference at(int) const;
      reference at(int);
      const_iterator begin() const;
      iterator begin();
      const_iterator end() const;
      iterator end();
      bool empty( ) const;
      GString substr(int _Off = 0, int _Count = npos) const;
#endif
   };

   typedef vector<GString> GStringVec;
   typedef GStringVec::iterator GStringItr;
   typedef GStringVec::const_iterator GStringCItr;


   GString operator + (const wchar_t *, const GString &);
   GString operator + (const char *, const GString &);
   GSString operator + (const char *, const GSString &);
}

_STDEXT_BEGIN

inline size_t hash_value(const GString & _Str)
{
   return hash_value(_Str.c_str());
}

inline size_t hash_value(const GSString & _Str)
{
   return hash_value(_Str.c_str());
}

_STDEXT_END

#ifdef _MANAGED   
#pragma unmanaged
#endif
#endif