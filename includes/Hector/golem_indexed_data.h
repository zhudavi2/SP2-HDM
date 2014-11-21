/************************************************
* golem_indexed_data.h
*
* Description
* ===========
*	 Set of class to manipulate a file that contains indexed data
*	 Includes reading, adding new entries and if necessary updating existing
*	 data.
*
* Owner
* =====
*   Mathieu Tremblay
*
* History:
* ========
*  20 juin 2003:			Creation
*
*
* Copyright (C) 2003, Laboratoires Golemlabs Inc.
*************************************************/

#ifndef _CIndexedData_H_
#define _CIndexedData_H_

#include "golem_smart_ptr.h"

namespace Hector
{

   /*!
   * This namespace contains classes that manipulate indexed data
   **/
   namespace IndexedData
   {

      //!Define a smart pointer to a buffer
      typedef GSmartPtr<INT8> GSPtrInt8;


      /*!
      * Class to write an Indexed Data file
      *    Data must be added, and when all the data has been added,
      *    a call to Write() must be done to generate the file.
      **/
      class GWriter
      {
      public:
         GWriter();
         ~GWriter();

         //! Adds data to write
         INT32 AddData(const void* in_pBuffer,const INT32 in_iBufferSize);
         //! Writes the added data into a new indexed file
         bool Write(const GString& in_sFilename);
         void NextId(INT32 in_iNextId);
      private:
         /*!
         * Small structure to hold the data to be written together
         **/
         struct GData
         {
            INT32            m_iId;
            GSmartPtr<INT8> m_Buffer;
            INT32			     m_iSize;
         };

         //! Id for the next added data
         INT32         m_iNextID;
         //! Vector of contained data elements
         vector<GData> m_Data;
      }; //end of class GWriter

   /*!
   * Indexed data file reader, reads an indexed file
   * and after that it can be queried on the data it contains
   * by index.
   **/
   class GReader
   {
   public:
	   GReader();
	   ~GReader();

	   //! Open the given indexed file and loads its index
	   bool Open(const GString& in_sFilename);
	   //! Closes the indexed file and clear everything
	   bool Close();

	   //! Gets data from the file, filling the already allocated buffer
	   bool Data(INT32 in_iIndex,void* out_pBuffer,INT32 in_iBufferSize,INT32& out_iBufferSize);
      //! Gets the size of the data at the given index
      INT32 Get_Size(INT32 in_iIndex);

      vector<INT32> Indexes();
      private:

         /*!
         * Small structure to hold an index entry in the Reader
         **/
         struct GIndex_Entry
         {
            INT32 m_iId;
            INT32 m_iOffset;
            INT32 m_iSize;
         };

         //! indexed file pointer
         FILE* m_pFile;

         //!Index map, contains the indexed entries
         stdext::hash_map<INT32,GIndex_Entry> m_Index;

      }; //end of class GReader

   }; //End of namespace IndexedData

}

#endif //_CIndexedData_H_
