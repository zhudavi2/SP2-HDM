/**************************************************************
*
* golem_quad_tree.h
*
* Description
* ===========
*  Description & implementation of GQuadTree & GTreeNode template 
*  classes
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_QUAD_TREE_H
#define _GOLEM_QUAD_TREE_H

namespace Hector
{
   template<typename T> 
   /*!
    * Class that implements a quad tree data structure.
    * A quad tree necessarily has 4 childs per node.
    * The current implementation uses a straight array to 
    * store all tree nodes, which means that it allocates 
    * h^4 nodes (where h is the maximum tree height).
    * 
    * This class is optimal when the tree is balaced, i.e.
    * all branches in the tree have the same height.
    **/
   class GQuadTree
   {
      friend class GNode;
   public:
      class GNode
      {
         friend class GQuadTree;
      private:
         GNode(UINT32 in_iID, GQuadTree* in_pOwner)
         {
            m_iId = in_iID;
            m_pOwner = in_pOwner;
         }
      public:
         //! Create an empty node
         GNode()
         {
            m_iId = 0xFFFFFFFF;
            m_pOwner = NULL;
         }

         //! Create a node copy
         GNode(const GNode& in_Copy)
         {
            m_iId = in_Copy.m_iId;
            m_pOwner = in_Copy.m_pOwner;
         }

         //! Get the parent of this node
         GNode Parent() const
         {
            if(m_iId == 0)
            {
               return *this;
            }
            return GNode( (m_iId - 1) >> 2, m_pOwner);
         }

         //! Return number of children for this node (can be 0 or 4)
         UINT32 NbChildren() const;

         //! Add 4 children to this node
         void AddQuadChildren() const;

         //! Remove all 4 children from this node
         void RemoveQuadChildren() const;

         //! Get the specified node child
         GNode Child(UINT32 in_iChildID) const;

         //! Get the constant node data
         const T& Data() const
         {
            return m_pOwner->m_vNodes[m_iId].m_Data;
         }

         //! Get the node data
         T& Data();

         //! Return the depth of the node
         UINT32 Depth() const;

      private:
         UINT32            m_iId;
         GQuadTree<T>*     m_pOwner;
      };

      //! Create a new quad tree with only a root element
      GQuadTree()
      {
         m_vNodes.resize(1);
         m_iNextSizeInc = 4;
      }

      //! Get root node
      GNode Root()
      {
         return GNode(0, this);
      }

      //! Return the height of the quad tree
      UINT32 Height() const
      {
         UINT32 l_iHeight = 0;
         UINT32 l_iSize = m_vNodes.size() - 1;
         while(l_iSize > 0)
         {
            l_iHeight ++;
            l_iSize = (l_iSize >> 2) - 1;
         }

         return l_iHeight;
      }

      typedef void (*VisitFunction)(GNode, void*);

      //! Traverse the whole tree by visiting parent before their children
      void PreorderTraversal(VisitFunction in_VisitingFunc, void* in_pFuncParam)
      {
         PreorderTraversalRecurse(Root(), in_VisitingFunc, in_pFuncParam);
      }

      //! Traverse the whole tree by visiting children before their parent
      void PostorderTraversal(VisitFunction in_VisitingFunc, void* in_pFuncParam);

      void PreorderTraversalRecurse(GNode in_CurrentNode, VisitFunction in_VisitingFunc, void* in_pFuncParam)
      {
         in_VisitingFunc(in_CurrentNode, in_pFuncParam);

         for(UINT32 i = 0;i < m_vNodes[in_CurrentNode.m_iId].m_iNbChildren;i ++)
         {
            PreorderTraversalRecurse(in_CurrentNode.Child(i), in_VisitingFunc, in_pFuncParam);
         }
      }

   private:
      class GElem
      {
      public:
         GElem()
         {
            m_iNbChildren = 0;
         }

         GElem(const GElem& in_Copy)
         {
            m_Data = in_Copy.m_Data;
            m_iNbChildren = in_Copy.m_iNbChildren;
         }

         T           m_Data;
         UINT32      m_iNbChildren;
      };

      UINT32            m_iNextSizeInc;
      vector<GElem>     m_vNodes;
   };

   template <typename T>
   UINT32 GQuadTree<T>::GNode::NbChildren() const
   {
      return m_pOwner->m_vNodes[m_iId].m_iNbChildren;
   }

   template <typename T>
   void GQuadTree<T>::GNode::AddQuadChildren() const
   {
      m_pOwner->m_vNodes[m_iId].m_iNbChildren = 4;

      UINT32 l_iChildID = (m_iId << 2) + 1;
      if(m_pOwner->m_vNodes.size() <= l_iChildID)
      {
         m_pOwner->m_vNodes.resize(m_pOwner->m_vNodes.size() + m_pOwner->m_iNextSizeInc);
         m_pOwner->m_iNextSizeInc *= 4;
      }
   }

   template <typename T>
   void GQuadTree<T>::GNode::RemoveQuadChildren() const
   {
      m_pOwner->m_vNodes[m_iId].m_iNbChildren = 0;

      //! \todo TBA : Reduce array size if needed
   }

   template <typename T>
   typename GQuadTree<T>::GNode GQuadTree<T>::GNode::Child(UINT32 in_iChildID) const
   {
      assert(in_iChildID < m_pOwner->m_vNodes[m_iId].m_iNbChildren);

      return GNode( (m_iId << 2) + in_iChildID + 1, m_pOwner);
   }

   template <typename T>
   T& GQuadTree<T>::GNode::Data()
   {
      return m_pOwner->m_vNodes[m_iId].m_Data;
   }

   template <typename T>
   UINT32 GQuadTree<T>::GNode::Depth() const
   {
      UINT32 l_iDepth = 0;
      UINT32 l_iID = m_iId;
      while(l_iID > 0)
      {
         l_iDepth ++;
         l_iID = (l_iID - 1) >> 2;
      }

      return l_iDepth;
   }
}

#endif
