/**************************************************************
*
* golem_datastruct.h
*
* Description
* ===========
*   Utility to define dynamically data structures
*
* Owner
* =====
*     Mathieu Tremblay
*
* History
* =======
*  23 oct 2002 (mtremblay):    
*      - Added a wrapper over the CStructuredData so a data field
*        can be associated with a name : GDynamicData class created
*  23 june 2003 (mtremblay)
*     - Conversion to respect the new coding rules
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_DATASTRUCT_
#define _GOLEM_DATASTRUCT_

#pragma warning(disable : 4786)

#include "golem_types.h"

namespace Hector
{

   /*!
   * Enumerates the possible dynamic data types
   **/
   enum EGolemDataType
   {
      e_UCHAR,          //unicode char
      e_CHAR,           //char
      e_INT8,           //8-bit  integer
      e_INT16,          //16-bit integer
      e_INT32,          //32-bit integer
      e_INT64,          //64-bit integer
      e_UINT8,          //8-bit  unsigned integer
      e_UINT16,         //16-bit unsigned integer
      e_UINT32,         //32-bit unsigned integer
      e_UINT64,         //64-bit unsigned integer
      e_REAL32,         //float
      e_REAL64,         //double
      e_DATETIME,       //date/time
      e_TIMESPAN,       //timespan
      e_STRUCT,         //some structure, variable size
      e_UNKNOWN = -1
   };


   /*!
   * Type definition
   *    Used to define a basic data type so it can be used
   *    to represent data in a file, memory, dynamically
   **/
   class GTypeDefinition
   {
   public:

      //! Get the type of data corresponding to this definition
      EGolemDataType     Type() const;
      //! Get the size of the data (bytes)
      UINT32               Size() const;

      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_STRUCT;  
      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_UNKNOWN; 
      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_CHAR;    
      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_UCHAR;   
      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_INT8;    
      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_INT16;   
      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_INT32;   
      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_INT64;   
      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_UINT8;   
      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_UINT16;  
      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_UINT32;  
      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_UINT64;  
      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_REAL32;  
      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_REAL64;  
      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_DATETIME;  
      //! Static declarations of the common type definitions
      static const GTypeDefinition DataType_TIMESPAN;  

      //! Get the static member corresponding to the given enum
      static const GTypeDefinition & From_Type(EGolemDataType in_eType);

   private:
      //! Constructor
      GTypeDefinition();
      //! Copy constructor
      GTypeDefinition(const GTypeDefinition &);
      //! Constructor - from a given type and size
      GTypeDefinition(EGolemDataType in_eType, UINT32 in_iSize);   
      ~GTypeDefinition();

      //! Type of the Type definition
      EGolemDataType m_eType; 
      //! Size of the type in bytes
      UINT32 m_iSize;        
   };


   /*!
   * Struct to associate an offset and a qty (to represent arrays) with a GTypeDefinition.
   * The CStructuredData contains a list of this
   */
   struct GDataField
   {
      //! Pointer to the type definition of the field
      const GTypeDefinition* m_pTypeDefPtr;  
      //! Offset from beginning
      UINT32                 m_iOffset    ;  
      //! For arrays
      UINT32                 m_iQty       ;  
      //! Field name
      GString                m_sName      ;

      /*!
      * Calculates the size of the actual field:
      *      size = byte size of type * m_qty
      *
      *      Ex : an array of 16 INT64 in a row
      *           takes : 16 * 8 bytes
      * @return The size in bytes of the field
      *****************/
      UINT32 Size() const
      { 
         return (m_iQty * m_pTypeDefPtr->Size());
      }

      /*!
      * Set the type of the actual field
      * @param in_eType: an enum value specifying the type of data
      **/
      void Type(EGolemDataType in_eType) 
      {
         m_pTypeDefPtr = &GTypeDefinition::From_Type(in_eType);
      }
   };

   /*!
   *
   * Class that represents a data structure, once defined, can be associated to a 
   * buffer of raw data to interpret it.
   */
   class GDataStructure
   {
      //! Holds the list of fields in the structure
      vector<GDataField> m_FieldList;

   public:
      //! Constructor
      GDataStructure();
      //! Constructs a structure from a string using the GStructBuilder
      GDataStructure(const GString & in_sDefinition);
      //! Destructor
      ~GDataStructure();

      //! Adds a field of data to the structure
      bool AddField(const GString & in_sName, 
         const GTypeDefinition* in_pType,
         UINT32 in_iOffset,
         UINT32 in_iQty);

      //! Removes a field of data from the structure
      bool RemoveField(const GString & in_sName);
      //! Removes a field of data from the structure
      bool RemoveField(UINT32 in_iIndex);

      //! Get a field from the structure
      bool GetField(UINT32 in_iIndex,GDataField& in_Field) const;
      //! Get a field from the structure
      bool GetField(const GString & in_sName,GDataField& in_Field) const;
      //! Get a field from the structure
      const GDataField * GetField(UINT32 in_iIndex) const;
      //! Get a field from the structure
      const GDataField * GetField(const GString & in_sName) const;    

      //! Get the index of the given field
      INT32 GetFieldIndex(const GString & in_sName) const;

      //! Get the total size (in bytes) of the structure
      UINT32 Size() const; 

      //! Get the list of fields that defines the structure
      const vector<GDataField> GetFieldList() const ;

      //! Get the number of fields that defines the structure
      UINT32 NbFields() const;
   };



   //! TODO: Remove this global symbol
   //!  Common data structure (see golem_datastruct.cpp for the real declaration)
   extern GDataStructure s_MOUSE_EVENT;
   //! TODO: Remove this global symbol
   //!  Common data structure (see golem_datastruct.cpp for the real declaration)
   extern GDataStructure s_KEYBOARD_EVENT;
   //Constants to define mouse events values - see .h for more information
#define MOUSE_UNKNOWN		'u'
#define MOUSE_LEFT			'l'
#define MOUSE_RIGHT			'r'
#define MOUSE_MIDDLE		   'm'
#define MOUSE_WHEEL			'w'





   /*!
   * Associates a buffer of raw data with a Data Structure
   **/
   class GDynamicData
   {
   private:
      //! Buffer of raw data
      INT8*                   m_pBuffer;
      //! Structure defining the data structure
      const GDataStructure*   m_pStructure;
      //! Specifies if the memory in the buffer is owned by the GDynamicData class (if the destructor must free m_buffer)
      bool                    m_bOwnMemory; 

   public:
      //! Constructor
      GDynamicData();
      //! Other constructor
      GDynamicData(GDataStructure* in_pDataStructPtr,INT8* in_pBuffer);
      //! Copy constructor
      GDynamicData(const GDynamicData& in_Data);
      //! Destructor
      ~GDynamicData();

      //! Get the memory usage
      bool                    Own_Memory();      
      //! Set the memory usage
      void                    Own_Memory(bool in_bOwn);

      //! Set the structure of the data
      bool                    Structure(const GDataStructure* in_pStructPtr);
      //! Get the structure of the data
      const GDataStructure *  Structure() const;

      //! Set the buffer to use
      bool                    Buffer(INT8* in_pBuffer);
      //! Get the buffer
      const INT8*             Buffer() const;
      //! Get the data at the given field
      const INT8*             Data(const GString & in_sName,  UINT * in_pLen = NULL) const;
      //! Get the data at the given field
      const INT8*             Data(UINT32 in_iIndex, UINT * in_pLen = NULL) const;
      //! Set the data at the given field
      bool                    Data(const GString & in_sName, const INT8* in_pData, UINT32 in_iSize);
      //! Set the data at the given field
      bool                    Data(UINT32 in_iIndex,const INT8* in_pData, UINT32 in_iSize);  

      //! Get the data at the given field as a String
      GString                 DataAsString(UINT32 in_iIndex) const; 
      //! Get the data at the given field as a String
      GString                 DataAsString(const GString & in_sName) const;
   };



   /*!
   * Class that helps building a GDataStructure from a string,
   * @see the Build() function
   **/
   class GStructBuilder
   { 
   private:
      //!  Adds a member to the constructed data structure
      bool AddMember(GDataStructure* in_pData,const GString * in_pTokenPtr,UINT32 in_iNbTokens);
   public:
      GStructBuilder();
      ~GStructBuilder();

      //! Fills a GDynamicData Structure with new fields that are given in a brief way with the input string
      bool Build(const GString & in_sDef,GDataStructure* in_pStructure);
   };

}

#endif

//EOF

