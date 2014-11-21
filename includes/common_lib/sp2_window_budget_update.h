/**************************************************************
*
* sp2_window_economic_health_update.h
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

#ifndef _GOLEM_EVENT_BUDGET_UPDATE_H_
#define _GOLEM_EVENT_BUDGET_UPDATE_H_


namespace SP2
{
   class GEconomicEventHandler;
   namespace Event
   {
      /*!
      * Event Sent from the Admin player to the server to request the 
      * beginning of the game
      **/
      class GBudgetUpdate: public SP2::Event::GRequestAckGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         friend class SP2::GEconomicEventHandler;
      public:

         // Network serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);

         REAL64 BudgetExpenseInfrastructure;
         REAL64 BudgetExpensePropaganda;
         REAL64 BudgetExpenseEnvironment;
         REAL64 BudgetExpenseHealthcare;
         REAL64 BudgetExpenseEducation;
         REAL64 BudgetExpenseTelecom;
         REAL64 BudgetExpenseGovernment;
         REAL64 BudgetExpenseImf;         
         REAL64 BudgetExpenseResearch;
			REAL64 BudgetExpenseTourism;

         INT16  CountryId;

      protected:
      private:
         GBudgetUpdate();
         ~GBudgetUpdate();
      };
   };//end namespace Event
};//end namespace SP2

#endif //_GOLEM_EVENT_BUDGET_UPDATE_H_
