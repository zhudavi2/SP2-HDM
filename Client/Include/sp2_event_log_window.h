/**************************************************************
*
* sp2_event_log_window.h
*
* Description
* ===========
*
* Copyright  (C) 2004, Laboratoires Golemlabs Inc.
**************************************************************/

#ifndef _CSP2_EVENT_LOG_WINDOW_H_
#define _CSP2_EVENT_LOG_WINDOW_H_

namespace SP2
{
   const INT32 c_iNewsMax      = 50;

   struct SEventLogData
   {
      GString              m_sMail;
      SP2::ESphere::Enum   m_eType;
      GVector2D<REAL32>    m_Position;
      UINT32               m_iCountryA;
		UINT32               m_iCountryB;
   };

   class GEventLogWindow : public GUI::GFrame
   {
   public:
	   GEventLogWindow();
	   ~GEventLogWindow();

	   bool OnCreate();
      bool OnDestroy();
      void OnShow();

      EEventStatus::Enum OnMouseClick(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnMouseDown(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnMouseMove(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnMouseOut(const GEventData & in_EventData, GBaseObject* in_pCaller);
      EEventStatus::Enum OnMouseLeave(const GEventData & in_EventData, GBaseObject* in_pCaller);

      void OnDrag(const GEventData & in_EventData, const GVector2D<INT32> & in_MouseOffset, const GVector2D<INT32> & in_MouseRelative);

      bool OnGotFocus(const GUI::GBaseObject *in_pFocusFrom, GUI::GBaseObject *in_pCaller);

      void AddEvent(GString in_sEvent, SP2::ESphere::Enum in_eType, GVector2D<REAL32> in_Position, UINT32 in_iCountryA, UINT32 in_iCountryB = 0);
      bool Update();

      static const GString TypeName;

   protected:
      friend class GDataDisplayLayer;
      static GBaseObject* New();

      GUI::GListBox* m_pObjEventsLst;
      GUI::GToggle*  m_pObjCategoryToggle[ESphere::Count];
      GUI::GButton*  m_pObjGotoBtn;

   private:
      void AddItem(SEventLogData in_Data);
      void DisplayOverlay();
      void HideOverlay();

      // <EventID, Data>
      map<UINT32, SEventLogData> m_Events[ESphere::Count];

      // <RowID,  EventID>
      vector< pair<UINT32, UINT32> > m_ShownEvents;
      INT32 m_iCounter;

      bool m_bDragging;
   };
};

#endif //_CSP2_EVENT_LOG_WINDOW_H_
