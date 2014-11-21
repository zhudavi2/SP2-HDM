/**************************************************************
*
* sp2_economic_health_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_EconomicHealthWindow_H_
#define _CSP2_EconomicHealthWindow_H_

#include "../include/sp2_main_bar.h"
#include "../include/sp2_mini_map.h"
#include "../include/sp2_frame_cursor_meter.h"
#include "../include/sp2_frame_updatable_value.h"

namespace SP2
{
   class GEconomicHealthWindow : public GBaseWindow, DesignPattern::GObserver, SP2::GRequestObject
   {
   public:
      static const GString TypeName;
	   GEconomicHealthWindow();
	   ~GEconomicHealthWindow();

	   bool OnCreate();
      void OnShow();
      void OnHide();

      void OnSubjectChange(DesignPattern::GSubject&  in_Subject,const DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );

      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      
      void Update(bool in_bForceUpdate = false);
      void Save();

      bool Enabled();
      void Enabled(bool in_bEnabled);

      GUI::GFrame* m_pObjInnerFrame;

      GUI::GLabel* m_pObjGdpTotal;
      GUI::GLabel* m_pObjGdpPerCapita;
      GUI::GLabel* m_pObjUnemploymentTxt;
      GUI::GLabel* m_pObjPovertyTxt;
      SP2::GCursorMeter* m_pObjUnemploymentMtr;
      SP2::GCursorMeter* m_pObjPovertyMtr;
      SP2::GUpdatableValue* m_pObjPersonalIncomeTaxVal;
      SP2::GUpdatableValue* m_pObjInterestLevelVal;
      SP2::GEditBoxNumeric* m_pObjPersonalIncomeTax;
      GUI::GLabel* m_pObjInflationValue;
      GUI::GLabel* m_pObjInflationGrowth;
      GUI::GPictureBox*     m_pObjDelimiter;
      SP2::GEditBoxNumeric* m_pObjInterestLevel;

      GDataEconomicHealthWindow* m_pData;

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

   private:
      REAL64 m_fInterestLevel;
      REAL64 m_fIncomeTax;
            
      bool   m_bBeingEdited;
   };
};

#endif //_CSP2_EconomicHealthWindow_H_
