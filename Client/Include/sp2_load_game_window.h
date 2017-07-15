/**************************************************************
*
* sp2_load_game_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_LoadGameWindow_H_
#define _CSP2_LoadGameWindow_H_

namespace SP2
{

   class GLoadGameWindow : public GBaseWindow
   {
   public:
	   GLoadGameWindow();
	   ~GLoadGameWindow();

	   bool OnCreate();
	   void OnHide();
      void OnShow();
      void Show(bool in_bShowSingleGames);

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller = NULL);

      static const GString TypeName;

      bool Update();

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GUI::GListBox* m_pObjGamesLst;
      GUI::GButton*  m_pObjLoadGameBtn;

   private:
      void FillSaveRow( GUI::GListBox::GRow* in_pRow, const pair< GString, GSaveGameData > & in_SaveData );
      void UnselectRow(GUI::GListBox::GRow* in_pRow);
      void SelectRow(GUI::GListBox::GRow* in_pRow);

      INT32 m_iLastSelectedId;
      bool  m_bShowSingleGames;
   };
};

#endif //_CSP2_LoadGameWindow_H_
