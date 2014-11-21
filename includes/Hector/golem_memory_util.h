/**************************************************************
*
* golem_memory_util.h
*
* Description
* ===========
*  Generic memory function, useful for cross-platform & type 
*  independant memory management.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_MEMORY_
#define _GOLEM_MEMORY_

#include "golem_types.h"
#include "golem_utils.h"

namespace Hector
{
   namespace Memory
   {
      /*!
       * Clear the number of elements specified in the memory buffer.
       *
       * @param   in_pMemory  : Memory buffer to clear
       * @param   in_iNbElems : Number of memory elements to clear
       **/
      template<typename T>
      inline void Clear(T* in_pMemory, UINT32 in_iNbElems)
      {
         memset(in_pMemory, 0, in_iNbElems * sizeof(T) );
      }

      /*!
       * Clears one element of the memory buffer.
       *
       * @param   in_pMemory  : Memory element to clear
       **/
      template<typename T>
      inline void Clear(T* in_pMemory)
      {
         memset(in_pMemory, 0, sizeof(T) );
      }

      /*!
       * Set each byte of the input variable to the specified value.
       *
       * @param   in_pMemory  : Memory element to set
       * @param   in_iValue   : Value of the memory
       **/
      template<typename T>
      inline void Set(T* in_pMemory, UINT32 in_iValue)
      {
         memset(in_pMemory, in_iValue, sizeof(T) );
      }

      /*!
       * Set each byte of the memory buffer to the specified value.
       *
       * @param   in_pMemory  : Memory element to set
       * @param   in_iValue   : Value of the memory
       * @param   in_iNbElems : Number of elements to set
       **/
      template<typename T>
      inline void Set(T* in_pMemory, UINT32 in_iValue, UINT32 in_iNbElems)
      {
         memset(in_pMemory, in_iValue, sizeof(T) * in_iNbElems);
      }

#pragma warning (disable : 4127)
      /*!
       * Fills a memory buffer with the specified value.
       *
       * @param   in_pMemory  : Memory buffer to fill
       * @param   in_Value    : Value used to fill the buffer
       * @param   in_iNbElems : Number of memory elements to fill
       **/
      template<typename T>
      inline void Fill(T* in_pMemory, const T& in_Value, UINT32 in_iNbElems)
      {
         if(sizeof(T) == 1)
         {
            memset(in_pMemory, (INT32) in_Value, in_iNbElems);
         }
         else
         {
            //! \todo TBA : this is a place where ASM optimization is possible
            for(UINT32 i = 0;i < in_iNbElems;i ++)
            {
               memcpy(in_pMemory + i, &in_Value, sizeof(T) );
            }
         }
      }
#pragma warning (default : 4127)

      /*!
       * Copy the number of elements from one buffer to another.
       * Copy is not guaranteed to work for overlapping memory buffers.
       *
       * @param   in_pDest    : Destination memory buffer
       * @param   in_pSource  : Source memory buffer
       * @param   in_iNbElems : Number of memory elements to copy
       **/
      template<typename T>
      inline void Copy(T* in_pDest, const T* in_pSource, UINT32 in_iNbElems)
      {
         memcpy(in_pDest, in_pSource, in_iNbElems * sizeof(T) );
      }

      /*!
       * Move the number of elements from one buffer to another.
       * Move is guaranteed to work for even with overlapping memory buffers.
       *
       * @param   in_pDest    : Destination memory buffer
       * @param   in_pSource  : Source memory buffer
       * @param   in_iNbElems : Number of memory elements to move
       **/
      template<typename T>
      inline void Move(T* in_pDest, const T* in_pSource, UINT32 in_iNbElems)
      {
         memmove(in_pDest, in_pSource, in_iNbElems * sizeof(T) );
      }

      /*!
       * Compares number of elements between two buffers.
       * Returns 0 if buffers are identical, < 0 if Buffer1
       * is smaller, > 0 if Buffer1 is larger.
       *
       * @param   in_pBuffer1 : First memory buffer
       * @param   in_pBuffer2 : Second memory buffer
       * @param   in_iNbElems : Number of memory elements to compare
       **/
      template<typename T>
      inline INT32 Compare(const T* in_pBuffer1, const T* in_pBuffer2, UINT32 in_iNbElems)
      {
         return memcmp(in_pBuffer1, in_pBuffer2, in_iNbElems * sizeof(T) );
      }

      //byte swapping util
      INT32   ByteSwapArray16( void *pData, UINT32 pNumBytes );
      INT32   ByteSwapArray32( void *pData, UINT32 pNumBytes );
      INT32   ByteSwapArray64( void *pData, UINT32 pNumBytes );

      INT32   ByteSwapArray16LEToHost( void *pData, UINT32 pNumBytes );
      INT32   ByteSwapArray32LEToHost( void *pData, UINT32 pNumBytes );
      INT32   ByteSwapArray64LEToHost( void *pData, UINT32 pNumBytes );

      INT32   ByteSwapArray16BEToHost( void *pData, UINT32 pNumBytes );
      INT32   ByteSwapArray32BEToHost( void *pData, UINT32 pNumBytes );
      INT32   ByteSwapArray64BEToHost( void *pData, UINT32 pNumBytes );
   }
}

#endif // #ifndef _GOLEM_MEMORY_
