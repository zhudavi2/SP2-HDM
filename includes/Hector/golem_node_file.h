/**************************************************************
*
* golem_node_file.h
*
* Description
* ===========
*  Description & implementation of classes describing the 
*  content of a node file.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_NODE_FILE_
#define _GOLEM_NODE_FILE_

namespace Hector
{
   /*!
    * Describe the header of a node file.
    **/
   class GNodeFileHeader
   {
   public:
      //! Retrieve the type of node file content (FOURCC)
      inline UINT32 Type() const
      {
         return m_iType;
      }

      //! Check if type is equal to the specified 4 char sequence
      inline bool IsTypeEqual(const char in_cTypeCompare[4] ) const
      {
         return m_iType == MAKEFOURCC(in_cTypeCompare[0],
                                      in_cTypeCompare[1],
                                      in_cTypeCompare[2],
                                      in_cTypeCompare[3] );
      }

      //! Change the type of node file content (FOURCC)
      inline void Type(UINT32 in_iNewType)
      {
         m_iType = in_iNewType;
      }

      //! Change the type of node file content (4 chars)
      inline void Type(const char in_cNewType[4] )
      {
         m_iType = MAKEFOURCC(in_cNewType[0], 
                              in_cNewType[1], 
                              in_cNewType[2], 
                              in_cNewType[3] );
      }

      //! Retrieve the version of node file content (FOURCC)
      inline UINT32 Version() const
      {
         return m_iVersion;
      }

      //! Check if version is equal to the specified 4 char sequence
      inline bool IsVersionEqual(const char in_cVersionCompare[4] ) const
      {
         return m_iVersion == MAKEFOURCC(in_cVersionCompare[0],
                                         in_cVersionCompare[1],
                                         in_cVersionCompare[2],
                                         in_cVersionCompare[3] );
      }

      //! Change the version of node file content (FOURCC)
      inline void Version(UINT32 in_iNewVersion)
      {
         m_iVersion = in_iNewVersion;
      }

      //! Change the version of node file content (4 chars)
      inline void Version(const char in_cNewVersion[4] )
      {
         m_iVersion = MAKEFOURCC(in_cNewVersion[0], 
                                 in_cNewVersion[1], 
                                 in_cNewVersion[2], 
                                 in_cNewVersion[3] );
      }

      //! Retrieve the encoding of node file content (FOURCC)
      inline UINT32 Encoding() const
      {
         return m_iEncoding;
      }

      //! Check if encoding is equal to the specified 4 char sequence
      inline bool IsEncodingEqual(const char in_cEncodingCompare[4] ) const
      {
         return m_iEncoding == MAKEFOURCC(in_cEncodingCompare[0],
                                          in_cEncodingCompare[1],
                                          in_cEncodingCompare[2],
                                          in_cEncodingCompare[3] );
      }

      //! Change the encoding of node file content (FOURCC)
      inline void Encoding(UINT32 in_iNewEncoding)
      {
         m_iEncoding = in_iNewEncoding;
      }

      //! Change the encoding of node file content (4 chars)
      inline void Encoding(const char in_cNewEncoding[4] )
      {
         m_iEncoding = MAKEFOURCC(in_cNewEncoding[0], 
                                  in_cNewEncoding[1], 
                                  in_cNewEncoding[2], 
                                  in_cNewEncoding[3] );
      }

   private:
      UINT32      m_iType;
      UINT32      m_iVersion;
      UINT32      m_iEncoding;
   };
}

#endif // #ifndef _GOLEM_NODE_FILE_
