/**************************************************************
*
* sp2_main_menu.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_Main_Menu_H_
#define _CSP2_Main_Menu_H_

namespace SP2
{
   // Foward Declaration
   class GSprite;

   class GMainMenu : public SP2::GBaseWindow
   {
   public:
	   GMainMenu();
	   ~GMainMenu();


	   bool OnCreate();
      void OnShow();
      void OnHide();

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID,const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      bool OnDestroy();
      static const GString TypeName;
   protected:

      GUI::GLabel* m_pVersionLbl;

      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

   private:
      GUI::GButton*   m_pObjSingleBtn;
      GUI::GButton*   m_pObjMultiBtn;
      GUI::GButton*   m_pObjStopBtn;
      GUI::GButton*   m_pObjOptionsBtn;
      GUI::GButton*   m_pObjCreditsBtn;
      GUI::GButton*   m_pObjQuitBtn;
      GUI::GPictureBox* m_pObjSep;

      GSprite*        m_pTitleLogo;
      CGFX_Track_Itf* m_pLogoFadeOutTrack;
   };
};

#endif //_CSP2_Main_Menu_H_
