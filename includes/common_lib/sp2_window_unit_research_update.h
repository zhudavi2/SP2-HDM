/**************************************************************
*
* sp2_window_military_main_update.h
*
* Description
* ===========
* Military unit research window update request game event
*
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_UNIT_RESEARCH_UPDATE_H_
#define _GOLEM_EVENT_UNIT_RESEARCH_UPDATE_H_

namespace SP2
{
   class GMilitaryEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GUnitResearchUpdate: public SP2::Event::GRequestAckGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class SP2::GMilitaryEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         INT32 m_iCountryId;


         REAL64 m_fBudgetGround;
         REAL64 m_fBudgetAir;
         REAL64 m_fBudgetNaval;
         REAL64 m_fBudgetNuclear;

         REAL32 m_fMaxValues[EUnitCategory::ItemCount][EUnitDesignCharacteristics::ItemCount];
         REAL64 m_fBudget  [EUnitCategory::ItemCount][EUnitDesignCharacteristics::ItemCount];

         bool SetData;

      protected:
      private:
         GUnitResearchUpdate();
         ~GUnitResearchUpdate();

      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_UNIT_RESEARCH_UPDATE_H_
