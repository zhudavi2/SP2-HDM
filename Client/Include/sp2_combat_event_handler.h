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

#ifndef _GOLEM_CombatEventHandler_H_
#define _GOLEM_CombatEventHandler_H_

namespace SP2
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
      bool HandleMovingUnitPath(SDK::GGameEventSPtr in_Event);
   protected:
   private:
   };

};//end namespace sp2

#endif //_GOLEM_CombatEventHandler_H_
