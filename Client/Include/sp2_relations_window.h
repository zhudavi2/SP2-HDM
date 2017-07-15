/**************************************************************
*
* sp2_relations_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_RELATIONS_WINDOW_H_
#define _CSP2_RELATIONS_WINDOW_H_

namespace SP2
{

   const GColorRGBInt c_RelationWindowGraphXAxisColor(200,200,200,255);
   const GColorRGBInt c_RelationWindowGraphValueColor(0,255,20,255);


   class GGraph;
   class GRelationsWindow : public GBaseWindow
   {
   public:
	   GRelationsWindow();
	   ~GRelationsWindow();

	   bool OnCreate();
      bool OnDestroy();
      void OnShow();
      void OnHide();

      GUI::EEventStatus::Enum OnCustomEvent(UINT32 in_EventId, const GUI::GEventData & in_EventData, GBaseObject* in_pCaller);
      static const GString TypeName;

      void SetHistoryRelation(const vector<REAL32>& in_vRelationValues);
		void UpdateWithNewCountries(UINT32 in_iCountryA, UINT32 in_iCountryB);

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      SDK::GUI::GComboBox* m_pObjCountryACbo;
      SDK::GUI::GComboBox* m_pObjCountryBCbo;
      SP2::GGraph*              m_pGraph;


   private:
   };
};

#endif //_CSP2_RELATIONS_WINDOW_H_
