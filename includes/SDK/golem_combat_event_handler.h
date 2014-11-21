/**************************************************************
*
* golem_CombatEventHandler.h
*
* Description
* ===========
*  Describes the interface of class GCombatEventHandler
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_SDK_CombatEventHandler_H_
#define _GOLEM_SDK_CombatEventHandler_H_

namespace SDK
{
   namespace Event
   {

      /*!
      * Handles most of the general events for the SP2 Client
      **/
      class GCombatEventHandler 
      {
      public:
         GCombatEventHandler();
         ~GCombatEventHandler();

         //! Handle the move troops event.
         bool HandleRecvSyncData(GGameEventSPtr in_Event);
      protected:
      private:
      };
   }
}

#endif //_GOLEM_SDK_CombatEventHandler_H_
