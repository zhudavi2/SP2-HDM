#include "golem_sdk_api.h"
/**************************************************************
*
* golem_joshua.h
*
* Description
* ===========
*  This is the class that reprensents the Game Enving.  It will put
*  all utility classes together so it creates a game engine.
*
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _GOLEM_JOSHUA__
#define _GOLEM_JOSHUA__
#pragma warning(disable: 4786)

#include "golem_mod.h"
class GSteamWrapper;
class GSteamWrapperServer;

namespace SDK
{

   #define SAFE_SHUTDOWN(p) { if(p) p->Shutdown(); }

   #define REGISTER_GAME_EVENT(in_Classname, in_pFuncSTD, in_pInstSTD) g_Joshua.RegisterGameEventInternal(typeid(in_Classname).raw_name(), in_Classname::New, (CALLBACK_HANDLER_bool_spGE)in_pFuncSTD, in_pInstSTD,    NULL,                                    NULL)
   #define REGISTER_GAME_EVENT_AI(in_Classname, in_pFuncSTD, in_pInstSTD,in_pFuncAI,in_pInstAI) g_Joshua.RegisterGameEventInternal(typeid(in_Classname).raw_name(), in_Classname::New, (CALLBACK_HANDLER_bool_spGE)in_pFuncSTD, in_pInstSTD,    (CALLBACK_HANDLER_bool_spGE) in_pFuncAI, in_pInstAI)
   #define CREATE_GAME_EVENT(in_Classname) g_Joshua.CreateGameEventInternal  (typeid(in_Classname).raw_name())
   #define GAME_EVENT_TYPE(in_Classname) g_Joshua.GameEventKeyInternal(typeid(in_Classname).raw_name())

   //======================================
   //  GAME CONSTANTS
   //======================================
   CWSTR           JOSHUA_LOG_FILE_CLIENT      = L"Joshua_Client.log";
   CWSTR           JOSHUA_LOG_FILE_SERVER      = L"Joshua_Server.log";
   const INT32     SERVER_NB_ITERATIONS_BY_SEC   = 30;
   CWSTR           ERROR_CANNOT_FIND_MOD       = L"Cannot find a .glm file in the mod folder :";


   // SDK Version INFO
   const UINT8 SDK_VERSION_MAJOR = 1;
   const UINT8 SDK_VERSION_MINOR = 1;
   const UINT8 SDK_VERSION_REVISION = 0;

   namespace EUdpPacketType { enum Enum
   {
      SDKPacket = 0,
      CustomPacket = 1
   };}

   enum EJOSHUA_MSG
   {
      //GENERAL MESSAGES
      JOSHUA_ERROR = 0,
      JOSHUA_OK,

      JOSHUA_SPAWN_SERVER_ERROR,

      //INIT ERRORS
      JOSHUA_NO_MOD_PRESENT,
      JOSHUA_ALREADY_INITIALIZED,
      JOSHUA_NOT_INITIALIZED,
      JOSHUA_INIT_ERROR,
      JOSHUA_INIT_SERVER_ERROR,
      JOSHUA_CREATE_SERVER_ERROR,
      JOSHUA_REGISTER_EVENTS_ERROR,
      JOSHUA_ACCEPT_CONNECTIONS_ERROR,
      JOSHUA_SERVER_ALREADY_RUNNING,

      //COMMUNICATION ERRORS
      //JOSHUA_CREATE_PIPE_ERROR,
      //JOSHUA_LOCAL_CONNECTION_PIPE_ERROR,
      JOSHUA_NOT_CONNECTED,

      //SHUTDOWN ERRORS
      JOSHUA_SHUTDOWN_ERROR,

      //GAMEPLAY ERRORS
      JOSHUA_INVALID_STATE,
      JOSHUA_INVALID_PLAYER,
      JOSHUA_RAISE_EVENT_ERROR,

      //MISCELLANOUS ERRORS
      JOSHUA_INVALID_FUNCTION_POINTER,
      JOSHUA_ACTIVE_PLAYERS_DOESNT_EXIST,
      JOSHUA_ACTIVE_PLAYER_ALREADY_HUMAN_CONTROLLED,

      //Last joshua msg
      JOSHUA_NOT_IMPLEMENTED = 9999
   };

   namespace ConnectionLostReason
   {
      enum Enum
      {
         CantConnect = 0,
         ConnectionLost,
         ConnectionTimeOut,
         InvalidSDKVersion,
         InvalidModVersion,
         InvalidMod,
         InvalidAuthentification,
         InvalidPassword,
         ServerIsFull,
         KickedFromServer,
         CantStartServer,
         TerminatedByHost,
         DisconnectedByUser,
         BanFromServer,
         TotalReason
      };
   };

   /*!
   * To specify the engine status
   **/
   enum EJOSHUA_STATUS
   {
      JSTATUS_NOT_INITIALIZED = 0,
      JSTATUS_INITIALIZED,
      JSTATUS_RUNNING,
      JSTATUS_SHUTTING_DOWN,
      JSTATUS_SHUTTED_DOWN
   };


   //=================================================
   //           FORWARD DECLARATIONS
   //=================================================
   class GGameClientItf;
   class GGameServerItf;
   class GPlayer;   

   namespace GUI{class GManager;};
   //=================================================
   //           TYPEDEFS
   //=================================================
   typedef std::map<INT32, GPlayer*> GPlayers;

   struct GSocketAddr
   {
      GSocketAddr(const sockaddr_in& in_Addr)
      {
         m_Addr = in_Addr;
      }

      sockaddr_in m_Addr;

      bool operator < (const GSocketAddr& in_Other) const
      {
         // Beware, this CANNOT work with IPv6
         return *( (INT64*) &m_Addr) < *( (INT64*) &(in_Other.m_Addr) );
      }

      operator size_t () const
      {
         return m_Addr.sin_addr.S_un.S_addr;
      }
   };

#define IF_RETURN_LOG(in_Cond,in_Return,in_sLog){if(in_Cond) {g_Joshua.Log(in_sLog); return in_Return;} }




   /*!
   * Game engine class, holds all the utility objects
   * needed for the game, automatically iterates the game client/server
   * and calls the AI.
   **/
   class GOLEM_SDK_API GJoshua
   {
      friend int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
      friend class   GGenericNetworkClient;
      friend class   GGameClientItf;
      friend class   GGameServerItf;
      friend class   Event::GDefaultHandler;
      friend class   Event::GSystemGameEvent;
      static unsigned long __stdcall AcceptConnections(void*);
      static unsigned long __stdcall ReceiveNetworkMessagesFromServer(void*);
      static unsigned long __stdcall ReceiveUDPPacketsThread(void*);
      static unsigned long __stdcall ConnectAsynchronously(void*);

   private:

      UINT8       m_Info[16];
      GSteamWrapper *mSteamWrapper;
      GSteamWrapperServer *mSteamWrapperServer;

      struct GConsoleCommandStruct
      {
         GString Command;
         GString SyntaxIS;
         GString SyntaxHelpMessage;
         void * HandlingInstance;
         CALLBACK_HANDLER_GS_crGS_cvrGS  HandlingFunction;
      };

      hash_map<GString, GConsoleCommandStruct> m_ConsoleCommandHandlers;
      set<GString> m_ConsoleCommandCalls;

      GString ConsoleSystemCommandsHandler(const GString &, const vector<GString> &);

      /*!
      * Small Container  - joshua embedded
      * to hold a game event Id and a creation Function
      * Used in joshua to associate a game event, an Id and
      * a string identifying the game event
      **/
      class GRegisteredGameEvent
      {
      public:
         GRegisteredGameEvent()
         {
            m_iType = -1;
            m_pGameEventCreationFunc      = NULL;
            m_pGameEventHandlerFuncStd    = NULL;
            m_sName                       = "undefined";
            m_pHandlingInstanceStd        = NULL;
            m_pGameEventHandlerFuncAI     = NULL;
            m_pHandlingInstanceAI         = NULL;
         }
         INT32                            m_iType;
         GString                          m_sName;
         //Pointer to the game event Creation Function
         CREATE_GAME_EVENT_FUNC           m_pGameEventCreationFunc;
         //Pointer to the handling instance
         void*                            m_pHandlingInstanceStd;
         //Pointer to a method handling function
         CALLBACK_HANDLER_bool_spGE       m_pGameEventHandlerFuncStd;

         //AI Handling methods
         CALLBACK_HANDLER_bool_spGE       m_pGameEventHandlerFuncAI;
         void*                            m_pHandlingInstanceAI;

      };

      typedef stdext::hash_map<GString,INT32>                   GRegisteredGEKeyByName;
      typedef stdext::hash_map<INT32,GRegisteredGameEvent>  GRegisteredGEByKey;

      //==========================
      //  THREAD HANDLES
      //==========================
      UINT32                    m_iThreadAcceptFromNetwork;//Handle thread accepts connections
      UINT32                    m_iThreadReceiveMsgFromServer;
      UINT32                    m_iThreadReceiveUDP;
      bool                      m_bUDPStarted;
      bool                      m_bUDPValid;

      //==========================
      //  PROCESS HANDLES
      //==========================
      HANDLE                  m_SpawnedServerHandle;
      HANDLE                  m_ServerMutexHandle;
      bool                    m_bSpawnedServer;

      //==========================
      //    SYSTEM MEMBERS
      //==========================
      GFile                      m_ServerDll;
      GFile                      m_ClientDll;
      GLogger*                   m_pLogger;
      GMutex                     m_LogMutex;
      EJOSHUA_STATUS             m_Status;
      bool                       m_bRunningAsServer;
      bool                       m_bConnectedToServer;   // Is the client connected to a server?
      bool                       m_bJustDisconnected;
      GSimpleNetwork             m_NetworkTool;          // TCP/IP Utility class
      sockaddr_in                m_ServerAddress;        // Address of the connected server
      INT32                      m_iListenPort;          // Port on which the server listens
      bool                       m_bWaitingServerAck;    // True when server is espected to send and ack udp packet

      GSemaphore                 m_ConnectionQueue;
      bool                       m_bAsyncConnect;
      bool                       m_bCancelConnect;
      bool                       m_bJustConnected;
      ConnectionLostReason::Enum m_eLastConnectionError;   

      GString                    m_sServerIP;
      INT16                      m_iServerPort;
	  std::vector<std::string>   m_vsPublicAndLocalIPs;
	  INT16						 m_iPublicAndLocalIPs_currentIndex;

      // JMercier : To move in mod struct
      GString                    m_sModPath;
      GString                    m_sModulePath;
      GMutex                     m_RaiseEventMutex;
      GMutex                     m_PlayerListMutex;
      GRegisteredGEKeyByName     m_RegGENameKeyMap;      // Map a registered Game Event name and the key
      GRegisteredGEByKey         m_RegGEKeyMap;          // Map between the registered Game Event key and its construction function

      //DLL Loaders
      GDllLoader                 m_ClientLoader;   // Client dll importer
      GDllLoader                 m_ServerLoader;   // Server dll importer
      GDllLoader                 m_DBLoader;       // Database dll importer
      GDllLoader                 m_RendererLoader; // Renderer dll importer
      GDllLoader                 m_SoundLoader;    // Sound dll importer

      // Variables needed by the AI
      REAL64                     m_fAIIterationPeriod;   // minimum amount of clock time betwen AI iterations
      REAL64                     m_fNextAIIteration;     // when the next AI iteration should occur


      INT16                      m_iPing;          // Gives the ping to the server, in msec

      GPlayers::iterator         m_LastPlayerIterated;   // identifies the last player iterated in the EHE


      //! unique identifier for the main thread (to know if we are in the main thread)
      DWORD                      m_iMainThreadID;

      //! Get the main thread Id
      DWORD                      MainThreadID();
 
      //  MAIN MODULES COMPOSING THE ENGINE
      //--------------------------------------
      GGameClientItf*            m_pClient;        // Game client instance
      GGameServerItf*            m_pServer;        // Game server instance
      GUI::GManager*             m_pGUIManager;    // GUI Manager
      GEventManager*             m_pEventManager;  // Event manager
      GDatabase*                 m_pDatabase;      // Database engine
      CGFX_Renderer_Itf*         m_pRenderer;      // Renderer
      CSFX_Player_Itf*           m_pSoundPlayer;   // Sound
      GConsole*                  m_pConsole;       // Console
      GFileManager*              m_pFileManager;   // File Manager
      GThreadPool*               m_pThreadPool;    // Pool of threads to perform tasks
      GStringTable*              m_pStringTable;   //String table
      SDK::Combat::GUnitManager* m_pUnitManager;
      GUDPStream                 m_UDPStream;

      //==========================
      // GAMEPLAY MEMBERS
      //==========================
      REAL64               m_fClock;                     //REAL64 Clock (precise by the second)
      REAL64               m_fStartClock;                //Needed to set the clock
      GTimeCounter         m_TimeCounter;                //Needed to manage time (ping, clock etc.)

      REAL64               m_fGameTime;                  //Game time, in the unit the game needs it
      REAL64               m_fGameSpeed;                 //Game speed, in game time units per seconds
      REAL64               m_fStartGameTime;             //Used to compute game time from engine clock
      REAL64               m_fStartGameClock;            //Used to compute game time from engine clock

      bool                 m_bAutoGameTimeSpeedSync;     // True if automatic game time & speed synchronization is activated
      UINT32               m_iSyncInterval;              // Interval (in iterations) for the auto time sync
      UINT32               m_iSyncIterCount;             // Number of iterations since the last time sync
      UINT32               m_iSyncID;                    // Next sync ID
      list<pair<UINT32, REAL64> >   m_SyncRequests;      // List of sync requests sent by client
      GMutex               m_SyncMutex;                  // Locked when synch'ing time
      REAL64               m_fSyncGameTimeClock;         // Time at which the game time must be reevaluated
      REAL64               m_fSyncGameTimeValue;         // Value of the game time at the reevaluation
      REAL64               m_fSyncGameSpeedValue;        // Value of the game speed at the reevaluation
      bool                 m_bSyncResetPing;             // Indicate next ping value will be the first one
      bool                 m_bSyncGameTimeUpdate;        // True if game time must be updated before iteration

      INT32                m_iNextID;                    //to allocate players id
      INT16                m_iMaxPlayers;                //Maximum number of players on the server
      INT32                m_iAdminPlayer;               //Id of the Admin player
      map<INT32,INT32>     m_MapModIDActivePlayerID;     //Associate a ModID with a PlayerID
      GPlayers             m_ActivePlayers;              //Active(playing) players map
      GPlayers             m_HumanPlayers;               //A map of all the players that joined this server.
      vector<pair<GPlayer*, sockaddr_in> > m_vPendingPlayers;//Players that connected but must wait end of iteration to join
      hash_map<GSocketAddr, INT32>  m_ClientAddrToPlayerID; //Convert an incoming network address into a human player ID
      hash_map<UINT32, sockaddr_in> m_vReceivedUDPConnect;  //Map between a client ID and its UDP adress
      Event::GDefaultHandler     m_DefaultEventHandler;  //Game Event Handler for the SDK built in events


      //Server Related Info for remote spawning
      GString              m_sServerName;
      GString              m_sPassword;
      UINT32               m_iMinPing;
      UINT32               m_iTimeLimit;
      bool                 m_bPrivate;
      bool                 m_bDependant;
      GString              m_sModCommandLine;		
      bool                 m_bIterateAI;                 //Specify if AI must Iterate or not

      //==========================
      // PRIVATE METHODS
      //==========================
      EJOSHUA_MSG          RegisterEvents();           // Register systems events
      void                 OnTerminate(GDynamicData*);  // Event handler for QUIT_PROGRAM

      //Init when client
      EJOSHUA_MSG          InitAsClient(GEngineInfo* in_pEngineInfo,
                                        CGFX_Renderer_Init_Mode in_InitMode);
      EJOSHUA_MSG          InitAsServer();      //Initialize game when server
      inline EJOSHUA_MSG   IterateAsClient();      //Iterate when running as the client
      inline EJOSHUA_MSG   IterateAsServer();      //Iterate when running as the server
      EJOSHUA_MSG          CreateServer(bool in_bMultiplayer,bool in_bLan);      //Creates a server
      EJOSHUA_MSG          AcceptConnections(INT32 in_iPort); //Accept TCPIP connections
      EJOSHUA_MSG          CreateLocalCommunicationPipes();                         //Creates the 2 local comm pipes
      void                 SpawnedServer(bool in_bSpawnedServer);

      bool ConnectToServer();
      void NotifyConnectionToClient(bool in_bConnected);

      //! Send a game time synchronization request to the server
      void RequestGameTimeSync();

      //! Respond to a game time synchronization request from a client
      void RespondToTimeSync(const class GGameTimeSyncRequest& in_SyncRequest, const sockaddr_in &in_From);

      //! Update client game time & speed in response to a time sync packet
      void ReceiveTimeSync(const class GGameTimeSyncResponse& in_Response);

      //! Update client game time from a server time change
      void ReceiveTimeChange(const class GGameTimeSyncTimeChange& in_Change);

      //! Update client game speed from a server speed change
      void ReceiveSpeedChange(const class GGameTimeSyncSpeedChange& in_Change);

      //Converts a byte stream to a game event
      GGameEventSPtr StreamToGameEvent(const GSPtrINT8 in_StreamBuffer, INT32 in_iStreamSize);
      //Converts a game event to a byte stream
      GSPtrINT8 GameEventToStream(const GGameEventSPtr in_Event, INT32& out_iSize);

      //========================================
      // GAMEFLOW METHODS (INIT, ITERATE, SHUTDOWN)
      //========================================
      //Initializes the game engine
      EJOSHUA_MSG Init(
         const GString           &in_sModulePath,      //Path where the modules are located (renderer,db,sound)
         GEngineInfo*            in_pEngineInfo,       //Filled engine info structure
         CGFX_Renderer_Init_Mode in_RendererMode,      //Init mode of the renderer
         bool                    in_bRunAsServer,      //Server mode or client mode
         GMod&                   in_CurrentMod,        //Current mod
         const GString           &in_sModCommandLine); //Mod specific command line

      inline EJOSHUA_MSG        Iterate();                     //Calls the correct Iterate function
      void                    IterateAI();               //Mid-level Iterate to determine if the EHE does something
      EJOSHUA_MSG               Shutdown();                    //Shutdowns the game engine

      //-----------------------------------------
      // Construction methods
      //-----------------------------------------
      GJoshua  ();
      GJoshua  (const GJoshua&);
      ~GJoshua  ();



      //! Adding Mod information
      GMod              m_CurrentMod;      
      list<GString>     m_bannedMacAddressList;
      
   public:

      // Mod Info
      const GMod& CurrentMod(void);


      //! Gets the unique Joshua's instance
      static GJoshua&   Instance();


      //==========================
      // PUBLIC MEMBER ACCESSORS
      //==========================

      //! Game client getter
      /*!
       * Gets a pointer to the game client
       * @return GGameClientItf*: A pointer to the game client, NULL if no game client
       **/
      inline GGameClientItf*     Client      () { return m_pClient; }

      //! Gets the Server
      /*!
       * Get a pointer to the Server
       * @return GGameServerItf*: A pointer to the Server, NULL if no Server
       **/
      inline GGameServerItf*     Server      () { return m_pServer; }

      //! Game Console getter
      /*!
       * Gets a pointer to the game Console
       * @return GConsole*: A pointer to the game Console, NULL if no game client
       **/
      inline GConsole*           Console     () { return m_pConsole; }
      void                       Console     (GConsole*);

      //! Get the logger
      /*!
       * Gets Joshua's logger
       * @return GLogger*: A pointer to Joshua's logger
       **/
      inline GLogger*            Logger      () { return m_pLogger; }

      //! Gets the database
      /*!
       * Get a pointer to the DB
       * @return GDatabase*: A pointer to the database, NULL if no database
       **/
      inline GDatabase*          Database    () { return m_pDatabase; }

      //! Gets the EventManager
      /*!
       * Get a pointer to the EventManager
       * @return GEventManager*: A pointer to the EventManager, NULL if no EventManager
       **/
      inline GEventManager*      EventManager() { return m_pEventManager; }

      //! Gets the FileManager
      /*!
       * Get a pointer to the FileManager
       * @return GFileManager*: A pointer to the FileManager, NULL if no FileManager
       **/
      inline GFileManager*       FileManager () { return m_pFileManager; }

      //! Gets the GUIManager
      /*!
       * Get a pointer to the GUIManager
       * @return GUI::GManager*: A pointer to the GUIManager, NULL if no GUIManager
       **/
      inline GUI::GManager*      GUIManager  () { return m_pGUIManager; }

      //! Gets the Renderer
      /*!
       * Get a pointer to the Renderer
       * @return CGFX_Renderer_Itf*: A pointer to the Renderer, NULL if no Renderer
       **/
      inline CGFX_Renderer_Itf*  Renderer    () { return m_pRenderer; }

      //! Gets the SoundPlayer
      /*!
       * Get a pointer to the SoundPlayer
       * @return CSFX_Player_Itf*: A pointer to the SoundPlayer, NULL if no SoundPlayer
       **/
      inline CSFX_Player_Itf*    SoundPlayer () { return m_pSoundPlayer; }

      //! Gets the time counter
      /*!
       * Get a reference to the time counter
       * @return GTimeCounter&: A reference to the time counter
       **/
      inline GTimeCounter&       TimeCounter () { return m_TimeCounter; }

      //! Gets the unit manager
      /*!
       * Get a reference to the unit manager
       * @return Combat::GUnitManager&: A reference to the unit manager
       **/
      inline Combat::GUnitManager& UnitManager(){ return *m_pUnitManager; }

      //==========================
      // PUBLIC METHODS
      //==========================
      const GString&    ModPath() const;                    //!< Gets the modpath
      void              ModPath(const GString &in_Path);    //!< Sets the modpath
      const GString&    ModulesPath() const;                //!< Gets the modules path
      void              ModulesPath(const GString &in_Path);//!< Sets the modules path
      INT32             NextID();                           //!< Gives the next player id
      EJOSHUA_STATUS    Status() const{return m_Status;}    //!< Gets the engine status
      void              Status(EJOSHUA_STATUS in_Stat){m_Status = in_Stat; }
      bool              Log(const GString& in_sMsg,EMessageType in_eType = MSGTYPE_MSG); //Logs a msg
      bool              Log(const GString& in_sMsg,const GString& in_sLocation,EMessageType type = MSGTYPE_MSG); //Logs a msg
      bool              Quit();
      bool              ExecuteConsoleCommand(const GString& in_Command);
      INT32             AdminPlayerID();
      void              AdminPlayerID(INT32 in_iAdminPlayerID);
      void              Banned(INT32 in_iClientID);
      void              ClearBanList(void);
      void              SaveBanPlayerList(void);
      void              LoadBanPlayerList(void);

      REAL64            Clock() const;	
      void              IterateSteam();
      GString           GetSteamLanguage();
      void              PassSteamAchievement(int inID);
      void              ServerSetSteamAchievementState(int inID, bool inState);
      bool              ServerGetSteamAchievementState(int inID);

      REAL64            GameTime() const;
      REAL64            GameTime();
      REAL64            GameTimeImmediate() const;
      void              GameTime(REAL64 in_fNewTime);

      REAL64            StartGameTime(){return m_fStartGameTime;}

      REAL64            GameSpeed() const;
      void              GameSpeed(REAL64 in_fNewSpeed);

      //! Activate or desactivate automatic time synchronization of game client.
      /*!
       * @param in_bActivate     : True if synchronization must be activated
       * @param in_iSyncInterval : Minimum period (in iterations) that the client waits before requesting game time sync
       **/
      void              GameTimeSpeedSynchroEnable(bool in_bActivate, 
                                                   UINT32 in_fSyncInterval);

      //! Tell if automatic time synchronization is enabled
      /*!
       * @param out_iSyncInterval   : Filled with the current sync interval if auto sync is enabled
       * @return Boolean that is true only if sync is activated
       **/
      bool              GameTimeSpeedSynchroIsEnabled(UINT32& out_iSyncInterval) const;

      REAL64            AIIterationPeriod() const;
      void              AIIterationPeriod( REAL64 in_fPeriod );
      INT16             Latency();//!< Get the latency to the server
      bool              SetProcessPriorityHigh(); //!< Set the process priority to high
      bool              SetProcessPriorityNormal(); //!< Set the process priority to high
      bool              SetProcessPriorityLow();    //!<Set the process priority to low

      bool              SendPacketUDP(const void *, short, const sockaddr_in&, EUdpPacketType::Enum = EUdpPacketType::CustomPacket);
      bool              BroadcastPacketUDP(const void * in_Data, short in_Size, EUdpPacketType::Enum in_PackType);

      //! Fetch a network client by its network address
      /*!
       * @param in_ClientAddress    : Address of the network client
       * @return Pointer to the network client or NULL if no client has this address
       **/
      GGenericNetworkClient*  ClientByAddress(const sockaddr_in& in_ClientAddress);

      //! Gets if the engine is connected to a server
      bool              Connected();

      //! Gets if the engine is connecting to a server
      bool              Connecting();

      //! Specifies if AI must Iterate or not
      void              MustIterateAI(const bool in_bIterateAI);
      bool              MustIterateAI() const;


      //========================================
      // GAMEPLAY FUNCTIONS
      //========================================
      GPlayer*          ActivePlayer(INT32 in_iID);            //!< Get active player with the given Id
      GPlayer*          ActivePlayerByModID(INT32 in_iModID);  //!< Get an active player by mod Id
      GPlayer*          HumanPlayer(INT32 in_iID);             //!< Get player with the given Id
      GPlayer*          FindPlayer(INT32 in_iID);              //!< Find a player, human or AI

      INT32             NbActivePlayers();                     //!< Get the number of active players
      const GPlayers &  ActivePlayers();
      const GPlayers &  HumanPlayers();
      EJOSHUA_MSG       ActivatePlayer(GPlayer* in_pPlayer);  //!<Adds a player to the active list and active it
      EJOSHUA_MSG       ReplaceAIPlayerByHuman(GPlayer* in_pPlayer);  //!<Replace an AI player and activate the new one.
      EJOSHUA_MSG       ReplaceHumanByAIPlayer(GPlayer* in_pPlayer);  //!<Deactivate the player & replace it with an AI.

      EJOSHUA_MSG       DeletePlayer(INT32 in_iID);            //!<Removes a player from the active list
      EJOSHUA_MSG       DeleteAllPlayers();                    //!<Removes all players from the active list

      INT32             MaxPlayers() const;
      void              MaxPlayers(INT32 in_iMaxPlayers);

      const GString&    ServerName() const;
      void              ServerName(const GString& in_sName);

      const GString&    Password() const;
      void              Password(const GString& in_sPassword);

      INT32             TimeLimit() const;
      void              TimeLimit(INT32 in_iTimeLimit);

      INT32             MinPing() const;
      void              MinPing(INT32 in_iMinPing);

      bool              Private() const;
      void              Private(bool in_bPrivate);

      bool              Dependant() const;
      void              Dependant(bool in_bDependant);

      const GString&    ModCommandLine() const;

      //Starts a new Process that is a dedicated game server
      EJOSHUA_MSG       SpawnServer(GString in_sServerName, GString in_sPassword, UINT32 in_iMaxPlayers, 
                                    UINT32 in_iTimeLimit, UINT32 in_iPort, bool in_bPrivate, bool in_bDedicated,
                                    GString in_sModCommandLine); 
      EJOSHUA_MSG       KillServer();
      EJOSHUA_MSG       RaiseEvent(GGameEventSPtr in_Event);  //Raise event function
      EJOSHUA_MSG       RaiseEventLocal(GGameEventSPtr in_Event);
      EJOSHUA_MSG       RemoveHumanPlayer(INT32 in_iPlayerKey,bool in_bReplaceByAI = true); //Remove a client from the connected Players
      EJOSHUA_MSG       SendDisconnectionReasonToPlayer(INT32 in_iPlayerKey,ConnectionLostReason::Enum in_eDisconnectionReason);
      EJOSHUA_MSG       SendDisconnectionToServer(INT32 in_iPlayerKey);



      GGameEventSPtr    CreateGameEventInternal(const GString &eventType);
      GGameEventSPtr    CreateGameEventInternal(INT32   eventType);
      UINT32            GameEventKeyInternal(const GString in_sEventType) const;

      void              EventHandlerInformation(const GGameEventSPtr&        in_Event,                                                 
                                                CALLBACK_HANDLER_bool_spGE& out_Function, 
                                                void*& out_Instance);

      INT32             Port() const;           // Get the port the game is listening on
      void              Port(INT32 in_iPort);   //Set the port the game is listening on

      //Connects to a server and returns if successful
      bool              Connect(const GString &in_sIP,
                                const GString &in_sPort,
                                const GString &in_sPwd,
                                bool in_bAsynchronous,
								const std::vector<std::string> &in_vsPublicAndLocalIPs);

      // If trying to connect asynchronously, cancel the connection
      bool              Disconnect();

      //Register a game event creation function
      void              RegisterGameEventInternal(const GString & in_sEventType, 
                                                  CREATE_GAME_EVENT_FUNC in_pCreateEventFunc, 
                                                  CALLBACK_HANDLER_bool_spGE in_pHandlingFunctionStd, 
                                                  void* in_pHandlingInstanceStd,
                                                  CALLBACK_HANDLER_bool_spGE in_pHandlingFunctionAI,
                                                  void* in_pHandlingInstanceAI);

      EJOSHUA_MSG       RegisterConsoleCommand(const GString & in_Command,
                                               const GString & in_SyntaxIS,
                                               const GString & in_SyntaxHelpMessage,
                                               CALLBACK_HANDLER_GS_crGS_cvrGS  in_HandlingFunction,
                                               void * in_HandlingInstance);

      //Query the database functions
      DB::EError QueryDB(const GSString &, DB::GTable &);
      DB::EError QueryDB(const vector<GSString> &, vector<DB::GTable*> &);

      bool QueryDBAsync(const GSString &, CALLBACK_HANDLER_bool_rGDBQRH, void *);
      bool QueryDBAsync(const vector<GSString> &, CALLBACK_HANDLER_bool_rGDBQRH, void *);

      //To Start a task in a thread
      bool        StartTaskInThread(void*         in_pInstance,
         CALLBACK_HANDLER_bool_pvoid in_pFunc,
         void* in_pArgs);

      //Initialize the String table
      bool           StringTableInit(const GFile& in_File);
      GStringTable*  StringTable();

      bool IsOfficial() const;

      //-----------------------------------------
      // Movie playback methods
      //-----------------------------------------
      GMovieController* CreateMovieFullScreen(GFile& in_MovieFile);
      bool DeleteMovieFullScreen(GMovieController* in_pMovieControl);
   };
}

#define g_Joshua SDK::GJoshua::Instance()
#endif
