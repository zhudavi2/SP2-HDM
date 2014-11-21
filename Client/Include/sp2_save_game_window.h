/**************************************************************
*
* sp2_save_game_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_SaveGameWindow_H_
#define _CSP2_SaveGameWindow_H_

namespace SP2
{

   class GSaveGameWindow : public GBaseWindow
   {
   public:
	   GSaveGameWindow();
	   ~GSaveGameWindow();

	   bool OnCreate();
      void OnShow();
      void OnHide();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller = NULL);

      bool Update();

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GUI::GFrame*  m_pObjGamesFrm;
      GUI::GButton* m_pObjSaveGameBtn;

      GUI::GListBox* m_pObjSaveLst;
      GUI::GEditBox* m_pObjNameEdt;

   private:
      void FillSaveRow( GUI::GListBox::GRow* in_pRow, const pair< GString, GSaveGameData > & in_SaveData );
      void UnselectRow(GUI::GListBox::GRow* in_pRow);
      void SelectRow(GUI::GListBox::GRow* in_pRow);

      INT32 m_iLastSelectedId;
   };
};

#endif //_CSP2_SaveGameWindow_H_