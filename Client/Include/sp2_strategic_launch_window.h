/**************************************************************
*
* sp2_strategic_launch_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_StrategicLaunchWindow_H_
#define _CSP2_StrategicLaunchWindow_H_

namespace SP2
{
   class GCursorMeter;

   class GStrategicLaunchWindow : public GUI::GFrame
   {
      friend class GTutorials;
   public:
	   GStrategicLaunchWindow();
	   ~GStrategicLaunchWindow();

	   bool OnCreate();
	   bool OnDestroy();
      void OnShow();
      void OnHide();

      void Initialize(bool in_bFromTargetSingleCountry = true);
      
      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      void Iterate();

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      friend class GStrategicTargetCountryWindow;
      friend class GStrategicTargetMultipleCountriesWindow;

      static GBaseObject* New();

      vector<UINT8> m_vCountryIdList;
      vector<UINT8> m_vAttackStrength;
      vector<UINT8> m_vAttackType;

      bool m_bFromTargetCountryWindow;

   private:
      void AnimateSequence(CGFX_Track_Itf* in_pTrack, UINT32 in_iStart, UINT32 in_iEnd);

      GUI::GFrame* m_pObjLaunchFrm;
      GUI::GButton* m_pObjAbortBtn;
      vector<GNukeLaunchRequest> m_NukeRequests;

      CGFX_Track_Itf*         m_pOnShowAnim;
      CGFX_Track_Itf*         m_pOnDowmAnim;
      CGFX_Track_Itf*         m_pOnFlashAnim;

      REAL32                  m_fDownMinimumTime;
      bool                    m_bPlaying;
   };

   /*!
   * MAIN Bar construction function
   **/
   GUI::GBaseObject* CreateStrategicLaunchWindow();
};

#endif //_CSP2_StrategicLaunchWindow_H_
