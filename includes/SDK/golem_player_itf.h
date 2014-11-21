#include "golem_sdk_api.h"
/**************************************************************
*
* golem_player_itf.h
*
* Description
* ===========
*  Define the interface for a player in the joshua engine.
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GAME_PLAYER_ITF_H
#define _GAME_PLAYER_ITF_H

namespace SDK
{

   enum GEPlayerStatus
   {
      PLAYER_STATUS_READY = 0,
      PLAYER_STATUS_IDLE,
      PLAYER_STATUS_ACTIVE
   };

   /*!
   * Define a player as seen by the server.  This class must be inherited to define
   * game specific players. (Ex: CSP2_Player).  The server has a list of players.
   **/
   class GOLEM_SDK_API GPlayer
   {
   protected:
      //!Player unique Id, allocated by the server or the engine
      INT32		         m_iId;
      //!Mod id, to identify the entity controlled within the mod
      INT32			      m_iModID;
      //! Description associated with the modId ex: Country name, X, O, blue, etc.
      GString			   m_sModName;
      //! Name identifying the player (used for human players)
      GString			   m_sName;
      
      //! Status of the player , Idle, Ready, etc..
      GEPlayerStatus    m_ePlayerStatus;

      /*!
      * Player client itf ptr, used to raise game events to the client
      * If the m_client pointer is null, means it is an AI player
      * Then the game events will be raised to the AI part of the player
      **/
      GGameClientItf* m_pClient;
      //! Specifies if a player is local or remote
      bool			      m_bLocal; //client on the same computer as the server

      //! Specifies if AI or not
      bool              m_bAIControlled;


   public:
      GPlayer();
      virtual ~GPlayer();

      GGameClientItf* Client();
      void			      Client(GGameClientItf* in_pClient,bool in_bAIControlled = false);
      INT32             Id();
      void			      Id(INT32 in_iID);
      INT32			      ModID();
      void			      ModID(INT32 in_iModID);
      GString		      Name();
      void			      Name(const GString &in_sName);

      GString		      ModName();
      void			      ModName(const GString &in_sName);

      bool              AIControlled();

      bool			      Local();
      void			      Local(bool in_bIsLocal);

      GEPlayerStatus    PlayerStatus();
      void              PlayerStatus(GEPlayerStatus in_ePlayerStatus);

   };
};//end of namespace GPlayer

#endif