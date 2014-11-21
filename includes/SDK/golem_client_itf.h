#include "golem_sdk_api.h"
#ifndef SDK_CLIENT_ITF_H
#define SDK_CLIENT_ITF_H


typedef bool (GCallbackHandlerItf::*CALLBACK_HANDLER_bool_rGDBQRH)(SDK::Event::GEventDBQueryResult::GDBQueryResultHolder &);


namespace SDK
{
   // Foward declaration
   namespace ConnectionLostReason
   {
      enum Enum;
   };

   namespace GUI
   {
      struct GEventData;
   };

   struct GListedServer
   {
      GString  m_sServerName;
      GString  m_sScenarioName;
      GString  m_sModName;
      INT32    m_iNbPlayers;
      INT32    m_iMaxPlayers;
      sockaddr m_ServerAdress;
      INT32    m_iPingTime;
      INT32    m_iServerPort;
      bool     m_bHasPassword;
      bool     m_bPublic;
      bool     m_bOfficial;
	  std::vector<std::string> m_ips; //public and local, all ips
   };

   class GJoshua;

   /*!
   * Represents a Game Client as viewed by the engine.  This interface
   * must be derivated to create a specific client class for each
   * type of game for example. for SP2, there is a class named SP2 Client.
   *
   *	The Initialize, Iterate and Shutdown functions must be created. So they can be called
   *	by the engine.
   *
   *	The Iterate function is called every frame by the game engine, the game Process can be placed
   *	there. Example: The processing of the game event queue must be placed there.
   **/
   class GOLEM_SDK_API GGameClientItf : public GGameProcessItf, public RGListener
   {
      friend class Event::GEventDBQueryResult;

      bool                       m_bGameSpyAvailable;
      bool                       m_bGameSpyComplete;

   protected:
      struct SQueryHandler
      {
         CALLBACK_HANDLER_bool_rGDBQRH   m_func;
         void * m_instance;
      };

      //! List of registered query handlers, by query_id
      stdext::hash_map<INT32, SQueryHandler> m_queryHandlers;

      //!Unique id number, to identify the client
      INT32	 m_iId;

      GString m_sPlayerName;

   public:

      GGameClientItf();
      virtual ~GGameClientItf();

      //! Initialize(): Inits the game client
      virtual GAME_MSG Initialize() = 0;

      //! Iterates, called every frame.
      virtual GAME_MSG Iterate(void* param) = 0;
      //! shutdown(): Shuts down the game client
      virtual GAME_MSG Shutdown()   = 0;

      //! Getters and setters for the client Id
      INT32   Id(){return m_iId;}
      void    Id(INT32);

      //!Database request result handler adding and calling
      bool AddDBRequestResultHandler(INT32 id, CALLBACK_HANDLER_bool_rGDBQRH function, void* instance);
      bool CallDBRequestResultHandler(Event::GEventDBQueryResult::GDBQueryResultHolder &);

      void OnUDPPacketReception(INT8, GOBuffer &, const sockaddr_in & l_From);
      virtual void OnUDPPacketReception(GOBuffer &, const sockaddr_in & l_From);

      //! Called when the client is connected to a server
      virtual void OnConnectedToServer(INT32 in_iPlayerID);
      virtual void OnServerDisconnected(ConnectionLostReason::Enum in_eReason);
      virtual void OnConnectionError(ConnectionLostReason::Enum in_eReason);
      void GetLocalServerListAsync();

	  virtual void OnNewServerListed(bool in_bIsLocal, const GListedServer & in_Server);

      virtual void OnChatMessage(INT32 in_iFromID, const GString & in_sMessage, bool in_bPrivate);
      virtual void OnGlobalHotkey(const GUI::GEventData &);
      virtual const GString & PlayerName() {return m_sPlayerName;};
      virtual void PlayerName(GString & in_sName) {m_sPlayerName = in_sName;};

	  //RGAPI
	  virtual void OnRGAPILoginCompleted(bool bSuccess, const GString & in_errMsg);
	  virtual void OnRGAPIRegisterCompleted(bool bSuccess, const GString & in_errMsg);

	  void onRGEvent(const unsigned int e, RGData::RGData *d);
	  void onRGAPIGameHostedEvent(RGData::RoomGameHosted* inData);
	  void onRGAPIGameDataUpdatedEvent(RGData::GameDataUpdated* inData);
     void GetRGAPIServerListAsync();
	  void RGAPIIterate();
	  void RGAPILogin(const GString& inUserName, const GString& inPassword);
	  void RGAPIDisconnect();
	  bool RGAPIPlayerIsLoggedIn();
	  void RAGPIRegisterAccount(const GString& inUserName, const GString& inPw, const GString& inEmail);
	  
private:
	//RGAPI
	RGInterface *m_RGAPI;
	GString m_sRGAPIPassword;
   GString m_sRGAPIEmail;
	bool InitRGAPI();
	void InitLanguage();
	void RGAPIUpdateServerData(imap& data_int, smap& data_str, std::vector<std::string> remote_ips);
   bool m_RGAPIConnected;
   bool m_RGAPILoggedIn;
   bool m_RGAPIIsInRegisterProcess;

   };

}
#endif