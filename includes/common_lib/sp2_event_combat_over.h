/**************************************************************
*
* golem_combatover.h
*
* Description
* ===========
*  Describes the interface of class GCombatOver
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_COMBATOVER_H_
#define _GOLEM_COMBATOVER_H_




namespace SP2
{
   namespace Event
   {
      /*!
      * Event that is raised when a combat is over
      **/
      class GCombatOver : public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();


      public:

         bool Serialize(GIBuffer & out_Obj) const;
         bool Unserialize(GOBuffer & in_Obj);

         SP2::GArenaInfo m_ArenaInfo;
      protected:
      private:
      };

   };//end namespace event
};//end namespace SP2
#endif //_GOLEM_COMBATOVER_H_
