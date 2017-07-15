/**************************************************************
*
* sp2_submenu_single.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_Submenu_single_H_
#define _CSP2_Submenu_single_H_

namespace SP2
{
   class GSubmenuSingle : public SP2::GBaseWindow
   {
   public:
	   GSubmenuSingle();
	   ~GSubmenuSingle();

	   bool OnCreate();
      void OnShow();
      void OnHide();

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      static const GString TypeName;
   protected:

      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

   private:
      GUI::GButton* m_pObjTutorial1;
      GUI::GButton* m_pObjTutorial2;
      GUI::GButton* m_pObjScenario;
      GUI::GButton* m_pObjCustomGame;
      GUI::GButton* m_pObjLoadGame;
      GUI::GButton* m_pObjReturnMain;
      GUI::GPictureBox* m_pObjSep;
      GUI::GPictureBox* m_pObjSep1;
   };
};

#endif //_CSP2_Submenu_single_H_
