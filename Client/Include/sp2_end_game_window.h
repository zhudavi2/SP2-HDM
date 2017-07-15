/**************************************************************
*
* sp2_end_game_window.h
*
* Description
* ===========
*  End Game window
*
* Owner
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _SP2_END_GAME_WINDOW_H_
#define _SP2_END_GAME_WINDOW_H_

namespace SP2
{
   namespace EndGameWindowMode
   {
      enum Enum
      {
         GameOver,
         ScenarioCompleted,
      };
   }

   class GEndGameWindow : public GBaseWindow
   {

    public:
      static const GString TypeName;

      //! Creation function
      static GUI::GBaseObject* New();
      GEndGameWindow();
      ~GEndGameWindow();

      //UI Functions
      void GameOver( SP2::Event::GGameOver* in_pGaveOverEvent);
      void ScenarioCompleted(void);
      bool OnCreate();

   protected:
      friend class GDataDisplayLayer;
   private:
      
      EndGameWindowMode::Enum m_eMode;
      void AddObjective(vector<GGameObjective>& in_Objective);

      // UI Events
      GUI::EEventStatus::Enum OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);

      // UI Elements
      GUI::GButton*           m_pObjMainMenuBtn;
//      GUI::GButton*           m_pObjChoiceBtn;
      GUI::GLabel*            m_pObjTitleLbl;
      GUI::GLabel*            m_pObjSummary;
      GUI::GListBox*          m_pObjObjectivesList;
   };
} // End SP2

#endif // _SP2_END_GAME_WINDOW_H_
