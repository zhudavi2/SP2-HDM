#include "golem_sdk_api.h"
/************************************************
* File:
* Owner:
* Description:
*
*
* History:
* ========
*
*************************************************/

#ifndef _CGame_Client_Generic_H_
#define _CGame_Client_Generic_H_

namespace SDK
{
   /*!
   * Defines a Generic client that just passes Game Events through the network
   * using TCP IP
   **/
   class GOLEM_SDK_API GGenericNetworkClient : public GGameClientItf, public GThread
   {
      friend class GJoshua;
   public:
      GGenericNetworkClient();
      virtual ~GGenericNetworkClient();

      //! Initializes the generic game client
      GAME_MSG Initialize();

      //! Iterates on the generic game client
      GAME_MSG Iterate(void* param);  

      //! Shutsdown the generic game client
      GAME_MSG Shutdown();

      GAME_MSG SendEventToRealClient(GGameEventSPtr);

      //! Thread function that is receiving incoming msgs from the network
      //! and translates them into game events
      void Process(void* args);

      bool Set_Socket(INT32 socket);

      //! Gets if the client is still connected.
      bool Connected(){return m_connected;}

      bool IsReceivingThreadRunning();

      void  InternetAddress(const SOCKADDR_IN& in_Addr);
      const SOCKADDR_IN& InternetAddress();

      void  MacAddress(GString &in_MacAddress);
      const GString& MacAddress(void);

      void UpdatePing(REAL64 in_fNewPingValue);

      REAL64 Ping() const;
      void Send(const INT8 *in_pDataToSend, UINT32 in_iDataSize);

   protected:
   private:
      bool  			   m_connected;
      GSimpleNetwork*   m_tcpTool;
      bool              m_bIsReceivingThreadRunning;

      SOCKADDR_IN       m_InternetAddress;
      REAL64            m_fPing;
      GString           m_MacAddress;
   };
}
#endif //_CGame_Client_Generic_H_
