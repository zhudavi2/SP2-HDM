/**************************************************************
*
* golem_scanner.h
*
* Description
* ===========
*	Scanner classes to easily parse text
*
* Owner
* =====
*	Mathieu Tremblay
*
* History:
* ========
* 8 novembre 2002(mtremblay)
*    - Creation of the CUnicodeScanner class        
*  23 june 2003 (mtremblay)
*     - Conversion to respect the new coding rules
*
* Copyright (C) 2003, Laboratoires Golemlabs Inc.
****************************************************************/



#ifndef _GOLEM_SCANNER_
#define _GOLEM_SCANNER_


//disable warning about truncated debug info
#pragma warning (disable : 4786)
#include "golem_types.h"
#include "golem_memory_buffer.h"
#include "golem_string.h"

namespace Hector
{

   /*!
   * Basic char scanner, to handle text
   **/
   class GBasicCharScanner
   {
   public:
      //! Constructor
      GBasicCharScanner();
      //! Destructor
      ~GBasicCharScanner();

      //! Get the variable m_bMultipleDelimiter which specifies if consecutive delimiters are considered as one
      bool MultipleDelimiterAsOne();
      //! Set if multiple consecutive delimiters are considered as one
      void MultipleDelimiterAsOne( bool in_bMult );
      //! Add a delimiter to the list
      bool AddDelimiter( char in_cDelimiter );
      //! Sets the text to scan
      bool SetText( const string& in_sText );
      //! Sets the text to scan
      bool SetText( const char* in_pText );
      //! Reset the scan
      bool ResetScan();
      //! Get the first token of the scanned string
      bool GetFirstToken( string& out_sTokenText );
      //! Get the first token of the scanned string
      bool GetFirstToken( string& out_sTokenText,char& out_cDelimiter );
      //! Get the next token of the scanned string
      bool GetNextToken( string& out_sTokenText );
      //! Get the next token of the scanned string
      bool GetNextToken( string& out_sTokenText, char& out_cDelimiter );
      //! Get the last token of the scanned string
      bool GetLastToken( string& out_sTokenText );
      //! Get the last token of the scanned string
      bool GetLastToken( string& out_sTokenText, char& out_cDelimiter );

   private:
      //! Vector of delimitors (where to cut the text)
      vector<char>    m_Delimiter;
      //! Position of the scan
      UINT32          m_iPosScan;
      //! Memory buffer
      GMemoryBuffer   m_Buffer;
      //! Length of the text
      UINT32          m_iTextLength;
      //! Specifies if multiple consecutive delimiters are considered as one
      bool            m_bMultipleDelimiter;
   };

}

#endif //#ifndef _GOLEM_SCANNER_