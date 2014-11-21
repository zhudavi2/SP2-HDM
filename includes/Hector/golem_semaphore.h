/**************************************************************
*
* golem_semaphore.h
*
* Description
* ===========
*  Describes & implements the class GSemaphore, which
*  encapsulate a semaphore synchronisation object.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef __GOLEM_SEMAPHORE_H__
#define __GOLEM_SEMAPHORE_H__

namespace Hector
{

   /*!
    * Semaphore class to synchronize threads. A semaphore
    * uses a counter to synchronize. When the counter is 0,
    * a wait operation blocks. When the counter is > 0, the
    * wait operation stops & decrements the counter by 1.
    */
   class GSemaphore
   {
   public:
      //! Default constructor
      GSemaphore(UINT32 in_iMaxCount = 0x7FFFFFFF);

      //! Destructor
      ~GSemaphore();

      //! Increment the count for the semaphore
      bool Increment(UINT32 in_iCount = 1);

      //! Waits that count is at least one and decrement semaphore
      void WaitAndDecrement();

   private:
      HANDLE         m_hSemaphore;
   };

}

#endif // #ifndef __GOLEM_SEMAPHORE_H__
