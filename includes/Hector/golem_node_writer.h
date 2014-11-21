/**************************************************************
*
* golem_node_writer.h
*
* Description
* ===========
*  Description & implementation of classes that allows generic
*  writing of a GNode object tree.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_NODE_WRITER_
#define _GOLEM_NODE_WRITER_

#include "golem_node.h"
#include "golem_node_file.h"

namespace Hector
{
   /*!
    * Virtual class that is to be implemented for a node to be writen to a file.
    **/
   class GNodeWriter
   {
      friend class GNodeTreeWriter;

   public:
      //! Default node writer constructor
      GNodeWriter() { m_pTreeWriter = NULL; m_pFile = NULL; }

      //! Virtual destructor for proper inherited class destruction
      virtual ~GNodeWriter() {;}

   protected:
      //! Called when file is just about to be opened.
      /*!
       * This function can be used to set the file header before it is writen.
       * Returning false will cancel the node file open.
       * @return Boolean indicating if node file must be opened.
       **/
      virtual bool BeginFile() = 0;

      //! Called when file is openened but file write has not begun yet.
      /*!
       * @return Boolean indicating if node file write must continue.
       **/
      virtual bool OpenFile() = 0;

      //! Called when a new node will be writen.
      /*!
       * To each BeginNode call there is a corresponding WriteNode and EndNode call.
       * WriteNode is always called immediately after the BeginNode method. If a 
       * BeginNode is called before a EndNode, it means the node is child of the
       * previous node passed to BeginNode. If this method returns false, the
       * node file write is stopped.
       * @param in_pNode         : Current node pointer.
       * @return Boolean indicating if node file write must continue.
       **/
      virtual bool BeginNode(const GNode* in_pNode) = 0;

      //! Called when a node is writen.
      /*!
       * See BeginNode. If this method returns false, the
       * node file write is stopped.
       * @param in_pNode         : Current node pointer.
       * @return Boolean indicating if node file write must continue.
       **/
      virtual bool WriteNode(const GNode* in_pNode) = 0;

      //! Called when a node is complete.
      /*!
       * See BeginNode. If this method returns false, the
       * node file write is stopped.
       * @param in_pNode         : Current node pointer.
       * @return Boolean indicating if node file write must continue.
       **/
      virtual bool EndNode(const GNode* in_pNode) = 0;

      //! Called when file write is complete and file is just about being closed.
      virtual void CloseFile() = 0;

      //! Called when file is closed.
      virtual void EndFile() = 0;

      //! Pointer to the tree writer that uses this object
      class GNodeTreeWriter*  m_pTreeWriter;

      //! Pointer to the file that must be written
      //! \todo TBM : Use GFile object when it will have write capability
      FILE*                   m_pFile;
   };

   /*!
    * Generic ASCII node file writer
    **/
   class GNodeWriterGenericASCII : public GNodeWriter
   {
   public:
      GNodeWriterGenericASCII();

   protected:
      //! See GNodeWriter::BeginFile.
      virtual bool BeginFile();

      //! See GNodeWriter::OpenFile.
      virtual bool OpenFile();

      //! See GNodeWriter::BeginNode.
      virtual bool BeginNode(const GNode* in_pNode);

      //! See GNodeWriter::WriteNode.
      virtual bool WriteNode(const GNode* in_pNode);

      //! See GNodeWriter::EndNode.
      virtual bool EndNode(const GNode* in_pNode);

      //! See GNodeWriter::CloseFile.
      virtual void CloseFile();

      //! See GNodeWriter::EndFile.
      virtual void EndFile();

   private:
      bool WriteTabs();
      bool WriteInt(INT32 in_iValue);
      bool WriteInt64(INT64 in_iValue);
      bool WriteUInt64(UINT64 in_iValue);
      bool WriteReal(REAL64 in_fValue, INT32 in_iDigitCount);
      bool WriteCStr(const char* in_sText);
      bool WriteStrN(const char* in_sText, UINT32 in_iDataSize);

      UINT32      m_iTabCount;

      // For conversion between values & text
      static char       m_pTempStrInt[256];
      static char       m_pTempStrFloat[_CVTBUFSIZE];
   };

   /*!
    * Generic binary node file writer
    **/
   class GNodeWriterGenericBinary : public GNodeWriter
   {
   public:
      GNodeWriterGenericBinary();
      ~GNodeWriterGenericBinary();

   protected:
      //! See GNodeWriter::BeginFile.
      virtual bool BeginFile();

      //! See GNodeWriter::OpenFile.
      virtual bool OpenFile();

      //! See GNodeWriter::BeginNode.
      virtual bool BeginNode(const GNode* in_pNode);

      //! See GNodeWriter::WriteNode.
      virtual bool WriteNode(const GNode* in_pNode);

      //! See GNodeWriter::EndNode.
      virtual bool EndNode(const GNode* in_pNode);

      //! See GNodeWriter::CloseFile.
      virtual void CloseFile();

      //! See GNodeWriter::EndFile.
      virtual void EndFile();

   private:
      UINT8* AllocateWriteBuffer(UINT32 in_iSize);

      vector<INT32>  m_vNodeOffset;

      UINT8*         m_pBuffer;
      UINT32         m_iSize;
   };

   /*!
    * Class that writes a tree of GNode objects into a file using 
    * a specified node writer class.
    **/
   class GNodeTreeWriter
   {
   public:
      //! Construct a default node file writer
      GNodeTreeWriter();

      //! Set the node writing object
      /*!
       * @param in_pNodeWriter   : Object that will be used to convert node objects to file content.
       **/
      void NodeWriter(GNodeWriter* in_pNodeWriter);

      //! Get the node writing object
      /*!
       * @return Pointer to the current node writing object
       **/
      GNodeWriter* NodeWriter();

      //! Retrieve the header of the node file
      /*!
       * @return Reference to node file header
       **/
      GNodeFileHeader& FileHeader();

      //! Write the specified tree of GNode objects to a file.
      /*!
       * The root of the tree is always ignored
       * @param in_sPath         : Path of the file to create & write.
       * @param in_NodeTree      : Tree of GNode objects to be writen.
       **/
      bool WriteNodeTree(const GString& in_sPath, const GTree<GNode>& in_NodeTree);

   private:
      // Disable copy & assigment
      GNodeTreeWriter(const GNodeTreeWriter&);
      GNodeTreeWriter& operator = (const GNodeTreeWriter&);

      bool WriteNodeChildren(const GTreeNode<GNode>* in_pTreeNode);
      bool WriteNode(const GTreeNode<GNode>* in_pTreeNode);

      GNodeFileHeader   m_Header;

      bool              m_bWriting;
      GNodeWriter*      m_pWriter;
   };
}

#endif // #ifndef _GOLEM_NODE_WRITER_
