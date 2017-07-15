/**************************************************************
*
* sp2_battle_info_window.h
*
* Description
* ===========
*  Describes the interface of class GBattleInfoWindow
*
* Owner
* =====
*  Frederic O'Reilly
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _SP2_BATTLE_INFO_WINDOW_H_
#define _SP2_BATTLE_INFO_WINDOW_H_

namespace SP2
{
   class GBattleInfoWindow : GBaseWindow
   {
   public:
      static const GString TypeName;

      static GUI::GBaseObject* New();

      GBattleInfoWindow();
      ~GBattleInfoWindow();

      bool OnCreate();
      bool OnDestroy();
      void OnShow();
      void OnHide();

      //! Set which one is the arena to display
      bool SetArena(SP2::GArenaInfo* in_pArena);

      //! Get which arena ID is displayed
      UINT32 ArenaId();

      //! Update the content of the window
      void UpdateContent();

   private:

      GUI::GListBox*    m_pObjCountryListA;
      GUI::GListBox*    m_pObjCountryListB;

      static const INT32 c_iNbSides = 2;

      GUI::GLabel*      m_pObjCountTxt[c_iNbSides][EUnitCategory::ItemCount];

      UINT32            m_iArenaID;
   };

}

#endif //_SP2_BATTLE_INFO_WINDOW_H_
