/**************************************************************
*
* sp2_constitutional_form_window.h
*
* Description
* ===========
*
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/
#ifndef _CSP2_ConstitutionalFormWindow_H_
#define _CSP2_ConstitutionalFormWindow_H_

namespace SP2
{
   class GConstitutionalFormWindow : public GBaseWindow, DesignPattern::GObserver, SP2::GRequestObject
   {
   public:
	   GConstitutionalFormWindow();
	   ~GConstitutionalFormWindow();

	   bool OnCreate();
      void OnShow();
      void OnHide();
      
      bool Enabled();
      void Enabled(bool in_bEnabled);

      GUI::EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GUI::GBaseObject* in_pCaller);  
      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller);

      void OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );

      void Reset();
      void Update(bool in_bForce = false);
      void Save();

      GUI::GFrame* m_pObjInnerFrame;

      GUI::GComboBox*   m_pObjConstitutionalForm;
      GUI::GComboBox*   m_pObjCapital;
      GUI::GLabel*      m_pObjNextElection;
      GUI::GButton*     m_pObjMartialLaw;
      GUI::GButton*     m_pObjOverride;

      map<GString,UINT32> & Cities();
      map<GString,INT32> & GvtType();
      bool MartialLaw();
      void MartialLaw(bool in_bMartialLaw);
      static const GString TypeName;

      GDataConstitutionalForm* m_pData;

		const DesignPattern::GSubject*   m_pSubjectThatWillAnswerLiberateCountryQuestion;
		INT32 m_iCountryToLiberateRegions;

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

   private:		
      map<GString,UINT32> m_mCities;
      map<GString,INT32> m_mGvt;
      bool m_bMartialLaw;
      GDateTime m_NextElection;
      INT32 m_iCapitalId;
      INT32 m_iGvtId;
   };
};

#endif //_CSP2_ConstitutionalFormWindow_H_
