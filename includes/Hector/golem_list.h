/**************************************************************
*
* golem_list.h
*
* Description
* ===========
*  Describes the interface and implementation of class GList,
*  a doubly linked list implemented with a sequence (array)
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _GOLEM_LIST_H_
#define _GOLEM_LIST_H_

namespace Hector
{

   /*!
   * Doubly linked list implemented with a sequence (array)
   **/
   template <typename T>
   class GList
   {
      //! Describes a node of the list
   protected:
      struct GNode
      {
         T        m_Value;
         GNode*   m_pPrev;
         GNode*   m_pNext;
         GList*   m_pCont;
      };

   public:
      //! Constant iterator of the list
      /*!
      * Represents a constant element of the list on which it is possible
      * to retrieve the data and Iterate forward & backward in the list.
      */
      class GIteratorConst
      {
         friend class GList;

      protected:
         const GNode*   m_pNode;

         GIteratorConst(const GNode* in_pNode)
         {
            m_pNode = in_pNode;
         }

         const GNode& operator *()
         {
            return *m_pNode;
         }

         const GNode* operator ->()
         {
            return (&**this);
         }

      public:
         /*!
         * \brief Construct an empty (non-valid) iterator
         */
         GIteratorConst()
         {
            m_pNode = NULL;
         }

         ///*!
         // * \brief Construct a constant iterator using an iterator
         // * \param in_Copy    Iterator to copy
         // */
         //GIteratorConst(const GIterator& in_Copy)
         //{
         //   *this = in_Copy;
         //}

         ///*!
         // * \brief Copy the of an another iterator
         // * \return A reference to this iterator
         // */
         //GIteratorConst& operator = (const GIterator& in_pCopy)
         //{
         //   m_pNode = in_pCopy.m_pNode;

         //   return *this;
         //}

         /*!
         * \brief Get the node constant value
         * \return A constant reference to the value pointed by this iterator
         */
         const T& Value() const
         {
            return m_pNode->m_Value;
         }

         /*!
         * \brief Get the node constant value
         * \return A constant reference to the value pointed by this iterator
         */
         const T& operator () () const
         {
            return m_pNode->m_Value;
         }

         /*!
         * \brief Get an iterator pointing to the next node in the list
         * \return An iterator
         */
         GIteratorConst Next() const
         {
            return GIteratorConst(m_pNode->m_pNext);
         }

         /*!
         * \brief Get an iterator pointing to the previous node in the list
         * \return An iterator
         */
         GIteratorConst Prev() const
         {
            return GIteratorConst(m_pNode->m_pPrev);
         }

         /*!
         * \brief Set this iterator to point to the next node in the list
         * \return A reference to this iterator
         */
         GIteratorConst& operator ++ ()
         {
            m_pNode = m_pNode->m_pNext;
            return *this;
         }

         /*!
         * \brief Set this iterator to point to the previous node in the list
         * \return A reference to this iterator
         */
         GIteratorConst& operator -- ()
         {
            m_pNode = m_pNode->m_pPrev;
            return *this;
         }

         /*!
         * \brief Compare if this iterator is equal to another iterator
         * \param in_Other      Iterator to compare with
         * \return A boolean indicating if both iterator are equal
         */
         bool operator == (GIteratorConst in_Other) const
         {
            return in_Other.m_pNode == m_pNode;
         }

         /*!
         * \brief Compare if this iterator is not equal to another iterator
         * \param in_Other      Iterator to compare with
         * \return A boolean indicating if both iterator are not equal
         */
         bool operator != (GIteratorConst in_Other) const
         {
            return in_Other.m_pNode != m_pNode;
         }

         /*!
         * \brief Test if this iterator is valid
         * \return A boolean indicating if this iterator is valid
         */
         bool IsValid() const
         {
            if(m_pNode)
            {
               return m_pNode->m_pCont != NULL;
            }

            return NULL;
         }

         /*!
         * \brief Get a pointer to the list that this iterator points to
         * \return An list
         */
         GList<T>* Container() const
         {
            return m_pNode->m_pCont;
         }
      };

      //! Iterator of the list
      /*!
      * Represents an element of the list on which it is possible
      * to retrieve the data and Iterate forward & backward in the list.
      */
      class GIterator : public GIteratorConst
      {
         friend class GList;

      protected:
         GIterator(GNode* in_pNode)
            : GIteratorConst(in_pNode)
         {
         }

         GNode& operator *()
         {
            return (GNode&)(__super::operator *());
         }

         GNode* operator ->()
         {
            return (&**this);
         }

      public:
         /*!
         * \brief Construct an empty (non-valid) iterator
         */
         GIterator()
         {
         }

         /*!
         * \brief Get the node value
         * \return A reference to the value pointed by this iterator
         */
         T& Value()
         {
            return (T&) GIteratorConst::Value();
         }

         /*!
         * \brief Get the node value
         * \return A reference to the value pointed by this iterator
         */
         T& operator () ()
         {
            return Value();
         }

         /*!
         * \brief Get an iterator pointing to the next node in the list
         * \return An iterator
         */
         GIterator Next() const
         {
            return GIterator(m_pNode->m_pNext);
         }

         /*!
         * \brief Get an iterator pointing to the previous node in the list
         * \return An iterator
         */
         GIterator Prev() const
         {
            return GIterator(m_pNode->m_pPrev);
         }

         /*!
         * \brief Set this iterator to point to the next node in the list
         * \return A reference to this iterator
         */
         GIterator& operator ++ ()
         {
            m_pNode = m_pNode->m_pNext;
            return *this;
         }

         /*!
         * \brief Set this iterator to point to the previous node in the list
         * \return A reference to this iterator
         */
         GIterator& operator -- ()
         {
            m_pNode = m_pNode->m_pPrev;
            return *this;
         }
      };

      /*!
      * \brief Construct an empty list
      * \param in_iDefaultArraySize   Indicate the minimum size of an array.
      */
      GList(UINT32 in_iDefaultArraySize = 16)
      {
         m_pRemovedNodes = NULL;
         m_iUsedNodes    = 0;
         Init(in_iDefaultArraySize);
      }

      /*!
      * \brief Construct a list by copying the content of another list
      * \param in_ListCopy   List to copy from
      */
      GList(const GList<T>& in_ListCopy)
      {
         m_iUsedNodes = in_ListCopy.m_iUsedNodes;
         // Construct with default values
         Init(in_ListCopy.m_iDefaultArraySize);

         // Resize our data structures
         UINT32 l_iTotalSize = in_ListCopy.Size();
         if(l_iTotalSize > 0)
         {
            // Allocate space for the list elements
            GNode_Array* l_NewArray = Allocate_Node_Array(l_iTotalSize);

            // Copy & link each element
            GNode* l_It = in_ListCopy.m_Head.m_pNext;
            for(UINT32 i = 0;i < l_iTotalSize;i ++)
            {
               // Copy value
               l_NewArray->m_pNodes[i].m_Value = l_It->m_Value;

               // Link elements
               l_NewArray->m_pNodes[i].m_pPrev = &(l_NewArray->m_pNodes[i - 1]);
               l_NewArray->m_pNodes[i].m_pNext = &(l_NewArray->m_pNodes[i + 1]);
               l_NewArray->m_pNodes[i].m_pCont = this;

               // Iterate through the original list
               l_It = l_It->m_pNext;
            }

            // Insert this list between head and tail
            Insert_Node_Array(&m_Head, l_NewArray->m_pNodes, l_iTotalSize);
         }
      }

      /*!
      * \brief Construct a list by using an array
      * \param in_pArray        Array of values
      * \param in_iArraySize    Number of elements in the array
      */
      GList(const T* in_pArray, UINT32 in_iArraySize)
      {
         m_iUsedNodes = 0;
         // Construct with default values
         Init(in_iArraySize);

         if(in_iArraySize > 0)
         {
            // Allocate space for the array
            GNode_Array* l_NewArray = Allocate_Node_Array(in_iArraySize);

            // Copy & link each element
            for(UINT32 i = 0;i < in_iArraySize;i ++)
            {
               // Copy value
               l_NewArray->m_pNodes[i].m_Value = in_pArray[i];
               l_NewArray->m_pNodes[i].m_pPrev = &(l_NewArray->m_pNodes[i - 1]);
               l_NewArray->m_pNodes[i].m_pNext = &(l_NewArray->m_pNodes[i + 1]);

               // Link elements
               l_NewArray->m_pNodes[i].m_pCont = this;
            }

            // Insert this list between head and tail
            Insert_Node_Array(&m_Head, l_NewArray->m_pNodes, in_iArraySize);
         }
      }

      /*!
      * \brief Destructor of the list
      */
      ~GList()
      {
         // Delete each node array
         for(UINT32 i = 0;i < m_iNbArrays;i ++)
         {
            SAFE_DELETE_ARRAY(m_pNodeArrays[i].m_pNodes);
         }

         // Delete the node array container
         SAFE_DELETE_ARRAY(m_pNodeArrays);
      }

      /*!
      * \brief Assign this list to the content of another list
      * \param in_ListCopy   List to copy from
      * \return A renference to this list
      */
      GList& operator = (const GList& in_ListCopy)
      {
         // Construct with default values
         Init(in_ListCopy.m_iDefaultArraySize);

         // Resize our data structures
         UINT32 l_iTotalSize = in_ListCopy.Size();
         if(l_iTotalSize > 0)
         {
            // Allocate space for the list elements
            GNode_Array* l_NewArray = Allocate_Node_Array(l_iTotalSize);

            // Copy & link each element
            GNode* l_It = in_ListCopy.m_Head.m_pNext;
            for(UINT32 i = 0;i < l_iTotalSize;i ++)
            {
               // Copy value
               l_NewArray->m_pNodes[i].m_Value = l_It->m_Value;

               // Link elements
               l_NewArray->m_pNodes[i].m_pPrev = &(l_NewArray->m_pNodes[i - 1]);
               l_NewArray->m_pNodes[i].m_pNext = &(l_NewArray->m_pNodes[i + 1]);
               l_NewArray->m_pNodes[i].m_pCont = this;

               // Iterate through the original list
               l_It = l_It->m_pNext;
            }

            // Insert this list between head and tail
            Insert_Node_Array(&m_Head, l_NewArray->m_pNodes, l_iTotalSize);
         }

         return *this;
      }

      /*!
      * \brief Get the number of elements in this list
      * \return A integer containing the number of element in this list
      */
      UINT32 Size() const
      {
         return m_iUsedNodes;
      }

      /*!
      * \brief Set the number of elements in this list
      * \param in_iCount     New number of elements
      * \return A boolean indicating if operation was successful
      */
      bool Size(UINT32 in_iCount)
      {
         if(in_iCount > 0)
         {
            // Maintain the count of elements to add
            UINT32 l_CountDelta = in_iCount;

            // Iterate trough the list until the end is reached OR required count is atteined
            GNode* l_It = m_Head.m_pNext;
            while( (l_It != &m_Tail) && (l_CountDelta > 0) )
            {
               l_CountDelta --;
               l_It = l_It->m_pNext;
            }

            // If count delta has reached 0, it means there are too many elements in the list
            if(l_CountDelta == 0)
            {
               // Swap tail with current node
               m_Tail.m_pPrev = l_It->m_pPrev;
               l_It->m_pPrev->m_pNext = &m_Tail;

               // Remove extra elements
               while(l_It != &m_Tail)
               {
                  l_It->m_pPrev = NULL;
                  l_It->m_pNext = NULL;
                  l_It->m_pCont = NULL;
                  l_It = l_It->m_pNext;
               }
            }
            else
            {
               // Not enough elements in the list, we need to add some
               if( (m_iNbArrays > 0) && (l_CountDelta > 0) )
               {
                  // Try to add these elements to extra space of last allocated node array
                  UINT32 l_LastArray = m_iNbArrays - 1;
                  if(m_iLastIndex < m_pNodeArrays[l_LastArray].m_iNbNodes)
                  {
                     // Insert up to remaining elements in the array OR required list element count
                     for(UINT32 j = m_iLastIndex;(j < m_pNodeArrays[l_LastArray].m_iNbNodes) && (l_CountDelta > 0);j ++)
                     {
                        // Link unused element to current list
                        Link(m_pNodeArrays[l_LastArray].m_pNodes[j], *(m_Tail.m_pPrev) );

                        l_CountDelta --;
                        m_iLastIndex ++;
                     }
                  }
               }

               if(l_CountDelta > 0)
               {
                  // There is not enough elements in the list, allocate more place
                  GNode_Array* l_NewNodeArray = Allocate_Node_Array(l_CountDelta);

                  // Link the new elements togheter
                  for(UINT32 i = 0;i < l_CountDelta;i ++)
                  {
                     l_NewNodeArray->m_pNodes[i].m_pPrev = &(l_NewNodeArray->m_pNodes[i - 1]);
                     l_NewNodeArray->m_pNodes[i].m_pNext = &(l_NewNodeArray->m_pNodes[i + 1]);
                     l_NewNodeArray->m_pNodes[i].m_pCont = this;
                  }

                  // Link previous list with new elements
                  Insert_Node_Array(m_Tail.m_pPrev, l_NewNodeArray->m_pNodes, l_CountDelta);
               }
            }
         }
         else
         {
            // Unlink elements
            GNode* l_It = m_Head.m_pNext;
            while(l_It != &m_Tail)
            {
               GNode* l_Cur = l_It;
               l_It = l_It->m_pNext;

               l_Cur->m_pPrev = NULL;
               l_Cur->m_pNext = NULL;
               l_Cur->m_pCont = NULL;
            }

            // Empty list by making head pointing to tail
            m_Head.m_pNext = &m_Tail;
            m_Tail.m_pPrev = &m_Head;
            m_iLastIndex = 0;
         }


         m_iUsedNodes  = in_iCount;
         return true;
      }

      /*!
      * \brief Get the number of elements in a sub-array (excluding last element)
      * \param in_Start      Iterator to the first element to include in the count
      * \param in_End        Iterator to the last element not included in the count
      * \return An integer containing the size of this sub-array
      * \remarks If the in_End interator is located be fore the in_Start iterator, 
      *          the count will stop at the end of the list.
      */
      UINT32 Size(GIterator in_Start, GIterator in_End) const
      {
         // Make sure both nodes belong to our list
         if( (in_Start.m_pNode->m_pCont != this) || 
            (in_End.m_pNode->m_pCont != this) )
         {
            return 0;
         }

         // Count elements by visiting each node
         UINT32 l_iCount = 0;
         GIteratorConst l_It = in_Start;
         while( (l_It != End() ) || (l_It != in_End) )
         {
            ++ l_It;
            ++ l_iCount;
         }

         return l_iCount;
      }

      /*!
      * \brief Reallocate internal structures so it uses the less memory it can
      * \return A boolean indicating if operation was sucessful
      */
      bool Pack()
      {
         // Count the number of elements
         UINT32 l_iTotalSize = Size();

         // Copy current information
         GNode_Array* l_pNodeArrays = m_pNodeArrays;
         UINT32 l_iNbArrays = m_iNbArrays;
         UINT32 l_iDefaultSize = m_iDefaultArraySize;
         GNode* l_ItOld = m_Head.m_pNext;

         // Initialize data as if we were constructing
         Init(l_iTotalSize);

         // Allocate space
         Size(l_iTotalSize);

         // Copy elements
         GNode* l_ItNew = m_Head.m_pNext;
         for(UINT32 i = 0;i < l_iTotalSize;i ++)
         {
            l_ItNew->m_Value = l_ItOld->m_Value;
            l_ItNew = l_ItNew->m_pNext;
            l_ItOld = l_ItOld->m_pNext;
         }

         // Delete previous information
         m_iDefaultArraySize = l_iDefaultSize;

         // Delete each node array
         for(UINT32 i = 0;i < l_iNbArrays;i ++)
         {
            SAFE_DELETE_ARRAY(l_pNodeArrays[i].m_pNodes);
         }

         // Delete the node array container
         SAFE_DELETE_ARRAY(l_pNodeArrays);


         m_pRemovedNodes = NULL;

         return true;
      }

      /*!
      * @brief Count the number of nodes allocated in the list
      *        by processing the node array
      * @return UINT32: Number of allocated nodes
      **/
      UINT32 NodesAllocated()
      {
         UINT32 l_iCount = 0;
         for(UINT32 i = 0 ; i < m_iNbArrays ; i++)
         {
            GNode_Array* l_pArray = &m_pNodeArrays[i];
            l_iCount += l_pArray->m_iNbNodes;
         }
         return l_iCount;
      }

      /*!
      * \brief Add an element at the end of the list
      * \param in_Value      Value of the new element
      * \return An iterator to the new list element
      */
      GIterator PushBack(T in_Value)
      {
         m_iUsedNodes++;

         //Check if a previously removed node could be reused
         if(m_pRemovedNodes)
         {
            GNode* l_pNode = m_pRemovedNodes;
            m_pRemovedNodes = l_pNode->m_pNext;
            if(m_pRemovedNodes)
               m_pRemovedNodes->m_pPrev = NULL;
            Link(*l_pNode,*(m_Tail.m_pPrev));
            l_pNode->m_pCont = this;
            l_pNode->m_Value = in_Value;
            return GIterator(m_Tail.m_pPrev);
         }
         // Try to add one element to extra space of last allocated node array
         else if(m_iNbArrays > 0)
         {
            UINT32 l_LastArray = m_iNbArrays - 1;
            if(m_iLastIndex < m_pNodeArrays[l_LastArray].m_iNbNodes)
            {
               // Link unused element to current list
               Link(m_pNodeArrays[l_LastArray].m_pNodes[m_iLastIndex], *(m_Tail.m_pPrev) );
               m_iLastIndex ++;

               m_Tail.m_pPrev->m_Value = in_Value;
               m_Tail.m_pPrev->m_pCont = this;
               return GIterator(m_Tail.m_pPrev);
            }
         }

         // There is not enough elements in the list, allocate more place
         GNode_Array* l_NewNodeArray = Allocate_Node_Array(1);

         // Link the first element
         m_iLastIndex = 1;
         Link(l_NewNodeArray->m_pNodes[0], *(m_Tail.m_pPrev) );

         m_Tail.m_pPrev->m_Value = in_Value;
         m_Tail.m_pPrev->m_pCont = this;
         return GIterator(m_Tail.m_pPrev);
      }

      /*!
      * Finds the given item in the list
      * @param Item to find
      * @return An iterator to the list element
      **/
      GIterator Find(const T& in_Item)
      {
         Hector::GList<T>::GNode* l_pNode = NULL;

         l_pNode = m_Head.m_pNext;
         while(l_pNode != &m_Tail)
         {
            if(l_pNode->m_Value == in_Item)
            {
               return GIterator(l_pNode);               
            }
            l_pNode = l_pNode->m_pNext;
         }
         return End();         
      }

      /*!
      * \brief Add an element at the beginning of the list
      * \param in_Value      Value of the new element
      * \return An iterator to the new list element
      */
      GIterator PushFront(T in_Value)
      {
         m_iUsedNodes++;

         //Check if a previously removed node could be reused
         if(m_pRemovedNodes)
         {
            GNode* l_pNode = m_pRemovedNodes;
            m_pRemovedNodes = l_pNode->m_pNext;
            if(m_pRemovedNodes)
               m_pRemovedNodes->m_pPrev = NULL;

            Link(*l_pNode,m_Head);
            m_Head.m_pNext->m_pCont = this;
            m_Head.m_pNext->m_Value = in_Value;
            return GIterator(m_Head.m_pNext);
         }
         // Try to add one element to extra space of last allocated node array
         else if(m_iNbArrays > 0)
         {
            UINT32 l_LastArray = m_iNbArrays - 1;
            if(m_iLastIndex < m_pNodeArrays[l_LastArray].m_iNbNodes)
            {
               // Link unused element to current list
               Link(m_pNodeArrays[l_LastArray].m_pNodes[m_iLastIndex], m_Head);
               m_iLastIndex ++;
               m_Head.m_pNext->m_pCont = this;
               m_Head.m_pNext->m_Value = in_Value;
               return GIterator(m_Head.m_pNext);
            }
         }

         // There is not enough elements in the list, allocate more place
         GNode_Array* l_NewNodeArray = Allocate_Node_Array(1);

         // Link the first element
         m_iLastIndex = 1;
         Link(l_NewNodeArray->m_pNodes[0], m_Head );

         m_Head.m_pNext->m_Value = in_Value;
         m_Head.m_pNext->m_pCont = this;
         return GIterator(m_Head.m_pNext);
      }

      /*!
      * \brief Insert another list anywhere in this list
      * \param in_List       List to insert
      * \param in_Where      Iterator that indicate where the list must be inserted
      * \return  An iterator to the first element of the inserted list or an iterator
      *          to the last element of the list if the insertion failed
      */
      GIterator Insert(const GList<T>& in_List, GIterator in_Where)
      {
         // Make sure node belongs to this list & it is not the head
         if( ( (*in_Where).m_pCont != this) || 
            (in_Where == Head() ) )
         {
            return End();
         }

         m_iUsedNodes               += in_List.m_iUsedNodes;
         GIteratorConst l_Current   =  in_List.Begin();
         const GNode* l_pReturnPrev =  (*in_Where).m_pPrev;


         //Try to insert using previously removed nodes
         {
            while( m_pRemovedNodes  && (l_Current != in_List.End() ) )
            {
               //Check if a previously removed node could be reused
               GNode* l_pNode = m_pRemovedNodes;
               m_pRemovedNodes = l_pNode->m_pNext;
               if(m_pRemovedNodes)
                  m_pRemovedNodes->m_pPrev = NULL;
               Link(*l_pNode,*(in_Where.Prev()));
               l_pNode->m_pNext->m_pCont = this;
               l_pNode->m_pNext->m_Value = l_Current.Value();
               ++ l_Current;
            }
         }


         // Try to add one element to extra space of last allocated node array
         if(m_iNbArrays > 0)
         {
            UINT32 l_LastArray = m_iNbArrays - 1;
            while( (m_iLastIndex < m_pNodeArrays[l_LastArray].m_iNbNodes) && (l_Current != in_List.End() ) )
            {
               // Link unused element to current list
               Link(m_pNodeArrays[l_LastArray].m_pNodes[m_iLastIndex], *(in_Where.Prev() ) );
               m_pNodeArrays[l_LastArray].m_pNodes[m_iLastIndex].m_pCont = this;
               m_pNodeArrays[l_LastArray].m_pNodes[m_iLastIndex].m_Value = l_Current.Value();
               m_iLastIndex ++;

               ++ l_Current;
            }
         }

         if(l_Current != in_List.End() )
         {
            // There is not enough elements in the list, allocate more place
            UINT32 l_iRemainCount = 0;
            GIteratorConst l_CurrentRemain = l_Current;
            while(l_CurrentRemain != in_List.End() )
            {
               ++ l_iRemainCount;
               ++ l_CurrentRemain;
            }

            GNode_Array* l_NewNodeArray = Allocate_Node_Array(l_iRemainCount);

            while(l_Current != in_List.End() )
            {
               // Link unused element to current list
               Link(l_NewNodeArray->m_pNodes[m_iLastIndex], *in_Where.Prev() );
               l_NewNodeArray->m_pNodes[m_iLastIndex].m_pCont = this;
               l_NewNodeArray->m_pNodes[m_iLastIndex].m_Value = l_Current.Value();
               m_iLastIndex ++;

               ++ l_Current;
            }
         }
         return l_pReturnPrev->m_pNext;
      }

      /*!
      * \brief Insert an element anywhere in the list
      * \param in_Value      Value of element to insert
      * \param in_iIndex     Index of the element where to insert
      * \return  An iterator to the element inserted or an iterator
      *          to the last element of the list if the insertion failed
      */
      GIterator Insert(T in_Value, UINT32 in_iIndex)
      {
         // Search the position to insert to
         UINT32 l_iCount = 0;
         GIterator l_It = Begin();
         while( (l_It != End() ) && (l_iCount < in_iIndex) )
         {
            ++ l_It;
            ++ l_iCount;
         }

         // Only remove if not at the end (index is valid)
         if(l_It != End() )
         {
            return Insert(in_Value, l_It);
         }

         return End();
      }

      /*!
      * \brief Insert an element anywhere in the list
      * \param in_List       Value of element to insert
      * \param in_Where      Iterator that indicate where the element must be inserted
      * \return  An iterator to the element inserted or an iterator
      *          to the last element of the list if the insertion failed
      */
      GIterator Insert(T in_Value, GIterator in_Where)
      {
         // Make sure node belongs to this list & it is not the head
         if( (in_Where.Container() != this) || 
            (in_Where == Head() ) )
         {
            return End();
         }

         m_iUsedNodes++;

         //Check if a previously removed node could be reused
         if(m_pRemovedNodes)
         {
            GNode* l_pNode = m_pRemovedNodes;
            m_pRemovedNodes = l_pNode->m_pNext;
            if(m_pRemovedNodes)
               m_pRemovedNodes->m_pPrev = NULL;

            Link(*l_pNode,*(in_Where.Prev()));
            l_pNode->m_pCont = this;
            l_pNode->m_Value = in_Value;
            return GIterator(l_pNode);
         }
         // Try to add one element to extra space of last allocated node array
         else if(m_iNbArrays > 0)
         {
            UINT32 l_LastArray = m_iNbArrays - 1;
            if(m_iLastIndex < m_pNodeArrays[l_LastArray].m_iNbNodes)
            {
               // Link unused element to current list
               Link(m_pNodeArrays[l_LastArray].m_pNodes[m_iLastIndex], *in_Where.Prev() );
               m_pNodeArrays[l_LastArray].m_pNodes[m_iLastIndex].m_pCont = this;
               m_pNodeArrays[l_LastArray].m_pNodes[m_iLastIndex].m_Value = in_Value;
               m_iLastIndex ++;
               return &(m_pNodeArrays[l_LastArray].m_pNodes[m_iLastIndex - 1] );
            }
         }

         // There is not enough elements in the list, allocate more place
         GNode_Array* l_NewNodeArray = Allocate_Node_Array(1);

         // Link the first element
         m_iLastIndex = 1;
         Link(l_NewNodeArray->m_pNodes[0], *in_Where.Prev() );

         l_NewNodeArray->m_pNodes[0].m_Value = in_Value;
         l_NewNodeArray->m_pNodes[0].m_pCont = this;
         return l_NewNodeArray->m_pNodes;
      }

      /*!
      * \brief Remove an element from anywhere in the list
      * \param in_iIndex     Index of the element to remove
      * \return  An iterator to the element that replaced the removed element or an iterator
      *          to the last element of the list if the insertion failed
      */
      GIterator Remove(UINT32 in_iIndex)
      {
         // Search the element to remove
         UINT32 l_iCount = 0;
         GIterator l_It = Begin();
         while( (l_It != End() ) && (l_iCount < in_iIndex) )
         {
            ++ l_It;
            ++ l_iCount;
         }

         // Only remove if not at the end (index is valid)
         if(l_It != End() )
         {
            GIterator l_Return = l_It.Next();

            // Remove element by linking prev and next elements
            Unlink(*l_It);

            // Make sure there wont be any mistake by unlinking removed element
            l_It->m_pPrev = NULL;
            l_It->m_pNext = NULL;

            m_iUsedNodes--;

            //Add the node to the list of m_pRemovedNodes, so it can be reused at next insertion
            l_It->m_pNext = m_pRemovedNodes;
            m_pRemovedNodes = &(*l_It);

            // Return the element that replaced the removed element
            return l_Return;
         }

         // Nothing removed, return the last element
         return End();
      }

      /*!
      * \brief Remove an element from anywhere in the list
      * \param in_Where      Iterator that points to the element to remove
      * \return  An iterator to the element that replaced the removed element or an iterator
      *          to the last element of the list if the insertion failed
      */
      GIterator Remove(GIterator in_Where)
      {
         // Make sure node belongs to this list & it is not the head or tail
         if( (in_Where->m_pCont != this) || 
            (in_Where == Head() ) || 
            (in_Where == End() ) )
         {
            return End();
         }

         GIterator l_Return = in_Where.Next();

         // Remove element by linking prev and next elements
         Unlink(*in_Where);

         // Make sure there wont be any mistake by unlinking removed element
         in_Where->m_pPrev = NULL;
         in_Where->m_pNext = NULL;
         in_Where->m_pCont = NULL;
         m_iUsedNodes--;


         //Add the node to the list of m_pRemovedNodes, so it can be reused at next insertion
         in_Where->m_pNext = m_pRemovedNodes;
         m_pRemovedNodes = &(*in_Where);


         // Return the element that replaced the removed element
         return l_Return;
      }

      /*!
      * \brief Get the first element of the list
      * \return  A constant iterator to the first element of the list or a constant iterator
      *          to the last element of the list if the list is empty
      */
      GIteratorConst Begin() const
      {
         return GIteratorConst(m_Head.m_pNext);
      }

      /*!
      * \brief Get the first element of the list
      * \return  An iterator to the first element of the list or an iterator
      *          to the last element of the list if the list is empty
      */
      GIterator Begin()
      {
         return GIterator(m_Head.m_pNext);
      }

      /*!
      * \brief Get the last element of the list
      * \return  A constant iterator to the last element of the list
      */
      GIteratorConst End() const
      {
         return GIteratorConst(&m_Tail);
      }

      /*!
      * \brief Get the last element of the list
      * \return  An iterator to the last element of the list
      */
      GIterator End()
      {
         return GIterator(&m_Tail);
      }

      /*!
      * \brief Get the head of the list (element before the begining)
      * \return  A constant iterator to the head of the list
      */
      GIteratorConst Head() const
      {
         return GIteratorConst(&m_Head);
      }

      /*!
      * \brief Get the head of the list (element before the begining)
      * \return  An iterator to the head of the list
      */
      GIterator Head()
      {
         return GIterator(&m_Head);
      }

      /*!
      * \brief Get a constant element from the list
      * \param in_iIndex        Index of the element to retrieve
      * \return  A constant iterator to the element in the list or a constant iterator
      *          the the last element if the index is invalid
      */
      GIteratorConst operator [] (UINT32 in_iIndex) const
      {
         // Search the element to return
         UINT32 l_iCount = 0;
         GIteratorConst l_It = Begin();
         while( (l_It != End() ) && (l_iCount < in_iIndex) )
         {
            ++ l_It;
            ++ l_iCount;
         }

         return l_It;
      }

      /*!
      * \brief Get an element from the list
      * \param in_iIndex        Index of the element to retrieve
      * \return  An iterator to the element in the list or an iterator
      *          the the last element if the index is invalid
      */
      GIterator operator [] (UINT32 in_iIndex)
      {
         // Search the element to return
         UINT32 l_iCount = 0;
         GIterator l_It = Begin();
         while( (l_It != End() ) && (l_iCount < in_iIndex) )
         {
            ++ l_It;
            ++ l_iCount;
         }

         return l_It;
      }

      /*!
      * \brief Move an element before another element in the list
      * \param in_Elem       Iterator that points to the element to move
      * \param in_Where      Iterator that points after the desired destination
      * \return  A boolean indicating if the call was sucessful
      */
      bool MoveBefore(GIterator in_Elem, GIterator in_Where)
      {
         // Make sure both nodes come from this list, both nodes are different, neither nodes is the head or the tail
         if( (in_Elem->m_pCont != this) || 
            (in_Where->m_pCont != this) ||
            (in_Elem == Head() ) || 
            (in_Where == Head() ) ||
            (in_Elem == End() ) || 
            (in_Elem == in_Where) )
         {
            return false;
         }

         // Unlink element to move
         Unlink(*in_Elem);

         // Link element after the required node
         Link(*in_Elem, *in_Where.Prev() );

         return true;
      }

      /*!
      * \brief Move an element after another element in the list
      * \param in_Elem       Iterator that points to the element to move
      * \param in_Where      Iterator that points before the desired destination
      * \return  A boolean indicating if the call was sucessful
      */
      bool MoveAfter(GIterator in_Elem, GIterator in_Where)
      {
         // Make sure both nodes come from this list, both nodes are different, neither nodes is the head or the tail
         if( (in_Elem->m_pCont != this) || 
            (in_Where->m_pCont != this) ||
            (in_Elem == Head() ) || 
            (in_Elem == End() ) || 
            (in_Where == End() ) ||
            (in_Elem == in_Where) )
         {
            return false;
         }

         // Unlink element to move
         Unlink(*in_Elem);

         // Link element after the required node
         Link(*in_Elem, *in_Where);

         return true;
      }

      /*!
      * \brief Move a sub-array of elements after another element in the list
      * \param in_Start      Iterator that points to the start of the sub-array
      * \param in_iCount     Number of elements in the subarray
      * \param in_Where      Iterator that points before the desired destination
      * \return  An iterator to the last element of the sub array or an iterator to the
      *          last element of the list if the call failed
      */
      GIterator MoveAfter(GIterator in_Start, UINT32 in_iCount, GIterator in_Where)
      {
         // Make sure both nodes come from this list, both nodes are different, neither nodes is the head or the tail
         if( (in_iCount == 0) ||
            (in_Start->m_pCont != this) || 
            (in_Where->m_pCont != this) ||
            (in_Start == Head() ) || 
            (in_Start == End() ) || 
            (in_Where == End() ) )
         {
            return End();
         }

         // Fetch the last element of the sub-array, while verifying none of them is the
         // insertion point
         GNode* l_Last = &*in_Start;
         UINT32 l_iCurrent = 0;
         bool l_bInsertionInArray = false;
         while(l_iCurrent != in_iCount)
         {
            if(l_Last == in_Where.m_pNode)
            {
               l_bInsertionInArray = true;
            }
            l_Last = l_Last->m_pNext;
            l_iCurrent ++;
         }

         if( (in_Where == in_Start.Prev() ) || l_bInsertionInArray)
         {
            return l_Last;
         }

         // Unlink the sub-array to move
         GNode* l_pNext = l_Last;
         GNode* l_pPrev = in_Start->m_pPrev;
         l_Last = l_Last->m_pPrev;

         l_pPrev->m_pNext = l_pNext;
         l_pNext->m_pPrev = l_pPrev;

         // Link the sub-array after the required node
         l_pNext = in_Where->m_pNext;
         l_pPrev = &*in_Where;

         l_pPrev->m_pNext = &*in_Start;
         in_Start->m_pPrev = l_pPrev;

         l_pNext->m_pPrev = l_Last;
         l_Last->m_pNext = l_pNext;

         return l_pNext;
      }

      /*!
      * \brief Move a sub-array of elements after another element in the list
      * \param in_First      Iterator that points to the start of the sub-array
      * \param in_Last       Iterator that points to the end of the sub-array (included)
      * \param in_Where      Iterator that points before the desired destination
      * \return  An iterator to the last element of the sub array or an iterator to the
      *          last element of the list if the call failed
      */
      GIterator MoveAfter(GIterator in_First, GIterator in_Last, GIterator in_Where)
      {
         // Make sure all nodes come from this list, all nodes are different, neither nodes is the head or the tail
         if( (in_First->m_pCont != this) || 
            (in_Last->m_pCont != this) || 
            (in_Where->m_pCont != this) ||
            (in_First == Head() ) || 
            (in_First == End() ) || 
            (in_Last == Head() ) || 
            (in_Last == End() ) || 
            (in_Where == End() ) )
         {
            return End();
         }

         if(in_Where == in_First.Prev() )
         {
            return in_Last;
         }

         // Unlink the sub-array to move
         GNode* l_pNext = in_Last->m_pNext;
         GNode* l_pPrev = in_First->m_pPrev;

         l_pPrev->m_pNext = l_pNext;
         l_pNext->m_pPrev = l_pPrev;

         // Link the sub-array after the required node
         l_pNext = in_Where->m_pNext;
         l_pPrev = &*in_Where;

         l_pPrev->m_pNext = &*in_First;
         in_First->m_pPrev = l_pPrev;

         l_pNext->m_pPrev = &*in_Last;
         in_Last->m_pNext = l_pNext;

         return in_Last;
      }

      /*!
      * \brief Swap two elements in the list
      * \param in_ElemA      Iterator that points to the first element to swap
      * \param in_ElemB      Iterator that points to the second element to swap
      * \return  A boolean indicating if the operation was a success
      */
      bool Swap(GIterator in_ElemA, GIterator in_ElemB)
      {
         // Make sure both nodes come from this list, both nodes are different, neither nodes is the tail
         if( (in_ElemA->m_pCont != this) || 
            (in_ElemB->m_pCont != this) ||
            (in_ElemA == Head() ) || 
            (in_ElemB == Head() ) ||
            (in_ElemA == End() ) || 
            (in_ElemB == End() ) ||
            (in_ElemA == in_ElemB) )
         {
            return false;
         }

         if(in_ElemA.Next() == in_ElemB)
         {
            // A is just before B in the list
            GNode* l_pPrev = in_ElemA->m_pPrev;
            GNode* l_pNext = in_ElemB->m_pNext;

            // Link node B and node before A
            l_pPrev->m_pNext = &*in_ElemB;
            in_ElemB->m_pPrev = l_pPrev;

            // Link node B and A
            in_ElemB->m_pNext = &*in_ElemA;
            in_ElemA->m_pPrev = &*in_ElemB;

            // Link node A with node after B
            in_ElemA->m_pNext = l_pNext;
            l_pNext->m_pPrev = &*in_ElemA;
         }
         else if(in_ElemA.Prev() == in_ElemB)
         {
            // A is just after B in the list
            GNode* l_pPrev = in_ElemB->m_pPrev;
            GNode* l_pNext = in_ElemA->m_pNext;

            // Link node A and node before B
            l_pPrev->m_pNext = &*in_ElemA;
            in_ElemA->m_pPrev = l_pPrev;

            // Link node A and B
            in_ElemA->m_pNext = &*in_ElemB;
            in_ElemB->m_pPrev = &*in_ElemA;

            // Link node B with node after A
            in_ElemB->m_pNext = l_pNext;
            l_pNext->m_pPrev = &*in_ElemB;
         }
         else
         {
            // A and B are not neighbours
            GNode* l_pPrevA = in_ElemA->m_pPrev;
            GNode* l_pNextA = in_ElemA->m_pNext;

            GNode* l_pPrevB = in_ElemB->m_pPrev;
            GNode* l_pNextB = in_ElemB->m_pNext;

            // Re-link node A
            l_pPrevB->m_pNext = &*in_ElemA;
            in_ElemA->m_pPrev = l_pPrevB;

            l_pNextB->m_pPrev = &*in_ElemA;
            in_ElemA->m_pNext = l_pNextB;

            // Re-link node B
            l_pPrevA->m_pNext = &*in_ElemB;
            in_ElemB->m_pPrev = l_pPrevA;

            l_pNextA->m_pPrev = &*in_ElemB;
            in_ElemB->m_pNext = l_pNextA;
         }

         return true;
      }

   protected:
      void Unlink(GNode& in_Node)
      {
         // Make previous and next element of node pointing each other
         GNode* l_pNext = in_Node.m_pNext;
         GNode* l_pPrev = in_Node.m_pPrev;

         l_pPrev->m_pNext = l_pNext;
         l_pNext->m_pPrev = l_pPrev;
      }

      void Link(GNode& in_Node, GNode& in_Where)
      {
         // Insert node between insertion point and its next element
         GNode* l_pNext = in_Where.m_pNext;
         GNode* l_pPrev = &in_Where;

         l_pPrev->m_pNext =&in_Node;
         in_Node.m_pPrev = l_pPrev;

         l_pNext->m_pPrev = &in_Node;
         in_Node.m_pNext = l_pNext;
      }

      void Insert_Node_Array(GNode* in_pPrev, GNode* in_pArray, UINT32 in_NbNodes)
      {
         m_iUsedNodes += in_NbNodes;

         // Get next element
         GNode* l_pNext = in_pPrev->m_pNext;

         // Link prev element
         in_pPrev->m_pNext = in_pArray;
         in_pArray->m_pPrev = in_pPrev;

         // Link next element
         l_pNext->m_pPrev = &(in_pArray[in_NbNodes - 1] );
         in_pArray[in_NbNodes - 1].m_pNext = l_pNext;
      }

      struct GNode_Array
      {
         GNode*   m_pNodes;
         UINT32   m_iNbNodes;
      };

      //! Allocate space for nodes
      GNode_Array* Allocate_Node_Array(UINT32 in_iTotalSize)
      {
         // Allocate new array of node arrays
         GNode_Array* l_NewNodeArrays = new GNode_Array[m_iNbArrays + 1];
         if(m_pNodeArrays)
         {
            // Copy old array of nodes array content
            memcpy(l_NewNodeArrays, m_pNodeArrays, m_iNbArrays * sizeof(GNode_Array) );
         }
         // Deallocate previous array of node arrays
         SAFE_DELETE_ARRAY(m_pNodeArrays);
         m_pNodeArrays = l_NewNodeArrays;

         // Allocate and append new node array
         m_pNodeArrays[m_iNbArrays].m_iNbNodes = max(in_iTotalSize, m_iDefaultArraySize);
         m_pNodeArrays[m_iNbArrays].m_pNodes = new GNode[m_pNodeArrays[m_iNbArrays].m_iNbNodes];
         m_iLastIndex = in_iTotalSize;

         // Empty the new node array
         memset(m_pNodeArrays[m_iNbArrays].m_pNodes, 0, sizeof(GNode) * m_pNodeArrays[m_iNbArrays].m_iNbNodes);

         // Update the node array count
         m_iNbArrays ++;

         // Return the newly created node array
         return m_pNodeArrays + m_iNbArrays - 1;
      }

   private:
      void Init(UINT32 in_iDefaultArraySize)
      {
         // Initialize head & tail
         memset(&(m_Head.m_Value), 0xcc, sizeof(T) );
         m_Head.m_pPrev = NULL;
         m_Head.m_pNext = &m_Tail;
         m_Head.m_pCont = this;

         memset(&(m_Tail.m_Value), 0xcc, sizeof(T) );
         m_Tail.m_pNext = NULL;
         m_Tail.m_pPrev = &m_Head;
         m_Tail.m_pCont = this;

         // Empty node arrays
         m_pNodeArrays = NULL;
         m_iNbArrays = 0;
         m_iDefaultArraySize = in_iDefaultArraySize;
         m_iLastIndex = 0;

         m_pRemovedNodes = NULL;
      }

      GNode_Array*   m_pNodeArrays;
      UINT32         m_iNbArrays;
      UINT32         m_iDefaultArraySize;
      UINT32         m_iLastIndex;
      UINT32         m_iUsedNodes;


      GNode*         m_pRemovedNodes; //!<Nodes that can be reused

      GNode          m_Head;
      GNode          m_Tail;
   };

};
#endif //_GOLEM_LIST_H_
