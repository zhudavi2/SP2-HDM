/**************************************************************
*
* sp2_communications_window.h
*
* Description
* ===========
*
*
* Owner
* =====
*  François Durand
*
* Copyright  (C) 2003, Laboratoires Golemlabs Inc.
***************************************************************/
#ifndef _CSP2_InternalLaws_H_
#define _CSP2_InternalLaws_H_

#include "sp2_frame_cursor_meter.h"

namespace SP2
{
   class GInternalLawsWindow : public GBaseWindow, DesignPattern::GObserver, SP2::GRequestObject
   {
   public:
	   GInternalLawsWindow();
	   ~GInternalLawsWindow();

	   bool OnCreate();
      bool OnDestroy();
      void OnShow();
      void OnHide();

      bool Enabled();
      void Enabled(bool in_bEnabled);

      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller);
      void OnSubjectChange(Hector::DesignPattern::GSubject&  in_Subject,const Hector::DesignPattern::GSubjectNotification& in_Notification,void* in_pAdditionalMsg );

      void Clear();
      void Update(bool in_bForce = false);
      void Save();

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GUI::GBaseObject* New();

      GFrame*       m_pObjInnerFrame;
      GFrame*       m_pObjMigration;
      GTabbed*      m_pObjTabGroups;
      GListBox*     m_pObjLstReligions;
      GListBox*     m_pObjLstLanguages;
      GListBox*     m_pObjLstParties;
      GListBox*     m_pObjLstLaws;
      GComboBox*    m_pObjImmigrationCbo;
      GComboBox*    m_pObjEmigrationCbo;

      void BuildLawChoicesList(vector<GString> &in_sChoices, UINT32 in_iLaw);
   private:
      GDataInternalLaws* m_pData;
      GUI::GComboBox*    m_pLastClickedCombo;
      INT32              m_iGvtType;
   };

};
#endif //_CSP2_InternalLaws_H_
