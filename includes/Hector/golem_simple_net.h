//====================================================================//
//		golem_simple_net.h
//
//
//		OVERVIEW
//	 	========
//        A simple network interface to create TCP/IP clients and servers.
//        UDP is also supported.
//
//			This class is used to abstract the client or the server.  
//          It simplifies the Process of:
//                  - Sending data to the client/server,
//			        - Connecting as a client or act asing a server.
//
//	Principal functions:
//			Connect, PrepareServer, Send
//
//		See the golem_simple_net.cpp file for details on each function
//
//
//
//=========================================================================

#ifndef _GOLEM_SIMPLE_NET
#define	_GOLEM_SIMPLE_NET 

#include "golem_types.h"
#define bufferSize 2048

#define FIRST_INVALID_CHAR_INDEX    32
#define LAST_INVALID_CHAR_INDEX		126

namespace Hector
{

   class GSimpleNetwork
   {
   public:
      //Functions
      GSimpleNetwork(int socket);	//Constructor if socket is already initialized
      GSimpleNetwork();			//Default Constructor
      ~GSimpleNetwork();			//Destructor

      int Connect(string ip, UINT16 port); //Fonction to call when we want to connect

      bool Send(const string & data);	 //Send data to the client/server to which we are connected
      bool Send(const INT8* data,INT32 size);	 //Send data to the client/server to which we are connected

      string Receive(bool cleaned); //Receive data from the connected client/server
      GSPtrINT8 ReceiveLen(INT32& recvSize); 

      /*!
      Receive data from the buffer, the memory is allocated by the Receive function
      and must be freed by the calling Process
      */
      bool PrepareServer(UINT16 Port, int NbOfConnectionsToQueue); //Prepare to act as a server

      int   AcceptConnection(sockaddr* out_pAddr= NULL);	//Accept incoming connections - a call to this function
      //should be made only once the PrepareServer function
      //executed succesfully

      int GetAndPrintError(char*);  //Display error # and description
      bool Close();				   //Close the connection
      static bool InitializeWinsock();	   //Initialize winsock if not already initialized

      static GString LocalIP();
      bool TestingIfAlive();		   //Check if connection is still up
      string RemoveInvalidChars(string string,int nbValidChars); //Remove invalid chars
      //from a string 

      int		GetSocket();		//Return the socket descriptor
      void	SetSocket(int);		//Set the socket descriptor

      string	GetIPAddress();	      //Return the ip address of the local cpu
      string	GetHostName();		  //Return the hostname of the local cpu
      bool	SetIPAddress(string); //Set the m_myIP field
      bool	SetHostName(string);  //Set the m_Hostname field
      int		FetchHostInfo(UINT16 Port); //Get the information on the local computer
      //It will fill the m_hostname / m_myIP fields.
      bool 	SetRcvTimeout(int TimeoutMSEC); //Sets a receive Timeout on the socket
      bool  SetSndTimeout(int TimeoutMSEC); //!<Sets a timeout on the send method
      bool  SetDontLinger();

      bool  CheckRdyToWrite();

      inline const string& LastErrorMessage() { return m_sLastError; }

   private:

      //Fields
      int 			   m_Socket;
      string         m_sLastError;
      string			m_hostname;
      string			m_myIP;
   };

}

#endif
// End of file