/**************************************************************
*
* golem_binary_heap.h
*
* Description
* ===========
*  Describes the interface of class GBinaryHeap
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_BINARYHEAP_H_
#define _GOLEM_BINARYHEAP_H_

#pragma warning(disable: 4701)

/*!
* Class that represents a binary heap 
*    All the nodes are represented in a 1D array.
*
*          1
*        2 | 3
*       4|5|6|7
*
*   In the array, it would look like:
*      1 2 3 4 5 6 7 
*
*   Each node has 2 childs, the childs of a given node can be accessed by the simple
*   calculation at the given index in the array, 
*    -- Child 1 : Node Id (first node = 1) * 2 
*    -- Child 2 : (Node Id * 2) + 1
*   Now, all the items in the heap are "ordered", meaning that the childs of a node
*   are either equal or, depending on the sort method, greater or lower than the node value.
*   
*   Hence, depending on the sorting method, the 1st node is always the lowest or the 
*   highest.
*
*   The value held in the heap is the key.  The binary heap contains key and
*   to each key is associated a value.
*
**/
namespace Hector
{
   namespace EOrderMethod { enum Enum
   { 
      OrderIncreasing,
      OrderDecreasing
   };}   

   template <typename K,typename T> //Key for Key, T for Type
   class GBinaryHeap
   {
      INT32 SwapNodeUp(UINT32 in_iIndex);
      INT32 SwapNodeDown(UINT32 in_iIndex);

      bool SwapNodes(UINT32 in_iIndex1,UINT32 in_iIndex2);

   public:

      /*!
      * Represents a node in the heap
      **/
      class GHeapNode
      {
         friend class GBinaryHeap;
         K   m_Key;
         T   m_Value;
         INT32 m_iArrayPosition;
      public:
         K& Key(){return m_Key;}
         T& Value(){return m_Value;}

         //!Is equal operator, compare the keys and the values
         bool operator ==(const GHeapNode& in_Node) const
         {
            if( (m_Key   == in_Node.m_Key   ) &&
               (m_Value == in_Node.m_Value ) )
            {
               return true;
            }
            else
            {
               return false;
            }
         }

         //!Greater than operator, compare the keys
         bool operator >=(const GHeapNode& in_Node) const
         {
            if(m_Key >= in_Node.m_Key)
               return true;
            else
               return false;
         }

         //! Lower than operator, compare the keys
         bool operator <=(const GHeapNode& in_Node) const
         {
            if(m_Key <= in_Node.m_Key)
               return true;
            else
               return false;
         }
      };

      //! Constructor
      GBinaryHeap ( const UINT32& in_iSize,EOrderMethod::Enum in_OrderMethod);
      //! Destructor
      ~GBinaryHeap();


      //! Gets a pointer to the top node of the heap
      GHeapNode* TopNode();

      //!Gets the top node of the heap
      bool      TopNode(K& out_Key,T& out_Value);
      //!Adds a node to the heap
      GHeapNode* AddNode(const K& in_Key, const T& in_Value);
      //! Remove the top node from the heap 
      bool      RemoveTopNode();
      //!Pops the top node from the heap
      bool      PopTopNode(K& out_Key,T& out_Value);
      //! Update a node Key - Value
      bool      UpdateNode(GHeapNode* in_Node,const K& in_NewKey,const T& in_NewVal);
      bool      UpdateNode(UINT32 in_iIndex,const K& in_NewKey,const T& in_NewVal);
      //! Clear the elements of the heap
      bool      Clear();
      //! Gets the number of elements in the heap
      UINT32    Size();


   protected:
   private:


      GHeapNode*    m_AllocatedNodePool;
      UINT32        m_iNextNode;
      //! Return a pointer to the next pre allocated node
      GHeapNode*    NextPreAllocatedNode()
      {
         return &m_AllocatedNodePool[m_iNextNode++];
      }

      EOrderMethod::Enum m_OrderMethod;
      GHeapNode**   m_NodeArray;
      UINT32       m_iHeapSize;
      UINT32       m_iNbElements;
   };

   /*!
   * Constructor for the heap.
   *
   * @param in_iSize: The maximum number of elements to be held in the heap
   * @param in_OrderMethod: Specifies wether the objects must be ordered in increasing order or decreasing
   **/
   template<typename K,typename T>
      GBinaryHeap<K,T>::GBinaryHeap( const UINT32& in_iSize,EOrderMethod::Enum in_OrderMethod)
   {
      m_OrderMethod = in_OrderMethod;
      m_iHeapSize   = in_iSize;
      m_iNbElements = 0;

      m_AllocatedNodePool = new GBinaryHeap<K,T>::GHeapNode[in_iSize];
      m_iNextNode   = 0;

      //Create the array
      m_NodeArray   = new GBinaryHeap::GHeapNode*[m_iHeapSize+1];
   }

   /*!
   * Heap Destructor
   **/
   template<class K,class T>
      GBinaryHeap<K,T>::~GBinaryHeap()
   {
      SAFE_DELETE_ARRAY(m_NodeArray);
      SAFE_DELETE_ARRAY(m_AllocatedNodePool);
   }

   /*!
   * Get the top node Key and Value
   * Depending on what order type, it returns either the node with the Highest key value
   * or the node with the lowest key value
   *
   * @param out_Key: A Reference to the key to be updated
   * @param out_Value: A Reference to the value to be updated
   * @return true if successfully got the node information, false otherwise
   **/
   template<class K,class T>
      bool GBinaryHeap<K,T>::TopNode(K& out_Key,T& out_Value)
   {
      if(m_iHeapSize == 0 || m_iNbElements < 1)
      {
         return false;
      }
      else
      {
         out_Key   = m_NodeArray[1]->Key();
         out_Value = m_NodeArray[1]->Value();
      }
      return true;
   }



   /*!
    * Gets a pointer to the top node of the heap
    * returns GHeapNode*: NULL if no node, pointer to node otherwise
    **/
    template<class K,class T>
      typename GBinaryHeap<K,T>::GHeapNode* GBinaryHeap<K,T>::TopNode()
    {
      if(m_iHeapSize == 0 || m_iNbElements < 1)
      {
         return NULL;
      }
      else
         return m_NodeArray[1];
    }





   /*!
   * Adds a node to the heap
   *  The node is added and then moved to the correct place
   *  (depending on the sorting order)
   * @param in_Key: The key of the node to be added
   * @param in_Value: The value of the node to add
   * @return true if node successfully added, false otherwise
   **/
   template<class K,class T>
      typename GBinaryHeap<K,T>::GHeapNode* GBinaryHeap<K,T>::AddNode(const K& in_Key, const T& in_Value)
   {
      //Heap already full...
      if(m_iNbElements >= m_iHeapSize)
      {
         return false;
      }
      m_iNbElements++;

      //Insert the element at the end of the heap

      GBinaryHeap<K,T>::GHeapNode* l_pNode = NextPreAllocatedNode();
      l_pNode->m_Key                = in_Key;
      l_pNode->m_Value              = in_Value;
      m_NodeArray[m_iNbElements]   = l_pNode;

      //Swap until the object is at the correct place
      UINT32 l_iActualIndex       = m_iNbElements;

      m_NodeArray[m_iNbElements]->m_iArrayPosition = m_iNbElements;

      //Update actual index with the new index in the array
      l_iActualIndex = SwapNodeUp(l_iActualIndex);

      m_NodeArray[l_iActualIndex]->m_iArrayPosition = l_iActualIndex;

      return m_NodeArray[l_iActualIndex];
   }


   /*!
   * Remove the top node from the heap
   * @return true if node successfully removed, false otherwise
   **/
   template<class K,class T>
      bool GBinaryHeap<K,T>::RemoveTopNode()
   {
      //Can't remove an unexisting item
      if(m_iNbElements < 1)
         return false;

      //Free the memory (dont call DELETE because the node is still in the pool)
      m_NodeArray[1] = NULL;
      m_iNbElements--;

      //Take the last item and place it at the place of the 1st item
      if(m_iNbElements > 0)
      {
         m_NodeArray[1] = m_NodeArray[m_iNbElements+1];
         UINT32 l_iActualIndex = 1;
         SwapNodeDown(l_iActualIndex);
      }
      return true;
   }

   /*!
   * Get the top node values and remove the top node
   * 
   * @param out_Key: A Reference to the key to be updated
   * @param out_Value: A Reference to the value to be updated
   * @return true if successful, false otherwise
   **/
   template<class K,class T>
      bool GBinaryHeap<K,T>::PopTopNode(K& out_Key,T& out_Value)
   {
      //Get the top node values and remove the node if data fetch was successful
      if(!TopNode(out_Key,out_Value))
      {
         return false;
      }
      else
      {
         RemoveTopNode();
      }
      return true;
   }

   /*!
   * Updates the node with the given Key and Value with the new values
   * and key and sorts everything to respect the heap order
   * (Since a key is not unique, we need it to match with a given value
   *  to make sure we update the correct node)
   *
   * @param in_Node: A pointer to the node to update
   * @param in_NewKey: New value for the key
   * @param in_NewVal: New value
   * @return true if node updated and heap successfully sorted, false otherwise
   **/
   template<class K,class T>
      bool GBinaryHeap<K,T>::UpdateNode(typename GBinaryHeap<K,T>::GHeapNode* in_Node,
      const K& in_NewKey,
      const T& in_NewVal)
   {
      //Cant update a null node
      if(!in_Node)
         return false;

      return UpdateNode(in_Node->m_iArrayPosition,in_NewKey,in_NewVal);
   }

   /*!
   * Update the values of the node at the in_iIndex position
   **/
   template<class K,class T>
      bool GBinaryHeap<K,T>::UpdateNode(UINT32 in_iIndex,const K& in_NewKey,const T& in_NewVal)
   {
      UINT32 l_iActualIndex = in_iIndex;
      if(l_iActualIndex <= 0 || l_iActualIndex > m_iNbElements)
      {
         //Invalid array index
         return false;
      }

      //Update the node values
      m_NodeArray[l_iActualIndex]->m_Key   = in_NewKey;
      m_NodeArray[l_iActualIndex]->m_Value = in_NewVal;

      bool l_bHasBeenSwappedWithParent = false;

      //Check to see if the node with the new key values is still 
      //respecting the heap rules, if not, swap it at the correct place

      //Must be swapped with its parent ???
      {
         UINT32 l_iParentIndex = 0;
         bool   l_bMustSwap    = false;
         l_iParentIndex =        (UINT32)(l_iActualIndex/2);
         if(l_iParentIndex)
         {
            switch(m_OrderMethod)
            {
            case EOrderMethod::OrderIncreasing: //low to high
               {
                  if(*m_NodeArray[l_iActualIndex] <= *m_NodeArray[l_iParentIndex])
                  {//must be swapped
                     l_bMustSwap = true;  
                  }
                  break;
               }
            case EOrderMethod::OrderDecreasing: //high to low
               {
                  if(*m_NodeArray[l_iActualIndex] >= *m_NodeArray[l_iParentIndex])
                  {//must be swapped
                     l_bMustSwap = true;
                  }
                  break;
               }       
            }//end switch
         }

         if(l_bMustSwap)
         {
            SwapNodeUp(l_iActualIndex);
            l_bHasBeenSwappedWithParent = true;
         }
      }//end must be swapped with the parent

      //If it hasnt been swapped with any parent, then it means that it might need to be swapped
      //with its child
      if(!l_bHasBeenSwappedWithParent)
      {
         SwapNodeDown(l_iActualIndex);  
      }
      return true;  
   }


   /*!
   * Check if a node can be swapped with one of its child until it is at the correct place
   * @param in_iPosition: The position of the node to swap
   **/
   template<class K,class T>
      INT32 GBinaryHeap<K,T>::SwapNodeDown(UINT32 in_iPosition)
   {
      if(m_iNbElements <= 1)
         return -1;

      bool l_bMustSwap      = false;
      bool l_bSwapWithFirst = false;
      bool l_bHas2Childs    = false;

      UINT32 l_iActualIndex = in_iPosition;
      UINT32 l_iChild1Index = 0;
      UINT32 l_iChild2Index = 0;
      K      l_Child1Key;
      K      l_Child2Key;

      //Swap until the top node is at the correct place
      for(;;)
      {
         //Check if the new item has childs, if no childs are present, must end the loop
         if((l_iActualIndex * 2) > m_iNbElements)
         {
            break;
         }

         l_bMustSwap = false;
         l_iChild1Index =  l_iActualIndex*2;
         l_iChild2Index = (l_iActualIndex*2) + 1;

         //There must be one child if we get here, but is there a second child
         if(l_iChild2Index <= m_iNbElements)
         {
            l_bHas2Childs = true;
         }
         else
         {
            l_bHas2Childs = false;
         }

         //Check if the nodes must be swapped
         switch(m_OrderMethod)
         {
         case EOrderMethod::OrderIncreasing:
            {
               l_Child1Key = m_NodeArray[l_iChild1Index]->Key();
               if(m_NodeArray[l_iActualIndex]->Key() >= l_Child1Key)
               {
                  l_bMustSwap = true;
               }
               if(l_bHas2Childs)
               {
                  l_Child2Key = m_NodeArray[l_iChild2Index]->Key();
                  if(m_NodeArray[l_iActualIndex]->Key() >= l_Child2Key)
                  {
                     l_bMustSwap = true;
                  }
               }
               break;
            }

         case EOrderMethod::OrderDecreasing:
            {
               l_Child1Key = m_NodeArray[l_iChild1Index]->Key();
               if(m_NodeArray[l_iActualIndex]->Key() <= l_Child1Key)
               {
                  l_bMustSwap = true;
               }
               if(l_bHas2Childs)
               {
                  l_Child2Key = m_NodeArray[l_iChild2Index]->Key();
                  if(m_NodeArray[l_iActualIndex]->Key() <= l_Child2Key)
                  {
                     l_bMustSwap = true;
                  }
               }
               break;
            }
         default:
            return -1;
         }//End Switch

         if(l_bMustSwap)
         {
            //If we must swap, check with which child we must swap
            if(l_bHas2Childs)
            {
               l_bSwapWithFirst = false;
               switch(m_OrderMethod)
               {
               case EOrderMethod::OrderIncreasing:
                  {
                     if(l_Child1Key <= l_Child2Key)
                     {
                        l_bSwapWithFirst = true;
                     }
                     break;
                  }
               case EOrderMethod::OrderDecreasing:
                  {
                     if(l_Child1Key >= l_Child2Key)
                     {
                        l_bSwapWithFirst = true;
                     }
                     break;
                  }
               default:
                  return -1;         
               } //End Switch
            }
            else //There is only one child and we must swap, meaning we must swap with the 1st child
            {
               l_bSwapWithFirst = true;
            }
            //Do the swap
            if(l_bSwapWithFirst)
            {
               SwapNodes(l_iActualIndex,l_iChild1Index);
               l_iActualIndex = l_iChild1Index;
            }
            else
            {
               SwapNodes(l_iActualIndex,l_iChild2Index);
               l_iActualIndex = l_iChild2Index;           
            }

         }//End if must swap     
         else
         {
            //We must not swap, end the process
            break;
         }
      }//End For(;;)    


      return l_iActualIndex;
   }

   /*!
   * Check if a node can be swapped with its parent until it is at the correct place
   * @param in_iPosition: The position of the node to swap
   **/
   template<class K,class T>
      INT32 GBinaryHeap<K,T>::SwapNodeUp(UINT32 in_iPosition)
   {
      //If position is invalid
      if((in_iPosition > m_iNbElements) || 
         (in_iPosition < 0)                 )
         return -1;

      INT32 l_iActualIndex = in_iPosition;
      //Must be swapped with its parent ???
      {   
         UINT32 l_iParentIndex = 0;
         bool   l_bMustSwap    = false;
         for(;;)
         {
            //If we are at the top
            if( l_iActualIndex <= 1)
            {
               return l_iActualIndex;
            }

            l_bMustSwap    = false;
            l_iParentIndex = (UINT32)(l_iActualIndex/2);

            switch(m_OrderMethod)
            {
            case EOrderMethod::OrderIncreasing: //low to high
               {
                  if(*m_NodeArray[l_iActualIndex] <= *m_NodeArray[l_iParentIndex])
                  {//must be swapped
                     l_bMustSwap = true;  
                  }
                  break;
               }
            case EOrderMethod::OrderDecreasing: //high to low
               {
                  if(*m_NodeArray[l_iActualIndex] >= *m_NodeArray[l_iParentIndex])
                  {//must be swapped
                     l_bMustSwap = true;
                  }
                  break;
               }       
            }//end switch

            //If we must swap
            if(l_bMustSwap)
            {
               SwapNodes(l_iActualIndex,l_iParentIndex); 
               l_iActualIndex                = l_iParentIndex;
            }
            else
            {
               break;
            }


         }//end for(;;)
      }//end must be swapped with the parent
      return l_iActualIndex;
   }

   //! Clear the elements of the heap
   template<class K,class T>
      bool GBinaryHeap<K,T>::Clear()
   {
      m_iNbElements = 0;
      m_iNextNode   = 0;
      return true;
   }

   //! Return the number of elements in the heap
   template<class K,class T>
      UINT32 GBinaryHeap<K,T>::Size()
   {
      return m_iNbElements;
   }



}; //End of namespace hector



/*!
* Swap two ones, given the nodes index
**/
template<class K,class T>
bool GBinaryHeap<K,T>::SwapNodes(UINT32 in_iIndex1,UINT32 in_iIndex2)
{
   if(in_iIndex1 <= 0 || in_iIndex2 <= 0|| in_iIndex1 > m_iNbElements || in_iIndex2 > m_iNbElements)
      return false;

   GBinaryHeap<K,T>::GHeapNode* l_pNode;
   l_pNode                       = m_NodeArray[in_iIndex1];
   m_NodeArray[in_iIndex1]       = m_NodeArray[in_iIndex2];
   m_NodeArray[in_iIndex2]       = l_pNode;

   m_NodeArray[in_iIndex1]->m_iArrayPosition = in_iIndex1;
   m_NodeArray[in_iIndex2]->m_iArrayPosition = in_iIndex2;
   return true;

}

#pragma warning(default: 4701)
#endif //_GOLEM_BINARYHEAP_H_



