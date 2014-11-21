/**************************************************************
*
* golem_node.h
*
* Description
* ===========
*  Description & implementation of classes specifying generic
*  data holder node and parameters.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_NODE_
#define _GOLEM_NODE_

namespace Hector
{
   namespace EParameterType
   {
      /*!
       * Enumerates all possible type of data that can be 
       * represented by a parameter.
       **/
      enum Enum
      {
         Unknown,
         Bool,
         Int8,
         UInt8,
         Int16,
         UInt16,
         Int32,
         UInt32,
         Int64,
         UInt64,
         Real32,
         Real64,
         PBool,   // All array elements must be placed AFTER this element
         PInt8,
         PUInt8,
         PInt16,
         PUInt16,
         PInt32,
         PUInt32,
         PInt64,
         PUInt64,
         PReal32,
         PReal64,
         PChar,
         PVoid,
         ItemCount // This element should always be last
      };

      //! Indicate the size of each type specified in the EParameterType::Enum.
      const UINT32 c_iTypeSize[ItemCount] =
      {
         // This array MUST be in sync with the previous enum
         0,
         sizeof(bool),
         sizeof(INT8),
         sizeof(UINT8),
         sizeof(INT16),
         sizeof(UINT16),
         sizeof(INT32),
         sizeof(UINT32),
         sizeof(INT64),
         sizeof(UINT64),
         sizeof(REAL32),
         sizeof(REAL64),
         sizeof(bool), // Pointer elements are in fact the elements themselve
         sizeof(INT8),
         sizeof(UINT8),
         sizeof(INT16),
         sizeof(UINT16),
         sizeof(INT32),
         sizeof(UINT32),
         sizeof(INT64),
         sizeof(UINT64),
         sizeof(REAL32),
         sizeof(REAL64),
         sizeof(char),
         1 // Void pointers are in considered pointers to byte elements
      };
   }

   struct GNodeParameterBase
   {
      friend class GNode;

      //! Retrieve the total memory taken by this parameter
      /*!
       * @return An integer.
       **/
      inline UINT32 MemorySize() const
      {
         return MemorySize( (EParameterType::Enum) m_iType, m_iCount);
      }

      //! Retrieve the total memory taken by any kind of parameter parameter
      /*!
       * @param in_eType      : Type of parameter.
       * @param in_iCount     : Number of array elements.
       * @return An integer.
       **/
      static inline UINT32 MemorySize(EParameterType::Enum in_eType, UINT32 in_iCount)
      {
         return sizeof(GNodeParameterBase) + in_iCount * EParameterType::c_iTypeSize[in_eType];
      }

   protected:
      INT32                   m_iType; // Same values as EParameterType::Enum
      UINT32                  m_iCount;
   };

   /*!
    * Represents a parameter of a node, which is can be represented by any
    * type of data. The parameter does not contain the data itself but a pointer
    * to the actual data in the node. It is important to notice that if the node
    * changes, any parameter reference acquired prior to the node change
    * will be invalidated.
    **/
   class GNodeParameter : public GNodeParameterBase
   {
   public:
      //! Retrieve the type for this parameter.
      /*!
       * @return Type of the parameter.
       **/
      inline EParameterType::Enum Type() const
      {
         return (EParameterType::Enum) m_iType;
      }

      //! Number of elements in an array.
      /*!
       * Count is always 1 when type is not an array (PUInt8, PReal64, etc.) .
       * @return An integer with the number of array elements.
       **/
      UINT32 Count() const
      {
         return m_iCount;
      }

      //! Retrieve the pointer to the actual data.
      /*!
       * @return A pointer to the parameter data.
       **/
      void* Pointer() const
      {
         return (void*) &m_iDummyContent;
      }

      /*!
       * @return A constant reference to the parameter value.
       **/
      inline const bool& BoolValue() const
      {
         assert(m_iType == EParameterType::Bool);
         return * ( (bool*) &m_iDummyContent);
      }

      /*!
       * @return A reference to the parameter value.
       **/
      inline bool& BoolValue()
      {
         assert(m_iType == EParameterType::Bool);
         return * ( (bool*) &m_iDummyContent);
      }

      /*!
       * @return A constant reference to the parameter value.
       **/
      inline const INT8& Int8Value() const
      {
         assert(m_iType == EParameterType::Int8);
         return * ( (INT8*) &m_iDummyContent);
      }

      /*!
       * @return A reference to the parameter value.
       **/
      inline INT8& Int8Value()
      {
         assert(m_iType == EParameterType::Int8);
         return * ( (INT8*) &m_iDummyContent);
      }

      /*!
       * @return A constant reference to the parameter value.
       **/
      inline const UINT8& UInt8Value() const
      {
         assert(m_iType == EParameterType::UInt8);
         return * ( (UINT8*) &m_iDummyContent);
      }

      /*!
       * @return A reference to the parameter value.
       **/
      inline UINT8& UInt8Value()
      {
         assert(m_iType == EParameterType::UInt8);
         return * ( (UINT8*) &m_iDummyContent);
      }

      /*!
       * @return A constant reference to the parameter value.
       **/
      inline const INT16& Int16Value() const
      {
         assert(m_iType == EParameterType::Int16);
         return * ( (INT16*) &m_iDummyContent);
      }

      /*!
       * @return A reference to the parameter value.
       **/
      inline INT16& Int16Value()
      {
         assert(m_iType == EParameterType::Int16);
         return * ( (INT16*) &m_iDummyContent);
      }

      /*!
       * @return A constant reference to the parameter value.
       **/
      inline const UINT16& UInt16Value() const
      {
         assert(m_iType == EParameterType::UInt16);
         return * ( (UINT16*) &m_iDummyContent);
      }

      /*!
       * @return A reference to the parameter value.
       **/
      inline UINT16& UInt16Value()
      {
         assert(m_iType == EParameterType::UInt16);
         return * ( (UINT16*) &m_iDummyContent);
      }

      /*!
       * @return A constant reference to the parameter value.
       **/
      inline const INT32& Int32Value() const
      {
         assert(m_iType == EParameterType::Int32);
         return * ( (INT32*) &m_iDummyContent);
      }

      /*!
       * @return A reference to the parameter value.
       **/
      inline INT32& Int32Value()
      {
         assert(m_iType == EParameterType::Int32);
         return * ( (INT32*) &m_iDummyContent);
      }

      /*!
       * @return A constant reference to the parameter value.
       **/
      inline const UINT32& UInt32Value() const
      {
         assert(m_iType == EParameterType::UInt32);
         return * ( (UINT32*) &m_iDummyContent);
      }

      /*!
       * @return A reference to the parameter value.
       **/
      inline UINT32& UInt32Value()
      {
         assert(m_iType == EParameterType::UInt32);
         return * ( (UINT32*) &m_iDummyContent);
      }

      /*!
       * @return A constant reference to the parameter value.
       **/
      inline const INT64& Int64Value() const
      {
         assert(m_iType == EParameterType::Int64);
         return * ( (INT64*) &m_iDummyContent);
      }

      /*!
       * @return A reference to the parameter value.
       **/
      inline INT64& Int64Value()
      {
         assert(m_iType == EParameterType::Int64);
         return * ( (INT64*) &m_iDummyContent);
      }

      /*!
       * @return A constant reference to the parameter value.
       **/
      inline const UINT64& UInt64Value() const
      {
         assert(m_iType == EParameterType::UInt64);
         return * ( (UINT64*) &m_iDummyContent);
      }

      /*!
       * @return A reference to the parameter value.
       **/
      inline UINT64& UInt64Value()
      {
         assert(m_iType == EParameterType::UInt64);
         return * ( (UINT64*) &m_iDummyContent);
      }

      /*!
       * @return A constant reference to the parameter value.
       **/
      inline const REAL32& Real32Value() const
      {
         assert(m_iType == EParameterType::Real32);
         return * ( (REAL32*) &m_iDummyContent);
      }

      /*!
       * @return A reference to the parameter value.
       **/
      inline REAL32& Real32Value()
      {
         assert(m_iType == EParameterType::Real32);
         return * ( (REAL32*) &m_iDummyContent);
      }

      /*!
       * @return A constant reference to the parameter value.
       **/
      inline const REAL64& Real64Value() const
      {
         assert(m_iType == EParameterType::Real64);
         return * ( (REAL64*) &m_iDummyContent);
      }

      /*!
       * @return A reference to the parameter value.
       **/
      inline REAL64& Real64Value()
      {
         assert(m_iType == EParameterType::Real64);
         return * ( (REAL64*) &m_iDummyContent);
      }

      /*!
       * @return A constant pointer to the parameter array.
       **/
      inline const bool* PBoolValue() const
      {
         assert(m_iType == EParameterType::PBool);
         return (bool*) &m_iDummyContent;
      }

      /*!
       * @return A pointer to the parameter array.
       **/
      inline bool* PBoolValue()
      {
         assert(m_iType == EParameterType::PBool);
         return (bool*) &m_iDummyContent;
      }

      /*!
       * @return A constant pointer to the parameter array.
       **/
      inline const INT8* PInt8Value() const
      {
         assert(m_iType == EParameterType::PInt8);
         return (INT8*) &m_iDummyContent;
      }

      /*!
       * @return A pointer to the parameter array.
       **/
      inline INT8* PInt8Value()
      {
         assert(m_iType == EParameterType::PInt8);
         return (INT8*) &m_iDummyContent;
      }

      /*!
       * @return A constant pointer to the parameter array.
       **/
      inline const UINT8* PUInt8Value() const
      {
         assert(m_iType == EParameterType::PUInt8);
         return (UINT8*) &m_iDummyContent;
      }

      /*!
       * @return A pointer to the parameter array.
       **/
      inline UINT8* PUInt8Value()
      {
         assert(m_iType == EParameterType::PUInt8);
         return (UINT8*) &m_iDummyContent;
      }

      /*!
       * @return A constant pointer to the parameter array.
       **/
      inline const INT16* PInt16Value() const
      {
         assert(m_iType == EParameterType::PInt16);
         return (INT16*) &m_iDummyContent;
      }

      /*!
       * @return A pointer to the parameter array.
       **/
      inline INT16* PInt16Value()
      {
         assert(m_iType == EParameterType::PInt16);
         return (INT16*) &m_iDummyContent;
      }

      /*!
       * @return A constant pointer to the parameter array.
       **/
      inline const UINT16* PUInt16Value() const
      {
         assert(m_iType == EParameterType::PUInt16);
         return (UINT16*) &m_iDummyContent;
      }

      /*!
       * @return A pointer to the parameter array.
       **/
      inline UINT16* PUInt16Value()
      {
         assert(m_iType == EParameterType::PUInt16);
         return (UINT16*) &m_iDummyContent;
      }

      /*!
       * @return A constant pointer to the parameter array.
       **/
      inline const INT32* PInt32Value() const
      {
         assert(m_iType == EParameterType::PInt32);
         return (INT32*) &m_iDummyContent;
      }

      /*!
       * @return A pointer to the parameter array.
       **/
      inline INT32* PInt32Value()
      {
         assert(m_iType == EParameterType::PInt32);
         return (INT32*) &m_iDummyContent;
      }

      /*!
       * @return A constant pointer to the parameter array.
       **/
      inline const UINT32* PUInt32Value() const
      {
         assert(m_iType == EParameterType::PUInt32);
         return (UINT32*) &m_iDummyContent;
      }

      /*!
       * @return A pointer to the parameter array.
       **/
      inline UINT32* PUInt32Value()
      {
         assert(m_iType == EParameterType::PUInt32);
         return (UINT32*) &m_iDummyContent;
      }

      /*!
       * @return A constant pointer to the parameter array.
       **/
      inline const INT64* PInt64Value() const
      {
         assert(m_iType == EParameterType::PInt64);
         return (INT64*) &m_iDummyContent;
      }

      /*!
       * @return A pointer to the parameter array.
       **/
      inline INT64* PInt64Value()
      {
         assert(m_iType == EParameterType::PInt64);
         return (INT64*) &m_iDummyContent;
      }

      /*!
       * @return A constant pointer to the parameter array.
       **/
      inline const UINT64* PUInt64Value() const
      {
         assert(m_iType == EParameterType::PUInt64);
         return (UINT64*) &m_iDummyContent;
      }

      /*!
       * @return A pointer to the parameter array.
       **/
      inline UINT64* PUInt64Value()
      {
         assert(m_iType == EParameterType::PUInt64);
         return (UINT64*) &m_iDummyContent;
      }

      /*!
       * @return A constant pointer to the parameter array.
       **/
      inline const REAL32* PReal32Value() const
      {
         assert(m_iType == EParameterType::PReal32);
         return (REAL32*) &m_iDummyContent;
      }

      /*!
       * @return A pointer to the parameter array.
       **/
      inline REAL32* PReal32Value()
      {
         assert(m_iType == EParameterType::PReal32);
         return (REAL32*) &m_iDummyContent;
      }

      /*!
       * @return A constant pointer to the parameter array.
       **/
      inline const REAL64* PReal64Value() const
      {
         assert(m_iType == EParameterType::PReal64);
         return (REAL64*) &m_iDummyContent;
      }

      /*!
       * @return A pointer to the parameter array.
       **/
      inline REAL64* PReal64Value()
      {
         assert(m_iType == EParameterType::PReal64);
         return (REAL64*) &m_iDummyContent;
      }

      /*!
       * @return A constant pointer to the parameter array.
       **/
      inline const char* PChar() const
      {
         assert(m_iType == EParameterType::PChar);
         return (char*) &m_iDummyContent;
      }

      /*!
       * @return A pointer to the parameter array.
       **/
      inline char* PChar()
      {
         assert(m_iType == EParameterType::PChar);
         return (char*) &m_iDummyContent;
      }

      /*!
       * @return A constant pointer to the parameter array.
       **/
      inline const void* PVoid() const
      {
         assert(m_iType == EParameterType::PVoid);
         return (void*) &m_iDummyContent;
      }

      /*!
       * @return A pointer to the parameter array.
       **/
      inline void* PVoid()
      {
         assert(m_iType == EParameterType::PVoid);
         return (void*) &m_iDummyContent;
      }

      //! Copy the data of another parameter of the same type.
      /*!
       * Nothing happens if both parameters have different types.
       * @param in_Copy    : Parameter to copy data from.
       * @return A reference to this parameter.
       **/
      GNodeParameter& Copy(const GNodeParameter& in_Copy)
      {
         // Make sure parameters are of the same type
         if( (in_Copy.m_iCount == m_iCount) && 
             (m_iType == in_Copy.m_iType) )
         {
            memcpy(&m_iDummyContent, &in_Copy.m_iDummyContent, m_iCount * EParameterType::c_iTypeSize[m_iType] );
         }

         return *this;
      }

   private:
      // It is impossible to construct a GNodeParameter class since its content
      // is controlled by the GNode class
      GNodeParameter()
      {
      }

      GNodeParameter(const GNodeParameter& in_Copy)
      {
      }

      GNodeParameter& operator = (const GNodeParameter& in_Copy)
      {
         return *this;
      }

      // This member serves only to know where the parameter data is located.
      // EX.: a 3 UINT16 array is located at
      //       (UINT16*) &m_iDummyContent
      // This way of doing is only valid as long as the GNode class stores
      // the parameters directly into its data buffer
      UINT32                  m_iDummyContent;
   };

   /*!
    * This strcuture is used to describe a node parameter content.
    * The m_iCountDim0 and m_iCountDim1 members are used to describe
    * a 2 dimensionnal array parameter (set one to 1 to remove the 2nd dimension).
    * If one of them is 0, the corresponding m_iParamCountIDDim0 or m_iParamCountIDDim1
    * member is used to indicate a previous parameter that must be used to allocate
    * number of array elements.
    **/
   struct GNodeParameterDesc
   {
      EParameterType::Enum    m_eType;
      UINT32                  m_iCountDim0;
      UINT32                  m_iCountDim1;
      UINT32                  m_iParamCountIDDim0;
      UINT32                  m_iParamCountIDDim1;
   };

   /*!
    * Represents a generic data container node. It contains a list of parameters
    * with variable type, all of which are stored in a single large buffer.
    **/
   class GNode
   {
   public:
      static const UINT32 c_iInvalidIndex = 0xFFFFFFFF;
      static const UINT32 c_iLastIndex = 0xFFFFFFFF;

      //! Constructs an empty node
      GNode();

      //! Constructs a node from a copy
      GNode(const GNode& in_Copy);

      //! Deletes the node content
      ~GNode();

      //! Copy content of a node
      GNode& operator = (const GNode& in_Copy);

      //! Append a predefined parameter array for this node
      /*!
       * This method does not verify the validity of the in_pDescArray content.
       * This means the number of parameters added can be less than in_iNbParamDesc.
       * Also, m_iParamCountIDDim0 & m_iParamCountIDDim1 members are ignored when 
       * building the parameter array. Added parameter values is 0 after a call 
       * to this method.
       * @param in_iNbParamDesc     : Number of elements the the description array.
       * @param in_pDescArray       : Array of parameter descriptions
       **/
      void ConstructParameterArray(UINT32 in_iNbParamDesc, 
                                   const GNodeParameterDesc* in_pDescArray);

      //! Append a predefined parameter array for and fill its data
      /*!
       * This method does not verify the validity of the in_pDescArray content.
       * This means the number of parameters added can be less than in_iNbParamDesc.
       * Parameter values will be as specified by the data buffer. If the buffer
       * is not big enough, the remaining parameter values is 0.
       * @param in_iNbParamDesc     : Number of elements the the description array.
       * @param in_pDescArray       : Array of parameter descriptions
       * @param in_iDataSize        : Size of the data buffer.
       * @param in_pData            : Data buffer.
       **/
      void ConstructParameterArray(UINT32 in_iNbParamDesc, 
                                   const GNodeParameterDesc* in_pDescArray, 
                                   UINT32 in_iDataSize, 
                                   void* in_pData);

      //! Retrieve the name of the node
      /*!
       * @return Constant string reference containing name of the node.
       **/
      inline const GString& Name() const
      {
         return m_sName;
      }

      //! Retrieve the name of the node
      /*!
       * @return String reference containing name of the node.
       **/
      inline GString& Name()
      {
         return m_sName;
      }

      //! Retrieve the ID of the node
      inline const UINT32 ID() const
      {
         return m_iID;
      }

      //! Change the ID of the node
      inline void ID(UINT32 in_iNewID)
      {
         m_iID = in_iNewID;
      }

      //! Retrieve the number of parameters on this node
      /*!
       * @return Current node parameter count.
       **/
      UINT32 NbParameters() const;

      //! Add a parameter to the node
      /*!
       * Value of the added parameter will be reseted to 0.
       * Method fails if count if != 1 when adding a non-array parameter type.
       * @param in_eType      : Type of parameter to add.
       * @param in_iCount     : Size of parameter array.
       * @return ID of the new parameter, or c_iInvalidIndex if parameter could not be added.
       **/
      UINT32 AddParameter(EParameterType::Enum in_eType, UINT32 in_iCount = 1);

      //! Insert a parameter anywhere in the node
      /*!
       * Value of the inserted parameter will be reseted to 0.
       * Method fails if count if != 1 when inserting a non-array parameter type.
       * If in_iWhere is larger than current parameter count, the parameter will 
       * be added at the end of the node.
       * @param in_eType      : Type of parameter to add.
       * @param in_iCount     : Size of parameter array.
       * @param in_iWhere     : ID where parameter must be inserted.
       * @return ID of the new parameter, or c_iInvalidIndex if parameter could not be inserted.
       **/
      UINT32 InsertParameter(EParameterType::Enum in_eType, UINT32 in_iCount = 1, UINT32 in_iWhere = 0);

      //! Change the size of a parameter array
      /*!
       * When increasing the size of an array, the value of new elements will be 0.
       * Method fails if parameter does not exist or if parameter is not an array type.
       * @param in_iID        : ID of the parameter to change.
       * @param in_iNewCount  : New size of the parameter array.
       * @return Boolean indicating success of method.
       **/
      bool ChangeParameterArraySize(UINT32 in_iID, UINT32 in_iNewCount);

      //! Change the text of a parameter of type PChar
      /*!
       * Method fails if parameter does not exist or if parameter is not PChar type.
       * @param in_iID        : ID of the parameter to change.
       * @param in_iNewCount  : New text for the parameter.
       * @return Boolean indicating success of method.
       **/
      bool ChangeParameterText(UINT32 in_iID, const char* in_sNewText);

      //! Change the size of a parameter array using the parameter description
      /*!
       * When increasing the size of an array, the value of new elements will be 0.
       * Method fails if parameter does not exist, if parameter is not an array type
       * or if parameter description type differs.
       * @param in_iID        : ID of the parameter to change.
       * @param in_ParamDesc  : Parameter description.
       * @return Boolean indicating success of method.
       **/
      bool ChangeParameterArraySizeFromDesc(UINT32 in_iID, const GNodeParameterDesc& in_ParamDesc);

      //! Retrieve the specified parameter.
      /*!
       * The returned reference will be invalid if the ID given to the function 
       * is higher than the current node parameter count.
       * Any reference acquired by this method will be invalidated if parameters are
       * added and/or removed from the node.
       * @param in_iID        : ID of the parameter to retrieve.
       * @return A constant reference to a node parameter.
       **/
      const GNodeParameter& Parameter(UINT32 in_iID) const;

      //! Retrieve the specified parameter.
      /*!
       * The returned reference will be invalid if the ID given to the function 
       * is higher than the current node parameter count.
       * Any reference acquired by this method will be invalidated if parameters are
       * added and/or removed from the node.
       * @param in_iID        : ID of the parameter to retrieve.
       * @return A reference to a node parameter.
       **/
      GNodeParameter& Parameter(UINT32 in_iID);

      //! Delete the specified parameter from the node.
      /*!
       * Method will fail (and do nothing) if parameter ID is higher or equal to
       * the current node parameter count.
       * @param in_iParameterID  : ID of the parameter to delete.
       * @return Boolean indicating success of method.
       **/
      bool RemoveParameter(UINT32 in_iParameterID);

      //! Delete all parameters from the node.
      /*!
       * Clear does not free memory already allocated for the node. Call Pack.
       **/
      void ClearParameters();

      //! Pre-allocate memory for the node parameters
      /*!
       * @param in_iParameterCount  : Number of parameters to reserve memory for.
       * @param in_iByteCount       : Size of the memory buffer to reserve for the parameters.
       **/
      void Reserve(UINT32 in_iParameterCount, UINT32 in_iByteCount);

      //! Make sure node uses only required memory
      void Pack();

      //! Read the node data from a memory buffer
      /*!
       * This method is garanteed to work on any data written by the Write method.
       * The size passed to the function tells how many bytes the function can
       * read from the buffer, but is only used to make sure there is enough
       * data contained in the buffer. If function returns 0, the read of the node failed.
       * @param io_pBuffer       : Data buffer to read from
       * @param in_iSize         : Size of the data buffer
       * @return An integer indicating the number of bytes read from the buffer
       **/
      UINT32 Read(UINT8* io_pBuffer, UINT32 in_iSize);

      //! Read the node data from a file
      /*!
       * This method behaves exactly as the Read method, but from an already opened
       * file. The file does not have to be located at the begining.
       * There is an error if file does not contain enough data to fill correctly
       * the node. After the method call, the file pointer is located right after
       * the node data. If method returns 0, the read of the node failed.
       * @param in_File          : File to read data from.
       * @return An integer indicating the number of bytes read from the file
       **/
      UINT32 ReadFromFile(GFile& in_File);

      //! Write the node data to a memory buffer
      /*!
       * Data written by this method is garanteed to be usable by the Read method.
       * To know how many byte must be allocated for this method to work, call 
       * WriteSize.
       * @param io_pBuffer       : Buffer to write to
       * @return An integer indicating number of bytes written to the buffer
       **/
      UINT32 Write(UINT8* io_pBuffer) const;

      //! \todo TBA : WriteToFile when GFiles support write

      //! Retrieve how many bytes of data would be written by method Write
      /*!
       * This method is garanteed to return the same value as Write if the node content
       * does not change between the calls.
       * @return An integer indicating number of bytes that would be written
       **/
      UINT32 WriteSize() const;

   private:
      // Allocate memory for parameter data
      void AllocateParamMemory(UINT32 in_iParamDataSize);

      // Make place for a new parameter or larger parameter size
      void MoveParameters(UINT32 in_iStartMoveID, INT32 in_iDataOffset);

      // Same as insert but does not clear the parameter value
      UINT32 NewParameter(EParameterType::Enum in_eType, UINT32 in_iCount, UINT32 in_iWhere);

      // Use paramter description to compute size of an array
      UINT32 ArraySizeFromDesc(const GNodeParameterDesc& in_Desc) const;

      // Return size of a read/write header
      UINT32 HeaderSize() const;

      // Use data buffer content to initialize parameter indexes
      void ConstructIndexesFromData();

      UINT8*                  m_pData;
      UINT32                  m_iAllocatedDataSize;
      UINT32                  m_iDataSize;
      vector<UINT32>          m_vParameterIndexes;

      GString                 m_sName;
      UINT32                  m_iID;
   };
}

#endif // #ifndef _GOLEM_NODE_
