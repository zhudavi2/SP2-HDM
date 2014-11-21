/*
golem_tcp_compressed.h

Class header file for GTCPCompressed

-> see golem_tcp_compressed.cpp for further
global and functions informations and
implementation
*/

#ifndef _GOLEM_TCP_COMPRESSED_
#define _GOLEM_TCP_COMPRESSED_

#include "golem_compression.h"
#include "golem_simple_net.h"

namespace Hector
{

   class GTCPCompressed
   {
   public:
      GTCPCompressed();
      GTCPCompressed(INT32 socket);
      ~GTCPCompressed();

      // Initialize winsock allowing us to create connection
      bool Init();
      // Prepare and start the server to receive incomming connection on TCP/IP network
      bool PrepareServer(UINT16 socket, UINT8 nbConnection);
      // Allow a client to connect a server via TCP/IP protocol
      bool Connect(string ip, UINT16 port);
      // Close connection
      bool Disconnect();
      // Send compressed data with zlib through TCP/IP
      bool Send(const INT8* buffer, UINT32 size);
      // Receive compressed data through TCP/IP and decompress it using zlib
      GSPtrINT8 Receive(INT32 &size);
      // Accept incoming connections for server
      INT32 AcceptConnection();

   private:
      //Member that add compression/decompression functionnality
      GDataCompressor m_dataCmp;
      //Member that add TCP/IP network functionnality
      GSimpleNetwork m_netTool;
   };

}

#endif