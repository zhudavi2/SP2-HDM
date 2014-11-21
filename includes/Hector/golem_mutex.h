/**************************************************************
*
* golem_mutex.h
*
* Description
* ===========
*  Describes & implements the class GMutex
*
* Owner
* =====
*  Mathieu Tremblay, Germain Sauve
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef __GOLEM_MUTEX__
#define __GOLEM_MUTEX__

namespace Hector
{

/*!
 * Mutual exclusion class to synchronize threads
 */
class GMutex
{
public:
   //! Default constructor
   GMutex();

   //! Destructor
   ~GMutex();

   //! Locks the mutex, if it is locked, waits for the mutex to
   void Lock();
   //Unlock and then locks it

   //! Unlock the mutex
   void Unlock();

private:
   //Members
   CRITICAL_SECTION  m_Mtx;
};

}

#endif // #ifndef __GOLEM_MUTEX__
