/**************************************************************
*
* golem_logger.h
*
* Description
* ===========
*  Contains the definition of a file logging class.
*
* Owner
* =====
*  Mathieu Tremblay
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_LOGGER__H
#define _GOLEM_LOGGER__H

#include "golem_string.h"
#include "golem_mutex.h"

namespace Hector
{

   //Type de message
   enum EMessageType
   {
      MSGTYPE_MSG = 0,
      MSGTYPE_WARNING,
      MSGTYPE_ERROR,
      MSGTYPE_CRITICAL_ERROR,
		MSGTYPE_CSV,
      MSGTYPE_FATAL_ERROR      
   };





   /*!
   * Unicode Logger Class, Writes into a file
   **/
   class GLogger
   {
   public:
      GLogger(GString in_sFilename);
      ~GLogger();

      bool Log(EMessageType in_eMsgType,GString in_sMsg);
      bool Log(EMessageType in_eMsgType,char* in_sMsg);


   private:
      GString m_sFilename;
      bool  InitializeFile();
      bool  WriteUnicode(GString *in_sString);
      void  ReportError(char* in_sMsg);

   };

}

#endif