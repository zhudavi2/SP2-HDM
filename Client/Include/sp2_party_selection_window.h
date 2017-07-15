/**************************************************************
*
* sp2_party_selection_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_PartySelectionWindow_H_
#define _CSP2_PartySelectionWindow_H_

namespace SP2
{
   const INT32 c_iOnPartySelection = 894653215;

   class GPartySelectionWindow : public GBaseWindow
   {
   public:
	   GPartySelectionWindow();
	   ~GPartySelectionWindow();

	   bool OnCreate();
      void OnShow();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnCustomEvent(UINT32 in_iEventID, const GUI::GEventData& in_EventData, GUI::GBaseObject * in_pCaller);

      void Show(const vector<GPoliticalParty> & in_vParties, INT32 in_iActivePartyID, GUI::GBaseObject* in_pCaller);
      
      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GUI::GListBox* m_pObjPartiesLst;
      GUI::GButton*  m_pObjConfirmBtn;
      GUI::GButton*  m_pObjCancelBtn;

   private:
      hash_map<GString, INT32> m_mParties;
      INT32 m_iSelectedRowID;
      GUI::GBaseObject* m_pCaller;
   };
};

#endif //_CSP2_PartySelectionWindow_H_
