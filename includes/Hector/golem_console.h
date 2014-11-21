/**************************************************************
*
* golem_console.h
*
* Description
* ===========
*  A console interface that can log information and receive 
*  commands.
*
* Owner
* =====
*  Mathieu Tremblay, Nicolas Hatier, Frederic O'Reilly
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_CONSOLE_
#define _GOLEM_CONSOLE_

namespace Hector
{
   /*!
   * Class that defines an interface to log messages, errors or other and to 
   * receive input messages
   **/
   class GConsole
   {
   public:
      //! Destructor
      virtual ~GConsole();

      //! Register the quit handling function
      virtual bool RegisterQuitFunction(void* in_pInstance,CALLBACK_HANDLER_bool_void in_pQuitFunc);

      //! Register the send command handling function
      virtual bool RegisterSendCommandFunction(void* in_pInstance,CALLBACK_HANDLER_bool_crGS in_pSendCommandFunc);

      //! Retrieve the registered send command handling function
      virtual CALLBACK_HANDLER_bool_crGS RetrieveSendCommandFunction();

      //! Retrieve the registered send command handler
      virtual void* RetrieveSendCommandHandler();

      //! Retrieve the registered send command handling function
      virtual CALLBACK_HANDLER_bool_void RetrieveQuitCommandFunction();

      //! Retrieve the registered send command handler
      virtual void* RetrieveQuitCommandHandler();

      //! Retrieve the number of commands that can be remembered by the console
      virtual UINT32 NbCommandHistory() const;

      //! Set the number of commands that can be remembered by the console
      virtual bool NbCommandHistory(UINT32 in_iCount);

      //! Retrieve the number of lines that can be displayed by the console
      virtual UINT32 NbOutputLines() const;

      //! Set the number of lines that can be displayed by the console
      virtual bool NbOutputLines(UINT32 in_iCount);

      //! Print something on the console
      virtual bool Print( const GString& in_sString ) = 0;

      //! Print something on the console
      virtual bool Printf( const WCHAR *in_pFormat, ...);

      //! Helper function to set the title of the log window, if any
      virtual bool SetTitle(const GString& in_sTitle);
   };

}

#endif // #ifndef _GOLEM_CONSOLE_