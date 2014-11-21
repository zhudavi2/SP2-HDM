/**************************************************************
*
* golem_game_time_sync_packet.h
*
* Description
* ===========
*  Various time synchronization packet definitions.
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _GOLEM_GAME_TIME_SYNC_PACKET_H_
#define _GOLEM_GAME_TIME_SYNC_PACKET_H_

#include "golem_sdk_constants.h"

namespace SDK
{
   /*!
    * Base time synchronization packet that identifies
    * data contained in the paquet
    **/
   class GGameTimeSyncPacket
   {
   protected:
      UINT32      m_iEventID;
   };

   /*!
    * Time synchronization packet sent by client
    * when requesting a time synchronization.
    **/
   class GGameTimeSyncRequest : protected GGameTimeSyncPacket
   {
   public:
      GGameTimeSyncRequest()
      {
         m_iEventID = c_UDPEventTimeSyncRequest;
      }

      REAL64      m_fClientGameTime;
      REAL64      m_fClientGameSpeed;
      UINT32      m_iPacketID;
   };

   /*!
    * Time synchronization packet sent by server
    * to respond to a time synchronization request.
    **/
   class GGameTimeSyncResponse : protected GGameTimeSyncPacket
   {
   public:
      GGameTimeSyncResponse()
      {
         m_iEventID = c_UDPEventTimeSyncResponse;
      }

      GGameTimeSyncRequest m_Request;
      REAL64      m_fServerGameTime;
      REAL64      m_fServerGameSpeed;
   };

   /*!
    * Time synchronization packet sent by server
    * when game time is changed.
    **/
   class GGameTimeSyncTimeChange : protected GGameTimeSyncPacket
   {
   public:
      GGameTimeSyncTimeChange()
      {
         m_iEventID = c_UDPEventTimeSyncTimeChange;
      }

      REAL64      m_fPreviousTime;
      REAL64      m_fNewTime;
      REAL64      m_fSpeedAtChange;
   };

   /*!
    * Time synchronization packet sent by server
    * when game speed is changed.
    **/
   class GGameTimeSyncSpeedChange : protected GGameTimeSyncPacket
   {
   public:
      GGameTimeSyncSpeedChange()
      {
         m_iEventID = c_UDPEventTimeSyncSpeedChange;
      }

      REAL64      m_fPreviousSpeed;
      REAL64      m_fNewSpeed;
      REAL64      m_fTimeAtChange;
   };

}

#endif // #ifndef _GOLEM_GAME_TIME_SYNC_PACKET_H_