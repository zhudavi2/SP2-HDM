#include "golem_sdk_api.h"
/**************************************************************
*
* golem_server_itf.h
*
* Description
* ===========
*  Game Server interface class definition.
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef SDK_SERVER_ITF_H
#define SDK_SERVER_ITF_H

// forward declaration
namespace SDK
{
   class GPlayer;   

   /*!
   * Defines an interface to a game server.  This class must be derivated to define a game
   * specific server (Ex: CSP2_Server).
   *	Pure virtual functions:
   *		Initialize()
   *		Iterate()
   *		Shutdown()
   *	
   *	The Iterate function is called every frame by the game engine, the game Process can be placed
   *	there. Example: The processing of the game event queue must be placed there.
   **/
   class GOLEM_SDK_API GGameServerItf : public GGameProcessItf, public RGListener
   {
   protected:
   public:

      GGameServerItf();
      virtual ~GGameServerItf();

      //Game flow methods.
      virtual GAME_MSG Initialize() = 0;
      virtual GAME_MSG Iterate(void* param) = 0;
      virtual GAME_MSG Shutdown() ;

      //Function that is called when a player gets connected to the server
      virtual void OnNewPlayerJoined(SDK::GPlayer* in_pPlayer);
      //Function that is called when a player is disconnected from the server
      virtual void OnPlayerDisconnect(SDK::GPlayer* in_pPlayer);
      
      void OnUDPPacketReception(INT8, GOBuffer &, const sockaddr_in & l_From);
      virtual void OnUDPPacketReception(GOBuffer &, const sockaddr_in & l_From);
/*
      void ServerName(const GString& in_sServerName);
      const GString& ServerName() const;
*/
      void ScenarioName(const GString& in_sScenarioName);
      const GString& ScenarioName() const;

      void ModName(const GString& in_sName);
      const GString& ModName() const;
/*
      void Password(const GString& in_sPassword);
      const GString& Password() const;
*/
/*
      bool Public();
      void Public(bool in_bPublic);
*/
	  
      unsigned int CountHumanPlayers();

	  //RGAPI
	  void onRGEvent(const unsigned int e, RGData::RGData *d);
	  bool RGAPIReportServer();
	  void RGAPIDirty();
	  void RGAPIIterate();
	  void RGAPIShutDown();

   private:
      GString m_sServerName;
      GString m_sScenarioName;
      GString m_sModName;
      GString m_sPassword;
      bool    m_bPublic;

      static int     m_iThreadId;
      static int     m_iReceiveSocket;

	  //RGAPI
	    unsigned int mDBID;
		unsigned int mGameID;
	    bool m_bRGAPIGameHosted;
		RGInterface *m_RGAPI;
		bool InitRGAPI();
		void RGAPIGetData(imap& integerMap, smap& stringMap);
		void RGAPIHost(unsigned int inRoomId);
   };
}

#endif