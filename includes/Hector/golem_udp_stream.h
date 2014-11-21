/**************************************************************
*
* golem_udpstream.h
*
* Description
* ===========
*  Describes the interface of class GUDPStream
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_UDPSTREAM_H_
#define _GOLEM_UDPSTREAM_H_

namespace Hector
{

/*!
 * UDP Stream class
 **/
class GUDPStream
{
public:
   GUDPStream();
   ~GUDPStream();

   //! Initializes the UDPStream so we are ready to send/recv datas
   bool Initialize(UINT32 in_iPort);
   //! Send the data to the given address
   bool  Send(const GOBuffer &, const sockaddr_in &);
   //! Recv the io_Size bytes of data and return
   bool Recv(GIBuffer &, sockaddr_in &);
   bool Initialized() const;

   inline const string& LastErrorMessage() { return m_sLastError; }

protected:
private:
   int GetAndPrintError(char* ModuleName);

   SOCKET m_Socket;
   char m_DataBuffer[516];
   bool m_Initialized;

   string         m_sLastError;

};

};
#endif //_GOLEM_UDPSTREAM_H_
