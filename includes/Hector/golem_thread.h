//SWIG-EXPORT 1
/********************************************************

golem_thread.h


Owner: Mathieu Tremblay

Thread basic class, to facilitate the threading


Warning: Si des erreurs de multi-threading apparaissent, 
s'assurer que le projet est en mode debug-multithreaded
ou multi-threaded (si en release mode)
history:


*********************************************************/
#ifndef _GOLEM_THREAD_
#define _GOLEM_THREAD_

#include "golem_types.h"

namespace Hector
{

   enum EThread_Status
   {
      e_Thread_Running = 0,
      e_Thread_Inactive,
      e_Thread_Suspended,
      e_Thread_Terminating,
   };

   //Time before the Terminate function forces the
   //thread to Terminate instead of waiting for it to
   //Terminate naturally
#define TERMINATE_TIMEOUT_SECS      10

   /******************************************* 
   *	GThread
   *		Multi platform class to implement a thread
   *
   *
   *	This class must be extended to be used.  The usage is to define the
   *	Process function to what we want the thread to do.  
   *
   *	The given function is then started, suspended, resumed or
   *	terminated with calls to the given GThread functions.
   *
   *********************************************/
   class GThread
   {
      void*    m_args ; //copy of the arguments to pass
   public:		
      GThread();					//Constructor
      ~GThread();					//~Destructor

      /***************
      *  Pure virtual function to extend.  Corresponds to the thread
      *  function that is executed when the Run function is called.
      *
      *  ---- IMPORTANT ----:
      *  The function Process() MUST do a check to the status (using 
      *  Status() or the m_status variable directly) and when  
      *  the status is e_Thead_Terminating it must end its loop and set
      *  the status to e_Thead_Inactive before exiting the loop.
      *  
      *  The reason to that is so the loop ends in a natural way.
      *  When the Terminate() function is called, if the thread didnt Resume
      *  in TERMINATE_TIMEOUT_SECS seconds, the thread will be forced
      *  to Terminate, ending the Process function immediately.
      *
      *  The Finalize() function can be implemented in the Child class
      *  to do some closing manipulation.  It is always called when a thread is
      *  terminated.
      ***************/
      virtual void Process(void* args) = 0;	//Pure virtual function to extend, thread function
      bool Run(void* args);
      bool Suspend();
      bool Resume();
      bool Terminate(bool Force_Terminate);

      /*!
      *
      **/
      INT32 Thread_Priority();
      bool  Thread_Priority(INT32 priority);

      EThread_Status Status();        
   protected:
      void Status(EThread_Status status);
      static DWORD WINAPI EntryPoint(void* pThis);
      EThread_Status m_status;     //Thread status
   private:
      bool Start_Thread();
      bool Force_Terminate();
      virtual void Finalize();    //Called after the Terminate function has been called
      //Corresponds to termination steps
      static void* args(void* pThis) 
      {               
         return ((GThread*)pThis)->m_args;
      }

#ifdef _WIN32
      INT32 m_handle;
#endif
   protected:

   };

}

#endif

//EOF

