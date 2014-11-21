/**************************************************************
*
* sp2_window_economic_update.h
*
* Description
* ===========
*
* Owner
* =====
*  Francois Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_EVENT_ECONOMIC_UPDATE_H_
#define _GOLEM_EVENT_ECONOMIC_UPDATE_H_

namespace SP2
{
   class GEconomicEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GEconomicUpdate: public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class SP2::GPoliticEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
        
         REAL64 Expense;        
         REAL64 Income;         
         REAL64 RessRatio; 
         REAL64 AvailableMoney;           
         REAL32 EconomicModel;
			REAL32 EconomicHealth;
         INT16  CountryId;

      protected:
      private:
         GEconomicUpdate();
         ~GEconomicUpdate();
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_POLITIC_UPDATE_H_
