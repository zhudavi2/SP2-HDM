/**************************************************************
*
* sp2_Military_event_handler.h
*
* Description
* ===========
*  Describes the interface of class GMilitaryEventHandler
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _SP2_MILITARY_EVENT_HANDLER_H_
#define _SP2_MILITARY_EVENT_HANDLER_H_

namespace SP2
{
   /*!
   * Handles Military events on Client side
   **/
   class GMilitaryEventHandler
   {
   public:
      GMilitaryEventHandler();
      ~GMilitaryEventHandler();

      void HandleUnitResearchUpdateRequest(SDK::GGameEventSPtr in_Event);
      void HandleRefuseMilitaryAction(SDK::GGameEventSPtr   in_Event);
      void HandleMissionOver(SDK::GGameEventSPtr in_Event);
      void HandleCombatOver(SDK::GGameEventSPtr in_Event);

      void HandleBombardmentNotification(SDK::GGameEventSPtr in_Event);



   protected:
   private:

   };

};//end namespace sp2

#endif //_SP2_MILITARY_EVENT_HANDLER_H_
