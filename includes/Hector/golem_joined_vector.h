/**************************************************************
*
* golem_joinedvectors.h
*
* Description
* ===========
*  Describes the interface of class GJoinedVectors
*
* Owner
* =====
*  mtremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_JOINEDVECTORS_H_
#define _GOLEM_JOINEDVECTORS_H_

namespace Hector
{

   /*!
   * Class that handles multiples vectors as one
   **/
   template<class T>
   class GJoinedVectors
   {
      friend class GIterator;
   public:

      class GIterator
      {
         friend GJoinedVectors<T>;
      public:
         GIterator(GJoinedVectors<T>* in_pJoinedVector);
         ~GIterator();

         GIterator&  operator++();
         GIterator&  operator++(int);
         T&          operator*();
         T&          Value();
         T*          operator->();

         bool        operator ==(const GIterator& in_Right) const;
         bool        operator !=(const GIterator& in_Right) const;



      private:
         GJoinedVectors<T>*            m_pAssignedJoinedVector;
         INT32                         m_iActualVector;
         typename vector<T>::iterator  m_ActualIterator;
      };

      GJoinedVectors();
      ~GJoinedVectors();

      //! Adds a vector, returns its index in the vector list
      UINT32       AddVector(vector<T>& in_vAddedVector);
      GIterator&   Find(const T& in_ToSearch);
      GIterator&   Begin();
      GIterator&   End();

   protected: 
   private:
      vector<vector<T>*> m_vVectors;
      vector<T>          m_pEmptyDummyVector; //Just so we have always a valid end iterator

      GIterator          m_End;
      GIterator          m_Begin;
   };

   template<class T>
   GJoinedVectors<T>::GJoinedVectors() : m_End(NULL), m_Begin(NULL)
   {
      //Construct the End Vector
      {
         m_End.m_iActualVector         = -1;
         m_End.m_pAssignedJoinedVector = NULL;
         m_End.m_ActualIterator        = m_pEmptyDummyVector.end();
      }
      //Construct the begin vector
      {
         m_Begin = m_End;
      }
   }
   template<class T>
   GJoinedVectors<T>::~GJoinedVectors()
   {
      m_vVectors.clear();
   }

   /*!
   * Adds a vector, returns its index in the vector list
   **/
   template<class T>
   UINT32 GJoinedVectors<T>::AddVector(vector<T>& in_vAddedVector)
   {
      //Get the index
      UINT32 l_iAddedVectorIndex = m_vVectors.size();
      //Add to the list and return the index
      m_vVectors.push_back(&in_vAddedVector);
      return     l_iAddedVectorIndex;
   }


   template<class T>
   typename GJoinedVectors<T>::GIterator&   GJoinedVectors<T>::Begin()
   {
      //Look for the 1st vector that contains something
      UINT32 l_iIndex = 0;
      for(;;)
      {
         //If the vector exists
         if(l_iIndex < m_vVectors.size())
         {
            //If there is an element in the vector
            if(m_vVectors[l_iIndex]->size())
            {
               m_Begin.m_pAssignedJoinedVector = this;
               m_Begin.m_iActualVector    = l_iIndex;
               m_Begin.m_ActualIterator   = m_vVectors[l_iIndex]->begin();
               return m_Begin;
            }
         }
         else
         {
            return End();
         }
         l_iIndex++;
      }
   }

   template<class T>
   typename GJoinedVectors<T>::GIterator&   GJoinedVectors<T>::End()
   {
      return m_End;
   }

   template<class T>
   typename GJoinedVectors<T>::GIterator&   GJoinedVectors<T>::Find(const T& in_ToSearch)
   {
      GIterator l_It(this);
      l_It = Begin();
      while(l_It != End())
      {
         if(*l_It == in_ToSearch)
            return l_It;
         l_It++;
      }
   }



   //////////////////////////////////// ITERATOR ////////////////////////////////////////////////

   /*!
   * Iterator constructor
   **/
   template<class T>
   GJoinedVectors<T>::GIterator::GIterator(GJoinedVectors<T>* in_pJoinedVector)
   {
      m_pAssignedJoinedVector    = in_pJoinedVector;

      if(m_pAssignedJoinedVector)
         *this = m_pAssignedJoinedVector->Begin();
      else
      {
         m_iActualVector = -1;
      }
   }

   /*!
   * Iterator destructor
   **/
   template<class T>
   GJoinedVectors<T>::GIterator::~GIterator()
   {
      m_pAssignedJoinedVector       = NULL;
      m_iActualVector               =   -1;
   }

   template<class T>
   T& GJoinedVectors<T>::GIterator::operator*()
   {
      return Value();
   }

   template<class T>
   T* GJoinedVectors<T>::GIterator::operator->()
   {
      return &Value();
   }


   template<class T>
   T& GJoinedVectors<T>::GIterator::Value()
   {
      return *m_ActualIterator;
   }


   template<class T>
   bool GJoinedVectors<T>::GIterator::operator ==(const GIterator& in_Right) const
   {
     if( 
         (m_ActualIterator        == in_Right.m_ActualIterator)       &&
         (m_pAssignedJoinedVector == in_Right.m_pAssignedJoinedVector) &&
         (m_iActualVector         == in_Right.m_iActualVector)
         )
         return true;
      else
         return false;
   }

   template<class T>
   bool GJoinedVectors<T>::GIterator::operator !=(const GIterator& in_Right) const
   {
      if(in_Right == *this)
         return false;
      else
         return true;
   }


   template<class T>
   typename GJoinedVectors<T>::GIterator& GJoinedVectors<T>::GIterator::operator++(int)
   {
      gassert(m_pAssignedJoinedVector,"GJoinedVectors<T>::GIterator ++, invalid null pointer");
      //If we are already at the end
      if(*this == m_pAssignedJoinedVector->End())
      {
         //already at the end, dont do anything
      }
      else
      {
         //Iterator is valid
         gassert(m_iActualVector >= 0 && m_iActualVector < (INT32)m_pAssignedJoinedVector->m_vVectors.size(),"GJoinedVector::Iterator, iterator ++ Invalid actual vector index");
         gassert(m_ActualIterator != m_pAssignedJoinedVector->m_vVectors[m_iActualVector]->end(),"GJoinedVector::Iterator Invalid actual iterator pointer");

         m_ActualIterator++;
         //If we reached the end of the actually pointed vector, check if we can reach another vector
         if(m_ActualIterator == m_pAssignedJoinedVector->m_vVectors[m_iActualVector]->end())
         {

            for(;;)
            {
               //If there is still room, increment the actual vector index
               if(m_iActualVector < (INT32)(m_pAssignedJoinedVector->m_vVectors.size()-1))
               {
                  m_iActualVector++;

                  //If the next vector contains elements, assign the actual iterator to the 1st 
                  //elemen
                  if(m_pAssignedJoinedVector->m_vVectors[m_iActualVector]->size())
                  {
                     m_ActualIterator = m_pAssignedJoinedVector->m_vVectors[m_iActualVector]->begin();
                  }
                  else
                  {
                     //No items in that vector, continue the loop (check for the next vector if it has items.
                     continue;
                  }

               }
               //Not enough room, means we are at the end
               else
               {
                  gassert(m_pAssignedJoinedVector,"Shouldnt be NULL");
                  *this = m_pAssignedJoinedVector->End(); 
                  break;
               }
            }

         }
         else
         {
            //Iterator was iterated correctly, dont do anything
         }
      }
      return *this;
   }



   template<class T>
   typename GJoinedVectors<T>::GIterator& GJoinedVectors<T>::GIterator::operator++()
   {
      (*this)++;
      return *this;

   }
};
#endif //_GOLEM_JOINEDVECTORS_H_
