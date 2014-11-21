/**************************************************************
*
* sp2_treaties_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_TREATIES_WINDOW_H_
#define _CSP2_TREATIES_WINDOW_H_

namespace SP2
{

   class GTreatiesWindow : public SP2::GBaseWindow, DesignPattern::GObserver
   {
   public:
      static const GString TypeName;

      GTreatiesWindow();
	   ~GTreatiesWindow();

      bool OnCreate();
      bool OnDestroy();
      void OnShow();
      void OnHide();
      bool Update();
      void Update(GString in_sMemberName, ETreatyType::Enum in_TypeFilter);

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);
      GUI::EEventStatus::Enum OnMouseDown(const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject * in_pCaller);
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const SDK::GUI::GEventData & in_EventData, GUI::GBaseObject* in_pCaller);

      void OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );

      void Sort();
      void AddTreaty(UINT32 in_iID);

      void FillCountries();

      void     SetFilter(GString in_sCountryName, ETreatyType::Enum in_eTreatyType, bool in_bUpdateWindow);
      void     FilterMember(GString in_sCountryName, bool in_bUpdate);
      GString  FilterMember();
      void     FilterType(ETreatyType::Enum in_eTreatyType, bool in_bUpdate);
      ETreatyType::Enum FilterType();

      GDataTreatiesWindow* m_pData;

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

      GUI::GListBox* m_pObjActiveTreatiesLst;
      GUI::GListBox* m_pObjPendingTreatiesLst;
      GUI::GComboBox* m_pObjFilterMember;
      GUI::GComboBox* m_pObjFilterType;
      GUI::GButton*   m_pObjNewTreaty;
      GUI::GButton*   m_pObjViewSelected;

   private:
      GString m_sFilterMember;
      ETreatyType::Enum m_eFilterType;

      vector<UINT32> m_TreatyPendingId;
      vector<UINT32> m_TreatyActiveId;

      INT32  m_iLastSelectedTreatyId;
      INT32  m_iLastSelectedTreaty;
      bool   m_bLastSelectedTreatyActive;
   };
};

#endif //_CSP2_TREATIES_WINDOW_H_
