/**************************************************************
*
* sp2_military_event_handler.h
*
* Description
* ===========
*  Describes the interface of class GEconomicEventHandler
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_MILITARY_EVENT_HANDLER_H_
#define _GOLEM_MILITARY_EVENT_HANDLER_H_

namespace SP2
{

   /*!
   * Handles economic events on the Server
   **/
   class GMilitaryEventHandler : public Hector::DesignPattern::GObserver
   {
   public:
      GMilitaryEventHandler();
      ~GMilitaryEventHandler();

      bool HandleStrategicWarfareLaunch(SDK::GGameEventSPtr in_Event);

      //! Handle when a SDK::Combat is over (do something)
      void HandleCombatOver(SDK::Combat::EBattleStatus::Enum in_Status,SDK::Combat::GArena* in_pArena);

      //! Handle destination reached
      bool HandleMoveUnit(SDK::GGameEventSPtr in_Event);

      bool HandleStatusChange(SDK::GGameEventSPtr in_Event);

		void HandleOneClickInvade(SDK::GGameEventSPtr in_Event);

      void HandleAddUnitDesign(SDK::GGameEventSPtr in_Event);

      void HandleMilitaryMainUpdateRequest(SDK::GGameEventSPtr in_Event);
      void HandleUnitResearchUpdateRequest(SDK::GGameEventSPtr in_Event);
      void HandleBattleOverviewUnitActionUpdate(SDK::GGameEventSPtr in_Event);
		void HandleUpdateWarState(SDK::GGameEventSPtr in_Event);

      void HandleCellCreate(SDK::GGameEventSPtr in_Event);
      void HandleCellUpdate(SDK::GGameEventSPtr in_Event);
      void HandleNewMission(SDK::GGameEventSPtr in_Event);

      void HandlePerformOffshoreBombardment(SDK::GGameEventSPtr in_Event);

      //! Notification received when the subject change, (actually the unit manager)
      void OnSubjectChange(DesignPattern::GSubject&  in_Subject,const DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg  );
         

   protected:
   private:

   };

};//end namespace sp2

#endif //_GOLEM_MILITARY_EVENT_HANDLER_H_
