/**************************************************************
*
* golem_tree.h
*
* Description
* ===========
*  Description & implementation of GTree & GTreeNode template 
*  classes
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_TREE_H
#define _GOLEM_TREE_H


#include "golem_utils.h"
#pragma warning(disable : 4786)

namespace Hector
{

   /*************************************
   * Structure to represent a tree node
   *
   *
   ************************************/
   template<class T> 
   class GTreeNode
   {
      vector<GTreeNode<T>*>   m_ChildList;
      T                       m_Data;   
      GTreeNode<T>*           m_pParent;
      UINT32                  m_iCurrentChildIndex;

      //Set_Parent
      void SetParent(GTreeNode<T>* ParentPtr)
      {
         m_pParent = ParentPtr;
      }

   public:

      GTreeNode<T>();
      ~GTreeNode<T>();

      //Data
      void        Data(const T&);
      const T&    Data() const;
      T&          Data();

      //Add Child - returns a pointer to the new Child
      GTreeNode<T>* AddChild(const T& data);
      GTreeNode<T>* AddChild(GTreeNode<T>* ChildNode);

      //Delete Child
      bool RemoveChild(UINT32 index);

      //Replace Child
      bool ReplaceChild(UINT32 index, const T& data);

      //Has Child ?
      bool HasChild() const;

      //Get First Child
      const GTreeNode<T>* FirstChild() const;
      GTreeNode<T>* FirstChild();

      //Get Last Child
      const GTreeNode<T>* LastChild() const;
      GTreeNode<T>* LastChild();

      UINT32        NbChilds() const
      {
         return (UINT32) m_ChildList.size();
      }

      const GTreeNode<T>* Child(UINT32 index) const;
      GTreeNode<T>* Child(UINT32 index);

      //Current_Child
      GTreeNode<T>* CurrentChild();

      //Next_Child
      GTreeNode<T>* NextChild();

      //Set Current Child
      bool          CurrentChild(UINT32 index);

      //GetParent
      const GTreeNode<T>* Parent() const;
      GTreeNode<T>* Parent();

      //Swap two Childs
      bool          SwapChilds(UINT32 ChildIndex1,UINT32 ChildIndex2);

      void ClearChilds();
   };

   /********************************************************************
   Tree class that simply holds a root node all the manipulation
   is done on the root node.
   ********************************************************************/
   template<class T>
   class GTree
   {
      GTreeNode<T>*   m_pRootNode;
   public:
      //Constructor
      GTree()
      {
         m_pRootNode = new GTreeNode<T>();
      }
      //Destructor
      ~GTree()
      {
         SAFE_DELETE(m_pRootNode);
      }
      //Get root node
      GTreeNode<T>*   Root()
      {
         return m_pRootNode;
      }
      //Get constant root node
      const GTreeNode<T>*   Root() const
      {
         return m_pRootNode;
      }
   };


#endif


   /***********************************************************************
   *
   *                  GTreeNode implementation
   *
   *************************************************************************/
   template<class T> GTreeNode<T>::GTreeNode()
   {    
      m_pParent = NULL;
      m_iCurrentChildIndex = 0;
      m_ChildList.clear();
   }

   /************
   class destructor
   Deletes all Childs under this node
   ************/
   template<class T> GTreeNode<T>::~GTreeNode()
   {    
      this->ClearChilds();
   }

   /**************************
   * Recursively deletes the Childs in a node
   *
   **************************/
   template<class T> void GTreeNode<T>::ClearChilds()
   {
      UINT32 ChildListSize = (UINT32) m_ChildList.size();

      GTreeNode* nodePtr;
      UINT32 i = 0;
      for(i = 0 ; i < ChildListSize ; i++)
      {
         nodePtr =  m_ChildList[i];
         nodePtr->ClearChilds();
         SAFE_DELETE(nodePtr);    
      }
      m_ChildList.clear();
   }

   /*****************************
   * Swap_Childs()
   * Swap two Childs 
   *         Given 2 indexes, it swaps the Childs in the vector
   *******************************/

   template<class T> bool GTreeNode<T>::SwapChilds(UINT32 ChildIndex1,UINT32 ChildIndex2)
   {

      if(ChildIndex1 < 0 || (unsigned)ChildIndex1 >= m_ChildList.size())
         return false;

      if(ChildIndex2 < 0 || (unsigned)ChildIndex2 >= m_ChildList.size())
         return false;



      GTreeNode<T>* Child1 = m_ChildList[ChildIndex1];
      m_ChildList[ChildIndex1] = m_ChildList[ChildIndex2];
      m_ChildList[ChildIndex2] = Child1;   

      return true;
   }


   template<class T> void GTreeNode<T>::Data(const T& data)
   {
      m_Data = data;
   }

   template<class T> const T& GTreeNode<T>::Data() const
   {
      return m_Data;
   }

   template<class T> T& GTreeNode<T>::Data()
   {
      return m_Data;
   }

   //Add Child
   template<class T> GTreeNode<T>* GTreeNode<T>::AddChild(const T& data)
   {
      GTreeNode<T>* newNode = new GTreeNode<T>();
      newNode->Data(data);
      newNode->SetParent(this);
      m_ChildList.push_back(newNode);
      return newNode;
   }

   template<class T> GTreeNode<T>* GTreeNode<T>::AddChild(GTreeNode<T>* ChildNode)
   {
      m_ChildList.push_back(ChildNode);
      return ChildNode;
   }


   //Delete Child
   template<class T>     bool GTreeNode<T>::RemoveChild(UINT32 index)
   {
      if((unsigned)index >= m_ChildList.size())
         return false;

      vector<GTreeNode<T>*>::iterator it;
      it = m_ChildList.begin();

      it+= index;

      //Backup the pointer (so we can free the memory after
      // it has been removed from the list)
      GTreeNode<T>* nodePtr = m_ChildList[index];

      //Erase it from the vector
      m_ChildList.erase(it);


      //Free the memory of all its Childs plus its own memory
      nodePtr->ClearChilds();
      SAFE_DELETE(nodePtr);


      return true;
   }

   //Replace Child
   template<class T>     bool GTreeNode<T>::ReplaceChild(UINT32 index, const T& data)
   {
      if(index >= m_ChildList.size())
         return false;

      m_ChildList[index]->m_Data = data;

      return true;
   }


   //Has Child ?
   template<class T> bool GTreeNode<T>::HasChild() const
   {
      if(m_ChildList.size() > 0)
         return true;
      else
         return false;
   }


   //Get First Child
   template<class T>     const GTreeNode<T>* GTreeNode<T>::FirstChild() const
   {
      if(m_ChildList.size() == 0)
         return NULL;

      return m_ChildList[0];
   }

   template<class T>     GTreeNode<T>* GTreeNode<T>::FirstChild()
   {
      if(m_ChildList.size() == 0)
         return NULL;

      return m_ChildList[0];
   }

   //Get Last Child
   template<class T>     const GTreeNode<T>* GTreeNode<T>::LastChild() const
   {
      _int32 size = m_ChildList.size();

      if(size > 0)
         return m_ChildList[size-1];
      else
         return NULL;
   }

   template<class T>     GTreeNode<T>* GTreeNode<T>::LastChild()
   {
      _int32 size = m_ChildList.size();

      if(size > 0)
         return m_ChildList[size-1];
      else
         return NULL;
   }

   template<class T> const GTreeNode<T>* GTreeNode<T>::Child(UINT32 index) const
   {
      if(index >= (signed)m_ChildList.size())
         return NULL;

      return m_ChildList[index];

   }

   template<class T> GTreeNode<T>* GTreeNode<T>::Child(UINT32 index)
   {
      if(index >= (signed)m_ChildList.size())
         return NULL;

      return m_ChildList[index];

   }

   //Current_Child
   template<class T>     GTreeNode<T>* GTreeNode<T>::CurrentChild()
   {
      if(m_iCurrentChildIndex >= m_ChildList.size())
         return NULL;

      return m_ChildList[m_iCurrentChildIndex];
   }

   template<class T>     bool GTreeNode<T>::CurrentChild(UINT32 index)
   {
      //is the index valid
      if(index < m_ChildList.size())
      {
         m_iCurrentChildIndex = index;
         return true;
      }
      else
         return false;


   }
   //Next_Child
   template<class T>    GTreeNode<T>* GTreeNode<T>::NextChild()
   {
      //Increment the current Child index
      m_iCurrentChildIndex++;

      //Validate the new index
      //If at the end of the list, we return NULL
      if(m_iCurrentChildIndex >= m_ChildList.size() )
      {
         m_iCurrentChildIndex--;
         return NULL;
      }

      //Return the new item pointed by the current index
      return m_ChildList[m_iCurrentChildIndex];
   }


   //GetParent
   template<class T>    const GTreeNode<T>* GTreeNode<T>::Parent() const
   {
      return m_pParent;
   }

   template<class T>    GTreeNode<T>* GTreeNode<T>::Parent()
   {
      return m_pParent;
   }
}
