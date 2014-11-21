/**************************************************************
*
* golem_console_mfc.h
*
* Description
* ===========
*  A MFC Console that can log information and act as a server 
*  console for example.
*
*
* Owner
* =====
*  Mathieu Tremblay, Nicolas Hatier
*
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/

#ifndef _GOLEM_CONSOLE_MFC_
#define _GOLEM_CONSOLE_MFC_


namespace Hector
{

   /*!
   * Class that defines an MFC console, used as the game console
   * to log messages, errors or other
   **/
   class GWin32UIConsole : public GConsole
   {
   public:
      //! Destructor
      ~GWin32UIConsole();
      //! Singleton allocator
      static GWin32UIConsole * Instance();

      bool SetTitle(const GString& in_sTitle);

      //! Register the quit handling function
      bool RegisterQuitFunction(void* in_pInstance,CALLBACK_HANDLER_bool_void in_pQuitFunc);
      //! Register the send command handling function
      bool RegisterSendCommandFunction(void* in_pInstance,CALLBACK_HANDLER_bool_crGS in_pSendCommandFunc);

      //! Retrieve the registered send command handling function
      CALLBACK_HANDLER_bool_crGS RetrieveSendCommandFunction();
      //! Retrieve the registered send command handler
      void* RetrieveSendCommandHandler();
      //! Retrieve the registered send command handling function
      CALLBACK_HANDLER_bool_void RetrieveQuitCommandFunction();
      //! Retrieve the registered send command handler
      void* RetrieveQuitCommandHandler();

      //! Print something on the console
      bool Print( const GString& in_sString );
      //! Print something on the console
      bool Printf( const WCHAR *in_pFormat, ...);

      // Displays or hides the console
      bool Show();
      bool Hide();

      HWND ConsoleWindow() {return m_Window;};

   private:
      friend class GWndThread;
      
      //! Temp string for printf
      WCHAR m_pTempStr[2048];

      //! Handles to the window components
      HWND m_Window, m_LogBox, m_EditBox;

      //! Pointer to the instance of the class that handles the quit event
      void*      m_pQuitHandlerInstance;
      //! Pointer to the instance of the class that handles the send command event
      void*	   m_pSendCommandHandlerInstance;
      //! Pointer to the function that handles the quit event
      CALLBACK_HANDLER_bool_void		      m_pQuitFunction;
      //! Pointer to the function that handles the send command event
      CALLBACK_HANDLER_bool_crGS  m_pSendCommandFunction;
      
      static GWin32UIConsole * m_Instance;      
      static WNDPROC m_EditWndProc;	
      static WNDPROC m_ListWndProc;	      

      //! Winproc that handles the events
      static LRESULT CALLBACK WndProcServer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
      static LRESULT CALLBACK EditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);      
      static LRESULT CALLBACK ListWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);  

      //! Constructor
      GWin32UIConsole();
      //! Copy constructor
      GWin32UIConsole(const GWin32UIConsole&);
      
      //! On_Send function, called when a command is sent using the send button, calls the send handler
      bool On_Send(const GString&);
      //! On_Quit function, called when a command is sent using the quit button, calls the quit handler
      bool On_Quit();

      //! Get the sent text
      GString GetSentText();
      

      class GWndThread : public GThread
      {
         void Process(void*);
      } m_WndThread;
   };

}

#endif //#ifndef _GOLEM_CONSOLE_MFC_