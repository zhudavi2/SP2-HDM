/**************************************************************
*
* sp2_submenu_multi.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_Submenu_multi_H_
#define _CSP2_Submenu_multi_H_

namespace SP2
{
   class GSubmenuMulti : public SP2::GBaseWindow
   {
   public:
	   GSubmenuMulti();
	   ~GSubmenuMulti();

	   bool OnCreate();
      void OnShow();
      void OnHide();

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      static const GString TypeName;
   protected:

      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

   private:
      GUI::GButton* m_pObjLAN;
      GUI::GButton* m_pObjInternet; 
      GUI::GButton* m_pObjLoad;
      GUI::GButton* m_pObjReturnMain;
      GUI::GPictureBox* m_pObjSep;
   };
};

#endif //_CSP2_Submenu_multi_H_
