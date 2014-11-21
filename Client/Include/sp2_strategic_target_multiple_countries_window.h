/**************************************************************
*
* sp2_strategic_target_multiple_countries_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_StrategicTargetMultipleCountriesWindow_H_
#define _CSP2_StrategicTargetMultipleCountriesWindow_H_

namespace SP2
{
   class GCursorMeter;

   class GStrategicTargetMultipleCountriesWindow : public GBaseWindow
   {
   public:
	   GStrategicTargetMultipleCountriesWindow();
	   ~GStrategicTargetMultipleCountriesWindow();

	   bool OnCreate();
      void OnShow();
      bool OnDestroy();
      void OnHide();

      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseMove(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseUp(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      static const GString TypeName;



      vector<INT32>                      m_iTargetCountryId;           
      vector<UINT32>                     m_iNbMissileUsed;
      vector<UINT32>                     m_iNbMissileInRange;
      vector<ENuclearTargetType::Enum>   m_TargetType;

      vector< vector< pair<GNukeTarget, vector<GNuclearMissile> > > >	m_TargetsMissilesList;
      vector< vector< pair<GNukeTarget, vector<GNuclearMissile> > > >   m_NewTargetsMissilesList;
      vector< multimap<UINT32,GNukeTarget> >					      m_TargetList;
      vector< vector<GNuclearMissile> >						      m_MissilesFull;
      vector< vector<GNuclearMissile> >						      m_MissilesUsed;

      vector<GNuclearMissile> GenerateMissilesUsedList(const vector< pair<GNukeTarget, vector<GNuclearMissile> > >& in_TargetsMissilesList);
      void OnClickFromEarth(const GVector2D<REAL32> in_Position);

   protected:
      friend class GDataDisplayLayer;
      friend class GStrategicMissileListWindow;

      static GBaseObject* New();

      vector<GUI::GComboBox*>   m_pObjCountry;
      vector<GUI::GComboBox*>   m_pObjAttackType;
      vector<class GProgressMeterYellowValueSmall*>   m_pObjAttackStrengthValue;
      vector<GUI::GLabel*>      m_pObjAvailableMissiles;
      vector<GUI::GLabel*>      m_pObjMissilesUsed;
      GUI::GLabel*              m_pObjProjCasualities;

      GUI::GListBox* m_pObjAttackList;

   private:
      void AddRow();
      void EraseRow(INT16 in_iPosition);

      GUI::GButton*  m_pObjPrepareLaunchBtn;
      GUI::GButton*  m_pObjMissileListBtn;

      vector<GString> m_vAttackType;
      bool m_bMeterChanged;
      bool m_bSkipModifyRow;
      bool m_bDeleteSelectedRow;
   };

   /*!
   * MAIN Bar construction function
   **/
   GUI::GBaseObject* CreateStrategicTargetMultipleCountriesWindow();
};

#endif //_CSP2_StrategicTargetMultipleCountriesWindow_H_
