/**************************************************************
*
* golem_generaleventhandler.h
*
* Description
* ===========
*  Describes the interface of class GGeneralEventHandler
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_GENERALEVENTHANDLER_H_
#define _GOLEM_GENERALEVENTHANDLER_H_

namespace SP2
{
   /*!
   * Handles most of the general events for the SP2 Client
   **/
   class GGeneralEventHandler 
   {
   public:
      GGeneralEventHandler();
      ~GGeneralEventHandler();

      //! Handle the start game event
      bool HandleStartGame(SDK::GGameEventSPtr in_Event);
      //! Handle the event that sends all the available countries
      bool HandleReceiveAvailableCountries(SDK::GGameEventSPtr in_Event);
      //! Handle the event that notifies something to the client from the server
      bool HandleNotifyClient(SDK::GGameEventSPtr in_Event);
      
		//! Handle the fetching of all relations
		void HandleRelationsCountries(SDK::GGameEventSPtr in_Event);

		//! Handle Random Events
		bool HandleRandomEvents(SDK::GGameEventSPtr in_Event);

      bool HandleGetRegionsCharacteristicResult(SDK::GGameEventSPtr in_Event);

      //! Handle the event that received a list of player
      bool HandleGetPlayersList(SDK::GGameEventSPtr in_Event);

      void HandleReceiveCountryList(SDK::GGameEventSPtr in_Event);
      void HandleReceiveCountryStaticData(SDK::GGameEventSPtr in_Event);
      void HandleErrorCountryStaticData(SDK::GGameEventSPtr);

      void HandleCountryRanksResult(SDK::GGameEventSPtr in_Event);
      void HandleCountriesRanksResult(SDK::GGameEventSPtr in_Event);

      void HandleSetGameObjectives(SDK::GGameEventSPtr in_Event);

      //! Handles the event that sends initial data from the server to the client
      void HandleSendData(SDK::GGameEventSPtr in_Event);

      //! Handles the event that sends updated data from the server to the client
      void HandleUpdateData(SDK::GGameEventSPtr in_Event);

      //! Handles the event that sends a update on a treaty to the client
      void HandleUpdateTreaty(SDK::GGameEventSPtr in_Event);

		//! Handles the event that sends a update on a war to the client
		void HandleUpdateWar(SDK::GGameEventSPtr in_Event);

      //! Handles the event that sends unit value to update from the server to the client
      void HandleUpdateUnitValue(SDK::GGameEventSPtr in_Event);

      //! Handles the event that indicate a country was completery conquered (destroyed)
      void HandleConquerCountry(SDK::GGameEventSPtr in_Event);

		//! Handles the event that indicate a news has just arrived 
      void HandleNews(SDK::GGameEventSPtr in_Event);

      //! should be in another class named GMilitaryEventHandler...
      void HandleStrategicWarfareNotify(SDK::GGameEventSPtr in_Event);

      //! Handles the event when the client receives the client country data
      void HandleSynchronizeCountryData(SDK::GGameEventSPtr in_Event);

      //! Handle the event when the advisor send you a plan
      void HandleAdvisorPlan(SDK::GGameEventSPtr in_Event);

      //! Handle the event when the game is over
      void HandleGameOver(SDK::GGameEventSPtr in_Event);

      //! Handle Objective Completed
      void HandleObjectiveCompleted(SDK::GGameEventSPtr in_Event);

      //! Handle the event for history markers
      void HandleHistoryMarkers(SDK::GGameEventSPtr in_Event);

      void HandleCountryInfo(SDK::GGameEventSPtr in_Event);

      //! Handles the reception of relation history date
      void HandleRelationHistoryReception(SDK::GGameEventSPtr in_Event);

      //! Handles for the party selection when you start a game
      void HandleGetCountryParties(SDK::GGameEventSPtr in_Event);

      void HandleProgress(SDK::GGameEventSPtr in_Event);
      void HandleAcknowledge(SDK::GGameEventSPtr in_Event);

      void HandleResetGame(SDK::GGameEventSPtr);

      void HandleNewComplete(SDK::GGameEventSPtr in_Event);
      void HandleLoadComplete(SDK::GGameEventSPtr in_Event);
      void HandleSaveComplete(SDK::GGameEventSPtr in_Event);

      //! Handle server registering its password
      void HandleSetPlayerInfo(SDK::GGameEventSPtr in_Event);

   protected:
   private:
		GString ReplaceNextPattern(GString in_sSentence, GString in_ReplacingString, GString in_sPattern) const;
   };

};//end namespace sp2

#endif //_GOLEM_GENERALEVENTHANDLER_H_
