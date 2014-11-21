/**************************************************************
*
* golem_threadpool.h
*
* Description
* ===========
*  This file contains classes needed to implement a pool of threads to perform
*  various tasks (with function pointers).  It automatically manage which thread can execute
*  the given task.
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_THREADPOOL_H_
#define _GOLEM_THREADPOOL_H_

namespace SDK
{

   /*!
   * Contains all the threads
   **/
   class GThreadPool
   {
      /*!
      * Represent one thread of the pool
      **/
      class GPoolThread : public GThread
      {
      public:
         void* m_pTaskInstance;
         void*                m_pTaskArgs;
         CALLBACK_HANDLER_bool_pvoid m_pTaskFunc;

         GPoolThread();
         ~GPoolThread();

         //!Thread function
         void Process(void* in_pArgs);
      };

      INT32        m_iNbThreads;
      GPoolThread* m_pThreadPool;

   public:
      GThreadPool(const INT32 in_iNbThreads,const GString& in_sAdditionalRef);
      ~GThreadPool();

      //! Starts a task into the thread pool
      bool StartTask(void* in_pInstance,CALLBACK_HANDLER_bool_pvoid in_pFunc,void* in_pArgs);


      static GString AdditionalReferenceName();

   protected:
   private:
      static GString m_sAdditionalRefName;
   };
}
#endif //_GOLEM_THREADPOOL_H_
