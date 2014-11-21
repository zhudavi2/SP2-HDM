/**************************************************************
*
* sp2_game_scenario_window.h
*
* Description
* ===========
*  Describes the interface of class GGameScenarioWindow
*
* Owner
* =====
*  Jonathan Mercier
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
***************************************************************/


// Include Guards
#ifndef _SP2_GAME_SCENARIO_WINDOW_H
#define _SP2_GAME_SCENARIO_WINDOW_H

#include "../include/sp2_frame_scenario.h"

namespace SP2
{
   // Foward Declaration
   class GGameScenario;

   /*!
   * Scenario Window, used to choose a scenario and during the game
   * See the completed objectvies
   **/
   class GGameScenarioWindow : public SP2::GBaseWindow
   {
   public:
      static const GString TypeName;
      //! Creation function
      static GUI::GBaseObject* New();
      GGameScenarioWindow();
      ~GGameScenarioWindow();

      //! UI Functions
      bool OnCreate();

      //! We receive this update when the country data is available
      void OnEndReceivedCountryData(void);
      // void DisplayCurrentScenario(void);

      void UpdateSelectedScenario(void);
      void ClearSelectedScenario();


   protected:

   private:

      void FillOutScenarios(void);


      // UI Events
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventID, const GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);

      // UI Elements
      GUI::GToggle*              m_pObjPoliticEasy;
      GUI::GToggle*              m_pObjPoliticHard;
      GUI::GToggle*              m_pObjPoliticNormal;
      GUI::GToggle*              m_pObjEconomicEasy;
      GUI::GToggle*              m_pObjEconomicNormal;
      GUI::GToggle*              m_pObjEconomicHard;
      GUI::GToggle*              m_pObjMilitaryEasy;
      GUI::GToggle*              m_pObjMilitaryNormal;
      GUI::GToggle*              m_pObjMilitaryHard;

      GUI::GListBox*             m_pObjCustomScenarioList;

      GUI::GButton*              m_pObjStartBtn;
      GUI::GButton*              m_pObjCancelBtn;

      SP2::GFrameScenario*       m_pScenarioFrm;
   };

}//end namespace SP2

#endif //_SP2_GAME_SCENARIO_WINDOW_H
