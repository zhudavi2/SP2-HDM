/**************************************************************
*
* sp2_game_control_events.h
*
* Description
* ===========
*  Describes game events classes to manage which player controls what
*   - Send the list of available countries
*   - Advise the server that one given country is selected
*   - Authorize or not the country selection
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_GAME_CONTROL_EVENTS_SP2__
#define _GOLEM_GAME_CONTROL_EVENTS_SP2__

namespace SP2 
{ 
   namespace Event 
   {

      /*!
      * This is a game event that is sent from the server to its newly
      * connected clients so it can pick a country that it wants to control
      **/
      class GSendAvailableCountries : public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;

      private:

         vector<UINT16> m_vIDs;

         GSendAvailableCountries();
         ~GSendAvailableCountries();

         static SDK::GGameEventSPtr New();
      protected:
      public:

         //Serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);


         //Set the available countries
         void AvailableCountries(vector<UINT16> in_vIDs);
         vector<UINT16> AvailableCountries();
      };
      //! SendAvailableCountries Creation function      
      
      class GGetRegionsCharacteristic : public SDK::Event::GGameEvent
      {         
         friend class GClient;
         friend class GServer;       
         GGetRegionsCharacteristic() {};
         static SDK::GGameEventSPtr New() {return SDK::GGameEventSPtr(new GGetRegionsCharacteristic);}
      public:
         ECharacteristicToHighlight::Enum Characteristic;         
         UINT32 SelectedCountry;
         virtual bool Serialize(GIBuffer &) const;
         virtual bool Unserialize(GOBuffer &);
      };

      class GGetRegionsCharacteristicResult : public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         GGetRegionsCharacteristicResult() {};
         static SDK::GGameEventSPtr New() {return SDK::GGameEventSPtr(new GGetRegionsCharacteristicResult);}
      public:
         ECharacteristicToHighlight::Enum Characteristic;
         vector<REAL64> Results;
         vector<REAL64> m_vAdditionalResults;
         virtual bool Serialize(GIBuffer &) const;
         virtual bool Unserialize(GOBuffer &);
      };

      /*!
      * This is a game event that is sent from the client to the server
      * to Advise The server that a player has change its name or its selected country
      **/
      class GSetPlayerInfo : public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New();
         GSetPlayerInfo();

      public:

         struct PlayerInfo
         {
            INT32                ClientID;
            INT32                CountryID;
            INT32                PartyID;
            GString              PlayerName;
            SDK::GEPlayerStatus  PlayerStatus;

         };

         PlayerInfo m_PlayerInfo;

         //Serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
      };

      class GGetPlayersList : public SDK::Event::GGameEvent
      {
         friend class GClient;
         friend class GServer;
         static SDK::GGameEventSPtr New() {return SDK::GGameEventSPtr(new GGetPlayersList);}      
         GGetPlayersList() {m_iPlayerAdminID = 0;}

      public:

         void  AdminPlayerID(INT32 in_iAdminID);
         INT32 AdminPlayerID(void);

         struct Player
         {
            INT32                ClientID;
            INT32                CountryID;
            GString              PlayerName;
            SDK::GEPlayerStatus  PlayerStatus;

         };
      
         INT32                   m_iPlayerAdminID;

         vector<Player> PlayersList;

         //Serialization functions
         virtual bool Serialize(GIBuffer&) const;
         virtual bool Unserialize(GOBuffer&);
      };

   }; //End of namespace Event
}; //End namespace SP2

#endif //_GOLEM_GAME_CONTROL_EVENTS_SP2__
