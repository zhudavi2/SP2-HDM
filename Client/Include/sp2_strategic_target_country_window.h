/************************************************
 * File:
 * Owner:
 * Description:
 *
 *
 * History:
 * ========
 *
 *************************************************/

#ifndef _CSP2_StrategicTargetCountryWindow_H_
#define _CSP2_StrategicTargetCountryWindow_H_

namespace SP2
{
   class GCursorMeter;
   class GProgressMeterYellowValue;

   class GStrategicTargetCountryWindow : public GUI::GFrame
   {
      friend class GTutorials;
   public:
	   GStrategicTargetCountryWindow();
	   ~GStrategicTargetCountryWindow();

	   bool OnCreate();
      bool OnDestroy();
      void OnShow();
      void OnHide();

      GUI::EEventStatus::Enum OnMouseDown (const SDK::GUI::GEventData&  in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData&            in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseMove (const GEventData&            in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseUp   (const GEventData&            in_EventData, GUI::GBaseObject* in_pCaller);

      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller);
      void OnClickFromEarth(const GVector2D<REAL32>);

      void Initialize();

      void FillWindowAndMeters();

      void CantNuke();

		bool SpecificTargetOn() const;
		void ChangeSelectedCountryToNuke(UINT32 in_iCountryID);

      static const GString TypeName;

      INT32                      m_iTargetCountryId;           
      UINT32                     m_iNbMissileUsed;
      UINT32                     m_iNbMissileInRange;
      ENuclearTargetType::Enum   m_TargetType;

      vector< pair<GNukeTarget, vector<GNuclearMissile> > >	m_TargetsMissilesList;
      vector< pair<GNukeTarget, vector<GNuclearMissile> > > m_NewTargetsMissilesList;
      vector<GNuclearMissile>						      m_MissilesFull;
      vector<GNuclearMissile>						      m_MissilesUsed;


      CGFX_Track_Itf* & AnimationSlidingTrack();

      //Specific target members
      //-------------------------
      multimap<UINT32,GNuclearMissile> m_SpecificTargetMissilesByRange;
      GNukeTarget                      m_SpecificTarget;
      bool                             m_bSpecificTargetMode;
		bool										m_bProcessOfChoosingSpecificTarget;
      //-------------------------


   protected:
      vector<GNuclearMissile> GenerateMissilesUsedList(const vector< pair<GNukeTarget, vector<GNuclearMissile> > > &in_TargetsMissilesList);
      vector<GNuclearMissile> GenerateMissilesUsedListFromSpec(const multimap<UINT32,GNuclearMissile> &in_TargetsMissilesList);

      void UpdateTargetsMissilesAndCasualties(const vector<GNuclearMissile>& in_vMissilesAvailable,
                                              UINT32                         in_iTargetCountryID,
                                              SP2::ENuclearTargetType::Enum  in_eTargetType,
                                              const UINT32                   in_iNbMissilesToUse);


      void OnNbMissilesUsedChange();


      friend class GDataDisplayLayer;
      friend class GStrategicMissileListWindow;
      static GBaseObject* New();

      GUI::GButton*     m_pObjSuggestedBtn;
      GUI::GComboBox*   m_pObjMissileTypeCbo;
      GUI::GButton*     m_pObjPrepareLaunchBtn;
      GUI::GButton*     m_pObjReturnBtn;
      GProgressMeterYellowValue* m_pObjAttackCursor;
      GUI::GComboBox*   m_pObjCountry;
      GUI::GComboBox*   m_pObjAttackType;
      GUI::GButton*     m_pObjIncreaseMissileUseBtn;
      GUI::GButton*     m_pObjDecreaseMissileUseBtn;
      GUI::GLabel*      m_pObjAttackStrengthValue;
      GUI::GLabel*      m_pObjAvailableMissiles;
      GUI::GLabel*      m_pObjMissilesUsed;
      GUI::GLabel*      m_pObjProjCasualities;

   private:
      vector<GString>   m_vAttackType;
      bool              m_bMeterChanged;
      CGFX_Track_Itf*   m_pSlidingTrack;
      bool              m_bDragging;
      ENuclearMissileType::Enum m_eMissileTypeToUse;
   };

   /*!
   * MAIN Bar construction function
   **/
   GUI::GBaseObject* CreateStrategicTargetCountryWindow();
};

#endif //_CSP2_StrategicTargetCountryWindow_H_
