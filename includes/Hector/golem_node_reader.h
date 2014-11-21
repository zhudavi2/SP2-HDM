/**************************************************************
*
* golem_node_reader.h
*
* Description
* ===========
*  Description & implementation of classes that allows generic
*  reading of a GNode object tree.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_NODE_READER_
#define _GOLEM_NODE_READER_

#include "golem_design_pattern_observer.h"

namespace Hector
{
   class GNode;

   //! Describe how a node is to be constructed
   struct GNodeTypeDesc
   {
      CHAR*                      m_sName;
      UINT32                     m_iID;
      UINT32                     m_iParamCount;
      const GNodeParameterDesc*  m_pParamDescs;
   };

   /*!
    * Virtual class that is to be implemented for a node to be read from a file.
    **/
   class GNodeReader
   {
      friend class GNodeTreeReader;

   public:
      //! Default node reader constructor
      GNodeReader() { m_pTreeReader = NULL; m_pFile = NULL; }

      //! Virtual destructor for proper inherited class destruction
      virtual ~GNodeReader() {;}

   protected:
      //! Called when file is just about to begin being read.
      /*!
       * Returning false will cancel the node file read.
       * @return Boolean indicating if node file must be read.
       **/
      virtual bool BeginRead() = 0;

      //! Called to read next node in the current hierarchical level.
      /*!
       * Set out_iNodeAvail to zero when no more node can be found in the current level.
       * @param out_iNodeAvail indicating if another node exists in the current level.
       * @return Boolean indicating if node file read must continue.
       **/
      virtual bool ReadNextNode(UINT32& out_iNodeAvail) = 0;

      //! Called to read the current node content.
      /*!
       * If this method returns false, the node file read is stopped.
       * @param in_pNode         : Current node pointer to fill.
       * @return Boolean indicating if node file read must continue.
       **/
      virtual bool ReadNode(GNode* in_pNode) = 0;

      //! Called when file read is complete and file is just about being closed.
      virtual void EndRead() = 0;

      //! Pointer to the tree reader that uses this object
      class GNodeTreeReader*  m_pTreeReader;

      //! Pointer to the current file read
      GFile*                  m_pFile;
   };

   /*!
    * Generic ASCII node file reader
    **/
   class GNodeReaderGenericASCII : public GNodeReader
   {
   public:
      GNodeReaderGenericASCII();
      ~GNodeReaderGenericASCII();

   protected:
      //! See GNodeReader::BeginRead.
      virtual bool BeginRead();

      //! See GNodeReader::ReadNextNode.
      virtual bool ReadNextNode(UINT32& out_iNodeAvail);

      //! See GNodeReader::ReadNode.
      virtual bool ReadNode(GNode* in_pNode);

      //! See GNodeReader::EndRead.
      virtual void EndRead();

   private:
      bool ReadNextChunk();

      bool ReadChar();

      bool SkipComments();

      void ReadParameter(GNode* in_pNode, UINT32 in_iParamID, UINT32 in_iValueIndex);

      bool IsDelimiter(char in_cData) const;

      char*          m_pBuffer;
      UINT32         m_iBufferSize;
      UINT32         m_iNextBufferIndex;

      char*          m_pLastChunk;
      char           m_cLastDelimiter;
      bool           m_bReadNextNode;
   };

   /*!
    * Generic binary node file reader
    **/
   class GNodeReaderGenericBinary : public GNodeReader
   {
   public:
      GNodeReaderGenericBinary();
      ~GNodeReaderGenericBinary();

   protected:
      //! See GNodeReader::BeginRead.
      virtual bool BeginRead();

      //! See GNodeReader::ReadNextNode.
      virtual bool ReadNextNode(UINT32& out_iNodeAvail);

      //! See GNodeReader::ReadNode.
      virtual bool ReadNode(GNode* in_pNode);

      //! See GNodeReader::EndRead.
      virtual void EndRead();

   private:
      vector<UINT32>       m_vNodeSize;
      UINT32               m_iNextNode;
   };

   /*!
    * Class that reads a node file and uses a specified node reader class
    * to convert file content into a tree of GNode objects
    **/
   class GNodeTreeReader : public DesignPattern::GSubject
   {
   public:
      //! Construct a default node file reader
      GNodeTreeReader();

      //! Register a type of node
      /*!
       * @param in_pNodeType     : Structure that describes the node type
       **/
      void RegisterNodeType(const GNodeTypeDesc& in_NodeType);

      //! Unregister a type of node by ID
      /*!
       * @param in_iNodeTypeID   : ID of the type of node.
       * @return A boolean indicating success of unregistration.
       **/
      bool UnregisterNodeType(UINT32 in_iNodeTypeID);

      //! Unregister a type of node by name
      /*!
       * @param in_sNodeTypeName   : Name of the type of node.
       * @return A boolean indicating success of unregistration.
       **/
      bool UnregisterNodeType(const GString& in_sNodeTypeName);

      //! Unregister all currently registered type of nodes
      void UnregisterAllNodeTypes();

      //! Retrieve a node type description by its name.
      /*!
       * Method returns NULL if node type is invalid.
       * @param in_sNodeTypeName : Name of the type of node.
       * @return A pointer to a node type description.
       **/
      const GNodeTypeDesc* NodeType(const GString& in_sNodeTypeName);

      //! Retrieve a node type description by its ID.
      /*!
       * Method returns NULL if node type is invalid.
       * @param in_iNodeTypeID   : ID of the type of node.
       * @return A pointer to a node type description.
       **/
      const GNodeTypeDesc* NodeType(UINT32 in_iNodeTypeID);

      //! Convert the specified file content into a tree of GNode objects
      /*!
       * @param in_File          : File to read
       * @param out_NodeTree     : Tree that will be filled with GNode objects
       **/
      bool CreateNodeTree(GFile& in_File, GTree<GNode>& out_NodeTree);

      //! Retrieve the header of the node file
      /*!
       * Only valid after a sucessful call to CreateNodeTree.
       * @return Reference to node file header
       **/
      GNodeFileHeader& FileHeader();

      static const DesignPattern::GSubjectNotification c_iHeaderRead = 1;

   private:
      // Disable copy & assigment
      GNodeTreeReader(const GNodeTreeReader&);
      GNodeTreeReader& operator = (const GNodeTreeReader&);

      bool ReadNodeRecursively(GFile& in_File, GTreeNode<GNode>* in_pTreeNode);

      vector<const GNodeTypeDesc*>        m_vNodeTypeList;
      stdext::hash_map<GString, UINT32>   m_NodeTypeNameMap;

      GNodeFileHeader                     m_Header;

      bool                                m_bReading;
      GNodeReader*                        m_pReader;

      GNodeReaderGenericASCII             m_DefaultASCIIReader;
      GNodeReaderGenericBinary            m_DefaultBinaryReader;
   };
}

#endif // #ifndef _GOLEM_NODE_READER_
