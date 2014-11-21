/********************************************************

golem_data_validate.h

Owner: Mathieu Tremblay

-----------------------------------------------
Description:
Different functions to validate data is valid
ex: isAlpha, isNum, isAlphaNum, isINT8, etc...
********************************************************/

#ifndef _GOLEM_DATA_VALIDATE_
#define _GOLEM_DATA_VALIDATE_

#include "golem_string.h"

namespace Hector
{

   /*************************************************
   * Validate if a string is in the range of the basic
   * data types  
   *
   * Each function is overloaded and provides an 
   * implementation for a string and for a GString
   *************************************************/
   bool isINT8( const string & value );
   bool isINT8( const GString & value );
   bool isINT16( const string &  value );
   bool isINT16( const GString & value );
   bool isINT32( const string &  value );
   bool isINT32( const GString & value );
   bool isINT64( const string &  value );
   bool isINT64( const GString & value );
   bool isUINT8( const string &  value );
   bool isUINT8( const GString & value );
   bool isUINT16( const string &  value );
   bool isUINT16( const GString & value );
   bool isUINT32( const string &  value );
   bool isUINT32( const GString & value );
   bool isUINT64( const string &  value );
   bool isUINT64( const GString & value );
   bool isREAL32( const string &  value );
   bool isREAL32( const GString & value );
   bool isREAL64( const string &  value );
   bool isREAL64( const GString & value );
   //bool isNumber( const string &  value );
   //bool isNumber( const GString & value );
   bool isDateTime( const string & value );
   bool isDateTime( const GString & value );
   bool isTimeSpan( const string & value );
   bool isTimeSpan( const GString & value );
   bool isNumber( const string & sValue, bool minusChar = true, bool decChar = false, bool sciChar = false );
   bool isNumber( const GString & sValue, bool minusChar = true, bool decChar = false, bool sciChar = false );   
   bool IsIP(const GString & in_String);

}

#endif