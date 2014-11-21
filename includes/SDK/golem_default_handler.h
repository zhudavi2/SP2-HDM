#include "golem_sdk_api.h"
/**************************************************************
*
* golem_sdkeventshandler.h
*
* Description
* ===========
*  Describes the interface of class GDefaultHandler
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SDKEVENTSHANDLER_H_
#define _GOLEM_SDKEVENTSHANDLER_H_


namespace SDK
{
   namespace Event
   {
      /*!
      * Game Event Handler for the SDK game events
      * As for example a Database query, a system game event etc.
      **/
      class GOLEM_SDK_API GDefaultHandler 
      {
      public:
         GDefaultHandler();
         ~GDefaultHandler();

         //! Handles When a database query has to be executed
         bool HandleDBQuery(GGameEventSPtr);

         //! Handles when the results of a database query are received
         bool HandleDBQueryResults(GGameEventSPtr);

         //! Handles System game events
         bool HandleSystemEvents(GGameEventSPtr);
         bool HandleChatEvents(GGameEventSPtr);

         //! Function that waits for the answer to a sync request
         DB::EError WaitForSyncDBRequestResults(UINT32, vector<DB::GTable*> &);
      protected:
      private:

         GEventDBQueryResult::GDBQueryResultHolder * m_Holder;
         UINT32 m_HolderQueryID;
         GMutex m_HolderMutex;
      };
   }
}
#endif //_GOLEM_SDKEVENTSHANDLER_H_
