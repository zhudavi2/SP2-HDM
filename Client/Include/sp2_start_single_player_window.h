/**************************************************************
*
* sp2_start_single_player_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_StartSinglePlayerWindow_H_
#define _CSP2_StartSinglePlayerWindow_H_

#include "sp2_country_profile.h"

namespace SP2
{

   class GStartSinglePlayerWindow : public GBaseWindow
   {
   public:
	   GStartSinglePlayerWindow();
	   ~GStartSinglePlayerWindow();

	   bool OnCreate();
      bool OnDestroy();
      void OnShow();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);

      static const GString TypeName;

      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const GUI::GEventData& in_EventData, GUI::GBaseObject* in_pCaller);

      void SetCountryRanks(const vector<INT16> & in_viMilitaryRanks, const vector<INT16> & in_viPoliticRanks, const vector<INT16> & in_viEconomicRanks, const vector<INT16> & in_viCountriesID);
      void SetPoliticalParties(const vector<GPoliticalParty> & in_vParties);

   protected:
      friend class GDataDisplayLayer;
      friend class GCountryProfileWindow;

      static GBaseObject* New();
      
      GUI::GButton*   m_pObjProfileBtn;
      GUI::GButton*   m_pObjPoliticalPartyBtn;
      GUI::GButton*   m_pObjStartGameBtn;
      GUI::GButton*   m_pObjCancelBtn;

      GUI::GToggle*   m_pObjEconomicHumanTog;
      GUI::GToggle*   m_pObjEconomicCpuTog;
      GUI::GToggle*   m_pObjPoliticHumanTog;
      GUI::GToggle*   m_pObjPoliticCpuTog;
      GUI::GToggle*   m_pObjMilitaryHumanTog;
      GUI::GToggle*   m_pObjMilitaryCpuTog;

      GUI::GToggle*   m_pObjWorldPeaceTog;
      GUI::GToggle*   m_pObjBalanceResTog;
      GUI::GToggle*   m_pObjConquerWorldTog;
      GUI::GToggle*   m_pObjDevelopNationTog;
      GUI::GToggle*   m_pObjRaiseGDPTog;

      GUI::GComboBox* m_pObjWorldPeaceCbo;
      GUI::GComboBox* m_pObjBalanceResCbo;
      GUI::GComboBox* m_pObjConquerWorldCbo;
      GUI::GComboBox* m_pObjDevelopNationCbo;
      GUI::GComboBox* m_pObjRaiseGDPCbo;

      SP2::GComboListBoxLess* m_pObjCountryCbo;

      void AddGameObjective(EObjectiveType::Enum in_eObjectiveType,
                            UINT32 in_fTimeLimitYears = 0, 
                            REAL32 in_fValueToReach = 1.f);
      void RemoveGameObjective(EObjectiveType::Enum in_eObjectiveType);

      //! Update the game objective in the list of the game objectives with the data from the interface
      void UpdateObjective(EObjectiveType::Enum in_eType);

      vector<INT16> m_viEconomicRanks;
      vector<INT16> m_viMilitaryRanks;
      vector<INT16> m_viPoliticRanks;
      vector<INT16> m_viCountriesID;
      vector<GPoliticalParty> m_vParties;

      GUI::GLabel* m_pObjPartyName;

      //! Feed the comboboxes with the goals we can set
      void FeedComboGoals(GUI::GComboBox* in_pCbo);

   private:
      INT32 m_iSelectedPartyID;
      bool m_bMustShowProfile;
      bool m_bMustShowParties;

   };
};

#endif //_CSP2_StartSinglePlayerWindow_H_
