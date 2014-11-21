/**************************************************************
*
* golem_buffer.h
*
* Description
* ===========
*  Contains GIBuffer and GOBuffer. GIBuffer implements a very efficient arbitrary storage class
*  which can be used for serialization of about anything. Just use the push operators (<<)
*  The GOBuffer is it's counterpart. The Buffers doesn't remember the type of data that has been put
*  in it, so data must be pop()ed in the same order that it were push()ed.
*  GIBuffer << val1 << val2
*  GOBuffer >> val1 >> val2
*
* Owner
* =====
*  Nicolas Hatier
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_BUFFER_H
#define _GOLEM_BUFFER_H

namespace Hector 
{
   class GSerializable;


   class GIOBuffer
   {
   public:
      //! construct buffer with desired chunk size. If you think it will contain a lot of data, increase this number.
      GIOBuffer(int = 1024);

      //! Copy constructor, the data is really copied
      GIOBuffer(const GIOBuffer &);
      ~GIOBuffer(void);

      //! Used to store almost all basic types in the buffer
      template< class T > GIOBuffer& operator << (T rsh)
      {
#ifdef GOLEM_DEBUG
         __if_exists(T::Serialize)
         {
            gassert(0, "Please use Serialize or cast your object in GSerializable");
         }
#endif
         return Push((INT8*)&rsh, sizeof(T));
      }

      //! Used to store almost all vector of basic types in the buffer
      template< class T > GIOBuffer& operator << (const GVector2D<T> & rsh)
      {
         return (*this) << rsh.x << rsh.y;         
      }

      template< class T > GIOBuffer& operator << (const vector<T> & in_vData)
      {
         (*this) << (UINT32) in_vData.size();

         for(vector<T>::const_iterator l_It = in_vData.begin();
             l_It != in_vData.end(); ++ l_It)
         {
            (*this) << *l_It;
         }

         return (*this);
      }

      template< class T > GIOBuffer& operator << (const set<T> & in_Data)
      {
         (*this) << (UINT32) in_Data.size();

         for(set<T>::const_iterator l_It = in_Data.begin();
             l_It != in_Data.end(); ++ l_It)
         {
            (*this) << *l_It;
         }

         return (*this);
      }

      template< class T > GIOBuffer& operator << (const stdext::hash_set<T> & in_Data)
      {
         (*this) << (UINT32) in_Data.size();

         for(stdext::hash_set<T>::const_iterator l_It = in_Data.begin();
             l_It != in_Data.end(); ++ l_It)
         {
            (*this) << *l_It;
         }

         return (*this);
      }

      //! Used to store most used classes in the buffer
      GIOBuffer& operator << (const GString   &);
      GIOBuffer& operator << (const GSString  &);
      GIOBuffer& operator << (const GDateTime &);
      GIOBuffer& operator << (const GTimeSpan &);
      GIOBuffer& operator << (const GIOBuffer &);

      //! All the >> operators are destructive, the buffer is "emptied" progressively
      template< class T >  GIOBuffer& operator >> (T & rsh)
      {
#ifdef GOLEM_DEBUG
         __if_exists(T::Unserialize)
         {
            gassert(0, "Please use Unserialize");
         }
#endif
         return Pop((INT8*)&rsh, sizeof(T));
      }


      template< class T >  GIOBuffer& operator >> (const GVector2D<T> & rsh)
      {
         return (*this) >> rsh.x >> rsh.y;         
      }

      template< class T >  GIOBuffer& operator >> (vector<T> & out_vData)
      {
         UINT32 l_iSize;
         (*this) >> l_iSize;

         out_vData.resize(l_iSize);
         for(vector<T>::const_iterator l_It = out_vData.begin();
             l_It != out_vData.end(); ++ l_It)
         {
            (*this) >> *l_It;
         }

         return (*this);
      }

      template< class T >  GIOBuffer& operator >> (set<T> & out_Data)
      {
         UINT32 l_iSize;
         (*this) >> l_iSize;

         for(UINT32 i = 0;i < l_iSize;i ++)
         {
            T l_Temp;
            (*this) >> l_Temp;
            out_Data.insert(l_Temp);
         }

         return (*this);
      }

      template< class T >  GIOBuffer& operator >> (stdext::hash_set<T> & out_Data)
      {
         UINT32 l_iSize;
         (*this) >> l_iSize;

         for(UINT32 i = 0;i < l_iSize;i ++)
         {
            T l_Temp;
            (*this) >> l_Temp;
            out_Data.insert(l_Temp);
         }

         return (*this);
      }

      GIOBuffer& operator >> (GString &);
      GIOBuffer& operator >> (GSString &);
      GIOBuffer& operator >> (GDateTime &);
      GIOBuffer& operator >> (GTimeSpan &);
      GIOBuffer& operator =  (const GIOBuffer &);//!<  assignment. data is copied, old data is deleted
        

      //! adds arbitrary data to buffer
      GIOBuffer& Push (const INT8* in_pData, UINT32 in_iSize);
      //! gets arbitraty data
      GIOBuffer& Pop  (INT8* out_pData, UINT32 in_iSize);
      void       Peek (INT8* out_pData, UINT32 in_iSize); //!<Read a value from the buffer without removing it
      void       Clear();
      UINT32     Size() const; //!< Total size of the data      
      GSmartPtr<INT8> Data() const; //!< Used to retreive the data contained in the buffer.

   private:      
      const UINT32 CHUNK_SIZE;
      
      //! Used for returning data, doesn't affects the internal contents
      mutable bool m_bModified;
      mutable Hector::GSmartPtr<INT8> m_Returned;
      
      list<INT8*> m_Data;
      INT8*       m_pCurrentChunk;
      INT32       m_iCurrentWriteOffset;
      INT32       m_iCurrentReadOffset;
      UINT32      m_iSize;
   };

   class GIBuffer
   {
   public:
      //! construct buffer with desired chunk size. If you think it will contain a lot of data, increase this number.
      GIBuffer(int = 1024);

      //! Copy constructor. The data is really copied.
      GIBuffer(const GIBuffer &);
      ~GIBuffer(void);

      //! Used to store almost all basic types in the buffer
      template< class T >
      GIBuffer& operator << (T rsh)
      {
#ifdef GOLEM_DEBUG
         __if_exists(T::Serialize)
         {
            gassert(0, "Please use Serialize or cast your object in GSerializable");
         }
#endif
         return push((INT8*)&rsh, sizeof(T));
      }

      //! Used to store almost all vector of basic types in the buffer
      template< class T >
      GIBuffer& operator << (const GVector2D<T> & rsh)
      {
         return (*this) << rsh.x << rsh.y;         
      }

      template< class T >
      GIBuffer& operator << (const vector<T> & in_vData)
      {
         (*this) << (UINT32) in_vData.size();

         for(vector<T>::const_iterator l_It = in_vData.begin();
             l_It != in_vData.end(); ++ l_It)
         {
            (*this) << *l_It;
         }

         return (*this);
      }

      template< class T >
      GIBuffer& operator << (const list<T> & in_Data)
      {
         (*this) << (UINT32) in_Data.size();

         for(list<T>::const_iterator l_It = in_Data.begin();
             l_It != in_Data.end(); ++ l_It)
         {
            (*this) << *l_It;
         }

         return (*this);
      }

      template< class T >
      GIBuffer& operator << (const set<T> & in_Data)
      {
         (*this) << (UINT32) in_Data.size();

         for(set<T>::const_iterator l_It = in_Data.begin();
             l_It != in_Data.end(); ++ l_It)
         {
            (*this) << *l_It;
         }

         return (*this);
      }

      template< class T >
      GIBuffer& operator << (const stdext::hash_set<T> & in_Data)
      {
         (*this) << (UINT32) in_Data.size();

         for(stdext::hash_set<T>::const_iterator l_It = in_Data.begin();
             l_It != in_Data.end(); ++ l_It)
         {
            (*this) << *l_It;
         }

         return (*this);
      }

      template< class T , class U >
      GIBuffer& operator << (const stdext::hash_map<T, U> & in_Data)
      {
         (*this) << (UINT32) in_Data.size();

         for(stdext::hash_map<T, U>::const_iterator l_It = in_Data.begin();
             l_It != in_Data.end(); ++ l_It)
         {
            (*this) << l_It->first;
            (*this) << l_It->second;
         }

         return (*this);
      }

      template< class T, class U >
      GIBuffer& operator << (const multimap<T, U> & in_Data)
      {
         (*this) << (UINT32) in_Data.size();

         for(multimap<T, U>::const_iterator l_It = in_Data.begin();
             l_It != in_Data.end(); ++ l_It)
         {
            (*this) << l_It->first << l_It->second;
         }

         return (*this);
      }

      //! Used to store most used classes in the buffer
      GIBuffer& operator << (const GString &);
      GIBuffer& operator << (const GSString &);
      GIBuffer& operator << (const GDateTime &);
      GIBuffer& operator << (const GTimeSpan &);
      GIBuffer& operator << (const GIBuffer &);
      GIBuffer& operator << (const GSerializable &);
      
      //! assignment. data is copied, old data is deleted
      GIBuffer& operator =  (const GIBuffer &);

      //! Special operator, is used to allocate (@param) bytes of space in the buffer, which can be filled later.
      GIBuffer& operator <<= (UINT32);
      
      //! Used to retreive the data contained in the buffer.
      GSmartPtr<INT8> Data() const;
      
      //! Total size of the data
      UINT32 Size() const;

      //! adds arbitrary data to buffer
      GIBuffer& push (const INT8* data, UINT32 size);

      void Clear();

   private:      
      const UINT32 CHUNK_SIZE;
      
      //! Used for returning data, doesn't affects the internal contents
      mutable bool m_bModified;
      mutable Hector::GSmartPtr<INT8> m_Returned;
      
      list<INT8*> m_Data;
      INT8 * m_pCurrentChunk;
      INT32 m_pCurrentChunkoffset;
      UINT32 m_iSize;
   };


   class GOBuffer
   {
   public:
      GOBuffer(GSmartPtr<INT8>, UINT32);
      GOBuffer(const GOBuffer &);
      GOBuffer(const GIBuffer &);
      ~GOBuffer(void);

      const INT8 * PeekPtr(void) const;

      //! All the >> operators are destructive, the buffer is "emptied" progressively
      template< class T >
      GOBuffer& operator >> (T & rsh)
      {
#ifdef GOLEM_DEBUG
         __if_exists(T::Unserialize)
         {
            gassert(0, "Please use Unserialize");
         }
#endif
         return pop((INT8*)&rsh, sizeof(T));
      }

      template< class T >
      GOBuffer& operator >> (const GVector2D<T> & rsh)
      {
         return (*this) >> rsh.x >> rsh.y;         
      }

      template< class T >
      GOBuffer& operator >> (vector<T> & out_vData)
      {
         UINT32 l_iSize;
         (*this) >> l_iSize;

         out_vData.resize(l_iSize);
         for(vector<T>::iterator l_It = out_vData.begin();
             l_It != out_vData.end(); ++ l_It)
         {
            T l_Value;
            (*this) >> l_Value;
            *l_It = l_Value;
         }

         return (*this);
      }

      template< class T >
      GOBuffer& operator >> (list<T> & out_Data)
      {
         UINT32 l_iSize;
         (*this) >> l_iSize;
         out_Data.clear();
         for(UINT32 i = 0;i < l_iSize;i ++)
         {
            T l_Value;
            (*this) >> l_Value;
            out_Data.push_back(l_Value);
         }

         return (*this);
      }

      template< class T >
      GOBuffer& operator >> (set<T> & out_Data)
      {
         UINT32 l_iSize;
         (*this) >> l_iSize;
         out_Data.clear();
         for(UINT32 i = 0;i < l_iSize;i ++)
         {
            T l_Temp;
            (*this) >> l_Temp;
            out_Data.insert(l_Temp);
         }

         return (*this);
      }

      template< class T >
      GOBuffer& operator >> (stdext::hash_set<T> & out_Data)
      {
         UINT32 l_iSize;
         (*this) >> l_iSize;
         out_Data.clear();
         for(UINT32 i = 0;i < l_iSize;i ++)
         {
            T l_Temp;
            (*this) >> l_Temp;
            out_Data.insert(l_Temp);
         }

         return (*this);
      }

      template< class T , class U >
      GOBuffer& operator >> (stdext::hash_map<T, U> & out_Data)
      {
         UINT32 l_iSize;
         (*this) >> l_iSize;
         out_Data.clear();
         for(UINT32 i = 0;i < l_iSize;i ++)
         {
            T l_Key;
            U l_Val;
            (*this) >> l_Key >> l_Val;
            out_Data.insert(make_pair(l_Key, l_Val) );
         }

         return (*this);
      }

      template< class T, class U >
      GOBuffer& operator >> (multimap<T, U> & out_Data)
      {
         UINT32 l_iSize;
         (*this) >> l_iSize;
         out_Data.clear();
         for(UINT32 i = 0;i < l_iSize;i ++)
         {
            T l_Key;
            U l_Value;
            (*this) >> l_Key >> l_Value;
            out_Data.insert(make_pair(l_Key, l_Value) );
         }

         return (*this);
      }

      GOBuffer& operator >> (GString &);
      GOBuffer& operator >> (GSString &);
      GOBuffer& operator >> (GDateTime &);
      GOBuffer& operator >> (GTimeSpan &);
      GOBuffer& operator >> (GSerializable &);

      void operator >> (GIBuffer &);

      //! The internal pointers of the buffer are sent to the other buffer
      void operator >> (GOBuffer &);

      //! Used to remove arbitrary amount of data without really retreiving it
      GOBuffer& operator >>= (UINT32);
      
      UINT32 Remaining() const;

      GOBuffer& pop(INT8 *, UINT32);

      //! Read a value from the buffer without removing it
      void Peek(INT8 *, UINT32);

      INT8* ActualPtr(){return m_pdata + m_position; }

   private:
      GSmartPtr<INT8> m_Data;
      INT8 * m_pdata;
      UINT32 m_position;
      UINT32 m_iSize;
   };



}

#endif
