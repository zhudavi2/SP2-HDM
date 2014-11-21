#include "golem_sdk_api.h"
/************************************************
* File:
*  golem_string_table.h
* Owner
* =====
*  Mathieu Tremblay
* Description
* ===========
*   Set of class to manipulate string tables, 
*   Used for internationalization (i18n)purposes.
*
* History:
* ========
*    23 june 2003 (mtremblay)
*       - Creation
*
* Copyright (C) 2003, Laboratoires Golemlabs Inc.
*************************************************/

#ifndef _GStringTable_H_
#define _GStringTable_H_

namespace SDK
{
   /*!
   * Class that gives access to a string table content
   **/
   class GOLEM_SDK_API GStringTable
   {
   public:
      GStringTable();
      ~GStringTable();

      //! Initializes the string table object so it will index the given string table file
      bool Open(const GFile& in_File);
      //! Get the string that corresponds to the given Id
      bool Get_String(INT32 in_iID,GString& out_sString);
      //! Closes the string table
      bool Close();

   protected:
   private:
      //! IndexedData reader, will read the String table and handle it
      IndexedData::GReader m_DataReader;
      //! GFile object that is used to handle the string table file
      GFile                 m_File;
      //! Specifies if the String Table is initialized or not
      bool                  m_bInitialized;
      //! Buffer used to get the strings, is allocated to the largest string
      INT8*                m_pBuffer;
      //! Size of the buffer used to get the strings
      INT32                m_iBufferSize;
   };

}

#endif //_GStringTable_H_
