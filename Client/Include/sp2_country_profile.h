/**************************************************************
*
* sp2_country_profile.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_country_profileWindow_H_
#define _CSP2_country_profileWindow_H_

#include "sp2_start_single_player_window.h"
#include "sp2_frame_cursor_meter.h"

namespace SP2
{
   const INT32 c_iOnCountryProfileWindowSelection = 1236654789;

   class GCountryProfileWindow : public GBaseWindow
   {
   public:
	   GCountryProfileWindow();
	   ~GCountryProfileWindow();

	   bool OnCreate();
      void OnShow();

      void Show(GUI::GBaseObject* in_pCaller, vector<INT16> & in_viCountriesID, vector<INT16> & in_viEconomicRank, vector<INT16> & in_viPoliticRank, vector<INT16> & in_viMilitaryRank);

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const GEventData & in_EventData, GBaseObject* in_pCaller = NULL);

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GUI::GButton* m_pObjConfirmBtn;
      GUI::GButton* m_pObjCancelBtn;

      GUI::GListBox* m_pObjSuggestedLst;
      
      SP2::GGradientBar* m_pObjWantedEconBar;
      SP2::GGradientBar* m_pObjWantedMiliBar;
      SP2::GGradientBar* m_pObjWantedPoliBar;

      SP2::GGradientBar* m_pObjSuggesEconBar;
      SP2::GGradientBar* m_pObjSuggesMiliBar;
      SP2::GGradientBar* m_pObjSuggesPoliBar;

      GUI::GToggle* m_pObjPoliticTog;
      GUI::GToggle* m_pObjEconomicTog;
      GUI::GToggle* m_pObjMilitaryTog;

   private:
      void CalculatePossibleCountries();
      
      vector<INT16> * m_pCountriesID;
      vector<INT16> * m_pMilitaryRank;
      vector<INT16> * m_pPoliticRank;
      vector<INT16> * m_pEconomicRank;

      GUI::GBaseObject* m_pCaller;
   };
};

#endif //_CSP2_country_profileWindow_H_
