/**************************************************************
*
* sp2_join_multiplayer_game_window.h
*
* Description
* ===========
*
*
* Owner
* =====
*  François Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _CSP2_RGAPILogin_H_
#define _CSP2_RGAPILogin_H_

namespace SP2
{
   class GLoginRGAPIWindow : public GBaseWindow
   {
   public:
	   GLoginRGAPIWindow();
	   ~GLoginRGAPIWindow();

	   bool OnCreate();
	   bool OnDestroy();

	   //! called when the button is released into our menu
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject *in_pCaller);
	   GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);

	  void Show(GUI::GBaseObject* in_pCaller);

     void OnLoginCompleted(bool bSuccess);
     void OnRegisterCompleted(bool bSuccess);

      GString m_sUserName;
      GString m_sPassword;	  
	  static const GString TypeName;
      
   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

   private:
      bool m_bIsLoginWindow;
      
	  GUI::GBaseObject* m_pCaller;

	  GUI::GFrame* l_mFrame;
	  GUI::GLabel* m_lblTitle;
	  
     GUI::GButton*  m_pObjConnectBtn;
     GUI::GButton*  m_pObjCancelLoginBtn;
	  GUI::GButton*  m_pObjRegisterBtn;
     GUI::GButton*  m_pObjCreateBtn;
     GUI::GButton*  m_pObjCancelRegisterBtn;
      
	  GUI::GLabel*	 m_txtUserName;
	  GUI::GLabel*	 m_txtPassword;
     GUI::GLabel*	 m_txtPassword2;
     GUI::GLabel*	 m_txtEmail;
      
      GUI::GEditBox* m_edtPassword;
      GUI::GEditBox* m_edtUserName;
      GUI::GEditBox* m_edtPassword2;
      GUI::GEditBox* m_edtEmail;

      void ShowRegisterWindow();
      void ShowLoginWindow();
   };
};
#endif //_CSP2_RGAPILogin_H_
