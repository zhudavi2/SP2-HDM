/**************************************************************
*
* sp2_window_battle_overview_unit_action.h
*
* Description
* ===========
* Military main window update request game event
*
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_BATTLE_OVERVIEW_UNIT_ACTION_UPDATE_H_
#define _GOLEM_EVENT_BATTLE_OVERVIEW_UNIT_ACTION_UPDATE_H_

namespace SP2
{
   class GMilitaryEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GBattleOverviewUnitActionUpdate: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class SP2::GMilitaryEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         INT32  m_iCountryId;
         bool   m_bRetreat;
         bool   m_bAttack[EUnitType::TotalCount];
         INT32  m_iCombatId;

      protected:
      private:
         GBattleOverviewUnitActionUpdate();
         ~GBattleOverviewUnitActionUpdate();

      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_BATTLE_OVERVIEW_UNIT_ACTION_UPDATE_H_
