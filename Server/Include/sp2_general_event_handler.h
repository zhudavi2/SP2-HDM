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
 * Handles most of the general events for the Server
 **/
class GGeneralEventHandler 
{
public:
   GGeneralEventHandler( );
   ~GGeneralEventHandler();

   //! Handle the start game event
   bool HandleStartGameRequest(SDK::GGameEventSPtr in_Event);

   //! Handles the event when a player selects a country to control
   bool HandleGetRegionCharacteristic(SDK::GGameEventSPtr in_Event);
   bool HandleSetPlayerInfo(SDK::GGameEventSPtr in_Event);
   bool HandleGetPlayersList(SDK::GGameEventSPtr in_Event);   

   void HandleRequestCountryList(SDK::GGameEventSPtr in_Event);
   void HandleRequestCountryStaticData(SDK::GGameEventSPtr in_Event);

	void HandleFetchCountriesRelations(SDK::GGameEventSPtr in_Event);

   void HandleGetCountryRanks(SDK::GGameEventSPtr in_Event);
   void HandleGetCountriesRanks(SDK::GGameEventSPtr in_Event);

   void HandleRequestAvailableCountries(SDK::GGameEventSPtr in_Event);
   void HandleUpdateAdvisorSettings(SDK::GGameEventSPtr in_Event);

   void HandleGameOverAnswer(SDK::GGameEventSPtr in_Event);

   void HandleSetObjectives(SDK::GGameEventSPtr in_Event);
   void HandleCountryInfo(SDK::GGameEventSPtr in_Event);

   void HandleChangeGameSpeed(SDK::GGameEventSPtr in_Event);
	void HandleChangeAIAggressiveness(SDK::GGameEventSPtr in_Event);

   void HandleRelationHistoryFetch(SDK::GGameEventSPtr in_Event);
   void HandleGetCountryParties(SDK::GGameEventSPtr in_Event);

   void HandleSetGouvernementType(Event::GSetPlayerInfo *in_pPlayerInfo);

   void HandleResetGame(SDK::GGameEventSPtr in_Event);
   void HandleNewGame(SDK::GGameEventSPtr in_Event);
   void HandleLoadGame(SDK::GGameEventSPtr in_Event);
   void HandleSaveGame(SDK::GGameEventSPtr in_Event);

protected:
private:

};

};//end namespace sp2

#endif //_GOLEM_GENERALEVENTHANDLER_H_
